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

#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/internal/text/text-gradient-style.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/public-api/visuals/text-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;

namespace
{

namespace UiInternal   = Dali::Ui::Internal;
namespace UiText       = Dali::Ui::Text;
namespace TextInternal = Dali::Ui::Text::Internal;
namespace Gradient     = Dali::Ui::Gradient;

constexpr float VISUAL_WIDTH  = 240.0f;
constexpr float VISUAL_HEIGHT = 96.0f;
constexpr float EPSILON       = 0.001f;
constexpr const char* UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME = "uTextGradientStartOffset";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_START_OFFSET_NAME = "uTextGradientOverlayStartOffset";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_MODE_NAME = "uTextGradientOverlayMode";

TextInternal::Gradient::Style MakeEnabledGradientStyle(float startOffset = 0.0f)
{
  Gradient::Linear linear(Vector2::ZERO, Vector2::ONE);
  linear.SetStartOffset(startOffset);

  Dali::Vector<Gradient::StopNode> stopNodes;
  stopNodes.PushBack(Gradient::StopNode(0.0f, Dali::Ui::UiColor(Color::RED)));
  stopNodes.PushBack(Gradient::StopNode(1.0f, Dali::Ui::UiColor(Color::BLUE)));
  linear.SetStopNodes(stopNodes);

  return TextInternal::Gradient::CreateStyle(linear);
}

struct RenderedTextVisual
{
  Dali::Ui::View                      view;
  Dali::Ui::Integration::Visual::Base internalVisual;
};

RenderedTextVisual CreateRenderedInternalTextVisualWithView(UiTestApplication& application, const char* text = "TextGradient")
{
  Dali::Ui::View view = Dali::Ui::View::New();
  view.SetProperty(Actor::Property::SIZE, Vector3(VISUAL_WIDTH, VISUAL_HEIGHT, 0.0f));

  Dali::Property::Map propertyMap;
  propertyMap.Add(Dali::Ui::VisualBasePropertyIndex::TYPE, Dali::Ui::Integration::InternalVisualType::TEXT);
  propertyMap.Add(Dali::Ui::TextVisualPropertyIndex::TEXT, text);
  propertyMap.Add(Dali::Ui::TextVisualPropertyIndex::FONT_SIZE, 12.0f);

  Dali::Ui::Integration::Visual::Base internalVisual = Dali::Ui::Integration::VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(internalVisual);
  Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view))
    .RegisterVisual(Dali::Ui::Text::LabelPropertyIndex::TEXT, internalVisual, Dali::Ui::Integration::DepthIndex::CONTENT);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  return {view, internalVisual};
}

void UpdateTextVisual(Dali::Ui::Integration::Visual::Base internalVisual)
{
  UiInternal::TextVisual::EnableRendererUpdate(internalVisual);
  UiInternal::TextVisual::UpdateRenderer(internalVisual);
}

Dali::Ui::Integration::Visual::Base CreateRenderedInternalTextVisual(UiTestApplication& application, const char* text = "TextGradient")
{
  return CreateRenderedInternalTextVisualWithView(application, text).internalVisual;
}

RenderedTextVisual CreateRenderedStyledTextVisual(UiTestApplication& application, const UiText::StyledText& styledText)
{
  RenderedTextVisual rendered = CreateRenderedInternalTextVisualWithView(application, styledText.GetText().CStr());
  UiInternal::TextVisual::GetController(rendered.internalVisual)->SetStyledText(styledText);
  UpdateTextVisual(rendered.internalVisual);
  return rendered;
}

RenderedTextVisual CreateRenderedFromMarkupTextVisual(UiTestApplication& application)
{
  return CreateRenderedStyledTextVisual(application, UiText::StyledText::FromMarkup("Default <color value='red'>Red</color> Default"));
}

RenderedTextVisual CreateRenderedFromMarkupUnderlineTextVisual(UiTestApplication& application)
{
  return CreateRenderedStyledTextVisual(application, UiText::StyledText::FromMarkup("Default <u color='red'>Underlined</u> Default"));
}

