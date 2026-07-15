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
#include <dali-ui-components/internal/markdown/markdown-component.h>

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <algorithm>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/markdown/markdown-text-component.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{

constexpr float       BLOCK_GAP                        = 20.0f;
constexpr float       LIST_FIRST_LEVEL_INDENT          = 12.0f;
constexpr float       LIST_NESTED_LEVEL_INDENT         = 0.0f;
constexpr float       LIST_ITEM_MARGIN_BOTTOM          = 10.0f;
constexpr float       LIST_MARKER_WIDTH                = 32.0f;
constexpr float       LIST_MARKER_DIGIT_WIDTH          = 11.0f;
constexpr float       LIST_CONTENT_GAP                 = 12.0f;
constexpr float       LIST_LINE_HEIGHT                 = 32.0f;
constexpr float       UNORDERED_LIST_MARKER_SIZE_RATIO = 0.3f;
constexpr float       LIST_HOLLOW_BULLET_WIDTH         = 1.25f;
constexpr float       QUOTE_BAR_WIDTH                  = 6.0f;
constexpr float       QUOTE_CONTENT_GAP                = 18.0f;
constexpr float       CODE_PADDING                     = 10.0f;
constexpr float       CODE_PADDING_BOTTOM              = 8.0f;
constexpr float       CODE_TITLE_PADDING_START         = 10.0f;
constexpr float       CODE_TITLE_PADDING_TOP           = 10.0f;
constexpr float       CODE_CORNER_RADIUS               = 12.0f;
constexpr uint32_t    CODE_BACKGROUND_COLOR            = 0xF4F4F4;
constexpr uint32_t    CODE_TITLE_BACKGROUND            = 0xEEEEEE;
constexpr const char* DEFAULT_FONT_FAMILY              = "SamsungOneUI_400";
constexpr const char* CODE_FONT_FAMILY                 = "SamsungOneUI_300";
constexpr float       TABLE_CELL_PADDING               = 5.0f;
constexpr float       TABLE_HEAD_RULE_HEIGHT           = 1.0f;
constexpr float       RULE_HEIGHT                      = 1.0f;
constexpr float       BLOCK_IMAGE_HEIGHT               = 160.0f;

void SetStackFill(Ui::View view)
{
  view.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
}

void SetStackWeightFill(Ui::View view)
{
  view.SetLayoutParams(StackLayoutParams::New()
                         .SetWeight(1.0f)
                         .SetAlignment(LayoutAlignment::FILL));
}

Ui::StackLayout NewStack(StackOrientation orientation, float spacing = 0.0f)
{
  Ui::StackLayout view = Ui::StackLayout::New(orientation);
  view.SetSpacing(spacing);
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(WRAP_CONTENT);
  SetStackFill(view);
  return view;
}

Ui::View NewColorBox(uint32_t color, float cornerRadius = 0.0f)
{
  Ui::View view = Ui::View::New();
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(WRAP_CONTENT);
  view.SetBackgroundColor(UiColor(color));
  if(cornerRadius > 0.0f)
  {
    view.SetCornerRadius(cornerRadius);
  }
  SetStackFill(view);
  return view;
}

Ui::Label NewLabel(const Dali::String& text, float fontSize, uint32_t color)
{
  Ui::Label label = Ui::Label::New();
  label.SetMultiLine(true);
  label.SetTextOverflowMode(Text::OverflowMode::CLIP);
  label.SetFontFamily(Dali::String(DEFAULT_FONT_FAMILY));
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  label.SetText(text);
  label.SetAsyncRendering(false);
  return label;
}

float ListMarkerWidth(const MarkdownRenderNode& node)
{
  if(node.listKind == MarkdownListKind::ORDERED)
  {
    const std::size_t markerLength       = node.listMarkerColumnLength > 0u
                                             ? node.listMarkerColumnLength
                                             : std::to_string(node.listOrdinal).size() + 1u;
    const float       orderedMarkerWidth = std::max(LIST_MARKER_WIDTH, static_cast<float>(markerLength) * LIST_MARKER_DIGIT_WIDTH);
    return orderedMarkerWidth;
  }
  return LIST_MARKER_WIDTH;
}

float ListItemStartIndent(const MarkdownRenderNode& node)
{
  return node.listDepth <= 1u ? LIST_FIRST_LEVEL_INDENT : LIST_NESTED_LEVEL_INDENT;
}

std::string OrderedMarkerText(uint32_t ordinal, bool isRtl)
{
  const std::string number = std::to_string(ordinal);
  return isRtl ? "." + number : number + ".";
}

std::string ListMarkerText(const MarkdownRenderNode& node, bool isRtl)
{
  if(node.listKind == MarkdownListKind::ORDERED)
  {
    return OrderedMarkerText(node.listOrdinal, isRtl);
  }
  return std::string();
}

std::string TaskMarkerText(const MarkdownRenderNode& node)
{
  return node.taskChecked ? "[x]" : "[ ]";
}

bool UseBulletMarker(const MarkdownRenderNode& node)
{
  return node.listKind == MarkdownListKind::UNORDERED;
}

Ui::View NewBulletMarker(uint32_t depth, float markerSize, float lineHeight)
{
  const float effectiveLineHeight = std::max(0.0f, lineHeight);
  const float effectiveMarkerSize = std::min(std::max(0.0f, markerSize), effectiveLineHeight);
  const float remainingHeight     = effectiveLineHeight - effectiveMarkerSize;
  const float topMargin           = remainingHeight * 0.5f;
  const float bottomMargin        = remainingHeight - topMargin;
  const float startMargin         = std::max(0.0f, LIST_MARKER_WIDTH - effectiveMarkerSize);

  Ui::View bullet = Ui::View::New();
  bullet.SetRequestedWidth(effectiveMarkerSize);
  bullet.SetRequestedHeight(effectiveMarkerSize);
  bullet.SetMargin(Insets(startMargin, 0.0f, topMargin, bottomMargin));

  switch((depth > 0u ? depth - 1u : 0u) % 3u)
  {
    case 1u:
      bullet.SetCornerRadiusPolicyRelative();
      bullet.SetCornerRadius(0.5f);
      bullet.SetBorderlineColor(UiColor(0x000000));
      bullet.SetBorderlineWidth(LIST_HOLLOW_BULLET_WIDTH);
      break;
    case 2u:
      bullet.SetBackgroundColor(UiColor(0x000000));
      break;
    default:
      bullet.SetBackgroundColor(UiColor(0x000000));
      bullet.SetCornerRadiusPolicyRelative();
      bullet.SetCornerRadius(0.5f);
      break;
  }

  return bullet;
}

Text::Alignment TextAlignmentForTableCell(MarkdownTableAlignment alignment)
{
  switch(alignment)
  {
    case MarkdownTableAlignment::CENTER:
      return Text::Alignment::CENTER;
    case MarkdownTableAlignment::RIGHT:
      return Text::Alignment::END;
    case MarkdownTableAlignment::LEFT:
    case MarkdownTableAlignment::DEFAULT:
    default:
      return Text::Alignment::START;
  }
}

/**
 * @brief Provides shared root and content-host handling for Markdown components.
 */
class BaseComponent : public MarkdownComponent
{
public:
  Ui::View GetRootView() const override
  {
    return mRoot;
  }

  Ui::View GetContentHost() const override
  {
    return mContentHost ? mContentHost : mRoot;
  }

protected:
  Ui::View mRoot;
  Ui::View mContentHost;
};

/**
 * @brief Renders a Markdown container as a stack layout.
 */
class StackComponent : public BaseComponent
{
public:
  StackComponent(StackOrientation orientation, float spacing)
  {
    mRoot = NewStack(orientation, spacing);
  }

  void Update(const MarkdownRenderNode*, const MarkdownRenderNode&, const MarkdownTextUpdate&) override
  {
  }
};

/**
 * @brief Renders a Markdown text block using a text component.
 */
class TextBlockComponent : public BaseComponent
{
public:
  TextBlockComponent(const MarkdownRenderNode& node)
  {
    mTextComponent = CreateMarkdownLabelTextComponent(node);
    mRoot          = mTextComponent->GetView();
    mTextComponent->SetTextContent(node);
  }

  Ui::View GetContentHost() const override
  {
    return Ui::View();
  }

  void Update(const MarkdownRenderNode*, const MarkdownRenderNode& current, const MarkdownTextUpdate& textUpdate) override
  {
    if(textUpdate.type != MarkdownTextUpdate::Type::UNCHANGED)
    {
      mTextComponent->UpdateTextContent(current, textUpdate);
    }
  }

private:
  std::unique_ptr<MarkdownTextComponent> mTextComponent;
};

/**
 * @brief Renders a Markdown list item and its marker.
 */
class ListItemComponent : public BaseComponent, public ConnectionTracker
{
public:
  ListItemComponent(const MarkdownRenderNode& node)
  {
    mItem = NewStack(StackOrientation::HORIZONTAL, 0.0f);
    ApplyItemMargin(node);
    mListKind               = node.listKind;
    mListOrdinal            = node.listOrdinal;
    mListDepth              = node.listDepth;
    mListMarkerColumnLength = node.listMarkerColumnLength;
    mTaskListItem           = node.taskListItem;
    mTaskChecked            = node.taskChecked;

    mMarkerHost = NewStack(StackOrientation::HORIZONTAL, 0.0f);
    mMarkerHost.SetRequestedWidth(ListMarkerWidth(node));
    mMarkerHost.SetMargin(Extents(0, static_cast<int16_t>(LIST_CONTENT_GAP), 0, 0));
    mMarkerHost.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));

    mMarkerLabel = NewLabel(Dali::String(), 20.0f, 0x000000);
    mMarkerLabel.SetMultiLine(false);
    mMarkerLabel.SetTextOverflowMode(Text::OverflowMode::CLIP);
    mMarkerLabel.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
    mMarkerLabel.SetLineHeight(LIST_LINE_HEIGHT);
    mMarkerLabel.SetRequestedWidth(ListMarkerWidth(node));
    mMarkerLabel.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    mContentHost = NewStack(StackOrientation::VERTICAL, 0.0f);
    SetStackWeightFill(mContentHost);
    ApplyMarkerPresentation(node);
    ApplyTaskPresentation(node);
    mItem.Add(mMarkerHost);
    mItem.Add(mContentHost);
    mItem.LayoutDirectionChangedSignal().Connect(this, &ListItemComponent::OnLayoutDirectionChanged);
    mRoot = mItem;

    UpdateTightText(nullptr, node, MarkdownTextUpdate());
  }

  void Update(const MarkdownRenderNode* previous, const MarkdownRenderNode& current, const MarkdownTextUpdate& textUpdate) override
  {
    if(mListKind != current.listKind ||
       mListOrdinal != current.listOrdinal ||
       mListDepth != current.listDepth ||
       mListMarkerColumnLength != current.listMarkerColumnLength)
    {
      mListKind               = current.listKind;
      mListOrdinal            = current.listOrdinal;
      mListDepth              = current.listDepth;
      mListMarkerColumnLength = current.listMarkerColumnLength;
      ApplyItemMargin(current);
      ApplyMarkerPresentation(current);
    }

    if(mTaskListItem != current.taskListItem || mTaskChecked != current.taskChecked)
    {
      mTaskListItem = current.taskListItem;
      mTaskChecked  = current.taskChecked;
      ApplyTaskPresentation(current);
    }
    UpdateTightText(previous, current, textUpdate);
  }

private:
  void OnLayoutDirectionChanged(Actor, LayoutDirection::Type type)
  {
    const bool isRtl = type == LayoutDirection::RIGHT_TO_LEFT;
    if(mIsRtl == isRtl)
    {
      return;
    }

    mIsRtl = isRtl;

    MarkdownRenderNode node;
    node.listKind               = mListKind;
    node.listOrdinal            = mListOrdinal;
    node.listDepth              = mListDepth;
    node.listMarkerColumnLength = mListMarkerColumnLength;
    node.taskListItem           = mTaskListItem;
    node.taskChecked            = mTaskChecked;
    ApplyMarkerPresentation(node);
  }

  void ApplyMarkerPresentation(const MarkdownRenderNode& node)
  {
    mMarkerHost.SetRequestedWidth(ListMarkerWidth(node));
    mMarkerHost.SetMargin(Extents(0, static_cast<int16_t>(LIST_CONTENT_GAP), 0, 0));

    if(mBulletMarkerAttached)
    {
      mMarkerHost.Remove(mBulletMarker, RemovePolicy::IMMEDIATE);
      mBulletMarkerAttached = false;
      mBulletMarker         = Ui::View();
    }
    if(mMarkerLabelAttached)
    {
      mMarkerHost.Remove(mMarkerLabel, RemovePolicy::IMMEDIATE);
      mMarkerLabelAttached = false;
    }

    const bool useBullet      = UseBulletMarker(node);
    const bool useMarkerLabel = node.listKind == MarkdownListKind::ORDERED;
    if(useBullet)
    {
      const float textFontSize = ResolveMarkdownTextFontSize(node);
      const float lineHeight   = ResolveMarkdownTextLineHeight(node);
      const float markerSize   = textFontSize * UNORDERED_LIST_MARKER_SIZE_RATIO;
      mBulletMarker            = NewBulletMarker(node.listDepth, markerSize, lineHeight);
      mMarkerHost.Add(mBulletMarker);
      mBulletMarkerAttached = true;
    }

    if(useMarkerLabel)
    {
      mMarkerLabel.SetHorizontalTextAlignment(Text::Alignment::END);
      mMarkerLabel.SetRequestedWidth(ListMarkerWidth(node));
      mMarkerLabel.SetMargin(Extents());
      ApplyMarkerText(node);
      mMarkerHost.Add(mMarkerLabel);
      mMarkerLabelAttached = true;
    }
    else
    {
      mMarkerText.clear();
    }
  }

  void ApplyTaskPresentation(const MarkdownRenderNode& node)
  {
    if(!node.taskListItem)
    {
      DetachTaskRow();
      mTaskMarkerText.clear();
      return;
    }

    EnsureTaskRow();
    const std::string marker = TaskMarkerText(node);
    if(marker != mTaskMarkerText)
    {
      mTaskMarkerLabel.SetText(Dali::String(marker.c_str()));
      mTaskMarkerText = marker;
    }

    const Vector3 markerNaturalSize = mTaskMarkerLabel.GetNaturalSize();
    mTaskMarkerLabel.SetRequestedWidth(markerNaturalSize.width);
    mTaskMarkerLabel.SetRequestedHeight(std::max(LIST_LINE_HEIGHT, markerNaturalSize.height));

    if(mTaskRowAttached)
    {
      return;
    }

    if(mTextComponent)
    {
      Ui::View textView = mTextComponent->GetView();
      mContentHost.Remove(textView, RemovePolicy::IMMEDIATE);
      SetStackWeightFill(textView);
    }

    mContentHost.Insert(0u, mTaskRow);
    mTaskRowAttached = true;
    if(mTextComponent)
    {
      mTaskRow.Add(mTextComponent->GetView());
    }
  }

  void EnsureTaskRow()
  {
    if(mTaskRow)
    {
      return;
    }

    mTaskRow         = NewStack(StackOrientation::HORIZONTAL, LIST_CONTENT_GAP);
    mTaskMarkerLabel = NewLabel(Dali::String(), 20.0f, 0x000000);
    mTaskMarkerLabel.SetMultiLine(false);
    mTaskMarkerLabel.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
    mTaskMarkerLabel.SetLineHeight(LIST_LINE_HEIGHT);
    mTaskMarkerLabel.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    mTaskRow.Add(mTaskMarkerLabel);
  }

  void DetachTaskRow()
  {
    if(!mTaskRowAttached)
    {
      return;
    }

    if(mTextComponent)
    {
      Ui::View textView = mTextComponent->GetView();
      mTaskRow.Remove(textView, RemovePolicy::IMMEDIATE);
      SetStackFill(textView);
    }

    mContentHost.Remove(mTaskRow, RemovePolicy::IMMEDIATE);
    mTaskRowAttached = false;
    if(mTextComponent)
    {
      mContentHost.Insert(0u, mTextComponent->GetView());
    }
  }

  void ApplyItemMargin(const MarkdownRenderNode& node)
  {
    mItem.SetMargin(Extents(static_cast<int16_t>(ListItemStartIndent(node)),
                            0,
                            0,
                            0));
  }

  void ApplyTextMargin()
  {
    if(mTextComponent)
    {
      mTextComponent->GetView().SetMargin(Extents(0,
                                                  0,
                                                  0,
                                                  static_cast<int16_t>(LIST_ITEM_MARGIN_BOTTOM)));
    }
  }

  bool ApplyMarkerText(const MarkdownRenderNode& node)
  {
    const std::string marker = ListMarkerText(node, mIsRtl);
    if(marker == mMarkerText)
    {
      return false;
    }

    mMarkerLabel.SetText(Dali::String(marker.c_str()));
    mMarkerText = marker;
    return true;
  }

  void UpdateTightText(const MarkdownRenderNode* previous, const MarkdownRenderNode& current, const MarkdownTextUpdate& textUpdate)
  {
    if(current.text.empty())
    {
      if(mTextComponent)
      {
        Ui::View textHost = mTaskRowAttached ? mTaskRow : mContentHost;
        textHost.Remove(mTextComponent->GetView(), RemovePolicy::IMMEDIATE);
        mTextComponent.reset();
      }
      return;
    }

    if(!mTextComponent)
    {
      mTextComponent = CreateMarkdownLabelTextComponent(current);
      ApplyTextMargin();
      mTextComponent->SetTextContent(current);
      if(mTaskRowAttached)
      {
        SetStackWeightFill(mTextComponent->GetView());
        mTaskRow.Add(mTextComponent->GetView());
      }
      else
      {
        mContentHost.Insert(0u, mTextComponent->GetView());
      }
      return;
    }

    if(!previous || textUpdate.type != MarkdownTextUpdate::Type::UNCHANGED)
    {
      if(previous)
      {
        mTextComponent->UpdateTextContent(current, textUpdate);
      }
      else
      {
        mTextComponent->SetTextContent(current);
      }
    }
  }

