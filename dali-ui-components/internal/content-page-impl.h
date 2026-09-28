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
#include <dali-ui-components/internal/page-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/content-page.h>

namespace Dali
{

namespace UI
{

namespace Internal
{

/**
 * @brief This is the internal implementation class for ContentPage.
 */
class ContentPageImpl : public PageImpl
{
public:

  // Creation & Destruction

  static UI::ContentPage New();

protected:

  virtual ~ContentPageImpl();

public:

  // API

protected:

  // Construction
  ContentPageImpl();

private:

  // Not copyable or movable
  ContentPageImpl(const ContentPageImpl&) = delete;
  ContentPageImpl(ContentPageImpl&&) = delete;
  ContentPageImpl& operator=(const ContentPageImpl&) = delete;
  ContentPageImpl& operator=(ContentPageImpl&&) = delete;
};

// Helpers for public-api forwarding methods

inline Internal::ContentPageImpl& GetImpl(UI::ContentPage& view)
{
  DALI_ASSERT_ALWAYS(view);
  Dali::RefObject& handle = view.GetImplementation();
  return static_cast<Internal::ContentPageImpl&>(handle);
}

inline const Internal::ContentPageImpl& GetImpl(const UI::ContentPage& view)
{
  DALI_ASSERT_ALWAYS(view);
  const Dali::RefObject& handle = view.GetImplementation();
  return static_cast<const Internal::ContentPageImpl&>(handle);
}

} // namespace Internal

} // namespace UI

} // namespace Dali
