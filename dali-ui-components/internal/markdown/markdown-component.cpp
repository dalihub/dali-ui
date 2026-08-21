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
#include <dali-ui-components/internal/markdown/markdown-view-defaults.h>
#include <dali-ui-components/internal/styles/markdown-view-style-impl.h>
#include <dali-ui-components/public-api/check-box.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{

constexpr float LIST_FIRST_LEVEL_INDENT          = 12.0f;
constexpr float LIST_NESTED_LEVEL_INDENT         = 0.0f;
constexpr float LIST_MARKER_DIGIT_WIDTH_RATIO    = 0.55f;
constexpr float LIST_CONTENT_GAP                 = 12.0f;
constexpr float UNORDERED_LIST_MARKER_SIZE_RATIO = 0.3f;
constexpr float LIST_HOLLOW_BULLET_WIDTH_MIN     = 1.0f;
constexpr float LIST_HOLLOW_BULLET_WIDTH_MAX     = 2.5f;
constexpr float LIST_HOLLOW_BULLET_WIDTH_DIVISOR = 16.0f;
constexpr float QUOTE_CONTENT_GAP                = 18.0f;
constexpr float BLOCK_IMAGE_HEIGHT               = 160.0f;

enum class MarkdownImagePresentation : uint8_t
{
  TEXT_WITH_URL,
  IMAGE
};

constexpr MarkdownImagePresentation DEFAULT_IMAGE_PRESENTATION = MarkdownImagePresentation::TEXT_WITH_URL;

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

Ui::View NewColorBox(const UiColor& color, float cornerRadius = 0.0f)
{
  Ui::View view = Ui::View::New();
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(WRAP_CONTENT);
  view.SetBackgroundColor(color);
  if(cornerRadius > 0.0f)
  {
    view.SetCornerRadius(cornerRadius);
  }
  SetStackFill(view);
  return view;
}

Ui::Label NewLabel(const Dali::String& text, const Dali::String& fontFamily, float fontSize, const UiColor& color)
{
  Ui::Label label = Ui::Label::New();
  label.SetMultiLine(true);
  label.SetTextOverflowMode(Text::OverflowMode::CLIP);
  label.SetFontFamily(fontFamily);
  label.SetFontSize(fontSize);
  label.SetSystemFontSizeScaleEnabled(false);
  label.SetTextColor(color);
  label.SetText(text);
  label.SetAsyncRendering(false);
  return label;
}