RenderedTextVisual CreateRenderedFromMarkupBackgroundTextVisual(UiTestApplication& application)
{
  return CreateRenderedStyledTextVisual(application, UiText::StyledText::FromMarkup("Default <background color='yellow'>Background</background> Default"));
}

void ExpectNoTextGradientOverlayRendererProperties(Renderer renderer)
{
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayStartPosition"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayEndPosition"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayStartOffset"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayBounds"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayType"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayRadialCenter"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayRadialScale"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayConicCenter"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayConicScale"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayConicStartAngle"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex(UNIFORM_TEXT_GRADIENT_OVERLAY_MODE_NAME), Property::INVALID_INDEX, TEST_LOCATION);
}

} // namespace

void utc_dali_text_gradient_visual_mask_plumbing_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_gradient_visual_mask_plumbing_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextGradientVisualMaskDisabledDoesNotCreateMaskP(void)
{
  UiTestApplication application;

  Dali::Ui::Integration::Visual::Base internalVisual = CreateRenderedInternalTextVisual(application);

  UpdateTextVisual(internalVisual);

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual);
  DALI_TEST_CHECK(!mask);
  END_TEST;
}

int UtcDaliTextGradientVisualMaskSimpleEnabledDoesNotCreateStoredMaskP(void)
{
  UiTestApplication application;

  Dali::Ui::Integration::Visual::Base internalVisual = CreateRenderedInternalTextVisual(application);
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledGradientStyle());

  UpdateTextVisual(internalVisual);

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual);
  DALI_TEST_CHECK(!mask);
  END_TEST;
}

int UtcDaliTextGradientOverlayVisualDisabledKeepsRendererCleanP(void)
{
  UiTestApplication application;

  Dali::Ui::Integration::Visual::Base internalVisual = CreateRenderedInternalTextVisual(application);

  UpdateTextVisual(internalVisual);

  Dali::VisualRenderer renderer = internalVisual.GetRenderer();
  ExpectNoTextGradientOverlayRendererProperties(renderer);
  END_TEST;
}

