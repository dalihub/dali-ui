#ifndef DALI_UI_GRADIENT_VISUAL_OBJECT_H
#define DALI_UI_GRADIENT_VISUAL_OBJECT_H

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
#include <dali/public-api/math/radian.h>
#include <dali/public-api/object/property-array.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/gradient/gradient-enumerations.h>
#include <dali-ui-foundation/public-api/gradient/gradient-stop-node.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

namespace Dali
{
namespace Ui
{

/**
 * @addtogroup dali_ui_visuals
 * @{
 */

#include "gradient-visual.autogen.h"
/**
 * @brief GradientVisual is a owner of Visual::Base with Visual::GRADIENT,
 * to render a smooth transition of colors to the visual's quad.
 *
 * It can use CornerRadius / CornerSquareness / Borderline feature.
 */
class DALI_UI_API GradientVisual : public VisualBase
{
public:
  /**
   * @brief Property indices for GradientVisual.
   *
   * These can be used with Dali::Ui::VisualBase::GetProperty() and SetProperty().
   */
  struct Property
  {
    enum
    {
      START_OFFSET   = GradientVisualPropertyIndex::START_OFFSET,
      START_POSITION = GradientVisualPropertyIndex::START_POSITION,
      END_POSITION   = GradientVisualPropertyIndex::END_POSITION,
      CENTER         = GradientVisualPropertyIndex::CENTER,
      RADIUS         = GradientVisualPropertyIndex::RADIUS,
      START_ANGLE    = GradientVisualPropertyIndex::START_ANGLE,
      STOP_OFFSET    = GradientVisualPropertyIndex::STOP_OFFSET,
      STOP_COLOR     = GradientVisualPropertyIndex::STOP_COLOR,
      UNITS          = GradientVisualPropertyIndex::UNITS,
      SPREAD_METHOD  = GradientVisualPropertyIndex::SPREAD_METHOD,
    };
  };

public:
  /**
   * @brief Creates a GradientVisual object.
   *
   * @return The newly created gradient visual object.
   */
  static GradientVisual New();

  /**
   * @brief Downcasts a handle to GradientVisual handle.
   *
   * If handle points to a GradientVisual, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a GradientVisual or an uninitialized handle
   */
  static GradientVisual DownCast(BaseHandle handle);

public: // Setters for chaining
  // @CHAIN_START(GradientVisual, VisualBase)
  /**
   * @brief Gets the start offset of the GradientVisual.
   *
   * @return The start offset of the GradientVisual
   */
  float GetStartOffset() const;

  /**
   * @brief Sets the start offset of the GradientVisual.
   *
   * @param[in] startOffset The start offset to set
   * @return Reference to this for fluent chaining
   */
  GradientVisual& SetStartOffset(float startOffset);

  /**
   * @brief Gets the start position of the GradientVisual.
   *
   * @return The start position of the GradientVisual
   */
  Dali::Vector2 GetStartPosition() const;

  /**
   * @brief Sets the start position of the GradientVisual.
   *
   * @param[in] startPosition The start position to set
   * @return Reference to this for fluent chaining
   */
  GradientVisual& SetStartPosition(const Dali::Vector2& startPosition);

  /**
   * @brief Gets the end position of the GradientVisual.
   *
   * @return The end position of the GradientVisual
   */
  Dali::Vector2 GetEndPosition() const;

  /**
   * @brief Sets the end position of the GradientVisual.
   *
   * @param[in] endPosition The end position to set
   * @return Reference to this for fluent chaining
   */
  GradientVisual& SetEndPosition(const Dali::Vector2& endPosition);

  /**
   * @brief Gets the center of the GradientVisual.
   *
   * @return The center of the GradientVisual
   */
  Dali::Vector2 GetCenter() const;

  /**
   * @brief Sets the center of the GradientVisual.
   *
   * @param[in] center The center to set
   * @return Reference to this for fluent chaining
   */
  GradientVisual& SetCenter(const Dali::Vector2& center);

  /**
   * @brief Gets the radius of the GradientVisual.
   *
   * @return The radius of the GradientVisual
   */
  float GetRadius() const;

  /**
   * @brief Sets the radius of the GradientVisual.
   *
   * @param[in] radius The radius to set
   * @return Reference to this for fluent chaining
   */
  GradientVisual& SetRadius(float radius);

  /**
   * @brief Gets the start angle of the GradientVisual.
   *
   * @return The start angle of the GradientVisual
   */
  Dali::Radian GetStartAngle() const;

  /**
   * @brief Sets the start angle of the GradientVisual.
   *
   * @param[in] startAngle The start angle to set
   * @return Reference to this for fluent chaining
   */
  GradientVisual& SetStartAngle(Dali::Radian startAngle);

  /**
   * @brief Gets the stop nodes of the GradientVisual.
   *
   * @return The stop nodes of the GradientVisual
   */
  Dali::Vector<Ui::Gradient::StopNode> GetStopNodes() const;

  /**
   * @brief Sets the stop nodes of the GradientVisual.
   * @note Stop nodes must have at least 2 items.
   *
   * @param[in] stopOffset The stop nodes to set.
   * @return Reference to this for fluent chaining
   */
  GradientVisual& SetStopNodes(const Dali::Vector<Ui::Gradient::StopNode>& stopNodes);

  // @CHAIN_MANUAL
  /**
   * @brief Sets the stop nodes of the GradientVisual in a declarative way.
   * This method allows for set stop nodes construction by passing
   * a brace-enclosed initializer list of Gradient::StopNode objects.
   * @note Stop nodes must have at least 2 items.
   *
   * @param[in] stopOffset The stop nodes to set
   * @return Reference to this for fluent chaining
   */
  GradientVisual& SetStopNodes(std::initializer_list<Ui::Gradient::StopNode> stopNodes)
  {
    Dali::Vector<Ui::Gradient::StopNode> convertedStopNodes;
    convertedStopNodes.Reserve(stopNodes.size());

    for(auto&& stopNode : stopNodes)
    {
      convertedStopNodes.PushBack(std::move(stopNode));
    }

    return SetStopNodes(convertedStopNodes);
  }

  /**
   * @brief Gets the gradient units of the GradientVisual.
   *
   * @return The gradient units of the GradientVisual
   */
  Ui::Gradient::Units GetUnits() const;

  /**
   * @brief Sets the gradient units of the GradientVisual.
   *
   * @param[in] gradientUnits The gradient units to set
   * @return Reference to this for fluent chaining
   */
  GradientVisual& SetUnits(Ui::Gradient::Units gradientUnits);

  /**
   * @brief Gets the spread method of the GradientVisual.
   *
   * @return The spread method of the GradientVisual
   */
  Ui::Gradient::SpreadMethod GetSpreadMethod() const;

  /**
   * @brief Sets the spread method of the GradientVisual.
   *
   * @param[in] spreadMethod The spread method to set
   * @return Reference to this for fluent chaining
   */
  GradientVisual& SetSpreadMethod(Ui::Gradient::SpreadMethod spreadMethod);
  // @CHAIN_END

public:
  GradientVisual()                                         = default;
  ~GradientVisual()                                        = default;
  GradientVisual(const GradientVisual& rhs)                = default;
  GradientVisual& operator=(const GradientVisual& rhs)     = default;
  GradientVisual(GradientVisual&& rhs) noexcept            = default;
  GradientVisual& operator=(GradientVisual&& rhs) noexcept = default;

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL GradientVisual(Dali::Ui::Internal::VisualBaseImpl* object);

public:
  DALI_UI_CHAIN_VISUALBASE_METHODS(GradientVisual)
};

/**
 * @}
 */
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_VISUAL_OBJECT_H
