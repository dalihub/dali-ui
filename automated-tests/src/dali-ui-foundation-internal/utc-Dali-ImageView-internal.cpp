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

#include <dali-ui-foundation/integration-api/visuals/visual-transform.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
Dali::Ui::Internal::ViewDataImpl& DataOf(ImageView view)
{
  return Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(view));
}
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

  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.Measure(500.0f, 500.0f);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(viewData.IsMeasureCacheValid());
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  view.SetResourceUrl("image.png");

  // Both measured axes are independent of the image's natural size. The visual is
  // rebuilt immediately, without retracting the still-correct measure cache.
  DALI_TEST_CHECK(viewData.GetVisual(ImageView::Property::IMAGE));
  DALI_TEST_CHECK(viewData.IsMeasureCacheValid());
  DALI_TEST_CHECK(!viewData.IsMeasureDirty());

  END_TEST;
}

int UtcDaliImageViewZeroRequestedSizeSetUrlInvalidatesMeasure(void)
{
  UiTestApplication application;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(0.0f);
  view.SetRequestedHeight(100.0f);
  view.Measure(500.0f, 500.0f);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(viewData.IsMeasureCacheValid());

  view.SetResourceUrl("image.png");

  // Preserve ImageView's existing strictly-positive fixed-size rule: zero does
  // not enter the SetUrl fast path.
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));
  DALI_TEST_CHECK(!viewData.IsMeasureCacheValid());
  DALI_TEST_CHECK(viewData.IsMeasureDirty());

  END_TEST;
}

int UtcDaliImageViewMatchParentSetUrlInvalidatesMeasure(void)
{
  UiTestApplication application;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(100.0f);
  view.Measure(500.0f, 500.0f);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(viewData.IsMeasureCacheValid());

  view.SetResourceUrl("image.png");

  // Keep MATCH_PARENT out of the explicit-size optimization. Its final size is
  // supplied by the parent constraint, so preserve the existing deferred path.
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));
  DALI_TEST_CHECK(!viewData.IsMeasureCacheValid());
  DALI_TEST_CHECK(viewData.IsMeasureDirty());

  END_TEST;
}

int UtcDaliImageViewWrapContentSetUrlInvalidatesMeasure(void)
{
  UiTestApplication application;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(WRAP_CONTENT);
  view.Measure(500.0f, 500.0f);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(viewData.IsMeasureCacheValid());
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  view.SetResourceUrl("image.png");

  // A WRAP_CONTENT axis can change from the image aspect ratio, so this path keeps
  // the existing deferred visual rebuild and measure invalidation.
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));
  DALI_TEST_CHECK(!viewData.IsMeasureCacheValid());
  DALI_TEST_CHECK(viewData.IsMeasureDirty());

  END_TEST;
}

int UtcDaliImageViewFixedSizeSetUrlPreservesLayoutFinishedFitting(void)
{
  UiTestApplication application;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(200.0f);
  view.SetDesiredWidth(100);
  view.SetDesiredHeight(50);
  view.SetFittingMode(Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
  view.Measure(500.0f, 500.0f);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(viewData.IsMeasureCacheValid());

  view.SetResourceUrl("image.png");

  auto visual = viewData.GetVisual(ImageView::Property::IMAGE);
  DALI_TEST_CHECK(visual);
  DALI_TEST_CHECK(viewData.IsMeasureCacheValid());

  // The fast path does not request a layout itself. Fitting-required visuals must
  // still be connected to the View's LayoutFinished path so the final bounds can be
  // applied after the common resource-ready invalidation schedules layout.
  viewData.EmitLayoutFinishedSignal(LayoutRect(0.0f, 0.0f, 200.0f, 200.0f));

  Property::Map visualMap;
  visual.CreatePropertyMap(visualMap);

  Property::Map transform;
  DALI_TEST_CHECK(visualMap.Find(Ui::VisualBasePropertyIndex::TRANSFORM)->Get(transform));

  Vector2 fittedSize;
  Vector2 fittedOffset;
  DALI_TEST_CHECK(transform.Find(Ui::Visual::Transform::Property::SIZE)->Get(fittedSize));
  DALI_TEST_CHECK(transform.Find(Ui::Visual::Transform::Property::OFFSET)->Get(fittedOffset));
  DALI_TEST_EQUALS(fittedSize, Vector2(200.0f, 100.0f), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(fittedOffset, Vector2(0.0f, 50.0f), 0.01f, TEST_LOCATION);

  END_TEST;
}
