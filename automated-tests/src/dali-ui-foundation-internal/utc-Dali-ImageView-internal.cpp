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

#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-transform.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
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

namespace
{
Dali::Ui::Internal::Visual::Base& VisualOf(ImageView view)
{
  auto visual = DataOf(view).GetVisual(ImageView::Property::IMAGE);
  return Ui::GetImplementation(visual);
}

ImageDimensions ReportedLoadSize(Ui::Integration::Visual::Base visual)
{
  Property::Map map;
  visual.CreatePropertyMap(map);
  return ImageDimensions(static_cast<uint32_t>(map.Find(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH)->Get<int>()),
                         static_cast<uint32_t>(map.Find(Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT)->Get<int>()));
}

ImageDimensions ReportedLoadSize(ImageView view)
{
  return ReportedLoadSize(DataOf(view).GetVisual(ImageView::Property::IMAGE));
}
} // namespace

int UtcDaliImageViewRebuildViewSizeAttachedFirstLoad(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.SetDesiredWidth(30);
  view.SetDesiredHeight(20);
  view.SetSynchronousLoading(true);
  view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  view.Arrange(LayoutRect(0, 0, 180, 90));

  view.SetImageLoadWithViewSizeEnabled(true);
  view.Measure(200, 100);
  auto& visual = VisualOf(view);
  DALI_TEST_EQUALS(ReportedLoadSize(view), ImageDimensions(180, 90), TEST_LOCATION);
  // Supplying the hint must not start an ATTACHED load while off scene.
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
  DALI_TEST_CHECK(!visual.GetRenderer().GetTextures());
  DALI_TEST_EQUALS(view.GetDesiredWidth(), 30, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetDesiredHeight(), 20, TEST_LOCATION);

  application.GetWindow().Add(view);
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
  const auto firstTexture = visual.GetRenderer().GetTextures().GetTexture(0);
  DALI_TEST_EQUALS(firstTexture.GetWidth(), 180u, TEST_LOCATION);
  DALI_TEST_EQUALS(firstTexture.GetHeight(), 90u, TEST_LOCATION);
  DataOf(view).EmitLayoutFinishedSignal(LayoutRect(0, 0, 180, 90));
  DALI_TEST_CHECK(visual.GetRenderer().GetTextures().GetTexture(0) == firstTexture);

  view.SetImageLoadWithViewSizeEnabled(false);
  view.Measure(200, 100);
  DALI_TEST_EQUALS(ReportedLoadSize(view), ImageDimensions(30, 20), TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewRebuildViewSizeOnScenePolicies(void)
{
  UiTestApplication application;
  for(auto policy : {Image::LoadPolicy::ATTACHED, Image::LoadPolicy::IMMEDIATE})
  {
    for(bool synchronous : {false, true})
    {
      ImageView view = ImageView::New();
      view.SetRequestedWidth(200);
      view.SetRequestedHeight(100);
      view.SetLoadPolicy(policy);
      view.SetSynchronousLoading(synchronous);
      view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
      application.GetWindow().Add(view);
      view.Arrange(LayoutRect(0, 0, 160, 80));
      view.SetImageLoadWithViewSizeEnabled(true);
      view.Measure(200, 100);
      // Inspect immediately, before LayoutFinished can conceal a wrong first load.
      DALI_TEST_EQUALS(ReportedLoadSize(view), ImageDimensions(160, 80), TEST_LOCATION);
      if(synchronous)
      {
        const auto texture = VisualOf(view).GetRenderer().GetTextures().GetTexture(0);
        DALI_TEST_EQUALS(texture.GetWidth(), 160u, TEST_LOCATION);
        DALI_TEST_EQUALS(texture.GetHeight(), 80u, TEST_LOCATION);
      }
      application.GetWindow().Remove(view);
    }
  }
  END_TEST;
}

int UtcDaliImageViewRebuildViewSizeUrlAndResize(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  view.SetRequestedWidth(200);
  view.SetRequestedHeight(100);
  view.SetImageLoadWithViewSizeEnabled(true);
  view.SetResourceUrl("rebuild-first.png");
  application.GetWindow().Add(view);
  view.Arrange(LayoutRect(0, 0, 150, 75));
  view.SetResourceUrl("rebuild-second.png");
  DALI_TEST_EQUALS(ReportedLoadSize(view), ImageDimensions(150, 75), TEST_LOCATION);

  view.Arrange(LayoutRect(0, 0, 120, 60));
  view.SetSamplingMode(Image::SamplingMode::NEAREST);
  view.Measure(200, 100);
  auto& visual = VisualOf(view);
  DALI_TEST_EQUALS(ReportedLoadSize(view), ImageDimensions(120, 60), TEST_LOCATION);
  DataOf(view).EmitLayoutFinishedSignal(LayoutRect(0, 0, 100, 50));
  DALI_TEST_EQUALS(ReportedLoadSize(view), ImageDimensions(100, 50), TEST_LOCATION);

  // A later desired-size property update must not reinstate the creation hint.
  Property::Map properties;
  properties.Insert(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, 20);
  properties.Insert(Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT, 10);
  visual.SetProperties(properties);
  DALI_TEST_EQUALS(ReportedLoadSize(view), ImageDimensions(100, 50), TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewRebuildViewSizeFallbackAndOtherVisuals(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  view.SetRequestedWidth(200);
  view.SetRequestedHeight(100);
  view.SetDesiredWidth(30);
  view.SetDesiredHeight(20);
  view.SetImageLoadWithViewSizeEnabled(true);
  view.SetResourceUrl("not-arranged.png");
  DALI_TEST_EQUALS(ReportedLoadSize(view), ImageDimensions(30, 20), TEST_LOCATION);

  view.Arrange(LayoutRect(0, 0, 200, 100));
  view.SetImageLoadWithViewSizeEnabled(false);
  view.Measure(200, 100);
  DALI_TEST_EQUALS(ReportedLoadSize(view), ImageDimensions(30, 20), TEST_LOCATION);

  view.SetImageLoadWithViewSizeEnabled(true);
  view.SetResourceUrl("preserve.svg");
  {
    Property::Map map;
    DataOf(view).GetVisual(ImageView::Property::IMAGE).CreatePropertyMap(map);
    DALI_TEST_CHECK(map.Find(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH));
    DALI_TEST_CHECK(map.Find(Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT));
    DALI_TEST_EQUALS(map.Find(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH)->Get<int>(), 30, TEST_LOCATION);
    DALI_TEST_EQUALS(map.Find(Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT)->Get<int>(), 20, TEST_LOCATION);
  }
  view.SetResourceUrl("preserve.9.png");
  DALI_TEST_EQUALS(Ui::GetImplementation(DataOf(view).GetVisual(ImageView::Property::IMAGE)).GetType(),
                   Ui::Integration::InternalVisualType::N_PATCH, TEST_LOCATION);

  // Desired size also seeds direct visual creation, without a factory option.
  // Put the flag before the size entries to verify property order independence.
  Property::Map map;
  map.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  map.Insert(Ui::Integration::ImageVisual::Property::URL, "ordinary-visual.png");
  map.Insert(Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE, true);
  map.Insert(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, 30);
  map.Insert(Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT, 20);
  auto visual = Ui::Integration::VisualFactory::Get().CreateVisual(map);
  DALI_TEST_EQUALS(ReportedLoadSize(visual), ImageDimensions(30, 20), TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewRebuildViewSizePreservesFitting(void)
{
  UiTestApplication application;
  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.5f);
  for(auto fitting : {Image::FittingMode::FILL, Image::FittingMode::FIT_KEEP_ASPECT_RATIO,
                      Image::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO, Image::FittingMode::CENTER})
  {
    ImageView view = ImageView::New();
    view.SetRequestedWidth(200);
    view.SetRequestedHeight(120);
    view.SetPadding(10, 10, 10, 10);
    view.SetFittingMode(fitting);
    view.SetSynchronousLoading(true);
    view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
    application.GetWindow().Add(view);
    view.Arrange(LayoutRect(0, 0, 300, 180));
    view.SetImageLoadWithViewSizeEnabled(true);
    view.Measure(300, 180);
    auto& visual = VisualOf(view);
    DALI_TEST_EQUALS(ReportedLoadSize(view), ImageDimensions(300, 180), TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
    const auto texture = visual.GetRenderer().GetTextures().GetTexture(0);
    // The loader preserves the source aspect ratio while covering the requested
    // 300x180 area: its decoded texture is 360x180, not a stretched 300x180.
    DALI_TEST_EQUALS(texture.GetWidth(), 360u, TEST_LOCATION);
    DALI_TEST_EQUALS(texture.GetHeight(), 180u, TEST_LOCATION);

    DataOf(view).EmitLayoutFinishedSignal(LayoutRect(0, 0, 300, 180));
    Property::Map map;
    DataOf(view).GetVisual(ImageView::Property::IMAGE).CreatePropertyMap(map);
    Property::Map transform;
    DALI_TEST_CHECK(map.Find(Ui::Integration::Visual::Property::TRANSFORM)->Get(transform));
    const bool fitInside = fitting == Image::FittingMode::FIT_KEEP_ASPECT_RATIO || fitting == Image::FittingMode::CENTER;
    // The decoded fixture has a 2:1 aspect ratio; padding is scaled exactly once.
    DALI_TEST_EQUALS(transform.Find(Ui::Integration::Visual::Transform::Property::SIZE)->Get<Vector2>(),
                     fitInside ? Vector2(270, 135) : Vector2(270, 150), 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(transform.Find(Ui::Integration::Visual::Transform::Property::OFFSET)->Get<Vector2>(),
                     fitInside ? Vector2(15, 22.5f) : Vector2(15, 15), 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(ReportedLoadSize(view), fitInside ? ImageDimensions(270, 135) : ImageDimensions(270, 150), TEST_LOCATION);
    application.GetWindow().Remove(view);
  }
  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

int UtcDaliImageViewRebuildViewSizeCreationHint(void)
{
  UiTestApplication application;
  auto factory = Ui::Integration::VisualFactory::Get();
  auto& factoryImpl = Ui::GetImplementation(factory);
  for(auto policy : {Image::LoadPolicy::ATTACHED, Image::LoadPolicy::IMMEDIATE})
  {
    Property::Map map;
    map.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::IMAGE);
    map.Insert(Ui::Integration::ImageVisual::Property::URL, DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
    map.Insert(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, 30);
    map.Insert(Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT, 20);
    map.Insert(Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE, true);
    map.Insert(Ui::Integration::ImageVisual::Property::LOAD_POLICY, static_cast<int>(policy));
    map.Insert(Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, true);

    auto visual = factoryImpl.CreateVisual(map, Ui::Integration::VisualFactory::NONE, Vector2(160.4f, 79.6f));
    DALI_TEST_EQUALS(ReportedLoadSize(visual), ImageDimensions(160, 80), TEST_LOCATION);
    Actor actor = Actor::New();
    application.GetWindow().Add(actor);
    auto& impl = Ui::GetImplementation(visual);
    impl.SetOnScene(actor);
    const auto texture = impl.GetRenderer().GetTextures().GetTexture(0);
    DALI_TEST_EQUALS(texture.GetWidth(), 160u, TEST_LOCATION);
    DALI_TEST_EQUALS(texture.GetHeight(), 80u, TEST_LOCATION);

    // Initial view size never replaces the stored desired-size properties.
    Property::Map disable;
    disable.Insert(Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE, false);
    impl.SetProperties(disable);
    DALI_TEST_EQUALS(ReportedLoadSize(visual), ImageDimensions(30, 20), TEST_LOCATION);
    impl.SetOffScene(actor);
    application.GetWindow().Remove(actor);

    map[Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE] = false;
    visual = factoryImpl.CreateVisual(map, Ui::Integration::VisualFactory::NONE, Vector2(160.4f, 79.6f));
    DALI_TEST_EQUALS(ReportedLoadSize(visual), ImageDimensions(30, 20), TEST_LOCATION);

    map[Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE] = true;
    visual = factoryImpl.CreateVisual(map, Ui::Integration::VisualFactory::NONE, Vector2::ZERO);
    DALI_TEST_EQUALS(ReportedLoadSize(visual), ImageDimensions(30, 20), TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliImageViewRebuildViewSizeDesiredDefaults(void)
{
  UiTestApplication application;
  auto factory = Ui::Integration::VisualFactory::Get();
  auto& factoryImpl = Ui::GetImplementation(factory);
  for(auto policy : {Image::LoadPolicy::ATTACHED, Image::LoadPolicy::IMMEDIATE})
  {
    for(bool loadWithViewSize : {false, true})
    {
      for(bool hasDesiredWidth : {false, true})
      {
        Property::Map map;
        map.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::IMAGE);
        map.Insert(Ui::Integration::ImageVisual::Property::URL, DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
        map.Insert(Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE, loadWithViewSize);
        map.Insert(Ui::Integration::ImageVisual::Property::LOAD_POLICY, static_cast<int>(policy));
        map.Insert(Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
        if(hasDesiredWidth)
        {
          // String keys must be preserved as well as numeric property keys.
          map.Insert("desiredWidth", 30);
        }
        const ImageDimensions desiredSize(hasDesiredWidth ? 30 : 0, 0);
        auto visual = factoryImpl.CreateVisual(map, Ui::Integration::VisualFactory::NONE, Vector2(160, 80));
        DALI_TEST_EQUALS(ReportedLoadSize(visual), loadWithViewSize ? ImageDimensions(160, 80) : desiredSize, TEST_LOCATION);

        Property::Map disable;
        disable.Insert(Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE, false);
        Ui::GetImplementation(visual).SetProperties(disable);
        DALI_TEST_EQUALS(ReportedLoadSize(visual), desiredSize, TEST_LOCATION);
        DALI_TEST_CHECK(!map.Find(Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT));
      }
    }
  }

  // The existing URL-and-size creation path still uses size as Desired Size.
  auto visual = factory.CreateVisual(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png", ImageDimensions(30, 20));
  DALI_TEST_EQUALS(ReportedLoadSize(visual), ImageDimensions(30, 20), TEST_LOCATION);
  END_TEST;
}
