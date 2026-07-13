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

#include <dali-ui-foundation/internal/text/editable-text-gradient-property-data.h>
#include <dali-ui-foundation/internal/text/text-gradient-helper.h>
#include <dali-ui-foundation/internal/text/text-gradient-property-data.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual-gradient-data.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-editor.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-field.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

Dali::Vector<Gradient::StopNode> MakeStopNodes()
{
  Dali::Vector<Gradient::StopNode> stopNodes;
  stopNodes.PushBack(Gradient::StopNode(0.0f, UiColor(Color::GREEN)));
  stopNodes.PushBack(Gradient::StopNode(1.0f, UiColor(Color::YELLOW)));
  return stopNodes;
}

Gradient::Linear MakeRenderableLinear()
{
  Gradient::Linear linear(Vector2::ZERO, Vector2::ONE);
  linear.SetStopNodes(MakeStopNodes());
  return linear;
}

} // unnamed namespace

void utc_dali_text_gradient_property_data_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_gradient_property_data_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextGradientPropertyDataLabelLazyLifecycleP(void)
{
  UiTestApplication application;

  Dali::Ui::Internal::Text::TextGradientPropertyDataPtr storage;
  DALI_TEST_CHECK(Dali::Ui::Internal::Text::GetTextGradientPropertyData(storage) == nullptr);

  auto& storageData = Dali::Ui::Internal::Text::GetOrCreateTextGradientPropertyData(storage);
  DALI_TEST_CHECK(Dali::Ui::Internal::Text::GetTextGradientPropertyData(storage) == &storageData);
  DALI_TEST_EQUALS(storageData.textGradient.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.textGradientOverlay.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.textGradientBoundsMode, Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.textGradientOverlayBoundsMode, Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.textGradientOverlayMode, Text::GradientOverlayMode::SRC_OVER, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.gradientAnimOffsetIndex, Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.gradientOverlayAnimOffsetIndex, Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.gradientAnimCount, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.gradientOverlayAnimCount, 0, TEST_LOCATION);

  Label label = Label::New();

  DALI_TEST_EQUALS(label.GetTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientOverlay().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayMode(), Text::GradientOverlayMode::SRC_OVER, TEST_LOCATION);

  label.SetTextGradient(Gradient::Base::None());
  label.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  label.SetTextGradientOverlay(Gradient::Base::None());
  label.SetTextGradientOverlayBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  label.SetTextGradientOverlayMode(Text::GradientOverlayMode::SRC_OVER);
  DALI_TEST_EQUALS(label.GetTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientOverlay().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayMode(), Text::GradientOverlayMode::SRC_OVER, TEST_LOCATION);

  label.SetTextGradientBoundsMode(Text::GradientBoundsMode::VIEW_BOUND);
  DALI_TEST_EQUALS(label.GetTextGradientBoundsMode(), Text::GradientBoundsMode::VIEW_BOUND, TEST_LOCATION);

  label.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  DALI_TEST_EQUALS(label.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  label.SetTextGradient(MakeRenderableLinear());
  DALI_TEST_EQUALS(label.GetTextGradient().GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientOverlay().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayMode(), Text::GradientOverlayMode::SRC_OVER, TEST_LOCATION);

  label.SetTextGradient(Gradient::Base::None());
  DALI_TEST_EQUALS(label.GetTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextGradientPropertyDataEditableLazyLifecycleP(void)
{
  UiTestApplication application;

  Dali::Ui::Internal::Text::EditableTextGradientPropertyDataPtr storage;
  DALI_TEST_CHECK(Dali::Ui::Internal::Text::GetEditableTextGradientPropertyData(storage) == nullptr);

  auto& storageData = Dali::Ui::Internal::Text::GetOrCreateEditableTextGradientPropertyData(storage);
  DALI_TEST_CHECK(Dali::Ui::Internal::Text::GetEditableTextGradientPropertyData(storage) == &storageData);
  DALI_TEST_EQUALS(storageData.atlasResources.GetTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.atlasResources.GetPlaceholderGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.atlasResources.GetBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.gradientAnimOffsetIndex, Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.placeholderGradientAnimOffsetIndex, Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.gradientAnimCount, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.placeholderGradientAnimCount, 0, TEST_LOCATION);

  InputField field = InputField::New();
  DALI_TEST_EQUALS(field.GetTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetPlaceholderTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  field.SetTextGradient(Gradient::Base::None());
  field.SetPlaceholderTextGradient(Gradient::Base::None());
  field.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  DALI_TEST_EQUALS(field.GetTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetPlaceholderTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  field.SetTextGradientBoundsMode(Text::GradientBoundsMode::VIEW_BOUND);
  DALI_TEST_EQUALS(field.GetTextGradientBoundsMode(), Text::GradientBoundsMode::VIEW_BOUND, TEST_LOCATION);

  field.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  DALI_TEST_EQUALS(field.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  field.SetTextGradient(MakeRenderableLinear());
  DALI_TEST_EQUALS(field.GetTextGradient().GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetPlaceholderTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  field.SetTextGradient(Gradient::Base::None());
  DALI_TEST_EQUALS(field.GetTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);

  InputEditor editor = InputEditor::New();

  editor.SetPlaceholderTextGradient(Gradient::Base::None());
  editor.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  DALI_TEST_EQUALS(editor.GetPlaceholderTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(editor.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  editor.SetPlaceholderTextGradient(MakeRenderableLinear());
  DALI_TEST_EQUALS(editor.GetTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(editor.GetPlaceholderTextGradient().GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_EQUALS(editor.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  editor.SetPlaceholderTextGradient(Gradient::Base::None());
  DALI_TEST_EQUALS(editor.GetPlaceholderTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextGradientPropertyDataVisualLazyLifecycleP(void)
{
  UiTestApplication application;

  Dali::Ui::Internal::TextVisualGradientDataPtr storage;
  DALI_TEST_CHECK(Dali::Ui::Internal::GetTextVisualGradientData(storage) == nullptr);

  auto& storageData = Dali::Ui::Internal::GetOrCreateTextVisualGradientData(storage);
  DALI_TEST_CHECK(Dali::Ui::Internal::GetTextVisualGradientData(storage) == &storageData);
  DALI_TEST_CHECK(!Dali::Ui::Text::Internal::Gradient::IsRenderable(storageData.mTextGradientStyle));
  DALI_TEST_CHECK(!Dali::Ui::Text::Internal::Gradient::IsRenderable(storageData.mTextGradientOverlayStyle));
  DALI_TEST_EQUALS(storageData.mTextGradientBoundsMode, Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.mTextGradientOverlayBoundsMode, Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.mTextGradientOverlayMode, Text::GradientOverlayMode::SRC_OVER, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.mGradientAnimOffsetIndex, Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(storageData.mGradientOverlayAnimOffsetIndex, Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_CHECK(!storageData.mTextGradientMaskPixelData);
  DALI_TEST_CHECK(!storageData.mGradientRenderer);
  DALI_TEST_CHECK(!storageData.mGradientOverlayRenderer);
  DALI_TEST_CHECK(storageData.mGradientAnimConstraints.empty());
  DALI_TEST_CHECK(storageData.mGradientOverlayAnimConstraints.empty());
  DALI_TEST_CHECK(!storageData.mGradientAnimApplyAlways);
  DALI_TEST_CHECK(!storageData.mGradientOverlayAnimApplyAlways);
  DALI_TEST_CHECK(!storageData.mHasGradientContext);
  DALI_TEST_CHECK(!storageData.mHasGradientOverlayContext);

  END_TEST;
}
