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

#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-transform.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <ui-event-thread-callback.h>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
Dali::Ui::Internal::ViewDataImpl& DataOf(ImageView view)
{
  return Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(view));
}

// Distinct cache keys for the same fixture without growing paths linearly.
// Keep paths short enough for the Windows file loader as the matrix grows.
Dali::String MakeImageViewTestUrl(int id)
{
  std::string path(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/");
  while(id > 0)
  {
    path += (id & 1) ? "./" : "../resources/";
    id >>= 1;
  }
  path += "view-size.png";
  return Dali::String(path.c_str());
}

UiConfig CreatePlaceholderFailureConfig()
{
  UiConfig config = UiConfig::New();
  // Failure tests must not depend on installed theme images in DALI_IMAGE_DIR.
  for(auto type : {UiConfig::BrokenImageType::SMALL, UiConfig::BrokenImageType::NORMAL, UiConfig::BrokenImageType::LARGE})
  {
    config.SetBrokenImageUrl(type, DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  }
  return config;
}

// Keeps an unrelated visual pending until the test explicitly completes it.
class PendingImageViewTestVisual : public Ui::Internal::Visual::Base
{
public:
  static IntrusivePtr<PendingImageViewTestVisual> New(Ui::Internal::VisualFactoryCache& cache)
  {
    IntrusivePtr<PendingImageViewTestVisual> visual(new PendingImageViewTestVisual(cache));
    visual->Initialize();
    return visual;
  }

private:
  explicit PendingImageViewTestVisual(Ui::Internal::VisualFactoryCache& cache)
  : Ui::Internal::Visual::Base(cache, Ui::Integration::InternalVisualType::IMAGE)
  {
  }
  void OnInitialize() override
  {
  }
  void DoCreatePropertyMap(Property::Map&) const override
  {
  }
  void DoCreateInstancePropertyMap(Property::Map&) const override
  {
  }
  void DoSetProperties(const Property::Map&) override
  {
  }
  void OnSetTransform() override
  {
  }
  void DoSetOnScene(Actor&) override
  {
  }
};

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

  bool borderOnly      = false;
  auto borderOnlyValue = visualMap.Find(Ui::Integration::ImageVisual::Property::BORDER_ONLY);
  DALI_TEST_CHECK(borderOnlyValue);
  DALI_TEST_CHECK(borderOnlyValue->Get(borderOnly));
  DALI_TEST_EQUALS(borderOnly, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewRealNPatchRenderingModesP(void)
{
  UiTestApplication application;
  const char*       url = "../samples/image-view/res/button-up-1.9.png";

  for(bool borderOnly : {false, true})
  {
    ImageView view = ImageView::New();
    view.SetSynchronousLoading(true);
    view.SetRequestedWidth(160.0f);
    view.SetRequestedHeight(100.0f);
    view.SetNPatchBorderOnly(borderOnly);
    if(borderOnly)
    {
      view.SetNPatchBorder(Insets(2.0f, 2.0f, 2.0f, 2.0f));
    }
    view.SetResourceUrl(url);
    application.GetScene().Add(view);
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
    DALI_TEST_CHECK(view.GetRendererCount() > 0u);
    DALI_TEST_CHECK(view.GetRendererAt(0u).GetTextures().GetTexture(0u));

    const Vector3 naturalSize = view.GetNaturalSize();
    DALI_TEST_CHECK(naturalSize.width > 0.0f);
    DALI_TEST_CHECK(naturalSize.height > 0.0f);

    view.Reload();
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
    application.GetScene().Remove(view);
  }
  END_TEST;
}

namespace
{
struct ArrangeSvgPanel : ConnectionTracker
{
  ImageView    arrow;
  View         root;
  bool         delaySettlement{true};
  int          measures{0};
  int          finishes{0};
  int          finishesWhilePending{0};
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
    if(Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(root)).IsMeasureDirty())
    {
      ++finishesWhilePending;
    }
  }
};
} // namespace

// Every pass is reported while the producer keeps another pass pending; the
// LayoutFinished-driven fitting gives the SVG its size, so the resource becomes ready
// without layout ever settling. Event processing is driven explicitly, independently of
// whether a follow-up idle wake is requested.
int UtcDaliImageViewSvgReadyWhileLayoutPending(void)
{
  UiTestApplication application;
  ArrangeSvgPanel   panel;
  View              root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  root.SetMeasureCallback(MeasureCallback::New(&panel, &ArrangeSvgPanel::Measure));
  root.SetArrangeCallback(ArrangeCallback::New(&panel, &ArrangeSvgPanel::Arrange));
  panel.root         = root;
  int windowFinishes = 0;
  LayoutController::Get(application.GetWindow()).LayoutFinishedSignal().Connect(&application, [&](Window)
  { ++windowFinishes; });
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
  DALI_TEST_CHECK(panel.finishes > 0);
  DALI_TEST_EQUALS(panel.finishes, windowFinishes, TEST_LOCATION);
  DALI_TEST_EQUALS(panel.finishesWhilePending, panel.finishes, TEST_LOCATION);
  DALI_TEST_CHECK(panel.measures < 12);
  DALI_TEST_CHECK(Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(root)).IsMeasureDirty());
  DALI_TEST_EQUALS(panel.arrow.GetProperty<float>(Actor::Property::SIZE_WIDTH), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(panel.arrow.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(panel.arrow.GetRendererCount(), 2u, TEST_LOCATION);
  Texture texture = panel.arrow.GetRendererAt(1u).GetTextures().GetTexture(0u);

  const int finishesBefore = panel.finishes;
  const int measuresBefore = panel.measures;
  panel.delaySettlement    = false;
  Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(root)).InvalidateMeasure();
  for(int i = 0; i < 5 && panel.finishes == finishesBefore; ++i)
  {
    application.SendNotification();
    application.Render();
  }

  DALI_TEST_CHECK(panel.finishes > finishesBefore);
  DALI_TEST_CHECK(panel.measures > measuresBefore);
  DALI_TEST_EQUALS(panel.finishes, windowFinishes, TEST_LOCATION);
  DALI_TEST_CHECK(!Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(root)).IsMeasureDirty());
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
  ImageView         view = ImageView::New();
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
  ImageView         view = ImageView::New();
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
  // LayoutFinished fits the visual to the View's latest arranged target, not to the
  // delivered payload, so an older payload cannot rewind the view-size load: the view
  // is still arranged at 120x60 here.
  DataOf(view).EmitLayoutFinishedSignal(LayoutRect(0, 0, 100, 50));
  DALI_TEST_EQUALS(ReportedLoadSize(view), ImageDimensions(120, 60), TEST_LOCATION);

  // A manual Arrange() publishes the new target but fits nothing by itself; the reload
  // to the finished size happens when LayoutFinished is delivered for that target.
  view.Arrange(LayoutRect(0, 0, 100, 50));
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
  ImageView         view = ImageView::New();
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
  const float       originalScale = UiScaleManager::Get().GetScale();
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
  auto              factory     = Ui::Integration::VisualFactory::Get();
  auto&             factoryImpl = Ui::GetImplementation(factory);
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
    visual                                                                 = factoryImpl.CreateVisual(map, Ui::Integration::VisualFactory::NONE, Vector2(160.4f, 79.6f));
    DALI_TEST_EQUALS(ReportedLoadSize(visual), ImageDimensions(30, 20), TEST_LOCATION);

    map[Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE] = true;
    visual                                                                 = factoryImpl.CreateVisual(map, Ui::Integration::VisualFactory::NONE, Vector2::ZERO);
    DALI_TEST_EQUALS(ReportedLoadSize(visual), ImageDimensions(30, 20), TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliImageViewRebuildViewSizeDesiredDefaults(void)
{
  UiTestApplication application;
  auto              factory     = Ui::Integration::VisualFactory::Get();
  auto&             factoryImpl = Ui::GetImplementation(factory);
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
        auto                  visual = factoryImpl.CreateVisual(map, Ui::Integration::VisualFactory::NONE, Vector2(160, 80));
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

int UtcDaliImageViewPlaceholderEmptyUrlLifecycle(void)
{
  UiTestApplication  application;
  const Dali::String url(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  for(bool fixedSize : {false, true})
  {
    ImageView view = ImageView::New();
    view.SetSynchronousLoading(true);
    if(fixedSize)
    {
      view.SetRequestedWidth(160.0f);
      view.SetRequestedHeight(100.0f);
    }
    view.SetPlaceholderUrl(url);
    application.GetScene().Add(view);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
    DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::IMAGE));
    DALI_TEST_CHECK(view.GetNaturalSize().width > 0.0f);
    view.SetResourceUrl(url);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
    DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
    view.SetResourceUrl(ImageUrl());
    DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String(), TEST_LOCATION);
    DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::IMAGE));
    DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
    view.SetResourceUrl(Dali::String());
    DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
    view.SetPlaceholderUrl(Dali::String());
    DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
    DALI_TEST_EQUALS(view.GetNaturalSize(), Vector3::ZERO, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliImageViewPlaceholderDoesNotCoverFailure(void)
{
  UiTestApplication application(CreatePlaceholderFailureConfig());
  ImageView         view = ImageView::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(100.0f);
  view.SetSynchronousLoading(true);
  view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  application.GetScene().Add(view);
  view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/missing-placeholder-test.png");
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::FAILED, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  view.SetPlaceholderUrl("another-placeholder.png");
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  view.SetResourceUrl(Dali::String());
  DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::IMAGE));
  END_TEST;
}

