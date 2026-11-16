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

#include <dali-ui-foundation/extension-api/layout-scale-manager.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-manager-impl.h>
#include <dali-ui-foundation/internal/text/async-text/text-loading-task.h>
#include <dali-ui-foundation/internal/text/styled-text/gradient-span-data.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/label-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/text/styled-text/gradient-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
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

struct UiScaleRestore
{
  Ui::Extension::LayoutScaleManager manager{Ui::Extension::LayoutScaleManager::Get()};
  const float                       scale{manager.GetUiScale()};
  const bool                        scalable{manager.IsScalable()};

  ~UiScaleRestore()
  {
    // Assertions throw, so restore global state during failure unwinding too.
    manager.SetUiScale(scale);
    manager.SetScalable(scalable);
  }
};

Ui::Internal::TextLoadingTaskPtr Run(Text::AsyncTextParameters parameters)
{
  Ui::Internal::TextLoadingTaskPtr task = Ui::Internal::TextLoadingTask::New(1u, std::move(parameters), nullptr);
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
    run.style.type         = Ui::Gradient::Type::LINEAR;
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

int UtcDaliLabelPixelLessCompletionScopeP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  // Exercise non-limit no-pixel results independently of loader allocation behavior.
  for(unsigned scenario = 0u; scenario < 7u; ++scenario)
  {
    auto label = Ui::Label::New("Original text before a non-limit pixel-less result");
    label.SetRequestedWidth(48.0f);
    label.SetRequestedHeight(24.0f);
    label.SetFontSize(12.0f);
    label.SetRenderScale(scenario == 0u || scenario == 5u ? 1.0f : 2.0f);
    label.SetAsyncRendering(true);
    if(scenario == 3u || scenario == 6u)
    {
      label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
    }
    unsigned          completions = 0u;
    ConnectionTracker tracker;
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
    {
      ++completions;
    });
    application.GetScene().Add(label);
    captured.Drain(application, 128);
    DALI_TEST_EQUALS(completions, 1u, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    auto& impl = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
    if(scenario == 6u)
    {
      label.SetText("Another scrolling text for the existing stale-color request path");
      application.SendNotification();
      application.Render();
    }
    else
    {
      impl.RequestAsyncRenderWithFixedSize(48.0f, 24.0f);
    }
    auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
    if(tasks.empty())
    {
      END_TEST;
    }
    auto task                        = Ui::Internal::TextLoadingTaskPtr(static_cast<Ui::Internal::TextLoadingTask*>(tasks.front().Get()));
    task->mParameters.maxTextureSize = 128;
    task->Process();
    DALI_TEST_CHECK(task->mRenderInfo.textPixelData);
    if(scenario == 2u)
    {
      // A default result alone is not evidence of a texture-limit rejection.
      task->mRenderInfo = Text::AsyncTextRenderInfo();
    }
    else
    {
      task->mRenderInfo.textPixelData.Reset();
      task->mRenderInfo.marqueePixelData.Reset();
      if(scenario == 4u || scenario == 5u)
      {
        task->mRenderInfo.size = Size(48.0f, 300.0f);
        if(scenario == 4u)
        {
          task->mParameters.maxTextureSize = 0;
        }
      }
    }
    if(scenario == 6u)
    {
      auto& viewData = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label));
      auto  visual   = viewData.GetVisual(Ui::Text::LabelPropertyIndex::TEXT);
      Ui::Internal::TextVisual::GetController(visual)->SetDefaultColor(Color::BLUE);
    }
    auto manager = Text::AsyncTextManager::Get();
    Text::GetImplementation(manager).LoadComplete(task);
    if(scenario == 6u)
    {
      // A non-limit no-pixel result retains the existing stale-color request path.
      DALI_TEST_EQUALS(completions, 1u, TEST_LOCATION);
      captured.Drain(application, 128);
    }
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(completions, 2u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetRendererCount(), scenario == 6u ? 1u : 0u, TEST_LOCATION);
    DALI_TEST_CHECK(!impl.IsManualAsyncRenderInProgress());
    DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
    label.SetText("Recovered normal text after the pixel-less result");
    captured.Drain(application, 128);
    DALI_TEST_EQUALS(completions, 3u, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    label.Unparent();
    captured.Drain(application, 128);
  }
  END_TEST;
}

