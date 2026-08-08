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

#include <dali-ui-foundation/public-api/views/web/web-cookie-manager.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
class WebEngineCookieManager;

namespace Ui
{
namespace Integration
{

class WebCookieManagerImpl;
using WebCookieManagerImplPtr = IntrusivePtr<WebCookieManagerImpl>;

/**
 * @brief Internal implementation of WebCookieManager.
 */
class DALI_UI_API WebCookieManagerImpl : public BaseObject
{
public:
  static WebCookieManagerImplPtr New(Dali::WebEngineCookieManager* cookieManager);

  void ClearAllCookies();
  bool IsEqual(const WebCookieManagerImpl& rhs) const;

protected:
  explicit WebCookieManagerImpl(Dali::WebEngineCookieManager* cookieManager);
  ~WebCookieManagerImpl() override;

private:
  Dali::WebEngineCookieManager* mCookieManager;
};

inline WebCookieManagerImpl& GetImpl(WebCookieManager& manager)
{
  DALI_ASSERT_ALWAYS(manager);
  return static_cast<WebCookieManagerImpl&>(manager.GetBaseObject());
}

inline const WebCookieManagerImpl& GetImpl(const WebCookieManager& manager)
{
  DALI_ASSERT_ALWAYS(manager);
  return static_cast<const WebCookieManagerImpl&>(manager.GetBaseObject());
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
