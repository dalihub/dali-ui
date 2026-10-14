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
#include <vector>
#include <dali-ui-foundation/integration-api/view-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/navigator.h>

namespace Dali
{

namespace UI
{

namespace Internal
{

/**
 * @brief This is the internal implementation class for Navigator.
 */
class NavigatorImpl : public Integration::ViewImpl
{
public:

  // Creation & Destruction

  /**
   * @brief Creates a new NavigatorImpl.
   */
  static UI::Navigator New();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~NavigatorImpl();

public:

  // API

  void Push(UI::Page page);

  UI::Page Pop();

  void Insert(UI::Page page, uint32_t index);

  void Remove(UI::Page page);

  UI::Page GetPage(uint32_t index) const;

  uint32_t GetPageCount() const;

protected:

  // Construction

  /**
   * @brief NavigatorImpl constructor.
   */
  NavigatorImpl();

private:

  // Not copyable or movable
  NavigatorImpl(const NavigatorImpl&) = delete;
  NavigatorImpl(NavigatorImpl&&) = delete;
  NavigatorImpl& operator=(const NavigatorImpl&) = delete;
  NavigatorImpl& operator=(NavigatorImpl&&) = delete;

private:

  // Data
  std::vector<UI::Page> mPages;
};

// Helpers for public-api forwarding methods

inline Internal::NavigatorImpl& GetImpl(UI::Navigator& navigator)
{
  DALI_ASSERT_ALWAYS(navigator);

  Dali::RefObject& handle = navigator.GetImplementation();

  return static_cast<Internal::NavigatorImpl&>(handle);
}

inline const Internal::NavigatorImpl& GetImpl(const UI::Navigator& navigator)
{
  DALI_ASSERT_ALWAYS(navigator);

  const Dali::RefObject& handle = navigator.GetImplementation();

  return static_cast<const Internal::NavigatorImpl&>(handle);
}

} // namespace Internal

} // namespace UI

} // namespace Dali
