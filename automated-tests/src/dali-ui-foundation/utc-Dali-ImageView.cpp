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

#include <dali-test-suite-utils.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/image-view/image-view.h>
#include <dali-ui-foundation/public-api/image-view/image-view-types.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_imageview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_imageview_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Construction

int UtcDaliImageViewConstructorP(void)
{
  TestApplication application;
  ImageView view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliImageViewNewP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliImageViewNewWithUrlP(void)
{
  TestApplication application;
  ImageView view = ImageView::New("test.jpg");
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("test.jpg"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewCopyConstructorP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  ImageView copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliImageViewMoveConstructorP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  ImageView moved(std::move(view));
  DALI_TEST_CHECK(moved);
  END_TEST;
}

int UtcDaliImageViewAssignmentOperatorP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  ImageView copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliImageViewMoveAssignmentOperatorP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  ImageView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  END_TEST;
}

// DownCast

int UtcDaliImageViewDownCastP(void)
{
  TestApplication application;
  ImageView view    = ImageView::New();
  View      baseView = view;
  ImageView downcast = ImageView::DownCast(baseView);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliImageViewDownCastN(void)
{
  TestApplication application;
  View      baseView = View::New();
  ImageView downcast  = ImageView::DownCast(baseView);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// SetResourceUrl / GetResourceUrl

int UtcDaliImageViewSetGetImageP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  view.SetResourceUrl("image.jpg");
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("image.jpg"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetImageNoChangeP(void)
{
  TestApplication application;
  ImageView view = ImageView::New("image.jpg");
  view.SetResourceUrl("image.jpg");
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("image.jpg"), TEST_LOCATION);
  END_TEST;
}

// PlaceholderImage

int UtcDaliImageViewSetGetPlaceholderUrlP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  view.SetPlaceholderUrl("placeholder.png");
  DALI_TEST_EQUALS(view.GetPlaceholderUrl(), Dali::String("placeholder.png"), TEST_LOCATION);
  END_TEST;
}

// FittingMode

int UtcDaliImageViewSetGetFittingModeP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetFittingMode(), Ui::FittingMode::FIT_KEEP_ASPECT_RATIO, TEST_LOCATION);

  view.SetFittingMode(Ui::FittingMode::FILL);
  DALI_TEST_EQUALS(view.GetFittingMode(), Ui::FittingMode::FILL, TEST_LOCATION);

  view.SetFittingMode(Ui::FittingMode::CENTER);
  DALI_TEST_EQUALS(view.GetFittingMode(), Ui::FittingMode::CENTER, TEST_LOCATION);
  END_TEST;
}

// DesiredSize

int UtcDaliImageViewSetGetDesiredSizeP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  view.SetDesiredSize(ImageDimensions(100, 200));
  DALI_TEST_EQUALS(view.GetDesiredSize().GetWidth(), static_cast<uint16_t>(100), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetDesiredSize().GetHeight(), static_cast<uint16_t>(200), TEST_LOCATION);
  END_TEST;
}

// ImageColor

int UtcDaliImageViewSetGetImageColorP(void)
{
  TestApplication application;
  ImageView view  = ImageView::New();
  UiColor   color(1.0f, 0.5f, 0.5f, 1.0f);
  view.SetImageColor(color);
  DALI_TEST_EQUALS(view.GetImageColor().Resolve(), color.Resolve(), TEST_LOCATION);
  END_TEST;
}

// PixelArea

int UtcDaliImageViewSetGetPixelAreaP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  Vector4   area = Vector4(0.1f, 0.1f, 0.8f, 0.8f);
  view.SetPixelArea(area);
  DALI_TEST_EQUALS(view.GetPixelArea(), area, TEST_LOCATION);
  END_TEST;
}

// PreMultipliedAlpha

int UtcDaliImageViewSetGetPreMultipliedAlphaP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetPreMultipliedAlpha(), false, TEST_LOCATION);
  view.SetPreMultipliedAlpha(true);
  DALI_TEST_EQUALS(view.GetPreMultipliedAlpha(), true, TEST_LOCATION);
  END_TEST;
}

// SynchronousLoading

int UtcDaliImageViewSetGetSynchronousLoadingP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetSynchronousLoading(), false, TEST_LOCATION);
  view.SetSynchronousLoading(true);
  DALI_TEST_EQUALS(view.GetSynchronousLoading(), true, TEST_LOCATION);
  END_TEST;
}

