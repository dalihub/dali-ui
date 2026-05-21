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

#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/image-view.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>

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
  UiTestApplication application;
  ImageView view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliImageViewNewP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliImageViewNewWithUrlP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New("test.jpg");
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("test.jpg"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewCopyConstructorP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  ImageView copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliImageViewMoveConstructorP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  ImageView moved(std::move(view));
  DALI_TEST_CHECK(moved);
  END_TEST;
}

int UtcDaliImageViewAssignmentOperatorP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  ImageView copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliImageViewMoveAssignmentOperatorP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  ImageView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  END_TEST;
}

// DownCast

int UtcDaliImageViewDownCastP(void)
{
  UiTestApplication application;
  ImageView view    = ImageView::New();
  View      baseView = view;
  ImageView downcast = ImageView::DownCast(baseView);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliImageViewDownCastN(void)
{
  UiTestApplication application;
  View      baseView = View::New();
  ImageView downcast  = ImageView::DownCast(baseView);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// SetResourceUrl / GetResourceUrl

int UtcDaliImageViewSetGetImageP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  view.SetResourceUrl("image.jpg");
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("image.jpg"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetImageNoChangeP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New("image.jpg");
  view.SetResourceUrl("image.jpg");
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("image.jpg"), TEST_LOCATION);
  END_TEST;
}

// PlaceholderImage

int UtcDaliImageViewSetGetPlaceholderUrlP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  view.SetPlaceholderUrl("placeholder.png");
  DALI_TEST_EQUALS(view.GetPlaceholderUrl(), Dali::String("placeholder.png"), TEST_LOCATION);
  END_TEST;
}

// FittingMode

int UtcDaliImageViewSetGetFittingModeP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetFittingMode(), Ui::Image::FittingMode::FILL, TEST_LOCATION);

  view.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
  DALI_TEST_EQUALS(view.GetFittingMode(), Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO, TEST_LOCATION);

  view.SetFittingMode(Ui::Image::FittingMode::CENTER);
  DALI_TEST_EQUALS(view.GetFittingMode(), Ui::Image::FittingMode::CENTER, TEST_LOCATION);
  END_TEST;
}

// DesiredWidth / DesiredHeight

int UtcDaliImageViewSetGetDesiredSizeP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  view.SetDesiredWidth(100);
  view.SetDesiredHeight(200);
  DALI_TEST_EQUALS(view.GetDesiredWidth(), 100, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetDesiredHeight(), 200, TEST_LOCATION);
  END_TEST;
}

// ImageColor

int UtcDaliImageViewSetGetImageColorP(void)
{
  UiTestApplication application;
  ImageView view  = ImageView::New();
  UiColor   color(1.0f, 0.5f, 0.5f, 1.0f);
  view.SetImageColor(color);
  DALI_TEST_EQUALS(view.GetImageColor().GetRgba(), color.GetRgba(), TEST_LOCATION);
  END_TEST;
}

// PixelArea

int UtcDaliImageViewSetGetPixelAreaP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  Vector4   area = Vector4(0.1f, 0.1f, 0.8f, 0.8f);
  view.SetPixelArea(area);
  DALI_TEST_EQUALS(view.GetPixelArea(), area, TEST_LOCATION);
  END_TEST;
}

// PreMultipliedAlpha

int UtcDaliImageViewSetGetPreMultipliedAlphaP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  // Default should be true
  DALI_TEST_CHECK(view.IsPreMultipliedAlpha());

  view.SetPreMultipliedAlpha(false);
  DALI_TEST_EQUALS(view.IsPreMultipliedAlpha(), false, TEST_LOCATION);

  view.SetPreMultipliedAlpha(true);
  DALI_TEST_EQUALS(view.IsPreMultipliedAlpha(), true, TEST_LOCATION);
  END_TEST;
}

// SynchronousLoading

int UtcDaliImageViewSetGetSynchronousLoadingP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.IsSynchronousLoading(), false, TEST_LOCATION);
  view.SetSynchronousLoading(true);
  DALI_TEST_EQUALS(view.IsSynchronousLoading(), true, TEST_LOCATION);
  END_TEST;
}

// NPatchBorder (N-Patch)

int UtcDaliImageViewSetGetNPatchBorderP(void)
{
  UiTestApplication application;
  ImageView view   = ImageView::New();
  Vector4   border = Vector4(10.0f, 10.0f, 10.0f, 10.0f);
  view.SetNPatchBorder(border);
  DALI_TEST_EQUALS(view.GetNPatchBorder(), border, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetGetNPatchBorderOnlyP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.IsNPatchBorderOnly(), false, TEST_LOCATION);
  view.SetNPatchBorderOnly(true);
  DALI_TEST_EQUALS(view.IsNPatchBorderOnly(), true, TEST_LOCATION);
  END_TEST;
}

// Reload

int UtcDaliImageViewReloadP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New("image.jpg");
  // Reload on a view with no visual (before scene add) should not crash
  view.Reload();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliImageViewReloadNoImageP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  // Reload without setting an image should not crash
  view.Reload();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// SamplingMode

int UtcDaliImageViewSetGetSamplingModeP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetSamplingMode(), Ui::Image::SamplingMode::BOX_THEN_LINEAR, TEST_LOCATION);

  view.SetSamplingMode(Ui::Image::SamplingMode::NEAREST);
  DALI_TEST_EQUALS(view.GetSamplingMode(), Ui::Image::SamplingMode::NEAREST, TEST_LOCATION);

  view.SetSamplingMode(Ui::Image::SamplingMode::LINEAR);
  DALI_TEST_EQUALS(view.GetSamplingMode(), Ui::Image::SamplingMode::LINEAR, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetSamplingModeNoChangeP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  view.SetSamplingMode(Ui::Image::SamplingMode::NEAREST);
  view.SetSamplingMode(Ui::Image::SamplingMode::NEAREST); // same value — should not dirty
  DALI_TEST_EQUALS(view.GetSamplingMode(), Ui::Image::SamplingMode::NEAREST, TEST_LOCATION);
  END_TEST;
}

