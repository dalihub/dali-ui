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

#include <dali-ui-foundation/public-api/views/recycler/items-layouter.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief ItemsLayouter handle for a single-axis (vertical or horizontal) list.
 *
 * The actual layout algorithm lives in the private LinearItemsLayouterImpl
 * which is compiled into the library. Fill and LayoutChunk are non-virtual
 * end-class methods on that impl — they are not part of the abstract
 * ItemsLayouterImpl interface and cannot be overridden by subclasses.
 */
class DALI_UI_API LinearItemsLayouter : public ItemsLayouter
{
public:
  LinearItemsLayouter()  = default;
  ~LinearItemsLayouter() = default;

  LinearItemsLayouter(const LinearItemsLayouter&)            = default;
  LinearItemsLayouter(LinearItemsLayouter&&)                 = default;
  LinearItemsLayouter& operator=(const LinearItemsLayouter&) = default;
  LinearItemsLayouter& operator=(LinearItemsLayouter&&)      = default;

  static LinearItemsLayouter New(Orientation orientation = Orientation::VERTICAL);

public: // Not intended for application developers
  /// @cond internal
  explicit LinearItemsLayouter(Integration::ItemsLayouterImpl* impl);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
