#ifndef DALI_UI_TEXT_VISUAL_OBJECT_H
#define DALI_UI_TEXT_VISUAL_OBJECT_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/visuals/text-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

namespace Dali
{
namespace Ui
{

/**
 * @addtogroup dali_ui_visuals
 * @{
 */

/**
 * @brief TextVisual is a owner of Visual::Base with Visual::TEXT, to render simple text.
 *
 * It cannot use CornerRadius / CornerSquareness / Borderline feature.
 */
class DALI_UI_API TextVisual : public VisualBase
{
public:
  /**
   * @brief Property indices for TextVisual.
   *
   * These can be used with Dali::Ui::VisualBase::GetProperty() and SetProperty().
   */
  struct Property
  {
    enum
    {
      TEXT                 = TextVisualPropertyIndex::TEXT,
      FONT_FAMILY          = TextVisualPropertyIndex::FONT_FAMILY,
      FONT_SIZE            = TextVisualPropertyIndex::FONT_SIZE,
      FONT_WEIGHT          = TextVisualPropertyIndex::FONT_WEIGHT,
      FONT_WIDTH           = TextVisualPropertyIndex::FONT_WIDTH,
      FONT_SLANT           = TextVisualPropertyIndex::FONT_SLANT,
      MULTI_LINE           = TextVisualPropertyIndex::MULTI_LINE,
      LINE_WRAP_MODE       = TextVisualPropertyIndex::LINE_WRAP_MODE,
      HORIZONTAL_ALIGNMENT = TextVisualPropertyIndex::HORIZONTAL_ALIGNMENT,
      VERTICAL_ALIGNMENT   = TextVisualPropertyIndex::VERTICAL_ALIGNMENT,
      OVERFLOW_MODE        = TextVisualPropertyIndex::OVERFLOW_MODE,
      LINE_HEIGHT          = TextVisualPropertyIndex::LINE_HEIGHT,
      LINE_HEIGHT_MODE     = TextVisualPropertyIndex::LINE_HEIGHT_MODE,
      TEXT_COLOR           = TextVisualPropertyIndex::TEXT_COLOR,
      MARKUP_ENABLED       = TextVisualPropertyIndex::MARKUP_ENABLED,
    };
  };

public:
  /**
   * @brief Creates a TextVisual object.
   *
   * @return The newly created color visual object.
   */
  static TextVisual New();

  /**
   * @brief Downcasts a handle to TextVisual handle.
   *
   * If handle points to a TextVisual, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a TextVisual or an uninitialized handle
   */
  static TextVisual DownCast(BaseHandle handle);

public: // Setters
  /**
   * @brief Gets the text.
   *
   * @return The text currently set on the TextVisual in UTF-8 format
   */
  Dali::String GetText() const;

  /**
   * @brief Sets the text to display.
   *
   * @param[in] text The text to display in UTF-8 format.
   */
  void SetText(const Dali::String& text);

  /**
   * @brief Gets the font family.
   *
   * @return The font family currently set on the TextVisual
   */
  Dali::String GetFontFamily() const;

  /**
   * @brief Sets the font family to use.
   *
   * @param[in] fontFamily The requested font family to use
   */
  void SetFontFamily(const Dali::String& fontFamily);

  /**
   * @brief Gets the font size in pixels.
   *
   * @return The font size in pixels
   */
  float GetFontSize() const;

  /**
   * @brief Sets the size of font in pixels.
   *
   * @param[in] fontSize The size of font in pixels
   */
  void SetFontSize(float fontSize);

  /**
   * @brief Gets the font weight.
   *
   * @return The font weight
   */
  Dali::Ui::Text::FontWeight GetFontWeight() const;

  /**
   * @brief Sets the font weight.
   *
   * @param[in] fontWeight The font weight to set
   */
  void SetFontWeight(Dali::Ui::Text::FontWeight fontWeight);

  /**
   * @brief Gets the font width.
   *
   * @return The font width
   */
  Dali::Ui::Text::FontWidth GetFontWidth() const;

  /**
   * @brief Sets the font width.
   *
   * @param[in] fontWidth The font width to set
   */
  void SetFontWidth(Dali::Ui::Text::FontWidth fontWidth);