int UtcDaliImageViewPlaceholderReplacementKeepsPreviousImage(void)
{
  UiTestApplication application;
  ImageView         view = ImageView::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(100.0f);
  view.SetSynchronousLoading(true);
  view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  for(int i = 0; i < 8 && DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE); ++i)
  {
    Test::WaitForEventThreadTrigger(1, 1);
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  view.SetSynchronousLoading(false);
  view.Measure(160.0f, 100.0f);
  application.SendNotification();
  application.Render();
  const auto previousRenderer = view.GetRendererAt(0u);
  view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/missing-replacement-test.png");
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  bool previousStillVisible = false;
  for(uint32_t i = 0; i < view.GetRendererCount(); ++i)
  {
    previousStillVisible |= view.GetRendererAt(i) == previousRenderer;
  }
  DALI_TEST_CHECK(previousStillVisible);
  view.SetResourceUrl(Dali::String());
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::IMAGE));
  DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  END_TEST;
}

int UtcDaliImageViewPlaceholderAsyncFailureRecovery(void)
{
  UiTestApplication application(CreatePlaceholderFailureConfig());
  ImageView         view = ImageView::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/missing-async-placeholder-test.png");
  for(int i = 0; i < 8 && (view.GetLoadingStatus() != Ui::Visual::ResourceStatus::FAILED ||
                           DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
      ++i)
  {
    Test::WaitForEventThreadTrigger(1, 1);
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::FAILED, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  view.SetResourceUrl(Dali::String());
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::IMAGE));
  END_TEST;
}

