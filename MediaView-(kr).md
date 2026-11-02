# DALi UI Foundation - MediaView

[→ English Documentation](MediaView.md)

`Dali::Ui::MediaView`는 앱이 소유한 MMFW `media_frame_source_h`의 프레임을
표시합니다. player/camera 생성, 설정, 시작, 정지와 파괴는 앱이 담당하고,
MediaView는 화면 표시를 담당합니다.

`<dali-ui-foundation/public-api/views/media/media-view.h>` 또는 foundation
umbrella 헤더를 include합니다. Tizen 소스 연결에는 선택적 패키지
`dali3-extension-media-frame-source-plugin`과 이에 맞는 mediatool,
player/camera 구현이 필요합니다.

Application 생성 후 main loop 진입 전에
`Dali::Ui::UiConfig::New().Apply()`를 호출합니다. view 크기는 Ui 레이아웃
API의 requested width/height로 설정합니다.

## Tizen 소스 연결

player가 IDLE이고 display NONE, gapless 비활성 상태일 때 소스를 얻습니다.
player를 prepare하기 전에 view를 연결합니다. `Dali::Any`에는 원래
`media_frame_source_h` 타입 그대로 넣습니다.

```cpp
// player 생성과 URI 설정이 완료된 상태입니다. 각 CAPI 반환값을 확인합니다.
media_frame_source_h source = nullptr;
if(player_get_media_frame_source(player, &source) != PLAYER_ERROR_NONE)
{
  return;
}

auto view = Dali::Ui::MediaView::New(Dali::Any(source));
if(!view)
{
  return; // 플러그인 없음, 미지원 소스 또는 이미 연결된 소스입니다.
}

view.SetRequestedWidth(640.0f);
view.SetRequestedHeight(360.0f);
window.Add(view);
// 이후 player_prepare(player), player_start(player)를 호출합니다.
```

camera는 CREATED 상태에서 display NONE을 설정하고
`camera_get_media_frame_source()`로 소스를 얻습니다. view를 연결하고
window에 추가한 뒤 `camera_start_preview()`를 호출합니다. 소스 연결과
public media-packet preview callback은 동시에 사용할 수 없습니다.

## 표시와 수명

| API | 동작 |
|---|---|
| `New()` | 소스 없는 view 생성 |
| `New(Any)` | 연결 성공 시 view, 실패 시 빈 handle 반환 |
| `SetSource(Any)` | 이전 연결 해제 후 새 연결 시도; 실패 시 연결 없는 상태 |
| `GetSource()` | 빌린 소스 반환; 연결 없으면 빈 Any |
| `ClearSource()` | callback 해제와 보유 packet 반환; 반복 호출 가능 |
| `SetSourceCrop(Rect<float>)` | 회전 전 source의 정규화 crop 설정; 잘못된 값이면 이전 crop 유지 |
| `SetHorizontalFlipEnabled(bool)` / `SetVerticalFlipEnabled(bool)` | source 회전 이후 화면 축 기준 반전 |

기본 crop은 `(0, 0, 1, 1)`이고 두 flip은 꺼져 있습니다. crop은 유한한
값으로 source 내부에 있어야 하며 width/height는 양수여야 합니다.
crop과 회전을 적용한 이미지의 비율을 유지하면서 view 크기에 맞춥니다.

모든 MediaView API는 DALi 이벤트 스레드에서 호출합니다. 한 소스에는
하나의 MediaView를 연결합니다. 씬에서 view를 제거하면 packet과 texture를
비우며, 다시 추가하면 새 프레임부터 표시를 재개합니다.

종료 순서는 producer 정지 → `view.ClearSource()` → view unparent/reset
→ player unprepare/destroy 또는 camera destroy입니다. 소스는 producer가
소유하고, MediaView가 producer의 수명을 연장하지 않습니다.

백엔드는 pending 프레임 하나와 최근 표시 packet 3개를 보유합니다.
RETURN_BUFFER에서는 모두 즉시 반환합니다. 이 정책은 GPU 완료를
추적하지 않습니다. gapless와 keep_screen은 지원하지 않습니다.

## Tizen 검증 실행 파일

GBS에서 player/camera를 `--define 'media_view_tests 1'`로 빌드합니다.
`XDG_RUNTIME_DIR=/run WAYLAND_DISPLAY=wayland-0` 환경에서
`player_test_media_view_ui 30` 또는 `camera_test_media_view_ui 30`을
실행합니다. player는 `/home/video-01.mp4`, camera는 설정된 preview
소스를 사용합니다. crop, flip, resize, 씬 제거·복귀, stop/start 10회와
종료를 실행하며, `RESULT PASS`와 실제 화면을 함께 확인합니다.

## 렌더링과 자연 크기

MediaView는 내부 StreamedImageVisual에 프레임을 전달한다. 이 Visual은 corner radius와
borderline 지원을 capability로 선언하고 이미지 vertex·fragment shader를 재사용한다.
일반 ImageVisual에는 상태를 추가하지 않으며 crop·반전은 signed pixelArea로,
90/270도 계열은 IS_REQUIRED_SWAP_UV shader 변형으로 처리한다. MediaView 연결부에서
백엔드의 표시 크기를 crop·회전 전 콘텐츠 크기로 변환하므로 texture padding은 레이아웃에 포함되지 않는다.
crop·회전 후 프레임 크기가 wrap-content 측정과 aspect fitting의 기준이다.
256×64 crop은 200×200 뷰에서 200×50으로 표시한다. requested 크기가 우선하며,
한 축만 지정하면 다른 축은 콘텐츠 종횡비로 정한다. Texture 핸들이 같아도
프레임의 표시 정보가 바뀌면 측정을 무효화한다. 프레임이 없으면 콘텐츠 자연 크기는 0이다.
