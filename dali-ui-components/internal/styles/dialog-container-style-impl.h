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
 */

#include <dali-ui-components/public-api/styles/dialog-container-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
class DialogContainerStyleImpl : public Extension::UiStyleImpl
{
public:
  /**
   * @brief Creates an unreferenced implementation only for BaseHandle construction.
   * @warning Pass the result directly to a BaseHandle-derived handle constructor.
   * The constructor must acquire its first reference (the returned reference count is zero).
   * Use New() when an IntrusivePtr owner is required.
   */
  static DALI_INTERNAL DialogContainerStyleImpl* NewForBaseHandle()
  {
    return new DialogContainerStyleImpl();
  }

  /**
   * @brief Creates an unreferenced implementation only for BaseHandle construction.
   * @warning Pass the result directly to a BaseHandle-derived handle constructor.
   * The constructor must acquire its first reference (the returned reference count is zero).
   * Use New() when an IntrusivePtr owner is required.
   */
  static DALI_INTERNAL DialogContainerStyleImpl* NewForBaseHandle(const DialogContainerStyleImpl& rhs)
  {
    return new DialogContainerStyleImpl(rhs);
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static IntrusivePtr<DialogContainerStyleImpl> New()
  {
    return IntrusivePtr<DialogContainerStyleImpl>(new DialogContainerStyleImpl());
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static IntrusivePtr<DialogContainerStyleImpl> New(const DialogContainerStyleImpl& rhs)
  {
    return IntrusivePtr<DialogContainerStyleImpl>(new DialogContainerStyleImpl(rhs));
  }
  struct Values
  {
    // OneUI fallback scrim; NoScrimPreset explicitly disables tint and blur.
    UiColor mScrimColor{UiColor(0x000000u, 0.4f)};
    float   mScrimBlurRadius{100.0f};
  } values;

protected:
  DialogContainerStyleImpl() = default;
  DialogContainerStyleImpl(const DialogContainerStyleImpl& rhs)
  : Extension::UiStyleImpl(),
    values(rhs.values)
  {
  }

  ~DialogContainerStyleImpl() override = default;
};
} // namespace Internal
} // namespace Ui
} // namespace DALI_NAMESPACE