int UtcDaliTextGradientOverlayVisualDefaultModeUniformP(void)
{
  UiTestApplication application;

  Dali::Ui::Integration::Visual::Base internalVisual = CreateRenderedInternalTextVisual(application);
  UiInternal::TextVisual::SetTextGradientOverlayStyle(internalVisual, MakeEnabledGradientStyle());

  UpdateTextVisual(internalVisual);

  Dali::VisualRenderer renderer = internalVisual.GetRenderer();
  const Property::Index modeIndex = renderer.GetPropertyIndex(UNIFORM_TEXT_GRADIENT_OVERLAY_MODE_NAME);
  DALI_TEST_CHECK(modeIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(modeIndex), 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientType"), Property::INVALID_INDEX, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientOverlayVisualScreenModeUniformP(void)
{
  UiTestApplication application;

  Dali::Ui::Integration::Visual::Base internalVisual = CreateRenderedInternalTextVisual(application);
  UiInternal::TextVisual::SetTextGradientOverlayStyle(internalVisual, MakeEnabledGradientStyle());
  UiInternal::TextVisual::SetTextGradientOverlayMode(internalVisual, UiText::GradientOverlayMode::SCREEN);

  UpdateTextVisual(internalVisual);

  Dali::VisualRenderer renderer = internalVisual.GetRenderer();
  const Property::Index modeIndex = renderer.GetPropertyIndex(UNIFORM_TEXT_GRADIENT_OVERLAY_MODE_NAME);
  DALI_TEST_CHECK(modeIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(modeIndex), 1.0f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientOverlayVisualStartOffsetAnimSourceP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedInternalTextVisualWithView(application);
  UiInternal::TextVisual::SetTextGradientOverlayStyle(rendered.internalVisual, MakeEnabledGradientStyle(0.25f));
  UpdateTextVisual(rendered.internalVisual);

  Dali::VisualRenderer renderer = rendered.internalVisual.GetRenderer();
  const Property::Index overlayRendererOffsetIndex =
    renderer.GetPropertyIndex(UNIFORM_TEXT_GRADIENT_OVERLAY_START_OFFSET_NAME);
  DALI_TEST_CHECK(overlayRendererOffsetIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(overlayRendererOffsetIndex), 0.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex(UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  const Property::Index overlaySourceOffsetIndex =
    rendered.view.RegisterProperty(UNIFORM_TEXT_GRADIENT_OVERLAY_START_OFFSET_NAME, 0.75f);
  UiInternal::TextVisual::SetGradientOverlayAnimProperties(rendered.internalVisual, overlaySourceOffsetIndex);
  UiInternal::TextVisual::SetGradientOverlayAnimApplyAlways(rendered.internalVisual, true, true);

  application.SendNotification();
  application.Render(16);

  renderer = rendered.internalVisual.GetRenderer();
  const Property::Index reboundOverlayRendererOffsetIndex =
    renderer.GetPropertyIndex(UNIFORM_TEXT_GRADIENT_OVERLAY_START_OFFSET_NAME);
  DALI_TEST_CHECK(reboundOverlayRendererOffsetIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(reboundOverlayRendererOffsetIndex), 0.75f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex(UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextGradientVisualStartOffsetStyleResetP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedInternalTextVisualWithView(application);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledGradientStyle(0.0f));
  UpdateTextVisual(rendered.internalVisual);

  const Property::Index sourceOffsetIndex =
    rendered.view.RegisterProperty(UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME, 0.75f);
  UiInternal::TextVisual::SetGradientAnimProperties(rendered.internalVisual, sourceOffsetIndex);
  UiInternal::TextVisual::SetGradientAnimApplyAlways(rendered.internalVisual, true, true);

  application.SendNotification();
  application.Render(16);

  Dali::VisualRenderer renderer            = rendered.internalVisual.GetRenderer();
  const Property::Index rendererOffsetIndex = renderer.GetPropertyIndex(UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME);
  DALI_TEST_CHECK(rendererOffsetIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(rendererOffsetIndex), 0.75f, EPSILON, TEST_LOCATION);

  UiInternal::TextVisual::SetGradientAnimApplyAlways(rendered.internalVisual, false, true);

  // Simulate LabelImpl::SyncGradientAnimProperties() after SetTextGradient(new Linear).
  rendered.view.SetProperty(sourceOffsetIndex, 0.25f);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledGradientStyle(0.25f));

  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  renderer = rendered.internalVisual.GetRenderer();
  const Property::Index resetRendererOffsetIndex = renderer.GetPropertyIndex(UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME);
  DALI_TEST_CHECK(resetRendererOffsetIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(resetRendererOffsetIndex), 0.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(resetRendererOffsetIndex), 0.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(rendered.view.GetProperty<float>(sourceOffsetIndex), 0.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(rendered.view.GetCurrentProperty<float>(sourceOffsetIndex), 0.25f, EPSILON, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextGradientVisualMaskStyleSetterSimpleDoesNotCreateStoredMaskP(void)
{
  UiTestApplication application;

  Dali::Ui::Integration::Visual::Base internalVisual = CreateRenderedInternalTextVisual(application);

  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledGradientStyle());

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual);
  DALI_TEST_CHECK(!mask);
  END_TEST;
}

int UtcDaliTextGradientVisualMaskDisabledClearsStoredMaskP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedFromMarkupTextVisual(application);
  Dali::Ui::Integration::Visual::Base internalVisual = rendered.internalVisual;
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledGradientStyle());

  DALI_TEST_CHECK(UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual));

  TextInternal::Gradient::Style disabledStyle;
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, disabledStyle);

  DALI_TEST_CHECK(!UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual));
  END_TEST;
}

int UtcDaliTextGradientVisualMaskStyleSetterReappliesAfterDisabledP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedFromMarkupTextVisual(application);
  Dali::Ui::Integration::Visual::Base internalVisual = rendered.internalVisual;
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledGradientStyle());

  DALI_TEST_CHECK(UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual));

  TextInternal::Gradient::Style disabledStyle;
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, disabledStyle);

  DALI_TEST_CHECK(!UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual));

  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledGradientStyle());

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual);
  DALI_TEST_CHECK(mask);
  DALI_TEST_EQUALS(mask.GetPixelFormat(), Pixel::L8, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientVisualMaskColorOnlyFromMarkupCreatesMaskP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedFromMarkupTextVisual(application);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledGradientStyle());

  auto controller = UiInternal::TextVisual::GetController(rendered.internalVisual);
  DALI_TEST_CHECK(!controller->GetRenderTextModel()->IsMarkupUnderlineSet());
  DALI_TEST_CHECK(!controller->GetRenderTextModel()->IsMarkupStrikethroughSet());
  DALI_TEST_CHECK(!controller->GetRenderTextModel()->IsMarkupBackgroundColorSet());
  DALI_TEST_CHECK(controller->GetRenderTextModel()->GetColors() != nullptr);
  DALI_TEST_CHECK(controller->GetRenderTextModel()->GetColorIndices() != nullptr);

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(rendered.internalVisual);
  DALI_TEST_CHECK(mask);
  DALI_TEST_EQUALS(mask.GetPixelFormat(), Pixel::L8, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientVisualMaskStyledSimpleThenPlainDoesNotCreateStoredMaskP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedFromMarkupUnderlineTextVisual(application);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledGradientStyle());

  DALI_TEST_CHECK(!UiInternal::TextVisual::GetTextGradientMaskPixelData(rendered.internalVisual));

  auto controller = UiInternal::TextVisual::GetController(rendered.internalVisual);
  controller->SetText("");
  controller->SetText("TextGradient");
  UpdateTextVisual(rendered.internalVisual);

  DALI_TEST_CHECK(controller->GetRenderTextModel()->GetNumberOfGlyphs() > 0u);

  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledGradientStyle());

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(rendered.internalVisual);
  DALI_TEST_CHECK(!mask);
  END_TEST;
}

