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
#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class MarkdownViewStyleImpl;
}

/**
 * @brief Defines the style used by MarkdownView.
 */
class DALI_UI_COMPONENTS_API MarkdownViewStyle : public UiStyle
{
public:
  class Builder;

  MarkdownViewStyle() = default;

  static UiStyleKey<MarkdownViewStyle> DefaultKey();

  /**
   * @brief Gets the built-in default style.
   *
   * @return The built-in default style.
   */
  static MarkdownViewStyle DefaultPreset();

  /**
   * @brief Gets the default MarkdownView style.
   *
   * @return The default MarkdownView style.
   */
  static MarkdownViewStyle Default();

  static MarkdownViewStyle DownCast(BaseHandle handle);
  static MarkdownViewStyle StaticDownCast(UiStyle style);

  Builder Configure() const;

  Dali::String GetTextFontFamily() const;
  Dali::String GetHeadingFontFamily() const;
  Dali::String GetCodeFontFamily() const;

  float GetTextFontSize() const;
  float GetHeading1FontSize() const;
  float GetHeading2FontSize() const;
  float GetHeading3FontSize() const;
  float GetHeading4FontSize() const;
  float GetHeading5FontSize() const;
  float GetHeading6FontSize() const;
  float GetCodeBlockFontSize() const;
  float GetCodeBlockTitleFontSize() const;

  UiColor GetTextColor() const;
  UiColor GetHeadingTextColor() const;
  UiColor GetQuoteTextColor() const;
  UiColor GetCodeTextColor() const;
  UiColor GetCodeBlockTitleTextColor() const;
  UiColor GetInlineCodeBackgroundColor() const;
  UiColor GetCodeBlockBackgroundColor() const;
  UiColor GetCodeBlockTitleBackgroundColor() const;
  UiColor GetQuoteBarColor() const;
  UiColor GetThematicBreakColor() const;
  UiColor GetTableRuleColor() const;

  SelectableImageInterface CreateTaskCheckBoxIcon() const;
  UiColor                  GetTaskCheckBoxIconColor() const;
  UiColor                  GetTaskCheckBoxSelectedIconColor() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL MarkdownViewStyle(Internal::MarkdownViewStyleImpl* impl);
  /// @endcond
};

/**
 * @brief Builds a MarkdownViewStyle.
 */
class DALI_UI_COMPONENTS_API MarkdownViewStyle::Builder
{
public:
  Builder();
  Builder(Builder&& rhs) noexcept;
  Builder& operator=(Builder&& rhs) noexcept;

  Builder(const Builder&)            = delete;
  Builder& operator=(const Builder&) = delete;

  ~Builder();

  Builder&  SetTextFontFamily(const Dali::String& value) &;
  Builder&& SetTextFontFamily(const Dali::String& value) &&;
  Builder&  SetHeadingFontFamily(const Dali::String& value) &;
  Builder&& SetHeadingFontFamily(const Dali::String& value) &&;
  Builder&  SetCodeFontFamily(const Dali::String& value) &;
  Builder&& SetCodeFontFamily(const Dali::String& value) &&;

  Builder&  SetTextFontSize(float value) &;
  Builder&& SetTextFontSize(float value) &&;
  Builder&  SetHeading1FontSize(float value) &;
  Builder&& SetHeading1FontSize(float value) &&;
  Builder&  SetHeading2FontSize(float value) &;
  Builder&& SetHeading2FontSize(float value) &&;
  Builder&  SetHeading3FontSize(float value) &;
  Builder&& SetHeading3FontSize(float value) &&;
  Builder&  SetHeading4FontSize(float value) &;
  Builder&& SetHeading4FontSize(float value) &&;
  Builder&  SetHeading5FontSize(float value) &;
  Builder&& SetHeading5FontSize(float value) &&;
  Builder&  SetHeading6FontSize(float value) &;
  Builder&& SetHeading6FontSize(float value) &&;
  Builder&  SetCodeBlockFontSize(float value) &;
  Builder&& SetCodeBlockFontSize(float value) &&;
  Builder&  SetCodeBlockTitleFontSize(float value) &;
  Builder&& SetCodeBlockTitleFontSize(float value) &&;

  Builder&  SetTextColor(const UiColor& value) &;
  Builder&& SetTextColor(const UiColor& value) &&;
  Builder&  SetHeadingTextColor(const UiColor& value) &;
  Builder&& SetHeadingTextColor(const UiColor& value) &&;
  Builder&  SetQuoteTextColor(const UiColor& value) &;
  Builder&& SetQuoteTextColor(const UiColor& value) &&;
  Builder&  SetCodeTextColor(const UiColor& value) &;
  Builder&& SetCodeTextColor(const UiColor& value) &&;
  Builder&  SetCodeBlockTitleTextColor(const UiColor& value) &;
  Builder&& SetCodeBlockTitleTextColor(const UiColor& value) &&;
  Builder&  SetInlineCodeBackgroundColor(const UiColor& value) &;
  Builder&& SetInlineCodeBackgroundColor(const UiColor& value) &&;
  Builder&  SetCodeBlockBackgroundColor(const UiColor& value) &;
  Builder&& SetCodeBlockBackgroundColor(const UiColor& value) &&;
  Builder&  SetCodeBlockTitleBackgroundColor(const UiColor& value) &;
  Builder&& SetCodeBlockTitleBackgroundColor(const UiColor& value) &&;
  Builder&  SetQuoteBarColor(const UiColor& value) &;
  Builder&& SetQuoteBarColor(const UiColor& value) &&;
  Builder&  SetThematicBreakColor(const UiColor& value) &;
  Builder&& SetThematicBreakColor(const UiColor& value) &&;
  Builder&  SetTableRuleColor(const UiColor& value) &;
  Builder&& SetTableRuleColor(const UiColor& value) &&;

  Builder& SetTaskCheckBoxIconGenerator(
    Ui::Callback<SelectableImageInterface()>&& generator) &;
  Builder&& SetTaskCheckBoxIconGenerator(
    Ui::Callback<SelectableImageInterface()>&& generator) &&;

  Builder&  SetTaskCheckBoxIconColor(const UiColor& color) &;
  Builder&& SetTaskCheckBoxIconColor(const UiColor& color) &&;
  Builder&  SetTaskCheckBoxSelectedIconColor(const UiColor& color) &;
  Builder&& SetTaskCheckBoxSelectedIconColor(const UiColor& color) &&;

  MarkdownViewStyle Build() &&;

private:
  explicit Builder(Internal::MarkdownViewStyleImpl* impl);

  friend class MarkdownViewStyle;

private:
  IntrusivePtr<Internal::MarkdownViewStyleImpl> mImpl;
};

} // namespace Ui
} // namespace Dali
