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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class MarkdownViewImpl;
}

/**
 * @brief A view for displaying Markdown content.
 */
class DALI_UI_API MarkdownView : public View
{
public:
  MarkdownView();
  ~MarkdownView();

  static MarkdownView New();
  static MarkdownView DownCast(BaseHandle handle);

  /**
   * @brief Converts Markdown source to plain text.
   *
   * @param[in] markdown The Markdown source.
   * @return The converted plain text.
   */
  static Dali::String ToPlainText(const Dali::String& markdown);

  MarkdownView(const MarkdownView& handle);
  MarkdownView(MarkdownView&& rhs) noexcept;
  MarkdownView& operator=(const MarkdownView& handle);
  MarkdownView& operator=(MarkdownView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(MarkdownView)

  /**
   * @brief Sets the Markdown source to display.
   *
   * @param[in] markdown The Markdown source.
   */
  void SetMarkdown(const Dali::String& markdown);

  /**
   * @brief Gets the current Markdown source.
   *
   * @return The original Markdown source currently assigned to the view.
   */
  Dali::String GetMarkdown() const;

  /**
   * @brief Clears the Markdown content.
   */
  void Clear();

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL MarkdownView(Internal::MarkdownViewImpl& implementation);
  explicit DALI_INTERNAL MarkdownView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
