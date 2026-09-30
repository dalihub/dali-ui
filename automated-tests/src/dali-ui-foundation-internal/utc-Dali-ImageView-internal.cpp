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

#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-transform.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <ui-event-thread-callback.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
Dali::Ui::Internal::ViewDataImpl& DataOf(ImageView view)
{
  return Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(view));
}

/**
 * @brief Counts how often the layout system asks this view to measure.
 *
 * ViewDataImpl::Measure() returns the cached size without reaching the measure
 * step while the cache is valid, so an unchanged count across a re-measure with
 * identical constraints means the cache survived. The callback must be installed
 * before the first Measure, because installing one invalidates the cache.
 */
struct MeasureCounter
{
  MeasuredSize OnMeasure(Ui::View view, float, float)
  {
    ++count;
    return MeasuredSize(view.GetRequestedWidth(), view.GetRequestedHeight());
  }

  uint32_t count{0u};
};
} // namespace

void utc_dali_image_view_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_image_view_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliImageViewFixedSizeSetUrlSkipsMeasureInvalidation(void)
{
  UiTestApplication application;

  // Installing a measure callback invalidates the cache, so it must be in place
  // before the first Measure() establishes the cached entry under test.
  MeasureCounter counter;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.SetMeasureCallback(MeasureCallback::New(&counter, &MeasureCounter::OnMeasure));
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  view.SetResourceUrl("image.png");

  // Both measured axes are independent of the image's natural size. The visual is
  // rebuilt immediately, without retracting the still-correct measure cache: the
  // repeated Measure() with identical constraints is served from the cache and
  // never reaches the measure step again.
  DALI_TEST_CHECK(viewData.GetVisual(ImageView::Property::IMAGE));
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewZeroRequestedSizeSetUrlInvalidatesMeasure(void)
{
  UiTestApplication application;

  // Installing a measure callback invalidates the cache, so it must be in place
  // before the first Measure() establishes the cached entry under test.
  MeasureCounter counter;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(0.0f);
  view.SetRequestedHeight(100.0f);
  view.SetMeasureCallback(MeasureCallback::New(&counter, &MeasureCounter::OnMeasure));
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  view.SetResourceUrl("image.png");

  // Preserve ImageView's existing strictly-positive fixed-size rule: zero does
  // not enter the SetUrl fast path.
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  // A repeated Measure() with identical constraints is served from the cache
  // unless SetResourceUrl() retracted it, so reaching the measure step a second
  // time is proof that the cached measurement was invalidated.
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewMatchParentSetUrlInvalidatesMeasure(void)
{
  UiTestApplication application;

  // Installing a measure callback invalidates the cache, so it must be in place
  // before the first Measure() establishes the cached entry under test.
  MeasureCounter counter;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(100.0f);
  view.SetMeasureCallback(MeasureCallback::New(&counter, &MeasureCounter::OnMeasure));
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  view.SetResourceUrl("image.png");

  // Keep MATCH_PARENT out of the explicit-size optimization. Its final size is
  // supplied by the parent constraint, so preserve the existing deferred path.
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  // A repeated Measure() with identical constraints is served from the cache
  // unless SetResourceUrl() retracted it, so reaching the measure step a second
  // time is proof that the cached measurement was invalidated.
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewWrapContentSetUrlInvalidatesMeasure(void)
{
  UiTestApplication application;

  // Installing a measure callback invalidates the cache, so it must be in place
  // before the first Measure() establishes the cached entry under test.
  MeasureCounter counter;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(WRAP_CONTENT);
  view.SetMeasureCallback(MeasureCallback::New(&counter, &MeasureCounter::OnMeasure));
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  view.SetResourceUrl("image.png");

  // A WRAP_CONTENT axis can change from the image aspect ratio, so this path keeps
  // the existing deferred visual rebuild and measure invalidation.
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  // A repeated Measure() with identical constraints is served from the cache
  // unless SetResourceUrl() retracted it, so reaching the measure step a second
  // time is proof that the cached measurement was invalidated.
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewFixedSizeSetUrlPreservesLayoutFinishedFitting(void)
{
  UiTestApplication application;

  // Installing a measure callback invalidates the cache, so it must be in place
  // before the first Measure() establishes the cached entry under test.
  MeasureCounter counter;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(200.0f);
  view.SetDesiredWidth(100);
  view.SetDesiredHeight(50);
  view.SetFittingMode(Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
  view.SetMeasureCallback(MeasureCallback::New(&counter, &MeasureCounter::OnMeasure));
  view.Measure(500.0f, 500.0f);

  auto& viewData = DataOf(view);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  view.SetResourceUrl("image.png");

  auto visual = viewData.GetVisual(ImageView::Property::IMAGE);
  DALI_TEST_CHECK(visual);
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  // The fast path does not request a layout itself. Fitting-required visuals must
  // still be connected to the View's LayoutFinished path at registration time, so
  // the final bounds are applied when the layout pass completes.
  viewData.EmitLayoutFinishedSignal(LayoutRect(0.0f, 0.0f, 200.0f, 200.0f));

  Property::Map visualMap;
  visual.CreatePropertyMap(visualMap);

  Property::Map transform;
  DALI_TEST_CHECK(visualMap.Find(Ui::Integration::Visual::Property::TRANSFORM)->Get(transform));

  Vector2 fittedSize;
  Vector2 fittedOffset;
  DALI_TEST_CHECK(transform.Find(Ui::Integration::Visual::Transform::Property::SIZE)->Get(fittedSize));
  DALI_TEST_CHECK(transform.Find(Ui::Integration::Visual::Transform::Property::OFFSET)->Get(fittedOffset));
  DALI_TEST_EQUALS(fittedSize, Vector2(200.0f, 100.0f), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(fittedOffset, Vector2(0.0f, 50.0f), 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewAutoNPatchBorderOnlyReachesVisual(void)
{
  UiTestApplication application;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.SetNPatchBorderOnly(true);
  view.SetResourceUrl("image.9.png");

  auto visual = DataOf(view).GetVisual(ImageView::Property::IMAGE);
  DALI_TEST_CHECK(visual);

  Property::Map visualMap;
  visual.CreatePropertyMap(visualMap);

  bool borderOnly = false;
  auto borderOnlyValue = visualMap.Find(Ui::Integration::ImageVisual::Property::BORDER_ONLY);
  DALI_TEST_CHECK(borderOnlyValue);
  DALI_TEST_CHECK(borderOnlyValue->Get(borderOnly));
  DALI_TEST_EQUALS(borderOnly, true, TEST_LOCATION);

  END_TEST;
}

namespace
{
struct ArrangeSvgPanel : ConnectionTracker
{
  ImageView    arrow;
  bool         delaySettlement{true};
  int          measures{0};
  int          finishes{0};
  MeasuredSize Measure(View view, float, float)
  {
    ++measures;
    if(!arrow)
    {
      arrow = ImageView::New(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/initial-layout.svg");
      arrow.SetRequestedWidth(40.0f);
      arrow.SetRequestedHeight(40.0f);
      arrow.SetBackgroundColor(UiColor(0x303030));
      view.Add(arrow);
      arrow.LayoutFinishedSignal().Connect(this, &ArrangeSvgPanel::Finished);
    }
    arrow.Measure(40.0f, 40.0f);
    // Bound the unsettled interval without changing any image property.
    if(delaySettlement && measures < 12)
    {
      Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(view)).InvalidateMeasure();
    }
    return MeasuredSize(100.0f, 100.0f);
  }
  LayoutRect Arrange(View, const LayoutRect& bounds)
  {
    arrow.Arrange(LayoutRect(4.0f, 4.0f, 40.0f, 40.0f));
    return bounds;
  }
  void Finished(View, LayoutRect)
  {
    ++finishes;
  }
};
} // namespace

// Drive event processing explicitly: this tests SVG size delivery while layout
// remains pending, independently of whether a follow-up idle wake is requested.
int UtcDaliImageViewSvgReadyBeforeLayoutSettles(void)
{
  UiTestApplication application;
  ArrangeSvgPanel   panel;
  View              root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  root.SetMeasureCallback(MeasureCallback::New(&panel, &ArrangeSvgPanel::Measure));
  root.SetArrangeCallback(ArrangeCallback::New(&panel, &ArrangeSvgPanel::Arrange));
  application.GetWindow().Add(root);

  for(int i = 0; i < 8 && (!panel.arrow || !panel.arrow.IsResourceReady()); ++i)
  {
    application.SendNotification();
    application.Render();
    if(!panel.arrow.IsResourceReady())
    {
      Test::WaitForEventThreadTrigger(1, 1);
    }
  }

  DALI_TEST_CHECK(panel.arrow.IsResourceReady());
  DALI_TEST_EQUALS(panel.finishes, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(panel.arrow.GetProperty<float>(Actor::Property::SIZE_WIDTH), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(panel.arrow.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(panel.arrow.GetRendererCount(), 2u, TEST_LOCATION);
  Texture texture = panel.arrow.GetRendererAt(1u).GetTextures().GetTexture(0u);

  panel.delaySettlement = false;
  Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(root)).InvalidateMeasure();
  for(int i = 0; i < 5 && panel.finishes == 0; ++i)
  {
    application.SendNotification();
    application.Render();
  }

  DALI_TEST_CHECK(panel.finishes > 0);
  DALI_TEST_CHECK(panel.arrow.IsResourceReady());
  DALI_TEST_EQUALS(panel.arrow.GetRendererCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(panel.arrow.GetRendererAt(1u).GetTextures().GetTexture(0u) == texture);
  END_TEST;
}
