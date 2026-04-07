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
#include "lottie-animation-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{
BaseHandle Create()
{
  LottieAnimationViewImplPtr impl = LottieAnimationViewImpl::New();
  Ui::View                   view(*impl);
  impl->Initialize();
  return view;
}
// clang-format off
DALI_TYPE_REGISTRATION_BEGIN(LottieAnimationViewImpl, ViewImpl, Create)

DALI_PROPERTY_REGISTRATION(Ui::Integration, LottieAnimationViewImpl, "image", STRING, IMAGE)

DALI_TYPE_REGISTRATION_END()
// clang-format on
} // namespace

LottieAnimationViewImpl::LottieAnimationViewImpl()
: ViewImpl(),
  mUrl(),
  mLoopCount(-1)
{
}

LottieAnimationViewImpl::~LottieAnimationViewImpl()
{
}

Dali::String LottieAnimationViewImpl::GetResourceUrl() const
{
  return mUrl;
}

LottieAnimationViewImplPtr LottieAnimationViewImpl::New()
{
  return new LottieAnimationViewImpl();
}

void LottieAnimationViewImpl::SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    LottieAnimationViewImpl& impl = static_cast<LottieAnimationViewImpl&>(GetImpl(view));
    switch(index)
    {
      case LottieAnimationViewImpl::Property::IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetResourceUrl(url);
        }
        break;
      }
    }
  }
}

Dali::Property::Value LottieAnimationViewImpl::GetProperty(Dali::BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::View              view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    LottieAnimationViewImpl& impl = static_cast<LottieAnimationViewImpl&>(GetImpl(view));
    switch(index)
    {
      case LottieAnimationViewImpl::Property::IMAGE:
      {
        value = impl.GetResourceUrl();
        break;
      }
    }
  }
  return value;
}

void LottieAnimationViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
}

MeasuredSize LottieAnimationViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // TODO: return natural size from lottie visual
  return ViewImpl::OnMeasure(widthConstraint, heightConstraint);
}

void LottieAnimationViewImpl::SetResourceUrl(const Dali::String& url)
{
  if(mUrl != url)
  {
    mUrl = url;
    UpdateVisual();
  }
}

void LottieAnimationViewImpl::Play()
{
  // TODO: trigger lottie play action
}

void LottieAnimationViewImpl::Pause()
{
  // TODO: trigger lottie pause action
}

void LottieAnimationViewImpl::Stop()
{
  // TODO: trigger lottie stop action
}

void LottieAnimationViewImpl::SetLoopCount(int count)
{
  if(mLoopCount != count)
  {
    // TODO: call UpdateVisual() when lottie visual is implemented
    mLoopCount = count;
  }
}

int LottieAnimationViewImpl::GetLoopCount() const
{
  return mLoopCount;
}

void LottieAnimationViewImpl::UpdateVisual()
{
  // TODO: register a lottie visual
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