int UtcDaliImageViewPlaceholderCompletionEmitsResourceReady(void)
{
  UiTestApplication application;
  for(bool attachBeforeUrl : {true, false})
  {
    for(bool synchronous : {true, false})
    {
      const Dali::String mainUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
      ImageView          cachedImage;
      if(!synchronous)
      {
        cachedImage = ImageView::New();
        cachedImage.SetRequestedWidth(80.0f);
        cachedImage.SetRequestedHeight(40.0f);
        cachedImage.SetSynchronousLoading(true);
        cachedImage.SetResourceUrl(mainUrl);
        application.GetScene().Add(cachedImage);
        DALI_TEST_EQUALS(cachedImage.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
      }

      ImageView view = ImageView::New();
      view.SetRequestedWidth(160.0f);
      view.SetRequestedHeight(100.0f);
      view.SetSynchronousLoading(synchronous);
      view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/initial-layout.svg");
      int readyCount = 0;
      view.ResourceReadySignal().Connect(&application, [&](View)
      { ++readyCount; });
      if(attachBeforeUrl)
      {
        application.GetScene().Add(view);
      }
      DALI_TEST_EQUALS(DataOf(view).GetVisualResourceStatus(ImageView::Property::PLACEHOLDER_IMAGE),
                       Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
      view.SetResourceUrl(mainUrl);
      if(!attachBeforeUrl)
      {
        application.GetScene().Add(view);
      }
      DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
      DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
      DALI_TEST_EQUALS(readyCount, 1, TEST_LOCATION);
      Test::WaitForEventThreadTrigger(1, 1);
      application.SendNotification();
      application.Render();
      DALI_TEST_EQUALS(readyCount, 1, TEST_LOCATION);
      application.GetScene().Remove(view);
      if(cachedImage)
      {
        application.GetScene().Remove(cachedImage);
      }
    }
  }
  END_TEST;
}

int UtcDaliImageViewPlaceholderChainedReplacementKeepsPreviousImage(void)
{
  UiTestApplication application;
  ImageView         view = ImageView::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(100.0f);
  view.SetSynchronousLoading(true);
  view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/initial-layout.svg");
  view.SetSynchronousLoading(false);
  view.Measure(160.0f, 100.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
  const auto previousRenderer = view.GetRendererAt(0u);

  view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/missing-chain-b.png");
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
  view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/missing-chain-c.png");
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  bool previousStillVisible = false;
  for(uint32_t i = 0; i < view.GetRendererCount(); ++i)
  {
    previousStillVisible |= view.GetRendererAt(i) == previousRenderer;
  }
  DALI_TEST_CHECK(previousStillVisible);
  view.SetResourceUrl(Dali::String());
  DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  END_TEST;
}

int UtcDaliImageViewPlaceholderFailureWithPendingBackground(void)
{
  UiTestApplication application(CreatePlaceholderFailureConfig());
  ImageView         view = ImageView::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  application.GetScene().Add(view);
  for(int i = 0; i < 8 && DataOf(view).GetVisualResourceStatus(ImageView::Property::PLACEHOLDER_IMAGE) != Ui::Visual::ResourceStatus::READY; ++i)
  {
    Test::WaitForEventThreadTrigger(1, 1);
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(DataOf(view).GetVisualResourceStatus(ImageView::Property::PLACEHOLDER_IMAGE),
                   Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
  auto                          factory    = Ui::Integration::VisualFactory::Get();
  auto                          background = PendingImageViewTestVisual::New(Ui::GetImplementation(factory).GetFactoryCache());
  Ui::Integration::Visual::Base backgroundHandle(background.Get());
  DataOf(view).RegisterVisual(Ui::Integration::View::Property::BACKGROUND, backgroundHandle);
  int readyCount = 0;
  view.ResourceReadySignal().Connect(&application, [&](View)
  { ++readyCount; });
  view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/missing-pending-background.png");
  for(int i = 0; i < 8 && view.GetLoadingStatus() != Ui::Visual::ResourceStatus::FAILED; ++i)
  {
    Test::WaitForEventThreadTrigger(1, 1);
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::FAILED, TEST_LOCATION);
  DALI_TEST_EQUALS(background->GetResourceStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  DALI_TEST_EQUALS(readyCount, 0, TEST_LOCATION);
  background->ResourceReady(Ui::Visual::ResourceStatus::READY);
  DALI_TEST_EQUALS(readyCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewPlaceholderChainedInitialLoadsKeepPlaceholder(void)
{
  UiTestApplication application;
  ImageView         view = ImageView::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  application.GetScene().Add(view);
  for(const char* url : {"/missing-initial-a.png", "/missing-initial-b.png", "/missing-initial-c.png"})
  {
    view.SetResourceUrl(Dali::String(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR) + Dali::String(url));
    DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
    DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  }
  END_TEST;
}

// Preserve enabled registrations when a visual is also registered at another,
// disabled property index before the view connects to the scene.
int UtcDaliImageViewSharedVisualSceneConnection(void)
{
  UiTestApplication             application;
  ImageView                     view       = ImageView::New();
  auto                          factory    = Ui::Integration::VisualFactory::Get();
  auto                          testVisual = PendingImageViewTestVisual::New(Ui::GetImplementation(factory).GetFactoryCache());
  Ui::Integration::Visual::Base visual(testVisual.Get());
  constexpr Property::Index     disabledIndex = 10001;
  constexpr Property::Index     enabledIndex  = 10002;
  DataOf(view).RegisterVisual(disabledIndex, visual, false);
  DataOf(view).RegisterVisual(enabledIndex, visual, true);
  DALI_TEST_CHECK(!testVisual->IsOnScene());
  application.GetScene().Add(view);
  DALI_TEST_CHECK(testVisual->IsOnScene());
  END_TEST;
}

namespace
{
int CheckPlaceholderReadyCallbackChangesUrl(bool replaceUrl, bool onScene = false)
{
  UiTestApplication  application(CreatePlaceholderFailureConfig());
  const Dali::String mainUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  const Dali::String nextUrl(replaceUrl ? (onScene ? DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/./view-size.png" : DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/missing-ready-callback.png") : "");
  ImageView          cachedImage = ImageView::New();
  cachedImage.SetRequestedWidth(80.0f);
  cachedImage.SetRequestedHeight(40.0f);
  cachedImage.SetSynchronousLoading(true);
  cachedImage.SetResourceUrl(mainUrl);
  application.GetScene().Add(cachedImage);
  DALI_TEST_EQUALS(cachedImage.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);

  ImageView view = ImageView::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(100.0f);
  view.SetLoadPolicy(onScene ? Image::LoadPolicy::ATTACHED : Image::LoadPolicy::IMMEDIATE);
  view.SetSynchronousLoading(onScene);
  view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/initial-layout.svg");
  if(onScene)
  {
    application.GetScene().Add(view);
    DALI_TEST_EQUALS(DataOf(view).GetVisualResourceStatus(ImageView::Property::PLACEHOLDER_IMAGE),
                     Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
  }
  Renderer                      completedRenderer;
  int                           readyCount           = 0;
  bool                          requestReturned      = false;
  bool                          callbackBeforeReturn = false;
  Ui::Integration::Visual::Base completedVisual;
  Ui::Integration::Visual::Base replacementVisual;
  view.ResourceReadySignal().Connect(&application, [&](View)
  {
    ++readyCount;
    if(readyCount == 1)
    {
      callbackBeforeReturn = !requestReturned;
      completedVisual      = DataOf(view).GetVisual(ImageView::Property::IMAGE);
      if(onScene)
      {
        completedRenderer = Ui::GetImplementation(completedVisual).GetRenderer();
      }
      view.SetResourceUrl(nextUrl);
      replacementVisual = DataOf(view).GetVisual(ImageView::Property::IMAGE);
    }
  });
  view.SetResourceUrl(mainUrl);
  requestReturned = true;
  DALI_TEST_CHECK(callbackBeforeReturn);
  DALI_TEST_EQUALS(readyCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetResourceUrl(), nextUrl, TEST_LOCATION);
  DALI_TEST_CHECK(completedVisual);
  DALI_TEST_CHECK(completedVisual != replacementVisual);
  DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::IMAGE) == replacementVisual);
  DALI_TEST_EQUALS(view.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE), onScene, TEST_LOCATION);
  if(onScene)
  {
    DALI_TEST_CHECK(completedRenderer);
    for(uint32_t i = 0; i < view.GetRendererCount(); ++i)
    {
      DALI_TEST_CHECK(view.GetRendererAt(i) != completedRenderer);
    }
    view.SetPlaceholderUrl(Dali::String());
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(view.GetRendererCount(), replaceUrl ? 1u : 0u, TEST_LOCATION);
    for(uint32_t i = 0; i < view.GetRendererCount(); ++i)
    {
      DALI_TEST_CHECK(view.GetRendererAt(i) != completedRenderer);
    }
    END_TEST;
  }
  if(replaceUrl)
  {
    DALI_TEST_CHECK(replacementVisual);
    DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
    application.GetScene().Add(view);
    for(int i = 0; i < 8 && view.GetLoadingStatus() != Ui::Visual::ResourceStatus::FAILED; ++i)
    {
      Test::WaitForEventThreadTrigger(1, 1);
      application.SendNotification();
      application.Render();
    }
    DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::FAILED, TEST_LOCATION);
    DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
    DALI_TEST_EQUALS(readyCount, 2, TEST_LOCATION);
  }
  else
  {
    DALI_TEST_CHECK(!replacementVisual);
    DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
    application.GetScene().Add(view);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::IMAGE));
    DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  }
  END_TEST;
}
} // namespace

int UtcDaliImageViewPlaceholderReadyCallbackClearsUrl(void)
{
  return CheckPlaceholderReadyCallbackChangesUrl(false);
}

int UtcDaliImageViewPlaceholderReadyCallbackReplacesUrl(void)
{
  return CheckPlaceholderReadyCallbackChangesUrl(true);
}

int UtcDaliImageViewPlaceholderOnSceneReadyCallbackClearsUrl(void)
{
  return CheckPlaceholderReadyCallbackChangesUrl(false, true);
}

int UtcDaliImageViewPlaceholderOnSceneReadyCallbackReplacesUrl(void)
{
  return CheckPlaceholderReadyCallbackChangesUrl(true, true);
}

// Exercise completion callbacks while both loading state and scene ownership
// change. Renderer identity and final ownership matter as well as URL state.
int UtcDaliImageViewPlaceholderCompletionLifecycleMatrix(void)
{
  UiTestApplication application;
  enum Action
  {
    CLEAR,
    REPLACE_READY,
    REPLACE_PENDING,
    DETACH,
    RECONNECT
  };
  int caseNumber = 0;
  for(int loading = 0; loading < 3; ++loading) // synchronous, cached, asynchronous
  {
    for(auto policy : {Image::LoadPolicy::ATTACHED, Image::LoadPolicy::IMMEDIATE})
    {
      for(bool offscreen : {false, true})
      {
        for(bool placeholderReady : {false, true})
        {
          for(auto action : {CLEAR, REPLACE_READY, REPLACE_PENDING, DETACH, RECONNECT})
          {
            ++caseNumber;
            const std::string label = "Completion lifecycle case " + std::to_string(caseNumber) +
                                      ": loading=" + std::to_string(loading) + ", policy=" + std::to_string(static_cast<int>(policy)) + ", offscreen=" + std::to_string(offscreen) +
                                      ", placeholderReady=" + std::to_string(placeholderReady) + ", action=" + std::to_string(action);
            tet_infoline(label.c_str());
            const auto mainUrl = MakeImageViewTestUrl(caseNumber * 3);
            const auto nextUrl = MakeImageViewTestUrl(caseNumber * 3 + 1);
            ImageView  cachedImage;
            if(loading == 1)
            {
              cachedImage = ImageView::New();
              cachedImage.SetRequestedWidth(160.0f);
              cachedImage.SetRequestedHeight(100.0f);
              cachedImage.SetSynchronousLoading(true);
              cachedImage.SetResourceUrl(mainUrl);
              application.GetScene().Add(cachedImage);
              DALI_TEST_EQUALS(cachedImage.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
            }
            ImageView view = ImageView::New();
            view.SetRequestedWidth(160.0f);
            view.SetRequestedHeight(100.0f);
            view.SetLoadPolicy(policy);
            view.SetSynchronousLoading(loading == 0);
            view.SetOffscreenRenderingEnabled(offscreen);
            view.SetPlaceholderUrl(MakeImageViewTestUrl(caseNumber * 3 + 2));
            application.GetScene().Add(view);
            if(placeholderReady)
            {
              for(int i = 0; i < 8 && DataOf(view).GetVisualResourceStatus(ImageView::Property::PLACEHOLDER_IMAGE) != Ui::Visual::ResourceStatus::READY; ++i)
              {
                Test::WaitForEventThreadTrigger(1, 1);
                application.SendNotification();
                application.Render();
              }
            }
            DALI_TEST_EQUALS(DataOf(view).GetVisualResourceStatus(ImageView::Property::PLACEHOLDER_IMAGE),
                             placeholderReady ? Ui::Visual::ResourceStatus::READY : Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
            bool                          callbackRan = false;
            Ui::Integration::Visual::Base completedVisual;
            Renderer                      completedRenderer;
            ConnectionTracker             callbackTracker;
            view.ResourceReadySignal().Connect(&callbackTracker, [&](View)
            {
              auto image = DataOf(view).GetVisual(ImageView::Property::IMAGE);
              if(callbackRan || !image || !Ui::GetImplementation(image).IsResourceReady())
              {
                return;
              }
              callbackRan       = true;
              completedVisual   = image;
              completedRenderer = Ui::GetImplementation(completedVisual).GetRenderer();
              switch(action)
              {
                case CLEAR:
                  view.SetResourceUrl(Dali::String());
                  break;
                case REPLACE_READY:
                case REPLACE_PENDING:
                  view.SetSynchronousLoading(action == REPLACE_READY);
                  view.SetResourceUrl(nextUrl);
                  break;
                case DETACH:
                  view.Unparent();
                  break;
                case RECONNECT:
                  view.Unparent();
                  application.GetScene().Add(view);
                  break;
              }
            });
            view.SetResourceUrl(mainUrl);
            if(loading == 2)
            {
              DALI_TEST_CHECK(!callbackRan);
            }
            for(int i = 0; i < 8 && !callbackRan; ++i)
            {
              Test::WaitForEventThreadTrigger(1, 1);
              application.SendNotification();
              application.Render();
            }
            DALI_TEST_CHECK(callbackRan);
            DALI_TEST_CHECK(completedVisual);
            DALI_TEST_CHECK(completedRenderer);
            if(action == REPLACE_PENDING || action == RECONNECT)
            {
              for(int i = 0; i < 8 && view.GetLoadingStatus() != Ui::Visual::ResourceStatus::READY; ++i)
              {
                Test::WaitForEventThreadTrigger(1, 1);
                application.SendNotification();
                application.Render();
              }
              DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
            }
            if(action == CLEAR)
            {
              DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String(), TEST_LOCATION);
              DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::IMAGE));
              DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
            }
            if(action == REPLACE_READY || action == REPLACE_PENDING)
            {
              DALI_TEST_EQUALS(view.GetResourceUrl(), nextUrl, TEST_LOCATION);
              DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::IMAGE) != completedVisual);
              DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
            }
            DALI_TEST_EQUALS(view.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE), action != DETACH, TEST_LOCATION);
            DALI_TEST_EQUALS(Ui::GetImplementation(completedVisual).IsOnScene(), action == RECONNECT, TEST_LOCATION);
            view.SetPlaceholderUrl(Dali::String());
            view.SetOffscreenRenderingEnabled(false);
            application.SendNotification();
            application.Render();
            DALI_TEST_EQUALS(view.GetRendererCount(), action == CLEAR || action == DETACH ? 0u : 1u, TEST_LOCATION);
            DALI_TEST_EQUALS(view.GetCacheRendererCount(), 0u, TEST_LOCATION);
            if(action != RECONNECT)
            {
              for(uint32_t i = 0; i < view.GetRendererCount(); ++i)
              {
                DALI_TEST_CHECK(view.GetRendererAt(i) != completedRenderer);
              }
            }
            view.SetResourceUrl(Dali::String());
            view.Unparent();
            DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
            DALI_TEST_EQUALS(view.GetCacheRendererCount(), 0u, TEST_LOCATION);
            if(cachedImage)
            {
              application.GetScene().Remove(cachedImage);
            }
          }
        }
      }
    }
  }
  DALI_TEST_EQUALS(caseNumber, 120, TEST_LOCATION);
  END_TEST;
}

