#ifndef CSHARP_VISUAL_OBJECTS_CONTAINER_IMPL_H
#define CSHARP_VISUAL_OBJECTS_CONTAINER_IMPL_H

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
#include <dali/devel-api/common/free-list.h>
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visuals-container.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class VisualsContainer;
using VisualsContainerPtr = Dali::IntrusivePtr<VisualsContainer>;

class VisualsContainer : public Dali::BaseObject
{
public:
  /**
   * @brief Creates a VisualsContainer object.
   *
   * @param[in] view The view that owns this VisualsContainer.
   * @param[in] rangeType The range type of this VisualsContainer.
   * @return The newly created visual object container.
   */
  static VisualsContainerPtr New(Dali::Ui::View view, Dali::Ui::Integration::Visual::InternalContainerRangeType rangeType);

public: ///< Public API
  /**
   * @copydoc Dali::VisualsContainer::GetOwner()
   */
  Dali::Ui::View GetOwner() const;

  /**
   * @copydoc Dali::VisualsContainer::GetContainerRangeType()
   */
  Dali::Ui::Integration::Visual::InternalContainerRangeType GetContainerRangeType() const;

  /**
   * @copydoc Dali::VisualsContainer::GetVisualBasesCount()
   */
  uint32_t GetVisualBasesCount() const;

  /**
   * @copydoc Dali::VisualsContainer::GetVisualBaseAt()
   */
  Dali::Ui::VisualBase GetVisualBaseAt(uint32_t index) const;

  /**
   * @copydoc Dali::VisualsContainer::AddVisualBase()
   */
  bool AddVisualBase(Dali::Ui::VisualBase visualObject, Dali::Ui::Integration::VisualsContainer::ShadowType shadowType = Dali::Ui::Integration::VisualsContainer::ShadowType::NONE);

  /**
   * @copydoc Dali::VisualsContainer::RemoveVisualBase()
   */
  void RemoveVisualBase(Dali::Ui::VisualBase visualObject);

public: ///< Called from Internal::VisualBaseImpl
  /**
   * @brief Replace visual object's visual into the control.
   * If the visual object is already registered, it will be replaced.
   * If VisualBase doesn't have visual yet, it will unregister visual.
   *
   * @param[in] visualObjectImpl visual object implements.
   */
  void ReplaceVisualBase(Dali::Ui::Internal::VisualBaseImpl& visualObjectImpl);

  /**
   * @copydoc Dali::VisualsContainer::ChangeSiblingOrder()
   */
  void ChangeSiblingOrder(uint32_t fromIndex, uint32_t toIndex);

private:
  /**
   * @brief Unregister visual object's visual from the control.
   *
   * @param[in] visualObjectImpl visual object implements.
   */
  void UnregisterVisualBase(Dali::Ui::Internal::VisualBaseImpl& visualObjectImpl);

protected:
  /**
   * @brief Constructor - creates a VisualsContainer.
   *
   * @param[in] control The control that owns this VisualsContainer.
   * @param[in] rangeType The range type of this VisualsContainer.
   */
  VisualsContainer(Dali::Ui::View view, Dali::Ui::Integration::Visual::InternalContainerRangeType rangeType);

public:
  /**
   * @brief Destructor.
   */
  ~VisualsContainer() override;

private:
  std::vector<Dali::Ui::VisualBase> mVisualBases;
  Dali::WeakHandle<Dali::Ui::View>  mView;

  const Dali::Ui::Integration::Visual::InternalContainerRangeType mRangeType;

  Dali::FreeList mVisualIndexConverter; ///< Convert from virtual visual index of VisualBase to actual visual index of mView.
};
} // namespace Internal

// Helpers for api forwarding methods

inline static Internal::VisualsContainer& GetImplementation(Dali::Ui::Integration::VisualsContainer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "VisualsContainer handle is empty.");

  Dali::BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::VisualsContainer&>(object);
}

inline static const Internal::VisualsContainer& GetImplementation(const Dali::Ui::Integration::VisualsContainer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "VisualsContainer handle is empty.");

  const Dali::BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::VisualsContainer&>(object);
}

} // namespace Ui
} // namespace Dali

#endif // CSHARP_VISUAL_OBJECTS_CONTAINER_IMPL_H