private:
  Ui::StackLayout                        mItem;
  Ui::StackLayout                        mMarkerHost;
  Ui::Label                              mMarkerLabel;
  Ui::View                               mBulletMarker;
  Ui::StackLayout                        mTaskRow;
  Ui::Label                              mTaskMarkerLabel;
  std::string                            mMarkerText;
  std::string                            mTaskMarkerText;
  MarkdownListKind                       mListKind{MarkdownListKind::NONE};
  uint32_t                               mListOrdinal{0u};
  uint32_t                               mListDepth{0u};
  uint32_t                               mListMarkerColumnLength{0u};
  bool                                   mTaskListItem{false};
  bool                                   mTaskChecked{false};
  bool                                   mIsRtl{false};
  bool                                   mMarkerLabelAttached{false};
  bool                                   mBulletMarkerAttached{false};
  bool                                   mTaskRowAttached{false};
  std::unique_ptr<MarkdownTextComponent> mTextComponent;
};

/**
 * @brief Renders a Markdown block quote container.
 */
class QuoteComponent : public BaseComponent
{
public:
  QuoteComponent()
  {
    Ui::StackLayout quote = NewStack(StackOrientation::HORIZONTAL, 0.0f);
    quote.SetPadding(Extents(10, 10, 10, 10));

    Ui::View decoration = NewColorBox(0xDFDFDF, 3.0f);
    decoration.SetRequestedWidth(QUOTE_BAR_WIDTH);
    decoration.SetMargin(Extents(0, static_cast<int16_t>(QUOTE_CONTENT_GAP), 0, 0));
    decoration.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));

    mContentHost = NewStack(StackOrientation::VERTICAL, BLOCK_GAP);
    SetStackWeightFill(mContentHost);
    quote.Add(decoration);
    quote.Add(mContentHost);
    mRoot = quote;
  }

  void Update(const MarkdownRenderNode*, const MarkdownRenderNode&, const MarkdownTextUpdate&) override
  {
  }
};

/**
 * @brief Renders a Markdown code block with optional language text.
 */
class CodeBlockComponent : public BaseComponent
{
public:
  CodeBlockComponent(const MarkdownRenderNode& node)
  {
    mCodeRoot = NewStack(StackOrientation::VERTICAL, 0.0f);
    mCodeRoot.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
    mCodeRoot.SetBackgroundColor(UiColor(CODE_BACKGROUND_COLOR));
    mCodeRoot.SetCornerRadius(CODE_CORNER_RADIUS);

    mLanguageHost = NewStack(StackOrientation::HORIZONTAL, 0.0f);
    mLanguageHost.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
    mLanguageHost.SetPadding(Extents(static_cast<int16_t>(CODE_TITLE_PADDING_START),
                                     static_cast<int16_t>(CODE_TITLE_PADDING_START),
                                     static_cast<int16_t>(CODE_TITLE_PADDING_TOP),
                                     static_cast<int16_t>(CODE_TITLE_PADDING_TOP)));
    mLanguageHost.SetBackgroundColor(UiColor(CODE_TITLE_BACKGROUND));
    mLanguageHost.SetCornerRadius(CODE_CORNER_RADIUS, CODE_CORNER_RADIUS, 0.0f, 0.0f);

    mLanguage      = node.language;
    mLanguageLabel = NewLabel(Dali::String(mLanguage.c_str()), 16.0f, 0x454545);
    mLanguageLabel.SetMultiLine(false);
    mLanguageLabel.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
    mLanguageLabel.SetFontFamily(Dali::String(CODE_FONT_FAMILY));
    mLanguageLabel.SetRequestedWidth(WRAP_CONTENT);
    mLanguageLabel.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    mLanguageHost.Add(mLanguageLabel);

    mCodeContentHost = NewStack(StackOrientation::VERTICAL, 0.0f);
    mCodeContentHost.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
    mCodeContentHost.SetPadding(Extents(static_cast<int16_t>(CODE_PADDING),
                                        static_cast<int16_t>(CODE_PADDING),
                                        static_cast<int16_t>(CODE_PADDING),
                                        static_cast<int16_t>(CODE_PADDING_BOTTOM)));

    mTextComponent = CreateMarkdownLabelTextComponent(node);
    mTextView      = mTextComponent->GetView();
    mTextView.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
    mTextView.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    mCodeContentHost.Add(mTextView);
    RebuildChildren();
    mRoot = mCodeRoot;
    mTextComponent->SetTextContent(node);
  }