// A visual removed by its synchronous ready callback must not subsequently be
// inserted into an offscreen renderer cache by the returning connection call.
int UtcDaliImageViewPlaceholderReadyCallbackRemovesOffscreenVisual(void)
{
  UiTestApplication application;
  ImageView         view = ImageView::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(100.0f);
  view.SetOffscreenRenderingEnabled(true);
  application.GetScene().Add(view);
  const auto    initialCacheRendererCount = view.GetCacheRendererCount();
  Property::Map map;
  map.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  map.Insert(Ui::Integration::ImageVisual::Property::URL, DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  map.Insert(Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  map.Insert(Ui::Integration::ImageVisual::Property::RELEASE_POLICY, static_cast<int>(Image::ReleasePolicy::NEVER));
  auto                      visual      = Ui::Integration::VisualFactory::Get().CreateVisual(map);
  constexpr Property::Index visualIndex = 10003;
  bool                      removed     = false;
  view.ResourceReadySignal().Connect(&application, [&](View)
  {
    if(!removed)
    {
      removed = true;
      DataOf(view).UnregisterVisual(visualIndex);
    }
  });
  DataOf(view).RegisterVisual(visualIndex, visual, Ui::Integration::DepthIndex::FOREGROUND_EFFECT);
  DALI_TEST_CHECK(removed);
  DALI_TEST_CHECK(!Ui::GetImplementation(visual).IsOnScene());
  DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCacheRendererCount(), initialCacheRendererCount, TEST_LOCATION);
  DataOf(view).RegisterVisual(visualIndex, visual, Ui::Integration::DepthIndex::FOREGROUND_EFFECT);
  DALI_TEST_CHECK(Ui::GetImplementation(visual).IsOnScene());
  DALI_TEST_EQUALS(view.GetCacheRendererCount(), initialCacheRendererCount + 1u, TEST_LOCATION);
  DataOf(view).UnregisterVisual(visualIndex);
  DALI_TEST_CHECK(!Ui::GetImplementation(visual).IsOnScene());
  DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCacheRendererCount(), initialCacheRendererCount, TEST_LOCATION);
  END_TEST;
}

