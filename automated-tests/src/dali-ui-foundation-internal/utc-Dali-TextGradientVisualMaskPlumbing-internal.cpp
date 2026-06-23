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

#include <dali-ui-foundation/devel-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/internal/text/text-gradient-style.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
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

TextInternal::TextGradientStyle MakeEnabledTextGradientStyle(float startOffset = 0.0f)
{
  Gradient::Linear linear(Vector2::ZERO, Vector2::ONE);
  linear.SetStartOffset(startOffset);

  Dali::Vector<Gradient::StopNode> stopNodes;
  stopNodes.PushBack(Gradient::StopNode(0.0f, Dali::Ui::UiColor(Color::RED)));
  stopNodes.PushBack(Gradient::StopNode(1.0f, Dali::Ui::UiColor(Color::BLUE)));
  linear.SetStopNodes(stopNodes);

  return TextInternal::CreateTextGradientStyle(linear);
}

struct RenderedTextVisual
{
  Dali::Ui::View         view;
  Dali::Ui::Visual::Base internalVisual;
};

RenderedTextVisual CreateRenderedInternalTextVisualWithView(UiTestApplication& application, const char* text = "TextGradient", bool markupEnabled = false)
{
  Dali::Ui::View view = Dali::Ui::View::New();
  view.SetProperty(Actor::Property::SIZE, Vector3(VISUAL_WIDTH, VISUAL_HEIGHT, 0.0f));

  Dali::Property::Map propertyMap;
  propertyMap.Add(Dali::Ui::VisualBasePropertyIndex::TYPE, Dali::Ui::InternalVisualType::TEXT);
  propertyMap.Add(Dali::Ui::TextVisualPropertyIndex::TEXT, text);
  propertyMap.Add(Dali::Ui::TextVisualPropertyIndex::FONT_SIZE, 12.0f);
  propertyMap.Add(Dali::Ui::TextVisualPropertyIndex::MARKUP_ENABLED, markupEnabled);

  Dali::Ui::Visual::Base internalVisual = Dali::Ui::VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(internalVisual);
  Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view))
    .RegisterVisual(Dali::Ui::Text::LabelPropertyIndex::TEXT, internalVisual, Dali::Ui::DepthIndex::CONTENT);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  return {view, internalVisual};
}

Dali::Ui::Visual::Base CreateRenderedInternalTextVisual(UiTestApplication& application, const char* text = "TextGradient", bool markupEnabled = false)
{
  return CreateRenderedInternalTextVisualWithView(application, text, markupEnabled).internalVisual;
}

RenderedTextVisual CreateRenderedMarkupTextVisual(UiTestApplication& application)
{
  return CreateRenderedInternalTextVisualWithView(application, "Default <color value='red'>Red</color> Default", true);
}

RenderedTextVisual CreateRenderedMarkupUnderlineTextVisual(UiTestApplication& application)
{
  return CreateRenderedInternalTextVisualWithView(application, "Default <u color='red'>Underlined</u> Default", true);
}

RenderedTextVisual CreateRenderedMarkupBackgroundTextVisual(UiTestApplication& application)
{
  return CreateRenderedInternalTextVisualWithView(application, "Default <background color='yellow'>Background</background> Default", true);
}

void UpdateTextVisual(Dali::Ui::Visual::Base internalVisual)
{
  UiInternal::TextVisual::EnableRendererUpdate(internalVisual);
  UiInternal::TextVisual::UpdateRenderer(internalVisual);
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

  Dali::Ui::Visual::Base internalVisual = CreateRenderedInternalTextVisual(application);

  UpdateTextVisual(internalVisual);

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual);
  DALI_TEST_CHECK(!mask);
  END_TEST;
}

int UtcDaliTextGradientVisualMaskSimpleEnabledDoesNotCreateStoredMaskP(void)
{
  UiTestApplication application;

  Dali::Ui::Visual::Base internalVisual = CreateRenderedInternalTextVisual(application);
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledTextGradientStyle());

  UpdateTextVisual(internalVisual);

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual);
  DALI_TEST_CHECK(!mask);
  END_TEST;
}

int UtcDaliTextGradientVisualStartOffsetStyleResetP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedInternalTextVisualWithView(application);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledTextGradientStyle(0.0f));
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
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledTextGradientStyle(0.25f));

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

  Dali::Ui::Visual::Base internalVisual = CreateRenderedInternalTextVisual(application);

  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledTextGradientStyle());

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual);
  DALI_TEST_CHECK(!mask);
  END_TEST;
}

int UtcDaliTextGradientVisualMaskDisabledClearsStoredMaskP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedMarkupTextVisual(application);
  Dali::Ui::Visual::Base internalVisual = rendered.internalVisual;
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledTextGradientStyle());

  DALI_TEST_CHECK(UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual));

  TextInternal::TextGradientStyle disabledStyle;
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, disabledStyle);

  DALI_TEST_CHECK(!UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual));
  END_TEST;
}

