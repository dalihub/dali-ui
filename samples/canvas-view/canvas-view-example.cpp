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
 * CanvasView sample — demonstrates Dali::Ui::CanvasView with ThorVG drawables.
 *
 * Three scenes (Prev / Next buttons to navigate):
 *   0. BasicShapes  — rounded rect, circle, arc, star, DrawableGroup
 *   1. Gradients    — radial-gradient rect, linear-gradient star with gradient stroke
 *   2. Animation    — 30 fps timer: rotating star, sweeping arc, morphing rect corners
 *
 * Extra keys:
 *   S   — toggle synchronous / asynchronous rasterization
 *   ESC — quit
 *
 * Design notes:
 *   - Root: AbsoluteLayout (MATCH_PARENT). CanvasView is a proportional (1.0 x 1.0)
 *     child, filling the entire window. The Prev/Next button bar is a second child
 *     with absolute pixel bounds at (0, mH-64, mW, 64), overlaid on top of the canvas.
 *     This keeps the canvas full-screen while the buttons remain visible.
 *   - For animation, RequestRasterization() is called explicitly each tick.
 *     This resets mLastCommitRasterized so the next Process() cycle is guaranteed
 *     to rasterize even if IsCanvasChanged() misses the change.
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/canvas/canvas-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-drawable-group.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-gradient.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-linear-gradient.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-radial-gradient.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-shape.h>
using namespace Dali;
using namespace Dali::Ui;

namespace
{
enum Scene
{
  SCENE_BASIC_SHAPES = 0,
  SCENE_GRADIENTS,
  SCENE_ANIMATION,
  SCENE_COUNT
};

// Star path in local space (centred at origin, fits ≈ ±174 units).
// Caller applies Scale + Translate to position in canvas space.
Dali::CanvasRenderer::Shape MakeStar()
{
  Dali::CanvasRenderer::Shape s = Dali::CanvasRenderer::Shape::New();
  s.AddMoveTo(Vector2(-1.0f, -165.0f));
  s.AddLineTo(Vector2(53.0f, -56.0f));
  s.AddLineTo(Vector2(174.0f, -39.0f));
  s.AddLineTo(Vector2(87.0f, 45.0f));
  s.AddLineTo(Vector2(107.0f, 166.0f));
  s.AddLineTo(Vector2(-1.0f, 110.0f));
  s.AddLineTo(Vector2(-103.0f, 166.0f));
  s.AddLineTo(Vector2(-88.0f, 46.0f));
  s.AddLineTo(Vector2(-174.0f, -38.0f));
  s.AddLineTo(Vector2(-54.0f, -56.0f));
  s.Close();
  return s;
}

} // namespace

class CanvasViewSample : public ConnectionTracker
{
public:
  explicit CanvasViewSample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &CanvasViewSample::OnInit);
  }

