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

#include <dali-ui-components/public-api/window/window-frame-options.h>

#include <dali/public-api/common/dali-common.h>

#define DALI_ASSERT_VALID_WINDOW_FRAME_OPTIONS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from WindowFrameOptions object")

namespace Dali::Ui
{

struct WindowFrameOptions::Impl
{
  WindowFrameInitialStatePolicy initialStatePolicy{WindowFrameInitialStatePolicy::AUTO};
  Dali::Vector2                 initialRestoreFrameSize;
  bool                          hasInitialRestoreFrameSize{false};
  bool                          moveEnabled{true};
  bool                          resizeEnabled{true};
  bool                          minimizeEnabled{true};
  bool                          maximizeRestoreEnabled{true};
};

WindowFrameOptions::WindowFrameOptions()
: mImpl(Dali::MakeUnique<Impl>())
{
}

WindowFrameOptions::WindowFrameOptions(const WindowFrameOptions& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_OPTIONS(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
}

WindowFrameOptions::WindowFrameOptions(WindowFrameOptions&& rhs) noexcept = default;

WindowFrameOptions& WindowFrameOptions::operator=(const WindowFrameOptions& rhs)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_OPTIONS(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
  return *this;
}

WindowFrameOptions& WindowFrameOptions::operator=(WindowFrameOptions&& rhs) noexcept = default;
WindowFrameOptions::~WindowFrameOptions()                                            = default;

void WindowFrameOptions::SetInitialStatePolicy(WindowFrameInitialStatePolicy policy)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_OPTIONS(mImpl);
  mImpl->initialStatePolicy = policy;
}

WindowFrameInitialStatePolicy WindowFrameOptions::GetInitialStatePolicy() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_OPTIONS(mImpl);
  return mImpl->initialStatePolicy;
}

void WindowFrameOptions::SetInitialRestoreFrameSize(const Dali::Vector2& size)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_OPTIONS(mImpl);
  mImpl->initialRestoreFrameSize    = size;
  mImpl->hasInitialRestoreFrameSize = true;
}

void WindowFrameOptions::ClearInitialRestoreFrameSize()
{
  DALI_ASSERT_VALID_WINDOW_FRAME_OPTIONS(mImpl);
  mImpl->initialRestoreFrameSize    = Dali::Vector2::ZERO;
  mImpl->hasInitialRestoreFrameSize = false;
}

bool WindowFrameOptions::GetInitialRestoreFrameSize(Dali::Vector2& size) const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_OPTIONS(mImpl);
  if(!mImpl->hasInitialRestoreFrameSize)
  {
    return false;
  }

  size = mImpl->initialRestoreFrameSize;
  return true;
}

void WindowFrameOptions::SetFeatureEnabled(WindowFrameFeature feature, bool enabled)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_OPTIONS(mImpl);
  switch(feature)
  {
    case WindowFrameFeature::MOVE:
      mImpl->moveEnabled = enabled;
      break;
    case WindowFrameFeature::RESIZE:
      mImpl->resizeEnabled = enabled;
      break;
    case WindowFrameFeature::MINIMIZE:
      mImpl->minimizeEnabled = enabled;
      break;
    case WindowFrameFeature::MAXIMIZE_RESTORE:
      mImpl->maximizeRestoreEnabled = enabled;
      break;
  }
}

bool WindowFrameOptions::IsFeatureEnabled(WindowFrameFeature feature) const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_OPTIONS(mImpl);
  switch(feature)
  {
    case WindowFrameFeature::MOVE:
      return mImpl->moveEnabled;
    case WindowFrameFeature::RESIZE:
      return mImpl->resizeEnabled;
    case WindowFrameFeature::MINIMIZE:
      return mImpl->minimizeEnabled;
    case WindowFrameFeature::MAXIMIZE_RESTORE:
      return mImpl->maximizeRestoreEnabled;
  }
  return false;
}

} // namespace Dali::Ui

#undef DALI_ASSERT_VALID_WINDOW_FRAME_OPTIONS
