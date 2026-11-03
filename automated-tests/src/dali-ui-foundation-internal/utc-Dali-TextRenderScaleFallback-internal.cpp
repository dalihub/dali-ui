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

// Install the test callback before the task includes the adaptor declaration.
#include <dali-ui/ui-event-thread-callback.h>

#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-manager-impl.h>
#include <dali-ui-foundation/internal/text/async-text/text-loading-task.h>
#include <dali-ui-foundation/internal/text/styled-text/gradient-span-data.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-async-task-manager.h>
#include <dali/devel-api/adaptor-framework/image-loading-devel.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/rendering/visual-renderer.h>
#include <cstring>
#include <limits>

using namespace Dali;
namespace
{
namespace Text = Ui::Text;
Ui::Internal::TextLoadingTaskPtr Run(Text::AsyncTextParameters parameters)
{
  Ui::Internal::TextLoadingTaskPtr task = new Ui::Internal::TextLoadingTask(1u, std::move(parameters), nullptr);
  task->mLoader                         = Text::AsyncTextLoader::New();
  task->Process();
  return task;
}
Text::AsyncTextParameters Parameters(float width, float height, float scale)
{
  Text::AsyncTextParameters p;
  p.text           = "Render scale boundary";
  p.fontSize       = 12.0f;
  p.textWidth      = width;
  p.textHeight     = height;
  p.renderScale    = scale;
  p.maxTextureSize = 128;
  p.requestType    = Ui::Integration::Text::Async::RENDER_FIXED_SIZE;
  return p;
}
void SamePixels(PixelData a, PixelData b)
{
  DALI_TEST_CHECK(a && b);
  if(!a || !b)
  {
    return;
  }
  DALI_TEST_EQUALS(a.GetWidth(), b.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetHeight(), b.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetPixelFormat(), b.GetPixelFormat(), TEST_LOCATION);
  const auto   x     = Integration::GetPixelDataBuffer(a);
  const auto   y     = Integration::GetPixelDataBuffer(b);
  const size_t bytes = a.GetWidth() * a.GetHeight() * Pixel::GetBytesPerPixel(a.GetPixelFormat());
  if(a.GetWidth() == b.GetWidth() && a.GetHeight() == b.GetHeight() && a.GetPixelFormat() == b.GetPixelFormat())
  {
    DALI_TEST_EQUALS(std::memcmp(x.buffer, y.buffer, bytes), 0, TEST_LOCATION);
  }
}
// Run the real task and completion paths without depending on worker scheduling.
struct CapturedTasks
{
  CapturedTasks()
  {
    Test::AsyncTaskManager::SetTaskCaptureEnabled(true);
  }

  ~CapturedTasks()
  {
    Test::AsyncTaskManager::SetTaskCaptureEnabled(false);
  }

  void Drain(UiTestApplication& app, int maximum = 0)
  {
    app.SendNotification();
    app.Render();
    auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
    for(auto& task : tasks)
    {
      auto* loading = static_cast<Ui::Internal::TextLoadingTask*>(task.Get());
      if(maximum > 0)
      {
        loading->mParameters.maxTextureSize = maximum;
      }
      DALI_TEST_CHECK(loading->IsReady());
      loading->Process();
      auto manager = Text::AsyncTextManager::Get();
      Text::GetImplementation(manager).LoadComplete(Ui::Internal::TextLoadingTaskPtr(loading));
    }
    app.SendNotification();
    app.Render();
  }
};
void CheckTiles(Ui::Label label, uint32_t maximum)
{
  DALI_TEST_CHECK(label.GetRendererCount() > 0u);
  float previousEnd = 0.0f;
  for(uint32_t i = 0; i < label.GetRendererCount(); ++i)
  {
    auto renderer = label.GetRendererAt(i);
    auto textures = renderer.GetTextures();
    for(uint32_t t = 0; t < textures.GetTextureCount(); ++t)
    {
      auto texture = textures.GetTexture(t);
      if(texture)
      {
        DALI_TEST_CHECK(texture.GetWidth() <= maximum);
        DALI_TEST_CHECK(texture.GetHeight() <= maximum);
      }
    }
    if(label.GetRendererCount() > 1u)
    {
      auto size   = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
      auto offset = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET);
      if(i)
      {
        DALI_TEST_EQUALS(offset.y, previousEnd, 0.001f, TEST_LOCATION);
      }
      DALI_TEST_EQUALS(size.y, float(textures.GetTexture(0u).GetHeight()), 0.001f, TEST_LOCATION);
      previousEnd = offset.y + size.y;
    }
  }
}
} //namespace

