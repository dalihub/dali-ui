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

// CLASS HEADER
#include <dali-ui-foundation/public-api/views/web/web-view-page-load-error.h>

// EXTERNAL INCLUDES
#include <utility>

namespace Dali
{
namespace Ui
{

struct WebViewPageLoadError::Impl
{
  Impl() = default;

  Impl(Dali::String             errorUrl,
       WebViewPageLoadErrorCode errorCode,
       Dali::String             errorDescription,
       WebViewPageLoadErrorType errorType)
  : mUrl(std::move(errorUrl)),
    mCode(errorCode),
    mDescription(std::move(errorDescription)),
    mType(errorType)
  {
  }

  Dali::String             mUrl;
  WebViewPageLoadErrorCode mCode{WebViewPageLoadErrorCode::UNKNOWN};
  Dali::String             mDescription;
  WebViewPageLoadErrorType mType{WebViewPageLoadErrorType::NONE};
};

WebViewPageLoadError::WebViewPageLoadError()
: mImpl(new Impl())
{
}

WebViewPageLoadError::~WebViewPageLoadError() = default;

WebViewPageLoadError::WebViewPageLoadError(const WebViewPageLoadError& error)
: mImpl(error.mImpl ? new Impl(*error.mImpl) : new Impl())
{
}

WebViewPageLoadError::WebViewPageLoadError(WebViewPageLoadError&& error) noexcept = default;

WebViewPageLoadError& WebViewPageLoadError::operator=(const WebViewPageLoadError& error)
{
  if(this != &error)
  {
    mImpl.Reset(error.mImpl ? new Impl(*error.mImpl) : new Impl());
  }
  return *this;
}

WebViewPageLoadError& WebViewPageLoadError::operator=(WebViewPageLoadError&& error) noexcept = default;

Dali::String WebViewPageLoadError::GetUrl() const
{
  return mImpl ? mImpl->mUrl : Dali::String();
}

WebViewPageLoadErrorCode WebViewPageLoadError::GetCode() const
{
  return mImpl ? mImpl->mCode : WebViewPageLoadErrorCode::UNKNOWN;
}

Dali::String WebViewPageLoadError::GetDescription() const
{
  return mImpl ? mImpl->mDescription : Dali::String();
}

WebViewPageLoadErrorType WebViewPageLoadError::GetType() const
{
  return mImpl ? mImpl->mType : WebViewPageLoadErrorType::NONE;
}

WebViewPageLoadError::WebViewPageLoadError(Dali::String             url,
                                           WebViewPageLoadErrorCode code,
                                           Dali::String             description,
                                           WebViewPageLoadErrorType type)
: mImpl(new Impl(std::move(url), code, std::move(description), type))
{
}

} // namespace Ui
} // namespace Dali
