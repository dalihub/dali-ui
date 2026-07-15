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

// CLASS HEADER
#include <dali-ui-components/internal/markdown/markdown-view-impl.h>

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/stack-layout-manager.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/common/unique-ptr.h>
#include <algorithm>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{

constexpr float COMMON_PADDING      = 10.0f;
constexpr float COMMON_ITEM_PADDING = 20.0f;

BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(MarkdownViewImpl, ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

bool HasPrefix(const std::string& value, const std::string& prefix)
{
  return value.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), value.begin());
}

uint32_t CommonUtf32PrefixLength(const std::string& lhs, const std::string& rhs)
{
  std::size_t byteLength = 0u;
  const auto  maxBytes   = std::min(lhs.size(), rhs.size());
  while(byteLength < maxBytes && lhs[byteLength] == rhs[byteLength])
  {
    ++byteLength;
  }
  while(byteLength > 0u && byteLength < lhs.size() && (static_cast<unsigned char>(lhs[byteLength]) & 0xC0u) == 0x80u)
  {
    --byteLength;
  }
  return MarkdownUtf8Length(lhs.substr(0u, byteLength));
}

template<typename Range, typename Start, typename Equal>
bool RangesEqualInPrefix(const std::vector<Range>& previous, const std::vector<Range>& next, uint32_t prefixLength, Start start, Equal equal)
{
  for(const auto& range : previous)
  {
    if(start(range) < prefixLength)
    {
      const auto found = std::find_if(next.begin(), next.end(), [&range, &equal](const Range& candidate)
      {
        return equal(range, candidate);
      });
      if(found == next.end())
      {
        return false;
      }
    }
  }

  for(const auto& range : next)
  {
    if(start(range) < prefixLength)
    {
      const auto found = std::find_if(previous.begin(), previous.end(), [&range, &equal](const Range& candidate)
      {
        return equal(range, candidate);
      });
      if(found == previous.end())
      {
        return false;
      }
    }
  }

  return true;
}

bool SemanticPrefixEqual(const MarkdownRenderNode& previous, const MarkdownRenderNode& next, uint32_t prefixLength)
{
  const auto textRangeStart = [](const auto& range)
  {
    return range.start;
  };
  const auto objectRangeStart = [](const MarkdownInlineObject& object)
  {
    return object.position;
  };
  const auto sameStyle = [](const MarkdownTextStyleRun& lhs, const MarkdownTextStyleRun& rhs)
  {
    return lhs.start == rhs.start && lhs.end == rhs.end && lhs.flags == rhs.flags;
  };
  const auto sameLink = [](const MarkdownLinkRange& lhs, const MarkdownLinkRange& rhs)
  {
    return lhs.start == rhs.start &&
           lhs.end == rhs.end &&
           lhs.href == rhs.href &&
           lhs.title == rhs.title &&
           lhs.isAutolink == rhs.isAutolink;
  };
  const auto sameObject = [](const MarkdownInlineObject& lhs, const MarkdownInlineObject& rhs)
  {
    return lhs.type == rhs.type &&
           lhs.position == rhs.position &&
           lhs.length == rhs.length &&
           lhs.sourceUrl == rhs.sourceUrl &&
           lhs.title == rhs.title &&
           lhs.altText == rhs.altText;
  };

  return RangesEqualInPrefix(previous.styleRuns, next.styleRuns, prefixLength, textRangeStart, sameStyle) &&
         RangesEqualInPrefix(previous.linkRanges, next.linkRanges, prefixLength, textRangeStart, sameLink) &&
         RangesEqualInPrefix(previous.inlineObjects, next.inlineObjects, prefixLength, objectRangeStart, sameObject);
}

} // namespace

Ui::MarkdownView MarkdownViewImpl::New()
{
  IntrusivePtr<MarkdownViewImpl> impl(new MarkdownViewImpl());
  Ui::MarkdownView               handle(*impl);
  impl->Initialize();
  return handle;
}

MarkdownViewImpl::MarkdownViewImpl()
: ViewImpl()
{
}

MarkdownViewImpl::~MarkdownViewImpl() = default;

void MarkdownViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  AttachLayoutManager(Dali::MakeUnique<StackLayoutManager>(StackOrientation::VERTICAL, COMMON_ITEM_PADDING));
  GetSelfView().SetPadding(Extents(static_cast<int16_t>(COMMON_PADDING),
                                   static_cast<int16_t>(COMMON_PADDING),
                                   static_cast<int16_t>(COMMON_PADDING),
                                   static_cast<int16_t>(COMMON_PADDING)));
}