float ListMarkerMinimumWidth(const MarkdownRenderNode& node, float fontSize)
{
  const float markerColumnWidth = std::max(MarkdownViewDefaults::DEFAULT_BODY_LINE_HEIGHT,
                                           fontSize * MarkdownViewDefaults::BODY_LINE_HEIGHT_RATIO);
  if(node.listKind == MarkdownListKind::ORDERED)
  {
    const std::size_t markerLength       = node.listMarkerColumnLength > 0u
                                             ? node.listMarkerColumnLength
                                             : std::to_string(node.listOrdinal).size() + 1u;
    const float       orderedMarkerWidth = std::max(markerColumnWidth,
                                                    static_cast<float>(markerLength) * fontSize * LIST_MARKER_DIGIT_WIDTH_RATIO);
    return orderedMarkerWidth;
  }
  return markerColumnWidth;
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

bool UseBulletMarker(const MarkdownRenderNode& node)
{
  return node.listKind == MarkdownListKind::UNORDERED;
}

bool HasRenderableText(const MarkdownRenderNode& node)
{
  return !node.text.empty() || !node.linkRanges.empty() || !node.inlineObjects.empty();
}

Ui::View NewBulletMarker(uint32_t depth, float fontSize, float markerSize, float lineHeight, float markerColumnWidth, const UiColor& color)
{
  const float effectiveLineHeight = std::max(0.0f, lineHeight);
  const float effectiveMarkerSize = std::min(std::max(0.0f, markerSize), effectiveLineHeight);
  const float remainingHeight     = effectiveLineHeight - effectiveMarkerSize;
  const float topMargin           = remainingHeight * 0.5f;
  const float bottomMargin        = remainingHeight - topMargin;
  const float startMargin         = std::max(0.0f, markerColumnWidth - effectiveMarkerSize);

  Ui::View bullet = Ui::View::New();
  bullet.SetRequestedWidth(effectiveMarkerSize);
  bullet.SetRequestedHeight(effectiveMarkerSize);
  bullet.SetMargin(Insets(startMargin, 0.0f, topMargin, bottomMargin));

  switch((depth > 0u ? depth - 1u : 0u) % 3u)
  {
    case 1u:
      bullet.SetCornerRadiusPolicyRelative();
      bullet.SetCornerRadius(0.5f);
      bullet.SetBorderlineColor(color);
      bullet.SetBorderlineWidth(std::clamp(fontSize / LIST_HOLLOW_BULLET_WIDTH_DIVISOR,
                                           LIST_HOLLOW_BULLET_WIDTH_MIN,
                                           LIST_HOLLOW_BULLET_WIDTH_MAX));
      break;
    case 2u:
      bullet.SetBackgroundColor(color);
      break;
    default:
      bullet.SetBackgroundColor(color);
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
  TextBlockComponent(const MarkdownRenderNode& node, const MarkdownViewStyle& style)
  {
    mTextComponent = CreateMarkdownLabelTextComponent(style);
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
  ListItemComponent(const MarkdownRenderNode&                   node,
                    const MarkdownViewStyle&                    style,
                    const MarkdownTaskSelectionChangedCallback& taskSelectionChanged)
  : mStyle(style),
    mTaskSelectionChanged(taskSelectionChanged)
  {
    mItem = NewStack(StackOrientation::HORIZONTAL, 0.0f);
    ApplyItemMargin(node);
    mListKind               = node.listKind;
    mListOrdinal            = node.listOrdinal;
    mListDepth              = node.listDepth;
    mListMarkerColumnLength = node.listMarkerColumnLength;
    mTaskListItem           = node.taskListItem;
    mTaskChecked            = node.taskChecked;
    mTaskMarkerOffset       = node.taskMarkerOffset;

    mMarkerHost = NewStack(StackOrientation::HORIZONTAL, 0.0f);
    mMarkerHost.SetMargin(Extents(0, static_cast<int16_t>(LIST_CONTENT_GAP), 0, 0));
    mMarkerHost.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));

    mMarkerLabel = NewLabel(Dali::String(),
                            mStyle.GetTextFontFamily(),
                            mStyle.GetTextFontSize(),
                            mStyle.GetTextColor());
    mMarkerLabel.SetMultiLine(false);
    mMarkerLabel.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
    mMarkerLabel.SetLineHeight(MarkdownViewDefaults::BODY_LINE_HEIGHT_RATIO);
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

    mTaskListItem     = current.taskListItem;
    mTaskChecked      = current.taskChecked;
    mTaskMarkerOffset = current.taskMarkerOffset;
    ApplyTaskPresentation(current);
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
    const float textFontSize = mStyle.GetTextFontSize();

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

    const bool  useBullet         = UseBulletMarker(node);
    const bool  useMarkerLabel    = node.listKind == MarkdownListKind::ORDERED;
    const float markerColumnWidth = ListMarkerMinimumWidth(node, textFontSize);
    const float lineHeight        = textFontSize * MarkdownViewDefaults::BODY_LINE_HEIGHT_RATIO;

    float markerHostWidth  = 0.0f;
    float markerHostHeight = lineHeight;
    if(useBullet)
    {
      const float markerSize = textFontSize * UNORDERED_LIST_MARKER_SIZE_RATIO;
      mBulletMarker          = NewBulletMarker(node.listDepth,
                                               textFontSize,
                                               markerSize,
                                               lineHeight,
                                               markerColumnWidth,
                                               mStyle.GetTextColor());
      mMarkerHost.Add(mBulletMarker);
      mBulletMarkerAttached = true;
      markerHostWidth       = std::max(markerColumnWidth, markerSize);
    }

    if(useMarkerLabel)
    {
      mMarkerLabel.SetHorizontalTextAlignment(Text::Alignment::END);
      ApplyMarkerText(node);
      const Vector3 markerNaturalSize = mMarkerLabel.GetNaturalSize();
      const float   markerLabelWidth  = std::max(markerColumnWidth, markerNaturalSize.width);
      mMarkerLabel.SetRequestedWidth(markerLabelWidth);
      mMarkerLabel.SetMargin(Extents());
      mMarkerHost.Add(mMarkerLabel);
      mMarkerLabelAttached = true;
      markerHostWidth      = markerLabelWidth;
      markerHostHeight     = std::max(markerHostHeight, markerNaturalSize.height);
    }
    else
    {
      mMarkerText.clear();
    }

    mMarkerHost.SetRequestedWidth(markerHostWidth);
    mMarkerHost.SetRequestedHeight(markerHostHeight);
  }

  void ApplyTaskPresentation(const MarkdownRenderNode& node)
  {
    if(!node.taskListItem)
    {
      DetachTaskRow();
      return;
    }

    EnsureTaskRow(node);
    mTaskCheckBox.SetAccessibilityName(Dali::String(node.text.c_str()));
    if(mTaskCheckBox.IsSelected() != node.taskChecked)
    {
      mApplyingTaskSelection = true;
      mTaskCheckBox.SetSelected(node.taskChecked);
      mApplyingTaskSelection = false;
    }

    if(mTaskRowAttached)
    {
      return;
    }

    if(mTextComponent)
    {
      Ui::View textView = mTextComponent->GetView();
      mContentHost.Remove(textView, RemovePolicy::IMMEDIATE);
      SetStackWeightFill(textView);
      // The checkbox exposes the task text as its accessibility name. Keep the
      // visual label hidden from accessibility to avoid announcing it twice.
      textView.SetAccessibilityHidden(true);
    }

    // Insert at logical index 0. InsertBelow places a fresh child at the
    // logical (layout) position matching its actor position; an empty anchor
    // means the host is empty, so append.
    Ui::View taskRowAnchor = mContentHost.GetChildViewAt(0u);
    if(taskRowAnchor)
    {
      mContentHost.InsertBelow(mTaskRow, taskRowAnchor);
    }
    else
    {
      mContentHost.Add(mTaskRow);
    }
    mTaskRowAttached = true;
    if(mTextComponent)
    {
      mTaskRow.Add(mTextComponent->GetView());
    }
  }

  void EnsureTaskRow(const MarkdownRenderNode& node)
  {
    if(mTaskRow)
    {
      return;
    }

    const float fontSize   = mStyle.GetTextFontSize();
    const float lineHeight = fontSize * MarkdownViewDefaults::BODY_LINE_HEIGHT_RATIO;

    const MarkdownViewStyleImpl& markdownStyleImpl = GetImpl(mStyle);
    CheckBoxStyle                taskCheckBoxStyle = CheckBoxStyle::Default();

    if(markdownStyleImpl.HasTaskCheckBoxIconGenerator() ||
       markdownStyleImpl.HasTaskCheckBoxIconColor() ||
       markdownStyleImpl.HasTaskCheckBoxSelectedIconColor())
    {
      CheckBoxStyle::Builder taskCheckBoxStyleBuilder = taskCheckBoxStyle.Configure();
      if(markdownStyleImpl.HasTaskCheckBoxIconGenerator())
      {
        // CheckBox invokes the generator synchronously in New(). Binding it to
        // this list item keeps icon injection on the existing style extension point.
        taskCheckBoxStyleBuilder.SetIconGenerator(
          CheckBoxStyle::IconGenerator::New(this, &ListItemComponent::CreateTaskCheckBoxIcon));
      }
      if(markdownStyleImpl.HasTaskCheckBoxIconColor())
      {
        taskCheckBoxStyleBuilder.SetIconColor(markdownStyleImpl.GetTaskCheckBoxIconColor());
      }
      if(markdownStyleImpl.HasTaskCheckBoxSelectedIconColor())
      {
        taskCheckBoxStyleBuilder.SetSelectedIconColor(markdownStyleImpl.GetTaskCheckBoxSelectedIconColor());
      }
      taskCheckBoxStyle = std::move(taskCheckBoxStyleBuilder).Build();
    }

    mTaskRow      = NewStack(StackOrientation::HORIZONTAL, LIST_CONTENT_GAP);
    mTaskCheckBox = Ui::CheckBox::New(taskCheckBoxStyle);
    mTaskCheckBox.SetPadding(Insets());
    mTaskCheckBox.SetMinimumWidth(0.0f);
    mTaskCheckBox.SetMinimumHeight(0.0f);
    mTaskCheckBox.SetIconWidth(fontSize);
    mTaskCheckBox.SetIconHeight(fontSize);
    mTaskCheckBox.SetRequestedWidth(fontSize);
    mTaskCheckBox.SetRequestedHeight(lineHeight);
    mTaskCheckBox.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    mTaskCheckBox.SetAccessibilityName(Dali::String(node.text.c_str()));
    mTaskCheckBox.SetSelected(node.taskChecked);
    mTaskCheckBox.SelectionChangedSignal().Connect(this, &ListItemComponent::OnTaskSelectionChanged);
    mTaskRow.Add(mTaskCheckBox);
  }

  SelectableImageInterface CreateTaskCheckBoxIcon()
  {
    return GetImpl(mStyle).CreateTaskCheckBoxIcon();
  }

  void OnTaskSelectionChanged(View, bool selected, InputEvent)
  {
    if(mApplyingTaskSelection || !mTaskListItem || !mTaskSelectionChanged)
    {
      return;
    }

    const bool sourceBackedSelection = mTaskChecked;
    mTaskChecked                     = selected;
    if(mTaskSelectionChanged(mTaskMarkerOffset, selected))
    {
      return;
    }

    // A stale or invalid source binding must not leave the visual state out of
    // sync with GetMarkdown(). Restore the last source-backed state immediately.
    mTaskChecked           = sourceBackedSelection;
    mApplyingTaskSelection = true;
    mTaskCheckBox.SetSelected(mTaskChecked);
    mApplyingTaskSelection = false;
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
      textView.SetAccessibilityHidden(false);
    }

    mContentHost.Remove(mTaskRow, RemovePolicy::IMMEDIATE);
    mTaskRowAttached = false;
    if(mTextComponent)
    {
      // Insert at logical index 0. InsertBelow places a fresh child at the
      // logical (layout) position matching its actor position; an empty anchor
      // means the host is empty, so append.
      Ui::View textView = mTextComponent->GetView();
      Ui::View anchor   = mContentHost.GetChildViewAt(0u);
      if(anchor)
      {
        mContentHost.InsertBelow(textView, anchor);
      }
      else
      {
        mContentHost.Add(textView);
      }
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
                                                  static_cast<int16_t>(MarkdownViewDefaults::LIST_ITEM_MARGIN_BOTTOM)));
    }
  }

  void ApplyMarkerText(const MarkdownRenderNode& node)
  {
    const std::string marker = ListMarkerText(node, mIsRtl);
    if(marker == mMarkerText)
    {
      return;
    }

    mMarkerLabel.SetText(Dali::String(marker.c_str()));
    mMarkerText = marker;
  }

  void UpdateTightText(const MarkdownRenderNode* previous, const MarkdownRenderNode& current, const MarkdownTextUpdate& textUpdate)
  {
    if(!HasRenderableText(current))
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
      mTextComponent = CreateMarkdownLabelTextComponent(mStyle);
      ApplyTextMargin();
      mTextComponent->SetTextContent(current);
      if(mTaskRowAttached)
      {
        SetStackWeightFill(mTextComponent->GetView());
        mTextComponent->GetView().SetAccessibilityHidden(true);
        mTaskRow.Add(mTextComponent->GetView());
      }
      else
      {
        // Insert at logical index 0. InsertBelow places a fresh child at the
        // logical (layout) position matching its actor position; an empty
        // anchor means the host is empty, so append.
        Ui::View textView = mTextComponent->GetView();
        Ui::View anchor   = mContentHost.GetChildViewAt(0u);
        if(anchor)
        {
          mContentHost.InsertBelow(textView, anchor);
        }
        else
        {
          mContentHost.Add(textView);
        }
      }
      return;
    }

    // List marker and task selection changes are component attributes, not
    // text changes. Avoid rebuilding an unchanged label when only those change.
    if(previous && previous->contentHash == current.contentHash)
    {
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
  MarkdownViewStyle                      mStyle;
  Ui::StackLayout                        mItem;
  Ui::StackLayout                        mMarkerHost;
  Ui::Label                              mMarkerLabel;
  Ui::View                               mBulletMarker;
  Ui::StackLayout                        mTaskRow;
  Ui::CheckBox                           mTaskCheckBox;
  std::string                            mMarkerText;
  MarkdownListKind                       mListKind{MarkdownListKind::NONE};
  uint32_t                               mListOrdinal{0u};
  uint32_t                               mListDepth{0u};
  uint32_t                               mListMarkerColumnLength{0u};
  uint32_t                               mTaskMarkerOffset{MARKDOWN_INVALID_SOURCE_OFFSET};
  bool                                   mTaskListItem{false};
  bool                                   mTaskChecked{false};
  bool                                   mApplyingTaskSelection{false};
  bool                                   mIsRtl{false};
  bool                                   mMarkerLabelAttached{false};
  bool                                   mBulletMarkerAttached{false};
  bool                                   mTaskRowAttached{false};
  MarkdownTaskSelectionChangedCallback   mTaskSelectionChanged;
  std::unique_ptr<MarkdownTextComponent> mTextComponent;
};

