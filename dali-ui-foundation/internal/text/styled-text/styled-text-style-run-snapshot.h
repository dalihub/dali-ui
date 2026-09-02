#pragma once

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

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector4.h>
#include <memory>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/strikethrough-style-properties.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>
#include <dali-ui-foundation/internal/text/text-gradient-style.h>
#include <dali-ui-foundation/internal/text/underline-style-properties.h>
#include <dali-ui-foundation/public-api/text/styled-text/gradient-span.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{

struct StyledTextColorRunSnapshot
{
  Dali::Ui::Text::CharacterIndex characterIndex{0u};
  Dali::Ui::Text::Length         numberOfCharacters{0u};
  Vector4                        color{Color::BLACK};
  uint32_t                       insertionOrder{0u};
};

struct StyledTextGradientRunSnapshot
{
  Dali::Ui::Text::CharacterIndex           characterIndex{0u};
  Dali::Ui::Text::Length                   numberOfCharacters{0u};
  Gradient::Style                          style{};
  Dali::Ui::Text::GradientSpan::BoundsMode boundsMode{Dali::Ui::Text::GradientSpan::BoundsMode::SPAN_BOUND};
  uint32_t                                 insertionOrder{0u};
};

struct StyledTextGradientSnapshotData
{
  std::vector<StyledTextGradientRunSnapshot> runs;
};

struct StyledTextFontRunSnapshot
{
  Dali::Ui::Text::CharacterIndex characterIndex{0u};
  Dali::Ui::Text::Length         numberOfCharacters{0u};

  bool        hasFamily{false};
  std::string family{};

  bool                            hasSize{false};
  Dali::Ui::Text::PointSize26Dot6 size{0u};

  bool                           hasWeight{false};
  Dali::Ui::Text::FontWeightType weight{TextAbstraction::FontWeight::NONE};

  bool                          hasWidth{false};
  Dali::Ui::Text::FontWidthType width{TextAbstraction::FontWidth::NONE};

  bool                          hasSlant{false};
  Dali::Ui::Text::FontSlantType slant{TextAbstraction::FontSlant::NONE};
};

struct StyledTextUnderlineRunSnapshot
{
  Dali::Ui::Text::CharacterIndex           characterIndex{0u};
  Dali::Ui::Text::Length                   numberOfCharacters{0u};
  Dali::Ui::Text::UnderlineStyleProperties properties{};
};

struct StyledTextLineThroughRunSnapshot
{
  Dali::Ui::Text::CharacterIndex               characterIndex{0u};
  Dali::Ui::Text::Length                       numberOfCharacters{0u};
  Dali::Ui::Text::StrikethroughStyleProperties properties{};
};

struct StyledTextAnchorRunSnapshot
{
  Dali::Ui::Text::CharacterIndex characterIndex{0u};
  Dali::Ui::Text::Length         numberOfCharacters{0u};
  std::string                    href{};
  Vector4                        color{Color::MEDIUM_BLUE};
  Vector4                        clickedColor{Color::DARK_MAGENTA};
  bool                           hasColor{false};
  bool                           hasClickedColor{false};
};

struct StyledTextStyleRunSnapshot
{
  std::vector<StyledTextColorRunSnapshot>               foregroundColorRuns;
  std::vector<StyledTextColorRunSnapshot>               backgroundColorRuns;
  std::vector<StyledTextFontRunSnapshot>                fontRuns;
  std::vector<StyledTextUnderlineRunSnapshot>           underlineRuns;
  std::vector<StyledTextLineThroughRunSnapshot>         lineThroughRuns;
  std::vector<StyledTextAnchorRunSnapshot>              anchorRuns;
  std::shared_ptr<const StyledTextGradientSnapshotData> gradientData;
};

} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali
