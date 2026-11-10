# video-view sample

dali-ui `VideoView`를 MMPlayer/ESPlayer 외부 source로 구동하는 데모. underlay(VideoShell
hole-punch) 경로와 NativeImage(GPU texture) 경로를 모두 보여준다.

## 실행 모드

```sh
video-view.example                # MMPlayer underlay (기본, VideoShell hole-punch)
video-view.example <video_file>   # MMPlayer underlay, 커스텀 URI
video-view.example mn             # MMPlayer NativeImage (GPU texture, 기본 URI)
video-view.example mn <video_file> # MMPlayer NativeImage, 커스텀 URI
video-view.example ms <video_file> # MMPlayer synchronized underlay (video-shell)
video-view.example es             # ESPlayer NativeImage, 하드코딩된 H.264 테스트 데이터
video-view.example es-underlay    # ESPlayer underlay, 하드코딩된 H.264 테스트 데이터
```

- tcore/ecore는 dali-extension의 빌드 타임 선택(`USE_TCORE_BACKEND`)이다.
- `es`/`es-underlay` 모드는 실제 파일이 아니라 코드에 내장된 짧은 H.264 비트스트림
  (`kTestH264Data`, SVA_Base_B QCIF 176x144)을 사용한다 — ESPlayer 경로 검증 목적이며
  URI 인자를 받지 않는다.
- MMPlayer 모드(기본/`mn`/`ms`)는 파일 경로 인자를 받는다. 인자가 없으면
  `/usr/apps/com.samsung.dali.video-view/res/video-02.mp4`를 사용한다.
- `ms`는 `VideoView::New()` → `SetSyncMode(ENABLED)` → `SetSource()` 순서로
  설정한다. Tizen 11 TCORE에서는 `PLAYER_DISPLAY_TYPE_OVERLAY_SYNC_UI2`를 사용한다.

## 모드별 동작

### MMPlayer underlay / MMPlayer NativeImage

- 앱이 CAPI player를 생성하고 URI만 설정한다: `player_create` → `player_set_uri` (아직 IDLE).
- `Dali::VideoSourceDescriptor`를 직접 채운 뒤(providerId `"tizen.mmplayer"`, nativeSession,
  renderingMode — underlay면 `VideoRenderingMode::UNDERLAY`, NativeImage면 `NATIVE_IMAGE`),
  `Dali::Ui::CreateVideoSource()`로 `VideoSource`로 만든다. (별도 provider별 헬퍼는 없다.)
  - 이 지점 외에는 `VideoView`가 MMPlayer/ESPlayer 여부를 알지 못한다.
- `VideoView::New(source)` → `window.Add(view)`로 scene에 붙인다. underlay 모드는 이때
  동기적으로 display target이 **IDLE 상태의 player에** 연결된다 + transparent underlay hole
  + geometry sync 동작. NativeImage 모드는 NativeImage/IMAGE visual이 생성되고
  `SetRenderingTarget(NativeImagePtr)`가 호출된다.
- **그 다음** 앱이 `player_prepare()` → `VideoView::Play()`를 호출한다.
- 종료 시 앱이 `player_stop()`으로 READY에 진입한 뒤 `ClearSource()`/View 해제로
  extension의 콜백과 decoded packet을 반환하고, 그 다음 `player_unprepare()` →
  `player_destroy()`를 직접 호출한다(external ownership). NativeImage source는 특히 이 순서를
  지켜야 하며, View 해제가 끝날 때까지 외부 player를 유효하게 유지해야 한다.
  - `VideoView`/extension은 external player를 `prepare`/`unprepare`/`destroy`하지 않는다.

> **중요 — 순서(underlay 모드)**: display target은 player가 `PLAYER_STATE_IDLE`일 때 연결해야
> 한다. Tizen의 `player_set_display_mode` / `player_set_ecore_wl_display`(또는 tcore)는
> IDLE에서만 동작하므로, **view를 scene에 붙여 display를 연결한 뒤 `player_prepare`** 를 호출한다.
> prepare를 먼저 하면 display 연결이 실패해 **소리는 나오지만 화면이 보이지 않는다.**

ecore / tcore backend에서 **동일한 코드/동일한 동작**이어야 한다(차이는 dali-extension 내부의
display attach 호출뿐).

### ESPlayer NativeImage / ESPlayer underlay

