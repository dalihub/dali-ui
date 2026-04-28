#ifndef DALI_UI_TEXT_CONTROLLER_INPUT_FONT_HANDLER_H
#define DALI_UI_TEXT_CONTROLLER_INPUT_FONT_HANDLER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
struct Controller::InputFontHandler
{
  static void           SetInputFontFamily(Controller& controller, const std::string& fontFamily);
  static std::string    GetInputFontFamily(const Controller& controller);
  static void           SetInputFontWeight(const Controller& controller, FontWeightType weight);
  static bool           IsInputFontWeightDefined(const Controller& controller);
  static FontWeightType GetInputFontWeight(const Controller& controller);
  static void           SetInputFontWidth(Controller& controller, FontWidthType width);
  static bool           IsInputFontWidthDefined(const Controller& controller);
  static FontWidthType  GetInputFontWidth(const Controller& controller);
  static void           SetInputFontSlant(Controller& controller, FontSlantType slant);
  static bool           IsInputFontSlantDefined(const Controller& controller);
  static FontSlantType  GetInputFontSlant(const Controller& controller);
  static void           SetInputFontPointSize(Controller& controller, float size, bool defaultFontSizeUpdated);
  static float          GetInputFontPointSize(const Controller& controller);
};

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_CONTROLLER_INPUT_FONT_HANDLER_H
