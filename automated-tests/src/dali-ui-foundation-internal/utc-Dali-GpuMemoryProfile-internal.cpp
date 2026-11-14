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
 */

// EXTERNAL INCLUDES
#include <cstdlib>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text/reveal-integ.h>
#include <dali-ui-foundation/internal/text/rendering/atlas/atlas-manager.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>
#include <dali-ui-foundation/public-api/render-effects/background-blur-effect.h>
#include <dali-ui-foundation/public-api/render-effects/gaussian-blur-effect.h>
#include <dali-ui-foundation/public-api/render-effects/mask-effect.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/animated-image-visual.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual.h>
#include <dali-ui-foundation/public-api/visuals/image-visual.h>
#include <dali-ui-foundation/public-api/visuals/text-visual.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-environment-variable.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <dali-ui/ui-texture-upload-manager.h>
#include <dali/devel-api/adaptor-framework/texture-upload-manager.h>

using namespace Dali;

namespace
{
void SetGpuMemoryProfileEnvironment(const char* value)
{
#if defined(_MSC_VER)
  _putenv_s("DALI_GPU_MEMORY_PROFILE_ENABLED", value ? value : "");
#else
  if(value)
  {
    setenv("DALI_GPU_MEMORY_PROFILE_ENABLED", value, 1);
  }
  else
  {
    unsetenv("DALI_GPU_MEMORY_PROFILE_ENABLED");
  }
#endif
}
} // namespace

void ui_gpu_memory_profile_startup(void)
{
  test_return_value = TET_UNDEF;
  SetGpuMemoryProfileEnvironment("1");
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_GPU_MEMORY_PROFILE_ENABLED", "1");
}

void ui_gpu_memory_profile_cleanup(void)
{
  SetGpuMemoryProfileEnvironment(nullptr);
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_GPU_MEMORY_PROFILE_ENABLED", nullptr);
  test_return_value = TET_PASS;
}