- `esplusplayer_create` / `open` / stream info 설정 (IDLE 상태).
- descriptor의 providerId를 `"tizen.esplayer"`로 두고 renderingMode로 모드를 고른다
  (NativeImage면 `NATIVE_IMAGE`, underlay면 `UNDERLAY`). 두 모드가 같은
  providerId다 — 팩토리가 렌더링 모드와 무관하게 이 providerId를 항상 `EsVideoPlayer`로 라우팅하므로
  tcore/ecore용 provider를 따로 둘 필요가 없다. `VideoView::New(source)`는 provider factory를 거쳐
  항상 **`EsVideoPlayer`** 를 생성한다 (`TizenVideoPlayer`는 MMPlayer 전용이라 사용되지 않음).
- 앱이 `ReadyToPrepareCallback`을 등록하고 `prepare_async`를 호출한다. 콜백에서 하드코딩된
  H.264 데이터 + EOS를 제출한다. 100ms 타이머로 READY 상태를 폴링한 뒤 `VideoView::Play()`.
- NativeImage 모드: decoded-packet 콜백 → `NativeImage::SetSource(tbmSurface)` → GPU 텍스처.
- underlay 모드: tcore/ecore 구분은 dali-extension의 빌드 타임 선택(`USE_TCORE_BACKEND`)이라
  `EsVideoPlayer::InitializeUnderlayMode()` 내부에서 알아서 처리한다 — 앱은 신경 쓸 필요 없다.
  - tcore: `tizen_core_wl_window_get_geometry()`로 조회한 실제 창 geometry로
    `esplusplayer_set_display_window()`를 호출해 창과 초기 ROI를 한 번에 바인딩한다 (플랫폼팀
    tcore 검증 테스트 슈트와 동일한 사용법). **주의**: `esplusplayer_set_display(handle,
    ESPLUSPLAYER_DISPLAY_TYPE_OVERLAY, window)`는 이 백엔드에서 esplusplayer 데몬 내부에서
    멈추므로(`ecore_pipe_write()` assertion), 대신 `esplusplayer_set_display_window()`를 사용한다.
  - ecore: MMPlayer ecore underlay와 동일한 패턴(`ecore_wl2_display_screen_size_get()` +
    `ecore_wl2_window_alpha_set(false)`)으로 화면 크기를 조회하고,
    `esplusplayer_set_ecore_display(handle, OVERLAY, window, 0, 0, w, h)`로 창을 바인딩한다
    (MMPlayer의 `player_set_ecore_wl_display()`와 대응하는 API).
  - 두 백엔드 모두 바인딩 전에 `esplusplayer_set_display_mode(handle,
    ESPLUSPLAYER_DISPLAY_MODE_DST_ROI)`를 호출한다 — `esplusplayer_set_display_roi()`의 전제조건이다.
    이후 실제 ROI는 `DoSetDisplayArea()`의 `esplusplayer_set_display_roi()`가 갱신한다.

각 모드 상세 흐름은 `video-view-example.cpp` 상단의 doc 주석을 참고.

## 키 조작

| 키 | 동작 |
| --- | --- |
| `p` | Play |
| `a` | Pause |
| `s` | Stop |
| `m` | 위치 이동 애니메이션 (ROI follow 검증) |
| `r` | 크기 변경 애니메이션 (ROI follow 검증) |
| `n` | 좌우 왕복 루프 애니메이션 |
| `Esc` / `Back` | 종료 (앱 stop → ClearSource/View 해제 → 앱 unprepare/destroy) |

## 디바이스 검증 순서

1. **ecore device**: 영상 표시 / Play·Pause·Stop / move·resize·loop 중 ROI follow /
   scene detach·attach / 종료 후 player_destroy 안전성.
2. **tcore device**: 동일 시나리오를 반복해 ecore와 결과가 같은지 확인.

### Ui 자동 검증

실기기 테스트 도구의 Ui 시나리오를 같은 실행 파일에 통합했다. Toolkit 의존성이나
CAPI 호출 가로채기를 추가하지 않고, Ui 공개 API와 앱 소유 MMPlayer를 사용한다.

```sh
video-view.example test-underlay /home/video-01.mp4
video-view.example test-native /home/video-01.mp4
video-view.example test-sync /home/video-01.mp4
```

