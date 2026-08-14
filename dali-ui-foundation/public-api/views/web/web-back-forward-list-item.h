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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/unique-ptr.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Holds a snapshot of one entry in a WebView back-forward list.
 *
 * An item does not change when the WebView navigates after the item has been
 * retrieved.
 */
class DALI_UI_API WebBackForwardListItem
{
public:
  /**
   * @brief Creates an invalid item.
   */
  WebBackForwardListItem();

  /**
   * @brief Destructor.
   */
  ~WebBackForwardListItem();

  /**
   * @brief Copy constructor.
   */
  WebBackForwardListItem(const WebBackForwardListItem& item);

  /**
   * @brief Move constructor.
   */
  WebBackForwardListItem(WebBackForwardListItem&& item) noexcept;

  /**
   * @brief Copy assignment operator.
   */
  WebBackForwardListItem& operator=(const WebBackForwardListItem& item);

  /**
   * @brief Move assignment operator.
   */
  WebBackForwardListItem& operator=(WebBackForwardListItem&& item) noexcept;

  /**
   * @brief Returns whether this item contains a history entry.
   */
  bool IsValid() const;

  /**
   * @brief Gets the final URL of the history entry.
   */
  Dali::String GetUrl() const;

  /**
   * @brief Gets the title of the history entry.
   */
  Dali::String GetTitle() const;

  /**
   * @brief Gets the URL originally requested for the history entry.
   */
  Dali::String GetOriginalUrl() const;

public: // Not intended for application developers
  /// @cond internal
  DALI_INTERNAL WebBackForwardListItem(Dali::String url, Dali::String title, Dali::String originalUrl);
  /// @endcond

private:
  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

} // namespace Ui
} // namespace Dali
