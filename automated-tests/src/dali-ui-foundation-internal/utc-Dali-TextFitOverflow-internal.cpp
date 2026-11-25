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
// Install adaptor mocks before production headers.
#include <dali-ui/ui-async-task-manager.h>
#include <dali-ui/ui-event-thread-callback.h>

// Resolve task types before other headers introduce Internal::Text.
#include <dali-ui-foundation/internal/text/async-text/text-loading-task.h>

// Test utilities and production APIs.
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>
#include <dali-ui-foundation/internal/text/text-view.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/text/styled-text/font-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <cstring>
#include <initializer_list>
#include <utility>

using namespace Dali;
namespace
{
namespace Text = Ui::Text;
using Task     = Ui::Internal::TextLoadingTaskPtr;

Task Run(Text::AsyncTextParameters parameters)
{
  auto loader = Text::AsyncTextLoader::New();
  auto task   = Ui::Internal::TextLoadingTask::New(1u, std::move(parameters), nullptr);
  task->SetLoader(loader);
  task->Process();
  return task;
}

bool EqualPixels(PixelData actual, PixelData expected)
{
  if(!actual || !expected || actual.GetWidth() != expected.GetWidth() ||
     actual.GetHeight() != expected.GetHeight() || actual.GetPixelFormat() != expected.GetPixelFormat())
  {
    return false;
  }
  const auto a = Integration::GetPixelDataBuffer(actual);
  const auto b = Integration::GetPixelDataBuffer(expected);
  return a.bufferSize == b.bufferSize &&
         (a.bufferSize == 0u || (a.buffer && b.buffer && std::memcmp(a.buffer, b.buffer, a.bufferSize) == 0));
}

Text::AsyncTextParameters Parameters(bool candidates, bool vertical, int scenario)
{
  Text::AsyncTextParameters p;
  p.requestType                = Ui::Integration::Text::Async::RENDER_FIXED_SIZE;
  p.text                       = vertical ? "One\nTwo\nThree\nFour" : "Text must fit";
  p.fontFamily                 = "DejaVu Sans";
  p.fontSize                   = 24.f;
  p.textWidth                  = vertical ? 220.f : scenario == 0 ? 400.f
                                                  : scenario == 1 ? 110.f
                                                                  : 35.f;
  p.textHeight                 = vertical ? (scenario == 0 ? 250.f : scenario == 1 ? 90.f
                                                                                   : 25.f)
                                          : 80.f;
  p.isMultiLine                = vertical;
  p.isTextFitEnabled           = !candidates;
  p.isTextFitCandidatesEnabled = candidates;
  p.textFitMinSize             = 9.f;
  p.textFitMaxSize             = 24.f;
  p.textFitStepSize            = 1.5f;
  p.textFitCandidates.PushBack(Text::Fit::Candidate(12.f, 16.f));
  p.textFitCandidates.PushBack(Text::Fit::Candidate(22.f, 28.f));
  p.textFitCandidates.PushBack(Text::Fit::Candidate(32.f, 40.f));
  p.marqueeTriggerPolicy = Text::MarqueeTriggerPolicy::ON_OVERFLOW;
  p.marqueeOrientation   = vertical ? Text::MarqueeOrientation::VERTICAL : Text::MarqueeOrientation::HORIZONTAL;
  p.maxTextureSize       = 4096;
  p.marqueeGap           = 20;
  p.ellipsis             = true;
  p.relativeLineSize     = -1.f; // Label defaults to LINE_HEIGHT_AUTO.
  return p;
}

void CheckWorker(bool candidates, bool vertical, int scenario, float scale, float quality)
{
  auto p               = Parameters(candidates, vertical, scenario);
  p.effectiveTextScale = scale;
  p.renderScale        = quality;
  p.textWidth *= scale;
  p.textHeight *= scale;
  auto referenceParameters                 = p;
  referenceParameters.marqueeTriggerPolicy = Text::MarqueeTriggerPolicy::MANUAL;
  auto        reference                    = Run(referenceParameters);
  auto        task                         = Run(p);
  const auto& selected                     = task->mParameters;
  const auto& result                       = task->mRenderInfo;
  const bool  overflow                     = scenario == 2;
  tet_printf("fit=%d vertical=%d scenario=%d scale=%f quality=%f selected=%f", candidates, vertical, scenario, scale, quality, selected.fontSize);
  DALI_TEST_EQUALS(selected.fontSize, reference->mParameters.fontSize, .02f, TEST_LOCATION);
  DALI_TEST_EQUALS(selected.minLineSize, reference->mParameters.minLineSize, .02f, TEST_LOCATION);
  DALI_TEST_EQUALS(selected.isMarqueeEnabled, overflow, TEST_LOCATION);
  if(scenario == 0)
  {
    DALI_TEST_EQUALS(selected.fontSize, 24.f * scale, .02f, TEST_LOCATION);
  }
  if(scenario == 1)
  {
    DALI_TEST_CHECK(selected.fontSize < 24.f * scale && selected.fontSize >= 9.f * scale);
  }
  if(scenario == 2)
  {
    DALI_TEST_EQUALS(selected.fontSize, 9.f * scale, .02f, TEST_LOCATION);
  }
  if(!overflow)
  {
    DALI_TEST_CHECK(!result.marqueePixelData);
    DALI_TEST_CHECK(EqualPixels(result.textPixelData, reference->mRenderInfo.textPixelData));
  }
  else
  {
    DALI_TEST_CHECK(result.marqueePixelData);
    // The established marquee publication also carries its ordinary text plane.
    DALI_TEST_CHECK(result.textPixelData);
    // Fresh full-source raster using the chosen font and line height is an
    // independent check against reusing the pre-Fit model or ellipsized glyphs.
    auto full = selected;
    if(quality == 1.f)
    {
      // RenderMarquee restores integer request dimensions after rasterization.
      // Keep the original fractional viewport for the fresh reference.
      full.textWidth  = p.textWidth;
      full.textHeight = p.textHeight;
    }
    full.marqueeTriggerPolicy       = Text::MarqueeTriggerPolicy::MANUAL;
    full.isTextFitEnabled           = false;
    full.isTextFitCandidatesEnabled = false;
    full.fontSize /= scale;
    if(candidates)
    {
      full.minLineSize *= quality;
    }
    auto loader   = Text::AsyncTextLoader::New();
    auto expected = loader.RenderMarquee(full, false, Size::ZERO);
    DALI_TEST_CHECK(EqualPixels(result.marqueePixelData, expected.marqueePixelData));
    DALI_TEST_EQUALS(result.controlSize, expected.controlSize, .02f, TEST_LOCATION);
  }
}

void CheckStaticFitResult(const Task& actual, const Task& expected)
{
  DALI_TEST_EQUALS(actual->mParameters.fontSize, expected->mParameters.fontSize, .02f, TEST_LOCATION);
  DALI_TEST_EQUALS(actual->mParameters.minLineSize, expected->mParameters.minLineSize, .02f, TEST_LOCATION);
  DALI_TEST_EQUALS(actual->mRenderInfo.lineCount, expected->mRenderInfo.lineCount, TEST_LOCATION);
  DALI_TEST_EQUALS(actual->mRenderInfo.renderedSize, expected->mRenderInfo.renderedSize, .02f, TEST_LOCATION);
  DALI_TEST_EQUALS(bool(actual->mRenderInfo.textPixelData), bool(expected->mRenderInfo.textPixelData), TEST_LOCATION);
  if(expected->mRenderInfo.textPixelData)
  {
    DALI_TEST_CHECK(EqualPixels(actual->mRenderInfo.textPixelData, expected->mRenderInfo.textPixelData));
  }
  DALI_TEST_CHECK(!actual->mRenderInfo.marqueePixelData);
}

void FlushFitAudit(UiTestApplication& app)
{
  app.SendNotification();
  app.UpdateOnly(0);
  app.GetCore().PostRender();
  app.SendNotification();
}

Text::ControllerPtr AuditController(Ui::Label label)
{
  auto visual = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label)).GetVisual(Text::LabelPropertyIndex::TEXT);
  return Ui::Internal::TextVisual::GetController(visual);
}

