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

#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/animated-image-visual.h>
#include <dali-ui-foundation/public-api/visuals/border-visual.h>
#include <dali-ui-foundation/public-api/visuals/color-visual.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual.h>
#include <dali-ui-foundation/public-api/visuals/image-visual.h>
#include <dali-ui-foundation/public-api/visuals/lottie-animation-visual.h>
#include <dali-ui-foundation/public-api/visuals/text-visual.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_image_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_image_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

/* Test creation, owner handling and attach/detach */
int UtcDaliImageVisualCreateAndOwner(void)
{
  UiTestApplication application;

  ImageVisual visual = ImageVisual::New();

  // Initially, the visual is not attached to any view.
  DALI_TEST_EQUALS(visual.GetOwner(), View(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::INVALID, TEST_LOCATION);

  View view = View::New();
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), 0u, TEST_LOCATION);

  DALI_TEST_EQUALS(view.AddVisual(visual, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), true, TEST_LOCATION);

  DALI_TEST_EQUALS(visual.GetOwner(), view, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualAt(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, 0u), visual, TEST_LOCATION);

  visual.Detach();

  DALI_TEST_EQUALS(visual.GetOwner(), View(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::INVALID, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), 0u, TEST_LOCATION);

  END_TEST;
}

/* Test that the visual type is IMAGE */
int UtcDaliImageVisualGetVisualType(void)
{
  UiTestApplication application;

  ImageVisual visual = ImageVisual::New();

  // Upcast the handle
  VisualBase visualBase = visual;

  DALI_TEST_EQUALS(visualBase.GetVisualType(), Ui::VisualType::IMAGE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageVisualDownCast(void)
{
  UiTestApplication application;

  ImageVisual visual = ImageVisual::New();

  // Upcast the handle
  BaseHandle baseHandle = visual;

  // Downcast check
  DALI_TEST_CHECK(VisualBase::DownCast(baseHandle));
  DALI_TEST_CHECK(ImageVisual::DownCast(baseHandle));

  // Do not convert to other type of visual
  DALI_TEST_CHECK(!AnimatedImageVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!BorderVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!ColorVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!GradientVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!LottieAnimationVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!TextVisual::DownCast(baseHandle));

  END_TEST;
}

/* Test inherited setters from VisualBase */
int UtcDaliImageVisualInheritedSetters(void)
{
  UiTestApplication application;

  // Test that inherited setters from VisualBase work.
  ImageVisual visual = ImageVisual::New();

  visual.SetName("ImageVisual");
  DALI_TEST_EQUALS(visual.GetName(), "ImageVisual", TEST_LOCATION);

  visual.SetOffsetX(12.0f);
  DALI_TEST_EQUALS(visual.GetOffsetX(), 12.0f, TEST_LOCATION);

  visual.SetOffsetY(34.0f);
  DALI_TEST_EQUALS(visual.GetOffsetY(), 34.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageVisualSetGetProperties01(void)
{
  UiTestApplication application;

  ImageVisual visual = ImageVisual::New();

  visual.SetResourceUrl("image.png");
  DALI_TEST_EQUALS(visual.GetResourceUrl(), Dali::String("image.png"), TEST_LOCATION);

  visual.SetSynchronousLoading(true);
  DALI_TEST_EQUALS(visual.IsSynchronousLoading(), true, TEST_LOCATION);

  visual.SetDesiredWidth(200);
  DALI_TEST_EQUALS(visual.GetDesiredWidth(), 200, TEST_LOCATION);

  visual.SetDesiredHeight(150);
  DALI_TEST_EQUALS(visual.GetDesiredHeight(), 150, TEST_LOCATION);

  visual.SetSamplingMode(Image::SamplingMode::BOX);
  DALI_TEST_EQUALS(visual.GetSamplingMode(), Image::SamplingMode::BOX, TEST_LOCATION);

  visual.SetPixelArea(Vector4(0.1f, 0.2f, 0.5f, 0.5f));
  DALI_TEST_EQUALS(visual.GetPixelArea(), Vector4(0.1f, 0.2f, 0.5f, 0.5f), TEST_LOCATION);

  visual.SetWrapModeU(Dali::WrapMode::REPEAT);
  DALI_TEST_EQUALS(visual.GetWrapModeU(), Dali::WrapMode::REPEAT, TEST_LOCATION);

  visual.SetWrapModeV(Dali::WrapMode::MIRRORED_REPEAT);
  DALI_TEST_EQUALS(visual.GetWrapModeV(), Dali::WrapMode::MIRRORED_REPEAT, TEST_LOCATION);

  visual.SetPreMultipliedAlpha(true);
  DALI_TEST_EQUALS(visual.IsPreMultipliedAlpha(), true, TEST_LOCATION);

  visual.SetAlphaMaskUrl("mask.png");
  DALI_TEST_EQUALS(visual.GetAlphaMaskUrl(), Dali::String("mask.png"), TEST_LOCATION);

  visual.SetMaskContentScale(2.0f);
  DALI_TEST_EQUALS(visual.GetMaskContentScale(), 2.0f, TEST_LOCATION);

  visual.SetCropToMask(false);
  DALI_TEST_EQUALS(visual.IsCropToMask(), false, TEST_LOCATION);

  visual.SetMaskingType(Image::MaskingType::MASKING_ON_RENDERING);
  DALI_TEST_EQUALS(visual.GetMaskingType(), Image::MaskingType::MASKING_ON_RENDERING, TEST_LOCATION);

  visual.SetBrokenImageEnabled(false);
  DALI_TEST_EQUALS(visual.IsBrokenImageEnabled(), false, TEST_LOCATION);

  visual.SetLoadPolicy(Image::LoadPolicy::ATTACHED);
  DALI_TEST_EQUALS(visual.GetLoadPolicy(), Image::LoadPolicy::ATTACHED, TEST_LOCATION);

  visual.SetReleasePolicy(Image::ReleasePolicy::DETACHED);
  DALI_TEST_EQUALS(visual.GetReleasePolicy(), Image::ReleasePolicy::DETACHED, TEST_LOCATION);

  visual.SetFittingMode(Image::FittingMode::CENTER);
  DALI_TEST_EQUALS(visual.GetFittingMode(), Image::FittingMode::CENTER, TEST_LOCATION);

  visual.SetOrientationCorrection(false);
  DALI_TEST_EQUALS(visual.IsOrientationCorrection(), false, TEST_LOCATION);

  visual.SetSynchronousSizing(true);
  DALI_TEST_EQUALS(visual.IsSynchronousSizing(), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliImageVisualSetGetProperties02(void)
{
  UiTestApplication application;

  ImageVisual visual = ImageVisual::New();

  visual.SetFastTrackUploading(true);
  DALI_TEST_EQUALS(visual.IsFastTrackUploading(), true, TEST_LOCATION);

  visual.SetNPatchBorder(Extents(1,2,3,4));
  DALI_TEST_EQUALS(visual.GetNPatchBorder(), Extents(1,2,3,4), TEST_LOCATION);

  visual.SetNPatchBorderOnly(true);
  DALI_TEST_EQUALS(visual.IsNPatchBorderOnly(), true, TEST_LOCATION);

  visual.SetNPatchAuxiliaryImage("aux.png");
  DALI_TEST_EQUALS(visual.GetNPatchAuxiliaryImage(), Dali::String("aux.png"), TEST_LOCATION);

  visual.SetNPatchAuxiliaryImageAlpha(0.5f);
  DALI_TEST_EQUALS(visual.GetNPatchAuxiliaryImageAlpha(), 0.5f, TEST_LOCATION);

  END_TEST;
}

/* Test that an empty handle crashes on any operation */
int UtcDaliImageVisualInvalidHandle(void)
{
  UiTestApplication application;

  // Empty ImageVisual handle.
  ImageVisual empty;

  auto TestAssertFunction = [&](std::function<void(void)> func){
    try
    {
      func();
      tet_result(TET_FAIL);
    }
    catch(DaliException& e)
    {
      tet_result(TET_PASS);
    }
  };

  // Inherit
  TestAssertFunction([&](){empty.SetName("ShouldBeCrash");});
  TestAssertFunction([&](){empty.SetOffsetX(1.0f);});
  TestAssertFunction([&](){empty.SetOffsetY(1.0f);});
  TestAssertFunction([&](){empty.SetWidth(100.0f);});
  TestAssertFunction([&](){empty.SetHeight(100.0f);});
  TestAssertFunction([&](){empty.SetProportionFlags(Visual::Transform::ProportionFlags::ALL);});
  TestAssertFunction([&](){empty.SetExtraWidth(10.0f);});
  TestAssertFunction([&](){empty.SetExtraHeight(10.0f);});
  TestAssertFunction([&](){empty.SetOrigin(Align::CENTER_BEGIN);});
  TestAssertFunction([&](){empty.SetPivot(Align::CENTER_BEGIN);});
  TestAssertFunction([&](){empty.SetSiblingOrder(0u);});
  TestAssertFunction([&](){empty.SetProperty(Property::INVALID_INDEX, Property::Value());});

  TestAssertFunction([&](){empty.GetOwner();});
  TestAssertFunction([&](){empty.GetContainerRangeType();});
  TestAssertFunction([&](){empty.GetName();});
  TestAssertFunction([&](){empty.GetOffsetX();});
  TestAssertFunction([&](){empty.GetOffsetY();});
  TestAssertFunction([&](){empty.GetWidth();});
  TestAssertFunction([&](){empty.GetHeight();});
  TestAssertFunction([&](){empty.GetProportionFlags();});
  TestAssertFunction([&](){empty.GetExtraWidth();});
  TestAssertFunction([&](){empty.GetExtraHeight();});
  TestAssertFunction([&](){empty.GetOrigin();});
  TestAssertFunction([&](){empty.GetPivot();});
  TestAssertFunction([&](){empty.GetSiblingOrder();});
  TestAssertFunction([&](){empty.GetProperty(Property::INVALID_INDEX);});

  END_TEST;
}