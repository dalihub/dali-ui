# video-view sample (4 rendering modes)

dali-ui `VideoView`를 MMPlayer/ESPlayer 외부 source로 구동하는 데모. underlay(VideoShell
hole-punch) 경로와 NativeImage(GPU texture) 경로를 모두 보여준다.

## 실행 모드

```sh
video-view.example                # MMPlayer underlay (기본, VideoShell hole-punch)
video-view.example <video_file>   # MMPlayer underlay, 커스텀 URI
video-view.example mn             # MMPlayer NativeImage (GPU texture, 기본 URI)
video-view.example mn <video_file> # MMPlayer NativeImage, 커스텀 URI
video-view.example es             # ESPlayer NativeImage, 하드코딩된 H.264 테스트 데이터
video-view.example es-underlay    # ESPlayer underlay, 하드코딩된 H.264 테스트 데이터
```

- `es-tcore`/`es-ecore`도 `es-underlay`의 별칭으로 계속 동작한다. tcore/ecore는
  dali-extension의 빌드 타임 선택(`USE_TCORE_BACKEND`)이라 앱이 구분할 필요가 없다 — 어떤
  이름으로 실행하든 같은 모드이고, 실제로 tcore/ecore 중 무엇이 동작하는지는 링크된
  dali-extension이 어떻게 빌드됐는지에 따라 결정된다.
- `es`/`es-underlay` 모드는 실제 파일이 아니라 코드에 내장된 짧은 H.264 비트스트림
  (`kTestH264Data`, SVA_Base_B QCIF 176x144)을 사용한다 — ESPlayer 경로 검증 목적이며
  URI 인자를 받지 않는다.
- MMPlayer 모드(기본/`mn`)만 파일 경로 인자를 받는다. 인자가 없으면 기본 경로
  (`/opt/usr/media/Videos/sample.mp4` 또는 샘플 리소스)를 사용한다.

## 모드별 동작

### MMPlayer underlay / MMPlayer NativeImage

- 앱이 CAPI player를 생성하고 URI만 설정한다: `player_create` → `player_set_uri` (아직 IDLE).
- native handle을 `Dali::Ui::Tizen::CreateVideoSourceFromMMPlayer()`(underlay) 또는
  `CreateVideoSourceFromMMPlayerNativeImage()`(NativeImage)로 감싸 `VideoSource`로 만든다.
  - 이 지점 외에는 `VideoView`가 MMPlayer/ESPlayer 여부를 알지 못한다.
- `VideoView::New(source)` → `window.Add(view)`로 scene에 붙인다. underlay 모드는 이때
  동기적으로 display target이 **IDLE 상태의 player에** 연결된다 + transparent underlay hole
  + geometry sync 동작. NativeImage 모드는 NativeImage/IMAGE visual이 생성되고
  `SetRenderingTarget(NativeImagePtr)`가 호출된다.
- **그 다음** 앱이 `player_prepare()` → `VideoView::Play()`를 호출한다.
- 종료 시 앱이 `ClearSource()` 후 `player_destroy()`를 직접 호출한다(external ownership).
  - `VideoView`/extension은 external player를 `prepare`/`unprepare`/`destroy`하지 않는다.

> **중요 — 순서(underlay 모드)**: display target은 player가 `PLAYER_STATE_IDLE`일 때 연결해야
> 한다. Tizen의 `player_set_display_mode` / `player_set_ecore_wl_display`(또는 tcore)는
> IDLE에서만 동작하므로, **view를 scene에 붙여 display를 연결한 뒤 `player_prepare`** 를 호출한다.
> prepare를 먼저 하면 display 연결이 실패해 **소리는 나오지만 화면이 보이지 않는다.**

ecore / tcore backend에서 **동일한 코드/동일한 동작**이어야 한다(차이는 dali-extension 내부의
display attach 호출뿐).

### ESPlayer NativeImage / ESPlayer underlay

- `esplusplayer_create` / `open` / stream info 설정 (IDLE 상태).
- `CreateVideoSourceFromESPlayer()`(NativeImage) 또는 `CreateVideoSourceFromESPlayerUnderlay()`
  (underlay, providerId `"tizen.esplayer"` — NativeImage와 같은 id다. 팩토리가 capability와
  무관하게 이 providerId를 항상 `EsVideoPlayer`로 라우팅하므로 tcore/ecore용 provider를 따로 둘
  필요가 없다)로 핸들을 감싼다. `VideoView::New(source)`는 provider factory를 거쳐 항상
  **`EsVideoPlayer`** 를 생성한다 (`TizenVideoPlayer`는 MMPlayer 전용이라 사용되지 않음).
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
| `Esc` / `Back` | 종료 (ClearSource 후 player_destroy) |

## 디바이스 검증 순서

1. **ecore device**: 영상 표시 / Play·Pause·Stop / move·resize·loop 중 ROI follow /
   scene detach·attach / 종료 후 player_destroy 안전성.
2. **tcore device**: 동일 시나리오를 반복해 ecore와 결과가 같은지 확인.

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
  - `tizen-video-source.h`는 `dali-ui-foundation` public-api에 있어 base
    `pkgconfig(dali2-ui-foundation)`만으로 찾아진다(별도 integration 패키지 불필요).
- `res/` (아이콘)

> GBS에서 위 헤더를 여전히 못 찾으면, rootstrap의 dali-ui 패키지가 video 헤더 추가 이전 버전일
> 수 있다. dali-ui를 먼저 GBS로 다시 빌드/설치한 뒤 이 샘플을 빌드한다.

spec이 `samples/video-view/`로 들어가 in-source cmake로 빌드한다. `appid`는
`com.samsung.dali.video-view`, exec는 `.../bin/video-view.example`이다.
