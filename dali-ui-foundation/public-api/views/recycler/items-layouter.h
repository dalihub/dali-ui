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

#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/recycler/items-layouter-impl.h>
#include <dali/public-api/object/base-handle.h>
#include <cstdint>

namespace Dali
{
namespace Ui
{

/**
 * @brief ABI-stable handle for a RecyclerView layout algorithm.
 *
 * ItemsLayouter is a reference-counted handle (like Dali::Animation or
 * EdgeEffect). The virtual dispatch happens inside ItemsLayouterImpl, so
 * the handle class itself carries no virtual methods and its size never
 * changes across releases.
 *
 * To implement a custom layout algorithm, subclass ItemsLayouterImpl and
 * wrap it in an ItemsLayouter-derived handle. See items-layouter-impl.h.
 */
class DALI_UI_API ItemsLayouter : public Dali::BaseHandle
{
public:
  using Orientation = ItemsLayouterImpl::Orientation;

  /**
   * @brief Creates an uninitialized (null) handle. Evaluates to false.
   */
  ItemsLayouter() = default;

  ~ItemsLayouter() = default;

  ItemsLayouter(const ItemsLayouter&)            = default;
  ItemsLayouter(ItemsLayouter&&)                 = default;
  ItemsLayouter& operator=(const ItemsLayouter&) = default;
  ItemsLayouter& operator=(ItemsLayouter&&)      = default;

  // --- App-facing query/configuration API ----------------------------------
  // (RecyclerView internals call layout/scroll methods directly via GetImpl())

  bool  CanScrollVertically() const;
  bool  CanScrollHorizontally() const;
  float ComputeScrollOffset() const;
  float ComputeScrollExtent() const;
  float ComputeScrollRange() const;

  uint32_t   GetFirstVisiblePosition() const;
  uint32_t   GetLastVisiblePosition() const;
  LayoutRect GetItemBounds(uint32_t position, float crossExtent) const;

  Orientation GetOrientation() const;
  void        SetOrientation(Orientation orientation);
  void        SetItemExtent(float extent);
  float       GetItemExtent() const;
  void        SetItemSpacing(float spacing);
  float       GetItemSpacing() const;

public: // Not intended for application developers
  /// @cond internal
  explicit ItemsLayouter(ItemsLayouterImpl* impl);

  ItemsLayouterImpl&       GetImpl();
  const ItemsLayouterImpl& GetImpl() const;
  /// @endcond
};

} // namespace Ui
} // namespace Dali