  Ui::View GetContentHost() const override
  {
    return Ui::View();
  }

  void Update(const MarkdownRenderNode*, const MarkdownRenderNode& current, const MarkdownTextUpdate& textUpdate) override
  {
    if(mLanguage != current.language)
    {
      mLanguageLabel.SetText(Dali::String(current.language.c_str()));
      mLanguage = current.language;
      RebuildChildren();
    }
    if(textUpdate.type != MarkdownTextUpdate::Type::UNCHANGED)
    {
      mTextComponent->UpdateTextContent(current, textUpdate);
    }
  }

private:
  void RebuildChildren()
  {
    if(mLanguageHostAttached)
    {
      mCodeRoot.Remove(mLanguageHost, RemovePolicy::IMMEDIATE);
      mLanguageHostAttached = false;
    }
    if(mCodeContentHostAttached)
    {
      mCodeRoot.Remove(mCodeContentHost, RemovePolicy::IMMEDIATE);
      mCodeContentHostAttached = false;
    }

    if(!mLanguage.empty())
    {
      mCodeRoot.Add(mLanguageHost);
      mLanguageHostAttached = true;
    }

    mCodeRoot.Add(mCodeContentHost);
    mCodeContentHostAttached = true;
  }

private:
  Ui::StackLayout                        mCodeRoot;
  Ui::StackLayout                        mLanguageHost;
  Ui::StackLayout                        mCodeContentHost;
  Ui::Label                              mLanguageLabel;
  Ui::View                               mTextView;
  std::string                            mLanguage;
  bool                                   mLanguageHostAttached{false};
  bool                                   mCodeContentHostAttached{false};
  std::unique_ptr<MarkdownTextComponent> mTextComponent;
};