int UtcDaliTextGradientVisualMaskStyleSetterReappliesAfterDisabledP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedMarkupTextVisual(application);
  Dali::Ui::Visual::Base internalVisual = rendered.internalVisual;
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledTextGradientStyle());

  DALI_TEST_CHECK(UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual));

  TextInternal::TextGradientStyle disabledStyle;
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, disabledStyle);

  DALI_TEST_CHECK(!UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual));

  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledTextGradientStyle());

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual);
  DALI_TEST_CHECK(mask);
  DALI_TEST_EQUALS(mask.GetPixelFormat(), Pixel::L8, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientVisualMaskColorOnlyMarkupCreatesMaskP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedMarkupTextVisual(application);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledTextGradientStyle());

  auto controller = UiInternal::TextVisual::GetController(rendered.internalVisual);
  DALI_TEST_CHECK(controller->IsMarkupProcessorEnabled());
  DALI_TEST_CHECK(!controller->GetTextModel()->IsMarkupUnderlineSet());
  DALI_TEST_CHECK(!controller->GetTextModel()->IsMarkupStrikethroughSet());
  DALI_TEST_CHECK(!controller->GetTextModel()->IsMarkupBackgroundColorSet());
  DALI_TEST_CHECK(controller->GetTextModel()->GetColors() != nullptr);
  DALI_TEST_CHECK(controller->GetTextModel()->GetColorIndices() != nullptr);

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(rendered.internalVisual);
  DALI_TEST_CHECK(mask);
  DALI_TEST_EQUALS(mask.GetPixelFormat(), Pixel::L8, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientVisualMaskStyledSimpleThenPlainDoesNotCreateStoredMaskP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedMarkupUnderlineTextVisual(application);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledTextGradientStyle());

  DALI_TEST_CHECK(!UiInternal::TextVisual::GetTextGradientMaskPixelData(rendered.internalVisual));

  auto controller = UiInternal::TextVisual::GetController(rendered.internalVisual);
  controller->SetMarkupProcessorEnabled(false);
  controller->SetText("");
  controller->SetText("TextGradient");
  UpdateTextVisual(rendered.internalVisual);

  DALI_TEST_CHECK(!controller->IsMarkupProcessorEnabled());
  DALI_TEST_CHECK(controller->GetTextModel()->GetNumberOfGlyphs() > 0u);

  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledTextGradientStyle());

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(rendered.internalVisual);
  DALI_TEST_CHECK(!mask);
  END_TEST;
}

int UtcDaliTextGradientVisualMaskStyledSimpleDoesNotCreateStoredMaskP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedMarkupUnderlineTextVisual(application);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledTextGradientStyle());

  auto controller = UiInternal::TextVisual::GetController(rendered.internalVisual);
  DALI_TEST_CHECK(controller->IsMarkupProcessorEnabled());
  DALI_TEST_CHECK(controller->GetTextModel()->IsMarkupUnderlineSet());
  DALI_TEST_CHECK(!UiInternal::TextVisual::GetTextGradientMaskPixelData(rendered.internalVisual));
  END_TEST;
}

int UtcDaliTextGradientVisualMaskBackgroundStyleDoesNotCreateStoredMaskP(void)
{
  UiTestApplication application;

  RenderedTextVisual rendered = CreateRenderedMarkupBackgroundTextVisual(application);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.internalVisual, MakeEnabledTextGradientStyle());

  auto controller = UiInternal::TextVisual::GetController(rendered.internalVisual);
  DALI_TEST_CHECK(controller->IsMarkupProcessorEnabled());
  DALI_TEST_CHECK(controller->GetTextModel()->IsMarkupBackgroundColorSet());
  DALI_TEST_CHECK(!UiInternal::TextVisual::GetTextGradientMaskPixelData(rendered.internalVisual));
  END_TEST;
}

int UtcDaliTextGradientVisualMaskMarqueeSimpleDoesNotCreateStoredMaskP(void)
{
  UiTestApplication application;

  Dali::Ui::Visual::Base internalVisual = CreateRenderedInternalTextVisual(application);
  UiInternal::TextVisual::GetController(internalVisual)
    ->SetMarqueeEnabled(true, false, UiText::MarqueeOrientation::HORIZONTAL);
  UiInternal::TextVisual::SetTextGradientStyle(internalVisual, MakeEnabledTextGradientStyle());

  UpdateTextVisual(internalVisual);

  PixelData mask = UiInternal::TextVisual::GetTextGradientMaskPixelData(internalVisual);
  DALI_TEST_CHECK(!mask);
  END_TEST;
}
