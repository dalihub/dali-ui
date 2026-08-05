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
#include <dali-ui-foundation/public-api/layouts/flex-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/flex-layout-params-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-dependency-scope.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
{

namespace
{

float GetFlexGrow(ViewImpl& childImpl)
{
  auto* params = Internal::FlexLayoutParamsImpl::Get(childImpl);
  return params ? params->GetFlexGrow() : 0.0f;
}

float GetFlexShrink(ViewImpl& childImpl)
{
  auto* params = Internal::FlexLayoutParamsImpl::Get(childImpl);
  return params ? params->GetFlexShrink() : 1.0f;
}

FlexAlign GetAlignSelf(ViewImpl& childImpl)
{
  auto* params = Internal::FlexLayoutParamsImpl::Get(childImpl);
  return params ? params->GetAlignSelf() : FlexAlign::AUTO;
}

float GetFlexBasis(ViewImpl& childImpl)
{
  auto* params = Internal::FlexLayoutParamsImpl::Get(childImpl);
  return params ? params->GetFlexBasis() : WRAP_CONTENT;
}

bool IsChildStandalone(ViewImpl& childImpl)
{
  return childImpl.GetLayoutMode() == LayoutMode::STANDALONE;
}

struct FlexLine
{
  std::vector<uint32_t> childIndices;
  float                 mainSize{0.0f};
  float                 crossSize{0.0f};
  float                 totalFlexGrow{0.0f};
  float                 totalFlexShrink{0.0f};
};

std::vector<FlexLine> BuildFlexLinesForArrange(std::vector<View>&         children,
                                               std::vector<MeasuredSize>& workingSizes,
                                               float                      availableMain,
                                               bool                       isMainAxisHorizontal,
                                               FlexWrap                   wrap)
{
  std::vector<FlexLine> lines;
  FlexLine              currentLine;
  for(uint32_t i = 0; i < children.size(); ++i)
  {
    ViewImpl& childImpl = GetImpl(children[i]);
    if(IsChildStandalone(childImpl))
    {
      continue;
    }
    float  childScale = childImpl.GetEffectiveScale();
    Insets margin     = childImpl.GetMargin();

    float basis = GetFlexBasis(childImpl);
    if(basis >= 0.0f)
    {
      if(isMainAxisHorizontal)
      {
        workingSizes[i].width = basis * childScale;
      }
      else
      {
        workingSizes[i].height = basis * childScale;
      }
    }

    float visMarginMainH = (margin.top + margin.bottom) * childScale;
    float visMarginMainW = (margin.start + margin.end) * childScale;
    float childMainSize  = isMainAxisHorizontal ? workingSizes[i].width + visMarginMainW
                                                : workingSizes[i].height + visMarginMainH;
    bool  shouldWrap     = (wrap != FlexWrap::NO_WRAP) && !currentLine.childIndices.empty() &&
                      (currentLine.mainSize + childMainSize > availableMain);
    if(shouldWrap)
    {
      lines.push_back(currentLine);
      currentLine = FlexLine();
    }
    currentLine.childIndices.push_back(i);
    currentLine.mainSize += childMainSize;
    currentLine.totalFlexGrow += GetFlexGrow(childImpl);
    currentLine.totalFlexShrink += GetFlexShrink(childImpl);
    float childCrossSize  = isMainAxisHorizontal ? workingSizes[i].height + visMarginMainH
                                                 : workingSizes[i].width + visMarginMainW;
    currentLine.crossSize = std::max(currentLine.crossSize, childCrossSize);
  }
  if(!currentLine.childIndices.empty())
  {
    lines.push_back(currentLine);
  }
  return lines;
}

void ApplyFlexGrowShrink(FlexLine& line, std::vector<View>& children,
                         std::vector<MeasuredSize>& workingSizes, float availableMain,
                         bool isMainAxisHorizontal)
{
  float freeSpace = availableMain - line.mainSize;

  if(freeSpace > 0.0f && line.totalFlexGrow > 0.0f)
  {
    for(uint32_t idx : line.childIndices)
    {
      ViewImpl& childImpl = GetImpl(children[idx]);
      float     grow      = GetFlexGrow(childImpl);
      if(grow > 0.0f)
      {
        float extra = (grow / line.totalFlexGrow) * freeSpace;
        if(isMainAxisHorizontal)
        {
          workingSizes[idx].width += extra;
        }
        else
        {
          workingSizes[idx].height += extra;
        }
      }
    }
    line.mainSize = availableMain;
  }
  else if(freeSpace < 0.0f && line.totalFlexShrink > 0.0f)
  {
    float totalWeightedShrink = 0.0f;
    for(uint32_t idx : line.childIndices)
    {
      ViewImpl& childImpl     = GetImpl(children[idx]);
      float     childMainSize = isMainAxisHorizontal ? workingSizes[idx].width : workingSizes[idx].height;
      float     shrink        = GetFlexShrink(childImpl);
      totalWeightedShrink += shrink * childMainSize;
    }
    if(totalWeightedShrink > 0.0f)
    {
      float overflow = -freeSpace;
      for(uint32_t idx : line.childIndices)
      {
        ViewImpl& childImpl     = GetImpl(children[idx]);
        float     childMainSize = isMainAxisHorizontal ? workingSizes[idx].width : workingSizes[idx].height;
        float     shrink        = GetFlexShrink(childImpl);
        float     reduction     = (shrink * childMainSize / totalWeightedShrink) * overflow;
        if(isMainAxisHorizontal)
        {
          workingSizes[idx].width = std::max(0.0f, workingSizes[idx].width - reduction);
        }
        else
        {
          workingSizes[idx].height = std::max(0.0f, workingSizes[idx].height - reduction);
        }
      }
      line.mainSize = availableMain;
    }
  }
}

struct FlexJustifyOffsets
{
  float mainOffset{0.0f};
  float spacing{0.0f};
};

FlexJustifyOffsets GetFlexJustifyOffsets(float freeSpace, FlexJustify justify, size_t lineChildCount)
{
  FlexJustifyOffsets out;
  switch(justify)
  {
    case FlexJustify::FLEX_START:
      break;
    case FlexJustify::FLEX_END:
      out.mainOffset = freeSpace;
      break;
    case FlexJustify::CENTER:
      out.mainOffset = freeSpace / 2.0f;
      break;
    // On overflow (freeSpace < 0) the distributed modes must not produce
    // negative gaps that overlap children: fall back to start packing
    // (SPACE_BETWEEN) or centring the overflow (SPACE_AROUND/EVENLY).
    case FlexJustify::SPACE_BETWEEN:
      if(freeSpace > 0.0f && lineChildCount > 1)
      {
        out.spacing = freeSpace / (lineChildCount - 1);
      }
      break;
    case FlexJustify::SPACE_AROUND:
      if(freeSpace > 0.0f)
      {
        out.spacing    = freeSpace / lineChildCount;
        out.mainOffset = out.spacing / 2.0f;
      }
      else
      {
        out.mainOffset = freeSpace / 2.0f;
      }
      break;
    case FlexJustify::SPACE_EVENLY:
      if(freeSpace > 0.0f)
      {
        out.spacing    = freeSpace / (lineChildCount + 1);
        out.mainOffset = out.spacing;
      }
      else
      {
        out.mainOffset = freeSpace / 2.0f;
      }
      break;
  }
  return out;
}

// @param[in] owner The arranging View, threaded through so the final MATCH_PARENT
//                  re-measure below can be attributed to it. nullptr makes the scope inert.
void ArrangeOneFlexLine(ViewImpl* owner, FlexLine& line, std::vector<View>& children,
                        std::vector<MeasuredSize>& workingSizes, const LayoutRect& bounds,
                        float contentWidth, float contentHeight, float& crossOffsetInOut, float& mainOffsetInOut,
                        float spacing, FlexAlign alignItems, bool isMainAxisHorizontal, bool isMainAxisReversed)
{
  for(uint32_t idx : line.childIndices)
  {
    ViewImpl& childImpl  = GetImpl(children[idx]);
    float     childScale = childImpl.GetEffectiveScale();
    Insets    margin     = childImpl.GetMargin();

    float childMainSize  = isMainAxisHorizontal ? workingSizes[idx].width : workingSizes[idx].height;
    float childCrossSize = isMainAxisHorizontal ? workingSizes[idx].height : workingSizes[idx].width;
    float marginMain     = isMainAxisHorizontal ? (margin.start + margin.end) * childScale
                                                : (margin.top + margin.bottom) * childScale;
    float marginCross    = isMainAxisHorizontal ? (margin.top + margin.bottom) * childScale
                                                : (margin.start + margin.end) * childScale;

    bool crossIsMatchParent = isMainAxisHorizontal ? (childImpl.GetRequestedHeight() == MATCH_PARENT)
                                                   : (childImpl.GetRequestedWidth() == MATCH_PARENT);
    if(crossIsMatchParent)
    {
      childCrossSize = line.crossSize - marginCross;
    }

    bool mainIsMatchParent = isMainAxisHorizontal ? (childImpl.GetRequestedWidth() == MATCH_PARENT)
                                                  : (childImpl.GetRequestedHeight() == MATCH_PARENT);
    // A main-axis MATCH_PARENT child only fills the whole line when it does not
    // participate in flex-grow distribution. When flex-grow is set, the grow
    // distribution already produced the child's share in workingSizes; trusting
    // that value keeps siblings from being overlapped by a full-line overwrite.
    if(mainIsMatchParent && GetFlexGrow(childImpl) == 0.0f)
    {
      float availMain = isMainAxisHorizontal ? contentWidth : contentHeight;
      childMainSize   = std::max(0.0f, availMain - marginMain);
    }

    FlexAlign effectiveAlign = GetAlignSelf(childImpl);
    if(effectiveAlign == FlexAlign::AUTO)
    {
      effectiveAlign = alignItems;
    }

    float childCrossOffset = crossOffsetInOut;
    float crossSpace       = line.crossSize - childCrossSize - marginCross;
    switch(effectiveAlign)
    {
      case FlexAlign::FLEX_START:
      case FlexAlign::AUTO:
        break;
      case FlexAlign::FLEX_END:
        childCrossOffset += crossSpace;
        break;
      case FlexAlign::CENTER:
        childCrossOffset += crossSpace / 2.0f;
        break;
      case FlexAlign::STRETCH:
        childCrossSize = line.crossSize - marginCross;
        break;
      case FlexAlign::BASELINE:
        break;
    }

    float      allocMain  = childMainSize + marginMain;
    float      allocCross = childCrossSize + marginCross;
    LayoutRect childBounds;
    if(isMainAxisHorizontal)
    {
      if(isMainAxisReversed)
      {
        mainOffsetInOut -= allocMain;
        childBounds.x = bounds.x + mainOffsetInOut;
        mainOffsetInOut -= spacing;
      }
      else
      {
        childBounds.x = bounds.x + mainOffsetInOut;
        mainOffsetInOut += allocMain + spacing;
      }
      childBounds.y      = bounds.y + childCrossOffset;
      childBounds.width  = allocMain;
      childBounds.height = allocCross;
    }
    else
    {
      if(isMainAxisReversed)
      {
        mainOffsetInOut -= allocMain;
        childBounds.y = bounds.y + mainOffsetInOut;
        mainOffsetInOut -= spacing;
      }
      else
      {
        childBounds.y = bounds.y + mainOffsetInOut;
        mainOffsetInOut += allocMain + spacing;
      }
      childBounds.x      = bounds.x + childCrossOffset;
      childBounds.width  = allocCross;
      childBounds.height = allocMain;
    }
    childBounds.x += margin.start * childScale;
    childBounds.y += margin.top * childScale;
    childBounds.width  = std::max(0.0f, childBounds.width - (margin.start + margin.end) * childScale);
    childBounds.height = std::max(0.0f, childBounds.height - (margin.top + margin.bottom) * childScale);

    if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(owner);
      childImpl.Measure(childBounds.width, childBounds.height);
    }
    childImpl.Arrange(childBounds);
  }
  crossOffsetInOut += line.crossSize;
}

} // namespace

class FlexLayoutManager::Impl : public LayoutManager::Impl
{
public:
  Impl(FlexDirection direction, FlexWrap wrap, FlexJustify justify, FlexAlign alignItems, FlexAlign alignContent)
  : mDirection(direction),
    mWrap(wrap),
    mJustifyContent(justify),
    mAlignItems(alignItems),
    mAlignContent(alignContent)
  {
  }

  FlexDirection mDirection;
  FlexWrap      mWrap;
  FlexJustify   mJustifyContent;
  FlexAlign     mAlignItems;
  FlexAlign     mAlignContent;
};

FlexLayoutManager::FlexLayoutManager(FlexDirection direction, FlexWrap wrap, FlexJustify justify, FlexAlign alignItems,
                                     FlexAlign alignContent)
: LayoutManager(new Impl(direction, wrap, justify, alignItems, alignContent))
{
}

FlexLayoutManager::~FlexLayoutManager()
{
}

void FlexLayoutManager::SetDirection(FlexDirection direction)
{
  GetImplAs<Impl>()->mDirection = direction;
}

FlexDirection FlexLayoutManager::GetDirection() const
{
  return GetImplAs<Impl>()->mDirection;
}

void FlexLayoutManager::SetWrap(FlexWrap wrap)
{
  GetImplAs<Impl>()->mWrap = wrap;
}

