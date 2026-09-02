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

#include <dali-ui-components/internal/window/default-window-decoration-impl.h>
#include <dali-ui-components/internal/window/window-frame-decoration-validator.h>
#include <dali-ui-components/public-api/window/default-window-decoration.h>

#include <dali/integration-api/debug.h>

namespace Dali::Ui
{

DefaultWindowDecoration::DefaultWindowDecoration()                                                  = default;
DefaultWindowDecoration::~DefaultWindowDecoration()                                                 = default;
DefaultWindowDecoration::DefaultWindowDecoration(const DefaultWindowDecoration& rhs)                = default;
DefaultWindowDecoration::DefaultWindowDecoration(DefaultWindowDecoration&& rhs) noexcept            = default;
DefaultWindowDecoration& DefaultWindowDecoration::operator=(const DefaultWindowDecoration& rhs)     = default;
DefaultWindowDecoration& DefaultWindowDecoration::operator=(DefaultWindowDecoration&& rhs) noexcept = default;

DefaultWindowDecoration::DefaultWindowDecoration(Internal::DefaultWindowDecorationImpl* impl)
: Dali::BaseHandle(impl)
{
}

DefaultWindowDecoration DefaultWindowDecoration::New(WindowFrame windowFrame)
{
  return New(windowFrame, DefaultWindowDecorationOptions());
}

DefaultWindowDecoration DefaultWindowDecoration::New(WindowFrame windowFrame, const DefaultWindowDecorationOptions& options)
{
  if(!windowFrame)
  {
    DALI_LOG_ERROR("[DefaultWindowDecoration] cannot decorate an empty WindowFrame handle\n");
    return DefaultWindowDecoration();
  }

  DefaultWindowDecoration           defaultDecoration(new Internal::DefaultWindowDecorationImpl(windowFrame, options));
  const WindowFrameDecorationResult result = GetImplementation(defaultDecoration).Install();
  if(result != WindowFrameDecorationResult::INSTALLED)
  {
    DALI_LOG_ERROR("[DefaultWindowDecoration] install rejected:%s\n", Internal::WindowFrameDecorationResultName(result));
    return DefaultWindowDecoration();
  }
  return defaultDecoration;
}

DefaultWindowDecoration DefaultWindowDecoration::DownCast(Dali::BaseHandle handle)
{
  return DefaultWindowDecoration(dynamic_cast<Internal::DefaultWindowDecorationImpl*>(handle.GetObjectPtr()));
}

WindowFrameDecoration DefaultWindowDecoration::GetDecoration() const
{
  return GetImplementation(*this).GetDecoration();
}

Dali::Ui::View DefaultWindowDecoration::GetMoveRegion() const
{
  return GetImplementation(*this).GetMoveRegion();
}

Dali::Ui::View DefaultWindowDecoration::GetTopArea() const
{
  return GetImplementation(*this).GetTopArea();
}

Dali::Ui::View DefaultWindowDecoration::AddBarAction(Dali::StringView iconUrl, Dali::Callback<void()> onClick)
{
  return GetImplementation(*this).AddBarAction(iconUrl, std::move(onClick));
}

Dali::Vector2 DefaultWindowDecoration::GetMinimumFrameSize() const
{
  return GetImplementation(*this).GetMinimumFrameSize();
}

} // namespace Dali::Ui
