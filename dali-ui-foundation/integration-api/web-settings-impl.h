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

#include <dali-ui-foundation/public-api/views/web/web-settings.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class WebSettingsImpl;
using WebSettingsImplPtr = IntrusivePtr<WebSettingsImpl>;

/**
 * @brief Internal implementation of WebSettings.
 */
class DALI_UI_API WebSettingsImpl : public BaseObject
{
public:
  /**
   * @brief Creates settings bound weakly to a web engine.
   */
  static WebSettingsImplPtr New(Dali::WebEngine webEngine);

  bool         SetExtraFeatureValue(const Dali::String& feature, const Dali::String& value);
  Dali::String GetExtraFeatureValue(const Dali::String& feature) const;
  void         SetSpatialNavigationEnabled(bool enabled);
  uint32_t     GetDefaultFontSize() const;
  void         SetDefaultFontSize(uint32_t defaultFontSize);
  void         SetWebSecurityEnabled(bool enabled);
  void         SetExtraFeatureEnabled(const Dali::String& feature, bool enabled);
  bool         IsExtraFeatureEnabled(const Dali::String& feature) const;

protected:
  explicit WebSettingsImpl(Dali::WebEngine webEngine);
  ~WebSettingsImpl() override;

private:
  Dali::WeakHandle<Dali::WebEngine> mWebEngine;
};

inline WebSettingsImpl& GetImpl(WebSettings& settings)
{
  DALI_ASSERT_ALWAYS(settings);
  return static_cast<WebSettingsImpl&>(settings.GetBaseObject());
}

inline const WebSettingsImpl& GetImpl(const WebSettings& settings)
{
  DALI_ASSERT_ALWAYS(settings);
  return static_cast<const WebSettingsImpl&>(settings.GetBaseObject());
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
