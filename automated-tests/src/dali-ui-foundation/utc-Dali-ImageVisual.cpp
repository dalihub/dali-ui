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

#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/animated-image-visual.h>
#include <dali-ui-foundation/public-api/visuals/border-visual.h>
#include <dali-ui-foundation/public-api/visuals/color-visual.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual.h>
#include <dali-ui-foundation/public-api/visuals/image-visual.h>
#include <dali-ui-foundation/public-api/visuals/lottie-animation-visual.h>
#include <dali-ui-foundation/public-api/visuals/text-visual.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>

#include <dali-ui/ui-environment-variable.h>
#include <dali-ui/ui-texture-upload-manager.h>
#include <dali/devel-api/adaptor-framework/texture-upload-manager.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_image_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

namespace Test
{
bool WaitForEventThreadTrigger(int triggerCount, int timeoutInSeconds, int executeCallbacks);
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
  DALI_TEST_EQUALS(visual.GetDepthLayer(), Visual::DepthLayer::NONE, TEST_LOCATION);

  View view = View::New();
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::DepthLayer::BACKGROUND), 0u, TEST_LOCATION);

  DALI_TEST_EQUALS(view.AddVisual(visual, Visual::DepthLayer::BACKGROUND), true, TEST_LOCATION);

  DALI_TEST_EQUALS(visual.GetOwner(), view, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetDepthLayer(), Visual::DepthLayer::BACKGROUND, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::DepthLayer::BACKGROUND), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualAt(Visual::DepthLayer::BACKGROUND, 0u), visual, TEST_LOCATION);

  visual.Detach();

  DALI_TEST_EQUALS(visual.GetOwner(), View(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetDepthLayer(), Visual::DepthLayer::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::DepthLayer::BACKGROUND), 0u, TEST_LOCATION);

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

  visual.SetPreMultiplyAlphaOnLoadEnabled(true);
  DALI_TEST_EQUALS(visual.IsPreMultiplyAlphaOnLoadEnabled(), true, TEST_LOCATION);

  visual.SetAlphaMaskUrl("mask.png");
  DALI_TEST_EQUALS(visual.GetAlphaMaskUrl(), Dali::String("mask.png"), TEST_LOCATION);

  visual.SetContentScaleForMasking(2.0f);
  DALI_TEST_EQUALS(visual.GetContentScaleForMasking(), 2.0f, TEST_LOCATION);

  visual.SetCropToMask(false);
  DALI_TEST_EQUALS(visual.IsCropToMask(), false, TEST_LOCATION);

  visual.SetMaskingPolicy(Image::MaskingPolicy::ON_RENDERING);
  DALI_TEST_EQUALS(visual.GetMaskingPolicy(), Image::MaskingPolicy::ON_RENDERING, TEST_LOCATION);

  visual.SetBrokenImageEnabled(false);
  DALI_TEST_EQUALS(visual.IsBrokenImageEnabled(), false, TEST_LOCATION);

  visual.SetLoadPolicy(Image::LoadPolicy::ATTACHED);
  DALI_TEST_EQUALS(visual.GetLoadPolicy(), Image::LoadPolicy::ATTACHED, TEST_LOCATION);

  visual.SetReleasePolicy(Image::ReleasePolicy::DETACHED);
  DALI_TEST_EQUALS(visual.GetReleasePolicy(), Image::ReleasePolicy::DETACHED, TEST_LOCATION);

  visual.SetFittingMode(Image::FittingMode::CENTER);
  DALI_TEST_EQUALS(visual.GetFittingMode(), Image::FittingMode::CENTER, TEST_LOCATION);

  visual.SetOrientationCorrectionEnabled(false);
  DALI_TEST_EQUALS(visual.IsOrientationCorrectionEnabled(), false, TEST_LOCATION);

  visual.SetImageLoadWithViewSizeEnabled(true);
  DALI_TEST_EQUALS(visual.IsImageLoadWithViewSizeEnabled(), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliImageVisualSetGetProperties02(void)
{
  UiTestApplication application;

  ImageVisual visual = ImageVisual::New();

  visual.SetFastTrackUploadEnabled(true);
  DALI_TEST_EQUALS(visual.IsFastTrackUploadEnabled(), true, TEST_LOCATION);

  visual.SetNPatchBorder(Dali::Insets(1.0f, 2.0f, 3.0f, 4.0f));
  DALI_TEST_EQUALS(visual.GetNPatchBorder(), Dali::Insets(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);

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

  auto TestAssertFunction = [&](std::function<void(void)> func)
  {
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
  TestAssertFunction([&]()
  { empty.SetName("ShouldBeCrash"); });
  TestAssertFunction([&]()
  { empty.SetOffsetX(1.0f); });
  TestAssertFunction([&]()
  { empty.SetOffsetY(1.0f); });
  TestAssertFunction([&]()
  { empty.SetWidth(100.0f); });
  TestAssertFunction([&]()
  { empty.SetHeight(100.0f); });
  TestAssertFunction([&]()
  { empty.SetTransformProportionFlags(Visual::Transform::ProportionFlags::ALL); });
  TestAssertFunction([&]()
  { empty.SetExtraWidth(10.0f); });
  TestAssertFunction([&]()
  { empty.SetExtraHeight(10.0f); });
  TestAssertFunction([&]()
  { empty.SetOrigin(VisualOrigin::CENTER_LEFT); });
  TestAssertFunction([&]()
  { empty.SetPivot(VisualPivot::CENTER_LEFT); });
  TestAssertFunction([&]()
  { empty.SetSiblingOrder(0u); });

  TestAssertFunction([&]()
  { empty.GetOwner(); });
  TestAssertFunction([&]()
  { empty.GetDepthLayer(); });
  TestAssertFunction([&]()
  { empty.GetName(); });
  TestAssertFunction([&]()
  { empty.GetOffsetX(); });
  TestAssertFunction([&]()
  { empty.GetOffsetY(); });
  TestAssertFunction([&]()
  { empty.GetWidth(); });
  TestAssertFunction([&]()
  { empty.GetHeight(); });
  TestAssertFunction([&]()
  { empty.GetTransformProportionFlags(); });
  TestAssertFunction([&]()
  { empty.GetExtraWidth(); });
  TestAssertFunction([&]()
  { empty.GetExtraHeight(); });
  TestAssertFunction([&]()
  { empty.GetOrigin(); });
  TestAssertFunction([&]()
  { empty.GetPivot(); });
  TestAssertFunction([&]()
  { empty.GetSiblingOrder(); });

  END_TEST;
}

int UtcDaliImageVisualReloadKeepsAttachmentP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(120.0f);
  view.SetRequestedHeight(80.0f);
  ImageVisual visual = ImageVisual::New();
  visual.SetResourceUrl("reload-missing-image.png");
  DALI_TEST_CHECK(view.AddVisual(visual, Visual::DepthLayer::CONTENT));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  visual.Reload();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(visual.GetResourceUrl(), Dali::String("reload-missing-image.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetOwner(), view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::DepthLayer::CONTENT), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageVisualFastTrackUploadAndReloadP(void)
{
  UiTestApplication application;
  Test::TextureUploadManager::InitializeGraphicsController(application.GetGraphicsController());

  ImageVisual visual = ImageVisual::New();
  visual.SetResourceUrl("../samples/image-view/res/sample.jpg");
  visual.SetFastTrackUploadEnabled(true);
  View view = View::New();
  view.SetRequestedWidth(120.0f);
  view.SetRequestedHeight(90.0f);
  DALI_TEST_CHECK(view.AddVisual(visual, Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  auto uploadManager = Dali::Devel::TextureUploadManager::Get();
  DALI_TEST_CHECK(uploadManager.ResourceUpload());
  application.Render();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view.IsResourceReady());
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  DALI_TEST_CHECK(view.GetRendererAt(0u).GetTextures());

  visual.Reload();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view.IsResourceReady());
  DALI_TEST_CHECK(view.GetRendererAt(0u).GetTextures());
  END_TEST;
}

int UtcDaliImageVisualFastTrackMissingResourceP(void)
{
  UiTestApplication application;
  Test::TextureUploadManager::InitializeGraphicsController(application.GetGraphicsController());

  ImageVisual visual = ImageVisual::New();
  visual.SetResourceUrl("../samples/image-view/res/does-not-exist.jpg");
  visual.SetFastTrackUploadEnabled(true);
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(80.0f);
  DALI_TEST_CHECK(view.AddVisual(visual, Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  auto uploadManager = Dali::Devel::TextureUploadManager::Get();
  DALI_TEST_CHECK(!uploadManager.ResourceUpload());
  application.Render();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  END_TEST;
}

int UtcDaliImageVisualFastTrackYuvPlaneUploadP(void)
{
  Dali::EnvironmentVariable::SetTestEnvironmentVariable("DALI_LOAD_IMAGE_YUV_PLANES", "1");
  Dali::EnvironmentVariable::SetTestEnvironmentVariable("DALI_ENABLE_DECODE_JPEG_TO_YUV_420", "1");
  UiTestApplication application;
  Test::TextureUploadManager::InitializeGraphicsController(application.GetGraphicsController());

  ImageVisual visual = ImageVisual::New();
  visual.SetResourceUrl("../samples/image-view/res/sample.jpg");
  visual.SetFastTrackUploadEnabled(true);
  View view = View::New();
  view.SetRequestedWidth(128.0f);
  view.SetRequestedHeight(96.0f);
  DALI_TEST_CHECK(view.AddVisual(visual, Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  auto uploadManager = Dali::Devel::TextureUploadManager::Get();
  DALI_TEST_CHECK(uploadManager.ResourceUpload());
  application.Render();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view.IsResourceReady());
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  DALI_TEST_EQUALS(view.GetRendererAt(0u).GetTextures().GetTextureCount(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageVisualGpuMaskRatioAndFailureP(void)
{
  UiTestApplication application;

  auto makeView = [&](const Dali::String& maskUrl, bool cropToMask)
  {
    ImageVisual visual = ImageVisual::New();
    visual.SetResourceUrl("../samples/image-view/res/sample.jpg");
    visual.SetAlphaMaskUrl(maskUrl);
    visual.SetMaskingPolicy(Image::MaskingPolicy::ON_RENDERING);
    visual.SetContentScaleForMasking(1.6f);
    visual.SetCropToMask(cropToMask);
    visual.SetSynchronousLoading(true);
    View view = View::New();
    view.SetRequestedWidth(160.0f);
    view.SetRequestedHeight(120.0f);
    DALI_TEST_CHECK(view.AddVisual(visual, Visual::DepthLayer::BACKGROUND));
    application.GetScene().Add(view);
    return view;
  };

  View cropped = makeView("../samples/image-view/res/mask.png", true);
  View uncropped = makeView("../samples/image-view/res/mask.png", false);
  View missingMask = makeView("../samples/image-view/res/does-not-exist.png", true);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(cropped.GetRendererCount() > 0u);
  DALI_TEST_CHECK(uncropped.GetRendererCount() > 0u);
  Renderer croppedRenderer = cropped.GetRendererAt(0u);
  Renderer uncroppedRenderer = uncropped.GetRendererAt(0u);
  DALI_TEST_EQUALS(croppedRenderer.GetTextures().GetTextureCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(uncroppedRenderer.GetTextures().GetTextureCount(), 2u, TEST_LOCATION);
  const Property::Index croppedIndex = croppedRenderer.GetPropertyIndex("maskTextureRatio");
  const Property::Index uncroppedIndex = uncroppedRenderer.GetPropertyIndex("maskTextureRatio");
  DALI_TEST_CHECK(croppedIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(uncroppedIndex != Property::INVALID_INDEX);
  const Vector2 croppedRatio = croppedRenderer.GetProperty<Vector2>(croppedIndex);
  DALI_TEST_CHECK(croppedRatio.x > 0.0f && croppedRatio.x < 1.0f);
  DALI_TEST_CHECK(croppedRatio.y > 0.0f && croppedRatio.y < 1.0f);
  DALI_TEST_EQUALS(uncroppedRenderer.GetProperty<Vector2>(uncroppedIndex), Vector2::ONE, TEST_LOCATION);

  DALI_TEST_CHECK(missingMask.GetRendererCount() > 0u);
  DALI_TEST_CHECK(missingMask.GetRendererAt(0u).GetTextures());
  END_TEST;
}

int UtcDaliImageVisualSvgPropertyMapAndFittingP(void)
{
  UiTestApplication application;
  ImageVisual visual = ImageVisual::New();
  visual.SetResourceUrl("../samples/image-view/res/svg-blocks.svg");
  visual.SetSynchronousLoading(true);
  visual.SetDesiredWidth(120);
  visual.SetDesiredHeight(80);
  visual.SetFittingMode(Image::FittingMode::CENTER);

  View view = View::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(160.0f);
  DALI_TEST_CHECK(view.AddVisual(visual, Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  auto internalVisual = GetImplementation(visual).GetVisual();
  DALI_TEST_CHECK(internalVisual);
  Property::Map map;
  internalVisual.CreatePropertyMap(map);
  const Property::Value* type = map.Find(Dali::Ui::Integration::Visual::Property::TYPE);
  const Property::Value* url = map.Find(Dali::Ui::Integration::ImageVisual::Property::URL);
  const Property::Value* width = map.Find(Dali::Ui::Integration::ImageVisual::Property::DESIRED_WIDTH);
  const Property::Value* fitting = map.Find(Dali::Ui::Integration::ImageVisual::Property::FITTING_MODE);
  DALI_TEST_CHECK(type && url && width && fitting);
  DALI_TEST_EQUALS(type->Get<int>(), static_cast<int>(Dali::Ui::Integration::InternalVisualType::SVG), TEST_LOCATION);
  DALI_TEST_EQUALS(url->Get<Dali::String>(), Dali::String("../samples/image-view/res/svg-blocks.svg"), TEST_LOCATION);
  DALI_TEST_EQUALS(width->Get<int>(), 120, TEST_LOCATION);
  DALI_TEST_EQUALS(fitting->Get<int>(), static_cast<int>(Image::FittingMode::CENTER), TEST_LOCATION);
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  END_TEST;
}
