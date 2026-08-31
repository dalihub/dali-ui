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
 *
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>
#include <dali-ui-foundation/public-api/views/image/selectable-lottie-animation-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <dali.h>
#include <dali/devel-api/adaptor-framework/vector-animation-renderer.h>
#include <dali/public-api/actors/actor.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace Test::UiVectorAnimationRenderer
{
void                  ResetDynamicPropertyProbe();
uint32_t              GetLastRenderedFrame();
uint32_t              GetDynamicPropertyRefreshCount();
uint32_t              GetDynamicPropertyCount();
uint32_t              GetDynamicPropertyCount(const std::string&                            keyPath,
                                              Dali::VectorAnimationRenderer::VectorProperty property);
Dali::Property::Value EvaluateDynamicProperty(const std::string&                            keyPath,
                                              Dali::VectorAnimationRenderer::VectorProperty property,
                                              uint32_t                                      frameNumber);
Dali::Property::Value GetLastEvaluatedDynamicProperty(const std::string&                            keyPath,
                                                      Dali::VectorAnimationRenderer::VectorProperty property);
} // namespace Test::UiVectorAnimationRenderer

namespace
{
using FrameRange = SelectableLottieImage::FrameRange;
using Binding    = SelectableLottieColorBinding;
using Policy     = Binding::ColorPolicy;

constexpr char INNER_PATH[]   = "radio_inner.inner_fill.color";
constexpr char OUTLINE_PATH[] = "radio_outline.outline_stroke.color";
const Vector4  DESELECTED_COLOR(1.0f, 0.0f, 0.0f, 1.0f);
const Vector4  SELECTED_COLOR(0.0f, 1.0f, 0.0f, 1.0f);

SelectableLottieAnimationView NewGlyph()
{
  return SelectableLottieAnimationView::New(
    SelectableLottieImage("checkbox.json", FrameRange(0, 30), FrameRange(30, 48)));
}

SelectableLottieImage NewImage(const Dali::String& keyPath = Dali::String())
{
  return SelectableLottieImage("radio.json", FrameRange(0, 19), FrameRange(20, 38), keyPath);
}

SelectableLottieColorBindings NewRadioBindings()
{
  SelectableLottieColorBindings bindings;
  bindings.PushBack(Binding(INNER_PATH,
                            LottieAnimation::VectorProperty::FILL_COLOR,
                            Policy::ALWAYS_SELECTED));
  bindings.PushBack(Binding(OUTLINE_PATH,
                            LottieAnimation::VectorProperty::STROKE_COLOR,
                            Policy::SELECTED_IN_FRAME_RANGE,
                            FrameRange(7, 26)));
  return bindings;
}

void RenderAndWaitForFrame(UiTestApplication& application, uint32_t expectedFrame)
{
  for(uint32_t attempt = 0u; attempt < 8u; ++attempt)
  {
    application.SendNotification();
    application.Render();
    if(Test::UiVectorAnimationRenderer::GetLastRenderedFrame() == expectedFrame)
    {
      return;
    }
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  }
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLastRenderedFrame(), expectedFrame, TEST_LOCATION);
}

void RenderAndWaitForBindingCount(UiTestApplication& application, uint32_t expectedBindingCount)
{
  bool workerCompleted = false;
  for(uint32_t attempt = 0u; attempt < 8u; ++attempt)
  {
    application.SendNotification();
    application.Render();
    if(Test::UiVectorAnimationRenderer::GetDynamicPropertyCount() == expectedBindingCount &&
       (expectedBindingCount > 0u || workerCompleted))
    {
      return;
    }
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
    workerCompleted = true;
  }
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetDynamicPropertyCount(), expectedBindingCount, TEST_LOCATION);
}

void RealizeGlyph(UiTestApplication&            application,
                  SelectableLottieAnimationView glyph,
                  uint32_t                      expectedBindingCount = 1u)
{
  View drawingView = glyph.GetView();
  LottieAnimationView::DownCast(drawingView).SetSynchronousLoading(true);
  drawingView.SetRequestedWidth(36.0f);
  drawingView.SetRequestedHeight(36.0f);
  application.GetScene().Add(drawingView);
  application.SendNotification();
  application.Render();

  glyph.SetStateColors(DESELECTED_COLOR, SELECTED_COLOR);
  // Force both transitions so the final deselected state re-registers bindings on the
  // current visual even when the initial false state was already active.
  glyph.SetSelected(true, false);
  glyph.SetSelected(false, false);
  RenderAndWaitForBindingCount(application, expectedBindingCount);
}

Vector4 EvaluateColor(const std::string&                            keyPath,
                      Dali::VectorAnimationRenderer::VectorProperty property,
                      uint32_t                                      frameNumber)
{
  return Test::UiVectorAnimationRenderer::EvaluateDynamicProperty(keyPath, property, frameNumber).Get<Vector4>();
}
} // namespace

