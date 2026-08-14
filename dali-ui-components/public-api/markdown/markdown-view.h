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

class MarkdownViewStyle;

/**
 * @brief A view for displaying Markdown content.
 *
 * MarkdownView is optimized for a resolved horizontal extent and vertically
 * growing content. A fixed width or @c MATCH_PARENT width together with a
 * @c WRAP_CONTENT height is recommended, especially when MarkdownView is used
 * as the content of a vertically scrolling ScrollView.
 *
 * Width measurement behaves as follows:
 * - A fixed width uses the requested width, subject to size constraints.
 * - @c MATCH_PARENT lets the parent assign the final width during arrangement.
 * - @c WRAP_CONTENT uses the full available width when measurement receives a
 *   bounded width constraint, making it similar to @c MATCH_PARENT in that case.
 *   With an unbounded width constraint, it retains content-based measurement
 *   and does not expand to the unbounded value.
 *
 * For predictable text wrapping in an unbounded-width container, specify a
 * maximum width or place MarkdownView in a container with a bounded width.
 */
class DALI_UI_COMPONENTS_API MarkdownView : public View
{
public:
  MarkdownView();
  ~MarkdownView();

  /**
   * @brief Creates a MarkdownView with the default style.
   *
   * @return The created MarkdownView.
   */
  static MarkdownView New();

  /**
   * @brief Creates a MarkdownView with the specified style.
   *
   * @param[in] style The style to use.
   * @return The created MarkdownView.
   */
  static MarkdownView New(const MarkdownViewStyle& style);
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
   * @return The Markdown source.
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