void MarkdownViewImpl::SetMarkdown(const Dali::String& markdown)
{
  if(mMarkdown == markdown)
  {
    return;
  }

  mMarkdown = markdown;

  auto snapshot = mParser.Parse(mMarkdown, ++mRevision);

  ApplySnapshot(std::move(snapshot));
}

Dali::String MarkdownViewImpl::GetMarkdown() const
{
  return mMarkdown;
}

void MarkdownViewImpl::Clear()
{
  Ui::View root = GetSelfView();
  for(auto& child : mComponentChildren)
  {
    RemoveComponentSubtree(root, child);
  }
  mComponentChildren.clear();

  mMarkdown.Clear();
  mSnapshot = MarkdownRenderSnapshot();
  ++mRevision;
  mChildrenByParent.clear();
}

void MarkdownViewImpl::ApplySnapshot(MarkdownRenderSnapshot&& snapshot)
{
  BuildChildrenByParent(snapshot);

  Ui::View root = GetSelfView();
  if(root && !snapshot.nodes.empty() && !mChildrenByParent.empty())
  {
    ReconcileChildren(root, mComponentChildren, mChildrenByParent[0u], snapshot);
  }
  else if(root)
  {
    for(auto& child : mComponentChildren)
    {
      RemoveComponentSubtree(root, child);
    }
    mComponentChildren.clear();
  }

  mSnapshot = std::move(snapshot);
}

void MarkdownViewImpl::BuildChildrenByParent(const MarkdownRenderSnapshot& snapshot)
{
  mChildrenByParent.clear();
  mChildrenByParent.resize(snapshot.nodes.size());

  for(const auto& node : snapshot.nodes)
  {
    if(!MarkdownCanMaterializeRole(node.role))
    {
      continue;
    }
    if(node.parentIndex < mChildrenByParent.size())
    {
      mChildrenByParent[node.parentIndex].push_back(node.index);
    }
  }
}

void MarkdownViewImpl::ReconcileChildren(Ui::View                                     parent,
                                         std::vector<std::unique_ptr<ComponentNode>>& currentChildren,
                                         const std::vector<uint32_t>&                 nextChildIndices,
                                         const MarkdownRenderSnapshot&                nextSnapshot)
{
  const bool siblingCountChanged = currentChildren.size() != nextChildIndices.size();
  const auto commonCount         = std::min(currentChildren.size(), nextChildIndices.size());

  std::size_t index = 0u;
  for(; index < commonCount; ++index)
  {
    const MarkdownRenderNode& nextNode = nextSnapshot.nodes[nextChildIndices[index]];
    if(!CanReuseAtOrdinal(*currentChildren[index], nextNode, siblingCountChanged))
    {
      break;
    }

    ComponentNode& componentNode = *currentChildren[index];
    const auto*    previousNode  = GetPreviousNode(componentNode);

    if(previousNode && componentNode.subtreeHash == nextNode.subtreeHash)
    {
      componentNode.snapshotIndex = nextNode.index;
      componentNode.subtreeHash   = nextNode.subtreeHash;
      continue;
    }

    MarkdownTextUpdate textUpdate;
    if(previousNode)
    {
      textUpdate = CalculateTextUpdate(*previousNode, nextNode);
    }

    componentNode.component->Update(previousNode, nextNode, textUpdate);
    componentNode.snapshotIndex = nextNode.index;
    componentNode.subtreeHash   = nextNode.subtreeHash;

    Ui::View contentHost = componentNode.component->GetContentHost();
    if(contentHost && nextNode.index < mChildrenByParent.size())
    {
      ReconcileChildren(contentHost, componentNode.children, mChildrenByParent[nextNode.index], nextSnapshot);
    }
  }

  for(std::size_t removeIndex = currentChildren.size(); removeIndex > index; --removeIndex)
  {
    RemoveComponentSubtree(parent, currentChildren[removeIndex - 1u]);
  }
  if(index < currentChildren.size())
  {
    currentChildren.erase(currentChildren.begin() + static_cast<std::ptrdiff_t>(index), currentChildren.end());
  }

  for(; index < nextChildIndices.size(); ++index)
  {
    std::unique_ptr<ComponentNode> child = CreateComponentSubtree(nextSnapshot, nextChildIndices[index]);
    if(child && child->component && child->component->GetRootView())
    {
      parent.Add(child->component->GetRootView());
      currentChildren.push_back(std::move(child));
    }
  }
}

