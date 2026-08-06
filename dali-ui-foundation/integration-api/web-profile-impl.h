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

#include <dali-ui-foundation/public-api/views/web/web-profile.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
class WebEngineContext;
class WebEngineCookieManager;

namespace Ui
{
namespace Integration
{

class WebProfileImpl;
using WebProfileImplPtr = IntrusivePtr<WebProfileImpl>;

/**
 * @brief Internal implementation of WebProfile.
 */
class DALI_UI_API WebProfileImpl : public BaseObject
{
public:
  static WebProfileImplPtr New(Dali::WebEngineContext* context, Dali::WebEngineCookieManager* cookieManager);

  void             ClearCache();
  WebCookieManager GetCookieManager() const;
  bool             IsEqual(const WebProfileImpl& rhs) const;

protected:
  WebProfileImpl(Dali::WebEngineContext* context, Dali::WebEngineCookieManager* cookieManager);
  ~WebProfileImpl() override;

private:
  Dali::WebEngineContext*       mContext;
  Dali::WebEngineCookieManager* mCookieManager;
  mutable WebCookieManager      mWebCookieManager;
};

inline WebProfileImpl& GetImpl(WebProfile& profile)
{
  DALI_ASSERT_ALWAYS(profile);
  return static_cast<WebProfileImpl&>(profile.GetBaseObject());
}

inline const WebProfileImpl& GetImpl(const WebProfile& profile)
{
  DALI_ASSERT_ALWAYS(profile);
  return static_cast<const WebProfileImpl&>(profile.GetBaseObject());
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