/**
 * @brief Renders a Markdown table head container.
 */
class TableHeadComponent : public BaseComponent
{
public:
  TableHeadComponent()
  {
    Ui::StackLayout root = NewStack(StackOrientation::VERTICAL, 0.0f);
    mContentHost         = NewStack(StackOrientation::VERTICAL, 0.0f);

    Ui::View rule = NewColorBox(0x000000);
    rule.SetRequestedHeight(TABLE_HEAD_RULE_HEIGHT);

    root.Add(mContentHost);
    root.Add(rule);
    mRoot = root;
  }

  void Update(const MarkdownRenderNode*, const MarkdownRenderNode&, const MarkdownTextUpdate&) override
  {
  }
};

/**
 * @brief Renders a Markdown table cell and its inline text.
 */
class TableCellComponent : public BaseComponent
{
public:
  TableCellComponent(const MarkdownRenderNode& node)
  {
    Ui::StackLayout cell = NewStack(StackOrientation::VERTICAL, BLOCK_GAP);
    cell.SetPadding(Extents(static_cast<int16_t>(TABLE_CELL_PADDING),
                            static_cast<int16_t>(TABLE_CELL_PADDING),
                            static_cast<int16_t>(TABLE_CELL_PADDING),
                            static_cast<int16_t>(TABLE_CELL_PADDING)));
    SetStackWeightFill(cell);
    mRoot        = cell;
    mContentHost = cell;
    mAlignment   = TextAlignmentForTableCell(node.tableAlignment);
    UpdateCellText(nullptr, node, MarkdownTextUpdate());
  }