int UtcDaliLabelRejectedRasterColorChangeP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  for(float scale : {1.0f, 2.0f})
  {
    auto label = Ui::Label::New("Original scrolling text before the rejected raster");
    label.SetRequestedWidth(96.0f);
    label.SetRequestedHeight(24.0f);
    label.SetFontSize(12.0f);
    label.SetRenderScale(scale);
    label.SetAsyncRendering(true);
    label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
    unsigned          completions = 0u;
    ConnectionTracker tracker;
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
    {
      ++completions;
    });
    application.GetScene().Add(label);
    captured.Drain(application, 128);
    DALI_TEST_EQUALS(completions, 1u, TEST_LOCATION);
    const auto renderer = label.GetRendererAt(0u);
    const auto textures = renderer.GetTextures();
    label.SetFontSize(200.0f);
    application.SendNotification();
    application.Render();
    auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
    if(tasks.empty())
    {
      END_TEST;
    }
    auto task                        = Ui::Internal::TextLoadingTaskPtr(static_cast<Ui::Internal::TextLoadingTask*>(tasks.front().Get()));
    task->mParameters.maxTextureSize = 128;
    task->Process();
    DALI_TEST_EQUALS(task->mParameters.renderScale, 1.0f, TEST_LOCATION);
    DALI_TEST_CHECK(task->mParameters.isMarqueeEnabled);
    DALI_TEST_CHECK(!task->mRenderInfo.textPixelData);
    DALI_TEST_CHECK(task->mRenderInfo.size.y > 128.0f);
    // Change the current color after rasterization without dispatching another task.
    auto& impl     = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
    auto& viewData = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label));
    auto  visual   = viewData.GetVisual(Ui::Text::LabelPropertyIndex::TEXT);
    Ui::Internal::TextVisual::GetController(visual)->SetDefaultColor(Color::BLUE);
    auto manager = Text::AsyncTextManager::Get();
    Text::GetImplementation(manager).LoadComplete(task);
    DALI_TEST_EQUALS(completions, 2u, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
    DALI_TEST_CHECK(renderer.GetTextures() == textures);
    DALI_TEST_CHECK(!impl.IsManualAsyncRenderInProgress());
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
    label.SetFontSize(12.0f);
    captured.Drain(application, 128);
    DALI_TEST_EQUALS(completions, 3u, TEST_LOCATION);
    label.Unparent();
    captured.Drain(application, 128);
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

int UtcDaliLabelGradientHeightTilingLifecycleP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  ConnectionTracker tracker;
  const uint32_t    maximum = Dali::GetMaxTextureSize();
  std::string       lines;
  for(uint32_t i = 0u; i < maximum / 8u + 100u; ++i)
  {
    lines += "Tile\n";
  }
  Ui::Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
  Ui::Label label = Ui::Label::New(lines.c_str());
  label.SetRequestedWidth(120.0f);
  label.SetRequestedHeight(static_cast<float>(maximum + 100u));
  label.SetMultiLine(true);
  // Use exact line advances so layout and raster heights agree at every cap.
  label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
  label.SetLineHeight(32.0f);
  label.SetTextGradient(gradient);
  label.SetTextGradientOverlay(gradient);
  label.SetTextColor(Ui::UiColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f)));
  unsigned completions = 0u;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
  {
    ++completions;
  });
  application.GetScene().Add(label);
  for(bool async : {false, true, false, true})
  {
    label.SetAsyncRendering(async);
    captured.Drain(application);
    DALI_TEST_CHECK(label.GetRendererCount() > 1u);
    CheckTiles(label, maximum);
    auto firstTextures = label.GetRendererAt(0u).GetTextures();
    for(uint32_t i = 0u; i < label.GetRendererCount(); ++i)
    {
      auto renderer = label.GetRendererAt(i);
      DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextGradientBounds") != Property::INVALID_INDEX);
      DALI_TEST_EQUALS(renderer.GetProperty<float>(renderer.GetPropertyIndex("visualTransformUseEffectiveScale")), 0.0f, TEST_LOCATION);
      DALI_TEST_CHECK(renderer.GetTextures().GetTexture(1u) == firstTextures.GetTexture(1u));
      DALI_TEST_CHECK(renderer.GetTextures().GetTexture(2u) == firstTextures.GetTexture(2u));
    }
  }
  Animation animation = Animation::New(2.0f);
  label.Animate(animation)
    .TextGradientStartOffset(0.8f, Ui::Duration(2.0f))
    .TextGradientOverlayStartOffset(0.6f, Ui::Duration(2.0f));
  animation.Play();
  auto       unchangedTextures = label.GetRendererAt(1u).GetTextures();
  const auto beforeAnimation   = completions;
  for(uint32_t frame = 0u; frame < 100u; ++frame)
  {
    application.SendNotification();
    application.Render(16u);
    for(uint32_t i = 1u; i < label.GetRendererCount(); ++i)
    {
      for(const char* name : {"uTextGradientStartOffset", "uTextGradientOverlayStartOffset"})
      {
        auto first = label.GetRendererAt(0u);
        auto tile  = label.GetRendererAt(i);
        DALI_TEST_EQUALS(tile.GetCurrentProperty<float>(tile.GetPropertyIndex(name)),
                         first.GetCurrentProperty<float>(first.GetPropertyIndex(name)), 0.001f, TEST_LOCATION);
      }
    }
  }
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetCurrentProperty<float>(label.GetRendererAt(0u).GetPropertyIndex("uTextGradientStartOffset")) > 0.1f);
  DALI_TEST_CHECK(label.GetRendererAt(1u).GetTextures() == unchangedTextures);
  DALI_TEST_EQUALS(completions, beforeAnimation, TEST_LOCATION);
  DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  // Automatic layout snaps the raster to complete lines. Leave room beyond
  // each cap; exact one-pixel raster boundaries are covered by direct publication.
  for(uint32_t height : {2u * maximum + 100u, maximum + 100u})
  {
    label.SetRequestedHeight(static_cast<float>(height));
    captured.Drain(application);
    application.SendNotification();
    application.Render(16u);
    DALI_TEST_CHECK(label.GetRendererCount() > 1u);
    for(uint32_t tile = 1u; tile < label.GetRendererCount(); ++tile)
    {
      for(const char* name : {"uTextGradientStartOffset", "uTextGradientOverlayStartOffset"})
      {
        auto first = label.GetRendererAt(0u);
        auto next  = label.GetRendererAt(tile);
        DALI_TEST_EQUALS(next.GetCurrentProperty<float>(next.GetPropertyIndex(name)),
                         first.GetCurrentProperty<float>(first.GetPropertyIndex(name)), 0.001f, TEST_LOCATION);
      }
    }
  }
  animation.Stop();
  animation.Play();
  application.SendNotification();
  application.Render(16u);
  animation.Stop();
  animation.Clear();
  animation.Reset();
  unchangedTextures.Reset();
  auto&      impl         = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
  const auto beforeManual = completions;
  impl.RequestAsyncRenderWithFixedSize(120.0f, static_cast<float>(maximum + 100u));
  captured.Drain(application);
  DALI_TEST_EQUALS(completions, beforeManual + 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!impl.IsManualAsyncRenderInProgress());
  label.SetRenderScale(2.0f);
  label.SetText((lines + "Again").c_str());
  captured.Drain(application);
  DALI_TEST_EQUALS(label.GetRenderScale(), 2.0f, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererCount() > 1u);
  CheckTiles(label, maximum);
  label.SetRequestedHeight(40.0f);
  label.SetText("short");
  captured.Drain(application);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  label.SetTextGradient({});
  label.SetTextGradientOverlay({});
  captured.Drain(application);
  DALI_TEST_EQUALS(label.GetRendererAt(0u).GetTextures().GetTextureCount(), 1u, TEST_LOCATION);
  label.Unparent();
  captured.Drain(application);
  DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);
  application.GetScene().Add(label);
  captured.Drain(application);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  auto                 renderer = label.GetRendererAt(0u);
  WeakHandle<Renderer> weak(renderer);
  renderer.Reset();
  label.Unparent();
  label.Reset();
  captured.Drain(application);
  application.RunIdles();
  DALI_TEST_CHECK(!weak.GetHandle());
  DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  END_TEST;
}

