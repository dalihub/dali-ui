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

namespace DALI_NAMESPACE
{
namespace Ui
{

/**
 * @brief Holds an immutable snapshot of a page load failure.
 *
 * PageLoadErrorSignal provides this value through a const reference that is
 * valid only for the duration of the signal callback. Copy the value to retain
 * the error after the callback returns.
 * @SINCE_3_0.0
 */
class DALI_UI_API WebViewPageLoadError
{
public:
  /**
   * @brief Creates an empty page load error.
   * @SINCE_3_0.0
   */
  WebViewPageLoadError();

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~WebViewPageLoadError();

  /**
   * @brief Creates an independent copy of a page load error.
   * @SINCE_3_0.0
   */
  WebViewPageLoadError(const WebViewPageLoadError& error);

  /**
   * @brief Moves a page load error.
   * @SINCE_3_0.0
   */
  WebViewPageLoadError(WebViewPageLoadError&& error) noexcept;

  /**
   * @brief Replaces this value with an independent copy of a page load error.
   * @SINCE_3_0.0
   */
  WebViewPageLoadError& operator=(const WebViewPageLoadError& error);

  /**
   * @brief Moves a page load error into this value.
   * @SINCE_3_0.0
   */
  WebViewPageLoadError& operator=(WebViewPageLoadError&& error) noexcept;

  /**
   * @brief Gets the URL that failed to load.
   * @SINCE_3_0.0
   */
  Dali::String GetUrl() const;

  /**
   * @brief Gets the error code.
   * @SINCE_3_0.0
   */
  WebViewPageLoadErrorCode GetCode() const;

  /**
   * @brief Gets the human-readable error description.
   * @SINCE_3_0.0
   */
  Dali::String GetDescription() const;

  /**
   * @brief Gets the error type.
   * @SINCE_3_0.0
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
} //namespace DALI_NAMESPACE
