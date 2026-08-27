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

#include <dali-ui-foundation/integration-api/recycler.h>
#include <dali-ui-foundation/public-api/views/recycler/items-layouter.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

namespace Dali
{
namespace Ui
{

namespace Integration
{

class ItemsLayouterImpl;
using ItemsLayouterImplPtr = IntrusivePtr<ItemsLayouterImpl>;

/**
 * @brief Abstract implementation base for ItemsLayouter.
 *
 * External users who want to provide a custom layout algorithm should
 * subclass ItemsLayouterImpl, override all pure-virtual methods, and
 * expose a matching ItemsLayouter-derived handle. The impl creates itself
 * via a static New() factory; the handle never calls new directly:
 *
 * @code
 * class MyLayouterImpl : public ItemsLayouterImpl {
 * public:
 *   static IntrusivePtr<MyLayouterImpl> New() { return new MyLayouterImpl(); }
 *   void OnLayoutChildren(Recycler& r) override { ... }
 *   ...
 * };
 *
 * class MyLayouter : public ItemsLayouter {
 * public:
 *   static MyLayouter New() {
 *     IntrusivePtr<MyLayouterImpl> impl = MyLayouterImpl::New();
 *     return MyLayouter(impl.Get());
 *   }
 *   explicit MyLayouter(ItemsLayouterImpl* impl) : ItemsLayouter(impl) {}
 * };
 * @endcode
 *
 * External libraries include this integration-api header to provide custom
 * layout algorithms.
 */
class DALI_UI_API ItemsLayouterImpl : public Dali::BaseObject
{
public:
  using Orientation = ItemsLayouter::Orientation;

  ~ItemsLayouterImpl() override;

  // Full relayout: recycles all children and lays out from the current
  // scroll offset. Called when the viewport size changes or the adapter
  // is replaced.
  virtual void OnLayoutChildren(Recycler& recycler) = 0;

  // Incremental scroll along the layout axis. Returns actually consumed
  // distance (may be less than |dy|/|dx| when hitting an edge).
  virtual float ScrollVerticallyBy(float dy, Recycler& recycler)   = 0;
  virtual float ScrollHorizontallyBy(float dx, Recycler& recycler) = 0;

  virtual bool CanScrollVertically() const   = 0;
  virtual bool CanScrollHorizontally() const = 0;

  // Scroll metrics consumed by RecyclerView to drive the scroll bar.
  virtual float ComputeScrollOffset() const = 0; ///< Current scroll position
  virtual float ComputeScrollExtent() const = 0; ///< Viewport size
  virtual float ComputeScrollRange() const  = 0; ///< Estimated total content size

  // First and last item positions currently in the layout (including cache).
  virtual uint32_t GetFirstVisiblePosition() const = 0;
  virtual uint32_t GetLastVisiblePosition() const  = 0;

  // Item bounds in content-space coordinates (used by ScrollToPosition).
  virtual LayoutRect GetItemBounds(uint32_t position, float crossExtent) const = 0;

  // Orientation and item sizing configuration.
  virtual Orientation GetOrientation() const        = 0;
  virtual void        SetOrientation(Orientation)   = 0;
  virtual void        SetItemExtent(float extent)   = 0;
  virtual float       GetItemExtent() const         = 0;
  virtual void        SetItemSpacing(float spacing) = 0;
  virtual float       GetItemSpacing() const        = 0;

  // Called when the adapter is replaced or cleared; discard cached state.
  virtual void OnAdapterChanged() = 0;

  // Emitted by InvalidateLayout() when the layouter needs a full relayout
  // (e.g. after SetItemExtent / SetItemSpacing / SetOrientation).
  //
  // RecyclerView connects to this signal in SetItemsLayouter and disconnects
  // when the layouter is replaced. ConnectionTracker manages lifetime
  // automatically — no explicit disconnect is needed on destruction.
  //
  // NOTE: an ItemsLayouter instance must NOT be shared between multiple
  // RecyclerViews. Each RecyclerView connects its own slot; a layouter shared
  // between two views would trigger both, causing double-layout.
  Dali::Signal<void()>& LayoutInvalidatedSignal();

  // Requests a full relayout without extending the virtual subclass ABI.
  void RequestLayout();

protected:
  void InvalidateLayout();

private:
  Dali::Signal<void()> mLayoutInvalidatedSignal;
};

/**
 * @brief Optional capability for layouters that cache decoration-dependent data.
 *
 * This is deliberately separate from ItemsLayouterImpl. ItemsLayouterImpl is
 * subclassed by external libraries, so adding a virtual function to it after
 * release would change its vtable. New optional layouter capabilities must use
 * a separate, versioned extension interface in the same manner.
 */
class DALI_UI_API ItemsLayouterDecorationSupport
{
public:
  virtual ~ItemsLayouterDecorationSupport() = default;

  virtual void OnDecorationChanged() = 0;
};

} // namespace Integration

} // namespace Ui
} // namespace Dali
