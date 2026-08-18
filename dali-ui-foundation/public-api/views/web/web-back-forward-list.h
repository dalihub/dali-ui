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
#include <dali-ui-foundation/public-api/views/web/web-back-forward-list-item.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/base-handle.h>
#include <cstdint>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class WebBackForwardListImpl;
} // namespace Integration DALI_INTERNAL

/**
 * @brief Provides a live, read-only view of a WebView navigation history.
 *
 * Each query reads the latest history state from the associated web engine.
 * Items returned by a query are independent snapshots.
 */
class DALI_UI_API WebBackForwardList : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized handle.
   */
  WebBackForwardList();

  /**
   * @brief Destructor.
   */
  ~WebBackForwardList();

  /**
   * @brief Copy constructor.
   */
  WebBackForwardList(const WebBackForwardList& list);

  /**
   * @brief Move constructor.
   */
  WebBackForwardList(WebBackForwardList&& list) noexcept;

  /**
   * @brief Copy assignment operator.
   */
  WebBackForwardList& operator=(const WebBackForwardList& list);

  /**
   * @brief Move assignment operator.
   */
  WebBackForwardList& operator=(WebBackForwardList&& list) noexcept;

  /**
   * @brief Downcasts a handle to WebBackForwardList.
   */
  static WebBackForwardList DownCast(BaseHandle handle);

  /**
   * @brief Gets the current history item.
   * @return A snapshot of the item, or an invalid item when unavailable
   */
  WebBackForwardListItem GetCurrentItem() const;

  /**
   * @brief Gets the item immediately preceding the current item.
   * @return A snapshot of the item, or an invalid item when unavailable
   */
  WebBackForwardListItem GetPreviousItem() const;

  /**
   * @brief Gets the item immediately following the current item.
   * @return A snapshot of the item, or an invalid item when unavailable
   */
  WebBackForwardListItem GetNextItem() const;

  /**
   * @brief Gets an item at the backend-defined history index.
   * @param[in] index The item index
   * @return A snapshot of the item, or an invalid item when unavailable
   */
  WebBackForwardListItem GetItemAtIndex(uint32_t index) const;

  /**
   * @brief Gets the number of history items, including the current item.
   */
  uint32_t GetItemCount() const;

  /**
   * @brief Gets items preceding the current item, oldest first.
   * @param[in] limit Maximum number of items, or -1 for all items
   */
  Dali::Vector<WebBackForwardListItem> GetBackwardItems(int32_t limit = -1) const;

  /**
   * @brief Gets items following the current item, oldest first.
   * @param[in] limit Maximum number of items, or -1 for all items
   */
  Dali::Vector<WebBackForwardListItem> GetForwardItems(int32_t limit = -1) const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL WebBackForwardList(Integration::WebBackForwardListImpl& implementation);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
