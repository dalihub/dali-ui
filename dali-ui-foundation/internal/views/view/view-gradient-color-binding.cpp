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
#include <dali-ui-foundation/internal/views/view/view-gradient-color-binding.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/traits/attachment-id.h>
#include <dali-ui-foundation/public-api/types/unique-any.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace ViewGradientColorBinding
{
namespace
{

const AttachmentId GRADIENT_COLOR_BINDING_DATA_ATTACHMENT_ID = AttachmentId::Alloc();

bool MatchesStringView(const Dali::String& string, StringView stringView)
{
  return StringView(string) == stringView;
}

class GradientColorBindingData
{
public:
  GradientColorBindingData(StringView bindingId, const Gradient::Base& gradient, Callback<void(const Gradient::Base&)> callback)
  : mBindingId(bindingId),
    mGradient(gradient),
    mCallback(std::move(callback))
  {
  }

  bool Matches(StringView bindingId) const
  {
    return MatchesStringView(mBindingId, bindingId);
  }

  void SetGradient(const Gradient::Base& gradient)
  {
    mGradient = gradient;
  }

  void Invoke()
  {
    mCallback.Invoke(mGradient);
  }

private:
  Dali::String                          mBindingId;
  Gradient::Base                        mGradient;
  Callback<void(const Gradient::Base&)> mCallback;
};

using GradientColorBindingDataList = std::vector<GradientColorBindingData>;

GradientColorBindingDataList* GetDataList(ViewImpl& viewImpl)
{
  UniqueAny* attachment = viewImpl.GetAttachment(GRADIENT_COLOR_BINDING_DATA_ATTACHMENT_ID);
  return attachment ? attachment->Get<GradientColorBindingDataList>() : nullptr;
}

GradientColorBindingData* FindData(ViewImpl& viewImpl, StringView bindingId)
{
  GradientColorBindingDataList* dataList = GetDataList(viewImpl);
  if(!dataList)
  {
    return nullptr;
  }

  auto iter = std::find_if(dataList->begin(), dataList->end(), [bindingId](const GradientColorBindingData& data)
  {
    return data.Matches(bindingId);
  });
  return iter == dataList->end() ? nullptr : &(*iter);
}

} // namespace

bool HasTokenColor(const Gradient::Base& gradient)
{
  Dali::Vector<Gradient::StopNode> stopNodes = gradient.GetStopNodes();
  for(const auto& stopNode : stopNodes)
  {
    if(stopNode.GetColor().HasColorId())
    {
      return true;
    }
  }
  return false;
}

bool Add(ViewImpl& viewImpl, StringView bindingId, const Gradient::Base& gradient, Callback<void(const Gradient::Base&)> callback)
{
  GradientColorBindingDataList* dataList = GetDataList(viewImpl);
  if(dataList)
  {
    dataList->emplace_back(bindingId, gradient, std::move(callback));
    return false;
  }
  else
  {
    GradientColorBindingDataList newDataList;
    newDataList.emplace_back(bindingId, gradient, std::move(callback));
    viewImpl.SetAttachment(GRADIENT_COLOR_BINDING_DATA_ATTACHMENT_ID, UniqueAny(Dali::Move(newDataList)));
    return true;
  }
}

bool Update(ViewImpl& viewImpl, StringView bindingId, const Gradient::Base& gradient)
{
  if(GradientColorBindingData* data = FindData(viewImpl, bindingId))
  {
    data->SetGradient(gradient);
    return true;
  }
  return false;
}

void ApplyAll(ViewImpl& viewImpl)
{
  GradientColorBindingDataList* dataList = GetDataList(viewImpl);
  if(!dataList)
  {
    return;
  }

  for(auto& data : *dataList)
  {
    data.Invoke();
  }
}

bool Clear(ViewImpl& viewImpl, StringView bindingId)
{
  GradientColorBindingDataList* dataList = GetDataList(viewImpl);
  if(dataList)
  {
    dataList->erase(std::remove_if(dataList->begin(), dataList->end(), [bindingId](const GradientColorBindingData& data)
    {
      return data.Matches(bindingId);
    }),
                    dataList->end());

    if(dataList->empty())
    {
      viewImpl.RemoveAttachment(GRADIENT_COLOR_BINDING_DATA_ATTACHMENT_ID);
      return true;
    }
  }
  return false;
}

} // namespace ViewGradientColorBinding
} // namespace Internal
} // namespace Ui
} // namespace Dali
