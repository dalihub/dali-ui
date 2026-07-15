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

#pragma once

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/view.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/markdown/markdown-render-model.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Defines the internal interface for a rendered Markdown component.
 */
class MarkdownComponent
{
public:
  /**
   * @brief Destructor.
   */
  virtual ~MarkdownComponent() = default;

  /**
   * @brief Returns the root view owned by this component.
   *
   * @return The root view.
   */
  virtual Ui::View GetRootView() const = 0;

  /**
   * @brief Returns the view that receives child Markdown components.
   *
   * @return The content host view.
   */
  virtual Ui::View GetContentHost() const = 0;

  /**
   * @brief Updates the component from the current render node.
   *
   * @param[in] previous The previously applied node, or @c nullptr.
   * @param[in] current The node to apply.
   * @param[in] textUpdate The classified text update.
   */
  virtual void Update(const MarkdownRenderNode* previous,
                      const MarkdownRenderNode& current,
                      const MarkdownTextUpdate& textUpdate) = 0;
};

/**
 * @brief Creates a component for the specified render node.
 *
 * @param[in] node The render node.
 * @return The created component.
 */
std::unique_ptr<MarkdownComponent> CreateMarkdownComponent(const MarkdownRenderNode& node);

} // namespace Internal
} // namespace Ui
} // namespace Dali