int UtcDaliLabelGradientHeightTilingMissingMixedCompletionP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  ConnectionTracker tracker;
  const uint32_t    maximum = Dali::GetMaxTextureSize();
  std::string       lines   = "<color value='red'>Explicit foreground</color>\n";
  for(uint32_t i = 0u; i < maximum / 8u + 100u; ++i)
  {
    lines += "Tile\n";
  }
  Ui::Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
  Ui::Label label = Ui::Label::New();
  label.SetRequestedWidth(120.0f);
  label.SetRequestedHeight(static_cast<float>(maximum + 100u));
  label.SetMultiLine(true);
  label.SetAsyncRendering(true);
  label.SetTextGradient(gradient);
  label.SetStyledText(Text::StyledText::FromMarkup(lines.c_str()));
  unsigned completions = 0u;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
  {
    ++completions;
  });
  application.GetScene().Add(label);
  captured.Drain(application);
  auto& impl   = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
  auto  visual = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label)).GetVisual(Text::LabelPropertyIndex::TEXT);
  for(bool manual : {false, true})
  {
    const auto before = completions;
    if(manual)
    {
      impl.RequestAsyncRenderWithFixedSize(120.0f, static_cast<float>(maximum + 100u));
    }
    else
    {
      label.SetStyledText(Text::StyledText::FromMarkup((lines + "updated").c_str()));
    }
    application.SendNotification();
    application.Render();
    auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
    for(auto& task : tasks)
    {
      auto* loading = static_cast<Ui::Internal::TextLoadingTask*>(task.Get());
      loading->Process();
      DALI_TEST_CHECK(loading->mRenderInfo.textGradientPreservedPixelData);
      DALI_TEST_CHECK(loading->mRenderInfo.textGradientMaskPixelData);
      loading->mRenderInfo.textGradientMaskPixelData.Reset();
      auto manager = Text::AsyncTextManager::Get();
      Text::GetImplementation(manager).LoadComplete(Ui::Internal::TextLoadingTaskPtr(loading));
    }
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(completions, before + 1u, TEST_LOCATION);
    DALI_TEST_CHECK(!impl.IsManualAsyncRenderInProgress());
    DALI_TEST_CHECK(!Ui::Internal::TextVisual::IsAsyncRenderPending(visual));
    DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
    DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);
    label.SetStyledText(Text::StyledText::FromMarkup((lines + (manual ? "recovered manual" : "recovered auto")).c_str()));
    captured.Drain(application);
    DALI_TEST_EQUALS(completions, before + 2u, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererCount() > 1u);
    CheckTiles(label, maximum);
  }
  END_TEST;
}

int UtcDaliLabelGradientHeightTilingFailureReentryP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  const uint32_t    maximum = Dali::GetMaxTextureSize();
  std::string       lines   = "<color value='red'>Explicit</color>\n";
  for(uint32_t i = 0u; i < maximum / 8u + 100u; ++i)
  {
    lines += "Tile\n";
  }
  Ui::Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
  // Prior output: none, one renderer, or multiple height tiles.
  // Completion action: no-op, new request, unparent, destroy, or synchronous.
  for(unsigned prior = 0u; prior < 3u; ++prior)
  {
    for(bool manual : {false, true})
    {
      for(unsigned action = 0u; action < 5u; ++action)
      {
        ConnectionTracker tracker;
        auto              label = Ui::Label::New();
        label.SetRequestedWidth(120.0f);
        label.SetRequestedHeight(prior == 1u ? 40.0f : static_cast<float>(maximum + 100u));
        label.SetMultiLine(true);
        // Use exact line advances so layout and raster heights agree at every cap.
        label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
        label.SetLineHeight(32.0f);
        label.SetAsyncRendering(true);
        label.SetTextGradient(gradient);
        label.SetStyledText(Text::StyledText::FromMarkup(lines.c_str()));
        application.GetScene().Add(label);
        if(prior != 0u)
        {
          captured.Drain(application);
          DALI_TEST_EQUALS(label.GetRendererCount() > 1u, prior == 2u, TEST_LOCATION);
        }
        unsigned completions = 0u;
        bool     armed       = true;
        label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View owner, float, float)
        {
          ++completions;
          if(!armed)
          {
            return;
          }
          armed = false;
          DALI_TEST_EQUALS(owner.GetRendererCount(), 0u, TEST_LOCATION);
          switch(action)
          {
            case 1u:
            {
              label.SetStyledText(Text::StyledText::FromMarkup((lines + "callback").c_str()));
              break;
            }
            case 2u:
            {
              label.Unparent();
              break;
            }
            case 3u:
            {
              label.Unparent();
              label.Reset();
              break;
            }
            case 4u:
            {
              label.SetAsyncRendering(false);
              break;
            }
            default:
            {
              break;
            }
          }
        });
        auto              visual = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label)).GetVisual(Text::LabelPropertyIndex::TEXT);
        WeakHandle<Actor> weakOwner(label);
        if(action == 3u)
        {
          // Do not keep the visual alive externally during the destroy callback.
          visual.Reset();
        }
        if(manual)
        {
          static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation()).RequestAsyncRenderWithFixedSize(120.0f, static_cast<float>(maximum + 100u));
        }
        else
        {
          label.SetRequestedHeight(static_cast<float>(maximum + 100u));
          label.SetStyledText(Text::StyledText::FromMarkup((lines + "failure").c_str()));
        }
        application.SendNotification();
        application.Render();
        auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
        DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
        for(auto& task : tasks)
        {
          auto* loading = static_cast<Ui::Internal::TextLoadingTask*>(task.Get());
          loading->Process();
          DALI_TEST_CHECK(loading->mRenderInfo.textGradientPreservedPixelData);
          DALI_TEST_CHECK(loading->mRenderInfo.textGradientMaskPixelData);
          DALI_TEST_EQUALS(loading->mRenderInfo.textGradientPreservedPixelData.GetWidth(), loading->mRenderInfo.textGradientMaskPixelData.GetWidth(), TEST_LOCATION);
          DALI_TEST_EQUALS(loading->mRenderInfo.textGradientPreservedPixelData.GetHeight(), loading->mRenderInfo.textGradientMaskPixelData.GetHeight(), TEST_LOCATION);
          loading->mRenderInfo.textGradientMaskPixelData.Reset();
          auto manager = Text::AsyncTextManager::Get();
          Text::GetImplementation(manager).LoadComplete(Ui::Internal::TextLoadingTaskPtr(loading));
        }
        tasks.clear();
        DALI_TEST_EQUALS(completions, 1u, TEST_LOCATION);
        if(label)
        {
          DALI_TEST_CHECK(!static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation()).IsManualAsyncRenderInProgress());
        }
        if(action == 3u)
        {
          visual.Reset();
          captured.Drain(application);
          application.RunIdles();
          DALI_TEST_CHECK(!weakOwner.GetHandle());
        }
        else
        {
          if(action == 2u)
          {
            application.GetScene().Add(label);
          }
          if(action != 1u)
          {
            label.SetStyledText(Text::StyledText::FromMarkup((lines + "recovery").c_str()));
          }
          captured.Drain(application);
          DALI_TEST_CHECK(label.GetRendererCount() > 0u);
          CheckTiles(label, maximum);
          DALI_TEST_CHECK(!Ui::Internal::TextVisual::IsAsyncRenderPending(visual));
          label.Unparent();
          label.Reset();
          visual.Reset();
          captured.Drain(application);
        }
        DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
      }
    }
  }
  END_TEST;
}

