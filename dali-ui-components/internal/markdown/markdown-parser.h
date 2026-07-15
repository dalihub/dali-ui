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
#include <dali-ui-components/internal/markdown/markdown-render-model.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Parses Markdown source into the internal render model.
 */
class MarkdownParser
{
public:
  /**
   * @brief Defines options used while parsing Markdown source.
   */
  struct Options
  {
    unsigned flags{0u};
    bool     plainTextMode{false};

    /**
     * @brief Creates the default options used for Markdown rendering.
     *
     * @return The default parser options.
     */
    static Options Default();

    /**
     * @brief Creates parser options used for plain-text conversion.
     *
     * @return The plain-text parser options.
     */
    static Options PlainText();
  };

  /**
   * @brief Parses Markdown source into a render snapshot.
   *
   * @param[in] markdown The Markdown source to parse.
   * @param[in] revision The revision assigned to the generated snapshot.
   * @param[in] options The parser options.
   * @return The generated render snapshot.
   */
  MarkdownRenderSnapshot Parse(const Dali::String& markdown, uint64_t revision, const Options& options = Options::Default()) const;
};

/**
 * @brief Converts a parsed Markdown snapshot to plain text.
 *
 * @param[in] snapshot The snapshot to convert.
 * @return The converted plain text.
 */
Dali::String MarkdownSnapshotToPlainText(const MarkdownRenderSnapshot& snapshot);

} // namespace Internal
} // namespace Ui
} // namespace Dali
