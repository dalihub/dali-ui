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
#include <dali-ui-foundation/public-api/visuals/visual-base.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_animatedimagevisual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_animatedimagevisual_cleanup(void)
{
  test_return_value = TET_PASS;
}

/* Test creation, owner handling and attach/detach */
int UtcDaliAnimatedImageVisualCreateAndOwner(void)
{
  UiTestApplication application;

  AnimatedImageVisual visual = AnimatedImageVisual::New();

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

/* Test that the visual type is ANIMATED_IMAGE */
int UtcDaliAnimatedImageVisualGetVisualType(void)
{
  UiTestApplication application;

  AnimatedImageVisual visual = AnimatedImageVisual::New();

  // Upcast the handle
  VisualBase visualBase = visual;

  DALI_TEST_EQUALS(visualBase.GetVisualType(), Visual::ANIMATED_IMAGE, TEST_LOCATION);

  END_TEST;
}

/* Test inherited setters from VisualBase */
int UtcDaliAnimatedImageVisualInheritedSetters(void)
{
  UiTestApplication application;

  // Test that inherited setters from VisualBase work.
  AnimatedImageVisual visual = AnimatedImageVisual::New();

  visual.SetName("AnimatedImageVisual");
  DALI_TEST_EQUALS(visual.GetName(), "AnimatedImageVisual", TEST_LOCATION);

  visual.SetOffsetX(12.0f);
  DALI_TEST_EQUALS(visual.GetOffsetX(), 12.0f, TEST_LOCATION);

  visual.SetOffsetY(34.0f);
  DALI_TEST_EQUALS(visual.GetOffsetY(), 34.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualSetGetProperties01(void)
{
  UiTestApplication application;

  AnimatedImageVisual visual = AnimatedImageVisual::New();

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

  visual.SetEnableBrokenImage(false);
  DALI_TEST_EQUALS(visual.IsBrokenImageEnabled(), false, TEST_LOCATION);

  visual.SetLoadPolicy(Image::LoadPolicy::ATTACHED);
  DALI_TEST_EQUALS(visual.GetLoadPolicy(), Image::LoadPolicy::ATTACHED, TEST_LOCATION);

  visual.SetReleasePolicy(Image::ReleasePolicy::DETACHED);
  DALI_TEST_EQUALS(visual.GetReleasePolicy(), Image::ReleasePolicy::DETACHED, TEST_LOCATION);

  visual.SetFittingMode(Image::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO);
  DALI_TEST_EQUALS(visual.GetFittingMode(), Image::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO, TEST_LOCATION);

  visual.SetOrientationCorrection(false);
  DALI_TEST_EQUALS(visual.IsOrientationCorrection(), false, TEST_LOCATION);

  visual.SetSynchronousSizing(true);
  DALI_TEST_EQUALS(visual.IsSynchronousSizing(), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliAnimatedImageVisualSetGetProperties02(void)
{
  UiTestApplication application;

  AnimatedImageVisual visual = AnimatedImageVisual::New();

  visual.SetResourceUrl("anim.gif");
  DALI_TEST_EQUALS(visual.GetResourceUrl(), Dali::String("anim.gif"), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetResourceUrlList().Empty(), true, TEST_LOCATION);

  visual.SetResourceUrlList({"image01.png", "image02.png"});
  DALI_TEST_EQUALS(visual.GetResourceUrl(), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetResourceUrlList().Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetResourceUrlList()[0], Dali::String("image01.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetResourceUrlList()[1], Dali::String("image02.png"), TEST_LOCATION);

  // Loop count
  visual.SetLoopCount(3);
  DALI_TEST_EQUALS(visual.GetLoopCount(), 3, TEST_LOCATION);

  // Play range (two integers)
  Dali::Property::Array range;
  range.PushBack(10);
  range.PushBack(20);
  visual.SetPlayRange(range);
  const auto resultRange = visual.GetPlayRange();
  DALI_TEST_EQUALS(resultRange.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(resultRange[0].Get<int32_t>(), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(resultRange[1].Get<int32_t>(), 20, TEST_LOCATION);

  // Stop behavior
  visual.SetStopBehavior(Ui::AnimatedImage::StopBehavior::CURRENT_FRAME);
  DALI_TEST_EQUALS(visual.GetStopBehavior(), Ui::AnimatedImage::StopBehavior::CURRENT_FRAME, TEST_LOCATION);

  // Frame speed factor
  visual.SetFrameSpeedFactor(2.5f);
  DALI_TEST_EQUALS(visual.GetFrameSpeedFactor(), 2.5f, TEST_LOCATION);

  // Batch size and cache size
  visual.SetBatchSize(4);
  DALI_TEST_EQUALS(visual.GetBatchSize(), 4, TEST_LOCATION);

  visual.SetCacheSize(6);
  DALI_TEST_EQUALS(visual.GetCacheSize(), 6, TEST_LOCATION);

  // Frame delay
  visual.SetFrameDelay(100);
  DALI_TEST_EQUALS(visual.GetFrameDelay(), 100, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualPlayState(void)
{
  UiTestApplication application;

  AnimatedImageVisual visual;

  View view = View::New()
    .SetRequestedWidth(100.0f)
    .SetRequestedHeight(100.0f)
    .Visuals(
      Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT,
      {
        AnimatedImageVisual::New()
          .SetResourceUrlList({"image01.png", "image02.png"})
          .Play()
          .As(visual),
      });
      
  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(visual.GetPlayState(), AnimatedImage::PlayState::STOPPED, TEST_LOCATION);

  visual.Pause();
  DALI_TEST_EQUALS(visual.GetPlayState(), AnimatedImage::PlayState::PAUSED, TEST_LOCATION);

  visual.Stop();
  DALI_TEST_EQUALS(visual.GetPlayState(), AnimatedImage::PlayState::STOPPED, TEST_LOCATION);

  visual.Play();
  DALI_TEST_EQUALS(visual.GetPlayState(), AnimatedImage::PlayState::PLAYING, TEST_LOCATION);

  visual.JumpTo(1);
  // TODO : Need to prepare real valid images for UTC.

  END_TEST;
}

/* Test that an empty handle crashes on any operation */
int UtcDaliAnimatedImageVisualInvalidHandle(void)
{
  UiTestApplication application;

  // Empty AnimatedImageVisual handle.
  AnimatedImageVisual empty;

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