int UtcDaliTextGradientVisualMaskStyledSimpleDoesNotCreateStoredMaskP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedFromMarkupUnderlineTextVisual(application);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledGradientStyle());

  auto controller = UiInternal::TextVisual::GetController(rendered.internalVisual);
  DALI_TEST_CHECK(controller->GetRenderTextModel()->IsMarkupUnderlineSet());
  DALI_TEST_CHECK(!UiInternal::TextVisual::GetTextGradientMaskPixelData(rendered.internalVisual));
  END_TEST;
}

int UtcDaliTextGradientVisualMaskBackgroundStyleDoesNotCreateStoredMaskP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedFromMarkupBackgroundTextVisual(application);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledGradientStyle());

  auto controller = UiInternal::TextVisual::GetController(rendered.internalVisual);
  DALI_TEST_CHECK(controller->GetRenderTextModel()->IsMarkupBackgroundColorSet());
  DALI_TEST_CHECK(!UiInternal::TextVisual::GetTextGradientMaskPixelData(rendered.internalVisual));
  END_TEST;
}

int UtcDaliTextGradientVisualMaskMarqueeSimpleDoesNotCreateStoredMaskP(void)
{
  UiTestApplication application;

  Dali::Ui::Integration::Visual::Base internalVisual = CreateRenderedInternalTextVisual(application);
  UiInternal::TextVisual::GetController(internalVisual)
    ->SetMarqueeEnabled(true, false, UiText::MarqueeOrientation::HORIZONTAL);
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledGradientStyle());

  UpdateTextVisual(internalVisual);

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual);
  DALI_TEST_CHECK(!mask);
  END_TEST;
}
