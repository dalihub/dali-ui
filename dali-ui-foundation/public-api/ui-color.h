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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief A color value that can hold either direct RGBA components or a
 * string identifier that is resolved at runtime via the current theme.
 *
 * When constructed with RGBA values, UiColor behaves like a lightweight
 * wrapper around Vector4. When constructed with a string color ID, the
 * actual RGBA values are looked up from the current ThemeLoaderInterface
 * at the point of Resolve().
 *
 * @code
 *   // Direct RGBA
 *   UiColor red(1.0f, 0.0f, 0.0f, 1.0f);
 *
 *   // ID-based (resolved from current theme)
 *   UiColor primary("Primary");
 *
 *   // Predefined color IDs
 *   view.SetBackgroundColor(UiColor::PRIMARY);
 * @endcode
 */
class DALI_UI_API UiColor
{
public:
  static const UiColor PRIMARY;
  static const UiColor BACKGROUND;
  static const UiColor OUTLINE;

  /**
   * @brief Creates a default UiColor (transparent black, no ID).
   */
  UiColor();

  /**
   * @brief Creates a UiColor with explicit RGBA components.
   *
   * @param[in] r Red component (0.0 ~ 1.0)
   * @param[in] g Green component (0.0 ~ 1.0)
   * @param[in] b Blue component (0.0 ~ 1.0)
   * @param[in] a Alpha component (0.0 ~ 1.0), defaults to 1.0
   */
  UiColor(float r, float g, float b, float a = 1.0f);

  /**
   * @brief Creates a UiColor from a Vector4.
   *
   * @param[in] color The RGBA color as Vector4
   */
  UiColor(const Vector4& color);

  /**
   * @brief Creates a UiColor with a string identifier.
   *
   * The actual RGBA value is resolved from the current theme when
   * Resolve() is called.
   *
   * @param[in] colorId The color identifier string
   */
  explicit UiColor(const String& colorId);

  /**
   * @brief Creates a UiColor with a string identifier (rvalue overload).
   *
   * The actual RGBA value is resolved from the current theme when
   * Resolve() is called.
   *
   * This overload enables moving the string into the internal token registry
   * to avoid an extra copy when the caller provides an rvalue.
   *
   * @param[in] colorId The color identifier string
   */
  explicit UiColor(String&& colorId);

  /**
   * @brief Creates a UiColor from a hex RGB value.
   *
   * @param[in] rgb 0xRRGGBB format hex color
   * @param[in] a Alpha component (0.0 ~ 1.0), defaults to 1.0
   */
  explicit UiColor(uint32_t rgb, float a = 1.0f);

  ~UiColor() = default;

  UiColor(const UiColor&)                = default;
  UiColor(UiColor&&) noexcept            = default;
  UiColor& operator=(const UiColor&)     = default;
  UiColor& operator=(UiColor&&) noexcept = default;

  /**
   * @brief Returns whether this UiColor holds a string color ID.
   *
   * @return True if this UiColor was constructed with a color ID
   */
  bool HasColorId() const;

  /**
   * @brief Returns the color ID string.
   *
   * @return The color ID, or an empty string if this is a direct RGBA color
   */
  String GetColorId() const;

  /**
   * @brief Resolves and returns the RGBA value as a Vector4.
   *
   * If this UiColor has a color ID, the value is looked up from the
   * current theme. If the ID is not found, returns the fallback
   * mColor value (default transparent black).
   * If this UiColor has direct RGBA values, returns them as-is.
   *
   * @return The resolved RGBA color
   */
  Vector4 Resolve() const;

  /**
   * @brief Implicit conversion to Vector4.
   *
   * Equivalent to calling Resolve().
   */
  operator Vector4() const;

  /**
   * @brief Returns a UiColor with alpha scaled by the given factor.
   *
   * - RGBA: out.a = in.a * factor
   * - Token: the resolved alpha is scaled by factor
   */
  UiColor ScaleAlpha(float factor) const;

  /**
   * @brief Returns a UiColor with alpha replaced by the given value.
   *
   * - RGBA: out.a = alpha
   * - Token: the resolved alpha is replaced with alpha
   */
  UiColor WithAlpha(float alpha) const;

private:
  enum class Type : uint8_t
  {
    Rgba  = 0u,
    Token = 1u,
  };

  enum class AlphaMode : uint8_t
  {
    Multiply = 0u,
    With     = 1u,
  };

  Type      GetType() const;
  void      SetType(Type type);
  AlphaMode GetAlphaMode() const;
  void      SetAlphaMode(AlphaMode mode);

  Vector4 GetRgba() const;
  void    SetRgba(const Vector4& rgba);

  uint32_t GetTokenId() const;
  void     SetTokenId(uint32_t tokenId);
  float    GetTokenAlpha() const;
  void     SetTokenAlpha(float alpha);

private:
  alignas(8) uint8_t mData[24]{};
};

} // namespace Ui
} // namespace Dali
