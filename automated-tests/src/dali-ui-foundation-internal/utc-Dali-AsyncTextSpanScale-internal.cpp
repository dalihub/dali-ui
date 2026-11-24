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
// Mock adaptor headers must precede their production counterparts.
#include <dali-ui/ui-async-task-manager.h>
#include <dali-ui/ui-event-thread-callback.h>

// Resolve task types before other headers introduce Internal::Text.
#include <dali-ui-foundation/internal/text/async-text/text-loading-task.h>

#include <dali-ui-test-suite-utils.h>

// Depends on declarations provided by the common test utilities.
#include <dali-ui/ui-adaptor-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/text/styled-text/font-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/pixel-data-integ.h>
#include <cstring>
#include <initializer_list>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
struct RestoreUiScale
{
  float scale{UiScaleManager::Get().GetScale()};
  ~RestoreUiScale()
  {
    UiScaleManager::Get().SetScale(scale);
  }
};

Text::ControllerPtr ControllerOf(Label label)
{
  auto visual = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label)).GetVisual(Text::LabelPropertyIndex::TEXT);
  return Ui::Internal::TextVisual::GetController(visual);
}
bool PixelsEqual(PixelData actual, PixelData expected)
{
  if(!actual || !expected || actual.GetWidth() != expected.GetWidth() ||
     actual.GetHeight() != expected.GetHeight() || actual.GetPixelFormat() != expected.GetPixelFormat())
  {
    return false;
  }
  const auto a = Dali::Integration::GetPixelDataBuffer(actual);
  const auto b = Dali::Integration::GetPixelDataBuffer(expected);
  return a.bufferSize == b.bufferSize &&
         (a.bufferSize == 0u || (a.buffer && b.buffer && std::memcmp(a.buffer, b.buffer, a.bufferSize) == 0));
}