  void Update(const MarkdownRenderNode* previous, const MarkdownRenderNode& current, const MarkdownTextUpdate& textUpdate) override
  {
    const auto alignment = TextAlignmentForTableCell(current.tableAlignment);
    if(alignment != mAlignment)
    {
      mAlignment = alignment;
      if(mTextComponent)
      {
        Ui::Label label = Ui::Label::DownCast(mTextComponent->GetView());
        if(label)
        {
          label.SetHorizontalTextAlignment(mAlignment);
        }
      }
    }
    UpdateCellText(previous, current, textUpdate);
  }

private:
  void UpdateCellText(const MarkdownRenderNode* previous, const MarkdownRenderNode& current, const MarkdownTextUpdate& textUpdate)
  {
    if(current.text.empty())
    {
      if(mTextComponent)
      {
        mContentHost.Remove(mTextComponent->GetView(), RemovePolicy::IMMEDIATE);
        mTextComponent.reset();
      }
      return;
    }

    if(!mTextComponent)
    {
      mTextComponent = CreateMarkdownLabelTextComponent(current);
      mTextComponent->SetTextContent(current);
      Ui::Label label = Ui::Label::DownCast(mTextComponent->GetView());
      if(label)
      {
        label.SetHorizontalTextAlignment(mAlignment);
      }
      mContentHost.Add(mTextComponent->GetView());
      return;
    }

    if(!previous || textUpdate.type != MarkdownTextUpdate::Type::UNCHANGED)
    {
      if(previous)
      {
        mTextComponent->UpdateTextContent(current, textUpdate);
      }
      else
      {
        mTextComponent->SetTextContent(current);
      }
    }
  }

private:
  std::unique_ptr<MarkdownTextComponent> mTextComponent;
  Text::Alignment                        mAlignment{Text::Alignment::START};
};

/**
 * @brief Renders a Markdown block image or its fallback text.
 */
class ImageComponent : public BaseComponent
{
public:
  ImageComponent(const MarkdownRenderNode& node)
  {
    mRoot = NewStack(StackOrientation::VERTICAL, 0.0f);
    Update(nullptr, node, MarkdownTextUpdate());
  }

  Ui::View GetContentHost() const override
  {
    return Ui::View();
  }

