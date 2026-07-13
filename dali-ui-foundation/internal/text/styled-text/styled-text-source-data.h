#ifndef DALI_UI_INTERNAL_TEXT_STYLED_TEXT_SOURCE_DATA_H
#define DALI_UI_INTERNAL_TEXT_STYLED_TEXT_SOURCE_DATA_H

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
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

class StyledTextSourceData
{
public:
  void Set(const Dali::Ui::Text::StyledText& styledText);

  void Clear();

  Dali::Ui::Text::StyledText GetStyledText() const;

private:
  Dali::Ui::Text::StyledText mStyledText;
};

using StyledTextSourceDataPtr = std::unique_ptr<StyledTextSourceData>;

void SetStyledTextSource(StyledTextSourceDataPtr& data, const Dali::Ui::Text::StyledText& styledText);

void ClearStyledTextSource(StyledTextSourceDataPtr& data);

Dali::Ui::Text::StyledText GetStyledTextSource(const StyledTextSourceDataPtr& data);

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_INTERNAL_TEXT_STYLED_TEXT_SOURCE_DATA_H