/**
 * @brief Renders a Markdown block quote container.
 */
class QuoteComponent : public BaseComponent
{
public:
  explicit QuoteComponent(const MarkdownViewStyle& style)
  {
    Ui::StackLayout quote = NewStack(StackOrientation::HORIZONTAL, 0.0f);
    quote.SetPadding(Extents(static_cast<int16_t>(MarkdownViewDefaults::QUOTE_PADDING),
                             static_cast<int16_t>(MarkdownViewDefaults::QUOTE_PADDING),
                             static_cast<int16_t>(MarkdownViewDefaults::QUOTE_PADDING),
                             static_cast<int16_t>(MarkdownViewDefaults::QUOTE_PADDING)));

    Ui::View decoration = NewColorBox(style.GetQuoteBarColor(), MarkdownViewDefaults::QUOTE_BAR_CORNER_RADIUS);
    decoration.SetRequestedWidth(MarkdownViewDefaults::QUOTE_BAR_WIDTH);
    decoration.SetMargin(Extents(0, static_cast<int16_t>(QUOTE_CONTENT_GAP), 0, 0));
    decoration.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));

    mContentHost = NewStack(StackOrientation::VERTICAL, MarkdownViewDefaults::BLOCK_SPACING);
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
  CodeBlockComponent(const MarkdownRenderNode& node, const MarkdownViewStyle& style)
  : mStyle(style)
  {
    mCodeRoot = NewStack(StackOrientation::VERTICAL, 0.0f);
    mCodeRoot.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
    mCodeRoot.SetBackgroundColor(style.GetCodeBlockBackgroundColor());
    mCodeRoot.SetCornerRadius(MarkdownViewDefaults::CODE_CORNER_RADIUS);

    mLanguage = node.language;
    if(!mLanguage.empty())
    {
      EnsureLanguageHost();
    }

    mCodeContentHost = NewStack(StackOrientation::VERTICAL, 0.0f);
    mCodeContentHost.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
    mCodeContentHost.SetPadding(Extents(static_cast<int16_t>(MarkdownViewDefaults::CODE_PADDING),
                                        static_cast<int16_t>(MarkdownViewDefaults::CODE_PADDING),
                                        static_cast<int16_t>(MarkdownViewDefaults::CODE_PADDING),
                                        static_cast<int16_t>(MarkdownViewDefaults::CODE_PADDING)));

    mTextComponent = CreateMarkdownLabelTextComponent(style);
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
      mLanguage = current.language;
      if(!mLanguage.empty())
      {
        EnsureLanguageHost();
        mLanguageLabel.SetText(Dali::String(mLanguage.c_str()));
      }
      RebuildChildren();
    }
    if(textUpdate.type != MarkdownTextUpdate::Type::UNCHANGED)
    {
      mTextComponent->UpdateTextContent(current, textUpdate);
    }
  }

