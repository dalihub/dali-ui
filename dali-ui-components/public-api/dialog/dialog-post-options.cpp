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
 */

// CLASS HEADER
#include <dali-ui-components/public-api/dialog/dialog-post-options.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

#define DALI_ASSERT_VALID_DIALOG_POST_OPTIONS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from DialogPostOptions object")

namespace DALI_NAMESPACE
{
namespace Ui
{
class DialogPostOptions::Impl
{
public:
  bool                     mAnimated{true};
  DialogContainerStyle     mContainerStyle;
  NavigationTransitionSpec mTransitionSpec;
};

DialogPostOptions::DialogPostOptions()
: mImpl(new Impl())
{
}

DialogPostOptions::DialogPostOptions(const DialogPostOptions& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_DIALOG_POST_OPTIONS(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

DialogPostOptions::DialogPostOptions(DialogPostOptions&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

DialogPostOptions& DialogPostOptions::operator=(const DialogPostOptions& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_DIALOG_POST_OPTIONS(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

DialogPostOptions& DialogPostOptions::operator=(DialogPostOptions&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

DialogPostOptions::~DialogPostOptions()
{
  delete mImpl;
}

void DialogPostOptions::SetAnimated(bool animated)
{
  DALI_ASSERT_VALID_DIALOG_POST_OPTIONS(mImpl);
  mImpl->mAnimated = animated;
}

bool DialogPostOptions::IsAnimated() const
{
  DALI_ASSERT_VALID_DIALOG_POST_OPTIONS(mImpl);
  return mImpl->mAnimated;
}

void DialogPostOptions::SetContainerStyle(const DialogContainerStyle& style)
{
  DALI_ASSERT_VALID_DIALOG_POST_OPTIONS(mImpl);
  mImpl->mContainerStyle = style;
}

const DialogContainerStyle& DialogPostOptions::GetContainerStyle() const
{
  DALI_ASSERT_VALID_DIALOG_POST_OPTIONS(mImpl);
  return mImpl->mContainerStyle;
}

void DialogPostOptions::SetTransitionSpec(const NavigationTransitionSpec& spec)
{
  DALI_ASSERT_VALID_DIALOG_POST_OPTIONS(mImpl);
  mImpl->mTransitionSpec = spec;
}

const NavigationTransitionSpec& DialogPostOptions::GetTransitionSpec() const
{
  DALI_ASSERT_VALID_DIALOG_POST_OPTIONS(mImpl);
  return mImpl->mTransitionSpec;
}
} // namespace Ui
} // namespace DALI_NAMESPACE
