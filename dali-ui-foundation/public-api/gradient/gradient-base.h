#ifndef DALI_UI_GRADIENT_BASE_H
#define DALI_UI_GRADIENT_BASE_H

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
#include <dali/public-api/common/dali-vector.h>
#include <initializer_list>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/gradient/gradient-enumerations.h>
#include <dali-ui-foundation/public-api/gradient/gradient-stop-node.h>

namespace Dali
{
namespace Ui
{
namespace Gradient
{

class Linear;
class Radial;
class Conic;

/**
 * @brief Common authored gradient value.
 *
 * Base stores only public authored data. It does not own or reference an internal renderer gradient object.
 */
class DALI_UI_API Base
{
public:
  /**
   * @brief Creates a gradient value with Type::NONE.
   */
  Base();

  /**
   * @brief Copies a gradient value.
   *
   * @param[in] rhs The gradient value to copy
   */
  Base(const Base& rhs);

  /**
   * @brief Moves a gradient value.
   *
   * @param[in] rhs The gradient value to move from
   */
  Base(Base&& rhs) noexcept;

  /**
   * @brief Destroys the gradient value.
   */
  ~Base();

  /**
   * @brief Copies a gradient value.
   *
   * @param[in] rhs The gradient value to copy
   * @return This gradient value
   */
  Base& operator=(const Base& rhs);

  /**
   * @brief Moves a gradient value.
   *
   * @param[in] rhs The gradient value to move from
   * @return This gradient value
   */
  Base& operator=(Base&& rhs) noexcept;

  /**
   * @brief Returns a gradient value representing no gradient.
   *
   * This value can be passed to APIs such as Label::SetTextGradient() to remove
   * an authored gradient and restore normal single-color text rendering.
   *
   * @return A shared immutable none gradient value.
   */
  static const Base& None();

  /**
   * @brief Returns the gradient type.
   *
   * @return The gradient type, or Type::NONE if this value has no gradient type
   */
  Type GetType() const;

  /**
   * @brief Sets the stop nodes.
   *
   * @param[in] stopNodes The authored stop nodes
   */
  void SetStopNodes(const Dali::Vector<StopNode>& stopNodes);

  /**
   * @brief Gets the stop nodes.
   *
   * @return The authored stop nodes
   */
  Dali::Vector<StopNode> GetStopNodes() const;

  /**
   * @brief Sets the stop nodes in a declarative way.
   *
   * @param[in] stopNodes The authored stop nodes
   */
  void SetStopNodes(std::initializer_list<StopNode> stopNodes)
  {
    Dali::Vector<StopNode> nodes;
    nodes.Reserve(stopNodes.size());

    for(const auto& stopNode : stopNodes)
    {
      nodes.PushBack(stopNode);
    }

    SetStopNodes(nodes);
  }

  /**
   * @brief Sets the gradient coordinate units.
   *
   * @param[in] units The coordinate units
   */
  void SetUnits(Units units);

  /**
   * @brief Gets the gradient coordinate units.
   *
   * @return The coordinate units
   */
  Units GetUnits() const;

  /**
   * @brief Sets the spread method.
   *
   * @param[in] spreadMethod The spread method
   */
  void SetSpreadMethod(SpreadMethod spreadMethod);

  /**
   * @brief Gets the spread method.
   *
   * @return The spread method
   */
  SpreadMethod GetSpreadMethod() const;

  /**
   * @brief Sets the start offset.
   *
   * @param[in] offset The start offset
   */
  void SetStartOffset(float offset);

  /**
   * @brief Gets the start offset.
   *
   * @return The start offset
   */
  float GetStartOffset() const;

private:
  class Impl;
  Impl* mImpl;

  friend class Linear;
  friend class Radial;
  friend class Conic;
};

} // namespace Gradient
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_GRADIENT_BASE_H
