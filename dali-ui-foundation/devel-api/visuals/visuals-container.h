#ifndef DALI_UI_VISUAL_OBJECTS_CONTAINER_H
#define DALI_UI_VISUAL_OBJECTS_CONTAINER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class VisualsContainer;
}

class VisualBase;

/**
 * @brief VisualsContainer is a container for visual objects for dali-ui specific policy.
 *
 * For each VisualBaseContainer, there is a corresponding view.
 * Each view can has only one VisualsContainer per each ContainerRangeType.
 *
 * It is used to manage visual objects properties to owned view;
 * e.g. SiblingOrder of visual objects to DepthIndex of visual.
 *
 * To avoid the collision between internal visual logic and dali-ui specific policy,
 * there is some limitation of visual object counts per each ContainerRangeType.
 *
 * For example, if ContainerRangeType is ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT,
 * it will use visual object's depth index only between
 * Dali::Ui::DepthIndex::Ranges::BACKGROUND and Dali::Ui::DepthIndex::Ranges::CONTENT.
 * If user try to add over the Dali::Ui::DepthIndex::Ranges::CONTENT, it will be ignored.
 */
class VisualsContainer : public Dali::BaseHandle
{
public:
  /**
   * @brief Enumeration for the type of shadow visual.
   *
   * This enum is used to specify the type of shadow when adding a shadow visual object
   * to the container. Different shadow types may require different rendering logic
   * and property calculations.
   */
  enum class ShadowType
  {
    BOX_SHADOW,   ///< Represents a standard box shadow (drop shadow) outside the visual.
    INNER_SHADOW, ///< Represents an inner shadow, appearing inside the boundaries of the visual.
    NONE,         ///< Represents the visual is not a shadow.
  };

  /**
   * @brief Creates a VisualsContainer object.
   *
   * @param[in] view The view that owns this VisualsContainer.
   * @param[in] rangeType The range type of this VisualsContainer.
   * @return The newly created visual objects container
   * @post The view should not create another VisualsContainer for each rangeType.
   */
  static VisualsContainer New(Dali::Ui::View view, Dali::Ui::DevelVisual::InternalContainerRangeType rangeType);

public: ///< Public API
  /**
   * @brief Get the owner of this container.
   *
   * @return The owner of this container, or empty handle if owner is invalid.
   */
  Dali::Ui::View GetOwner() const;

  /**
   * @brief Gets the range type of VisualsContainer.
   *
   * @return The range type of VisualsContainer.
   */
  Dali::Ui::DevelVisual::InternalContainerRangeType GetContainerRangeType() const;

  /**
   * @brief Gets the number of visual objects in the container.
   *
   * @return The number of visual objects in the container.
   */
  uint32_t GetVisualBasesCount() const;

  /**
   * @brief Get the visual objects from the container.
   * @note If index is out of range, will throw exception.
   *
   * @param[in] index The index of the visual object to get.
   * @return The visual object at the given index.
   */
  Dali::Ui::VisualBase GetVisualBaseAt(uint32_t index) const;

  /**
   * @brief Adds a visual object to the container.
   * Added visual object will be placed top of other visuals.
   * If the container cannot add more than maximum count of objects
   * or the visual object is already added, It will be ignored.
   *
   * @param[in] visualObject The visual object to be added.
   * @return True if the visual object is added successfully.
   */
  bool AddVisualBase(Dali::Ui::VisualBase visualObject);

  /**
   * @brief Adds a shadow visual object to the container.
   *
   * This method adds a visual object specifically intended to be rendered as a shadow.
   * It configures the visual object based on the specified shadow type (e.g., BOX_SHADOW or INNER_SHADOW),
   * applying appropriate internal flags and constraints for correct rendering.
   * For instance, inner shadows require specialized corner radius calculations to account for borderline width.
   * The added visual object will be placed on top of other visuals within the container.
   *
   * If the container cannot add more than the maximum count of objects, or if the visual object
   * is already added to this or another container, the operation will be ignored.
   *
   * @param[in] visualObject The visual object to be added as a shadow.
   * @param[in] shadowType The type of shadow (e.g., BOX_SHADOW, INNER_SHADOW).
   * @return True if the shadow visual object is added successfully, false otherwise.
   */
  bool AddShadowVisualBase(Dali::Ui::VisualBase visualObject, ShadowType shadowType);

  /**
   * @brief Removes a visual object from the container.
   * All other visual objects will be shifted down.
   *
   * @param[in] visualObject The visual object to be added.
   */
  void RemoveVisualBase(Dali::Ui::VisualBase visualObject);

public:
  VisualsContainer()                                           = default;
  ~VisualsContainer()                                          = default;
  VisualsContainer(const VisualsContainer& handle)             = default;
  VisualsContainer& operator=(const VisualsContainer& rhs)     = default;
  VisualsContainer(VisualsContainer&& rhs) noexcept            = default;
  VisualsContainer& operator=(VisualsContainer&& rhs) noexcept = default;

  /**
   * @brief Downcasts a handle to VisualsContainer handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a VisualsContainer or an uninitialized handle
   */
  static VisualsContainer DownCast(BaseHandle handle);

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL VisualsContainer(Dali::Ui::Internal::VisualsContainer* object);
};
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_VISUAL_OBJECTS_CONTAINER_H