int UtcDaliLabelGradientHeightTilingUiScaleP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  UiScaleRestore    restore;
  auto              scaleManager = restore.manager;
  const uint32_t    maximum      = Dali::GetMaxTextureSize();
  std::string       lines;
  for(uint32_t i = 0u; i < maximum / 8u + 100u; ++i)
  {
    lines += "Tile\n";
  }
  scaleManager.SetScalable(true);
  Ui::Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
  for(bool span : {false, true})
  {
    Ui::Label label = Ui::Label::New(lines.c_str());
    label.SetMultiLine(true);
    // Use exact line advances so layout and raster heights agree at every cap.
    label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
    Ui::Extension::View::SetUiScalePolicy(label, Ui::Extension::UiScalePolicy::ENABLED);
    if(span)
    {
      auto builder = Text::StyledTextBuilder::New(lines.c_str());
      builder.SetSpan(Text::GradientSpan::New(gradient), 0u, builder.GetUtf32Length());
      label.SetStyledText(builder.Build());
    }
    application.GetScene().Add(label);
    for(bool async : {false, true, false})
    {
      label.SetAsyncRendering(async);
      for(float scale : {0.5f, 1.0f, 1.25f, 2.0f})
      {
        scaleManager.SetUiScale(scale);
        label.SetFontSize(12.0f / scale);
        label.SetLineHeight(32.0f / scale);
        label.SetRequestedWidth(120.0f / scale);
        label.SetRequestedHeight(static_cast<float>(maximum + 100u) / scale);
        captured.Drain(application);
        DALI_TEST_CHECK(label.GetRendererCount() > 1u);
        CheckTiles(label, maximum);
        std::vector<Renderer> renderers;
        std::vector<uint32_t> propertyCounts;
        for(uint32_t tile = 0u; tile < label.GetRendererCount(); ++tile)
        {
          auto       renderer = label.GetRendererAt(tile);
          const auto policy   = renderer.GetPropertyIndex("visualTransformUseEffectiveScale");
          DALI_TEST_CHECK(policy != Property::INVALID_INDEX);
          DALI_TEST_EQUALS(renderer.GetProperty<float>(policy), 0.0f, TEST_LOCATION);
          DALI_TEST_EQUALS(renderer.GetTextures().GetTextureCount(), 1u, TEST_LOCATION);
          DALI_TEST_EQUALS(renderer.GetTextures().GetTexture(0u).GetPixelFormat(), span ? Pixel::RGBA8888 : Pixel::L8, TEST_LOCATION);
          renderers.push_back(renderer);
          propertyCounts.push_back(renderer.GetPropertyCount());
        }
        for(uint32_t frame = 0u; frame < 100u; ++frame)
        {
          application.SendNotification();
          application.Render(16u);
        }
        DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
        DALI_TEST_EQUALS(label.GetRendererCount(), renderers.size(), TEST_LOCATION);
        for(uint32_t tile = 0u; tile < label.GetRendererCount(); ++tile)
        {
          DALI_TEST_CHECK(label.GetRendererAt(tile) == renderers[tile]);
          DALI_TEST_EQUALS(renderers[tile].GetPropertyCount(), propertyCounts[tile], TEST_LOCATION);
        }
        renderers.clear();
        if(async)
        {
          auto& impl = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
          // Allow for complete-line rounding; exact raster boundaries are tested by direct publication.
          impl.RequestAsyncRenderWithFixedSize(120.0f, static_cast<float>(maximum + 100u));
          captured.Drain(application);
          DALI_TEST_CHECK(!impl.IsManualAsyncRenderInProgress());
          DALI_TEST_CHECK(label.GetRendererCount() > 1u);
          CheckTiles(label, maximum);
          for(uint32_t tile = 0u; tile < label.GetRendererCount(); ++tile)
          {
            auto renderer = label.GetRendererAt(tile);
            DALI_TEST_EQUALS(renderer.GetProperty<float>(renderer.GetPropertyIndex("visualTransformUseEffectiveScale")), 0.0f, TEST_LOCATION);
          }
        }
        std::vector<WeakHandle<Renderer>> retired;
        for(uint32_t tile = 1u; tile < label.GetRendererCount(); ++tile)
        {
          auto renderer = label.GetRendererAt(tile);
          retired.emplace_back(renderer);
        }
        if(async)
        {
          // Manual completion suppresses an immediate size-only request; resume automatic rendering with a property update.
          label.SetFontSize(13.0f / scale);
        }
        label.SetRequestedHeight(40.0f / scale);
        captured.Drain(application);
        application.RunIdles();
        DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
        for(const auto& renderer : retired)
        {
          DALI_TEST_CHECK(!renderer.GetHandle());
        }
      }
    }
    label.Unparent();
    captured.Drain(application);
    DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);
    application.GetScene().Add(label);
    captured.Drain(application);
    DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
    auto                 renderer = label.GetRendererAt(0u);
    WeakHandle<Renderer> retired(renderer);
    renderer.Reset();
    label.Unparent();
    label.Reset();
    captured.Drain(application);
    application.RunIdles();
    DALI_TEST_CHECK(!retired.GetHandle());
  }
  END_TEST;
}

