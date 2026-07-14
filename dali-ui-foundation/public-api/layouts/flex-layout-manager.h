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
 * @brief Implements the flexbox layout algorithm.
 *
 * Arranges children in a single-direction flow that may wrap onto multiple
 * lines. Per-child flex-grow, flex-shrink, flex-basis, and align-self are
 * read from FlexLayoutParams.
 */
class DALI_UI_API FlexLayoutManager : public LayoutManager
{
public:
  FlexLayoutManager(FlexDirection direction, FlexWrap wrap, FlexJustify justify, FlexAlign alignItems,
                    FlexAlign alignContent);
  ~FlexLayoutManager() override;

  void          SetDirection(FlexDirection direction);
  FlexDirection GetDirection() const;
  void          SetWrap(FlexWrap wrap);
  FlexWrap      GetWrap() const;
  void          SetJustifyContent(FlexJustify justify);
  FlexJustify   GetJustifyContent() const;
  void          SetAlignItems(FlexAlign align);
  FlexAlign     GetAlignItems() const;
  void          SetAlignContent(FlexAlign align);
  FlexAlign     GetAlignContent() const;

  bool IsMainAxisHorizontal() const;
  bool IsMainAxisReversed() const;

  MeasuredSize Measure(ViewImpl* view, float widthConstraint, float heightConstraint) override;
  void         Arrange(ViewImpl* view, const LayoutRect& bounds) override;

private:
  class Impl;
};

} // namespace Ui
} // namespace Dali
