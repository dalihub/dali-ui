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
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/view-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/dialog/dialog-container.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Implementation class for Ui::DialogContainer.
 */
class DALI_UI_API DialogContainerImpl : public ViewImpl
{
public:
  static Ui::DialogContainer New();

  void     SetModalContent(Ui::View modalContent);
  Ui::View GetModalContent() const;
  void     SetScrim(Ui::View scrim);
  Ui::View GetScrim() const;

  Ui::DialogContainer::ScrimClickedSignalType& ScrimClickedSignal()
  {
    return mScrimClickedSignal;
  }

protected:
  DialogContainerImpl();
  virtual ~DialogContainerImpl();

  void OnInitialize() override;

private:
  void CreateDefaultScrim();
  void OnScrimClicked(Ui::View view, Ui::InputEvent event);

  DialogContainerImpl(const DialogContainerImpl&)            = delete;
  DialogContainerImpl(DialogContainerImpl&&)                 = delete;
  DialogContainerImpl& operator=(const DialogContainerImpl&) = delete;
  DialogContainerImpl& operator=(DialogContainerImpl&&)      = delete;

private:
  Ui::View                                    mScrim;
  Ui::View                                    mModalContent;
  Ui::DialogContainer::ScrimClickedSignalType mScrimClickedSignal;
};

} // namespace Integration

inline Integration::DialogContainerImpl& GetImpl(Ui::DialogContainer& dialogContainer)
{
  DALI_ASSERT_ALWAYS(dialogContainer);
  Dali::RefObject& handle = dialogContainer.GetImplementation();
  return static_cast<Integration::DialogContainerImpl&>(handle);
}

inline const Integration::DialogContainerImpl& GetImpl(const Ui::DialogContainer& dialogContainer)
{
  DALI_ASSERT_ALWAYS(dialogContainer);
  const Dali::RefObject& handle = dialogContainer.GetImplementation();
  return static_cast<const Integration::DialogContainerImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