private:
  // ---------------------------------------------------------------------------
  // Initialisation
  // ---------------------------------------------------------------------------

  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::BLACK);
    window.KeyEventSignal().Connect(this, &CanvasViewSample::OnKeyEvent);

    auto posSize = window.GetPositionSize();
    mW           = static_cast<float>(posSize.width);
    mH           = static_cast<float>(posSize.height);

    // Canvas fills the full window area; AbsoluteLayout overlay lets the
    // button bar sit on top without shrinking the canvas.
    mCanvasView = CanvasView::New();
    mCanvasView.SetLayoutParams(AbsoluteLayoutParams::New()
                                  .SetBounds(LayoutRect(0.f, 0.f, 1.f, 1.f))
                                  .SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL |
                                            AbsoluteLayoutFlags::POSITION_PROPORTIONAL));

    const float btnH = 64.0f;

    Label prevLabel = Label::New("< Prev");
    prevLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetX(0.5f).SetY(0.5f).SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
    prevLabel.SetRequestedWidth(WRAP_CONTENT);
    prevLabel.SetRequestedHeight(WRAP_CONTENT);
    prevLabel.SetTextColor(UiColor(1.f, 1.f, 1.f, 1.f));
    prevLabel.SetFontSize(26.f);
    prevLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    prevLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    AbsoluteLayout prevButton = AbsoluteLayout::New();
    prevButton.SetRequestedWidth(WRAP_CONTENT);
    prevButton.SetRequestedHeight(MATCH_PARENT);
    prevButton.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    prevButton.SetBackgroundColor(UiColor(0x222222, 0.75f));
    prevButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    { OnPrevScene(); });
    prevButton.Add(prevLabel);

    Label nextLabel = Label::New("Next >");
    nextLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetX(0.5f).SetY(0.5f).SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
    nextLabel.SetRequestedWidth(WRAP_CONTENT);
    nextLabel.SetRequestedHeight(WRAP_CONTENT);
    nextLabel.SetTextColor(UiColor(1.f, 1.f, 1.f, 1.f));
    nextLabel.SetFontSize(26.f);
    nextLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    nextLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    AbsoluteLayout nextButton = AbsoluteLayout::New();
    nextButton.SetRequestedWidth(WRAP_CONTENT);
    nextButton.SetRequestedHeight(MATCH_PARENT);
    nextButton.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    nextButton.SetBackgroundColor(UiColor(0x222222, 0.75f));
    nextButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    { OnNextScene(); });
    nextButton.Add(nextLabel);

    StackLayout btnBar = StackLayout::New(StackOrientation::HORIZONTAL);
    btnBar.SetSpacing(4.0f);
    btnBar.SetLayoutParams(AbsoluteLayoutParams::New()
                             .SetX(0.f)
                             .SetY(mH - btnH)
                             .SetWidth(mW)
                             .SetHeight(btnH));
    btnBar.Add(prevButton);
    btnBar.Add(nextButton);

    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.Add(mCanvasView);
    root.Add(btnBar);
    window.Add(root);

    mTimer = Timer::New(static_cast<uint32_t>(1000.0f / 30.0f)); // 30 fps
    mTimer.TickSignal().Connect(this, &CanvasViewSample::OnTick);

    LoadScene(mCurrentScene);
  }

  // ---------------------------------------------------------------------------
  // Scene management
  // ---------------------------------------------------------------------------

  void LoadScene(int scene)
  {
    mTimer.Stop();
    mCanvasView.RemoveAllDrawables();

    mAnimStar = Dali::CanvasRenderer::Shape();
    mAnimArc  = Dali::CanvasRenderer::Shape();
    mAnimRect = Dali::CanvasRenderer::Shape();
    mAnimTick = 0;

    switch(scene)
    {
      case SCENE_BASIC_SHAPES:
        CreateBasicShapes();
        break;
      case SCENE_GRADIENTS:
        CreateGradients();
        break;
      case SCENE_ANIMATION:
        CreateAnimation();
        break;
    }

    // Explicitly request a rasterization pass after loading the new scene
    // so the content appears even before any automatic canvas-change detection.
    mCanvasView.RequestRasterization();
  }

  // ---------------------------------------------------------------------------
  // Scene 0: BasicShapes
  // ---------------------------------------------------------------------------

  void CreateBasicShapes()
  {
    const float W = mW;
    const float H = mH;

    // White background
    Dali::CanvasRenderer::Shape bg = Dali::CanvasRenderer::Shape::New();
    bg.AddRect(Bounds(0.f, 0.f, W, H), Vector2::ZERO);
    bg.SetFillColor(Color::WHITE);
    mCanvasView.AddDrawable(bg);

    // Rounded rect: blue fill, dashed yellow stroke
    Dali::CanvasRenderer::Shape roundRect = Dali::CanvasRenderer::Shape::New();
    roundRect.AddRect(Bounds(W * 0.06f, H * 0.05f, W * 0.52f, H * 0.13f), Vector2(22.f, 22.f));
    roundRect.SetFillColor(Vector4(0.18f, 0.54f, 0.92f, 0.9f));
    roundRect.SetStrokeColor(Vector4(1.0f, 0.85f, 0.0f, 1.0f));
    roundRect.SetStrokeWidth(6.f);
    {
      Dali::Vector<float> dash;
      dash.PushBack(20.f);
      dash.PushBack(10.f);
      roundRect.SetStrokeDash(dash);
    }
    mCanvasView.AddDrawable(roundRect);

    // Red circle: white outline
    Dali::CanvasRenderer::Shape circle = Dali::CanvasRenderer::Shape::New();
    circle.AddCircle(Vector2(W * 0.77f, H * 0.14f), Vector2(W * 0.15f, W * 0.15f));
    circle.SetFillColor(Vector4(0.92f, 0.22f, 0.18f, 0.88f));
    circle.SetStrokeColor(Color::WHITE);
    circle.SetStrokeWidth(4.f);
    mCanvasView.AddDrawable(circle);

    // Green arc (round cap, 200° sweep)
    Dali::CanvasRenderer::Shape arc = Dali::CanvasRenderer::Shape::New();
    arc.AddArc(Vector2(W * 0.26f, H * 0.41f), W * 0.20f, -20.f, 200.f, false);
    arc.SetStrokeColor(Vector4(0.1f, 0.75f, 0.3f, 1.0f));
    arc.SetStrokeWidth(9.f);
    arc.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);
    mCanvasView.AddDrawable(arc);

    // Orange rotated square
    Dali::CanvasRenderer::Shape square = Dali::CanvasRenderer::Shape::New();
    square.AddRect(Bounds(-W * 0.10f, -H * 0.07f, W * 0.20f, H * 0.14f), Vector2::ZERO);
    square.SetFillColor(Vector4(1.0f, 0.55f, 0.0f, 0.65f));
    square.SetStrokeColor(Vector4(0.7f, 0.1f, 0.1f, 0.9f));
    square.SetStrokeWidth(5.f);
    square.Rotate(Degree(35.f));
    square.Translate(Vector2(W * 0.74f, H * 0.41f));
    mCanvasView.AddDrawable(square);

    // DrawableGroup: two overlapping rects with shared opacity
    Dali::CanvasRenderer::Shape rect1 = Dali::CanvasRenderer::Shape::New();
    rect1.AddRect(Bounds(W * 0.06f, H * 0.37f, W * 0.40f, H * 0.10f), Vector2::ZERO);
    rect1.SetFillColor(Vector4(1.0f, 0.35f, 0.0f, 1.0f));

    Dali::CanvasRenderer::Shape rect2 = Dali::CanvasRenderer::Shape::New();
    rect2.AddRect(Bounds(W * 0.22f, H * 0.42f, W * 0.40f, H * 0.10f), Vector2::ZERO);
    rect2.SetFillColor(Vector4(0.5f, 0.0f, 0.9f, 1.0f));

    Dali::CanvasRenderer::DrawableGroup group = Dali::CanvasRenderer::DrawableGroup::New();
    group.AddDrawable(rect1);
    group.AddDrawable(rect2);
    group.SetOpacity(0.75f);
    mCanvasView.AddDrawable(group);

    // Cyan star
    Dali::CanvasRenderer::Shape starA = MakeStar();
    starA.SetFillColor(Vector4(0.0f, 0.88f, 0.88f, 1.0f));
    starA.SetStrokeColor(Vector4(0.1f, 0.1f, 0.4f, 1.0f));
    starA.SetStrokeWidth(6.f);
    starA.Scale(W / 650.f * 0.55f);
    starA.Translate(Vector2(W * 0.26f, H * 0.70f));
    mCanvasView.AddDrawable(starA);

    // Magenta star (even-odd fill rule)
    Dali::CanvasRenderer::Shape starB = MakeStar();
    starB.SetFillColor(Vector4(0.9f, 0.25f, 0.75f, 0.9f));
    starB.SetStrokeColor(Color::WHITE);
    starB.SetStrokeWidth(4.f);
    starB.SetFillRule(Dali::CanvasRenderer::Shape::FillRule::EVEN_ODD);
    starB.Scale(W / 650.f * 0.42f);
    starB.Translate(Vector2(W * 0.74f, H * 0.72f));
    mCanvasView.AddDrawable(starB);

    // Bottom bar
    Dali::CanvasRenderer::Shape bar = Dali::CanvasRenderer::Shape::New();
    bar.AddRect(Bounds(W * 0.06f, H * 0.87f, W * 0.88f, H * 0.08f), Vector2::ZERO);
    bar.SetFillColor(Vector4(0.92f, 0.92f, 0.92f, 1.0f));
    bar.SetStrokeColor(Vector4(0.3f, 0.3f, 0.3f, 1.0f));
    bar.SetStrokeWidth(4.f);
    bar.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::MITER);
    mCanvasView.AddDrawable(bar);
  }

  // ---------------------------------------------------------------------------
  // Scene 1: Gradients
  // ---------------------------------------------------------------------------

  void CreateGradients()
  {
    const float W = mW;
    const float H = mH;

    // White background
    Dali::CanvasRenderer::Shape bg = Dali::CanvasRenderer::Shape::New();
    bg.AddRect(Bounds(0.f, 0.f, W, H), Vector2::ZERO);
    bg.SetFillColor(Color::WHITE);
    mCanvasView.AddDrawable(bg);

    // Radial gradient rounded rect (top)
    {
      Dali::CanvasRenderer::RadialGradient grad = Dali::CanvasRenderer::RadialGradient::New();
      grad.SetBounds(Vector2(W * 0.5f, H * 0.18f), W * 0.38f);
      grad.AddColorStop(0.0f, Vector4(1.0f, 0.95f, 0.2f, 1.0f));
      grad.AddColorStop(0.55f, Vector4(1.0f, 0.45f, 0.0f, 1.0f));
      grad.AddColorStop(1.0f, Vector4(0.7f, 0.0f, 0.0f, 1.0f));
      grad.SetSpread(Dali::CanvasRenderer::Gradient::Spread::PAD);

      Dali::CanvasRenderer::Shape rect = Dali::CanvasRenderer::Shape::New();
      rect.AddRect(Bounds(W * 0.06f, H * 0.04f, W * 0.88f, H * 0.28f), Vector2(28.f, 28.f));
      rect.SetFillGradient(grad);
      rect.SetStrokeColor(Vector4(0.3f, 0.3f, 0.3f, 1.0f));
      rect.SetStrokeWidth(3.f);
      mCanvasView.AddDrawable(rect);
    }

    // Radial gradient REFLECT spread (middle-left)
    {
      Dali::CanvasRenderer::RadialGradient grad = Dali::CanvasRenderer::RadialGradient::New();
      grad.SetBounds(Vector2(W * 0.26f, H * 0.56f), W * 0.15f);
      grad.AddColorStop(0.0f, Vector4(0.2f, 0.9f, 0.5f, 1.0f));
      grad.AddColorStop(1.0f, Vector4(0.0f, 0.2f, 0.8f, 1.0f));
      grad.SetSpread(Dali::CanvasRenderer::Gradient::Spread::REFLECT);

      Dali::CanvasRenderer::Shape rect = Dali::CanvasRenderer::Shape::New();
      rect.AddRect(Bounds(W * 0.06f, H * 0.38f, W * 0.40f, H * 0.36f), Vector2(20.f, 20.f));
      rect.SetFillGradient(grad);
      mCanvasView.AddDrawable(rect);
    }

    // Linear gradient REPEAT spread (middle-right)
    {
      Dali::CanvasRenderer::LinearGradient grad = Dali::CanvasRenderer::LinearGradient::New();
      grad.SetBounds(Vector2(W * 0.54f, H * 0.38f), Vector2(W * 0.54f + W * 0.08f, H * 0.38f));
      grad.AddColorStop(0.0f, Vector4(0.6f, 0.1f, 0.9f, 1.0f));
      grad.AddColorStop(1.0f, Vector4(0.1f, 0.8f, 0.9f, 1.0f));
      grad.SetSpread(Dali::CanvasRenderer::Gradient::Spread::REPEAT);

      Dali::CanvasRenderer::Shape rect = Dali::CanvasRenderer::Shape::New();
      rect.AddRect(Bounds(W * 0.54f, H * 0.38f, W * 0.40f, H * 0.36f), Vector2(20.f, 20.f));
      rect.SetFillGradient(grad);
      mCanvasView.AddDrawable(rect);
    }

    // Star: linear gradient fill + linear gradient stroke
    {
      Dali::CanvasRenderer::LinearGradient fillGrad = Dali::CanvasRenderer::LinearGradient::New();
      fillGrad.SetBounds(Vector2(-160.f, -160.f), Vector2(160.f, 160.f));
      fillGrad.AddColorStop(0.0f, Vector4(0.0f, 0.9f, 0.5f, 1.0f));
      fillGrad.AddColorStop(1.0f, Vector4(0.0f, 0.3f, 1.0f, 1.0f));

      Dali::CanvasRenderer::LinearGradient strokeGrad = Dali::CanvasRenderer::LinearGradient::New();
      strokeGrad.SetBounds(Vector2(-160.f, -160.f), Vector2(160.f, 160.f));
      strokeGrad.AddColorStop(0.0f, Vector4(1.0f, 0.85f, 0.0f, 1.0f));
      strokeGrad.AddColorStop(1.0f, Vector4(1.0f, 0.2f, 0.2f, 1.0f));

      Dali::CanvasRenderer::Shape star = MakeStar();
      star.SetFillGradient(fillGrad);
      star.SetStrokeGradient(strokeGrad);
      star.SetStrokeWidth(20.f);
      star.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);
      star.Scale(W / 650.f * 0.68f);
      star.Translate(Vector2(W * 0.5f, H * 0.80f));
      mCanvasView.AddDrawable(star);
    }
  }

  // ---------------------------------------------------------------------------
  // Scene 2: Animation
  // ---------------------------------------------------------------------------

  void CreateAnimation()
  {
    const float W = mW;
    const float H = mH;

    // Dark background
    Dali::CanvasRenderer::Shape bg = Dali::CanvasRenderer::Shape::New();
    bg.AddRect(Bounds(0.f, 0.f, W, H), Vector2::ZERO);
    bg.SetFillColor(Vector4(0.07f, 0.07f, 0.11f, 1.0f));
    mCanvasView.AddDrawable(bg);

    // Rotating star
    mAnimStar = MakeStar();
    mAnimStar.SetFillColor(Vector4(0.0f, 0.88f, 0.88f, 0.9f));
    mAnimStar.SetStrokeColor(Color::WHITE);
    mAnimStar.SetStrokeWidth(5.f);
    mAnimStar.Scale(W / 650.f * 0.52f);
    mAnimStar.Translate(Vector2(W * 0.5f, H * 0.22f));
    mCanvasView.AddDrawable(mAnimStar);

    // Arc track ring
    Dali::CanvasRenderer::Shape track = Dali::CanvasRenderer::Shape::New();
    track.AddArc(Vector2(W * 0.5f, H * 0.53f), W * 0.22f, 0.f, 360.f, false);
    track.SetStrokeColor(Vector4(0.25f, 0.25f, 0.25f, 0.7f));
    track.SetStrokeWidth(14.f);
    mCanvasView.AddDrawable(track);

    // Sweeping arc (path updated each tick via ResetPath)
    mAnimArc = Dali::CanvasRenderer::Shape::New();
    mAnimArc.AddArc(Vector2(W * 0.5f, H * 0.53f), W * 0.22f, -90.f, 1.f, false);
    mAnimArc.SetStrokeColor(Vector4(0.25f, 0.85f, 0.35f, 1.0f));
    mAnimArc.SetStrokeWidth(14.f);
    mAnimArc.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);
    mCanvasView.AddDrawable(mAnimArc);

    // Morphing rounded rect (corner radius updated each tick)
    mAnimRect = Dali::CanvasRenderer::Shape::New();
    mAnimRect.AddRect(Bounds(W * 0.10f, H * 0.74f, W * 0.80f, H * 0.12f), Vector2::ZERO);
    mAnimRect.SetFillColor(Vector4(0.88f, 0.42f, 0.10f, 0.9f));
    mAnimRect.SetStrokeColor(Vector4(1.0f, 0.82f, 0.0f, 1.0f));
    mAnimRect.SetStrokeWidth(5.f);
    mCanvasView.AddDrawable(mAnimRect);

    mTimer.Start();
  }

  // ---------------------------------------------------------------------------
  // Animation tick
  // ---------------------------------------------------------------------------

  bool OnTick()
  {
    if(!mAnimStar)
    {
      return false;
    }

    const float W = mW;
    const float H = mH;
    const int   t = mAnimTick;

    // Absolute rotation — increases 2° per frame
    mAnimStar.Rotate(Degree(t * 2.0f));

    // Sweep arc clockwise from -90° (top), 0→360° over 360 ticks
    float sweep = float(t % 360) + 1.f;
    mAnimArc.ResetPath();
    mAnimArc.AddArc(Vector2(W * 0.5f, H * 0.53f), W * 0.22f, -90.f, sweep, false);

    // Morph corner radius 0 → max → 0 over 200 ticks
    float maxCorner = H * 0.06f;
    float phase     = float(t % 200) / 200.f;
    float corner    = (phase < 0.5f ? phase * 2.f : 2.f - phase * 2.f) * maxCorner;
    mAnimRect.ResetPath();
    mAnimRect.AddRect(Bounds(W * 0.10f, H * 0.74f, W * 0.80f, H * 0.12f), Vector2(corner, corner));

    ++mAnimTick;

    // Explicitly request rasterization so the next Process() cycle forces a
    // re-render regardless of whether IsCanvasChanged() detects the change.
    mCanvasView.RequestRasterization();
    return true;
  }

  // ---------------------------------------------------------------------------
  // Button callbacks
  // ---------------------------------------------------------------------------

  void OnPrevScene()
  {
    mCurrentScene = (mCurrentScene - 1 + SCENE_COUNT) % SCENE_COUNT;
    LoadScene(mCurrentScene);
  }

  void OnNextScene()
  {
    mCurrentScene = (mCurrentScene + 1) % SCENE_COUNT;
    LoadScene(mCurrentScene);
  }

  // ---------------------------------------------------------------------------
  // Key handling
  // ---------------------------------------------------------------------------

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
    else if(event.GetKeyName() == "s" || event.GetKeyName() == "S")
    {
      mIsSynchronous = !mIsSynchronous;
      mCanvasView.SetSynchronousLoading(mIsSynchronous);
    }
  }

private:
  Application&         mApplication;
  Dali::Ui::CanvasView mCanvasView;
  Timer                mTimer;

  float mW{0.f};
  float mH{0.f};

  // Animation scene — live handles modified in OnTick()
  Dali::CanvasRenderer::Shape mAnimStar;
  Dali::CanvasRenderer::Shape mAnimArc;
  Dali::CanvasRenderer::Shape mAnimRect;
  int                         mAnimTick{0};

  int  mCurrentScene{SCENE_BASIC_SHAPES};
  bool mIsSynchronous{true};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  CanvasViewSample sample(application);
  application.MainLoop();
  return 0;
}
