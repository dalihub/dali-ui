#ifndef DALI_UI_GRADIENT_STOP_H
#define DALI_UI_GRADIENT_STOP_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/ui-color.h>

namespace Dali
{
namespace Ui
{
namespace Gradient
{
/**
 * The stop node tells the gradient what color it should be at certain position.
 */
class DALI_UI_API StopNode
{
public:
  StopNode();
  StopNode(float offset, const UiColor& color);
  ~StopNode();

  StopNode(const StopNode& rhs);
  StopNode(StopNode&& rhs) noexcept;
  StopNode& operator=(const StopNode& rhs);
  StopNode& operator=(StopNode&& rhs) noexcept;

public:
  float     GetOffset() const;
  StopNode& SetOffset(float offset);
  UiColor   GetColor() const;
  StopNode& SetColor(const UiColor& color);

  bool operator<(const StopNode& rhs) const
  {
    return GetOffset() < rhs.GetOffset();
  }

private:
  class Impl;
  Impl* mImpl;
};
} // namespace Gradient
} // namespace Ui
} // namespace Dali
#endif