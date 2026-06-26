/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include "test-config.h"
#include "test-theme-loader.h"

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Provider;

class TestConfigImpl : public UiConfigImpl
{
public:
  ThemeLoaderInterface* CreateThemeLoader() override
  {
    auto* loader = new TestThemeLoader();
    SetTestThemeLoader(loader);
    return loader;
  }
};

TestConfig TestConfig::New()
{
  IntrusivePtr<TestConfigImpl> impl = new TestConfigImpl();
  TestConfig handle(impl.Get());
  return handle;
}
