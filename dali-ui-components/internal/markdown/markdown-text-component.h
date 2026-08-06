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
#include <dali-ui-foundation/public-api/views/view.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/markdown/markdown-render-model.h>
#include <dali-ui-components/public-api/styles/markdown-view-style.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Defines the internal interface used to render Markdown text nodes.
 */
class MarkdownTextComponent
{
public:
  /**
   * @brief Destructor.
   */
  virtual ~MarkdownTextComponent() = default;

  /**
   * @brief Returns the view that displays the text content.
   *
   * @return The text view.
   */
  virtual Ui::View GetView() const = 0;

  /**
   * @brief Applies the complete text content of a render node.
   *
   * @param[in] node The render node to apply.
   */
  virtual void SetTextContent(const MarkdownRenderNode& node) = 0;

  /**
   * @brief Updates the rendered text using the classified text change.
   *
   * @param[in] node The current render node.
   * @param[in] update The classified text update.
   */
  virtual void UpdateTextContent(const MarkdownRenderNode& node, const MarkdownTextUpdate& update) = 0;
};

/**
 * @brief Creates a label-backed text component.
 *
 * @param[in] style The Markdown style.
 * @return The created text component.
 */
std::unique_ptr<MarkdownTextComponent> CreateMarkdownLabelTextComponent(const MarkdownViewStyle& style);

} // namespace Internal
} // namespace Ui
} // namespace Dali