// Border (N-Patch)

int UtcDaliImageViewSetGetBorderP(void)
{
  TestApplication application;
  ImageView view   = ImageView::New();
  Vector4   border = Vector4(10.0f, 10.0f, 10.0f, 10.0f);
  view.SetBorder(border);
  DALI_TEST_EQUALS(view.GetBorder(), border, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetGetBorderOnlyP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetBorderOnly(), false, TEST_LOCATION);
  view.SetBorderOnly(true);
  DALI_TEST_EQUALS(view.GetBorderOnly(), true, TEST_LOCATION);
  END_TEST;
}

// Reload

int UtcDaliImageViewReloadP(void)
{
  TestApplication application;
  ImageView view = ImageView::New("image.jpg");
  // Reload on a view with no visual (before scene add) should not crash
  view.Reload();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliImageViewReloadNoImageP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  // Reload without setting an image should not crash
  view.Reload();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// SamplingMode

int UtcDaliImageViewSetGetSamplingModeP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetSamplingMode(), Ui::SamplingMode::BOX_THEN_LINEAR, TEST_LOCATION);

  view.SetSamplingMode(Ui::SamplingMode::NEAREST);
  DALI_TEST_EQUALS(view.GetSamplingMode(), Ui::SamplingMode::NEAREST, TEST_LOCATION);

  view.SetSamplingMode(Ui::SamplingMode::LINEAR);
  DALI_TEST_EQUALS(view.GetSamplingMode(), Ui::SamplingMode::LINEAR, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetSamplingModeNoChangeP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  view.SetSamplingMode(Ui::SamplingMode::NEAREST);
  view.SetSamplingMode(Ui::SamplingMode::NEAREST); // same value — should not dirty
  DALI_TEST_EQUALS(view.GetSamplingMode(), Ui::SamplingMode::NEAREST, TEST_LOCATION);
  END_TEST;
}

// ReleasePolicy

int UtcDaliImageViewSetGetReleasePolicyP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::ReleasePolicy::DETACHED, TEST_LOCATION);

  view.SetReleasePolicy(Ui::ReleasePolicy::DESTROYED);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::ReleasePolicy::DESTROYED, TEST_LOCATION);

  view.SetReleasePolicy(Ui::ReleasePolicy::NEVER);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::ReleasePolicy::NEVER, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetReleasePolicyNoChangeP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  view.SetReleasePolicy(Ui::ReleasePolicy::DESTROYED);
  view.SetReleasePolicy(Ui::ReleasePolicy::DESTROYED); // same value — should not dirty
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::ReleasePolicy::DESTROYED, TEST_LOCATION);
  END_TEST;
}

// Property set/get via Property::Index

int UtcDaliImageViewPropertySamplingModeP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();

  view.SetProperty(Dali::Property::Index(Dali::PROPERTY_REGISTRATION_START_INDEX + 2), // SAMPLING_MODE
                   static_cast<int>(Ui::SamplingMode::NO_FILTER));
  Dali::Property::Value value = view.GetProperty(Dali::Property::Index(Dali::PROPERTY_REGISTRATION_START_INDEX + 2));
  int mode = 0;
  DALI_TEST_CHECK(value.Get(mode));
  DALI_TEST_EQUALS(static_cast<Ui::SamplingMode::Type>(mode), Ui::SamplingMode::NO_FILTER, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewPropertyReleasePolicyP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();

  view.SetProperty(Dali::Property::Index(Dali::PROPERTY_REGISTRATION_START_INDEX + 12), // RELEASE_POLICY
                   static_cast<int>(Ui::ReleasePolicy::NEVER));
  Dali::Property::Value value = view.GetProperty(Dali::Property::Index(Dali::PROPERTY_REGISTRATION_START_INDEX + 12));
  int policy = 0;
  DALI_TEST_CHECK(value.Get(policy));
  DALI_TEST_EQUALS(static_cast<Ui::ReleasePolicy::Type>(policy), Ui::ReleasePolicy::NEVER, TEST_LOCATION);
  END_TEST;
}

// AlphaMaskUrl / CropToMask / MaskingMode