  /**
   * @brief Gets the font slant.
   *
   * @return The font slant
   */
  Dali::Ui::Text::FontSlant GetFontSlant() const;

  /**
   * @brief Sets the font slant.
   *
   * @param[in] fontSlant The font slant to set
   */
  void SetFontSlant(Dali::Ui::Text::FontSlant fontSlant);

  /**
   * @brief Gets whether multi-line layout is enabled.
   *
   * @return True if multi-line layout is enabled
   */
  bool IsMultiLine() const;

  /**
   * @brief Sets the single-line or multi-line layout option.
   *
   * @param[in] multiLine True to enable multi-line layout, false for single-line
   */
  void SetMultiLine(bool multiLine);

  /**
   * @brief Gets the line wrap mode.
   *
   * @return The line wrap mode
   */
  Dali::Ui::Text::LineWrapMode GetLineWrapMode() const;

  /**
   * @brief Sets the line wrap mode.
   *
   * @param[in] lineWrapMode The line wrap mode to set
   */
  void SetLineWrapMode(Dali::Ui::Text::LineWrapMode lineWrapMode);

  /**
   * @brief Gets the horizontal alignment.
   *
   * @return The horizontal alignment
   */
  Dali::Ui::Text::Alignment GetHorizontalAlignment() const;

  /**
   * @brief Sets the horizontal alignment.
   *
   * @param[in] alignment The horizontal alignment to set
   */
  void SetHorizontalAlignment(Dali::Ui::Text::Alignment alignment);

  /**
   * @brief Gets the vertical alignment.
   *
   * @return The vertical alignment
   */
  Dali::Ui::Text::Alignment GetVerticalAlignment() const;

  /**
   * @brief Sets the vertical alignment.
   *
   * @param[in] alignment The vertical alignment to set
   */
  void SetVerticalAlignment(Dali::Ui::Text::Alignment alignment);

  /**
   * @brief Gets the overflow mode.
   *
   * @return The overflow mode
   */
  Dali::Ui::Text::OverflowMode GetOverflowMode() const;

  /**
   * @brief Sets the overflow mode.
   *
   * @param[in] overflowMode The overflow mode to set
   */
  void SetOverflowMode(Dali::Ui::Text::OverflowMode overflowMode);

  /**
   * @brief Gets the line height.
   *
   * @return The line height
   */
  float GetLineHeight() const;

  /**
   * @brief Sets the line height.
   *
   * @param[in] lineHeight The line height to set
   */
  void SetLineHeight(float lineHeight);

  /**
   * @brief Gets the line height mode.
   *
   * @return The line height mode
   */
  Dali::Ui::Text::LineHeightMode GetLineHeightMode() const;

  /**
   * @brief Sets the line height mode.
   *
   * @param[in] lineHeightMode The line height mode to set
   */
  void SetLineHeightMode(Dali::Ui::Text::LineHeightMode lineHeightMode);

  /**
   * @brief Gets the text color.
   *
   * @return The text color
   */
  UiColor GetTextColor() const;

  /**
   * @brief Sets the color of the text.
   *
   * @param[in] textColor The color of the text
   */
  void SetTextColor(const UiColor& textColor);

  /**
   * @brief Gets whether mark-up processing is enabled.
   *
   * @return True if mark-up processing is enabled
   */
  bool IsMarkupEnabled() const;

  /**
   * @brief Sets whether the mark-up processing is enabled.
   *
   * @param[in] enabled True to enable mark-up processing
   */
  void SetMarkupEnabled(bool enabled);

public:
  TextVisual()                                     = default;
  ~TextVisual()                                    = default;
  TextVisual(const TextVisual& rhs)                = default;
  TextVisual& operator=(const TextVisual& rhs)     = default;
  TextVisual(TextVisual&& rhs) noexcept            = default;
  TextVisual& operator=(TextVisual&& rhs) noexcept = default;

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL TextVisual(Dali::Ui::Internal::VisualBaseImpl* object);

public:
};

/**
 * @}
 */
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_TEXT_VISUAL_OBJECT_H