int UtcDaliLabelGradientHeightTilingPolicyTransitionP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  UiScaleRestore    restore;
  auto              scaleManager = restore.manager;
  const uint32_t    maximum      = Dali::GetMaxTextureSize();
  std::string       lines;
  for(uint32_t i = 0u; i < maximum / 8u + 100u; ++i)
  {
    lines += "Tile\n";
  }
  scaleManager.SetScalable(true);
  Ui::Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
  for(float scale : {0.5f, 1.0f, 1.25f, 2.0f})
  {
    scaleManager.SetUiScale(scale);
    for(unsigned feature = 0u; feature < 5u; ++feature)
    {
      for(bool async : {false, true})
      {
        Ui::Label label = Ui::Label::New(lines.c_str());
        Ui::Extension::View::SetUiScalePolicy(label, Ui::Extension::UiScalePolicy::ENABLED);
        label.SetMultiLine(true);
        // Use exact line advances so layout and raster heights agree at every cap.
        label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
        label.SetFontSize(12.0f / scale);
        label.SetLineHeight(32.0f / scale);
        label.SetRequestedWidth(120.0f / scale);
        label.SetRequestedHeight(static_cast<float>(maximum + 100u) / scale);
        if(feature == 1u)
        {
          auto builder = Text::StyledTextBuilder::New(lines.c_str());
          builder.SetSpan(Text::GradientSpan::New(gradient), 0u, builder.GetUtf32Length());
          label.SetStyledText(builder.Build());
        }
        if(feature == 2u || feature == 4u)
        {
          label.SetTextGradient(gradient);
        }
        if(feature == 3u || feature == 4u)
        {
          label.SetTextGradientOverlay(gradient);
        }
        label.SetAsyncRendering(async);
        application.GetScene().Add(label);
        captured.Drain(application);
        DALI_TEST_CHECK(label.GetRendererCount() > 1u);
        auto  visual = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label)).GetVisual(Text::LabelPropertyIndex::TEXT);
        auto& impl   = Ui::GetImplementation(visual).GetVisualObject();
        impl.SetFittingModeRequired(false);
        for(bool fitting : {true, false, false, true, true, false, true})
        {
          std::vector<Renderer>   renderers;
          std::vector<TextureSet> textures;
          std::vector<uint32_t>   counts;
          std::vector<Vector2>    sizes;
          std::vector<Vector2>    offsets;
          for(uint32_t tile = 0u; tile < label.GetRendererCount(); ++tile)
          {
            auto renderer = label.GetRendererAt(tile);
            renderers.push_back(renderer);
            textures.push_back(renderer.GetTextures());
            counts.push_back(renderer.GetPropertyCount());
            sizes.push_back(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE));
            offsets.push_back(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET));
          }
          impl.SetTransformMapUsageForFittingMode(fitting);
          // Keep layout dimensions fixed while changing only the transform policy.
          impl.SetTransformAndSize(Property::Map{}, Vector2(120.0f, static_cast<float>(maximum + 100u)), 1.0f);
          application.SendNotification();
          application.Render();
          application.SendNotification();
          application.Render();
          DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
          DALI_TEST_CHECK(label.GetRendererCount() > 1u);
          CheckTiles(label, maximum);
          for(uint32_t tile = 0u; tile < label.GetRendererCount(); ++tile)
          {
            auto       renderer = label.GetRendererAt(tile);
            const auto index    = renderer.GetPropertyIndex("visualTransformUseEffectiveScale");
            DALI_TEST_CHECK(index != Property::INVALID_INDEX);
            DALI_TEST_EQUALS(renderer.GetProperty<float>(index), fitting ? 0.0f : 1.0f, TEST_LOCATION);
            DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(index), fitting ? 0.0f : 1.0f, TEST_LOCATION);
            if(async)
            {
              DALI_TEST_EQUALS(label.GetRendererCount(), renderers.size(), TEST_LOCATION);
              DALI_TEST_CHECK(renderer == renderers[tile]);
              DALI_TEST_CHECK(renderer.GetTextures() == textures[tile]);
              DALI_TEST_EQUALS(renderer.GetPropertyCount(), counts[tile], TEST_LOCATION);
              DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), sizes[tile], TEST_LOCATION);
              DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET), offsets[tile], TEST_LOCATION);
            }
          }
        }
        if(async)
        {
          label.SetRequestedHeight(static_cast<float>(maximum + 200u) / scale);
          application.SendNotification();
          application.Render();
          auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
          DALI_TEST_CHECK(!tasks.empty());
          impl.SetTransformMapUsageForFittingMode(false);
          impl.SetTransformAndSize(Property::Map{}, Vector2(120.0f, static_cast<float>(maximum + 200u)), 1.0f);
          application.SendNotification();
          application.Render();
          DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
          for(auto& task : tasks)
          {
            auto* loading = static_cast<Ui::Internal::TextLoadingTask*>(task.Get());
            loading->Process();
            auto manager = Text::AsyncTextManager::Get();
            Text::GetImplementation(manager).LoadComplete(Ui::Internal::TextLoadingTaskPtr(loading));
          }
          application.SendNotification();
          application.Render();
          application.SendNotification();
          application.Render();
          // A completed Label request restores the existing fitted-layout policy.
          DALI_TEST_CHECK(label.GetRendererCount() > 1u);
          for(uint32_t tile = 0u; tile < label.GetRendererCount(); ++tile)
          {
            auto renderer = label.GetRendererAt(tile);
            DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(renderer.GetPropertyIndex("visualTransformUseEffectiveScale")), 0.0f, TEST_LOCATION);
          }
          DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
        }
        visual.Reset();
        label.Unparent();
        label.Reset();
        captured.Drain(application);
        application.RunIdles();
        DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
      }
    }
  }
  END_TEST;
}