int CheckTextFitExplicitSpanScale(bool replacement)
{
  UiTestApplication application;
  RestoreUiScale    restoreScale;
  auto              fontClient    = TextAbstraction::FontClient::Get();
  unsigned          horizontalDpi = 0u, verticalDpi = 0u;
  fontClient.GetDpi(horizontalDpi, verticalDpi);
  DALI_TEST_CHECK(verticalDpi > 0u);
  const float pixelsToPoints = 72.f / static_cast<float>(verticalDpi);
  const char* source         = "Scale ABC mixed 한글 😀";
  for(float uiScale : {1.f, 1.2f})
  {
    UiScaleManager::Get().SetScale(uiScale);
    for(float fontScale : {1.f, 1.25f, 1.5f})
    {
      for(int fit = 0; fit < 4; ++fit)
      {
        for(bool explicitSize : {false, true})
        {
          Label label = Label::New(source);
          label.SetFontFamily("DejaVu Sans");
          label.SetFontSize(18.f);
          label.SetRequestedWidth(fit == 3 ? 20.f : 1000.f);
          label.SetRequestedHeight(fit == 3 ? 4.f : 200.f);
          label.SetMultiLine(true);
          label.SetTextOverflowMode(Text::OverflowMode::CLIP);
          label.SetMinimumFontSizeScale(fontScale);
          label.SetMaximumFontSizeScale(fontScale);
          label.SetSystemFontSizeScaleEnabled(false);
          if(fit)
          {
            label.SetTextFit(Text::Fit::Range(fit == 1 ? 18.f : 12.f, fit == 1 ? 18.f : 32.f, 2.f));
          }
          auto                 builder = Text::StyledTextBuilder::New(source);
          Text::FontAttributes attributes;
          if(explicitSize)
          {
            attributes.SetSize(28.f);
          }
          attributes.SetFamily("DejaVu Serif");
          attributes.SetWeight(Text::FontWeight::BOLD);
          attributes.SetSlant(Text::FontSlant::ITALIC);
          DALI_TEST_CHECK(builder.SetSpan(Text::FontSpan::New(attributes), 0u, 5u));
          if(replacement)
          {
            const auto start = builder.GetUtf32Length();
            builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
            DALI_TEST_CHECK(builder.SetSpan(Text::ImageSpan::New(Text::ImageAttributes("missing-fit-span.png", Vector2(12.f, 12.f))), start, start + 1u));
          }
          const auto styled = builder.Build();
          label.SetStyledText(styled);
          application.GetScene().Add(label);
          application.SendNotification();
          application.Render();
          auto                      controller = ControllerOf(label);
          const auto*               sync       = controller->GetRenderTextModel();
          Text::AsyncTextParameters p;
          p.requestType                = Ui::Integration::Text::Async::RENDER_FIXED_SIZE;
          p.text                       = label.GetText().CStr();
          p.fontFamily                 = "DejaVu Sans";
          p.fontSize                   = 18.f * pixelsToPoints;
          p.textWidth                  = label.GetSize().x;
          p.textHeight                 = label.GetSize().y;
          p.maxTextureSize             = 4096;
          p.isMultiLine                = true;
          p.ellipsis                   = false;
          p.effectiveTextScale         = controller->GetEffectiveTextScale();
          p.relativeLineSize           = controller->GetRelativeLineSize();
          p.minLineSize                = controller->GetDefaultLineSize();
          p.verticalLineAlignment      = controller->GetVerticalLineAlignment();
          p.isTextFitEnabled           = fit != 0;
          p.textFitMinSize             = (fit == 1 ? 18.f : 12.f) * pixelsToPoints;
          p.textFitMaxSize             = (fit == 1 ? 18.f : 32.f) * pixelsToPoints;
          p.textFitStepSize            = 2.f * pixelsToPoints;
          p.replacementSourceSnapshot  = controller->GetReplacementSourceSnapshot();
          p.hasStyledTextStyleSnapshot = true;
          p.styledTextStyleSnapshot    = Ui::Internal::Text::StyledTextApplier::BuildTextStyleRunSnapshot(styled, static_cast<float>(verticalDpi));
          auto loader                  = Text::AsyncTextLoader::New();
          auto info                    = fit ? loader.RenderTextFit(p, false, Size::ZERO) : loader.RenderText(p, false, Size::ZERO);
          DALI_TEST_CHECK(info.textPixelData);
          // The synchronous controller is an independent shaping/layout oracle.
          // Compare the full raster, including mixed-script glyphs and span sizes.
          auto typesetter = Text::Typesetter::New(sync);
          if(replacement)
          {
            typesetter->SetFinalElisionResult(&controller->GetReplacementRenderState().finalElision);
          }
          const auto expectedPixels = typesetter->Render(info.size, Text::Direction::LEFT_TO_RIGHT,
                                                         Text::Typesetter::RENDER_NO_STYLES, false,
                                                         info.textPixelData.GetPixelFormat());
          DALI_TEST_CHECK(PixelsEqual(info.textPixelData, expectedPixels));
          DALI_TEST_EQUALS(info.lineCount, static_cast<int>(sync->GetNumberOfLines()), TEST_LOCATION);
          if(fit)
          {
            DALI_TEST_EQUALS(p.fontSize, controller->GetTextFitFontSize(Text::Controller::POINT_SIZE), .02f, TEST_LOCATION);
          }
          if(explicitSize)
          {
            // Pin the numeric authored-size expectation in the independent
            // oracle as well as comparing the worker's complete raster to it.
            const auto expected = static_cast<TextAbstraction::PointSize26Dot6>(28.f * pixelsToPoints * p.effectiveTextScale * fontClient.GetNumberOfPointsPerOneUnitOfPointSize());
            auto       client   = TextAbstraction::FontClient::Get();
            bool       found    = false;
            for(unsigned glyph = 0u; glyph < sync->GetNumberOfGlyphs(); ++glyph)
            {
              const auto font = sync->GetGlyphs()[glyph].fontId;
              found |= font && client.GetPointSize(font) == expected;
            }
            DALI_TEST_CHECK(found);
          }
          label.Unparent();
          application.SendNotification();
          application.Render();
        }
      }
    }
  }
  END_TEST;
}

} // unnamed namespace

int UtcDaliAsyncTextFitExplicitSpanScaleP(void)
{
  return CheckTextFitExplicitSpanScale(false);
}

int UtcDaliAsyncTextFitReplacementSpanScaleP(void)
{
  return CheckTextFitExplicitSpanScale(true);
}
