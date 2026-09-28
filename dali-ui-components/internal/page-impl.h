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
#include <dali-ui-components/public-api/page.h>

namespace Dali
{

namespace UI
{

namespace Internal
{

/**
 * @brief This is the internal implementation class for Page.
 */
class PageImpl : public Integration::ViewImpl
{
public:

  // Creation & Destruction

  /**
   * @brief Creates a new PageImpl.
   */
  static UI::Page New();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PageImpl();

public:

  // API

protected:

  // Construction

  /**
   * @brief PageImpl constructor.
   */
  PageImpl();

private:

  // Not copyable or movable
  PageImpl(const PageImpl&) = delete;
  PageImpl(PageImpl&&) = delete;
  PageImpl& operator=(const PageImpl&) = delete;
  PageImpl& operator=(PageImpl&&) = delete;

private:

  // Data
};

// Helpers for public-api forwarding methods

inline Internal::PageImpl& GetImpl(UI::Page& page)
{
  DALI_ASSERT_ALWAYS(page);

  Dali::RefObject& handle = page.GetImplementation();

  return static_cast<Internal::PageImpl&>(handle);
}

inline const Internal::PageImpl& GetImpl(const UI::Page& page)
{
  DALI_ASSERT_ALWAYS(page);

  const Dali::RefObject& handle = page.GetImplementation();

  return static_cast<const Internal::PageImpl&>(handle);
}

} // namespace Internal

} // namespace UI

} // namespace Dali