private:
  void EnsureLanguageHost()
  {
    if(mLanguageHost)
    {
      return;
    }

    mLanguageHost = NewStack(StackOrientation::HORIZONTAL, 0.0f);
    mLanguageHost.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
    mLanguageHost.SetPadding(Extents(static_cast<int16_t>(MarkdownViewDefaults::CODE_PADDING),
                                     static_cast<int16_t>(MarkdownViewDefaults::CODE_PADDING),
                                     static_cast<int16_t>(MarkdownViewDefaults::CODE_PADDING),
                                     static_cast<int16_t>(MarkdownViewDefaults::CODE_PADDING)));
    mLanguageHost.SetBackgroundColor(mStyle.GetCodeBlockTitleBackgroundColor());
    mLanguageHost.SetCornerRadius(MarkdownViewDefaults::CODE_CORNER_RADIUS,
                                  MarkdownViewDefaults::CODE_CORNER_RADIUS,
                                  0.0f,
                                  0.0f);

    mLanguageLabel = NewLabel(Dali::String(mLanguage.c_str()),
                              mStyle.GetCodeFontFamily(),
                              mStyle.GetCodeBlockTitleFontSize(),
                              mStyle.GetCodeBlockTitleTextColor());
    mLanguageLabel.SetMultiLine(false);
    mLanguageLabel.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
    mLanguageLabel.SetRequestedWidth(WRAP_CONTENT);
    mLanguageLabel.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    mLanguageHost.Add(mLanguageLabel);
  }

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
  MarkdownViewStyle                      mStyle;
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
  explicit TableHeadComponent(const MarkdownViewStyle& style)
  {
    Ui::StackLayout root = NewStack(StackOrientation::VERTICAL, 0.0f);
    mContentHost         = NewStack(StackOrientation::VERTICAL, 0.0f);

    Ui::View rule = NewColorBox(style.GetTableRuleColor());
    rule.SetRequestedHeight(MarkdownViewDefaults::TABLE_RULE_HEIGHT);

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
  TableCellComponent(const MarkdownRenderNode& node, const MarkdownViewStyle& style)
  : mStyle(style)
  {
    Ui::StackLayout cell = NewStack(StackOrientation::VERTICAL, MarkdownViewDefaults::BLOCK_SPACING);
    cell.SetPadding(Extents(static_cast<int16_t>(MarkdownViewDefaults::TABLE_CELL_PADDING),
                            static_cast<int16_t>(MarkdownViewDefaults::TABLE_CELL_PADDING),
                            static_cast<int16_t>(MarkdownViewDefaults::TABLE_CELL_PADDING),
                            static_cast<int16_t>(MarkdownViewDefaults::TABLE_CELL_PADDING)));
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
    if(!HasRenderableText(current))
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
      mTextComponent = CreateMarkdownLabelTextComponent(mStyle);
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
  MarkdownViewStyle                      mStyle;
  std::unique_ptr<MarkdownTextComponent> mTextComponent;
  Text::Alignment                        mAlignment{Text::Alignment::START};
};

/**
 * @brief Renders a Markdown block image or its fallback text.
 */
class ImageComponent : public BaseComponent
{
public:
  ImageComponent(const MarkdownRenderNode& node, const MarkdownViewStyle& style)
  : mStyle(style)
  {
    mRoot = NewStack(StackOrientation::VERTICAL, 0.0f);
    Update(nullptr, node, MarkdownTextUpdate());
  }

  Ui::View GetContentHost() const override
  {
    return Ui::View();
  }

  void Update(const MarkdownRenderNode* previous, const MarkdownRenderNode& current, const MarkdownTextUpdate& textUpdate) override
  {
    if(DEFAULT_IMAGE_PRESENTATION == MarkdownImagePresentation::TEXT_WITH_URL)
    {
      MarkdownRenderNode fallback = current;
      fallback.role               = MarkdownRenderRole::PARAGRAPH;

      if(!mTextComponent)
      {
        mTextComponent = CreateMarkdownLabelTextComponent(mStyle);
        mTextComponent->SetTextContent(fallback);
        mRenderedChild = mTextComponent->GetView();
        mRoot.Add(mRenderedChild);
      }
      else if(!previous || textUpdate.type != MarkdownTextUpdate::Type::UNCHANGED)
      {
        mTextComponent->UpdateTextContent(fallback, textUpdate);
      }
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
      fallback.inlineObjects.clear();
      mTextComponent = CreateMarkdownLabelTextComponent(mStyle);
      mTextComponent->SetTextContent(fallback);
      mRenderedChild = mTextComponent->GetView();
    }

    if(mRenderedChild)
    {
      mRoot.Add(mRenderedChild);
    }
  }

private:
  MarkdownViewStyle                      mStyle;
  Ui::View                               mRenderedChild;
  std::unique_ptr<MarkdownTextComponent> mTextComponent;
};

/**
 * @brief Renders a Markdown thematic break.
 */
class ThematicBreakComponent : public BaseComponent
{
public:
  explicit ThematicBreakComponent(const MarkdownViewStyle& style)
  {
    Ui::View rule = NewColorBox(style.GetThematicBreakColor());
    rule.SetRequestedHeight(MarkdownViewDefaults::THEMATIC_BREAK_HEIGHT);
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

std::unique_ptr<MarkdownComponent> CreateMarkdownComponent(const MarkdownRenderNode&                   node,
                                                           const MarkdownViewStyle&                    style,
                                                           const MarkdownTaskSelectionChangedCallback& taskSelectionChanged)
{
  switch(node.role)
  {
    case MarkdownRenderRole::LIST:
      return std::unique_ptr<MarkdownComponent>(new StackComponent(StackOrientation::VERTICAL, 0.0f));
    case MarkdownRenderRole::LIST_ITEM:
      return std::unique_ptr<MarkdownComponent>(new ListItemComponent(node, style, taskSelectionChanged));
    case MarkdownRenderRole::QUOTE:
      return std::unique_ptr<MarkdownComponent>(new QuoteComponent(style));
    case MarkdownRenderRole::CODE_BLOCK:
      return std::unique_ptr<MarkdownComponent>(new CodeBlockComponent(node, style));
    case MarkdownRenderRole::TABLE:
    case MarkdownRenderRole::TABLE_BODY:
      return std::unique_ptr<MarkdownComponent>(new StackComponent(StackOrientation::VERTICAL, 0.0f));
    case MarkdownRenderRole::TABLE_HEAD:
      return std::unique_ptr<MarkdownComponent>(new TableHeadComponent(style));
    case MarkdownRenderRole::TABLE_ROW:
      return std::unique_ptr<MarkdownComponent>(new StackComponent(StackOrientation::HORIZONTAL, 0.0f));
    case MarkdownRenderRole::TABLE_CELL:
      return std::unique_ptr<MarkdownComponent>(new TableCellComponent(node, style));
    case MarkdownRenderRole::BLOCK_IMAGE:
      return std::unique_ptr<MarkdownComponent>(new ImageComponent(node, style));
    case MarkdownRenderRole::THEMATIC_BREAK:
      return std::unique_ptr<MarkdownComponent>(new ThematicBreakComponent(style));
    default:
      if(MarkdownIsTextRole(node.role))
      {
        return std::unique_ptr<MarkdownComponent>(new TextBlockComponent(node, style));
      }
      return std::unique_ptr<MarkdownComponent>(new StackComponent(StackOrientation::VERTICAL, MarkdownViewDefaults::BLOCK_SPACING));
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
