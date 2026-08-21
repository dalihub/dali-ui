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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/unique-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/web/web-view-types.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Holds an immutable snapshot of a page load failure.
 *
 * PageLoadErrorSignal provides this value through a const reference that is
 * valid only for the duration of the signal callback. Copy the value to retain
 * the error after the callback returns.
 */
class DALI_UI_API WebViewPageLoadError
{
public:
  /**
   * @brief Creates an empty page load error.
   */
  WebViewPageLoadError();

  /**
   * @brief Destructor.
   */
  ~WebViewPageLoadError();

  /**
   * @brief Creates an independent copy of a page load error.
   */
  WebViewPageLoadError(const WebViewPageLoadError& error);

  /**
   * @brief Moves a page load error.
   */
  WebViewPageLoadError(WebViewPageLoadError&& error) noexcept;

  /**
   * @brief Replaces this value with an independent copy of a page load error.
   */
  WebViewPageLoadError& operator=(const WebViewPageLoadError& error);

  /**
   * @brief Moves a page load error into this value.
   */
  WebViewPageLoadError& operator=(WebViewPageLoadError&& error) noexcept;

  /**
   * @brief Gets the URL that failed to load.
   */
  Dali::String GetUrl() const;

  /**
   * @brief Gets the error code.
   */
  WebViewPageLoadErrorCode GetCode() const;

  /**
   * @brief Gets the human-readable error description.
   */
  Dali::String GetDescription() const;

  /**
   * @brief Gets the error type.
   */
  WebViewPageLoadErrorType GetType() const;

public: // Not intended for application developers
  /// @cond internal
  DALI_INTERNAL WebViewPageLoadError(Dali::String             url,
                                     WebViewPageLoadErrorCode code,
                                     Dali::String             description,
                                     WebViewPageLoadErrorType type);
  /// @endcond

private:
  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

} // namespace Ui
} // namespace Dali