int UtcDaliLabelGradientHeightTilingCustomTransformP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  UiScaleRestore    restore;
  auto              scaleManager = restore.manager;
  scaleManager.SetScalable(true);
  const float          height = 2.0f * static_cast<float>(Dali::GetMaxTextureSize()) + 1.0f;
  Ui::Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
  for(float scale : {0.5f, 1.0f, 1.25f, 2.0f})
  {
    scaleManager.SetUiScale(scale);
    for(unsigned feature = 0u; feature < 3u; ++feature)
    {
      Ui::Label label = Ui::Label::New("Tile\nTile\nTile\nTile");
      Ui::Extension::View::SetUiScalePolicy(label, Ui::Extension::UiScalePolicy::ENABLED);
      label.SetMultiLine(true);
      label.SetRequestedWidth(180.0f / scale);
      label.SetRequestedHeight(height / scale);
      if(feature == 1u)
      {
        auto builder = Text::StyledTextBuilder::New("Tile\nTile\nTile\nTile");
        builder.SetSpan(Text::GradientSpan::New(gradient), 0u, builder.GetUtf32Length());
        label.SetStyledText(builder.Build());
      }
      if(feature == 2u)
      {
        label.SetTextGradient(gradient);
        label.SetTextGradientOverlay(gradient);
      }
      application.GetScene().Add(label);
      captured.Drain(application);
      auto  visual = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label)).GetVisual(Text::LabelPropertyIndex::TEXT);
      auto& impl   = Ui::GetImplementation(visual).GetVisualObject();
      impl.SetFittingModeRequired(false);
      for(unsigned mode = 0u; mode < 3u; ++mode)
      {
        namespace TransformProperty = Ui::Integration::Visual::Transform::Property;
        const Vector2 size          = mode == 1u ? Vector2(0.75f, 1.0f) : Vector2(140.0f, height / scale);
        const Vector2 sizeMode      = mode == 1u ? Vector2::ZERO : Vector2::ONE;
        // Binary-exact offsets avoid fixture rounding beyond the absolute tolerance at large caps.
        const Vector2 offset     = mode == 2u ? Vector2(0.125f, 0.03125f) : Vector2(13.0f, 9.0f);
        const Vector2 offsetMode = mode == 2u ? Vector2::ZERO : Vector2::ONE;
        const Vector2 extra      = mode == 2u ? Vector2(4.0f, 8.0f) : Vector2::ZERO;
        const bool    centered   = mode == 2u;
        Property::Map map;
        map.Add(TransformProperty::SIZE, size).Add(TransformProperty::SIZE_POLICY, sizeMode).Add(TransformProperty::OFFSET, offset).Add(TransformProperty::OFFSET_POLICY, offsetMode).Add(TransformProperty::EXTRA_SIZE, extra).Add(TransformProperty::ORIGIN, static_cast<int>(Ui::VisualOrigin::TOP_LEFT)).Add(TransformProperty::PIVOT, static_cast<int>(centered ? Ui::VisualPivot::CENTER : Ui::VisualPivot::TOP_LEFT));
        impl.SetTransformMapUsageForFittingMode(false);
        impl.SetTransformAndSize(map, Vector2(180.0f, height), scale);
        application.SendNotification();
        application.Render();
        application.SendNotification();
        application.Render();
        DALI_TEST_CHECK(label.GetRendererCount() > 1u);
        const Vector3 actorSize = label.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
        const Vector2 controlSize(actorSize.x, actorSize.y);
        Vector2       fullSize, fullOffset;
        for(unsigned axis = 0u; axis < 2u; ++axis)
        {
          fullSize[axis]   = size[axis] * (sizeMode[axis] == 1.0f ? scale : controlSize[axis]) + extra[axis] * scale;
          fullOffset[axis] = offset[axis] * (offsetMode[axis] == 1.0f ? scale : controlSize[axis]);
        }
        double rasterHeight = 0.0;
        for(uint32_t tile = 0u; tile < label.GetRendererCount(); ++tile)
        {
          rasterHeight += label.GetRendererAt(tile).GetTextures().GetTexture(0u).GetHeight();
        }
        double rasterTop = 0.0;
        for(uint32_t tile = 0u; tile < label.GetRendererCount(); ++tile)
        {
          auto          renderer   = label.GetRendererAt(tile);
          const Vector2 tileSize   = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
          const Vector2 tileOffset = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET);
          const Vector2 tileExtra  = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::EXTRA_SIZE);
          const Vector2 pivot      = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_PIVOT);
          const Vector4 modes      = renderer.GetCurrentProperty<Vector4>(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE);
          const double  tileHeight = renderer.GetTextures().GetTexture(0u).GetHeight();
          for(unsigned axis = 0u; axis < 2u; ++axis)
          {
            const float extent         = tileSize[axis] * (modes[axis + 2u] == 1.0f ? scale : controlSize[axis]) + tileExtra[axis] * scale;
            const float origin         = tileOffset[axis] * (modes[axis] == 1.0f ? scale : controlSize[axis]);
            const float first          = (-0.5f + pivot[axis]) * extent + origin;
            const float expectedFirst  = static_cast<float>(fullOffset[axis] - (centered ? 0.5f * fullSize[axis] : 0.0f) + (axis == 1u ? fullSize.y * rasterTop / rasterHeight : 0.0));
            const float expectedExtent = static_cast<float>(axis == 1u ? fullSize.y * tileHeight / rasterHeight : fullSize.x);
            DALI_TEST_EQUALS(first, expectedFirst, 0.001f, TEST_LOCATION);
            DALI_TEST_EQUALS(extent, expectedExtent, 0.001f, TEST_LOCATION);
          }
          rasterTop += tileHeight;
        }
      }
      label.Unparent();
    }
  }
  END_TEST;
}

int UtcDaliLabelGradientHeightTilingCallbackLifetimeP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  for(unsigned action = 0u; action < 3u; ++action)
  {
    std::string lines;
    for(uint32_t i = 0u; i < Dali::GetMaxTextureSize() / 4u + 100u; ++i)
    {
      lines += "Tile\n";
    }
    Ui::Label label = Ui::Label::New(lines.c_str());
    label.SetMultiLine(true);
    label.SetAsyncRendering(true);
    label.SetRequestedWidth(120.0f);
    label.SetRequestedHeight(2.0f * static_cast<float>(Dali::GetMaxTextureSize()) + 1.0f);
    application.GetScene().Add(label);
    captured.Drain(application);
    DALI_TEST_CHECK(label.GetRendererCount() > 1u);
    auto                  visual = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label)).GetVisual(Text::LabelPropertyIndex::TEXT);
    auto&                 impl   = Ui::GetImplementation(visual).GetVisualObject();
    auto                  extra  = label.GetRendererAt(1u);
    WeakHandle<Renderer>  retired(extra);
    WeakHandle<Ui::Label> owner(label);
    const auto            policy = extra.GetPropertyIndex("visualTransformUseEffectiveScale");
    ConnectionTracker     tracker;
    bool                  fired = false;
    extra.PropertySetSignal().Connect(&tracker, [&](Handle, Property::Index index, const Property::Value&)
    {
      if(index == policy && !fired)
      {
        fired = true;
        if(action == 2u)
        {
          label.SetAsyncRendering(false);
          label.SetRequestedHeight(40.0f);
        }
        else
        {
          label.Unparent();
          if(action == 1u)
          {
            label.Reset();
          }
        }
      }
    });
    impl.SetTransformMapUsageForFittingMode(false);
    impl.SetTransformAndSize(Property::Map{}, Vector2(120.0f, 2.0f * static_cast<float>(Dali::GetMaxTextureSize()) + 1.0f), 1.0f);
    tracker.DisconnectAll();
    extra.Reset();
    captured.Drain(application);
    application.RunIdles();
    DALI_TEST_CHECK(fired);
    DALI_TEST_CHECK(!retired.GetHandle());
    if(action == 1u)
    {
      DALI_TEST_CHECK(!owner.GetHandle());
    }
    else if(action == 2u)
    {
      DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
    }
    else
    {
      DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);
      application.GetScene().Add(label);
      captured.Drain(application);
      DALI_TEST_CHECK(label.GetRendererCount() > 1u);
    }
    if(label)
    {
      label.Unparent();
    }
  }
  END_TEST;
}