std::unique_ptr<MarkdownViewImpl::ComponentNode> MarkdownViewImpl::CreateComponentSubtree(const MarkdownRenderSnapshot& snapshot, uint32_t nodeIndex)
{
  if(nodeIndex >= snapshot.nodes.size())
  {
    return nullptr;
  }

  const MarkdownRenderNode&      node = snapshot.nodes[nodeIndex];
  std::unique_ptr<ComponentNode> componentNode(new ComponentNode());
  componentNode->role          = node.role;
  componentNode->snapshotIndex = node.index;
  componentNode->subtreeHash   = node.subtreeHash;
  componentNode->component     = CreateMarkdownComponent(node);

  Ui::View contentHost = componentNode->component ? componentNode->component->GetContentHost() : Ui::View();
  if(contentHost && node.index < mChildrenByParent.size())
  {
    for(uint32_t childIndex : mChildrenByParent[node.index])
    {
      std::unique_ptr<ComponentNode> child = CreateComponentSubtree(snapshot, childIndex);
      if(child && child->component && child->component->GetRootView())
      {
        contentHost.Add(child->component->GetRootView());
        componentNode->children.push_back(std::move(child));
      }
    }
  }

  return componentNode;
}

void MarkdownViewImpl::RemoveComponentSubtree(Ui::View parent, std::unique_ptr<ComponentNode>& node)
{
  if(!node)
  {
    return;
  }

  if(parent && node->component && node->component->GetRootView())
  {
    parent.Remove(node->component->GetRootView(), RemovePolicy::IMMEDIATE);
  }
}

bool MarkdownViewImpl::CanReuseAtOrdinal(const ComponentNode&      previousNode,
                                         const MarkdownRenderNode& nextNode,
                                         bool                      siblingCountChanged) const
{
  if(previousNode.role != nextNode.role)
  {
    return false;
  }

  if(siblingCountChanged && previousNode.subtreeHash != nextNode.subtreeHash)
  {
    return false;
  }

  return true;
}

const MarkdownRenderNode* MarkdownViewImpl::GetPreviousNode(const ComponentNode& node) const
{
  if(node.snapshotIndex < mSnapshot.nodes.size())
  {
    return &mSnapshot.nodes[node.snapshotIndex];
  }
  return nullptr;
}

MarkdownTextUpdate MarkdownViewImpl::CalculateTextUpdate(const MarkdownRenderNode& previous, const MarkdownRenderNode& next) const
{
  MarkdownTextUpdate update;
  if(previous.role != next.role || previous.attributeHash != next.attributeHash)
  {
    update.type                  = MarkdownTextUpdate::Type::REPLACE_ALL;
    update.removedCharacterCount = previous.utf32Length;
    return update;
  }

  if(previous.contentHash == next.contentHash && previous.styleHash == next.styleHash)
  {
    update.type                  = MarkdownTextUpdate::Type::UNCHANGED;
    update.unchangedPrefixLength = previous.utf32Length;
    update.removedCharacterCount = 0u;
    return update;
  }

  if(HasPrefix(next.text, previous.text) && next.text.size() > previous.text.size() && SemanticPrefixEqual(previous, next, previous.utf32Length))
  {
    update.type                  = MarkdownTextUpdate::Type::APPEND;
    update.unchangedPrefixLength = previous.utf32Length;
    update.removedCharacterCount = 0u;
    return update;
  }

  const uint32_t commonPrefix = CommonUtf32PrefixLength(previous.text, next.text);
  if(commonPrefix > 0u && SemanticPrefixEqual(previous, next, commonPrefix))
  {
    update.type                  = MarkdownTextUpdate::Type::REPLACE_TAIL;
    update.unchangedPrefixLength = commonPrefix;
    update.removedCharacterCount = previous.utf32Length - commonPrefix;
    return update;
  }

  update.type                  = MarkdownTextUpdate::Type::REPLACE_ALL;
  update.removedCharacterCount = previous.utf32Length;
  return update;
}

Ui::View MarkdownViewImpl::GetSelfView() const
{
  return Ui::View::DownCast(Self());
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
