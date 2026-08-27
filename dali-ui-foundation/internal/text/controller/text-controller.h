#ifndef DALI_UI_TEXT_CONTROLLER_H
#define DALI_UI_TEXT_CONTROLLER_H

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
#include <dali/integration-api/adaptor-framework/input-method-context-integ.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/adaptor-framework/clipboard.h>
#include <dali/public-api/events/gesture.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text/text-anchor-control-interface.h>
#include <dali-ui-foundation/integration-api/text/text-control-interface.h>
#include <dali-ui-foundation/integration-api/text/text-editable-control-interface.h>
#include <dali-ui-foundation/integration-api/text/text-selectable-control-interface.h>
#include <dali-ui-foundation/integration-api/text/text-update-type.h>
#include <dali-ui-foundation/internal/controls/text-controls/text-anchor.h>
#include <dali-ui-foundation/internal/controls/text-controls/text-selection-popup-callback-interface.h>
#include <dali-ui-foundation/internal/text/decorator/text-decorator.h>
#include <dali-ui-foundation/internal/text/hidden-text.h>
#include <dali-ui-foundation/internal/text/layouts/layout-engine.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-render-state.h>
#include <dali-ui-foundation/internal/text/text-enumerations.h>
#include <dali-ui-foundation/internal/text/text-model-interface.h>
#include <dali-ui-foundation/public-api/text/fit/text-fit.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>
#include <dali-ui-foundation/public-api/text/input-filter.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali::Ui::Text
{
class Controller;
struct MarqueeStartAnchor;
class StyledText;
class View;
class RenderingController;

/**
 * @brief Text selection operations .
 */
enum SelectionType
{
  INTERACTIVE = 0x0000, ///< Select the word where the cursor is located.
  ALL         = 0x0001, ///< Select the whole text.
  NONE        = 0x0002, ///< Unselect the whole text.
  RANGE       = 0x0003  ///< Select the range text.
};

typedef IntrusivePtr<Controller> ControllerPtr;

/**
 * @brief A Text Controller is used by UI Controls which display text.
 *
 * It manipulates the Logical & Visual text models on behalf of the UI Controls.
 * It provides a view of the text that can be used by rendering back-ends.
 *
 * For selectable/editable UI controls, the controller handles input events from the UI control
 * and decorations (grab handles etc) via the Decorator::ControllerInterface interface.
 *
 * The text selection popup button callbacks are as well handled via the TextSelectionPopupCallbackInterface interface.
 */
class Controller : public RefObject,
                   public Decorator::ControllerInterface,
                   public TextSelectionPopupCallbackInterface,
                   public HiddenText::Observer,
                   public ConnectionTracker,
                   public Dali::Integration::Processor
{
public: // Enumerated types.
  /**
   * @brief Text related operations to be done in the relayout process.
   */
  enum OperationsMask
  {
    NO_OPERATION       = 0x0000,
    CONVERT_TO_UTF32   = 0x0001,
    GET_SCRIPTS        = 0x0002,
    VALIDATE_FONTS     = 0x0004,
    GET_LINE_BREAKS    = 0x0008,
    BIDI_INFO          = 0x0010,
    SHAPE_TEXT         = 0x0020,
    GET_GLYPH_METRICS  = 0x0040,
    LAYOUT             = 0x0080,
    UPDATE_LAYOUT_SIZE = 0x0100,
    REORDER            = 0x0200,
    ALIGN              = 0x0400,
    COLOR              = 0x0800,
    UPDATE_DIRECTION   = 0x1000,
    ALL_OPERATIONS     = 0xFFFF
  };

  /**
   * @brief Used to distinguish between regular key events and InputMethodContext events
   */
  enum InsertType
  {
    COMMIT,
    PRE_EDIT
  };

  /**
   * @brief Used to specify whether to update the input style.
   */
  enum UpdateInputStyleType
  {
    UPDATE_INPUT_STYLE,
    DONT_UPDATE_INPUT_STYLE
  };

  using UpdateTextType = Ui::Integration::Text::UpdateTextType;

  static constexpr UpdateTextType NONE_UPDATED      = Ui::Integration::Text::TextUpdate::NONE_UPDATED;
  static constexpr UpdateTextType MODEL_UPDATED     = Ui::Integration::Text::TextUpdate::MODEL_UPDATED;
  static constexpr UpdateTextType DECORATOR_UPDATED = Ui::Integration::Text::TextUpdate::DECORATOR_UPDATED;

  /**
   * @brief Different placeholder-text can be shown when the control is active/inactive.
   */
  enum PlaceholderType
  {
    PLACEHOLDER_TYPE_ACTIVE,
    PLACEHOLDER_TYPE_INACTIVE,
  };

  /**
   * @brief Enumeration for Font Size Type.
   */
  enum FontSizeType
  {
    POINT_SIZE, // The size of font in points.
    PIXEL_SIZE  // The size of font in pixels.
  };

  struct NoTextTap
  {
    enum Action
    {
      NO_ACTION,           ///< Does no action if there is a tap on top of an area with no text.
      HIGHLIGHT,           ///< Highlights the nearest text (at the beginning or end of the text) and shows the text's selection
                           ///< popup.
      SHOW_SELECTION_POPUP ///< Shows the text's selection popup.
    };
  };

public: // Constructor.
  /**
   * @brief Create a new instance of a Controller.
   *
   * @return A pointer to a new Controller.
   */
  static ControllerPtr New()
  {
    return ControllerPtr(new Controller());
  }

  /**
   * @brief Create a new instance of a Controller.
   *
   * @param[in] controlInterface The control's interface.
   *
   * @return A pointer to a new Controller.
   */
  static ControllerPtr New(Ui::Integration::Text::ControlInterface* controlInterface)
  {
    return ControllerPtr(new Controller(controlInterface));
  }

  /**
   * @brief Create a new instance of a Controller.
   *
   * @param[in] controlInterface The control's interface.
   * @param[in] editableControlInterface The editable control's interface.
   * @param[in] selectableControlInterface The selectable control's interface.
   * @param[in] anchorControlInterface The anchor control's interface.
   *
   * @return A pointer to a new Controller.
   */
  static ControllerPtr New(Ui::Integration::Text::ControlInterface*           controlInterface,
                           Ui::Integration::Text::EditableControlInterface*   editableControlInterface,
                           Ui::Integration::Text::SelectableControlInterface* selectableControlInterface,
                           Ui::Integration::Text::AnchorControlInterface*     anchorControlInterface)
  {
    return ControllerPtr(
      new Controller(controlInterface, editableControlInterface, selectableControlInterface, anchorControlInterface));
  }

public: // Configure the text controller.
  /**
   * @brief Called to enable text input.
   *
   * @note Selectable or editable controls should call this once after Controller::New().
   * @param[in] decorator Used to create cursor, selection handle decorations etc.
   * @param[in] inputMethodContext Used to manager ime.
   */
  void EnableTextInput(DecoratorPtr decorator, InputMethodContext& inputMethodContext);

  /**
   * @brief Used to switch between bitmap & vector based glyphs
   *
   * @param[in] glyphType The type of glyph; note that metrics for bitmap & vector based glyphs are different.
   */
  void SetGlyphType(TextAbstraction::GlyphType glyphType);

  /**
   * @brief Retrieves whether the current text contains anchors.
   *
   * @return @e true if the current text contains anchors. @e false.
   */
  bool HasAnchors() const;

  /**
   * @brief Enables/disables the auto text scrolling
   *
   * By default is disabled.
   *
   * @param[in] enable Whether to enable the marqueeing
   * @param[in] requestRelayout Whether to request the relayout
   * @param[in] direction Direction of the marquee.
   */
  void SetMarqueeEnabled(bool enable, bool requestRelayout = true,
                         Text::MarqueeOrientation orientation = Text::MarqueeOrientation::HORIZONTAL);

  /**
   * @brief Whether the marqueeing texture exceed max texture.
   *
   * By default is false.
   *
   * @param[in] exceed Whether the marqueeing texture exceed max texture.
   */
  void SetMarqueeMaxTextureExceeded(bool exceed);

  /**
   * @brief Retrieves whether auto text scrolling is enabled.
   *
   * By default is disabled.
   *
   * @return @e true if marqueeing is enabled, otherwise returns @e false.
   */
  bool IsMarqueeEnabled() const;

  /**
   * @brief Get direction of the text from the first line of text,
   * @return bool rtl (right to left) is true
   */
  CharacterDirection GetMarqueeTextDirection() const;

  /**
   * @brief Get the alignment offset of the first line of text.
   *
   * @return The alignment offset.
   */
  float GetMarqueeLineAlignment() const;

  /**
   * @brief Enables the horizontal scrolling.
   *
   * @param[in] enable Whether to enable the horizontal scrolling.
   */
  void SetHorizontalScrollEnabled(bool enable);

  /**
   * @brief Retrieves whether the horizontal scrolling is enabled.
   *
   * @return @e true if the horizontal scrolling is enabled, otherwise it returns @e false.
   */
  bool IsHorizontalScrollEnabled() const;

  /**
   * @brief Enables the vertical scrolling.
   *
   * @param[in] enable Whether to enable the vertical scrolling.
   */
  void SetVerticalScrollEnabled(bool enable);

  /**
   * @brief Retrieves whether the verticall scrolling is enabled.
   *
   * @return @e true if the vertical scrolling is enabled, otherwise it returns @e false.
   */
  bool IsVerticalScrollEnabled() const;

  /**
   * @brief Enables the smooth handle panning.
   *
   * @param[in] enable Whether to enable the smooth handle panning.
   */
  void SetSmoothHandlePanEnabled(bool enable);

  /**
   * @brief Retrieves whether the smooth handle panning is enabled.
   *
   * @return @e true if the smooth handle panning is enabled.
   */
  bool IsSmoothHandlePanEnabled() const;

  /**
   * @brief Sets the maximum number of characters that can be inserted into the TextModel
   *
   * @param[in] maxCharacters maximum number of characters to be accepted
   */
  void SetMaximumNumberOfCharacters(Length maxCharacters);

  /**
   * @brief Sets the maximum number of characters that can be inserted into the TextModel
   *
   * @param[in] maxCharacters maximum number of characters to be accepted
   */
  int GetMaximumNumberOfCharacters();

  /**
   * @brief Called to enable/disable cursor blink.
   *
   * @note Only editable controls should calls this.
   * @param[in] enabled Whether the cursor should blink or not.
   */
  void SetEnableCursorBlink(bool enable);

  /**
   * @brief Query whether cursor blink is enabled.
   *
   * @return Whether the cursor should blink or not.
   */
  bool GetEnableCursorBlink() const;

  /**
   * @brief Whether to enable the multi-line layout.
   *
   * @param[in] enable \e true enables the multi-line (by default)
   */
  void SetMultiLineEnabled(bool enable);

  /**
   * @return Whether the multi-line layout is enabled.
   */
  bool IsMultiLineEnabled() const;

  /**
   * @brief Sets the text's horizontal alignment.
   *
   * @param[in] alignment The horizontal alignment.
   */
  void SetHorizontalAlignment(Alignment alignment);

  /**
   * @copydoc ModelInterface::GetHorizontalAlignment()
   */
  Alignment GetHorizontalAlignment() const;

  /**
   * @brief Sets the text's vertical alignment.
   *
   * @param[in] alignment The vertical alignment.
   */
  void SetVerticalAlignment(Alignment alignment);

  /**
   * @copydoc ModelInterface::GetVerticalAlignment()
   */
  Alignment GetVerticalAlignment() const;

  /**
   * @brief Sets the text's wrap mode
   * @param[in] text wrap mode The unit of wrapping
   */
  void SetLineWrapMode(LineWrapMode textWarpMode);

  /**
   * @brief Retrieve text wrap mode previously set.
   * @return text wrap mode
   */
  LineWrapMode GetLineWrapMode() const;

  /**
   * @brief Enable or disable the text elide.
   *
   * @param[in] enabled Whether to enable the text elide.
   */
  void SetTextElideEnabled(bool enabled);

  /**
   * @copydoc ModelInterface::IsTextElideEnabled()
   */
  bool IsTextElideEnabled() const;

  /**
   * @brief Enable or disable the text fit.
   *
   * @param[in] enabled Whether to enable the text fit.
   */
  void SetTextFitEnabled(bool enabled);

  /**
   * @brief Whether the text fit is enabled or not.
   *
   * @return True if the text fit is enabled
   */
  bool IsTextFitEnabled() const;

  /**
   * @brief Sets current line size.
   *
   * @param[in] lineSize line size value to store the MinLineSize set by user when TextFitCandidates is enabled.
   */
  void SetCurrentLineSize(float lineSize);

  /**
   * @brief Retrieves the current line size.
   *
   * @return The current line size
   */
  float GetCurrentLineSize() const;

  /**
   * @brief Sets minimum size valid for text fit.
   *
   * @param[in] minimum size value.
   * @param[in] type The font size type is point size or pixel size
   */
  void SetTextFitMinSize(float pointSize, FontSizeType type);

  /**
   * @brief Retrieves the minimum point size valid for text fit.
   * @param[in] type The font size type is point size or pixel size
   * @return The minimum point size valid for text fit
   */
  float GetTextFitMinSize(FontSizeType type) const;

  /**
   * @brief Sets maximum size valid for text fit.
   *
   * @param[in] maximum size value.
   * @param[in] type The font size type is point size or pixel size
   */
  void SetTextFitMaxSize(float pointSize, FontSizeType type);

  /**
   * @brief Retrieves the maximum point size valid for text fit.
   * @param[in] type The font size type is point size or pixel size
   * @return The maximum point size valid for text fit
   */
  float GetTextFitMaxSize(FontSizeType type) const;

  /**
   * @brief Sets step size for font increase valid for text fit.
   *
   * @param[in] step size value.
   * @param[in] type The font size type is point size or pixel size
   */
  void SetTextFitStepSize(float step, FontSizeType type);

  /**
   * @brief Retrieves the step point size valid for text fit.
   * @param[in] type The font size type is point size or pixel size
   * @return The step point size valid for text fit
   */
  float GetTextFitStepSize(FontSizeType type) const;

  /**
   * @brief Sets content size valid for text fit.
   *
   * @param[in] Content size value.
   */
  void SetTextFitContentSize(Vector2 size);

  /**
   * @brief Retrieves the content size valid for text fit.
   *
   * @return The content size valid for text fit
   */
  Vector2 GetTextFitContentSize() const;

  /**
   * @brief Retrieve the fited font size.
   * @param[in] type The font size type is point size or pixel size
   * @return The fited font size.
   */
  float GetTextFitFontSize(FontSizeType type) const;

  /**
   * @brief Sets the text fit point size.
   *
   * @param[in] pointSize The fited point size.
   */
  void SetTextFitPointSize(float pointSize);

  /**
   * @brief Sets whether the text fit properties have changed.
   *
   * @param[in] changed Whether to changed the text fit.
   */
  void SetTextFitChanged(bool changed);

  /**
   * @brief Whether the text fit properties are changed or not.
   *
   * @return True if the text fit properties are changed
   */
  bool IsTextFitChanged() const;

  /**
   * @brief Enable or disable the text fit candidates.
   *
   * @param[in] enabled Whether to enable the text fit candidates.
   */
  void SetTextFitCandidatesEnabled(bool enabled);

  /**
   * @brief Whether the text fit candidates are enabled or not.
   *
   * @return True if the text fit candidates are enabled.
   */
  bool IsTextFitCandidatesEnabled() const;

  /**
   * @brief Returns the maximum text fit candidate.
   *
   * The maximum candidate is selected based on the largest font size.
   * If multiple candidates have the same font size, the one with the larger
   * line height is returned.
   *
   * @return A pointer to the maximum fit candidate, or nullptr if no candidates exist.
   */
  const Text::Fit::Candidate* GetMaxFitCandidate() const;

  /**
   * @brief Sets the text fit candidates.
   *
   * @param[in] candidates The list of text fit candidates.
   */
  void SetTextFitCandidates(const Dali::Vector<Text::Fit::Candidate>& candidates);

  /**
   * @brief Retrieves the text fit candidates.
   *
   * @return The list of text fit candidates.
   */
  const Dali::Vector<Text::Fit::Candidate>& GetTextFitCandidates() const;

  /**
   * @brief Clears the text fit candidates.
   */
  void ClearTextFitCandidates();

  /**
   * @brief Sets disabled color opacity.
   *
   * @param[in] opacity The color opacity value in disabled state.
   */
  void SetDisabledColorOpacity(float opacity);

  /**
   * @brief Retrieves the disabled color opacity.
   *
   * @return The disabled color opacity value for disabled state.
   */
  float GetDisabledColorOpacity() const;

  /**
   * @brief Enable or disable the placeholder text elide.
   * @param enabled Whether to enable the placeholder text elide.
   */
  void SetPlaceholderTextElideEnabled(bool enabled);

  /**
   * @brief Whether the placeholder text elide property is enabled.
   * @return True if the placeholder text elide property is enabled, false otherwise.
   */
  bool IsPlaceholderTextElideEnabled() const;

  /**
   * @brief Enable or disable the text selection.
   * @param[in] enabled Whether to enable the text selection.
   */
  void SetSelectionEnabled(bool enabled);

  /**
   * @brief Whether the text selection is enabled or not.
   * @return True if the text selection is enabled
   */
  bool IsSelectionEnabled() const;

  /**
   * @brief Enable or disable the text selection using Shift key.
   * @param enabled Whether to enable the text selection using Shift key
   */
  void SetShiftSelectionEnabled(bool enabled);

  /**
   * @brief Whether the text selection using Shift key is enabled or not.
   * @return True if the text selection using Shift key is enabled
   */
  bool IsShiftSelectionEnabled() const;

  /**
   * @brief Enable or disable the grab handles for text selection.
   *
   * @param[in] enabled Whether to enable the grab handles
   */
  void SetGrabHandleEnabled(bool enabled);

  /**
   * @brief Returns whether the grab handles are enabled.
   *
   * @return True if the grab handles are enabled
   */
  bool IsGrabHandleEnabled() const;

  /**
   * @brief Enable or disable the grab handles for text selection.
   *
   * @param[in] enabled Whether to enable the grab handles
   */
  void SetGrabHandlePopupEnabled(bool enabled);

  /**
   * @brief Returns whether the grab handles are enabled.
   *
   * @return True if the grab handles are enabled
   */
  bool IsGrabHandlePopupEnabled() const;

  /**
   * @brief Sets input type to password
   *
   * @note The string is displayed hidden character
   *
   * @param[in] passwordInput True if password input is enabled.
   */
  void SetInputModePassword(bool passwordInput);

  /**
   * @brief Returns whether the input mode type is set as password.
   *
   * @return True if input mode type is password
   */
  bool IsInputModePassword();

  /**
   * @brief Sets the action when there is a double tap event on top of a text area with no text.
   *
   * @param[in] action The action to do.
   */
  void SetNoTextDoubleTapAction(NoTextTap::Action action);

  /**
   * @brief Retrieves the action when there is a double tap event on top of a text area with no text.
   *
   * @return The action to do.
   */
  NoTextTap::Action GetNoTextDoubleTapAction() const;

  /**
   * @brief Sets the action when there is a long press event on top of a text area with no text.
   *
   * @param[in] action The action to do.
   */
  void SetNoTextLongPressAction(NoTextTap::Action action);

  /**
   * @brief Retrieves the action when there is a long press event on top of a text area with no text.
   *
   * @return The action to do.
   */
  NoTextTap::Action GetNoTextLongPressAction() const;

  /**
   * @brief Query if Underline settings were provided by string or map
   * @return bool true if set by string
   */
  bool IsUnderlineSetByString();

  /**
   * Set method underline setting were set by
   * @param[in] bool, true if set by string
   */
  void UnderlineSetByString(bool setByString);

  /**
   * @brief Query if shadow settings were provided by string or map
   * @return bool true if set by string
   */
  bool IsShadowSetByString();

  /**
   * Set method shadow setting were set by
   * @param[in] bool, true if set by string
   */
  void ShadowSetByString(bool setByString);

  /**
   * @brief Query if outline settings were provided by string or map
   * @return bool true if set by string
   */
  bool IsOutlineSetByString();

  /**
   * Set method outline setting were set by
   * @param[in] bool, true if set by string
   */
  void OutlineSetByString(bool setByString);

  /**
   * @brief Query if font style settings were provided by string or map
   * @return bool true if set by string
   */
  bool IsFontStyleSetByString();

  /**
   * Set method font style setting were set by
   * @param[in] bool, true if set by string
   */
  void FontStyleSetByString(bool setByString);

  /**
   * @brief Query if Strikethrough settings were provided by string or map
   * @return bool true if set by string
   */
  bool IsStrikethroughSetByString();

  /**
   * Set method Strikethrough setting were set by
   * @param[in] bool, true if set by string
   */
  void StrikethroughSetByString(bool setByString);

  /**
   * @brief Set the override used for strikethrough height, 0 indicates height will be supplied by font metrics
   *
   * @param[in] height The height in pixels of the strikethrough
   */
  void SetStrikethroughHeight(float height);

  /**
   * @brief Retrieves the override height of an strikethrough, 0 indicates height is supplied by font metrics
   *
   * @return The height of the strikethrough, or 0 if height is not overrided.
   */
  float GetStrikethroughHeight() const;

  /**
   * @brief Set the strikethrough color.
   *
   * @param[in] color color of strikethrough.
   */
  void SetStrikethroughColor(const Vector4& color);

  /**
   * @brief Retrieve the strikethrough color.
   *
   * @return The strikethrough color.
   */
  const Vector4& GetStrikethroughColor() const;

  /**
   * @brief Set the strikethrough enabled flag.
   *
   * @param[in] enabled The strikethrough enabled flag.
   */
  void SetStrikethroughEnabled(bool enabled);

  /**
   * @brief Returns whether the text has a strikethrough or not.
   *
   * @return The strikethrough state.
   */
  bool IsStrikethroughEnabled() const;

public: // Update.
  /**
   * @brief Replaces any text previously set.
   *
   * @note This will be converted into UTF-32 when stored in the text model.
   * @param[in] text A string of UTF-8 characters.
   */
  void SetText(const std::string& text);

  /**
   * @brief Replaces any text previously set with a StyledText source.
   *
   * StyledText is applied as plain text plus span attachments. The raw markup
   * processor is not used by this path.
   *
   * @param[in] styledText The styled text snapshot to apply.
   */
  void SetStyledText(const StyledText& styledText);

  /**
   * @brief Gets the current editable StyledText snapshot.
   *
   * @return The current snapshot, or an empty handle for plain text content.
   */
  StyledText GetStyledText() const;

  /**
   * @brief Retrieve any text previously set.
   *
   * @param[out] text A string of UTF-8 characters.
   */
  void GetText(std::string& text) const;

  /**
   * @brief Retrieves number of characters previously set.
   *
   * @return A length of string of UTF-32 characters.
   */
  Length GetNumberOfCharacters() const;

  /**
   * @brief Replaces any placeholder text previously set.
   *
   * @param[in] type Different placeholder-text can be shown when the control is active/inactive.
   * @param[in] text A string of UTF-8 characters.
   */
  void SetPlaceholderText(PlaceholderType type, const std::string& text);

  /**
   * @brief Retrieve any placeholder text previously set.
   *
   * @param[in] type Different placeholder-text can be shown when the control is active/inactive.
   * @param[out] A string of UTF-8 characters.
   */
  void GetPlaceholderText(PlaceholderType type, std::string& text) const;

  /**
   * @brief Sets whether the placeholder text is shown when the input field has focus.
   *
   * When enabled, the placeholder text may remain visible while the input field
   * is focused, depending on the current text state.
   *
   * @param[in] enabled True to show the placeholder text when focused, false otherwise.
   */
  void SetShowPlaceholderOnFocus(bool enabled);

  /**
   * @brief Returns whether the placeholder text is shown when the input field has focus.
   *
   * @return True if the placeholder text is shown when focused, false otherwise.
   */
  bool IsPlaceholderShownOnFocus() const;

  /**
   * @ brief Update the text after a font change
   * @param[in] newDefaultFont The new font to change to
   */
  void UpdateAfterFontChange(const std::string& newDefaultFont);

  /**
   * @brief The method acquires currently selected text
   * @param selectedText variable to place selected text in
   */
  void RetrieveSelection(std::string& selectedText) const;

  /**
   * @brief The method sets selection in given range
   * @param start index of first character
   * @param end   index of first character after selection
   */
  void SetSelection(int start, int end);

  /**
   * @brief This method retrieve indexes of current selection
   *
   * @return a pair, where first element is left index of selection and second is the right one
   */
  std::pair<int, int> GetSelectionIndexes() const;

  /**
   * Place string in system clipboard
   * @param source std::string
   */
  void CopyStringToClipboard(const std::string& source);

  /**
   * Place currently selected text in system clipboard
   * @param deleteAfterSending flag pointing if text should be deleted after sending to clipboard
   */
  void SendSelectionToClipboard(bool deleteAfterSending);

  /**
   * @brief Retrieve the line height of the default font.
   */
  float GetDefaultFontLineHeight();

  /**
   * @brief Retrieve the default editable line box height including line spacing.
   */
  float GetDefaultLineBoxHeight();

public: // Default style & Input style
  /**
   * @brief Set the default font family.
   *
   * @param[in] defaultFontFamily The default font family.
   */
  void SetDefaultFontFamily(const std::string& defaultFontFamily);

  /**
   * @brief Retrieve the default font family.
   *
   * @return The default font family.
   */
  std::string GetDefaultFontFamily() const;

  /**
   * @brief Sets the placeholder text font family.
   * @param[in] placeholderTextFontFamily The placeholder text font family.
   */
  void SetPlaceholderFontFamily(const std::string& placeholderTextFontFamily);

  /**
   * @brief Retrieves the placeholder text font family.
   *
   * @return The placeholder text font family
   */
  std::string GetPlaceholderFontFamily() const;

  /**
   * @brief Sets the default font weight.
   *
   * @param[in] weight The font weight.
   */
  void SetDefaultFontWeight(FontWeightType weight);

  /**
   * @brief Whether the font's weight has been defined.
   */
  bool IsDefaultFontWeightDefined() const;

  /**
   * @brief Retrieves the default font weight.
   *
   * @return The default font weight.
   */
  FontWeightType GetDefaultFontWeight() const;

  /**
   * @brief Sets the placeholder text font weight.
   *
   * @param[in] weight The font weight
   */
  void SetPlaceholderTextFontWeight(FontWeightType weight);

  /**
   * @brief Whether the font's weight has been defined.
   *
   * @return True if the placeholder text font weight is defined
   */
  bool IsPlaceholderTextFontWeightDefined() const;

  /**
   * @brief Retrieves the placeholder text font weight.
   *
   * @return The placeholder text font weight
   */
  FontWeightType GetPlaceholderTextFontWeight() const;

  /**
   * @brief Sets the default font width.
   *
   * @param[in] width The font width.
   */
  void SetDefaultFontWidth(FontWidthType width);

  /**
   * @brief Whether the font's width has been defined.
   */
  bool IsDefaultFontWidthDefined() const;

  /**
   * @brief Retrieves the default font width.
   *
   * @return The default font width.
   */
  FontWidthType GetDefaultFontWidth() const;

  /**
   * @brief Sets the placeholder text font width.
   *
   * @param[in] width The font width
   */
  void SetPlaceholderTextFontWidth(FontWidthType width);

  /**
   * @brief Whether the font's width has been defined.
   *
   * @return True if the placeholder text font width is defined
   */
  bool IsPlaceholderTextFontWidthDefined() const;

  /**
   * @brief Retrieves the placeholder text font width.
   *
   * @return The placeholder text font width
   */
  FontWidthType GetPlaceholderTextFontWidth() const;

  /**
   * @brief Sets the default font slant.
   *
   * @param[in] slant The font slant.
   */
  void SetDefaultFontSlant(FontSlantType slant);

  /**
   * @brief Whether the font's slant has been defined.
   */
  bool IsDefaultFontSlantDefined() const;

  /**
   * @brief Retrieves the default font slant.
   *
   * @return The default font slant.
   */
  FontSlantType GetDefaultFontSlant() const;

  /**
   * @brief Sets the placeholder text font slant.
   *
   * @param[in] slant The font slant
   */
  void SetPlaceholderTextFontSlant(FontSlantType slant);

  /**
   * @brief Whether the font's slant has been defined.
   *
   * @return True if the placeholder text font slant is defined
   */
  bool IsPlaceholderTextFontSlantDefined() const;

  /**
   * @brief Retrieves the placeholder text font slant.
   *
   * @return The placeholder text font slant
   */
  FontSlantType GetPlaceholderTextFontSlant() const;

  /**
   * @brief Set the default font size.
   *
   * @param[in] fontSize The default font size
   * @param[in] type The font size type is point size or pixel size
   */
  void SetDefaultFontSize(float fontSize, FontSizeType type);

  /**
   * @brief Retrieve the default point size.
   *
   * @param[in] type The font size type
   * @return The default point size.
   */
  float GetDefaultFontSize(FontSizeType type) const;

  /**
   * @brief Gets the effective scale used for text-specific metrics.
   *
   * The effective text scale combines the adjusted font size scale and UI scale.
   *
   * @return The effective text scale.
   */
  float GetEffectiveTextScale() const;

  /**
   * @brief Sets the UI scale used for text-specific metrics.
   *
   * This scale is used for text-specific size calculation, such as font size.
   *
   * @param[in] scale The UI scale.
   * @return True if the scale was changed, false otherwise.
   */
  bool SetUiScale(float scale);

  /**
   * @brief Gets the UI scale used for text-specific metrics.
   *
   * @return The UI scale.
   */
  float GetUiScale() const;

  /**
   * @brief Set the font size scale.
   *
   * @param[in] scale The font size scale
   */
  void SetFontSizeScale(float scale);

  /**
   * @brief Get the font size scale.
   *
   * @return The font size scale.
   */
  float GetFontSizeScale() const;

  /**
   * @brief Sets the minimum font size scale.
   *
   * If this value is greater than the maximum font size scale,
   * the adjusted font size scale follows this minimum value.
   *
   * @param[in] scale The minimum font size scale.
   */
  void SetMinimumFontSizeScale(float scale);

  /**
   * @brief Gets the minimum font size scale.
   *
   * @return The minimum font size scale.
   */
  float GetMinimumFontSizeScale() const;

  /**
   * @brief Sets the maximum font size scale.
   *
   * If this value is less than the minimum font size scale,
   * the adjusted font size scale follows the minimum font size scale.
   *
   * @param[in] scale The maximum font size scale.
   */
  void SetMaximumFontSizeScale(float scale);

  /**
   * @brief Gets the maximum font size scale.
   *
   * @return The maximum font size scale.
   */
  float GetMaximumFontSizeScale() const;

  /**
   * @brief Sets whether the system font size scale is applied.
   *
   * When enabled, the system font size scale is used instead of the user-defined font size scale
   * before applying the minimum and maximum constraints.
   *
   * @param[in] enabled True to apply the system font size scale, false otherwise.
   */
  void SetSystemFontSizeScaleEnabled(bool enabled);

  /**
   * @brief Gets whether the system font size scale is applied.
   *
   * @return True if the system font size scale is applied, otherwise false.
   */
  bool IsSystemFontSizeScaleEnabled() const;

  /**
   * @brief Sets the system font size scale.
   *
   * @param[in] scale The system font size scale.
   */
  void SetSystemFontSizeScale(float scale);

  /**
   * @brief Gets the adjusted font size scale used for rendering.
   *
   * The adjusted font size scale is resolved after applying the current
   * minimum and maximum font size scale constraints and, if enabled,
   * the system font size scale.
   *
   * If the minimum font size scale is greater than the maximum font size scale,
   * the minimum font size scale takes precedence and is used as the adjusted scale.
   *
   * @return The adjusted font size scale used for rendering.
   */
  float GetAdjustedFontSizeScale() const;

  /**
   * @brief Sets the Placeholder text font size.
   * @param[in] fontSize The placeholder text font size
   * @param[in] type The font size type is point size or pixel size
   */
  void SetPlaceholderTextFontSize(float fontSize, FontSizeType type);

  /**
   * @brief Retrieves the Placeholder text font size.
   * @param[in] type The font size type
   * @return The placeholder font size
   */
  float GetPlaceholderTextFontSize(FontSizeType type) const;

  /**
   * @brief Sets the text's default color.
   *
   * @param color The default color.
   */
  void SetDefaultColor(const Vector4& color);

  /**
   * @brief Retrieves the text's default color.
   *
   * @return The default color.
   */
  const Vector4& GetDefaultColor() const;

  /**
   * @brief Sets the anchor's default color.
   *
   * @param color The anchor color.
   */
  void SetAnchorColor(const Vector4& color);

  /**
   * @brief Retrieves the anchor's default color.
   *
   * @return The anchor color.
   */
  const Vector4& GetAnchorColor() const;

  /**
   * @brief Sets the anchor's clicked color.
   *
   * @param color The anchor color.
   */
  void SetAnchorClickedColor(const Vector4& color);

  /**
   * @brief Retrieves the anchor's clicked color.
   *
   * @return The anchor color.
   */
  const Vector4& GetAnchorClickedColor() const;

  /**
   * @brief Sets the user interaction enabled.
   *
   * @param enabled whether to enable the user interaction.
   */
  void SetUserInteractionEnabled(bool enabled);

  /**
   * @brief Whether the user interaction is enabled.
   *
   * @return true if the user interaction is enabled, false otherwise.
   */
  bool IsUserInteractionEnabled() const;

  /**
   * @brief Set the text color
   *
   * @param textColor The text color
   */
  void SetPlaceholderTextColor(const Vector4& textColor);

  /**
   * @brief Retrieve the text color
   *
   * @return The text color
   */
  const Vector4& GetPlaceholderTextColor() const;

  /**
   * @brief Set the shadow offset.
   *
   * @param[in] shadowOffset The shadow offset.
   */
  void SetShadowOffset(const Vector2& shadowOffset);

  /**
   * @brief Retrieve the shadow offset.
   *
   * @return The shadow offset.
   */
  const Vector2& GetShadowOffset() const;

  /**
   * @brief Set the shadow enabled flag.
   *
   * @param[in] enabled The shadow enabled flag.
   */
  void SetShadowEnabled(bool enabled);

  /**
   * @brief Returns whether the text shadow is enabled or not.
   *
   * @return The shadow state.
   */
  bool IsShadowEnabled() const;

  /**
   * @brief Set the shadow color.
   *
   * @param[in] shadowColor The shadow color.
   */
  void SetShadowColor(const Vector4& shadowColor);

  /**
   * @brief Retrieve the shadow color.
   *
   * @return The shadow color.
   */
  const Vector4& GetShadowColor() const;

  /**
   * @brief Set the shadow blur radius.
   *
   * @param[in] shadowBlurRadius The shadow blur radius, 0,0 indicates no blur.
   */
  void SetShadowBlurRadius(const float& shadowBlurRadius);

  /**
   * @brief Retrieve the shadow blur radius.
   *
   * @return The shadow blur radius.
   */
  const float& GetShadowBlurRadius() const;

  /**
   * @brief Set the emboss enabled flag.
   *
   * @param[in] enable The emboss enabled flag.
   */
  void SetEmbossEnabled(const bool enable);

  /**
   * @brief Returns whether the text is embossed or not.
   *
   * @return The emboss state.
   */
  bool IsEmbossEnabled() const;

  /**
   * @brief Set the emboss direction.
   *
   * @param[in] direction The emboss direction.
   */
  void SetEmbossDirection(const Vector2& direction);

  /**
   * @brief Retrieve the emboss direction.
   *
   * @return The emboss direction.
   */
  const Vector2& GetEmbossDirection() const;

  /**
   * @brief Set the emboss strength.
   *
   * @param[in] strength The emboss strength.
   */
  void SetEmbossStrength(const float strength);

  /**
   * @brief Retrieve the emboss strength.
   *
   * @return The emboss strength.
   */
  float GetEmbossStrength() const;

  /**
   * @brief Set the emboss light color.
   *
   * @param[in] lightColor The emboss light color.
   */
  void SetEmbossLightColor(const Vector4& lightColor);

  /**
   * @brief Retrieve the emboss light color.
   *
   * @return The emboss light color.
   */
  const Vector4& GetEmbossLightColor() const;

  /**
   * @brief Set the emboss shadow color.
   *
   * @param[in] shadowColor The emboss shadow color.
   */
  void SetEmbossShadowColor(const Vector4& shadowColor);

  /**
   * @brief Retrieve the emboss shadow color.
   *
   * @return The emboss shadow color.
   */
  const Vector4& GetEmbossShadowColor() const;

  /**
   * @brief Set the underline color.
   *
   * @param[in] color color of underline.
   */
  void SetUnderlineColor(const Vector4& color);

  /**
   * @brief Retrieve the underline color.
   *
   * @return The underline color.
   */
  const Vector4& GetUnderlineColor() const;

  /**
   * @brief Set the underline enabled flag.
   *
   * @param[in] enabled The underline enabled flag.
   */
  void SetUnderlineEnabled(bool enabled);

  /**
   * @brief Returns whether the text is underlined or not.
   *
   * @return The underline state.
   */
  bool IsUnderlineEnabled() const;

  /**
   * @brief Set the override used for underline height, 0 indicates height will be supplied by font metrics
   *
   * @param[in] height The height in pixels of the underline
   */
  void SetUnderlineHeight(float height);

  /**
   * @brief Retrieves the override height of an underline, 0 indicates height is supplied by font metrics
   *
   * @return The height of the underline, or 0 if height is not overrided.
   */
  float GetUnderlineHeight() const;

  /**
   * @brief Sets the underline type.
   * @param[in] type The underline type.
   */
  void SetUnderlineType(Text::Underline::Type type);

  /**
   * @brief Retrieve underline type.
   * @return The underline type.
   */
  Text::Underline::Type GetUnderlineType() const;

  /**
   * @brief Set the width of the dashes of the dashed underline.
   *
   * @param[in] width The width in pixels of the dashes of the dashed underline.
   */
  void SetDashedUnderlineWidth(float width);

  /**
   * @brief Retrieves the width of the dashes of the dashed underline.
   *
   * @return The width of the dashes of the dashed underline.
   */
  float GetDashedUnderlineWidth() const;

  /**
   * @brief Set the gap between the dashes of the dashed underline.
   *
   * @param[in] gap The gap between the dashes of the dashed underline.
   */
  void SetDashedUnderlineGap(float gap);

  /**
   * @brief Retrieves the gap between the dashes of the dashed underline.
   *
   * @return The The gap between the dashes of the dashed underline.
   */
  float GetDashedUnderlineGap() const;

  /**
   * @brief Set the outline offset.
   *
   * @param[in] outlineOffset The outline offset.
   */
  void SetOutlineOffset(const Vector2& outlineOffset);

  /**
   * @brief Retrieve the outline offset.
   *
   * @return The outline offset.
   */
  const Vector2& GetOutlineOffset() const;

  /**
   * @brief Set the outline color.
   *
   * @param[in] color color of outline.
   */
  void SetOutlineColor(const Vector4& color);

  /**
   * @brief Retrieve the outline color.
   *
   * @return The outline color.
   */
  const Vector4& GetOutlineColor() const;

  /**
   * @brief Set the outline width
   *
   * @param[in] width The width in pixels of the outline.
   */
  void SetOutlineWidth(uint16_t width);

  /**
   * @brief Retrieves the width of an outline
   *
   * @return The width of the outline.
   */
  uint16_t GetOutlineWidth() const;

  /**
   * @brief Set the outline enabled flag.
   *
   * @param[in] enabled The outline enabled flag.
   */
  void SetOutlineEnabled(bool enabled);

  /**
   * @brief Returns whether the text outline is enabled or not.
   *
   * @return The outline state.
   */
  bool IsOutlineEnabled() const;

  /**
   * @brief Set the outline blur radius.
   *
   * @param[in] outlineBlurRadius The outline blur radius, 0,0 indicates no blur.
   */
  void SetOutlineBlurRadius(const float& outlineBlurRadius);

  /**
   * @brief Retrieve the outline blur radius.
   *
   * @return The outline blur radius.
   */
  const float& GetOutlineBlurRadius() const;

  /**
   * @brief Set the background color.
   *
   * @param[in] color color of background.
   */
  void SetBackgroundColor(const Vector4& color);

  /**
   * @brief Retrieve the background color.
   *
   * @return The background color.
   */
  const Vector4& GetBackgroundColor() const;

  /**
   * @brief Set the background enabled flag.
   *
   * @param[in] enabled The background enabled flag.
   */
  void SetBackgroundEnabled(bool enabled);

  /**
   * @brief Returns whether to enable text background or not.
   *
   * @return Whether text background is enabled.
   */
  bool IsBackgroundEnabled() const;

  /**
   * @brief Sets the emboss's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] embossProperties The emboss's properties string.
   */
  void SetDefaultEmbossProperties(const std::string& embossProperties);

  /**
   * @brief Retrieves the emboss's properties string.
   *
   * @return The emboss's properties string.
   */
  std::string GetDefaultEmbossProperties() const;

  /**
   * @brief Sets the outline's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] outlineProperties The outline's properties string.
   */
  void SetDefaultOutlineProperties(const std::string& outlineProperties);

  /**
   * @brief Retrieves the outline's properties string.
   *
   * @return The outline's properties string.
   */
  std::string GetDefaultOutlineProperties() const;

  /**
   * @brief Sets the default line spacing.
   *
   * @param[in] lineSpacing The line spacing.
   *
   * @return True if lineSpacing has been updated, false otherwise
   */
  bool SetDefaultLineSpacing(float lineSpacing);

  /**
   * @brief Retrieves the default line spacing.
   *
   * @return The line spacing.
   */
  float GetDefaultLineSpacing() const;

  /**
   * @brief Sets the default line size.
   *
   * @param[in] lineSize The line size.
   *
   * @return True if lineSize has been updated, false otherwise
   */
  bool SetDefaultLineSize(float lineSize);

  /**
   * @brief Retrieves the default line size.
   *
   * @return The line size.
   */
  float GetDefaultLineSize() const;

  /**
   * @brief Sets the relative line size to the original line size.
   *
   * @param[in] relativeLineSize The relativeline size.
   *
   * @return True if relativeLineSize has been updated, false otherwise
   */
  bool SetRelativeLineSize(float lineSize);

  /**
   * @brief Retrieves the relative line size.
   *
   * @return The relative line size.
   */
  float GetRelativeLineSize() const;

  /**
   * @brief Sets the input text's color.
   *
   * @param[in] color The input text's color.
   */
  void SetInputColor(const Vector4& color);

  /**
   * @brief Retrieves the input text's color.
   *
   * @return The input text's color.
   */
  const Vector4& GetInputColor() const;

  /**
   * @brief Sets the input text's font family name.
   *
   * @param[in] fontFamily The text's font family name.
   */
  void SetInputFontFamily(const std::string& fontFamily);

  /**
   * @brief Retrieves the input text's font family name.
   *
   * @return The input text's font family name.
   */
  std::string GetInputFontFamily() const;

  /**
   * @brief Sets the input font's weight.
   *
   * @param[in] weight The input font's weight.
   */
  void SetInputFontWeight(FontWeightType weight);

  /**
   * @return Whether the font's weight has been defined.
   */
  bool IsInputFontWeightDefined() const;

  /**
   * @brief Retrieves the input font's weight.
   *
   * @return The input font's weight.
   */
  FontWeightType GetInputFontWeight() const;

  /**
   * @brief Sets the input font's width.
   *
   * @param[in] width The input font's width.
   */
  void SetInputFontWidth(FontWidthType width);

  /**
   * @return Whether the font's width has been defined.
   */
  bool IsInputFontWidthDefined() const;

  /**
   * @brief Retrieves the input font's width.
   *
   * @return The input font's width.
   */
  FontWidthType GetInputFontWidth() const;

  /**
   * @brief Sets the input font's slant.
   *
   * @param[in] slant The input font's slant.
   */
  void SetInputFontSlant(FontSlantType slant);

  /**
   * @return Whether the font's slant has been defined.
   */
  bool IsInputFontSlantDefined() const;

  /**
   * @brief Retrieves the input font's slant.
   *
   * @return The input font's slant.
   */
  FontSlantType GetInputFontSlant() const;

  /**
   * @brief Sets the input font size in the specified unit.
   *
   * @param[in] fontSize The input font size.
   * @param[in] type The unit of the input font size.
   * @param[in] defaultFontSizeUpdated True if the default font size is updated
   * and sets the input font size, false otherwise.
   */
  void SetInputFontSize(float fontSize, FontSizeType type, bool defaultFontSizeUpdated = false);

  /**
   * @brief Retrieves the input font size in the requested unit.
   *
   * @param[in] type The font size unit to return.
   * @return The input font size in the requested unit.
   */
  float GetInputFontSize(FontSizeType type) const;

  /**
   * @brief Sets the input line spacing.
   *
   * @param[in] lineSpacing The line spacing.
   */
  void SetInputLineSpacing(float lineSpacing);

  /**
   * @brief Retrieves the input line spacing.
   *
   * @return The line spacing.
   */
  float GetInputLineSpacing() const;

  /**
   * @brief Sets the input shadow's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] shadowProperties The shadow's properties string.
   */
  void SetInputShadowProperties(const std::string& shadowProperties);

  /**
   * @brief Retrieves the input shadow's properties string.
   *
   * @return The shadow's properties string.
   */
  std::string GetInputShadowProperties() const;

  /**
   * @brief Sets the input underline's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] underlineProperties The underline's properties string.
   */
  void SetInputUnderlineProperties(const std::string& underlineProperties);

  /**
   * @brief Retrieves the input underline's properties string.
   *
   * @return The underline's properties string.
   */
  std::string GetInputUnderlineProperties() const;

  /**
   * @brief Sets the input emboss's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] embossProperties The emboss's properties string.
   */
  void SetInputEmbossProperties(const std::string& embossProperties);

  /**
   * @brief Retrieves the input emboss's properties string.
   *
   * @return The emboss's properties string.
   */
  std::string GetInputEmbossProperties() const;

  /**
   * @brief Sets input the outline's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] outlineProperties The outline's properties string.
   */
  void SetInputOutlineProperties(const std::string& outlineProperties);

  /**
   * @brief Retrieves the input outline's properties string.
   *
   * @return The outline's properties string.
   */
  std::string GetInputOutlineProperties() const;

  /**
   * @brief Sets the input strikethrough's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] strikethroughProperties The strikethrough's properties string.
   */
  void SetInputStrikethroughProperties(const std::string& strikethroughProperties);

  /**
   * @brief Retrieves the input strikethrough's properties string.
   *
   * @return The strikethrough's properties string.
   */
  std::string GetInputStrikethroughProperties() const;

  /**
   * @brief Set the control's interface.
   *
   * @param[in] controlInterface The control's interface.
   */
  void SetControlInterface(Ui::Integration::Text::ControlInterface* controlInterface);

  /**
   * @brief Set the anchor control's interface.
   *
   * @param[in] anchorControlInterface The control's interface.
   */
  void SetAnchorControlInterface(Ui::Integration::Text::AnchorControlInterface* anchorControlInterface);

  /**
   * @brief Sets the character spacing.
   *
   * @note A positive value will make the characters far apart (expanded) and a negative value will bring them closer
   * (condensed).
   *
   * @param[in] characterSpacing The character spacing.
   */
  void SetCharacterSpacing(float characterSpacing);

  /**
   * @brief Retrieves the character spacing.
   *
   * @note A positive value will make the characters far apart (expanded) and a negative value will bring them closer
   * (condensed).
   *
   * @return The character spacing.
   */
  const float GetCharacterSpacing() const;

  /**
   * @brief Sets the layout alignment offset.
   *
   * @note This does not include padding. If the layout alignment offset includes padding, it is a visual transform
   * offset.
   *
   * @param[in] offset The offset.
   */
  void SetLayoutAlignmentOffset(Vector2 offset);

  /**
   * @brief Sets the layout alignment offset with padding.
   *
   * @param[in] offset The offset.
   */
  void SetLayoutOffsetWithPadding(Vector2 offset);

  /**
   * @brief Retrieves the layout alignment offset with padding.
   *
   * @return The offset from the text content coordinates to the control coordinates.
   */
  Vector2 GetLayoutOffsetWithPadding() const;

  /**
   * @brief Sets whether background color with cutout is enabled.
   *
   * @param[in] enable True if enabled.
   */
  void SetBackgroundWithCutoutEnabled(bool enable);

  /**
   * @brief Whether background color with cutout is enabled.
   *
   * @return True if enabled.
   */
  bool IsBackgroundWithCutoutEnabled() const;

  /**
   * @brief Sets whether background color with cutout.
   *
   * @param[in] color The color to set.
   */
  void SetBackgroundColorWithCutout(const Vector4& color);

  /**
   * @brief Retrieves background color with cutout.
   *
   * @return The color.
   */
  const Vector4 GetBackgroundColorWithCutout() const;

  /**
   * @brief Sets offset with cutout.
   *
   * @param[in] offset The offset.
   */
  void SetOffsetWithCutout(const Vector2& offset);

public: // Queries & retrieves.
  /**
   * @brief Return the layout engine.
   *
   * @return A reference to the layout engine.
   */
  Layout::Engine& GetLayoutEngine();

  /**
   * @brief Return a view of the text.
   *
   * @return A reference to the view.
   */
  View& GetView();

  /**
   * @copydoc Control::GetNaturalSize()
   */
  Vector3 GetNaturalSize(bool convertToEven = true);

  /**
   * @copydoc Control::GetHeightForWidth()
   */
  float GetHeightForWidth(float width);

  /**
   * @brief Called by the Controller to get the layout size for a particular width and height.
   *
   * @param[in] width The width.
   * @param[in] height The height.
   * @param[in] forceUpdate Forces updates to recalculate layout, alignment etc.
   * @return Size of the laid-out text.
   */
  Vector2 CalculateLayoutSize(float width, float height, bool forceUpdate = false);

  /**
   * @brief Calculates the point size for text for given layout()
   */
  void FitPointSizeforLayout(Size layoutSize);

  /**
   * @brief Calculates the point size for text for the given layout using text fit candidates.
   */
  void FitCandidatesPointSizeForLayout(Size layoutSize);

  /**
   * @brief Checks if the point size fits within the layout size.
   *
   * @return Whether the point size fits within the layout size.
   */
  bool CheckForTextFit(float pointSize, Size& layoutSize);

  /**
   * @brief Retrieves the text's number of lines for a given width.
   * @param[in] width The width of the text's area.
   * @ return The number of lines.
   */
  int GetLineCount(float width);

  /**
   * @brief Sets whether the TextChangedSignal is currently being emitted.
   * @param[in] emitting Whether the signal is being emitted.
   */
  void SetTextChangedSignalEmission(bool emitting);

  /**
   * @brief Gets whether the TextChangedSignal is currently being emitted.
   * @return Whether the signal is being emitted.
   */
  bool IsTextChangedSignalEmission() const;

  /**
   * @brief Retrieves the immutable logical/source-domain model.
   *
   * This model always retains the original UTF-32 text and authored semantic
   * ranges. Editing, accessibility and public source queries use this model.
   */
  const ModelInterface* GetLogicalTextModel() const;

  /**
   * @brief Retrieves the authoritative layout/render-domain model.
   *
   * This is the logical model on the ordinary fast path and the projected
   * processing model while valid replacements are active.
   */
  const ModelInterface* GetRenderTextModel() const;

  /**
   * @brief Checks whether valid replacement source data is present.
   *
   * @return true if replacement processing is required.
   */
  bool HasValidReplacementSource() const;

  /**
   * @brief Retrieves the final replacement glyph sequence.
   *
   * @return The resolved result, or nullptr when replacements are inactive.
   */
  const FinalElisionResult* GetFinalElisionResult() const;

  /**
   * @brief Resolves a retained static END-ellipsis anchor for a horizontal marquee transition.
   *
   * @return A stable retained source anchor, or an invalid legacy fallback.
   */
  MarqueeStartAnchor GetMarqueeStartAnchor() const;

  /**
   * @brief Gets the authored replacement snapshot.
   *
   * @return The copy-safe replacement source for the current text.
   */
  const ReplacementSourceSnapshot& GetReplacementSourceSnapshot() const;

  /**
   * @brief Gets the latest replacement render state.
   *
   * @return The projected layout and final replacement placements.
   */
  const ReplacementRenderState& GetReplacementRenderState() const;

  /**
   * @brief Used to get scrolled distance by user input
   *
   * @return Distance from last scroll offset to new scroll offset
   */
  float GetScrollAmountByUserInput();

  /**
   * @brief Get latest scroll amount, control size and layout size
   *
   * This method is used to get information of control's scroll
   * @param[out] scrollPosition The current scrolled position
   * @param[out] controlHeight The size of a UI control
   * @param[out] layoutHeight The size of a bounding box to layout text within.
   *
   * @return Whether the text scroll position is changed or not after last update.
   */
  bool GetTextScrollInfo(float& scrollPosition, float& controlHeight, float& layoutHeight);

  /**
   * @brief Sets the password display mode.
   *
   * @param[in] mode The password display mode.
   */
  void SetPasswordMode(PasswordMode mode);

  /**
   * @brief Gets the password display mode.
   *
   * @return The password display mode.
   */
  PasswordMode GetPasswordMode() const;

  /**
   * @brief Sets the character used to mask password text.
   *
   * @param[in] character The Unicode code point used as the password mask character.
   */
  void SetPasswordMaskCharacter(uint32_t character);

  /**
   * @brief Gets the character used to mask password text.
   *
   * @return The Unicode code point used as the password mask character.
   */
  uint32_t GetPasswordMaskCharacter() const;

  /**
   * @brief Sets the duration for which the last entered password character remains visible.
   *
   * This value is used when PasswordMode is REVEAL_LAST_CHARACTER.
   *
   * @param[in] duration The duration in milliseconds.
   */
  void SetPasswordRevealDuration(int duration);

  /**
   * @brief Gets the duration for which the last entered password character remains visible.
   *
   * @return The duration in milliseconds.
   */
  int GetPasswordRevealDuration() const;

  /**
   * @brief Clears hidden text substitution.
   */
  void ClearHiddenText();

  /**
   * @brief Masks all characters.
   */
  void HideAllText();

  /**
   * @brief Masks the first count characters.
   *
   * Characters after count remain visible.
   *
   * @param[in] count The number of characters to mask from the start.
   */
  void HideFirstCharacters(uint32_t count);

  /**
   * @brief Shows the first count characters and masks the remaining characters.
   *
   * @param[in] count The number of characters to show from the start.
   */
  void ShowFirstCharacters(uint32_t count);

  /**
   * @brief Gets the current hidden text mode.
   *
   * @return The current hidden text mode.
   */
  HiddenText::Mode GetHiddenTextMode() const;

  /**
   * @brief Gets the character count used by HIDE_COUNT and SHOW_COUNT modes.
   *
   * @return The configured character count.
   */
  uint32_t GetHiddenTextSubstituteCount() const;

  /**
   * @brief Sets the input filter.
   *
   * @param[in] inputFilter The input filter to apply.
   */
  void SetInputFilter(const InputFilter& inputFilter);

  /**
   * @brief Gets the input filter.
   *
   * @return The current input filter, or Text::InputFilter::None() if not set.
   */
  InputFilter GetInputFilter() const;

  /**
   * @brief Sets the Placeholder Properties.
   *
   * @param[in] map The placeholder property map
   */
  void SetPlaceholderProperty(const Property::Map& map);

  /**
   * @brief Retrieves the Placeholder Property map.
   *
   * @param[out] map The property map
   */
  void GetPlaceholderProperty(Property::Map& map);

  /**
   * @brief Checks text direction.
   * @return The text direction.
   */
  Direction GetTextDirection();

  /**
   * @brief Retrieves vertical line alignment
   * @return The vertical line alignment
   */
  Alignment GetVerticalLineAlignment() const;

  /**
   * @brief Sets vertical line alignment
   * @param[in] alignment The vertical line alignment for the text
   */
  void SetVerticalLineAlignment(Alignment alignment);

  /**
   * @brief Retrieves ellipsis position
   * @return The ellipsis position
   */
  Text::EllipsisPosition::Type GetEllipsisPosition() const;

  /**
   * @brief Sets ellipsis position
   * @param[in] ellipsisPosition The ellipsis position for the text
   */
  void SetEllipsisPosition(Text::EllipsisPosition::Type ellipsisPosition);

  /**
   * @brief Sets the render scale
   * @param[in] renderScale The render scale
   */
  void SetRenderScale(const float renderScale);

  /**
   * @brief Retrieves the render scale
   * @return The value of the render scale
   */
  float GetRenderScale() const;

  /**
   * @brief Retrieves removeFrontInset value from model
   * @return The value of removeFrontInset
   */
  bool IsRemoveFrontInset() const;

  /**
   * @brief Sets removeFrontInset value to model
   * @param[in] remove The value of removeFrontInset for the text
   */
  void SetRemoveFrontInset(bool remove);

  /**
   * @brief Retrieves removeBackInset value from model
   * @return The value of removeBackInset
   */
  bool IsRemoveBackInset() const;

  /**
   * @brief Sets removeBackInset value to model
   * @param[in] remove The value of removeBackInset for the text
   */
  void SetRemoveBackInset(bool remove);

  /**
   * @brief Retrieves cursorInsetEnabled value.
   * @return Whether the cursor inset is enabled.
   */
  bool IsCursorInsetEnabled() const;

  /**
   * @brief Sets cursorInsetEnabled value.
   * @param[in] enable Whether the cursor inset is enabled.
   */
  void SetCursorInsetEnabled(bool enable);

  /**
   * @brief Retrieves cutout value to model
   * @return The value of cutout for the text
   */
  bool IsTextCutout() const;

  /**
   * @brief Sets cutout value to model
   * @param[in] cutout The value of cutout for the text
   */
  void SetTextCutout(bool cutout);

  /**
   * @brief Retrieves variation values from the model
   * @param[out] map The font variation map
   */
  void GetVariationsMap(Property::Map& map);

  /**
   * @brief Sets variation values to the model
   * @param[in] map The font variation map
   */
  void SetVariationsMap(const Property::Map& map);

  /**
   * @brief Retrieves font variation values as variation axes.
   * @return The font variation axes.
   */
  Dali::Vector<Text::FontVariation::Axis> GetVariations() const;

  /**
   * @brief Sets font variation values from variation axes.
   * @param[in] axes The font variation axes.
   */
  void SetVariations(const Dali::Vector<Text::FontVariation::Axis>& axes);

  /**
   * @brief Clears variation values from the model
   */
  void ClearVariationsMap();

  /**
   * @brief Sets SetLayoutDirectionMode value to model
   * @param[in] match The value of LayoutDirectionMode for the text
   */
  void SetLayoutDirectionMode(LayoutDirectionMode type);

  /**
   * @brief Retrieves LayoutDirectionMode value from model
   * @return The value of LayoutDirectionMode
   */
  LayoutDirectionMode GetLayoutDirectionMode() const;

  /**
   * @brief Sets layoutDirection type value.
   * @param[in] layoutDirection The value of the layout direction type.
   */
  void SetLayoutDirection(Dali::LayoutDirection::Type layoutDirection);

  /**
   * @brief Gets layoutDirection type value.
   * @param[in] actor The actor which will get the layout direction type.
   * @return The value of the layout direction type.
   */
  Dali::LayoutDirection::Type GetLayoutDirection(Dali::Actor& actor) const;

  /**
   * @brief Get the rendered size of a specific text range.
   * if the requested text is at multilines, multiple sizes will be returned for each text located in a separate line.
   * if a line contains characters with different directions, multiple sizes will be returned for each block of
   * contiguous characters with the same direction.
   *
   * @param[in] startIndex start index of the text requested to calculate size for.
   * @param[in] endIndex end index(included) of the text requested to calculate size for.
   * @return list of sizes of the reuested text.
   */
  Vector<Vector2> GetTextSize(CharacterIndex startIndex, CharacterIndex endIndex);

  /**
   * @brief Get the top/left rendered position of a specific text range.
   * if the requested text is at multilines, multiple positions will be returned for each text located in a separate
   * line. if a line contains characters with different directions, multiple positions will be returned for each block
   * of contiguous characters with the same direction.
   *
   * @param[in] startIndex start index of the text requested to get position to.
   * @param[in] endIndex end index(included) of the text requested to get position to.
   * @return list of positions of the requested text.
   */
  Vector<Vector2> GetTextPosition(CharacterIndex startIndex, CharacterIndex endIndex);

  /**
   * @brief Get the line bounding rectangle.
   * if the requested index is out of range or the line is not yet rendered, a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] lineIndex line index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
  Bounds GetLineBoundingRectangle(const uint32_t lineIndex);

  /**
   * @brief Get the char bounding rectangle.
   * If the text is not yet rendered or the index > text.Count(); a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] charIndex character index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
  Bounds GetCharacterBoundingRectangle(const uint32_t charIndex);

  /**
   * @brief Get the character index.
   * If the text is not yet rendered or the text is empty, -1 is returned.
   *
   * @param[in] visualX visual x position.
   * @param[in] visualY visual y position.
   * @return character index.
   */
  int GetCharacterIndexAtPosition(float visualX, float visualY);

  /**
   * @brief Gets the bounding box of a specific text range.
   *
   * @param[in] startIndex start index of the text requested to get bounding box to.
   * @param[in] endIndex end index(included) of the text requested to get bounding box to.
   * @return bounding box of the requested text.
   */
  Bounds GetTextBoundingRectangle(CharacterIndex startIndex, CharacterIndex endIndex);

  /**
   * @brief Sets the layout direction changed.
   */
  void ChangedLayoutDirection();

  /**
   * @brief Invalidates cached font-related data and requests relayout.
   */
  void InvalidateFontData();

  /**
   * @brief Retrieves if showing placeholder text or not.
   * @return The value of showing placeholder text.
   */
  bool IsShowingPlaceholderText() const;

  /**
   * @brief Retrieves if showing real text or not.
   * @return The value of showing real text.
   */
  bool IsShowingRealText() const;

  /**
   * @brief Sets whether text is rendered asynchronously.
   *
   * @param[in] asyncRendering True to render text asynchronously, false to render it synchronously.
   */
  void SetAsyncRendering(bool asyncRendering);

  /**
   * @brief Gets whether text is rendered asynchronously.
   *
   * @return True if text is rendered asynchronously, otherwise false.
   */
  bool IsAsyncRendering() const;

public: // Relayout.
  /**
   * @brief Triggers a relayout which updates View (if necessary).
   *
   * @note UI Controls are expected to minimize calls to this method e.g. call once after size negotiation.
   * @param[in] size A the size of a bounding box to layout text within.
   * @param[in] layoutDirection The direction of the system language.
   *
   * @return Whether the text model or decorations were updated.
   */
  UpdateTextType Relayout(const Size&                 size,
                          Dali::LayoutDirection::Type layoutDirection = Dali::LayoutDirection::LEFT_TO_RIGHT);

  /**
   * @brief Request a relayout using the Ui::Integration::Text::ControlInterface.
   */
  void RequestRelayout();

  /**
   * @brief Invalidate the measured size using the Ui::Integration::Text::ControlInterface.
   */
  void InvalidateMeasure();

  /**
   * @brief Requests asynchronous text rendering.
   */
  void RequestAsyncRender();

public: // Input style change signals.
  /**
   * @return Whether the queue of input style changed signals is empty.
   */
  bool IsInputStyleChangedSignalsQueueEmpty();

  /**
   * @brief Request process all pending input style changed signals.
   *
   * Request to calls the Ui::Integration::Text::ControlInterface::InputStyleChanged() method which is overridden by the
   * text controls. Text controls may send signals to state the input style has changed.
   *
   * The signal will be execute next idle time, or skip if we fail to add idler.
   */
  void RequestProcessInputStyleChangedSignals();

private:
  /**
   * @brief Callbacks called on idle.
   *
   * If there are notifications of change of input style on the queue, Ui::TextField::InputStyleChangedSignal() are
   * emitted.
   */
  void OnIdleSignal();

public: // Text-input Event Queuing.
  /**
   * @brief Called by editable UI controls when keyboard focus is gained.
   *
   * @param[in] scrollToCursor Whether to scroll the current cursor position into view.
   *                           Should be false when focus is gained by touch/tap to prevent
   *                           scroll before tap hit-test is processed.
   */
  void KeyboardFocusGainEvent(bool scrollToCursor = true);

  /**
   * @brief Called by editable UI controls when focus is lost.
   */
  void KeyboardFocusLostEvent();

  /**
   * @brief Called by editable UI controls when key events are received.
   *
   * @param[in] event The key event.
   * @param[in] type Used to distinguish between regular key events and InputMethodContext events.
   */
  bool KeyEvent(const Dali::KeyEvent& event);

  /**
   * @brief Called from AnchorEvent or TextAnchor's OnAccessibilityActivate.
   * @param[in] cursorPosition Checks if an anchor exists at the given cursor position.
   * @param[out] href If an anchor exists at the given cursor position, the href is written.
   * @return True if an anchor exists at the given cursor position, false otherwise.
   */
  bool AnchorClickEvent(uint32_t cursorPosition, std::string& href);

  /**
   * @brief Called by anchor when a tap gesture occurs.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   */
  void AnchorEvent(float x, float y);

  /**
   * @brief Called by editable UI controls when a tap gesture occurs.
   * @param[in] tapCount The number of taps.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   */
  void TapEvent(unsigned int tapCount, float x, float y);

  /**
   * @brief Called by editable UI controls when a pan gesture occurs.
   *
   * @param[in] state The state of the gesture.
   * @param[in] displacement This distance panned since the last pan gesture.
   */
  void PanEvent(GestureState state, const Vector2& displacement);

  /**
   * @brief Called by editable UI controls when a long press gesture occurs.
   *
   * @param[in] state The state of the gesture.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   */
  void LongPressEvent(GestureState state, float x, float y);

  /**
   * @brief Used to get the Primary cursor position.
   *
   * @return Primary cursor position.
   */
  CharacterIndex GetPrimaryCursorPosition() const;

  /**
   * @brief Used to set the Primary cursor position.
   *
   * @param[in] index for the Primary cursor position.
   * @param[in] focused true if UI control has gained focus to receive key event, false otherwise.
   * @return[in] true if cursor position changed, false otherwise.
   */
  bool SetPrimaryCursorPosition(CharacterIndex index, bool focused);

  /**
   * @brief Creates a selection event.
   *
   * It could be called from the TapEvent (double tap) or when the text selection popup's sellect all button is pressed.
   *
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   * @param[in] selection type like the whole text is selected or unselected.
   */
  void SelectEvent(float x, float y, SelectionType selection);

  /**
   * @copydoc Ui::Integration::Text::SelectableControlInterface::SetTextSelectionRange()
   */
  void SetTextSelectionRange(const uint32_t* start, const uint32_t* end);

  /**
   * @copydoc Ui::Integration::Text::SelectableControlInterface::GetTextSelectionRange()
   */
  Ui::Integration::Text::Uint32Pair GetTextSelectionRange() const;

  /**
   * @copydoc Ui::Integration::Text::SelectableControlInterface::SelectWholeText()
   */
  void SelectWholeText();

  /**
   * @copydoc Ui::Integration::Text::EditableControlInterface::CopyText()
   */
  std::string CopyText();

  /**
   * @copydoc Ui::Integration::Text::EditableControlInterface::CutText()
   */
  std::string CutText();

  /**
   * @copydoc Ui::Integration::Text::EditableControlInterface::PasteText()
   */
  void PasteText();

  /**
   * @copydoc Ui::Integration::Text::SelectableControlInterface::SelectNone()
   */
  void SelectNone();

  /**
   * @copydoc Ui::Integration::Text::SelectableControlInterface::SelectText()
   */
  void SelectText(const uint32_t start, const uint32_t end);

  /**
   * @copydoc Ui::Integration::Text::SelectableControlInterface::GetSelectedText()
   */
  std::string GetSelectedText() const;

  /**
   * @copydoc Ui::Integration::Text::EditableControlInterface::IsEditable()
   */
  virtual bool IsEditable() const;

  /**
   * @copydoc Ui::Integration::Text::EditableControlInterface::SetEditable()
   */
  virtual void SetEditable(bool editable);

  /**
   * @copydoc Dali::Ui::Internal::TextEditor::ScrollBy()
   */
  virtual void ScrollBy(Vector2 scroll);

  /**
   * @brief Whether the text is scrollable.
   * @return Returns true if the text is scrollable.
   */
  bool IsScrollable(const Vector2& displacement);

  /**
   * @copydoc Dali::Ui::Internal::TextEditor::GetHorizontalScrollPosition()
   */
  float GetHorizontalScrollPosition();

  /**
   * @copydoc Dali::Ui::Internal::TextEditor::GetVerticalScrollPosition()
   */
  float GetVerticalScrollPosition();

  /**
   * @brief Event received from input method context
   *
   * @param[in] inputMethodContext The input method context.
   * @param[in] inputMethodContextEvent The event received.
   * @return A data struture indicating if update is needed, cursor position and current text.
   */
  Dali::Integration::InputMethodContext::CallbackData OnInputMethodContextEvent(
    InputMethodContext&                                     inputMethodContext,
    const Dali::Integration::InputMethodContext::EventData& inputMethodContextEvent);

  /**
   * @brief Event from Clipboard notifying an Item has been selected for pasting
   *
   * @param[in] id The id of the data request.
   * @param[in] mimeType The mime type of data received.
   * @param[in] data The data received.
   * @note
   * This event is executed by receiving the integration Clipboard DataReceivedSignal.
   */
  void PasteClipboardItemEvent(uint32_t id, const char* mimeType, const char* data);

  /**
   * @brief Return true when text control should clear key input focus when escape key is pressed.
   *
   * @return Whether text control should clear key input focus or not when escape key is pressed.
   */
  bool ShouldClearFocusOnEscape() const;

  /**
   * @brief Create an actor that renders the text background color
   *
   * @return the created actor or an empty handle if no background color needs to be rendered.
   */
  Actor CreateBackgroundActor();

  /**
   * @brief Used to reset the cursor position after setting a new text.
   *
   * @param[in] cursorIndex Where to place the cursor.
   */
  void ResetCursorPosition(CharacterIndex cursorIndex);

  /**
   * @brief The method acquires current position of cursor
   * @return unsigned value with cursor position
   */
  CharacterIndex GetCursorPosition();

  /**
   * @brief Resets a provided vector with actors that marks the position of anchors in markup enabled text
   *
   * @param[out] anchorActors the vector of actor (empty collection if no anchors available).
   */
  void GetAnchorActors(std::vector<Ui::TextAnchor>& anchorActors);

  /**
   * @brief Return an index of first anchor in the anchor vector whose boundaries includes given character offset
   *
   * @param[in] characterOffset A position in text coords.
   *
   * @return the index in anchor vector (-1 if an anchor not found)
   */
  int GetAnchorIndex(size_t characterOffset);

protected: // Inherit from Text::Decorator::ControllerInterface.
  /**
   * @copydoc Dali::Ui::Text::Decorator::ControllerInterface::GetTargetSize()
   */
  void GetTargetSize(Vector2& targetSize) override;

  /**
   * @copydoc Dali::Ui::Text::Decorator::ControllerInterface::AddDecoration()
   */
  void AddDecoration(Actor& actor, Ui::Integration::Text::DecorationType type, bool needsClipping) override;

  /**
   * @copydoc Dali::Ui::Text::Decorator::ControllerInterface::DecorationEvent()
   */
  void DecorationEvent(HandleType handle, HandleState state, float x, float y) override;

protected: // Inherit from TextSelectionPopup::TextPopupButtonCallbackInterface.
  /**
   * @copydoc Dali::Ui::TextSelectionPopup::TextPopupButtonCallbackInterface::TextPopupButtonTouched()
   */
  void TextPopupButtonTouched(Dali::Ui::Text::InputCommandType button) override;

protected: // Inherit from HiddenText.
  /**
   * @brief Invoked from HiddenText when showing time of the last character was expired
   */
  void DisplayTimeExpired() override;

protected: // Inherit from Dali::Integration::Processor
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcess) override;

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName()
   */
  std::string_view GetProcessorName() const override
  {
    return "Text::Controller";
  }

private: // Private contructors & copy operator.
  /**
   * @brief Private constructor.
   */
  Controller()
  : Controller(nullptr, nullptr, nullptr, nullptr)
  {
  }

  /**
   * @brief Private constructor.
   */
  Controller(Ui::Integration::Text::ControlInterface* controlInterface)
  : Controller(controlInterface, nullptr, nullptr, nullptr)
  {
  }

  /**
   * @brief Private constructor.
   */
  Controller(Ui::Integration::Text::ControlInterface*           controlInterface,
             Ui::Integration::Text::EditableControlInterface*   editableControlInterface,
             Ui::Integration::Text::SelectableControlInterface* selectableControlInterface,
             Ui::Integration::Text::AnchorControlInterface*     anchorControlInterface);

  Controller(const Controller& handle)            = delete;
  Controller& operator=(const Controller& handle) = delete;

protected: // Destructor.
  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Controller();

public:
  struct Impl; ///< Made public for testing purposes

private:
  struct EventHandler;
  struct InputFontHandler;
  struct InputProperties;
  struct PlaceholderHandler;
  struct Relayouter;
  struct TextUpdater;

  std::unique_ptr<Impl> mImpl{nullptr};
};

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_CONTROLLER_H