FlexWrap FlexLayoutManager::GetWrap() const
{
  return GetImplAs<Impl>()->mWrap;
}

void FlexLayoutManager::SetJustifyContent(FlexJustify justify)
{
  GetImplAs<Impl>()->mJustifyContent = justify;
}

FlexJustify FlexLayoutManager::GetJustifyContent() const
{
  return GetImplAs<Impl>()->mJustifyContent;
}

void FlexLayoutManager::SetAlignItems(FlexAlign align)
{
  GetImplAs<Impl>()->mAlignItems = align;
}

FlexAlign FlexLayoutManager::GetAlignItems() const
{
  return GetImplAs<Impl>()->mAlignItems;
}

void FlexLayoutManager::SetAlignContent(FlexAlign align)
{
  GetImplAs<Impl>()->mAlignContent = align;
}

FlexAlign FlexLayoutManager::GetAlignContent() const
{
  return GetImplAs<Impl>()->mAlignContent;
}

bool FlexLayoutManager::IsMainAxisHorizontal() const
{
  const FlexDirection direction = GetImplAs<Impl>()->mDirection;
  return direction == FlexDirection::ROW || direction == FlexDirection::ROW_REVERSE;
}

bool FlexLayoutManager::IsMainAxisReversed() const
{
  const FlexDirection direction = GetImplAs<Impl>()->mDirection;
  return direction == FlexDirection::ROW_REVERSE || direction == FlexDirection::COLUMN_REVERSE;
}