  void Update(const MarkdownRenderNode* previous, const MarkdownRenderNode& current, const MarkdownTextUpdate&) override
  {
    if(previous && previous->sourceUrl == current.sourceUrl && previous->altText == current.altText)
    {
      return;
    }

    if(mRenderedChild)
    {
      mRoot.Remove(mRenderedChild, RemovePolicy::IMMEDIATE);
      mRenderedChild = Ui::View();
      mTextComponent.reset();
    }

    if(!current.sourceUrl.empty())
    {
      Ui::ImageView image = Ui::ImageView::New(Dali::String(current.sourceUrl.c_str()));
      image.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
      image.SetRequestedWidth(MATCH_PARENT);
      image.SetRequestedHeight(BLOCK_IMAGE_HEIGHT);
      SetStackFill(image);
      mRenderedChild = image;
    }
    else
    {
      MarkdownRenderNode fallback = current;
      fallback.role               = MarkdownRenderRole::PARAGRAPH;
      fallback.text               = current.altText;
      fallback.utf32Length        = MarkdownUtf8Length(fallback.text);
      mTextComponent              = CreateMarkdownLabelTextComponent(fallback);
      mTextComponent->SetTextContent(fallback);
      mRenderedChild = mTextComponent->GetView();
    }

    if(mRenderedChild)
    {
      mRoot.Add(mRenderedChild);
    }
  }

private:
  Ui::View                               mRenderedChild;
  std::unique_ptr<MarkdownTextComponent> mTextComponent;
};

/**
 * @brief Renders a Markdown thematic break.
 */
class ThematicBreakComponent : public BaseComponent
{
public:
  ThematicBreakComponent()
  {
    Ui::View rule = NewColorBox(0xDFDFDF);
    rule.SetRequestedHeight(RULE_HEIGHT);
    mRoot = rule;
  }

  Ui::View GetContentHost() const override
  {
    return Ui::View();
  }

  void Update(const MarkdownRenderNode*, const MarkdownRenderNode&, const MarkdownTextUpdate&) override
  {
  }
};

} // namespace

std::unique_ptr<MarkdownComponent> CreateMarkdownComponent(const MarkdownRenderNode& node)
{
  switch(node.role)
  {
    case MarkdownRenderRole::LIST:
      return std::unique_ptr<MarkdownComponent>(new StackComponent(StackOrientation::VERTICAL, 0.0f));
    case MarkdownRenderRole::LIST_ITEM:
      return std::unique_ptr<MarkdownComponent>(new ListItemComponent(node));
    case MarkdownRenderRole::QUOTE:
      return std::unique_ptr<MarkdownComponent>(new QuoteComponent());
    case MarkdownRenderRole::CODE_BLOCK:
      return std::unique_ptr<MarkdownComponent>(new CodeBlockComponent(node));
    case MarkdownRenderRole::TABLE:
    case MarkdownRenderRole::TABLE_BODY:
      return std::unique_ptr<MarkdownComponent>(new StackComponent(StackOrientation::VERTICAL, 0.0f));
    case MarkdownRenderRole::TABLE_HEAD:
      return std::unique_ptr<MarkdownComponent>(new TableHeadComponent());
    case MarkdownRenderRole::TABLE_ROW:
      return std::unique_ptr<MarkdownComponent>(new StackComponent(StackOrientation::HORIZONTAL, 0.0f));
    case MarkdownRenderRole::TABLE_CELL:
      return std::unique_ptr<MarkdownComponent>(new TableCellComponent(node));
    case MarkdownRenderRole::BLOCK_IMAGE:
      return std::unique_ptr<MarkdownComponent>(new ImageComponent(node));
    case MarkdownRenderRole::THEMATIC_BREAK:
      return std::unique_ptr<MarkdownComponent>(new ThematicBreakComponent());
    default:
      if(MarkdownIsTextRole(node.role))
      {
        return std::unique_ptr<MarkdownComponent>(new TextBlockComponent(node));
      }
      return std::unique_ptr<MarkdownComponent>(new StackComponent(StackOrientation::VERTICAL, BLOCK_GAP));
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