// ReleasePolicy

int UtcDaliImageViewSetGetReleasePolicyP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::Image::ReleasePolicy::DETACHED, TEST_LOCATION);

  view.SetReleasePolicy(Ui::Image::ReleasePolicy::DESTROYED);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::Image::ReleasePolicy::DESTROYED, TEST_LOCATION);

  view.SetReleasePolicy(Ui::Image::ReleasePolicy::NEVER);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::Image::ReleasePolicy::NEVER, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetReleasePolicyNoChangeP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  view.SetReleasePolicy(Ui::Image::ReleasePolicy::DESTROYED);
  view.SetReleasePolicy(Ui::Image::ReleasePolicy::DESTROYED); // same value — should not dirty
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::Image::ReleasePolicy::DESTROYED, TEST_LOCATION);
  END_TEST;
}

// Property set/get via Property::Index

int UtcDaliImageViewPropertySamplingModeP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();

  view.SetProperty(Dali::Ui::ImageView::Property::SAMPLING_MODE, static_cast<int>(Ui::Image::SamplingMode::NO_FILTER));
  Dali::Property::Value value = view.GetProperty(Dali::Ui::ImageView::Property::SAMPLING_MODE);
  int mode = 0;
  DALI_TEST_CHECK(value.Get(mode));
  DALI_TEST_EQUALS(static_cast<Ui::Image::SamplingMode>(mode), Ui::Image::SamplingMode::NO_FILTER, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewPropertyReleasePolicyP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();

  view.SetProperty(Dali::Ui::ImageView::Property::RELEASE_POLICY, static_cast<int>(Ui::Image::ReleasePolicy::NEVER));
  Dali::Property::Value value = view.GetProperty(Dali::Ui::ImageView::Property::RELEASE_POLICY);
  int policy = 0;
  DALI_TEST_CHECK(value.Get(policy));
  DALI_TEST_EQUALS(static_cast<Ui::Image::ReleasePolicy>(policy), Ui::Image::ReleasePolicy::NEVER, TEST_LOCATION);
  END_TEST;
}

// AlphaMaskUrl / CropToMask / MaskingMode

int UtcDaliImageViewSetGetAlphaMaskUrlP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  view.SetAlphaMaskUrl("mask.png");
  DALI_TEST_EQUALS(view.GetAlphaMaskUrl(), Dali::String("mask.png"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetGetCropToMaskP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.IsCropToMask(), false, TEST_LOCATION);
  view.SetCropToMask(true);
  DALI_TEST_EQUALS(view.IsCropToMask(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewSetGetMaskingModeP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  view.SetMaskingMode(Ui::Image::MaskingType::MASKING_ON_LOADING);
  DALI_TEST_EQUALS(view.GetMaskingMode(), Ui::Image::MaskingType::MASKING_ON_LOADING, TEST_LOCATION);
  view.SetMaskingMode(Ui::Image::MaskingType::MASKING_ON_RENDERING);
  DALI_TEST_EQUALS(view.GetMaskingMode(), Ui::Image::MaskingType::MASKING_ON_RENDERING, TEST_LOCATION);
  END_TEST;
}

// ImageLoadWithViewSize

int UtcDaliImageViewSetGetImageLoadWithViewSizeP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.IsImageLoadWithViewSizeEnabled(), false, TEST_LOCATION);
  view.SetImageLoadWithViewSize(true);
  DALI_TEST_EQUALS(view.IsImageLoadWithViewSizeEnabled(), true, TEST_LOCATION);
  END_TEST;
}

// FastTrackUploading

int UtcDaliImageViewSetGetFastTrackUploadingP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.IsFastTrackUploadEnabled(), false, TEST_LOCATION);
  view.SetFastTrackUpload(true);
  DALI_TEST_EQUALS(view.IsFastTrackUploadEnabled(), true, TEST_LOCATION);
  END_TEST;
}

// OrientationCorrection

int UtcDaliImageViewSetGetOrientationCorrectionP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  DALI_TEST_EQUALS(view.IsOrientationCorrectionEnabled(), true, TEST_LOCATION);
  view.SetOrientationCorrection(false);
  DALI_TEST_EQUALS(view.IsOrientationCorrectionEnabled(), false, TEST_LOCATION);
  END_TEST;
}

// GetLoadingStatus / Signals

int UtcDaliImageViewGetLoadingStatusP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New("image.jpg");
  Ui::Visual::ResourceStatus status = view.GetLoadingStatus();
  DALI_TEST_CHECK(status == Ui::Visual::ResourceStatus::PREPARING ||
                  status == Ui::Visual::ResourceStatus::READY ||
                  status == Ui::Visual::ResourceStatus::FAILED);
  END_TEST;
}

int UtcDaliImageViewSignalsP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New();
  // ResourceReadySignal is inherited from View — just call, don't store
  view.ResourceReadySignal();
  DALI_TEST_CHECK(true);
  END_TEST;
}

// Fluent chaining

int UtcDaliImageViewChainingP(void)
{
  UiTestApplication application;
  ImageView view = ImageView::New()
                     .SetResourceUrl("image.jpg")
                     .SetFittingMode(Ui::Image::FittingMode::FILL)
                     .SetImageColor(UiColor(1.0f, 1.0f, 1.0f, 0.5f));
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("image.jpg"), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetFittingMode(), Ui::Image::FittingMode::FILL, TEST_LOCATION);
  END_TEST;
}