각 모드는 약 18초 동안 영상을 반복 재생하며 아래 순서로 검사하고 자동 종료한다.

1. IDLE 상태에서 source/view를 연결한 뒤 앱이 prepare하고 재생한다.
2. source 설정 후 sync 모드 변경 요청이 현재 모드와 source를 보존하는지 검사한다.
3. 재생 중 같은 Window에 재연결하고, 위치 `(240,140)`·크기 `480x270`·부모 이동을 적용한다.
4. Pause 상태 재연결과 Play 재개를 검사한다.
5. underlay/sync는 다른 Window로의 바인딩 거부와 원래 Window 복귀를 검사한다.
   NativeImage의 다른 Window 이동은 이 자동 검증에 포함하지 않는다.
6. 앱 stop → ClearSource/View 해제 → 앱 unprepare 후, 다음 Timer 이벤트에서
   새 View를 만들고 레이아웃 처리 시간을 둔 다음 prepare/play한다.
7. 재생 위치가 다시 진행되는지 확인하고 앱 소유 Player와 추가 Window를 정리한다.

`CHECK PASS/FAIL`, `STAGE`별 실제 화면 영역·Player 상태·재생 위치,
최종 `RESULT PASS/FAIL`을 출력한다. 성공 시 종료 코드 0, 실패 시 1이다.
Esc/Back은 다음 Timer 이벤트에서 안전하게 정리하고 중단하며, 미완료로 종료 코드 1을 반환한다.

이 검사는 display 바인딩의 실제 API 반환값이나 프레임 단위 동기화 지연을 직접 측정하지
않는다. 실제 영상 표시·가림·위치와 움직임은 `ms`의 `m`/`r`/`n` 키 또는 자동 검증 화면으로
확인한다. 테스트 영상은 mute 상태로 재생한다.

Tizen 11 compositor/libds 조합은 video-shell 초기 destination `(-1,-1)` 해제를 지원해야
한다. 외부 MMPlayer 콜백 정리와 sync 좌표 수정이 포함된 dali-extension도 필요하다.

## 빌드 의존성

- `capi-media-player` (CAPI `player.h`, source: `submit/player`). 이 패키지가 prefix에
  설치돼 있어야 빌드된다. 없으면 CMake가 이 샘플을 건너뛴다(데스크톱 빌드에서 제외).
- `libmm-player`(`mm_player.h` / `MMHandleType`)는 **이 샘플에 직접 쓰이지 않는다.**
  `capi-media-player`가 내부적으로 사용하는 하위 레이어다.

## GBS 빌드

hello-world와 동일하게 독립 GBS 패키지로 구성돼 있다.

- `com.samsung.dali.video-view.manifest`
- `com.samsung.dali.video-view.xml` (tizen package manifest)
- `packaging/com.samsung.dali.video-view.spec`
  - `capi-media-player`, `esplusplayer` BuildRequires (CAPI `player_h`, ESPlayer 헤더)
  - `VideoSourceDescriptor`(`video-source-descriptor.h`)는 dali-adaptor public-api에 있어
    `pkgconfig(dali3-adaptor)`만으로 찾아진다(별도 integration 패키지 불필요).
- `res/` (아이콘)

> GBS에서 위 헤더를 여전히 못 찾으면, rootstrap의 dali-adaptor 패키지가 VideoSourceDescriptor 추가
> 이전 버전일 수 있다. dali-adaptor를 먼저 GBS로 다시 빌드/설치한 뒤 이 샘플을 빌드한다.

spec이 `samples/video-view/`로 들어가 in-source cmake로 빌드한다. `appid`는
`com.samsung.dali.video-view`, exec는 `.../bin/video-view.example`이다.

`dali-ui` 저장소 루트에서, 설정된 RPI4 GBS profile을 사용한 예:

```sh
cd samples/video-view
gbs build -A armv7l -P rpi4 --spec com.samsung.dali.video-view.spec --include-all --overwrite --threads 6
```

`rpi4`는 로컬에 설정한 profile 이름으로 바꾼다. 빌드한 RPM을 설치한 타겟에서는:

```sh
sdb shell 'XDG_RUNTIME_DIR=/run WAYLAND_DISPLAY=wayland-0 /usr/apps/com.samsung.dali.video-view/bin/video-view.example test-sync /home/video-01.mp4'
```
