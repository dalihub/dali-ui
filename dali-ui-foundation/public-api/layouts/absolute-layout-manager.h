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
#include <dali-ui-foundation/public-api/layouts/layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Implements the absolute positioning layout algorithm.
 *
 * AbsoluteLayoutManager arranges each child at an explicit position and
 * size described by its AbsoluteLayoutParams.
 */
class DALI_UI_API AbsoluteLayoutManager : public LayoutManager
{
public:
  AbsoluteLayoutManager();
  ~AbsoluteLayoutManager() override;

  MeasuredSize Measure(ViewImpl* view, float widthConstraint, float heightConstraint) override;
  MeasuredSize Arrange(ViewImpl* view, const LayoutRect& bounds) override;

private:
  class Impl;
};

} // namespace Ui
} // namespace Dali
