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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/span-impl.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

/**
 * @brief Implementation for foreground color span payloads.
 */
class ForegroundColorSpan : public Span
{
public:
  explicit ForegroundColorSpan(const UiColor& color);
  ~ForegroundColorSpan() override;

  const UiColor& GetColor() const;

private:
  UiColor mColor;
};

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
