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
#include <dali/public-api/common/dali-namespace.h>
#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{

/**
 * @brief Enumeration for the scroll edge.
 * @SINCE_3_0.0
 */
enum class WebViewScrollEdge
{
  LEFT,   ///< Left edge reached. @SINCE_3_0.0
  RIGHT,  ///< Right edge reached. @SINCE_3_0.0
  TOP,    ///< Top edge reached. @SINCE_3_0.0
  BOTTOM, ///< Bottom edge reached. @SINCE_3_0.0
};

/**
 * @brief Enumeration for the over-scroll direction.
 * @SINCE_3_0.0
 */
enum class WebViewOverScrolled
{
  LEFT,   ///< Left over-scrolled. @SINCE_3_0.0
  RIGHT,  ///< Right over-scrolled. @SINCE_3_0.0
  TOP,    ///< Top over-scrolled. @SINCE_3_0.0
  BOTTOM, ///< Bottom over-scrolled. @SINCE_3_0.0
};

/**
 * @brief Enumeration for the find-text options (can be combined with bitwise OR).
 * @SINCE_3_0.0
 */
enum class WebViewFindOption : uint32_t
{
  NONE                               = 0,
  CASE_INSENSITIVE                   = 1 << 0,
  AT_WORD_STARTS                     = 1 << 1,
  TREAT_MEDIAL_CAPITAL_AS_WORD_START = 1 << 2,
  BACKWARDS                          = 1 << 3,
  WRAP_AROUND                        = 1 << 4,
  SHOW_OVERLAY                       = 1 << 5,
  SHOW_FIND_INDICATOR                = 1 << 6,
  SHOW_HIGHLIGHT                     = 1 << 7,
};

/**
 * @brief Bitwise OR operator for WebViewFindOption.
 * @SINCE_3_0.0
 */
inline WebViewFindOption operator|(WebViewFindOption lhs, WebViewFindOption rhs)
{
  return static_cast<WebViewFindOption>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

/**
 * @brief Bitwise OR assignment operator for WebViewFindOption.
 * @SINCE_3_0.0
 */
inline WebViewFindOption& operator|=(WebViewFindOption& lhs, WebViewFindOption rhs)
{
  lhs = lhs | rhs;
  return lhs;
}

/**
 * @brief Bitwise AND operator for WebViewFindOption.
 * @SINCE_3_0.0
 */
inline WebViewFindOption operator&(WebViewFindOption lhs, WebViewFindOption rhs)
{
  return static_cast<WebViewFindOption>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

/**
 * @brief Bitwise NOT operator for WebViewFindOption.
 * @SINCE_3_0.0
 */
inline WebViewFindOption operator~(WebViewFindOption option)
{
  return static_cast<WebViewFindOption>(~static_cast<uint32_t>(option));
}

/**
 * @brief Enumeration for the error code of a page load failure.
 *
 * The values mirror the underlying web engine's load-error codes.
 * @SINCE_3_0.0
 */
enum class WebViewPageLoadErrorCode
{
  UNKNOWN = 0,           ///< Unknown error. @SINCE_3_0.0
  CANCELED,              ///< User canceled. @SINCE_3_0.0
  CANT_SUPPORT_MIMETYPE, ///< Can't show the page for this MIME type. @SINCE_3_0.0
  FAILED_FILE_IO,        ///< File IO error. @SINCE_3_0.0
  CANT_CONNECT,          ///< Cannot connect to the network. @SINCE_3_0.0
  CANT_LOOKUP_HOST,      ///< Failed to look up the host from the DNS. @SINCE_3_0.0
  FAILED_TLS_HANDSHAKE,  ///< Failed the SSL/TLS handshake. @SINCE_3_0.0
  INVALID_CERTIFICATE,   ///< The received certificate is invalid. @SINCE_3_0.0
  REQUEST_TIMEOUT,       ///< Connection timeout. @SINCE_3_0.0
  TOO_MANY_REDIRECTS,    ///< Too many redirects. @SINCE_3_0.0
  TOO_MANY_REQUESTS,     ///< Too many requests during this load. @SINCE_3_0.0
  BAD_URL,               ///< Malformed URL. @SINCE_3_0.0
  UNSUPPORTED_SCHEME,    ///< Unsupported scheme. @SINCE_3_0.0
  AUTHENTICATION,        ///< User authentication failed on the server. @SINCE_3_0.0
  INTERNAL_SERVER,       ///< The web server reported an internal error. @SINCE_3_0.0
  OTHER,                 ///< Other error. @SINCE_3_0.0
};

/**
 * @brief Enumeration for the type of a page load failure.
 * @SINCE_3_0.0
 */
enum class WebViewPageLoadErrorType
{
  NONE,     ///< No error type. @SINCE_3_0.0
  INTERNAL, ///< Internal error. @SINCE_3_0.0
  NETWORK,  ///< Network error. @SINCE_3_0.0
  POLICY,   ///< Policy error. @SINCE_3_0.0
  PLUGIN,   ///< Plugin error. @SINCE_3_0.0
  DOWNLOAD, ///< Download error. @SINCE_3_0.0
  PRINT,    ///< Print error. @SINCE_3_0.0
};

} // namespace Ui

} //namespace DALI_NAMESPACE
