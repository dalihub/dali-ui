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

#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <memory>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/markdown/markdown-component.h>
#include <dali-ui-components/internal/markdown/markdown-parser.h>
#include <dali-ui-components/public-api/markdown/markdown-view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Implements Markdown parsing, reconciliation, and rendering for MarkdownView.
 */
class MarkdownViewImpl : public ViewImpl
{
public:
  /**
   * @brief Creates a new MarkdownView handle backed by this implementation.
   *
   * @return The created MarkdownView.
   */
  static Ui::MarkdownView New();

  /**
   * @brief Replaces the Markdown source rendered by this view.
   *
   * @param[in] markdown The complete Markdown source.
   */
  void SetMarkdown(const Dali::String& markdown);

  /**
   * @brief Returns the current Markdown source.
   *
   * @return The current Markdown source.
   */
  Dali::String GetMarkdown() const;

  /**
   * @brief Clears the current Markdown source and rendered content.
   */
  void Clear();

  /**
   * @brief Returns the most recently parsed render snapshot.
   *
   * @return The current render snapshot.
   */
  const MarkdownRenderSnapshot& GetSnapshot() const
  {
    return mSnapshot;
  }

protected:
  MarkdownViewImpl();
  ~MarkdownViewImpl() override;

  void OnInitialize() override;

private:
  /**
   * @brief Stores a materialized component and its reconciled child subtree.
   */
  struct ComponentNode
  {
    MarkdownRenderRole                          role{MarkdownRenderRole::PARAGRAPH};
    uint32_t                                    snapshotIndex{MARKDOWN_INVALID_NODE_INDEX};
    uint64_t                                    subtreeHash{0u};
    std::unique_ptr<MarkdownComponent>          component;
    std::vector<std::unique_ptr<ComponentNode>> children;
  };

  /**
   * @brief Applies a parsed snapshot to the rendered view hierarchy.
   *
   * @param[in] snapshot The snapshot to apply.
   */
  void ApplySnapshot(MarkdownRenderSnapshot&& snapshot);

  /**
   * @brief Builds the child-node lookup for a render snapshot.
   *
   * @param[in] snapshot The snapshot to index.
   */
  void BuildChildrenByParent(const MarkdownRenderSnapshot& snapshot);

  /**
   * @brief Reconciles rendered children with the next snapshot children.
   *
   * @param[in] parent The parent view that owns the child components.
   * @param[in,out] currentChildren The currently rendered component children.
   * @param[in] nextChildIndices The next child node indices.
   * @param[in] nextSnapshot The snapshot being applied.
   */
  void ReconcileChildren(Ui::View                                     parent,
                         std::vector<std::unique_ptr<ComponentNode>>& currentChildren,
                         const std::vector<uint32_t>&                 nextChildIndices,
                         const MarkdownRenderSnapshot&                nextSnapshot);

  /**
   * @brief Creates a rendered component subtree for a snapshot node.
   *
   * @param[in] snapshot The snapshot that owns the node.
   * @param[in] nodeIndex The node index to materialize.
   * @return The created component subtree.
   */
  std::unique_ptr<ComponentNode> CreateComponentSubtree(const MarkdownRenderSnapshot& snapshot, uint32_t nodeIndex);

  /**
   * @brief Removes a rendered component subtree from a parent view.
   *
   * @param[in] parent The parent view.
   * @param[in,out] node The component subtree to remove.
   */
  void RemoveComponentSubtree(Ui::View parent, std::unique_ptr<ComponentNode>& node);

  /**
   * @brief Checks whether a previous component can be reused for a next node.
   *
   * @param[in] previousNode The previously rendered component node.
   * @param[in] nextNode The next render node.
   * @param[in] siblingCountChanged Whether the sibling count changed.
   * @return @c true if the previous component can be reused.
   */
  bool CanReuseAtOrdinal(const ComponentNode&      previousNode,
                         const MarkdownRenderNode& nextNode,
                         bool                      siblingCountChanged) const;

  /**
   * @brief Returns the previous render node for a component node.
   *
   * @param[in] node The component node.
   * @return The previous render node, or @c nullptr.
   */
  const MarkdownRenderNode* GetPreviousNode(const ComponentNode& node) const;

  /**
   * @brief Classifies the text change between two render nodes.
   *
   * @param[in] previous The previous render node.
   * @param[in] next The next render node.
   * @return The classified text update.
   */
  MarkdownTextUpdate CalculateTextUpdate(const MarkdownRenderNode& previous, const MarkdownRenderNode& next) const;

  /**
   * @brief Returns the public view handle for this implementation.
   *
   * @return The MarkdownView as a View.
   */
  Ui::View GetSelfView() const;

private:
  Dali::String mMarkdown;
  uint64_t     mRevision{0u};

  MarkdownParser         mParser;
  MarkdownRenderSnapshot mSnapshot;

  std::vector<std::unique_ptr<ComponentNode>> mComponentChildren;
  std::vector<std::vector<uint32_t>>          mChildrenByParent;
};

/**
 * @brief Gets the internal implementation from a MarkdownView handle.
 *
 * @param[in] markdownView The MarkdownView handle.
 * @return The internal implementation.
 */
inline MarkdownViewImpl& GetImpl(Ui::MarkdownView& markdownView)
{
  DALI_ASSERT_ALWAYS(markdownView);
  Dali::RefObject& handle = markdownView.GetImplementation();
  return static_cast<MarkdownViewImpl&>(handle);
}

/**
 * @brief Gets the internal implementation from a MarkdownView handle.
 *
 * @param[in] markdownView The MarkdownView handle.
 * @return The internal implementation.
 */
inline const MarkdownViewImpl& GetImpl(const Ui::MarkdownView& markdownView)
{
  DALI_ASSERT_ALWAYS(markdownView);
  const Dali::RefObject& handle = markdownView.GetImplementation();
  return static_cast<const MarkdownViewImpl&>(handle);
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
