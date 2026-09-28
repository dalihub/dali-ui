# ScrollView gradient mask PoC

`scrollview-gradient-mask-example.cpp`는 고정된 ScrollView에 `MaskEffect`를 적용해 위·아래 콘텐츠를 투명하게 만든다. 이미지 리소스 없이 `GradientVisual`의 알파를 0 → 1 → 1 → 0으로 설정한다. 고정 페이드 폭은 양쪽 각각 100이다. 각 페이드 영역은 smoothstep 곡선을 16개 구간으로 나눈 중간점을 사용해, 투명·불투명 영역과 연결되는 부분의 알파 변화를 완만하게 한다.

마스크 View는 ScrollView와 같은 위치·크기의 형제 View로 장면에 추가한다. 현재 MaskEffect 구현이 타깃 카메라를 마스크 캡처에도 사용하므로 위치를 일치시켜야 한다. 스크롤 콘텐츠 안에 마스크를 넣지 않는다. 배경은 짙은 회색(`#454A52`)의 View이며, 효과 대상 밖에 있어 투명한 영역을 통해 보인다.

타깃과 마스크 모두 매 프레임 캡처한다. 타깃은 스크롤을 반영해야 한다. `create()`에 장면 구성 순서와 MaskEffect 생성·설정을 모으고, 배경·스크롤 콘텐츠·그라디언트 생성은 별도 함수로 분리했다. 마스크는 항상 적용하며 화면 안내와 토글 기능은 두지 않는다. 고정 크기의 기능 검증용 샘플이며, 성능 및 리사이즈 검증은 포함하지 않는다.

## 빌드 및 실행

DALi 개발 환경의 pkg-config 경로를 설정한 상태에서 `dali-ui` 루트 기준:

```sh
cmake -S samples -B samples/_build-mask -DDALI_UI_SAMPLE_LIST=scrollview -DCMAKE_BUILD_TYPE=Debug
cmake --build samples/_build-mask --target scrollview-gradient-mask-example -j4
./samples/scrollview/bin/scrollview-gradient-mask-example
```

- 드래그 / 휠: 스크롤
- 방향키: ScrollView 기본 키 동작 사용
- 창 닫기: 종료

## 검증

2026-09-10 로컬 설치 DALi 라이브러리로 타깃 빌드 성공. 데스크톱 실행 화면에서 상하 투명도, 배경 투과 및 방향키 스크롤 후 고정된 페이드 영역 확인. Window 키 처리 제거 후 기본 방향키 동작과 드래그·휠은 별도 수동 확인 필요.

## TizenFX NUI 확인

로컬 `/home/jy/work/tizenfx` 소스에 다음 API가 존재한다.

- `src/Tizen.NUI/src/public/RenderEffects/RenderEffect.cs`: `RenderEffect.CreateMaskEffect(View)` 및 Alpha/Luminance·위치·스케일 지정 오버로드
- `src/Tizen.NUI/src/public/BaseComponents/ViewPublicMethods.cs`: `View.SetRenderEffect`, `GetRenderEffect`, `ClearRenderEffect`
- `src/Tizen.NUI/src/public/RenderEffects/MaskEffect.cs`: `TargetMaskOnce`, `SourceMaskOnce`
- `test/Tizen.NUI.Samples/Tizen.NUI.Samples/Samples/MaskEffectSample.cs`: 실제 View 적용 예제

```csharp
// maskView: 타깃과 같은 위치·크기로 장면에 추가한 알파 그라디언트 View
var effect = RenderEffect.CreateMaskEffect(maskView, MaskEffectMode.Alpha);
effect.TargetMaskOnce = false;
effect.SourceMaskOnce = false;
targetView.SetRenderEffect(effect);
```

특이 사항: 생성 및 View 설정 API에 `EditorBrowsable(EditorBrowsableState.Never)`가 붙어 있다. C# public 메서드이지만 일반 IntelliSense 노출 대상은 아니다. 기존 효과가 있으면 `ClearRenderEffect()` 후 설정해야 한다. 이 확인은 로컬 소스 기준이며, 특정 배포 SDK/기기의 네이티브 심볼 지원 및 NUI 실행은 검증하지 않았다.
