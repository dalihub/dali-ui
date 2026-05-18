#ifndef DALI_UI_INTERNAL_VISUAL_TRANSFORM_H
#define DALI_UI_INTERNAL_VISUAL_TRANSFORM_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/align-enumerations.h>

namespace Dali::Ui::Internal::Visual
{
struct Transform
{
  /**
   * Default constructor ensures the visual fills the control
   */
  Transform();

  /**
   * Use the property map to set zero or more of the transform
   * attributes, and sets the remaining attributes to their default
   * values.
   */
  void SetPropertyMap(const Property::Map& map);

  /**
   * Add the transform attributes to the map (using integer keys)
   */
  void GetPropertyMap(Property::Map& map) const;

  /**
   * Update zero or more attributes from the property map.
   */
  void UpdatePropertyMap(const Property::Map& map);

  /**
   * Convert the control size and the transform attributes into the actual
   * size of the visual.
   */
  Vector2 GetVisualSize(const Vector2& controlSize, float viewEffectiveScale);

  /**
   * Get property index from given key, relative with visual transform.
   */
  static Property::Index GetIntKey(const Property::Key& key);

  /**
   * Get property maps for the default transform.
   */
  static const Property::Map& GetDefaultTransformMap();

  Vector2         mOffset;
  Vector2         mSize;
  Vector2         mExtraSize;
  Vector4         mOffsetSizeMode;
  Ui::Align::Type mOrigin;
  Ui::Align::Type mPivot;
};
} // namespace Dali::Ui::Internal::Visual

#endif // DALI_UI_INTERNAL_VISUAL_TRANSFORM_H