// SVG resolves during scene connection through a separate loading path. Keep
// its natural-size/rasterization contract while honouring reentrant removal.
int UtcDaliImageViewPlaceholderSvgReadyCallbackClearsUrl(void)
{
  UiTestApplication application;
  ImageView         view = ImageView::New();
  view.SetRequestedWidth(64.0f);
  view.SetRequestedHeight(64.0f);
  view.SetDesiredWidth(64);
  view.SetDesiredHeight(64);
  view.SetSynchronousLoading(true);
  view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  application.GetScene().Add(view);
  bool                          cleared = false;
  Ui::Integration::Visual::Base completedVisual;
  view.ResourceReadySignal().Connect(&application, [&](View)
  {
    auto image = DataOf(view).GetVisual(ImageView::Property::IMAGE);
    if(!cleared && image && Ui::GetImplementation(image).IsResourceReady())
    {
      cleared         = true;
      completedVisual = image;
      view.SetResourceUrl(Dali::String());
    }
  });
  view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/initial-layout.svg");
  for(int i = 0; i < 8 && !cleared; ++i)
  {
    Test::WaitForEventThreadTrigger(1, 1);
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(cleared);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String(), TEST_LOCATION);
  DALI_TEST_CHECK(!Ui::GetImplementation(completedVisual).IsOnScene());
  DALI_TEST_CHECK(!DataOf(view).GetVisual(ImageView::Property::IMAGE));
  DALI_TEST_CHECK(DataOf(view).GetVisual(ImageView::Property::PLACEHOLDER_IMAGE));
  view.SetPlaceholderUrl(Dali::String());
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewPlaceholderCachedImmediateReplacement(void)
{
  UiTestApplication  application;
  const Dali::String firstUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  const Dali::String secondUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/./view-size.png");
  ImageView          cached = ImageView::New();
  cached.SetRequestedWidth(160.0f);
  cached.SetRequestedHeight(100.0f);
  cached.SetSynchronousLoading(true);
  cached.SetResourceUrl(secondUrl);
  application.GetScene().Add(cached);
  DALI_TEST_EQUALS(cached.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
  ImageView view = ImageView::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(100.0f);
  view.SetLoadPolicy(Image::LoadPolicy::IMMEDIATE);
  view.SetSynchronousLoading(true);
  view.SetResourceUrl(firstUrl);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  const auto oldRenderer = view.GetRendererAt(0u);
  view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/initial-layout.svg");
  view.SetSynchronousLoading(false);
  int readyCount = 0;
  view.ResourceReadySignal().Connect(&application, [&](View)
  { ++readyCount; });
  view.SetResourceUrl(secondUrl);
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(view.GetRendererAt(0u) != oldRenderer);
  DALI_TEST_CHECK(!DataOf(view).GetPreviousVisual(ImageView::Property::IMAGE));
  DALI_TEST_EQUALS(readyCount, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewPlaceholderDetachWithFollowingBackground(void)
{
  UiTestApplication application;
  ImageView         view = ImageView::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(100.0f);
  view.SetSynchronousLoading(true);
  view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/./view-size.png");
  view.SetResourceUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/view-size.png");
  view.SetBackgroundColor(UiColor(0xFF0000));
  auto background = DataOf(view).GetVisual(Ui::Integration::View::Property::BACKGROUND);
  bool detached   = false;
  view.ResourceReadySignal().Connect(&application, [&](View)
  {
    if(!detached)
    {
      detached = true;
      view.Unparent();
    }
  });
  application.GetScene().Add(view);
  for(int i = 0; i < 8 && !detached; ++i)
  {
    Test::WaitForEventThreadTrigger(1, 1);
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(detached);
  DALI_TEST_EQUALS(view.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE), false, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!Ui::GetImplementation(background).IsOnScene());
  END_TEST;
}

// Multiple ready visuals can publish completion at different points in the
// connection snapshot. Cover both registration orders and reentrant ownership.
int UtcDaliImageViewPlaceholderMultiVisualSceneCallbacks(void)
{
  UiTestApplication application;
  int               caseNumber = 0;
  for(auto policy : {Image::LoadPolicy::ATTACHED, Image::LoadPolicy::IMMEDIATE})
  {
    for(bool cached : {false, true})
    {
      for(bool backgroundFirst : {false, true})
      {
        for(bool offscreen : {false, true})
        {
          for(bool reconnect : {false, true})
          {
            for(bool usePlaceholder : {false, true})
            {
              ++caseNumber;
              tet_infoline(("Multi-visual scene case " + std::to_string(caseNumber)).c_str());
              const auto url = MakeImageViewTestUrl(caseNumber + 400);
              ImageView  cacheOwner;
              if(cached)
              {
                cacheOwner = ImageView::New();
                cacheOwner.SetRequestedWidth(160.0f);
                cacheOwner.SetRequestedHeight(100.0f);
                cacheOwner.SetSynchronousLoading(true);
                cacheOwner.SetResourceUrl(url);
                application.GetScene().Add(cacheOwner);
                DALI_TEST_EQUALS(cacheOwner.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
              }
              ImageView view = ImageView::New();
              view.SetRequestedWidth(160.0f);
              view.SetRequestedHeight(100.0f);
              view.SetLoadPolicy(policy);
              view.SetSynchronousLoading(!cached);
              view.SetOffscreenRenderingEnabled(offscreen);
              if(usePlaceholder)
              {
                view.SetPlaceholderUrl(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/initial-layout.svg");
              }
              if(backgroundFirst)
              {
                view.SetBackgroundColor(UiColor(0xFF0000));
              }
              bool              callbackRan = false;
              ConnectionTracker tracker;
              view.ResourceReadySignal().Connect(&tracker, [&](View)
              {
                auto image = DataOf(view).GetVisual(ImageView::Property::IMAGE);
                if(!callbackRan && image && Ui::GetImplementation(image).IsResourceReady())
                {
                  callbackRan = true;
                  view.Unparent();
                  if(reconnect)
                  {
                    application.GetScene().Add(view);
                  }
                }
              });
              view.SetResourceUrl(url);
              if(!backgroundFirst)
              {
                view.SetBackgroundColor(UiColor(0xFF0000));
              }
              auto background = DataOf(view).GetVisual(Ui::Integration::View::Property::BACKGROUND);
              if(!callbackRan)
              {
                application.GetScene().Add(view);
              }
              for(int i = 0; i < 8 && !callbackRan; ++i)
              {
                Test::WaitForEventThreadTrigger(1, 1);
                application.SendNotification();
                application.Render();
              }
              DALI_TEST_CHECK(callbackRan);
              DALI_TEST_EQUALS(view.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE), reconnect, TEST_LOCATION);
              DALI_TEST_EQUALS(Ui::GetImplementation(background).IsOnScene(), reconnect, TEST_LOCATION);
              auto image = DataOf(view).GetVisual(ImageView::Property::IMAGE);
              DALI_TEST_CHECK(image);
              DALI_TEST_EQUALS(Ui::GetImplementation(image).IsOnScene(), reconnect, TEST_LOCATION);
              view.SetOffscreenRenderingEnabled(false);
              application.SendNotification();
              application.Render();
              DALI_TEST_EQUALS(view.GetRendererCount(), reconnect ? 2u : 0u, TEST_LOCATION);
              DALI_TEST_EQUALS(view.GetCacheRendererCount(), 0u, TEST_LOCATION);
              view.SetPlaceholderUrl(Dali::String());
              view.SetResourceUrl(Dali::String());
              view.Unparent();
              DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
              DALI_TEST_EQUALS(view.GetCacheRendererCount(), 0u, TEST_LOCATION);
              if(cacheOwner)
              {
                application.GetScene().Remove(cacheOwner);
              }
            }
          }
        }
      }
    }
  }
  DALI_TEST_EQUALS(caseNumber, 64, TEST_LOCATION);
  END_TEST;
}