int UtcDaliTextRenderScaleFallbackWidthP(void)
{
  UiTestApplication application;
  for(float width : {63.0f, 64.0f, 64.5f, 65.0f, 128.0f, 129.0f, 300.0f})
  {
    auto p      = Parameters(width, 48.0f, 2.0f);
    auto result = Run(p);
    DALI_TEST_CHECK(result->mRenderInfo.textPixelData);
    DALI_TEST_CHECK(result->mRenderInfo.textPixelData.GetWidth() <= 128u);
    DALI_TEST_EQUALS(result->mParameters.renderScale, width <= 64.0f ? 2.0f : 1.0f, TEST_LOCATION);
    if(width > 64.0f)
    {
      p.renderScale  = 1.0f;
      auto reference = Run(p);
      SamePixels(result->mRenderInfo.textPixelData, reference->mRenderInfo.textPixelData);
      DALI_TEST_EQUALS(result->mRenderInfo.size, reference->mRenderInfo.size, TEST_LOCATION);
    }
  }
  END_TEST;
}

int UtcDaliTextRenderScaleFallbackHeightAndStylesP(void)
{
  UiTestApplication application;
  for(float height : {63.0f, 64.0f, 64.5f, 65.0f, 128.0f, 129.0f, 400.0f})
  {
    for(int style = 0; style < 4; ++style)
    {
      auto p = Parameters(48.0f, height, 2.0f);
      p.text.clear();
      for(int i = 0; i < 100; ++i)
      {
        p.text += "A אב 😀\n";
      }
      p.isMultiLine        = true;
      p.ellipsis           = false;
      p.isShadowEnabled    = style == 1;
      p.shadowOffset       = Vector2(2.0f, 3.0f);
      p.isOutlineEnabled   = style == 2;
      p.outlineWidth       = 2;
      p.isUnderlineEnabled = style == 3;
      auto result          = Run(p);
      DALI_TEST_CHECK(result->mRenderInfo.textPixelData);
      DALI_TEST_EQUALS(result->mParameters.renderScale, height <= 64.0f ? 2.0f : 1.0f, TEST_LOCATION);
      if(height > 64.0f)
      {
        p.renderScale  = 1.0f;
        auto reference = Run(p);
        SamePixels(result->mRenderInfo.textPixelData, reference->mRenderInfo.textPixelData);
        if(style == 1 || style == 2)
        {
          SamePixels(result->mRenderInfo.stylePixelData, reference->mRenderInfo.stylePixelData);
        }
        if(style == 3)
        {
          SamePixels(result->mRenderInfo.overlayStylePixelData, reference->mRenderInfo.overlayStylePixelData);
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTextRenderScaleFallbackFitAndMarqueeP(void)
{
  UiTestApplication application;
  auto              p = Parameters(100.0f, 48.0f, 2.0f);
  p.isTextFitEnabled  = true;
  auto result         = Run(p);
  p.renderScale       = 1.0f;
  auto reference      = Run(p);
  DALI_TEST_EQUALS(result->mParameters.renderScale, 1.0f, TEST_LOCATION);
  SamePixels(result->mRenderInfo.textPixelData, reference->mRenderInfo.textPixelData);
  p                  = Parameters(48.0f, 48.0f, 2.0f);
  p.isMarqueeEnabled = true;
  result             = Run(p);
  DALI_TEST_EQUALS(result->mParameters.renderScale, 2.0f, TEST_LOCATION);
  DALI_TEST_CHECK(result->mRenderInfo.textPixelData.GetWidth() <= 128u);
  END_TEST;
}

int UtcDaliLabelRenderScaleFallbackAutoAndTilingP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  bool              finished = false;
  ConnectionTracker tracker;
  const uint32_t    maximum = Dali::GetMaxTextureSize();
  Ui::Label         label   = Ui::Label::New("Auto async fallback");
  label.SetRequestedWidth(float(maximum / 2u + 1u));
  label.SetRequestedHeight(80.0f);
  label.SetAsyncRendering(true);
  label.SetRenderScale(2.0f);
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
  {
    finished = true;
  });
  finished = false;
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  captured.Drain(application);
  DALI_TEST_CHECK(finished);
  CheckTiles(label, maximum);
  DALI_TEST_EQUALS(label.GetRenderScale(), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererAt(0u).GetTextures().GetTexture(0u).GetWidth(), maximum / 2u + 1u, TEST_LOCATION);
  finished = false;
  label.SetText("Updated automatic request");
  label.SetRequestedWidth(120.0f);
  application.SendNotification();
  application.Render();
  captured.Drain(application);
  DALI_TEST_CHECK(finished);
  DALI_TEST_EQUALS(label.GetRendererAt(0u).GetTextures().GetTexture(0u).GetWidth(), 240u, TEST_LOCATION);
  std::string lines;
  for(uint32_t i = 0; i < maximum / 8u + 100u; ++i)
  {
    lines += "Tile\n";
  }
  finished = false;
  label.SetText(String(lines.c_str()));
  label.SetMultiLine(true);
  label.SetRequestedHeight(float(maximum + 100u));
  application.SendNotification();
  application.Render();
  captured.Drain(application);
  DALI_TEST_CHECK(finished);
  DALI_TEST_CHECK(label.GetRendererCount() > 1u);
  CheckTiles(label, maximum);
  label.SetAsyncRendering(false);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(label.GetRendererCount() > 1u);
  CheckTiles(label, maximum);
  END_TEST;
}

int UtcDaliLabelRenderScaleInvalidInputP(void)
{
  UiTestApplication application;
  auto              label = Ui::Label::New("finite");
  for(float value : {0.f, -1.f, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()})
  {
    label.SetRenderScale(2.f);
    label.SetRenderScale(value);
    DALI_TEST_EQUALS(label.GetRenderScale(), 1.f, TEST_LOCATION);
  }
  auto p         = Parameters(64.f, 48.f, std::numeric_limits<float>::max());
  auto result    = Run(p);
  p.renderScale  = 1.f;
  auto reference = Run(p);
  SamePixels(result->mRenderInfo.textPixelData, reference->mRenderInfo.textPixelData);
  END_TEST;
}

int UtcDaliTextRenderScaleFallbackRequestKindsAndSpanP(void)
{
  UiTestApplication application;
  for(auto request : {Ui::Integration::Text::Async::RENDER_FIXED_SIZE,
                      Ui::Integration::Text::Async::RENDER_FIXED_WIDTH,
                      Ui::Integration::Text::Async::RENDER_FIXED_HEIGHT,
                      Ui::Integration::Text::Async::RENDER_CONSTRAINT})
  {
    auto p                                 = Parameters(100.f, 200.f, 2.f);
    p.requestType                          = request;
    p.isMultiLine                          = true;
    p.text                                 = "Gradient span אב 😀 with several wrapping lines";
    p.hasStyledTextStyleSnapshot           = true;
    auto data                              = std::make_shared<Text::Internal::StyledTextGradientSnapshotData>();
    p.styledTextStyleSnapshot.gradientData = data;
    Text::Internal::StyledTextGradientRunSnapshot run;
    run.numberOfCharacters = 10;
    run.style.enabled      = true;
    run.style.units        = Ui::Gradient::Units::USER_SPACE;
    run.style.linearStart  = Vector2::ZERO;
    run.style.linearEnd    = Vector2(80, 40);
    run.style.stops.PushBack({0.f, Color::RED});
    run.style.stops.PushBack({1.f, Color::BLUE});
    data->runs.push_back(run);
    auto result    = Run(p);
    p.renderScale  = 1.f;
    auto reference = Run(p);
    DALI_TEST_EQUALS(result->mParameters.renderScale, 1.f, TEST_LOCATION);
    SamePixels(result->mRenderInfo.textPixelData, reference->mRenderInfo.textPixelData);
  }
  END_TEST;
}

int UtcDaliLabelRenderScaleFallbackParentAndManualP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  bool              finished = false;
  ConnectionTracker tracker;
  const uint32_t    maximum = Dali::GetMaxTextureSize();
  auto              parent  = Ui::View::New();
  parent.SetRequestedWidth(float(maximum / 2 + 10));
  parent.SetRequestedHeight(100.f);
  auto label = Ui::Label::New("Parent resize issues an automatic async request with wrapping text");
  label.SetRequestedWidth(Ui::MATCH_PARENT);
  label.SetRequestedHeight(100.f);
  label.SetAsyncRendering(true);
  label.SetRenderScale(2.f);
  label.SetMultiLine(true);
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
  {
    finished = true;
  });
  parent.Add(label);
  finished = false;
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();
  captured.Drain(application);
  DALI_TEST_CHECK(finished);
  CheckTiles(label, maximum);
  DALI_TEST_EQUALS(label.GetRendererAt(0).GetTextures().GetTexture(0).GetWidth(), maximum / 2 + 10, TEST_LOCATION);
  finished = false;
  parent.SetRequestedWidth(120.f);
  label.SetLineWrapMode(Text::LineWrapMode::CHARACTER);
  application.SendNotification();
  application.Render();
  captured.Drain(application);
  DALI_TEST_CHECK(finished);
  DALI_TEST_EQUALS(label.GetRendererAt(0).GetTextures().GetTexture(0).GetWidth(), 240u, TEST_LOCATION);
  auto& impl = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
  for(int i = 0; i < 4; ++i)
  {
    finished = false;
    switch(i)
    {
      case 0:
      {
        impl.RequestAsyncRenderWithFixedSize(float(maximum / 2 + 10), 100.f);
        break;
      }

      case 1:
      {
        impl.RequestAsyncRenderWithFixedWidth(float(maximum / 2 + 10), 100.f);
        break;
      }

      case 2:
      {
        impl.RequestAsyncRenderWithFixedHeight(float(maximum), 100.f);
        break;
      }

      case 3:
      {
        impl.RequestAsyncRenderWithConstraints(float(maximum), 100.f);
        break;
      }
    }
    captured.Drain(application);
    DALI_TEST_CHECK(finished);
    CheckTiles(label, maximum);
  }
  finished = false;
  for(int i = 0; i < 4; ++i)
  {
    application.SendNotification();
    application.Render();
    captured.Drain(application);
  }
  DALI_TEST_CHECK(!finished);
  END_TEST;
}

int UtcDaliTextRenderScaleEarlyFallbackP(void)
{
  UiTestApplication application;
  for(auto request : {Ui::Integration::Text::Async::RENDER_FIXED_SIZE,
                      Ui::Integration::Text::Async::RENDER_FIXED_WIDTH,
                      Ui::Integration::Text::Async::RENDER_FIXED_HEIGHT,
                      Ui::Integration::Text::Async::RENDER_CONSTRAINT})
  {
    for(bool longText : {false, true})
    {
      auto parameters        = Parameters(48.0f, 300.0f, 2.0f);
      parameters.requestType = request;
      parameters.isMultiLine = true;
      parameters.ellipsis    = false;
      parameters.text        = "Hi";
      if(longText)
      {
        for(int i = 0; i < 60; ++i)
        {
          parameters.text += "\nTile";
        }
      }
      auto actual            = Run(parameters);
      parameters.renderScale = 1.0f;
      auto expected          = Run(parameters);
      DALI_TEST_EQUALS(actual->mParameters.renderScale, 1.0f, TEST_LOCATION);
      SamePixels(actual->mRenderInfo.textPixelData, expected->mRenderInfo.textPixelData);
      DALI_TEST_EQUALS(actual->mRenderInfo.size, expected->mRenderInfo.size, TEST_LOCATION);

      parameters             = Parameters(48.0f, 24.0f, 2.0f);
      parameters.requestType = request;
      parameters.text        = "Hi";
      actual                 = Run(parameters);
      DALI_TEST_CHECK(actual->mRenderInfo.textPixelData);
      DALI_TEST_EQUALS(actual->mParameters.renderScale, 2.0f, TEST_LOCATION);
    }
  }
  END_TEST;
}

int UtcDaliTextRenderScaleFinalLimitAfterEarlyCheckP(void)
{
  UiTestApplication application;
  for(bool cutout : {false, true})
  {
    // Both requested axes fit at RS2, but the actual raster grows beyond 128.
    auto parameters = Parameters(cutout ? 60.0f : 48.0f, cutout ? 40.0f : 24.0f, 2.0f);
    parameters.text = "Tall scrolling";
    if(cutout)
    {
      parameters.padding                       = Insets(10, 10, 10, 10);
      parameters.isCutoutEnabled               = true;
      parameters.isBackgroundWithCutoutEnabled = true;
      parameters.backgroundColorWithCutout     = Color::RED;
    }
    else
    {
      parameters.fontSize         = 100.0f;
      parameters.isMarqueeEnabled = true;
    }
    auto actual            = Run(parameters);
    parameters.renderScale = 1.0f;
    auto expected          = Run(parameters);
    if(cutout)
    {
      // Padding is known before Load, so this request renders once at RS1.
      DALI_TEST_EQUALS(actual->mParameters.renderScale, 1.0f, TEST_LOCATION);
      SamePixels(actual->mRenderInfo.textPixelData, expected->mRenderInfo.textPixelData);
      SamePixels(actual->mRenderInfo.stylePixelData, expected->mRenderInfo.stylePixelData);
      DALI_TEST_EQUALS(actual->mRenderInfo.size, expected->mRenderInfo.size, TEST_LOCATION);
    }
    else
    {
      // Natural marquee height is only known after layout. Reject without retry.
      DALI_TEST_EQUALS(actual->mParameters.renderScale, 2.0f, TEST_LOCATION);
      DALI_TEST_CHECK(!actual->mRenderInfo.textPixelData);
      DALI_TEST_CHECK(!actual->mRenderInfo.marqueePixelData);
      DALI_TEST_CHECK(!expected->mRenderInfo.textPixelData);
      DALI_TEST_CHECK(!expected->mRenderInfo.marqueePixelData);
    }
  }
  END_TEST;
}

int UtcDaliTextRenderScaleEarlyCheckPreservesMeasurementsP(void)
{
  UiTestApplication application;
  for(auto request : {Ui::Integration::Text::Async::COMPUTE_NATURAL_SIZE,
                      Ui::Integration::Text::Async::COMPUTE_HEIGHT_FOR_WIDTH})
  {
    for(float scale : {1.0f, 1.5f, 2.0f})
    {
      auto parameters        = Parameters(48.0f, 300.0f, scale);
      parameters.requestType = request;
      parameters.isMultiLine = true;
      auto actual            = Run(parameters);
      auto loader            = Text::AsyncTextLoader::New();
      auto expected          = request == Ui::Integration::Text::Async::COMPUTE_NATURAL_SIZE
                                 ? loader.GetNaturalSize(parameters)
                                 : loader.GetHeightForWidth(parameters);
      DALI_TEST_EQUALS(actual->mParameters.renderScale, scale, TEST_LOCATION);
      DALI_TEST_EQUALS(actual->mRenderInfo.renderedSize, expected.renderedSize, TEST_LOCATION);
      DALI_TEST_EQUALS(actual->mRenderInfo.requestType, request, TEST_LOCATION);
      DALI_TEST_CHECK(!actual->mRenderInfo.textPixelData);
    }
  }
  END_TEST;
}

int UtcDaliLabelRenderScaleLateLimitCompletionP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  for(unsigned mode = 0u; mode < 6u; ++mode)
  {
    const bool  manual = (mode & 1u) != 0u;
    const float scale  = mode < 2u ? 1.0f : 2.0f;
    const float width  = mode < 4u ? 48.0f : 96.0f; // RS2 falls back to RS1 before loading.
    auto        label  = Ui::Label::New("Long scrolling text for the final raster size limit");
    label.SetRequestedWidth(width);
    label.SetRequestedHeight(24.0f);
    label.SetFontSize(12.0f);
    label.SetRenderScale(scale);
    if(mode >= 2u)
    {
      Ui::Gradient::Linear gradient(Vector2::ZERO, Vector2::ONE);
      gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
      label.SetTextGradient(gradient);
      label.SetTextColor(Ui::UiColor(Color::TRANSPARENT));
    }
    label.SetAsyncRendering(true);
    label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
    unsigned          completions = 0u;
    ConnectionTracker tracker;
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
    {
      ++completions;
    });
    const auto drain = [&]()
    {
      captured.Drain(application, 128);
    };
    application.GetScene().Add(label);
    drain();
    DALI_TEST_EQUALS(completions, 1u, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    const auto renderer = label.GetRendererAt(0u);
    const auto textures = renderer.GetTextures();
    const auto shader   = renderer.GetShader();
    // Label font size is in pixels; exceed the limit even at RenderScale 1.
    label.SetFontSize(200.0f);
    if(manual)
    {
      auto& impl = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
      impl.RequestAsyncRenderWithFixedSize(width, 24.0f);
    }
    drain();
    DALI_TEST_EQUALS(completions, 2u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
    DALI_TEST_CHECK(renderer.GetTextures() == textures);
    DALI_TEST_CHECK(renderer.GetShader() == shader);
    DALI_TEST_CHECK(label.IsMarqueeRunning());
    auto& impl = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
    DALI_TEST_CHECK(!impl.IsManualAsyncRenderInProgress());
    DALI_TEST_EQUALS(label.GetRenderScale(), scale, TEST_LOCATION);
    DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
    label.SetFontSize(12.0f);
    label.SetText("Recovered scrolling text after the final raster limit");
    drain();
    DALI_TEST_EQUALS(completions, 3u, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    label.Unparent();
    drain();
  }
  END_TEST;
}

int UtcDaliLabelRenderScaleFailureLifecycleP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  for(unsigned scenario = 0u; scenario < 10u; ++scenario)
  {
    auto label = Ui::Label::New("Original scrolling text before the rejected raster");
    label.SetRequestedWidth(48.0f);
    label.SetRequestedHeight(24.0f);
    label.SetFontSize(scenario == 0u ? 200.0f : 12.0f);
    label.SetRenderScale(2.0f);
    label.SetAsyncRendering(true);
    label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
    unsigned          completions = 0u;
    bool              reenter     = false;
    ConnectionTracker tracker;
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
    {
      ++completions;
      if(reenter)
      {
        reenter = false;
        if(scenario == 3u)
        {
          label.Unparent();
          label.Reset();
        }
        else
        {
          label.SetFontSize(12.0f);
          label.SetText("New request from the rejected result callback");
          auto& impl = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
          impl.RequestAsyncRenderWithFixedSize(48.0f, 24.0f);
        }
      }
    });
    application.GetScene().Add(label);
    captured.Drain(application, 128);
    DALI_TEST_EQUALS(completions, 1u, TEST_LOCATION);
    if(scenario == 0u)
    {
      DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);
    }
    else
    {
      const auto renderer = label.GetRendererAt(0u);
      const auto textures = renderer.GetTextures();
      label.SetFontSize(200.0f);
      auto& impl = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
      impl.RequestAsyncRenderWithFixedSize(48.0f, 24.0f);
      reenter = scenario == 2u || scenario == 3u;
      if(scenario == 4u || scenario == 5u)
      {
        auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
        DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
        if(tasks.empty())
        {
          END_TEST;
        }
        auto task = Ui::Internal::TextLoadingTaskPtr(static_cast<Ui::Internal::TextLoadingTask*>(tasks.front().Get()));
        if(scenario == 4u)
        {
          label.Unparent();
          label.Reset();
        }
        else
        {
          label.SetFontSize(12.0f);
          label.SetText("Replacement request before the stale result completes");
          impl.RequestAsyncRenderWithFixedSize(48.0f, 24.0f);
        }
        task->mParameters.maxTextureSize = 128;
        task->Process();
        auto manager = Text::AsyncTextManager::Get();
        Text::GetImplementation(manager).LoadComplete(task);
        DALI_TEST_EQUALS(completions, 1u, TEST_LOCATION);
      }
      else
      {
        captured.Drain(application, 128);
        DALI_TEST_EQUALS(completions, 2u, TEST_LOCATION);
        if(label)
        {
          DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
          DALI_TEST_CHECK(renderer.GetTextures() == textures);
          DALI_TEST_CHECK(label.IsMarqueeRunning());
          DALI_TEST_EQUALS(impl.IsManualAsyncRenderInProgress(), scenario == 2u, TEST_LOCATION);
        }
        if(scenario == 1u)
        {
          for(unsigned repeat = 0u; repeat < 3u; ++repeat)
          {
            impl.RequestAsyncRenderWithFixedSize(48.0f, 24.0f);
            captured.Drain(application, 128);
            DALI_TEST_EQUALS(completions, 3u + repeat, TEST_LOCATION);
            DALI_TEST_CHECK(renderer.GetTextures() == textures);
            DALI_TEST_CHECK(!impl.IsManualAsyncRenderInProgress());
          }
        }
      }
    }
    if(label)
    {
      if(scenario == 6u)
      {
        label.SetText("");
        captured.Drain(application, 128);
        DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);
        DALI_TEST_CHECK(!label.IsMarqueeRunning());
        label.SetFontSize(12.0f);
        label.SetText("   \t");
        captured.Drain(application, 128);
        DALI_TEST_CHECK(!label.IsMarqueeRunning());
      }
      if(scenario == 7u)
      {
        label.Unparent();
        captured.Drain(application, 128);
        application.GetScene().Add(label);
      }
      if(scenario == 8u)
      {
        label.SetAsyncRendering(false);
      }
      label.SetFontSize(12.0f);
      label.SetText("Recovered valid scrolling text after failure");
      if(scenario == 9u)
      {
        label.SetTextColor(Ui::UiColor(Color::BLUE));
      }
      captured.Drain(application, 128);
      if(scenario == 8u)
      {
        label.SetAsyncRendering(true);
        captured.Drain(application, 128);
      }
      DALI_TEST_CHECK(label.GetRendererCount() > 0u);
      DALI_TEST_CHECK(!static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation()).IsManualAsyncRenderInProgress());
      label.Unparent();
      label.Reset();
    }
    tracker.DisconnectAll();
    captured.Drain(application, 128);
    DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  }
  END_TEST;
}

