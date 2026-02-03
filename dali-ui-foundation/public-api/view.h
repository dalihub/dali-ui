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
#include <functional>
#include <dali-toolkit/public-api/controls/control.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
// None

namespace Dali
{

namespace UI
{

// Forward declarations
namespace Integration
{
class ViewImpl;
}

#ifndef DEFINE_BASE_CHAINING_METHOD
#define DEFINE_BASE_CHAINING_METHOD(ClassType, PropertyName, TArg, VArg, Body) \
  /**                                                                          \
   * @brief Sets the PropertyName.                                             \
   * @param[in] VArg The required value.                                       \
   * @return A reference to the instance itself for method chaining.           \
   */                                                                          \
  ClassType& PropertyName(TArg VArg)                                           \
  {                                                                            \
    Body return static_cast<ClassType&>(*this);                                \
  }

#define DEFINE_CHAINING_METHOD(ClassType, PropertyName, TArg, VArg) \
  DEFINE_BASE_CHAINING_METHOD(ClassType, PropertyName, TArg, VArg, Set##PropertyName(VArg);)
#endif

#ifndef DEFINE_CHAINING_METHOD_VIEW
#define DEFINE_CHAINING_METHOD_VIEW(ClassType)                                           \
  DEFINE_CHAINING_METHOD(ClassType, SizeWidth, float, width)                             \
  DEFINE_CHAINING_METHOD(ClassType, SizeHeight, float, height)                           \
  DEFINE_CHAINING_METHOD(ClassType, PositionX, float, x)                                 \
  DEFINE_CHAINING_METHOD(ClassType, PositionY, float, y)                                 \
  DEFINE_CHAINING_METHOD(ClassType, BackgroundColor, const Vector4&, color)              \
  DEFINE_CHAINING_METHOD(ClassType, ParentOrigin, const Vector3&, point)                 \
  DEFINE_CHAINING_METHOD(ClassType, PivotPoint, const Vector3&, point)                   \
  /**                                                                                    \
   * @brief Adds a list of children to this View in a declarative way.                   \
   * This method allows for a hierarchical UI tree construction by passing               \
   * a brace-enclosed initializer list of View objects.                                  \
   * @param[in] children The initializer list containing child View handles to be added. \
   * @return A reference to the instance itself for method chaining.                     \
   */                                                                                    \
  ClassType& Contents(std::initializer_list<Actor> children)                             \
  {                                                                                      \
    for (const auto& child : children)                                                   \
    {                                                                                    \
      Add(child);                                                                        \
    }                                                                                    \
    return static_cast<ClassType&>(*this);                                               \
  }                                                                                      \
                                                                                         \
  /**                                                                                    \
   * @brief Assigns this View instance to a target variable.                             \
   * This method is useful for capturing a reference to a View created within            \
   * a declarative UI tree for later use.                                                \
   * @param[out] self The variable where this instance will be assigned.                 \
   * @return A reference to the instance itself for method chaining.                     \
   */                                                                                    \
  ClassType& As(ClassType& self)                                                         \
  {                                                                                      \
    self = static_cast<ClassType&>(*this);                                               \
    return static_cast<ClassType&>(*this);                                               \
  }                                                                                      \
                                                                                         \
  /**                                                                                    \
   * @brief Executesa custom action on this View instance.                               \
   * Use this method to perform additional initialization or logic on a View             \
   * without breaking the declarative method chaining.                                   \
   * @param[in] action A function or lambda to be executed with this instance.           \
   * @return A reference to the instance itself for method chaining.                     \
   */                                                                                    \
  ClassType& With(std::function<void(ClassType&)> action)                                \
  {                                                                                      \
    if (action)                                                                          \
    {                                                                                    \
      action(*this);                                                                     \
    }                                                                                    \
    return static_cast<ClassType&>(*this);                                               \
  }
#endif

/**
 * @brief View is a base UI component class that extends Control.
 *
 * View provides basic UI functionality and can be used as a foundation
 * for creating custom UI components. It inherits all the capabilities
 * of Control including styling, gesture detection, and keyboard navigation.
 *
 */
class DALI_UI_API View : public Toolkit::Control
{
public:

  // Typedefs

public: // Creation & Destruction

  /**
   * @brief Creates an uninitialized View handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  View();

  /**
   * @brief Creates an initialized View.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static View New();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] view Handle to copy
   */
  View(const View& view);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  View(View&& rhs) noexcept;

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~View();

public: // Operators

  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  View& operator=(const View& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  View& operator=(View&& rhs) noexcept;

public: // Static Methods

  /**
   * @brief Downcasts a handle to View handle.
   *
   * If handle points to a View, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a View or an uninitialized handle
   */
  static View DownCast(BaseHandle handle);

public: // API

  /**
   * @brief Gets the width of the View.
   *
   * @return The width of the View
   */
  float GetSizeWidth() const;

  /**
   * @brief Gets the height of the View.
   *
   * @return The height of the View
   */
  float GetSizeHeight() const;

  /**
   * @brief Gets the X position of the View.
   *
   * @return The X position of the View
   */
  float GetPositionX() const;

  /**
   * @brief Gets the Y position of the View.
   *
   * @return The Y position of the View
   */
  float GetPositionY() const;

  /**
   * @brief Gets the parent origin of the View.
   *
   * @return The parent origin of the View
   */
  Vector3 GetParentOrigin() const;

  /**
   * @brief Gets the pivot point of the View.
   *
   * @return The pivot point of the View
   */
  Vector3 GetPivotPoint() const;

  DEFINE_CHAINING_METHOD_VIEW(View)

public: // Signals

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  explicit View(Integration::ViewImpl& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit View(Dali::Internal::CustomActor* internal);
  /// @endcond

protected:

  /**
   * @brief Sets the width of the View.
   *
   * @param[in] width The width to set
   */
  void SetSizeWidth(float width);

  /**
   * @brief Sets the height of the View.
   *
   * @param[in] height The height to set
   */
  void SetSizeHeight(float height);

  /**
   * @brief Sets the X position of the View.
   *
   * @param[in] x The X position to set
   */
  void SetPositionX(float x);

  /**
   * @brief Sets the Y position of the View.
   *
   * @param[in] y The Y position to set
   */
  void SetPositionY(float y);

  /**
   * @brief Sets the parent origin of the View.
   *
   * @param[in] point The parent origin to set
   */
  void SetParentOrigin(const Vector3& point);

  /**
   * @brief Sets the pivot point of the View.
   *
   * @param[in] point The pivot point to set
   */
  void SetPivotPoint(const Vector3& point);
};

} // namespace UI

} // namespace Dali
