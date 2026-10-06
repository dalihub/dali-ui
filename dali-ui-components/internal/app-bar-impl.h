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

// EXTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/app-bar.h>

namespace Dali
{

namespace UI
{

namespace Internal
{

/**
 * @brief This is the internal implementation class for AppBar.
 */
class AppBarImpl : public Integration::ViewImpl
{
public:

  // Creation & Destruction

  static UI::AppBar New();

protected:

  virtual ~AppBarImpl();

public:

  // API

protected:

  // Construction
  AppBarImpl();

private:

  // Not copyable or movable
  AppBarImpl(const AppBarImpl&) = delete;
  AppBarImpl(AppBarImpl&&) = delete;
  AppBarImpl& operator=(const AppBarImpl&) = delete;
  AppBarImpl& operator=(AppBarImpl&&) = delete;
};

// Helpers for public-api forwarding methods

inline Internal::AppBarImpl& GetImpl(UI::AppBar& view)
{
  DALI_ASSERT_ALWAYS(view);
  Dali::RefObject& handle = view.GetImplementation();
  return static_cast<Internal::AppBarImpl&>(handle);
}

inline const Internal::AppBarImpl& GetImpl(const UI::AppBar& view)
{
  DALI_ASSERT_ALWAYS(view);
  const Dali::RefObject& handle = view.GetImplementation();
  return static_cast<const Internal::AppBarImpl&>(handle);
}

} // namespace Internal

} // namespace UI

} // namespace Dali