int UtcDaliLabelGradientHeightTilingCreationReentryP(void)
{
  UiTestApplication application;
  CapturedTasks     captured;
  struct ReentryCase
  {
    Property::Index property;
    uint32_t        occurrence;
  };
  const ReentryCase cases[] = {
    {Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, 1u},
    {VisualRenderer::Property::TRANSFORM_SIZE, 1u},
    {VisualRenderer::Property::TRANSFORM_PIVOT, 2u},
    {VisualRenderer::Property::EXTRA_SIZE, 2u},
    {VisualRenderer::Property::TRANSFORM_SIZE, 2u},
    {VisualRenderer::Property::TRANSFORM_OFFSET, 2u},
    {Renderer::Property::BLEND_MODE, 1u},
    {Property::INVALID_INDEX, 1u}}; // Resolve the existing uHasMultipleTextColors index per renderer.
  for(bool gradientEnabled : {false, true})
  {
    for(unsigned action = 0u; action < 3u; ++action)
    {
      for(const auto& reentry : cases)
      {
        std::string lines;
        for(uint32_t i = 0u; i < Dali::GetMaxTextureSize() / 4u + 100u; ++i)
        {
          lines += "Tile\n";
        }
        Ui::Label label = Ui::Label::New(lines.c_str());
        label.SetAsyncRendering(false);
        label.SetMultiLine(true);
        label.SetRequestedWidth(120.0f);
        label.SetRequestedHeight(2.0f * static_cast<float>(Dali::GetMaxTextureSize()) + 1.0f);
        if(gradientEnabled)
        {
          Ui::Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
          gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
          label.SetTextGradient(gradient);
        }
        application.GetScene().Add(label);
        captured.Drain(application);
        DALI_TEST_CHECK(label.GetRendererCount() > 1u);
        auto              visual = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label)).GetVisual(Text::LabelPropertyIndex::TEXT);
        auto&             impl   = Ui::GetImplementation(visual).GetVisualObject();
        auto              first  = label.GetRendererAt(0u);
        ConnectionTracker tracker;
        bool              fired    = false;
        bool              building = false;
        uint32_t          writes   = 0u;
        const auto        trigger  = reentry.property == Property::INVALID_INDEX ? first.GetPropertyIndex("uHasMultipleTextColors") : reentry.property;
        DALI_TEST_CHECK(trigger != Property::INVALID_INDEX);
        first.PropertySetSignal().Connect(&tracker, [&](Handle, Property::Index index, const Property::Value&)
        {
          // Ignore property writes outside CreateTextureSet. The second transform
          // write is the cropped tile value, after the shared transform was applied.
          if(index == Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA)
          {
            building = true;
          }
          if(building && index == trigger && ++writes == reentry.occurrence && !fired)
          {
            fired                     = true;
            auto*          owner      = &label;
            auto*          target     = &impl;
            const unsigned nextAction = action;
            tracker.DisconnectAll();
            if(nextAction < 2u)
            {
              owner->Unparent();
              if(nextAction == 1u)
              {
                owner->Reset();
              }
            }
            else
            {
              Property::Map next;
              next.Add(Ui::Integration::Visual::Transform::Property::SIZE, Vector2(100.0f, 50.0f));
              next.Add(Ui::Integration::Visual::Transform::Property::SIZE_POLICY, Vector2::ONE);
              next.Add(Ui::Integration::Visual::Transform::Property::EXTRA_SIZE, Vector2::ZERO);
              target->SetTransformAndSize(next, Vector2(180.0f, 1100.0f), 1.0f);
            }
          }
        });
        impl.SetFittingModeRequired(false);
        impl.SetTransformMapUsageForFittingMode(false);
        Property::Map map;
        map.Add(Ui::Integration::Visual::Transform::Property::SIZE, Vector2(140.0f, 2.0f * static_cast<float>(Dali::GetMaxTextureSize()) + 10.0f));
        map.Add(Ui::Integration::Visual::Transform::Property::SIZE_POLICY, Vector2::ONE);
        map.Add(Ui::Integration::Visual::Transform::Property::OFFSET_POLICY, Vector2::ZERO);
        map.Add(Ui::Integration::Visual::Transform::Property::EXTRA_SIZE, Vector2(4.0f, 8.0f));
        impl.SetTransformAndSize(map, Vector2(180.0f, 2.0f * static_cast<float>(Dali::GetMaxTextureSize()) + 10.0f), 1.0f);
        DALI_TEST_CHECK(fired);
        if(action == 0u)
        {
          DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);
          application.GetScene().Add(label);
          captured.Drain(application);
          DALI_TEST_CHECK(label.GetRendererCount() > 1u);
        }
        else if(action == 1u)
        {
          DALI_TEST_CHECK(!label);
          DALI_TEST_CHECK(!first.GetTextures());
        }
        else
        {
          DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
          DALI_TEST_EQUALS(first.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(100.0f, 50.0f), TEST_LOCATION);
          DALI_TEST_EQUALS(first.GetTextures().GetTexture(0u).GetHeight(), 50u, TEST_LOCATION);
        }
        if(label)
        {
          label.Unparent();
        }
      }
    }
  }
  END_TEST;
}

