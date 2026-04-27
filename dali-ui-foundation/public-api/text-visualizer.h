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
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/text/text-visualizer-properties.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{

namespace Ui
{

// Forward declarations
namespace Integration
{
class TextVisualizerImpl;
}

/**
 * @brief TextVisualizer is a read-only View intended for dynamic text layout.
 *
 * This commit only introduces the public handle / internal implementation skeleton.
 */
class DALI_UI_API TextVisualizer : public View
{
public:
  /**
   * @brief Property indices for TextVisualizer.
   *
   * @note See Dali::Ui::Text::TextVisualizerPropertyIndex for the underlying property definitions.
   */
  struct Property
  {
    enum
    {
      TEXT        = Text::TextVisualizerPropertyIndex::TEXT,
      FONT_FAMILY = Text::TextVisualizerPropertyIndex::FONT_FAMILY,
      FONT_SIZE   = Text::TextVisualizerPropertyIndex::FONT_SIZE,
      TEXT_COLOR  = Text::TextVisualizerPropertyIndex::TEXT_COLOR,
      LINE_HEIGHT = Text::TextVisualizerPropertyIndex::LINE_HEIGHT
    };
  };

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized TextVisualizer handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  TextVisualizer();

  /**
   * @brief Creates an initialized TextVisualizer.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static TextVisualizer New();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] textVisualizer Handle to copy
   */
  TextVisualizer(const TextVisualizer& textVisualizer);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  TextVisualizer(TextVisualizer&& rhs) noexcept;

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextVisualizer();

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  TextVisualizer& operator=(const TextVisualizer& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  TextVisualizer& operator=(TextVisualizer&& rhs) noexcept;

public: // Static Methods
  /**
   * @brief Downcasts a handle to TextVisualizer handle.
   *
   * If handle points to a TextVisualizer, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a TextVisualizer or an uninitialized handle
   */
  static TextVisualizer DownCast(BaseHandle handle);

public: // Setters for chaining
  /**
   * @brief Sets the text.
   *
   * @param[in] text The text to display in UTF-8 format.
   */
  TextVisualizer& SetText(const Dali::String& text);

  /**
   * @brief Gets the text.
   *
   * @return The text currently set on the text visualizer in UTF-8 format.
   */
  Dali::String GetText() const;

  /**
   * @brief Sets the font family of the text.
   *
   * @param[in] fontFamily The requested font family to use.
   */
  TextVisualizer& SetFontFamily(const Dali::String& fontFamily);

  /**
   * @brief Gets the font family of the text.
   *
   * @return The font family currently set on the text visualizer.
   */
  Dali::String GetFontFamily() const;

  /**
   * @brief Sets the font size of the text.
   *
   * @param[in] fontSize The font pixel size.
   */
  TextVisualizer& SetFontSize(float fontSize);

  /**
   * @brief Gets the font size of the text.
   *
   * @return The font pixel size currently set on the text visualizer.
   */
  float GetFontSize() const;

  /**
   * @brief Sets the relative line height multiplier.
   *
   * If the line height is positive, it is calculated as a multiplier of the font pixel size:
   *
   * `CalculatedLineHeight(px) = FontSize(px) * lineHeight`
   *
   * If the line height is -1.0f, natural line height is used.
   *
   * Only relative line height is supported by TextVisualizer in this revision.
   *
   * @param[in] lineHeight The relative line height multiplier, or -1.0f for automatic line height.
   */
  TextVisualizer& SetLineHeight(float lineHeight);

  /**
   * @brief Gets the current line height multiplier.
   *
   * @return The relative line height multiplier, or -1.0f when natural line height is used.
   */
  float GetLineHeight() const;

  /**
   * @brief Clears the line height override and restores natural line height.
   */
  void ClearLineHeight();

  /**
   * @brief Sets the color of the text.
   *
   * @param[in] color The required text color value.
   */
  TextVisualizer& SetTextColor(const UiColor& color);

  /**
   * @brief Gets the color of the text.
   *
   * @return The text color currently set on the text visualizer.
   */
  UiColor GetTextColor();

  /**
   * @brief Prepares the current text state for layout.
   *
   * This is an eager command. In this commit it only updates internal dirty state.
   */
  void Prepare();

  /**
   * @brief Sets the exclusion regions used by the future layout pass.
   *
   * @param[in] regions Regions where text should not be placed.
   */
  void SetExclusionRegions(const Dali::Vector<Rect<float>>& regions);

  /**
   * @brief Gets the currently stored exclusion regions.
   *
   * @return The current exclusion regions.
   */
  Dali::Vector<Rect<float>> GetExclusionRegions() const;

  /**
   * @brief Clears all exclusion regions.
   */
  void ClearExclusionRegions();

public:
  explicit DALI_UI_API TextVisualizer(Integration::TextVisualizerImpl& implementation);
  explicit DALI_UI_API TextVisualizer(Dali::Internal::CustomActor* internal);
};

} // namespace Ui

} // namespace Dali