int UtcDaliLabelHeightTilesSurviveRasterRejectionP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  const uint32_t    maximum = Dali::GetMaxTextureSize();
  for(bool manual : {false, true})
  {
    std::string lines;
    for(uint32_t i = 0u; i < maximum / 8u + 100u; ++i)
    {
      lines += "Tile A\n";
    }
    auto label = Ui::Label::New(String(lines.c_str()));
    label.SetRequestedWidth(96.0f);
    label.SetRequestedHeight(static_cast<float>(maximum + 100u));
    label.SetFontSize(12.0f);
    label.SetMultiLine(true);
    label.SetAsyncRendering(true);
    unsigned          completions = 0u;
    ConnectionTracker tracker;
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
    {
      ++completions;
    });
    application.GetScene().Add(label);
    captured.Drain(application);
    DALI_TEST_EQUALS(completions, 1u, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererCount() > 1u);
    CheckTiles(label, maximum);
    const auto lineCount = label.GetAsyncLineCount();
    tet_printf("Height tiles: manual=%d tiles=%u lines=%d limit=%u\n", manual, label.GetRendererCount(), lineCount, maximum);

    std::vector<Renderer>   renderers;
    std::vector<TextureSet> textures;
    std::vector<Vector2>    sizes;
    std::vector<Vector2>    offsets;
    std::vector<WeakHandle<Texture>> weakTextures;
    for(uint32_t i = 0u; i < label.GetRendererCount(); ++i)
    {
      auto renderer = label.GetRendererAt(i);
      renderers.push_back(renderer);
      textures.push_back(renderer.GetTextures());
      for(uint32_t slot = 0u; slot < textures.back().GetTextureCount(); ++slot)
      {
        auto texture = textures.back().GetTexture(slot);
        if(texture)
        {
          weakTextures.emplace_back(texture);
        }
      }
      sizes.push_back(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE));
      offsets.push_back(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET));
    }

    // Keep the view size unchanged. The marquee's natural glyph height exceeds
    // the real test texture limit even at RS1, after layout in the loader.
    label.SetText("Oversized scrolling B");
    label.SetMultiLine(false);
    label.SetFontSize(static_cast<float>(maximum * 2u));
    label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
    auto& impl = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
    if(manual)
    {
      impl.RequestAsyncRenderWithFixedSize(96.0f, static_cast<float>(maximum + 100u));
    }
    application.SendNotification();
    application.Render();
    auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
    if(tasks.empty())
    {
      END_TEST;
    }
    auto task = Ui::Internal::TextLoadingTaskPtr(static_cast<Ui::Internal::TextLoadingTask*>(tasks.front().Get()));
    task->Process();
    DALI_TEST_EQUALS(task->mParameters.renderScale, 1.0f, TEST_LOCATION);
    DALI_TEST_CHECK(task->mParameters.isMarqueeEnabled);
    DALI_TEST_CHECK(!task->mRenderInfo.textPixelData);
    DALI_TEST_CHECK(task->mRenderInfo.size.y > static_cast<float>(maximum));
    auto manager = Text::AsyncTextManager::Get();
    Text::GetImplementation(manager).LoadComplete(task);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(completions, 2u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetRendererCount(), static_cast<uint32_t>(renderers.size()), TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetAsyncLineCount(), lineCount, TEST_LOCATION);
    DALI_TEST_CHECK(!label.IsMarqueeRunning());
    DALI_TEST_CHECK(!impl.IsManualAsyncRenderInProgress());
    auto& viewData = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label));
    auto  visual   = viewData.GetVisual(Ui::Text::LabelPropertyIndex::TEXT);
    DALI_TEST_CHECK(!Ui::Internal::TextVisual::IsAsyncRenderPending(visual));
    DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
    for(uint32_t i = 0u; i < label.GetRendererCount(); ++i)
    {
      DALI_TEST_CHECK(label.GetRendererAt(i) == renderers[i]);
      DALI_TEST_CHECK(renderers[i].GetTextures() == textures[i]);
      DALI_TEST_EQUALS(renderers[i].GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), sizes[i], TEST_LOCATION);
      DALI_TEST_EQUALS(renderers[i].GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET), offsets[i], TEST_LOCATION);
    }

    label.SetFontSize(12.0f);
    label.SetText("C");
    captured.Drain(application);
    DALI_TEST_EQUALS(completions, 3u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetAsyncLineCount(), 1, TEST_LOCATION);
    DALI_TEST_CHECK(!Ui::Internal::TextVisual::IsAsyncRenderPending(visual));
    DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures() != textures[0u]);
    std::vector<WeakHandle<Renderer>> weakRenderers;
    for(auto& renderer : renderers)
    {
      weakRenderers.emplace_back(renderer);
    }
    auto currentRenderer = label.GetRendererAt(0u);
    weakRenderers.emplace_back(currentRenderer);
    currentRenderer.Reset();
    renderers.clear();
    textures.clear();
    visual.Reset();
    label.Unparent();
    label.Reset();
    captured.Drain(application);
    application.RunIdles();
    for(const auto& renderer : weakRenderers)
    {
      DALI_TEST_CHECK(!renderer.GetHandle());
    }
    for(const auto& texture : weakTextures)
    {
      DALI_TEST_CHECK(!texture.GetHandle());
    }
    DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  }
  END_TEST;
}
