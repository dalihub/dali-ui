/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * CanvasView scene-effect sample.
 *
 * Draws a speech-bubble shape (rounded rectangle + tail) in a single CanvasView and applies the
 * ThorVG-backed scene effects exposed on CanvasView:
 *   - SetDropShadow(color, offsetX, offsetY, blurRadius)
 *   - SetGaussianBlur(blurRadius)
 *   - SetEffectAutoPaddingEnable(enable)
 *
 * Auto-padding (on by default) renders the effect into a larger buffer and scales it back into the
 * view, so the shadow/blur is never clipped at the canvas edges without the app reserving margin.
 *
 * Keys:
 *   D   — cycle the effect: drop shadow -> gaussian blur -> none
 *   P   — toggle effect auto-padding (when off, the effect is clipped at the canvas edges)
 *   ESC — quit
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/canvas-view.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer-shape.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
// Overall scale applied to the bubble geometry.
constexpr float kScale = 2.0f;

// Bubble geometry, in unscaled local coordinates (origin at the rounded-rect top-left).
constexpr float kBubbleWidth  = 200.0f;
constexpr float kBubbleHeight = 90.0f;
constexpr float kCornerRadius = 16.0f;

// Tail: a small downward triangle joined to the bottom-left of the rounded rectangle.
constexpr float kTailLeft  = 28.0f;
constexpr float kTailRight = 52.0f;
constexpr float kTailTipX  = 34.0f;
constexpr float kTailDepth = 16.0f;

const Vector4 kBubbleColor(0.0f, 0.6f, 1.0f, 1.0f);

// Drop shadow parameters.
const Vector4   kShadowColor(0.0f, 0.0f, 0.0f, 0.5f);
constexpr float kShadowOffsetX = 0.0f;
constexpr float kShadowOffsetY = 6.0f;
constexpr float kShadowBlur    = 10.0f;

// Blur radius for the standalone Gaussian-blur mode.
constexpr float kBlurRadius = 8.0f;

enum class EffectMode
{
  DropShadow,
  GaussianBlur,
  None
};

} // namespace

class TooltipShadowSample : public ConnectionTracker
{
public:
  explicit TooltipShadowSample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TooltipShadowSample::OnInit);
  }

private:
  // Builds the speech bubble (rounded rect + tail) as a single filled shape, scaled by kScale.
  static Dali::CanvasRenderer::Shape MakeBubble()
  {
    Dali::CanvasRenderer::Shape bubble = Dali::CanvasRenderer::Shape::New();

    bubble.AddRect(Bounds(0.0f, 0.0f, kBubbleWidth * kScale, kBubbleHeight * kScale),
                   Vector2(kCornerRadius * kScale, kCornerRadius * kScale));

    bubble.AddMoveTo(Vector2(kTailLeft * kScale, kBubbleHeight * kScale));
    bubble.AddLineTo(Vector2(kTailTipX * kScale, (kBubbleHeight + kTailDepth) * kScale));
    bubble.AddLineTo(Vector2(kTailRight * kScale, kBubbleHeight * kScale));
    bubble.Close();

    bubble.SetFillColor(kBubbleColor);
    return bubble;
  }

  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &TooltipShadowSample::OnKeyEvent);

    const PositionSize ws = window.GetPositionSize();

    // The canvas is sized to the bubble bounds (tail included). Auto-padding makes room for the
    // effect inside this buffer, so no manual margin is required here.
    const float canvasW = kBubbleWidth * kScale;
    const float canvasH = (kBubbleHeight + kTailDepth) * kScale;

    mCanvasView = CanvasView::New();
    mCanvasView.SetLayoutParams(AbsoluteLayoutParams::New()
                                  .SetX((ws.width - canvasW) * 0.5f)
                                  .SetY((ws.height - canvasH) * 0.5f)
                                  .SetWidth(canvasW)
                                  .SetHeight(canvasH));

    Dali::CanvasRenderer::Shape bubble = MakeBubble();
    mCanvasView.AddDrawable(bubble);
    mCanvasView.SetEffectAutoPaddingEnable(mAutoPadding);
    ApplyEffect();

    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.AddChildren({mCanvasView});
    window.Add(root);
  }

  void ApplyEffect()
  {
    switch(mEffectMode)
    {
      case EffectMode::DropShadow:
        mCanvasView.SetDropShadow(kShadowColor,
                                  kShadowOffsetX * kScale,
                                  kShadowOffsetY * kScale,
                                  kShadowBlur * kScale);
        break;
      case EffectMode::GaussianBlur:
        mCanvasView.SetGaussianBlur(kBlurRadius * kScale);
        break;
      case EffectMode::None:
        mCanvasView.ClearDropShadow();
        mCanvasView.ClearGaussianBlur();
        break;
    }
    mCanvasView.RequestRasterization();
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyName() == "d" || event.GetKeyName() == "D")
    {
      mEffectMode = static_cast<EffectMode>((static_cast<int>(mEffectMode) + 1) % 3);
      ApplyEffect();
    }
    else if(event.GetKeyName() == "p" || event.GetKeyName() == "P")
    {
      mAutoPadding = !mAutoPadding;
      mCanvasView.SetEffectAutoPaddingEnable(mAutoPadding);
      mCanvasView.RequestRasterization();
    }
  }

private:
  Application&         mApplication;
  Dali::Ui::CanvasView mCanvasView;
  EffectMode           mEffectMode{EffectMode::DropShadow};
  bool                 mAutoPadding{true};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TooltipShadowSample sample(application);
  application.MainLoop();
  return 0;
}