namespace
{
int CheckGradientTileUniformReentry(bool asynchronous, bool rebind)
{
  UiTestApplication application;
  CapturedTasks     captured;
  for(bool overlay : {false, true})
  {
    for(unsigned action = 0u; action < 3u; ++action)
    {
      for(const char* suffix : {"StartPosition", "EvaluationScale", "Bounds"})
      {
        std::string lines;
        for(unsigned i = 0u; i < Dali::GetMaxTextureSize() / 4u + 100u; ++i)
        {
          lines += "Tile\n";
        }
        Ui::Label label = Ui::Label::New(lines.c_str());
        label.SetAsyncRendering(asynchronous);
        label.SetMultiLine(true);
        label.SetRequestedWidth(120.0f);
        label.SetRequestedHeight(2.0f * static_cast<float>(Dali::GetMaxTextureSize()) + 1.0f);
        Ui::Gradient::Linear gradient(Vector2(12.0f, 80.0f), Vector2(80.0f, 850.0f));
        gradient.SetUnits(Ui::Gradient::Units::USER_SPACE);
        gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
        if(overlay)
        {
          label.SetTextGradientOverlay(gradient);
        }
        else
        {
          label.SetTextGradient(gradient);
        }
        application.GetScene().Add(label);
        captured.Drain(application);
        DALI_TEST_CHECK(label.GetRendererCount() > 1u);
        auto              visual = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label)).GetVisual(Text::LabelPropertyIndex::TEXT);
        auto&             impl   = Ui::GetImplementation(visual).GetVisualObject();
        auto              first  = label.GetRendererAt(0u);
        auto              target = label.GetRendererAt(rebind ? 1u : 0u);
        const std::string prefix = overlay ? "uTextGradientOverlay" : "uTextGradient";
        const auto        index  = target.GetPropertyIndex((prefix + suffix).c_str());
        DALI_TEST_CHECK(index != Property::INVALID_INDEX);
        ConnectionTracker tracker;
        Property::Map     expected;
        bool              fired = false;
        target.PropertySetSignal().Connect(&tracker, [&](Handle, Property::Index changed, const Property::Value&)
        {
          if(changed != index || fired)
          {
            return;
          }
          fired                     = true;
          auto*      owner          = &label;
          auto*      nextVisual     = &impl;
          auto*      properties     = &expected;
          const auto nextAction     = action;
          auto       renderer       = first;
          const auto propertyPrefix = prefix;
          tracker.DisconnectAll();
          if(nextAction < 2u)
          {
            owner->Unparent();
            if(nextAction == 1u)
            {
              owner->Reset();
            }
          }
          else
          {
            owner->SetAsyncRendering(false);
            Property::Map next;
            next.Add(Ui::Integration::Visual::Transform::Property::SIZE, Vector2(100.0f, 50.0f));
            next.Add(Ui::Integration::Visual::Transform::Property::SIZE_POLICY, Vector2::ONE);
            nextVisual->SetTransformAndSize(next, Vector2(180.0f, 1100.0f), 1.0f);
            for(const char* name : {"StartPosition", "EndPosition", "EvaluationScale", "Bounds"})
            {
              const auto propertyName = propertyPrefix + name;
              properties->Insert(propertyName.c_str(), renderer.GetProperty(renderer.GetPropertyIndex(propertyName.c_str())));
            }
          }
        });
        impl.SetFittingModeRequired(false);
        impl.SetTransformMapUsageForFittingMode(false);
        if(rebind)
        {
          const auto source = label.RegisterProperty("tileReentryOffset", 0.25f);
          if(overlay)
          {
            Ui::Internal::TextVisual::SetGradientOverlayAnimProperties(visual, source);
          }
          else
          {
            Ui::Internal::TextVisual::SetGradientAnimProperties(visual, source);
          }
        }
        else if(asynchronous)
        {
          label.SetRequestedWidth(140.0f);
          application.SendNotification();
          application.Render();
          auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
          DALI_TEST_CHECK(!tasks.empty());
          for(auto& task : tasks)
          {
            auto* loading = static_cast<Ui::Internal::TextLoadingTask*>(task.Get());
            DALI_TEST_CHECK(loading->IsReady());
            loading->Process();
            auto manager = Text::AsyncTextManager::Get();
            Text::GetImplementation(manager).LoadComplete(Ui::Internal::TextLoadingTaskPtr(loading));
          }
          // Inspect the completion before ordinary relayout can replace the custom transform.
        }
        else
        {
          Property::Map map;
          map.Add(Ui::Integration::Visual::Transform::Property::SIZE, Vector2(140.0f, 2.0f * static_cast<float>(Dali::GetMaxTextureSize()) + 10.0f));
          map.Add(Ui::Integration::Visual::Transform::Property::SIZE_POLICY, Vector2::ONE);
          impl.SetTransformAndSize(map, Vector2(180.0f, 2.0f * static_cast<float>(Dali::GetMaxTextureSize()) + 10.0f), 1.0f);
        }
        DALI_TEST_CHECK(fired);
        if(action == 1u)
        {
          DALI_TEST_CHECK(!label);
          DALI_TEST_CHECK(!first.GetTextures());
        }
        else if(action == 0u)
        {
          DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);
          application.GetScene().Add(label);
          captured.Drain(application);
          DALI_TEST_CHECK(label.GetRendererCount() > 1u);
        }
        else
        {
          DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
          DALI_TEST_EQUALS(first.GetTextures().GetTexture(0u).GetHeight(), 50u, TEST_LOCATION);
          for(unsigned i = 0u; i < expected.Count(); ++i)
          {
            const auto property = expected.GetKeyValue(i);
            DALI_TEST_CHECK(first.GetProperty(first.GetPropertyIndex(property.first)) == property.second);
          }
        }
        if(label)
        {
          label.Unparent();
        }
        visual.Reset();
        captured.Drain(application);
        application.RunIdles();
      }
    }
  }
  END_TEST;
}
} // namespace

int UtcDaliLabelGradientHeightTilingSyncUniformReentryP(void)
{
  return CheckGradientTileUniformReentry(false, false);
}

int UtcDaliLabelGradientHeightTilingAsyncUniformReentryP(void)
{
  return CheckGradientTileUniformReentry(true, false);
}

int UtcDaliLabelGradientHeightTilingRebindUniformReentryP(void)
{
  return CheckGradientTileUniformReentry(false, true);
}