MeasuredSize FlexLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto* impl = GetImplAs<Impl>();

  const uint32_t    childCount = GetChildViewCount(view);
  std::vector<View> children;
  children.reserve(childCount);
  for(uint32_t i = 0; i < childCount; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  float availableMain = IsMainAxisHorizontal() ? widthConstraint : heightConstraint;

  std::vector<FlexLine> lines;
  FlexLine              currentLine;

  for(uint32_t i = 0; i < children.size(); ++i)
  {
    ViewImpl& childImpl = GetImpl(children[i]);

    if(IsChildStandalone(childImpl))
    {
      continue;
    }

    float        childScale            = childImpl.GetEffectiveScale();
    Insets       margin                = childImpl.GetMargin();
    float        marginW               = (margin.start + margin.end) * childScale;
    float        marginH               = (margin.top + margin.bottom) * childScale;
    float        childWidthConstraint  = std::max(0.0f, widthConstraint - marginW);
    float        childHeightConstraint = std::max(0.0f, heightConstraint - marginH);
    MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);

    MeasuredSize workingSize = childSize;
    float        basis       = GetFlexBasis(childImpl);
    if(basis >= 0.0f)
    {
      if(IsMainAxisHorizontal())
      {
        workingSize.width = basis * childScale;
      }
      else
      {
        workingSize.height = basis * childScale;
      }
    }

    float childMainSize  = IsMainAxisHorizontal() ? workingSize.width + marginW
                                                  : workingSize.height + marginH;
    float childCrossSize = IsMainAxisHorizontal() ? workingSize.height + marginH
                                                  : workingSize.width + marginW;

    bool shouldWrap = (impl->mWrap != FlexWrap::NO_WRAP) && !currentLine.childIndices.empty() &&
                      (currentLine.mainSize + childMainSize > availableMain);

    if(shouldWrap)
    {
      lines.push_back(currentLine);
      currentLine = FlexLine();
    }

    currentLine.childIndices.push_back(i);
    currentLine.mainSize += childMainSize;
    currentLine.crossSize = std::max(currentLine.crossSize, childCrossSize);
  }

  if(!currentLine.childIndices.empty())
  {
    lines.push_back(currentLine);
  }

  float totalMainSize  = 0.0f;
  float totalCrossSize = 0.0f;

  for(const auto& line : lines)
  {
    totalMainSize = std::max(totalMainSize, line.mainSize);
    totalCrossSize += line.crossSize;
  }

  MeasuredSize result;
  if(IsMainAxisHorizontal())
  {
    result.width  = totalMainSize;
    result.height = totalCrossSize;
  }
  else
  {
    result.width  = totalCrossSize;
    result.height = totalMainSize;
  }

  return result;
}