void utc_dali_selectable_lottie_animation_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_selectable_lottie_animation_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

// An uninitialized handle is empty.
int UtcDaliSelectableLottieAnimationViewConstructorP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

// New(SelectableLottieImage) produces an initialized handle.
int UtcDaliSelectableLottieAnimationViewNewP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// GetView() returns the composed drawing view (a LottieAnimationView) carrying the url.
int UtcDaliSelectableLottieAnimationViewGetViewP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  Ui::View drawingView = view.GetView();
  DALI_TEST_CHECK(drawingView);

  LottieAnimationView asLottie = LottieAnimationView::DownCast(drawingView);
  DALI_TEST_CHECK(asLottie);
  DALI_TEST_EQUALS(asLottie.GetResourceUrl(), std::string("checkbox.json"), TEST_LOCATION);

  END_TEST;
}

// DownCasts from a base handle through the SelectableImageInterface handle chain.
int UtcDaliSelectableLottieAnimationViewDownCastP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  BaseHandle                    base      = view;
  SelectableLottieAnimationView roundTrip = SelectableLottieAnimationView::DownCast(base);
  DALI_TEST_CHECK(roundTrip);

  SelectableImageInterface asInterface = SelectableImageInterface::DownCast(base);
  DALI_TEST_CHECK(asInterface);

  END_TEST;
}

// DownCast of an empty or unrelated handle yields an empty handle.
int UtcDaliSelectableLottieAnimationViewDownCastN(void)
{
  UiTestApplication application;

  DALI_TEST_CHECK(!SelectableLottieAnimationView::DownCast(BaseHandle()));
  DALI_TEST_CHECK(!SelectableLottieAnimationView::DownCast(Actor::New()));

  END_TEST;
}

// SetSelected snap/animate variants run without a live visual (headless).
int UtcDaliSelectableLottieAnimationViewSetSelectedP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  view.SetSelected(true);
  DALI_TEST_CHECK(view);

  view.SetSelected(false, false);
  DALI_TEST_CHECK(view);

  view.SetSelected(true, true);
  DALI_TEST_CHECK(view);

  END_TEST;
}

// SetStateColors configures the glyph without crashing; IsTransitioning is false headless.
int UtcDaliSelectableLottieAnimationViewConfigureP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  view.SetStateColors(Vector4(0.f, 0.f, 0.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f));
  view.SetSelected(true, false);
  DALI_TEST_CHECK(!view.IsTransitioning());

  END_TEST;
}

int UtcDaliSelectableLottieAnimationViewSnapClampsToLoadedFrameRangeP(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetDynamicPropertyProbe();

  SelectableLottieAnimationView view = SelectableLottieAnimationView::New(
    SelectableLottieImage("checkbox.json", FrameRange(0, 19), FrameRange(20, 39)));
  View drawingView = view.GetView();
  LottieAnimationView::DownCast(drawingView).SetSynchronousLoading(true);
  drawingView.SetRequestedWidth(36.0f);
  drawingView.SetRequestedHeight(36.0f);
  application.GetScene().Add(drawingView);
  application.SendNotification();
  application.Render();

  view.SetSelected(false, false);
  RenderAndWaitForFrame(application, 4u);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLastRenderedFrame(), 4u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSelectableLottieAnimationViewSnapStopsTransitionP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  view.SetSelected(true, true);
  DALI_TEST_CHECK(view.IsTransitioning());
  view.SetSelected(false, false);
  DALI_TEST_CHECK(!view.IsTransitioning());
  END_TEST;
}

int UtcDaliSelectableLottieAnimationViewStoppedColorRefreshP(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetDynamicPropertyProbe();

  SelectableLottieAnimationView view = SelectableLottieAnimationView::New(NewImage(INNER_PATH));
  RealizeGlyph(application, view);
  const uint32_t refreshCount = Test::UiVectorAnimationRenderer::GetDynamicPropertyRefreshCount();

  view.SetStateColors(Color::BLUE, Color::RED);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(Test::UiVectorAnimationRenderer::GetDynamicPropertyRefreshCount() > refreshCount);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLastEvaluatedDynamicProperty(
                     INNER_PATH, Dali::VectorAnimationRenderer::VectorProperty::FILL_COLOR)
                     .Get<Vector4>(),
                   Color::BLUE,
                   TEST_LOCATION);
  END_TEST;
}

// The transition-finished signal is accessible through the interface.
int UtcDaliSelectableLottieAnimationViewTransitionFinishedSignalP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  // Simply accessing the signal must not crash (it forwards the composed view's signal).
  (void)view.TransitionFinishedSignal();
  DALI_TEST_CHECK(view);

  END_TEST;
}

