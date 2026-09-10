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
#include <dali-ui-foundation/public-api/image/lottie-animation-dynamic-property.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

#include <utility>

#define DALI_ASSERT_VALID_DYNAMIC_PROPERTY(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from DynamicProperty object")

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace LottieAnimation
{
class DynamicProperty::Impl
{
public:
  Impl()
  : mKeyPath(),
    mCallback(),
    mId(0),
    mProperty(ContentProperty::FILL_COLOR)
  {
  }

  Impl(int32_t id, const Dali::String& keyPath, ContentProperty contentProperty, DynamicPropertyCallback callback)
  : mKeyPath(keyPath),
    mCallback(std::move(callback)),
    mId(id),
    mProperty(contentProperty)
  {
  }

  Dali::String            mKeyPath;
  DynamicPropertyCallback mCallback;
  int32_t                 mId;
  ContentProperty         mProperty;
};

DynamicProperty::DynamicProperty()
: mImpl(new Impl())
{
}

DynamicProperty::DynamicProperty(int32_t id, const Dali::String& keyPath, ContentProperty contentProperty, DynamicPropertyCallback callback)
: mImpl(new Impl(id, keyPath, contentProperty, std::move(callback)))
{
}

DynamicProperty::DynamicProperty(DynamicProperty&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

DynamicProperty& DynamicProperty::operator=(DynamicProperty&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

DynamicProperty::~DynamicProperty()
{
  delete mImpl;
}

DynamicProperty& DynamicProperty::SetId(int32_t id)
{
  DALI_ASSERT_VALID_DYNAMIC_PROPERTY(mImpl);
  mImpl->mId = id;
  return *this;
}

int32_t DynamicProperty::GetId() const
{
  DALI_ASSERT_VALID_DYNAMIC_PROPERTY(mImpl);
  return mImpl->mId;
}

DynamicProperty& DynamicProperty::SetKeyPath(const Dali::String& keyPath)
{
  DALI_ASSERT_VALID_DYNAMIC_PROPERTY(mImpl);
  mImpl->mKeyPath = keyPath;
  return *this;
}

const Dali::String& DynamicProperty::GetKeyPath() const
{
  DALI_ASSERT_VALID_DYNAMIC_PROPERTY(mImpl);
  return mImpl->mKeyPath;
}

DynamicProperty& DynamicProperty::SetProperty(ContentProperty contentProperty)
{
  DALI_ASSERT_VALID_DYNAMIC_PROPERTY(mImpl);
  mImpl->mProperty = contentProperty;
  return *this;
}

ContentProperty DynamicProperty::GetProperty() const
{
  DALI_ASSERT_VALID_DYNAMIC_PROPERTY(mImpl);
  return mImpl->mProperty;
}

DynamicProperty& DynamicProperty::SetCallback(DynamicPropertyCallback callback)
{
  DALI_ASSERT_VALID_DYNAMIC_PROPERTY(mImpl);
  mImpl->mCallback = std::move(callback);
  return *this;
}

DynamicPropertyCallback& DynamicProperty::GetCallback()
{
  DALI_ASSERT_VALID_DYNAMIC_PROPERTY(mImpl);
  return mImpl->mCallback;
}

const DynamicPropertyCallback& DynamicProperty::GetCallback() const
{
  DALI_ASSERT_VALID_DYNAMIC_PROPERTY(mImpl);
  return mImpl->mCallback;
}

} // namespace LottieAnimation

} // namespace Ui

} // namespace DALI_NAMESPACE
