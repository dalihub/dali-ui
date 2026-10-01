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

#include <dali-ui-foundation/internal/controls/text-controls/placeholder-properties.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/decorator/text-decorator.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_text_placeholder_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_placeholder_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextPlaceholderPropertiesAndFocusP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  Text::DecoratorPtr decorator = Text::Decorator::New(*controller, *controller);
  InputMethodContext inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);

  Property::Map fontStyle;
  fontStyle.Insert("weight", "bold");
  fontStyle.Insert("width", "condensed");
  fontStyle.Insert("slant", "italic");

  Property::Map placeholder;
  placeholder.Insert("text", "Enter a value");
  placeholder.Insert("textFocused", "Editing a value");
  placeholder.Insert("color", Color::CYAN);
  placeholder.Insert("fontFamily", "Sans");
  placeholder.Insert("fontStyle", fontStyle);
  placeholder.Insert("pointSize", 17.0f);
  placeholder.Insert("ellipsis", true);
  controller->SetPlaceholderProperty(placeholder);

  std::string text;
  controller->GetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE, text);
  DALI_TEST_EQUALS(text, std::string("Enter a value"), TEST_LOCATION);
  controller->GetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_ACTIVE, text);
  DALI_TEST_EQUALS(text, std::string("Editing a value"), TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetPlaceholderFontFamily(), std::string("Sans"), TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetPlaceholderTextColor(), Color::CYAN, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetPlaceholderTextFontSize(Text::Controller::POINT_SIZE), 17.0f, TEST_LOCATION);
  DALI_TEST_CHECK(controller->IsPlaceholderTextElideEnabled());

  Property::Map roundTrip;
  controller->GetPlaceholderProperty(roundTrip);
  DALI_TEST_CHECK(roundTrip.Find(Text::PlaceHolder::Property::TEXT));
  DALI_TEST_CHECK(roundTrip.Find(Text::PlaceHolder::Property::TEXT_FOCUSED));
  DALI_TEST_CHECK(roundTrip.Find(Text::PlaceHolder::Property::FONT_STYLE));
  DALI_TEST_CHECK(roundTrip.Find(Text::PlaceHolder::Property::POINT_SIZE));
  DALI_TEST_CHECK(roundTrip.Find(Text::PlaceHolder::Property::ELLIPSIS));

  controller->Relayout(Size(120.0f, 40.0f));
  auto& impl = Text::Controller::Impl::GetImplementation(*controller.Get());
  DALI_TEST_CHECK(impl.IsShowingPlaceholderText());
  controller->KeyboardFocusGainEvent(false);
  controller->Relayout(Size(120.0f, 40.0f));
  DALI_TEST_CHECK(impl.IsShowingPlaceholderText());

  controller->SetText("Actual value");
  controller->Relayout(Size(120.0f, 40.0f));
  DALI_TEST_CHECK(!impl.IsShowingPlaceholderText());

  Property::Map pixelSize;
  pixelSize.Insert(Text::PlaceHolder::Property::PIXEL_SIZE, 22.0f);
  pixelSize.Insert(Text::PlaceHolder::Property::ELLIPSIS, false);
  controller->SetPlaceholderProperty(pixelSize);
  controller->GetPlaceholderProperty(roundTrip);
  DALI_TEST_CHECK(roundTrip.Find(Text::PlaceHolder::Property::PIXEL_SIZE));
  DALI_TEST_CHECK(!controller->IsPlaceholderTextElideEnabled());

  controller->SetText("");
  controller->Relayout(Size(120.0f, 40.0f));
  DALI_TEST_CHECK(impl.IsShowingPlaceholderText());

  END_TEST;
}
