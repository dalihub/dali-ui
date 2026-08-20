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
#include <cstdint>

namespace Dali
{
namespace Ui
{

/**
 * @brief Enumeration for the scroll edge.
 */
enum class WebViewScrollEdge
{
  LEFT,   ///< Left edge reached.
  RIGHT,  ///< Right edge reached.
  TOP,    ///< Top edge reached.
  BOTTOM, ///< Bottom edge reached.
};

/**
 * @brief Enumeration for the over-scroll direction.
 */
enum class WebViewOverScrolled
{
  LEFT,   ///< Left over-scrolled.
  RIGHT,  ///< Right over-scrolled.
  TOP,    ///< Top over-scrolled.
  BOTTOM, ///< Bottom over-scrolled.
};

/**
 * @brief Enumeration for the find-text options (can be combined with bitwise OR).
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
 */
inline WebViewFindOption operator|(WebViewFindOption lhs, WebViewFindOption rhs)
{
  return static_cast<WebViewFindOption>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

/**
 * @brief Bitwise OR assignment operator for WebViewFindOption.
 */
inline WebViewFindOption& operator|=(WebViewFindOption& lhs, WebViewFindOption rhs)
{
  lhs = lhs | rhs;
  return lhs;
}

/**
 * @brief Bitwise AND operator for WebViewFindOption.
 */
inline WebViewFindOption operator&(WebViewFindOption lhs, WebViewFindOption rhs)
{
  return static_cast<WebViewFindOption>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

/**
 * @brief Bitwise NOT operator for WebViewFindOption.
 */
inline WebViewFindOption operator~(WebViewFindOption option)
{
  return static_cast<WebViewFindOption>(~static_cast<uint32_t>(option));
}

/**
 * @brief Enumeration for the error code of a page load failure.
 *
 * The values mirror the underlying web engine's load-error codes.
 */
enum class WebViewPageLoadErrorCode
{
  UNKNOWN = 0,           ///< Unknown error.
  CANCELED,              ///< User canceled.
  CANT_SUPPORT_MIMETYPE, ///< Can't show the page for this MIME type.
  FAILED_FILE_IO,        ///< File IO error.
  CANT_CONNECT,          ///< Cannot connect to the network.
  CANT_LOOKUP_HOST,      ///< Failed to look up the host from the DNS.
  FAILED_TLS_HANDSHAKE,  ///< Failed the SSL/TLS handshake.
  INVALID_CERTIFICATE,   ///< The received certificate is invalid.
  REQUEST_TIMEOUT,       ///< Connection timeout.
  TOO_MANY_REDIRECTS,    ///< Too many redirects.
  TOO_MANY_REQUESTS,     ///< Too many requests during this load.
  BAD_URL,               ///< Malformed URL.
  UNSUPPORTED_SCHEME,    ///< Unsupported scheme.
  AUTHENTICATION,        ///< User authentication failed on the server.
  INTERNAL_SERVER,       ///< The web server reported an internal error.
  OTHER,                 ///< Other error.
};

/**
 * @brief Enumeration for the type of a page load failure.
 */
enum class WebViewPageLoadErrorType
{
  NONE,     ///< No error type.
  INTERNAL, ///< Internal error.
  NETWORK,  ///< Network error.
  POLICY,   ///< Policy error.
  PLUGIN,   ///< Plugin error.
  DOWNLOAD, ///< Download error.
  PRINT,    ///< Print error.
};

} // namespace Ui

} // namespace Dali
