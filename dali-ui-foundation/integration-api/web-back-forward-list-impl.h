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

#include <dali-ui-foundation/public-api/views/web/web-back-forward-list.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <memory>

namespace Dali
{
class WebEngineBackForwardListItem;

namespace Ui
{
namespace Integration
{

class WebBackForwardListImpl;
using WebBackForwardListImplPtr = IntrusivePtr<WebBackForwardListImpl>;

/**
 * @brief Internal implementation of WebBackForwardList.
 */
class DALI_UI_API WebBackForwardListImpl : public BaseObject
{
public:
  static WebBackForwardListImplPtr New(Dali::WebEngine webEngine);

  WebBackForwardListItem               GetCurrentItem() const;
  WebBackForwardListItem               GetPreviousItem() const;
  WebBackForwardListItem               GetNextItem() const;
  WebBackForwardListItem               GetItemAtIndex(uint32_t index) const;
  uint32_t                             GetItemCount() const;
  Dali::Vector<WebBackForwardListItem> GetBackwardItems(int32_t limit) const;
  Dali::Vector<WebBackForwardListItem> GetForwardItems(int32_t limit) const;

protected:
  explicit WebBackForwardListImpl(Dali::WebEngine webEngine);
  ~WebBackForwardListImpl() override;

private:
  WebBackForwardListItem CreateItem(std::unique_ptr<Dali::WebEngineBackForwardListItem> item) const;

private:
  Dali::WeakHandle<Dali::WebEngine> mWebEngine;
};

inline WebBackForwardListImpl& GetImpl(WebBackForwardList& list)
{
  DALI_ASSERT_ALWAYS(list);
  return static_cast<WebBackForwardListImpl&>(list.GetBaseObject());
}

inline const WebBackForwardListImpl& GetImpl(const WebBackForwardList& list)
{
  DALI_ASSERT_ALWAYS(list);
  return static_cast<const WebBackForwardListImpl&>(list.GetBaseObject());
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