void FlexLayoutManager::Arrange(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return;
  }

  auto* impl = GetImplAs<Impl>();

  const uint32_t    childCount = GetChildViewCount(view);
  std::vector<View> children;
  children.reserve(childCount);
  for(uint32_t i = 0; i < childCount; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  float contentWidth  = bounds.width;
  float contentHeight = bounds.height;
  float availableMain = IsMainAxisHorizontal() ? contentWidth : contentHeight;

  // Local working buffer: seeded from each child's current measured size.
  std::vector<MeasuredSize> workingSizes(children.size());
  for(uint32_t i = 0; i < children.size(); ++i)
  {
    workingSizes[i] = GetImpl(children[i]).GetMeasuredSize();
  }

  std::vector<FlexLine> lines =
    BuildFlexLinesForArrange(children, workingSizes, availableMain, IsMainAxisHorizontal(), impl->mWrap);

  for(auto& line : lines)
  {
    ApplyFlexGrowShrink(line, children, workingSizes, availableMain, IsMainAxisHorizontal());
  }

  if(impl->mWrap == FlexWrap::WRAP_REVERSE)
  {
    std::reverse(lines.begin(), lines.end());
  }

  float availableCross = IsMainAxisHorizontal() ? contentHeight : contentWidth;

  if(impl->mWrap == FlexWrap::NO_WRAP && lines.size() == 1)
  {
    lines[0].crossSize = std::max(lines[0].crossSize, availableCross);
  }

  float totalLineCross = 0.0f;
  for(const auto& line : lines)
  {
    totalLineCross += line.crossSize;
  }
  float freeCross = std::max(0.0f, availableCross - totalLineCross);

  float crossOffset = 0.0f;

  if(impl->mWrap != FlexWrap::NO_WRAP && !lines.empty())
  {
    switch(impl->mAlignContent)
    {
      case FlexAlign::FLEX_END:
        crossOffset = freeCross;
        break;
      case FlexAlign::CENTER:
        crossOffset = freeCross / 2.0f;
        break;
      case FlexAlign::STRETCH:
      {
        float extraPerLine = freeCross / static_cast<float>(lines.size());
        for(auto& line : lines)
        {
          line.crossSize += extraPerLine;
        }
        break;
      }
      case FlexAlign::FLEX_START:
      case FlexAlign::AUTO:
      case FlexAlign::BASELINE:
      default:
        break;
    }
  }

  for(auto& line : lines)
  {
    float              freeSpace  = availableMain - line.mainSize;
    FlexJustifyOffsets justify    = GetFlexJustifyOffsets(freeSpace, impl->mJustifyContent, line.childIndices.size());
    float              mainOffset = justify.mainOffset;
    if(IsMainAxisReversed())
    {
      float contentMain = IsMainAxisHorizontal() ? contentWidth : contentHeight;
      mainOffset        = contentMain - justify.mainOffset;
    }
    ArrangeOneFlexLine(view, line, children, workingSizes, bounds, contentWidth, contentHeight, crossOffset, mainOffset,
                       justify.spacing, impl->mAlignItems, IsMainAxisHorizontal(), IsMainAxisReversed());
  }
}

} // namespace Ui
} // namespace Dali