// Copy and move preserve the underlying image.
int UtcDaliSelectableLottieAnimationViewCopyMoveP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  SelectableLottieAnimationView copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy.GetView());

  SelectableLottieAnimationView moved(std::move(copy));
  DALI_TEST_CHECK(moved);

  END_TEST;
}

int UtcDaliSelectableLottieAnimationViewLegacyBindingP(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetDynamicPropertyProbe();

  SelectableLottieAnimationView glyph = SelectableLottieAnimationView::New(NewImage(INNER_PATH));
  RealizeGlyph(application, glyph);

  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetDynamicPropertyCount(
                     INNER_PATH, Dali::VectorAnimationRenderer::VectorProperty::FILL_COLOR),
                   1u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(EvaluateColor(INNER_PATH, Dali::VectorAnimationRenderer::VectorProperty::FILL_COLOR, 0u),
                   DESELECTED_COLOR,
                   TEST_LOCATION);

  glyph.SetSelected(true, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(EvaluateColor(INNER_PATH, Dali::VectorAnimationRenderer::VectorProperty::FILL_COLOR, 0u),
                   SELECTED_COLOR,
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliSelectableLottieAnimationViewExplicitEmptyBindingsP(void)
{
  UiTestApplication             application;
  SelectableLottieColorBindings emptyBindings;
  Test::UiVectorAnimationRenderer::ResetDynamicPropertyProbe();

  SelectableLottieAnimationView glyph = SelectableLottieAnimationView::New(NewImage(), emptyBindings);
  RealizeGlyph(application, glyph, 0u);

  END_TEST;
}

int UtcDaliSelectableLottieAnimationViewDuplicateBindingN(void)
{
  UiTestApplication             application;
  SelectableLottieColorBindings bindings;
  bindings.PushBack(Binding(INNER_PATH, LottieAnimation::VectorProperty::FILL_COLOR, Policy::ALWAYS_SELECTED));
  bindings.PushBack(Binding(INNER_PATH, LottieAnimation::VectorProperty::FILL_COLOR, Policy::ALWAYS_DESELECTED));

  DALI_TEST_ASSERTION(SelectableLottieAnimationView::New(NewImage(), bindings),
                      "unique key-path and property pairs");
  END_TEST;
}

int UtcDaliSelectableLottieAnimationViewColorPoliciesP(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetDynamicPropertyProbe();

  SelectableLottieAnimationView glyph = SelectableLottieAnimationView::New(NewImage(), NewRadioBindings());
  RealizeGlyph(application, glyph, 2u);

  DALI_TEST_EQUALS(EvaluateColor(INNER_PATH, Dali::VectorAnimationRenderer::VectorProperty::FILL_COLOR, 0u),
                   SELECTED_COLOR,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(EvaluateColor(INNER_PATH, Dali::VectorAnimationRenderer::VectorProperty::FILL_COLOR, 100u),
                   SELECTED_COLOR,
                   TEST_LOCATION);

  glyph.SetSelected(true, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(EvaluateColor(INNER_PATH, Dali::VectorAnimationRenderer::VectorProperty::FILL_COLOR, 0u),
                   SELECTED_COLOR,
                   TEST_LOCATION);

  DALI_TEST_EQUALS(EvaluateColor(OUTLINE_PATH, Dali::VectorAnimationRenderer::VectorProperty::STROKE_COLOR, 6u),
                   DESELECTED_COLOR,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(EvaluateColor(OUTLINE_PATH, Dali::VectorAnimationRenderer::VectorProperty::STROKE_COLOR, 7u),
                   SELECTED_COLOR,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(EvaluateColor(OUTLINE_PATH, Dali::VectorAnimationRenderer::VectorProperty::STROKE_COLOR, 26u),
                   SELECTED_COLOR,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(EvaluateColor(OUTLINE_PATH, Dali::VectorAnimationRenderer::VectorProperty::STROKE_COLOR, 27u),
                   DESELECTED_COLOR,
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliSelectableLottieAnimationViewRebuildDoesNotDuplicateBindingsP(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetDynamicPropertyProbe();

  SelectableLottieAnimationView glyph = SelectableLottieAnimationView::New(NewImage(), NewRadioBindings());
  RealizeGlyph(application, glyph, 2u);

  glyph.SetSelected(true, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetDynamicPropertyCount(), 2u, TEST_LOCATION);

  glyph.SetSelected(false, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetDynamicPropertyCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetDynamicPropertyCount(
                     INNER_PATH, Dali::VectorAnimationRenderer::VectorProperty::FILL_COLOR),
                   1u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetDynamicPropertyCount(
                     OUTLINE_PATH, Dali::VectorAnimationRenderer::VectorProperty::STROKE_COLOR),
                   1u,
                   TEST_LOCATION);
  END_TEST;
}