Ui::Label AuditLabel(const Text::AsyncTextParameters& p)
{
  auto label = Ui::Label::New(p.text.c_str());
  label.SetRequestedWidth(p.textWidth);
  label.SetRequestedHeight(p.textHeight);
  label.SetFontFamily(p.fontFamily.c_str());
  label.SetFontSize(32.f);
  label.SetMultiLine(p.isMultiLine);
  label.SetMaximumLines(p.maximumNumberOfLines);
  label.SetTextOverflowMode(p.ellipsis ? Text::OverflowMode::ELLIPSIS : Text::OverflowMode::CLIP);
  if(p.isTextFitCandidatesEnabled)
  {
    label.SetTextFit(p.textFitCandidates);
  }
  else
  {
    label.SetTextFit(Text::Fit::Range(12.f, 32.f, 2.f));
  }
  label.SetMarqueeOrientation(p.marqueeOrientation);
  label.SetMarqueeTriggerPolicy(p.marqueeTriggerPolicy);
  label.SetMarqueeLoopCount(0);
  return label;
}
} // namespace

int UtcDaliTextFitOverflowAuditMaximumLinesP(void)
{
  UiTestApplication app;
  for(bool candidates : {false, true})
  {
    for(bool ellipsis : {false, true})
    {
      for(unsigned cap : {0u, 1u, 2u, 3u})
      {
        for(int scenario : {0, 1, 2, 3})
        {
          auto p                 = Parameters(candidates, true, 0);
          p.maximumNumberOfLines = cap;
          p.ellipsis             = ellipsis;
          // At 25px a capped line fits, but four full lines do not. At 500px
          // only the line limit fails, so AUTO must restore capped static text.
          p.textHeight = scenario == 0 ? 25.f : 500.f;
          if(scenario == 2)
          {
            p.text = "OK";
          }
          if(scenario == 3)
          {
            p.text = "One\nTwo\nThree\n";
          }
          auto       actual   = Run(p);
          const bool overflow = scenario == 0;
          DALI_TEST_EQUALS(actual->mParameters.maximumNumberOfLines, cap, TEST_LOCATION);
          DALI_TEST_EQUALS(actual->mParameters.isMarqueeEnabled, overflow, TEST_LOCATION);
          if(cap && scenario != 2)
          {
            // Exceeding the line limit fails Fit even if capped height fits.
            DALI_TEST_EQUALS(actual->mParameters.fontSize, 9.f, .02f, TEST_LOCATION);
          }
          if(overflow)
          {
            auto full                 = actual->mParameters;
            full.maximumNumberOfLines = 0u;
            full.marqueeTriggerPolicy = Text::MarqueeTriggerPolicy::MANUAL;
            full.isTextFitEnabled = full.isTextFitCandidatesEnabled = false;
            auto reference                                          = Run(full);
            DALI_TEST_EQUALS(actual->mRenderInfo.lineCount, 4, TEST_LOCATION);
            DALI_TEST_CHECK(EqualPixels(actual->mRenderInfo.marqueePixelData, reference->mRenderInfo.marqueePixelData));
          }
          else
          {
            auto reference                 = p;
            reference.marqueeTriggerPolicy = Text::MarqueeTriggerPolicy::MANUAL;
            CheckStaticFitResult(actual, Run(reference));
            reference.isTextFitEnabled = reference.isTextFitCandidatesEnabled = false;
            reference.fontSize                                                = actual->mParameters.fontSize;
            reference.minLineSize                                             = actual->mParameters.minLineSize;
            CheckStaticFitResult(actual, Run(reference));
          }
          auto label = AuditLabel(p);
          app.GetScene().Add(label);
          FlushFitAudit(app);
          auto controller = AuditController(label);
          tet_printf("audit cap=%u candidates=%d ellipsis=%d scenario=%d selected=%f async=%d sync=%d", cap, candidates, ellipsis, scenario, actual->mParameters.fontSize, actual->mParameters.isMarqueeEnabled, label.IsMarqueeRunning());
          DALI_TEST_EQUALS(controller->GetTextFitFontSize(Text::Controller::POINT_SIZE), actual->mParameters.fontSize, .02f, TEST_LOCATION);
          DALI_TEST_EQUALS(label.IsMarqueeRunning(), overflow, TEST_LOCATION);
          label.Unparent();
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTextFitOverflowAuditOtherAxisStaticP(void)
{
  UiTestApplication app;
  for(bool candidates : {false, true})
  {
    for(bool vertical : {false, true})
    {
      for(bool ellipsis : {false, true})
      {
        for(unsigned cap : {0u, 1u, 2u, 3u})
        {
          auto p                         = Parameters(candidates, vertical, 0);
          p.maximumNumberOfLines         = cap;
          p.ellipsis                     = ellipsis;
          p.textWidth                    = vertical ? 1.f : 500.f;
          p.textHeight                   = vertical ? 500.f : 1.f;
          auto actual                    = Run(p);
          auto reference                 = p;
          reference.marqueeTriggerPolicy = Text::MarqueeTriggerPolicy::MANUAL;
          DALI_TEST_CHECK(!actual->mParameters.isMarqueeEnabled);
          DALI_TEST_EQUALS(actual->mParameters.maximumNumberOfLines, cap, TEST_LOCATION);
          DALI_TEST_EQUALS(actual->mParameters.fontSize, 9.f, .02f, TEST_LOCATION);
          CheckStaticFitResult(actual, Run(reference));
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTextFitOverflowAuditEllipsisSnapshotP(void)
{
  UiTestApplication app;
  for(bool candidates : {false, true})
  {
    for(bool vertical : {false, true})
    {
      for(bool ellipsis : {false, true})
      {
        for(int scenario : {0, 1, 2, 3, 4, 5})
        {
          auto p                 = Parameters(candidates, vertical, scenario < 3 ? scenario : 0);
          p.ellipsis             = ellipsis;
          p.maximumNumberOfLines = scenario == 3 ? 1u : 0u;
          if(scenario == 4)
          {
            p.suppressAutoMarquee = true;
          }
          if(scenario == 5)
          {
            p.text.clear();
          }
          auto actual = Run(p);
          DALI_TEST_EQUALS(actual->mParameters.ellipsis, ellipsis, TEST_LOCATION);
          DALI_TEST_EQUALS(actual->mParameters.maximumNumberOfLines, p.maximumNumberOfLines, TEST_LOCATION);
          if(scenario == 0 || scenario == 1 || scenario == 4)
          {
            auto reference                 = p;
            reference.marqueeTriggerPolicy = Text::MarqueeTriggerPolicy::MANUAL;
            CheckStaticFitResult(actual, Run(reference));
          }
        }
      }
    }
  }
  auto invalid             = Parameters(false, true, 0);
  invalid.isTextFitEnabled = false;
  auto loader              = Text::AsyncTextLoader::New();
  auto empty               = loader.RenderTextFit(invalid, false, Size::ZERO);
  DALI_TEST_CHECK(!empty.textPixelData);
  DALI_TEST_CHECK(invalid.ellipsis);
  END_TEST;
}

int UtcDaliTextFitOverflowAuditControllerCacheP(void)
{
  UiTestApplication app;
  for(bool candidates : {false, true})
  {
    auto p                 = Parameters(candidates, true, 0);
    p.textHeight           = 500.f;
    p.maximumNumberOfLines = 1u;
    auto label             = AuditLabel(p);
    app.GetScene().Add(label);
    FlushFitAudit(app);
    auto controller = AuditController(label);
    for(unsigned cap : {1u, 0u, 1u, 2u, 3u, 1u})
    {
      for(float width : {220.f, 80.f, 220.f})
      {
        label.SetMaximumLines(cap);
        label.SetRequestedWidth(width);
        FlushFitAudit(app);
        auto freshParameters                 = p;
        freshParameters.maximumNumberOfLines = cap;
        freshParameters.textWidth            = width;
        auto fresh                           = AuditLabel(freshParameters);
        app.GetScene().Add(fresh);
        FlushFitAudit(app);
        auto  expected   = AuditController(fresh);
        float fittedSize = controller->GetTextFitFontSize(Text::Controller::POINT_SIZE);
        float capped     = expected->GetHeightForWidth(width);
        float full       = controller->GetHeightForWidth(width, true);
        DALI_TEST_CHECK(full >= capped);
        for(unsigned repeat = 0u; repeat < 3u; ++repeat)
        {
          DALI_TEST_EQUALS(controller->GetHeightForWidth(width), capped, .02f, TEST_LOCATION);
          DALI_TEST_EQUALS(controller->GetHeightForWidth(width, true), full, .02f, TEST_LOCATION);
        }
        controller->Relayout(Size(width, p.textHeight), LayoutDirection::LEFT_TO_RIGHT);
        expected->Relayout(Size(width, p.textHeight), LayoutDirection::LEFT_TO_RIGHT);
        DALI_TEST_EQUALS(controller->GetView().GetLayoutSize(), expected->GetView().GetLayoutSize(), .02f, TEST_LOCATION);
        DALI_TEST_EQUALS(controller->GetView().GetNumberOfLines(), expected->GetView().GetNumberOfLines(), TEST_LOCATION);
        DALI_TEST_EQUALS(controller->GetTextFitFontSize(Text::Controller::POINT_SIZE), fittedSize, .02f, TEST_LOCATION);
        Dali::Vector<Text::GlyphInfo> glyphs, referenceGlyphs;
        Dali::Vector<Vector2>         positions, referencePositions;
        auto                          count = controller->GetView().GetNumberOfGlyphs();
        DALI_TEST_EQUALS(count, expected->GetView().GetNumberOfGlyphs(), TEST_LOCATION);
        glyphs.Resize(count);
        referenceGlyphs.Resize(count);
        positions.Resize(count);
        referencePositions.Resize(count);
        float offset = 0.f, referenceOffset = 0.f;
        auto  visible          = controller->GetView().GetGlyphs(glyphs.Begin(), positions.Begin(), offset, 0u, count);
        auto  referenceVisible = expected->GetView().GetGlyphs(referenceGlyphs.Begin(), referencePositions.Begin(), referenceOffset, 0u, count);
        DALI_TEST_EQUALS(visible, referenceVisible, TEST_LOCATION);
        for(unsigned i = 0u; i < visible; ++i)
        {
          DALI_TEST_EQUALS(glyphs[i].index, referenceGlyphs[i].index, TEST_LOCATION);
          DALI_TEST_EQUALS(positions[i], referencePositions[i], .02f, TEST_LOCATION);
        }
        fresh.Unparent();
      }
    }
    label.Unparent();
  }
  END_TEST;
}

int UtcDaliTextFitOverflowWorkerMatrixP(void)
{
  UiTestApplication application;
  for(bool candidates : {false, true})
  {
    for(bool vertical : {false, true})
    {
      for(int scenario : {0, 1, 2})
      {
        for(float scale : {1.f, 1.5f})
        {
          for(float quality : {1.f, 1.5f})
          {
            CheckWorker(candidates, vertical, scenario, scale, quality);
          }
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTextFitOverflowSuppressedInvalidEmptyP(void)
{
  UiTestApplication application;
  for(bool candidates : {false, true})
  {
    for(int mode : {0, 1, 2})
    {
      auto p             = Parameters(candidates, false, 2);
      p.isMarqueeEnabled = true; // Simulate a preceding MANUAL request.
      if(mode == 0)
      {
        p.suppressAutoMarquee = true;
      }
      if(mode == 1)
      {
        p.marqueeOrientation = Text::MarqueeOrientation::VERTICAL;
      }
      if(mode == 2)
      {
        p.text.clear();
      }
      auto task = Run(p);
      DALI_TEST_CHECK(!task->mParameters.isMarqueeEnabled);
      DALI_TEST_CHECK(!task->mRenderInfo.marqueePixelData);
      if(mode != 2)
      {
        DALI_TEST_EQUALS(task->mParameters.fontSize, 9.f, .02f, TEST_LOCATION);
      }
    }
  }
  // AUTO without Fit must reset the inherited enable flag as well.
  auto p             = Parameters(false, false, 0);
  p.isTextFitEnabled = false;
  p.isMarqueeEnabled = true;
  auto task          = Run(p);
  DALI_TEST_CHECK(!task->mParameters.isMarqueeEnabled);
  DALI_TEST_CHECK(task->mRenderInfo.textPixelData);
  END_TEST;
}

int UtcDaliTextFitOverflowManualSelectionP(void)
{
  UiTestApplication application;
  for(bool candidates : {false, true})
  {
    for(bool vertical : {false, true})
    {
      for(bool overflow : {false, true})
      {
        auto p                 = Parameters(candidates, vertical, overflow ? 2 : 0);
        auto automatic         = Run(p);
        p.marqueeTriggerPolicy = Text::MarqueeTriggerPolicy::MANUAL;
        p.isMarqueeEnabled     = true;
        auto manual            = Run(p);
        DALI_TEST_EQUALS(manual->mParameters.fontSize, overflow ? 9.f : 24.f, .02f, TEST_LOCATION);
        DALI_TEST_EQUALS(manual->mParameters.fontSize, automatic->mParameters.fontSize, .02f, TEST_LOCATION);
        DALI_TEST_EQUALS(manual->mParameters.minLineSize, automatic->mParameters.minLineSize, .02f, TEST_LOCATION);
        DALI_TEST_CHECK(manual->mParameters.isMarqueeEnabled);
        DALI_TEST_EQUALS(manual->mRenderInfo.lineCount, vertical ? 4 : 1, TEST_LOCATION);
        if(!vertical)
        {
          DALI_TEST_EQUALS(manual->mRenderInfo.isMarqueeContentOverflow, overflow, TEST_LOCATION);
        }
        if(overflow)
        {
          DALI_TEST_CHECK(EqualPixels(manual->mRenderInfo.marqueePixelData, automatic->mRenderInfo.marqueePixelData));
        }
        // MANUAL also renders fitting text. Compare its selected model to a
        // fresh non-Fit marquee, rather than only checking for nonempty pixels.
        auto full                       = manual->mParameters;
        full.isTextFitEnabled           = false;
        full.isTextFitCandidatesEnabled = false;
        auto reference                  = Run(full);
        DALI_TEST_CHECK(EqualPixels(manual->mRenderInfo.marqueePixelData, reference->mRenderInfo.marqueePixelData));
      }
    }
  }
  END_TEST;
}

int UtcDaliTextFitOverflowSpanScaleP(void)
{
  UiTestApplication application;
  for(bool candidates : {false, true})
  {
    for(float scale : {1.f, 1.5f})
    {
      for(bool explicitSize : {false, true})
      {
        auto                 p       = Parameters(candidates, false, 2);
        auto                 builder = Text::StyledTextBuilder::New(p.text.c_str());
        Text::FontAttributes attributes;
        attributes.SetFamily("DejaVu Serif");
        attributes.SetWeight(Text::FontWeight::BOLD);
        if(explicitSize)
        {
          attributes.SetSize(28.f);
        }
        builder.SetSpan(Text::FontSpan::New(attributes), 0u, 4u);
        p.hasStyledTextStyleSnapshot = true;
        p.styledTextStyleSnapshot    = Ui::Internal::Text::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(), 96.f);
        p.effectiveTextScale         = scale;
        auto task                    = Run(p);
        DALI_TEST_CHECK(task->mParameters.isMarqueeEnabled);
        auto full = task->mParameters;
        full.fontSize /= scale;
        full.isTextFitEnabled = full.isTextFitCandidatesEnabled = false;
        full.marqueeTriggerPolicy                               = Text::MarqueeTriggerPolicy::MANUAL;
        auto loader                                             = Text::AsyncTextLoader::New();
        auto reference                                          = loader.RenderMarquee(full, false, Size::ZERO);
        DALI_TEST_CHECK(EqualPixels(task->mRenderInfo.marqueePixelData, reference.marqueePixelData));
      }
    }
  }
  END_TEST;
}

int UtcDaliTextFitOverflowSyncAsyncParityP(void)
{
  UiTestApplication application;
  for(bool candidates : {false, true})
  {
    for(bool vertical : {false, true})
    {
      for(int scenario : {0, 1, 2})
      {
        for(float scale : {1.f, 1.5f})
        {
          auto p               = Parameters(candidates, vertical, scenario);
          p.effectiveTextScale = scale;
          p.textWidth *= scale;
          p.textHeight *= scale;
          Ui::Label label = Ui::Label::New(p.text.c_str());
          label.SetFontFamily("DejaVu Sans");
          label.SetRequestedWidth(p.textWidth);
          label.SetRequestedHeight(p.textHeight);
          label.SetFontSize(32.f);
          label.SetMultiLine(vertical);
          label.SetMinimumFontSizeScale(scale);
          label.SetMaximumFontSizeScale(scale);
          label.SetSystemFontSizeScaleEnabled(false);
          if(candidates)
          {
            label.SetTextFit(p.textFitCandidates);
          }
          else
          {
            label.SetTextFit(Text::Fit::Range(12.f, 32.f, 2.f));
          }
          label.SetMarqueeOrientation(p.marqueeOrientation);
          label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
          label.SetMarqueeLoopCount(0);
          application.GetScene().Add(label);
          application.SendNotification();
          application.UpdateOnly(0);
          application.GetCore().PostRender();
          application.SendNotification();
          auto visual     = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label)).GetVisual(Text::LabelPropertyIndex::TEXT);
          auto controller = Ui::Internal::TextVisual::GetController(visual);
          auto task       = Run(p);
          tet_printf("parity candidates=%d vertical=%d scenario=%d scale=%f", candidates, vertical, scenario, scale);
          DALI_TEST_EQUALS(controller->GetTextFitFontSize(Text::Controller::POINT_SIZE), task->mParameters.fontSize, .02f, TEST_LOCATION);
          DALI_TEST_EQUALS(controller->IsMarqueeEnabled(), task->mParameters.isMarqueeEnabled, TEST_LOCATION);
          DALI_TEST_EQUALS(label.IsMarqueeRunning(), scenario == 2, TEST_LOCATION);
          if(candidates)
          {
            DALI_TEST_EQUALS(controller->GetDefaultLineSize(), task->mParameters.minLineSize, .02f, TEST_LOCATION);
          }
          label.Unparent();
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTextFitOverflowMaximumLinesP(void)
{
  UiTestApplication application;
  auto              p    = Parameters(true, true, 2);
  p.maximumNumberOfLines = 1u;
  auto task              = Run(p);
  DALI_TEST_CHECK(task->mParameters.isMarqueeEnabled);
  DALI_TEST_EQUALS(task->mParameters.fontSize, 9.f, .02f, TEST_LOCATION);
  DALI_TEST_EQUALS(task->mRenderInfo.lineCount, 4, TEST_LOCATION);
  Ui::Label label = Ui::Label::New(p.text.c_str());
  label.SetFontFamily("DejaVu Sans");
  label.SetRequestedWidth(p.textWidth);
  label.SetRequestedHeight(p.textHeight);
  label.SetMultiLine(true);
  label.SetMaximumLines(1);
  label.SetTextFit(p.textFitCandidates);
  label.SetMarqueeOrientation(Text::MarqueeOrientation::VERTICAL);
  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
  application.GetScene().Add(label);
  application.SendNotification();
  application.UpdateOnly(0);
  application.GetCore().PostRender();
  application.SendNotification();
  DALI_TEST_CHECK(label.IsMarqueeRunning());
  END_TEST;
}

int UtcDaliTextFitOverflowCandidateQualityP(void)
{
  UiTestApplication application;
  for(bool ascendingHeight : {false, true})
  {
    auto p       = Parameters(true, true, 1);
    p.textHeight = 130.f;
    p.textFitCandidates.Clear();
    p.textFitCandidates.PushBack(Text::Fit::Candidate(12.f, 20.f));
    p.textFitCandidates.PushBack(Text::Fit::Candidate(22.f, ascendingHeight ? 60.f : 100.f));
    p.textFitCandidates.PushBack(Text::Fit::Candidate(32.f, 90.f));
    auto reference = Run(p);
    DALI_TEST_EQUALS(reference->mParameters.fontSize, 9.f, .02f, TEST_LOCATION);
    for(float quality : {1.5f, 2.f})
    {
      p.renderScale = quality;
      auto task     = Run(p);
      DALI_TEST_EQUALS(task->mParameters.fontSize, reference->mParameters.fontSize, .02f, TEST_LOCATION);
      // The publication snapshot keeps authored line height; raster quality
      // applies only when preparing worker layout metrics.
      DALI_TEST_EQUALS(task->mParameters.minLineSize, 20.f, .02f, TEST_LOCATION);
      DALI_TEST_CHECK(!task->mParameters.isMarqueeEnabled);
    }
  }
  END_TEST;
}
