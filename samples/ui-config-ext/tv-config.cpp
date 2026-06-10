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

#include "tv-config.h"

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

namespace
{

void TVViewInitializer(View view)
{
  UiConfig::DefaultViewInitializer(view);

  view.SetBackgroundColor(UiColor(0x00FFFF));

  Label label = Label::DownCast(view);
  if(label)
  {
    label.SetTextColor(UiColor(0x0000FF));
  }
}

} // unnamed namespace

class TVConfigImpl : public UiConfigImpl
{
};

TVConfig::TVConfig(UiConfigImpl* impl)
: UiConfig(impl)
{
}

TVConfig TVConfig::New()
{
  IntrusivePtr<TVConfigImpl> impl = new TVConfigImpl();
  TVConfig                   handle(impl.Get());

  // 간단히 구현하기 위해 이 구조로 했지만, UiConfigImpl을 상속 받는것이 사업부 전용 기능 확장에 좋습니다.
  handle.SetViewInitializer(TVViewInitializer);
  return handle;
}