int UtcDaliImageViewSetGetAlphaMaskUrlP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  view.SetAlphaMaskUrl("mask.png");
  DALI_TEST_EQUALS(view.GetAlphaMaskUrl(), Dali::String("mask.png"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetGetCropToMaskP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetCropToMask(), false, TEST_LOCATION);
  view.SetCropToMask(true);
  DALI_TEST_EQUALS(view.GetCropToMask(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetGetMaskingModeP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  view.SetMaskingMode(Ui::MaskingType::MASKING_ON_LOADING);
  DALI_TEST_EQUALS(view.GetMaskingMode(), Ui::MaskingType::MASKING_ON_LOADING, TEST_LOCATION);
  view.SetMaskingMode(Ui::MaskingType::MASKING_ON_RENDERING);
  DALI_TEST_EQUALS(view.GetMaskingMode(), Ui::MaskingType::MASKING_ON_RENDERING, TEST_LOCATION);
  END_TEST;
}

// SynchronousSizing

int UtcDaliImageViewSetGetSynchronousSizingP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetSynchronousSizing(), false, TEST_LOCATION);
  view.SetSynchronousSizing(true);
  DALI_TEST_EQUALS(view.GetSynchronousSizing(), true, TEST_LOCATION);
  END_TEST;
}

// FastTrackUploading

int UtcDaliImageViewSetGetFastTrackUploadingP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetFastTrackUploading(), false, TEST_LOCATION);
  view.SetFastTrackUploading(true);
  DALI_TEST_EQUALS(view.GetFastTrackUploading(), true, TEST_LOCATION);
  END_TEST;
}

// OrientationCorrection

int UtcDaliImageViewSetGetOrientationCorrectionP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetOrientationCorrection(), true, TEST_LOCATION);
  view.SetOrientationCorrection(false);
  DALI_TEST_EQUALS(view.GetOrientationCorrection(), false, TEST_LOCATION);
  END_TEST;
}

// GetLoadingStatus / Signals

int UtcDaliImageViewGetLoadingStatusP(void)
{
  TestApplication application;
  ImageView view = ImageView::New("image.jpg");
  Ui::Visual::ResourceStatus status = view.GetLoadingStatus();
  DALI_TEST_CHECK(status == Ui::Visual::ResourceStatus::PREPARING ||
                  status == Ui::Visual::ResourceStatus::READY ||
                  status == Ui::Visual::ResourceStatus::FAILED);
  END_TEST;
}

int UtcDaliImageViewSignalsP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  // ResourceReadySignal is inherited from View — just call, don't store
  view.ResourceReadySignal();
  DALI_TEST_CHECK(true);
  END_TEST;
}

// Fluent chaining

int UtcDaliImageViewChainingP(void)
{
  TestApplication application;
  ImageView view = ImageView::New()
                     .SetResourceUrl("image.jpg")
                     .SetFittingMode(Ui::FittingMode::FILL)
                     .SetImageColor(UiColor(1.0f, 1.0f, 1.0f, 0.5f));
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("image.jpg"), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetFittingMode(), Ui::FittingMode::FILL, TEST_LOCATION);
  END_TEST;
}

// FitSizeToImage

int UtcDaliImageViewSetIsFitSizeToImageP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.IsFitSizeToImage(), false, TEST_LOCATION); // default: disabled

  view.SetFitSizeToImage(true);
  DALI_TEST_EQUALS(view.IsFitSizeToImage(), true, TEST_LOCATION);

  view.SetFitSizeToImage(false);
  DALI_TEST_EQUALS(view.IsFitSizeToImage(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetFitSizeToImageNoChangeP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();
  view.SetFitSizeToImage(true);
  view.SetFitSizeToImage(true); // same value — no update
  DALI_TEST_EQUALS(view.IsFitSizeToImage(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewPropertyFitSizeToImageP(void)
{
  TestApplication application;
  ImageView view = ImageView::New();

  // ADJUST_VIEW_SIZE comes after BORDER_ONLY in the property enum
  const Dali::Property::Index adjustViewSizeIndex = Dali::PROPERTY_REGISTRATION_START_INDEX + 18; // ADJUST_VIEW_SIZE
  view.SetProperty(adjustViewSizeIndex, true);
  Dali::Property::Value value = view.GetProperty(adjustViewSizeIndex);
  bool result                 = false;
  DALI_TEST_CHECK(value.Get(result));
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);
  END_TEST;
}