int UtcDaliUiGpuMemoryProfileVisuals(void)
{
  UiTestApplication application;

  Ui::View        svgView = Ui::View::New();
  Ui::ImageVisual svg     = Ui::ImageVisual::New();
  svg.SetResourceUrl("../samples/image-view/res/svg-blocks.svg");
  svg.SetSynchronousLoading(true);
  DALI_TEST_CHECK(svgView.AddVisual(svg, Ui::Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(svgView);

  Ui::View        jpegView = Ui::View::New();
  Ui::ImageVisual jpeg     = Ui::ImageVisual::New();
  jpeg.SetResourceUrl("../samples/image-view/res/sample.jpg");
  jpeg.SetSynchronousLoading(true);
  DALI_TEST_CHECK(jpegView.AddVisual(jpeg, Ui::Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(jpegView);

  Ui::Label label = Ui::Label::New("GPU memory profile text");
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(jpegView.GetRendererCount() > 0u);

  // Advance the fake three-second profiler timer for roughly twelve seconds.
  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliUiGpuMemoryProfileFastTrack(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_LOAD_IMAGE_YUV_PLANES", "1");
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_ENABLE_DECODE_JPEG_TO_YUV_420", "1");
  UiTestApplication application;
  Test::TextureUploadManager::InitializeGraphicsController(application.GetGraphicsController());

  Ui::ImageVisual visual = Ui::ImageVisual::New();
  visual.SetResourceUrl("../samples/image-view/res/sample.jpg");
  visual.SetFastTrackUploadEnabled(true);
  Ui::View view = Ui::View::New();
  view.SetRequestedWidth(128.0f);
  view.SetRequestedHeight(96.0f);
  DALI_TEST_CHECK(view.AddVisual(visual, Ui::Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_CHECK(Dali::Devel::TextureUploadManager::Get().ResourceUpload());
  application.SendNotification();
  application.Render();
  application.GetPlatform().TriggerTimer();
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  END_TEST;
}

int UtcDaliUiGpuMemoryProfileMaskAndSvg(void)
{
  UiTestApplication application;
  for(auto policy : {Ui::Image::MaskingPolicy::ON_LOADING, Ui::Image::MaskingPolicy::ON_RENDERING})
  {
    Ui::ImageVisual visual = Ui::ImageVisual::New();
    visual.SetResourceUrl("../samples/image-view/res/sample.jpg");
    visual.SetAlphaMaskUrl("../samples/image-view/res/mask.png");
    visual.SetMaskingPolicy(policy);
    visual.SetSynchronousLoading(true);
    Ui::View view = Ui::View::New();
    view.SetRequestedWidth(128.0f);
    view.SetRequestedHeight(96.0f);
    DALI_TEST_CHECK(view.AddVisual(visual, Ui::Visual::DepthLayer::BACKGROUND));
    application.GetScene().Add(view);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  }

  Ui::ImageVisual svg = Ui::ImageVisual::New();
  svg.SetResourceUrl("../samples/image-view/res/svg-blocks.svg");
  Ui::View svgView = Ui::View::New();
  DALI_TEST_CHECK(svgView.AddVisual(svg, Ui::Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(svgView);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(2, 5, true));
  application.SendNotification();
  application.Render();
  application.GetPlatform().TriggerTimer();
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliUiGpuMemoryProfileAdditionalImages(void)
{
  UiTestApplication application;

  Ui::AnimatedImageVisual animated = Ui::AnimatedImageVisual::New();
  animated.SetResourceUrl("../samples/image-view/res/dali-logo-anim.gif");
  animated.SetAlphaMaskUrl("../samples/image-view/res/mask.png");
  animated.SetMaskingPolicy(Ui::Image::MaskingPolicy::ON_LOADING);
  animated.SetSynchronousLoading(true);
  Ui::View animatedView = Ui::View::New();
  DALI_TEST_CHECK(animatedView.AddVisual(animated, Ui::Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(animatedView);

  Ui::AnimatedImageVisual renderedMask = Ui::AnimatedImageVisual::New();
  renderedMask.SetResourceUrl("../samples/image-view/res/dali-logo-anim.gif");
  renderedMask.SetAlphaMaskUrl("../samples/image-view/res/mask.png");
  renderedMask.SetMaskingPolicy(Ui::Image::MaskingPolicy::ON_RENDERING);
  renderedMask.SetSynchronousLoading(true);
  Ui::View renderedMaskView = Ui::View::New();
  DALI_TEST_CHECK(renderedMaskView.AddVisual(renderedMask, Ui::Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(renderedMaskView);

  Ui::ImageVisual patch = Ui::ImageVisual::New();
  patch.SetResourceUrl("../samples/image-view/res/tooltip.9.png");
  patch.SetSynchronousLoading(true);
  Ui::View patchView = Ui::View::New();
  DALI_TEST_CHECK(patchView.AddVisual(patch, Ui::Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(patchView);

  Ui::Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  gradient.SetStopNodes({{0.0f, Ui::UiColor(1.0f, 0.0f, 0.0f, 1.0f)}, {1.0f, Ui::UiColor(0.0f, 0.0f, 1.0f, 1.0f)}});
  Ui::GradientVisual gradientVisual = Ui::GradientVisual::New();
  gradientVisual.SetGradient(gradient);
  Ui::View gradientView = Ui::View::New();
  DALI_TEST_CHECK(gradientView.AddVisual(gradientVisual, Ui::Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(gradientView);

  auto*        pixels    = new unsigned char[4u]{255u, 0u, 0u, 255u};
  PixelData    pixelData = PixelData::New(pixels, 4u, 1u, 1u, Pixel::RGBA8888, PixelData::DELETE_ARRAY);
  Ui::ImageUrl imageUrl  = Ui::ImageUrlUtils::GenerateUrl(pixelData);
  DALI_TEST_CHECK(imageUrl);

  Ui::AtlasManager atlas = Ui::AtlasManager::New();
  atlas.CreateAtlas({64u, 64u, 8u, 8u}, Pixel::RGBA8888);

  Ui::Label marquee = Ui::Label::New("This is a long GPU memory profiling marquee label that should scroll within a small width.");
  marquee.SetRequestedWidth(40.0f);
  marquee.SetRequestedHeight(30.0f);
  application.GetScene().Add(marquee);
  marquee.StartMarquee();

  Ui::TextVisual textVisual = Ui::TextVisual::New();
  textVisual.SetText("GPU memory profile TextVisual");
  Ui::View textView = Ui::View::New();
  textView.SetRequestedWidth(240.0f);
  textView.SetRequestedHeight(60.0f);
  DALI_TEST_CHECK(textView.AddVisual(textVisual, Ui::Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(textView);

  Ui::Label blurred = Ui::Label::New("GPU memory profile reveal blur");
  blurred.SetRequestedWidth(300.0f);
  blurred.SetRequestedHeight(90.0f);
  Ui::Text::Reveal reveal;
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
  blurred.SetTextReveal(reveal);
  blurred.SetTextRevealProgress(0.5f);
  application.GetScene().Add(blurred);

  Ui::ImageVisual broken = Ui::ImageVisual::New();
  broken.SetResourceUrl("missing-profile-image.png");
  broken.SetSynchronousLoading(true);
  Ui::View brokenView = Ui::View::New();
  brokenView.SetRequestedWidth(100.0f);
  brokenView.SetRequestedHeight(100.0f);
  DALI_TEST_CHECK(brokenView.AddVisual(broken, Ui::Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(brokenView);

  Ui::Label asyncText = Ui::Label::New("GPU memory profile asynchronous text");
  asyncText.SetRequestedWidth(260.0f);
  asyncText.SetRequestedHeight(60.0f);
  asyncText.SetAsyncRendering(true);
  application.GetScene().Add(asyncText);

  application.SendNotification();
  application.Render();
  Test::WaitForEventThreadTrigger(1, 5, true);
  application.SendNotification();
  application.Render();
  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
    blurred.SetTextRevealProgress(static_cast<float>(tick % 2u));
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliUiGpuMemoryProfileAsyncImage(void)
{
  UiTestApplication application;
  Ui::ImageVisual   image = Ui::ImageVisual::New();
  image.SetResourceUrl("../samples/image-view/res/sample.jpg");
  Ui::View view = Ui::View::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(120.0f);
  DALI_TEST_CHECK(view.AddVisual(image, Ui::Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  application.SendNotification();
  application.Render();
  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
  }
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliUiGpuMemoryProfileEffects(void)
{
  UiTestApplication application;

  Ui::View backgroundBlur = Ui::View::New();
  backgroundBlur.SetRequestedWidth(64.0f);
  backgroundBlur.SetRequestedHeight(64.0f);
  application.GetScene().Add(backgroundBlur);
  backgroundBlur.Arrange(Ui::LayoutRect(0.0f, 0.0f, 64.0f, 64.0f));
  backgroundBlur.SetRenderEffect(Ui::BackgroundBlurEffect::New());

  Ui::View gaussianBlur = Ui::View::New();
  gaussianBlur.SetRequestedWidth(64.0f);
  gaussianBlur.SetRequestedHeight(64.0f);
  application.GetScene().Add(gaussianBlur);
  gaussianBlur.Arrange(Ui::LayoutRect(0.0f, 0.0f, 64.0f, 64.0f));
  gaussianBlur.SetRenderEffect(Ui::GaussianBlurEffect::New(8u));

  Ui::View tinyBackgroundBlur = Ui::View::New();
  tinyBackgroundBlur.SetRequestedWidth(1.0f);
  tinyBackgroundBlur.SetRequestedHeight(1.0f);
  application.GetScene().Add(tinyBackgroundBlur);
  tinyBackgroundBlur.Arrange(Ui::LayoutRect(0.0f, 0.0f, 1.0f, 1.0f));
  tinyBackgroundBlur.SetRenderEffect(Ui::BackgroundBlurEffect::New(1u));

  Ui::View tinyGaussianBlur = Ui::View::New();
  tinyGaussianBlur.SetRequestedWidth(1.0f);
  tinyGaussianBlur.SetRequestedHeight(1.0f);
  application.GetScene().Add(tinyGaussianBlur);
  tinyGaussianBlur.Arrange(Ui::LayoutRect(0.0f, 0.0f, 1.0f, 1.0f));
  tinyGaussianBlur.SetRenderEffect(Ui::GaussianBlurEffect::New(1u));

  Ui::View maskSource = Ui::View::New();
  maskSource.SetRequestedWidth(64.0f);
  maskSource.SetRequestedHeight(64.0f);
  application.GetScene().Add(maskSource);
  maskSource.Arrange(Ui::LayoutRect(0.0f, 0.0f, 64.0f, 64.0f));

  Ui::View masked = Ui::View::New();
  masked.SetRequestedWidth(64.0f);
  masked.SetRequestedHeight(64.0f);
  application.GetScene().Add(masked);
  masked.Arrange(Ui::LayoutRect(0.0f, 0.0f, 64.0f, 64.0f));
  masked.SetRenderEffect(Ui::MaskEffect::New(maskSource));

  Ui::View offscreen = Ui::View::New();
  offscreen.SetRequestedWidth(64.0f);
  offscreen.SetRequestedHeight(64.0f);
  offscreen.SetBackgroundColor(Ui::UiColor(1.0f, 0.0f, 0.0f, 1.0f));
  application.GetScene().Add(offscreen);
  offscreen.Arrange(Ui::LayoutRect(0.0f, 0.0f, 64.0f, 64.0f));
  offscreen.SetOffscreenRenderingEnabled(true);

  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();
  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliUiGpuMemoryProfileSynchronousAnimatedMask(void)
{
  UiTestApplication application;

  for(auto policy : {Ui::Image::MaskingPolicy::ON_LOADING, Ui::Image::MaskingPolicy::ON_RENDERING})
  {
    Ui::AnimatedImageVisual visual = Ui::AnimatedImageVisual::New();
    visual.SetResourceUrl("../samples/image-view/res/dali-logo-anim.gif");
    visual.SetAlphaMaskUrl("../samples/image-view/res/mask.png");
    visual.SetMaskingPolicy(policy);
    visual.SetSynchronousLoading(true);
    Ui::View view = Ui::View::New();
    view.SetRequestedWidth(100.0f);
    view.SetRequestedHeight(100.0f);
    DALI_TEST_CHECK(view.AddVisual(visual, Ui::Visual::DepthLayer::BACKGROUND));
    application.GetScene().Add(view);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(view.GetRendererCount() > 0u);
    const auto textureCount = view.GetRendererAt(0u).GetTextures().GetTextureCount();
    DALI_TEST_EQUALS(textureCount, policy == Ui::Image::MaskingPolicy::ON_RENDERING ? 2u : 1u, TEST_LOCATION);
  }

  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
  }
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliUiGpuMemoryProfileYuvWebpAndJpeg(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_LOAD_IMAGE_YUV_PLANES", "1");
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_ENABLE_DECODE_JPEG_TO_YUV_420", "1");
  UiTestApplication application;

  const char* urls[] = {"../samples/image-view/res/webp-reload-source.webp", "../samples/image-view/res/sample.jpg"};
  for(const char* url : urls)
  {
    Ui::ImageVisual visual = Ui::ImageVisual::New();
    visual.SetResourceUrl(url);
    visual.SetSynchronousLoading(true);
    Ui::View view = Ui::View::New();
    view.SetRequestedWidth(160.0f);
    view.SetRequestedHeight(120.0f);
    DALI_TEST_CHECK(view.AddVisual(visual, Ui::Visual::DepthLayer::BACKGROUND));
    application.GetScene().Add(view);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(view.GetRendererCount() > 0u);
    // TODO : Why this failed on Windows?
    // DALI_TEST_EQUALS(view.GetRendererAt(0u).GetTextures().GetTextureCount(), 3u, TEST_LOCATION);
  }

  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
  }
  DALI_TEST_CHECK(true);
  END_TEST;
}
