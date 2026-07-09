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
#include <dali-ui-foundation/public-api/views/recycler/item-decoration.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <cstdint>

namespace Dali
{
namespace Ui
{

/**
 * @brief Bridge between ItemsLayouter and RecyclerView.
 *
 * Layouter implementations call these methods to obtain or recycle item views
 * without knowing about the view pool, the adapter, or the scene graph.
 * RecyclerView provides the concrete implementation (RecyclerImpl, defined in
 * recycler-view-impl.cpp).
 *
 * This interface lives in integration-api because application code never
 * instantiates or subclasses it directly; it is only seen by ItemsLayouterImpl
 * subclasses and RecyclerViewImpl.
 */
class DALI_UI_API Recycler
{
public:
  virtual ~Recycler() = default;

  // Obtain a view for position: returns an existing active view or pulls one
  // from the recycle pool (binding it to position) or creates a new one.
  virtual View GetViewForPosition(uint32_t position) = 0;

  // Return the active view for position to the recycle pool.
  virtual void RecycleViewForPosition(uint32_t position) = 0;

  // Move all currently active views to the recycle pool.
  virtual void RecycleAllViews() = 0;

  virtual uint32_t GetItemCount() const      = 0;
  virtual float    GetViewportExtent() const = 0;
  virtual float    GetCrossExtent() const    = 0;
  virtual float    GetCacheBefore() const    = 0;
  virtual float    GetCacheAfter() const     = 0;

  // Returns the sum of all registered ItemDecoration::GetItemOffsets() for the
  // item at @a position. Called by the layouter in LayoutChunk after
  // GetViewForPosition() has populated the holder.
  virtual ItemOffsets GetDecorationOffsets(uint32_t position) const = 0;
};

} // namespace Ui
} // namespace Dali
