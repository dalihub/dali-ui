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

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-components/internal/markdown/markdown-parser.h>
#include <dali-ui-components/internal/markdown/markdown-view-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/text/styled-text/anchor-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/background-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/font-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/line-through-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/image/selectable-lottie-animation-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

namespace
{

bool gUseAlternateMarkdownColors       = false;
int  gMarkdownStyleOverrideCreateCount = 0;
int  gTaskCheckBoxIconCreateCount      = 0;

} // namespace

void utc_dali_markdown_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_markdown_view_cleanup(void)
{
  gUseAlternateMarkdownColors       = false;
  gMarkdownStyleOverrideCreateCount = 0;
  gTaskCheckBoxIconCreateCount      = 0;
  test_return_value = TET_PASS;
}

namespace
{

UiStyle CreateMarkdownStyleOverride()
{
  ++gMarkdownStyleOverrideCreateCount;
  return MarkdownViewStyle::Builder()
    .SetTextFontSize(31.0f)
    .SetTextColor(UiColor("MarkdownDefaultText"))
    .Build();
}

SelectableImageInterface CreateTaskCheckBoxIcon()
{
  ++gTaskCheckBoxIconCreateCount;
  using FrameRange = SelectableLottieImage::FrameRange;
  return SelectableLottieAnimationView::New(
    SelectableLottieImage("i_check_box.json", FrameRange(0, 0), FrameRange(0, 0)));
}

SelectableImageInterface CreateEmptyTaskCheckBoxIcon()
{
  return SelectableImageInterface();
}

bool OverrideMarkdownColors(StringView colorId, Vector4& outColor)
{
  if(colorId == "MarkdownBody")
  {
    outColor = gUseAlternateMarkdownColors ? Color::GREEN : Color::RED;
    return true;
  }
  if(colorId == "MarkdownCodeBackground")
  {
    outColor = gUseAlternateMarkdownColors ? Color::YELLOW : Color::BLUE;
    return true;
  }
  if(colorId == "MarkdownQuoteBar")
  {
    outColor = gUseAlternateMarkdownColors ? Color::CYAN : Color::MAGENTA;
    return true;
  }
  return false;
}

class ScopedMarkdownColorOverride
{
public:
  ScopedMarkdownColorOverride(UiColorManager manager, ColorOverrideFunc overrideFunction)
  : mManager(manager)
  {
    mManager.SetColorOverride(overrideFunction);
  }

  ~ScopedMarkdownColorOverride()
  {
    mManager.ClearColorOverride();
    gUseAlternateMarkdownColors = false;
  }

  ScopedMarkdownColorOverride(const ScopedMarkdownColorOverride&)            = delete;
  ScopedMarkdownColorOverride& operator=(const ScopedMarkdownColorOverride&) = delete;

private:
  UiColorManager mManager;
};

const MarkdownRenderNode* FindFirstRole(const MarkdownRenderSnapshot& snapshot, MarkdownRenderRole role)
{
  for(const auto& node : snapshot.nodes)
  {
    if(node.role == role)
    {
      return &node;
    }
  }
  return nullptr;
}

const MarkdownRenderNode* FindText(const MarkdownRenderSnapshot& snapshot, const std::string& text)
{
  for(const auto& node : snapshot.nodes)
  {
    if(node.text == text)
    {
      return &node;
    }
  }
  return nullptr;
}

uint32_t CountRole(const MarkdownRenderSnapshot& snapshot, MarkdownRenderRole role)
{
  uint32_t count = 0u;
  for(const auto& node : snapshot.nodes)
  {
    if(node.role == role)
    {
      ++count;
    }
  }
  return count;
}

uint32_t CountTaskListItems(const MarkdownRenderSnapshot& snapshot)
{
  uint32_t count = 0u;
  for(const auto& node : snapshot.nodes)
  {
    if(node.taskListItem)
    {
      ++count;
    }
  }
  return count;
}

uint32_t CountAutolinks(const MarkdownRenderSnapshot& snapshot)
{
  uint32_t count = 0u;
  for(const auto& node : snapshot.nodes)
  {
    for(const auto& link : node.linkRanges)
    {
      if(link.isAutolink)
      {
        ++count;
      }
    }
  }
  return count;
}

uint32_t CountStyleFlag(const MarkdownRenderSnapshot& snapshot, uint32_t flag)
{
  uint32_t count = 0u;
  for(const auto& node : snapshot.nodes)
  {
    for(const auto& styleRun : node.styleRuns)
    {
      if((styleRun.flags & flag) != 0u)
      {
        ++count;
      }
    }
  }
  return count;
}

uint32_t CountZeroLengthLinks(const MarkdownRenderSnapshot& snapshot)
{
  uint32_t count = 0u;
  for(const auto& node : snapshot.nodes)
  {
    for(const auto& link : node.linkRanges)
    {
      if(link.start == link.end)
      {
        ++count;
      }
    }
  }
  return count;
}

uint32_t CountZeroLengthInlineObjects(const MarkdownRenderSnapshot& snapshot)
{
  uint32_t count = 0u;
  for(const auto& node : snapshot.nodes)
  {
    for(const auto& object : node.inlineObjects)
    {
      if(object.length == 0u)
      {
        ++count;
      }
    }
  }
  return count;
}

bool HasPlainTextLineBreakMetadata(const MarkdownRenderSnapshot& snapshot)
{
  return !snapshot.plainTextLineBreaks.empty();
}

void CheckPlainTextLineBreakEvents(const MarkdownRenderSnapshot& snapshot)
{
  for(const auto& event : snapshot.plainTextLineBreaks)
  {
    DALI_TEST_CHECK(event.nodeIndex < snapshot.nodes.size());
    if(event.nodeIndex < snapshot.nodes.size())
    {
      DALI_TEST_CHECK(event.position <= snapshot.nodes[event.nodeIndex].utf32Length);
    }
  }
}

void CheckPlainText(const char* markdown, const char* expected)
{
  DALI_TEST_EQUALS(MarkdownView::ToPlainText(Dali::String(markdown)), expected, TEST_LOCATION);
}

void CollectLabels(Ui::View view, std::vector<Label>& labels)
{
  Label label = Label::DownCast(view);
  if(label)
  {
    labels.push_back(label);
  }

  for(uint32_t index = 0u; index < view.GetChildViewCount(); ++index)
  {
    CollectLabels(view.GetChildViewAt(index), labels);
  }
}

View GetListItemAtDepth(MarkdownView view, uint32_t depth)
{
  if(depth == 0u || view.GetChildViewCount() == 0u)
  {
    return View();
  }

  View list = view.GetChildViewAt(0u);
  for(uint32_t currentDepth = 1u; currentDepth <= depth; ++currentDepth)
  {
    if(!list || list.GetChildViewCount() == 0u)
    {
      return View();
    }

    View listItem = list.GetChildViewAt(0u);
    if(currentDepth == depth)
    {
      return listItem;
    }
    if(!listItem || listItem.GetChildViewCount() < 2u)
    {
      return View();
    }

    View contentHost = listItem.GetChildViewAt(1u);
    if(!contentHost || contentHost.GetChildViewCount() == 0u)
    {
      return View();
    }
    list = contentHost.GetChildViewAt(contentHost.GetChildViewCount() - 1u);
  }
  return View();
}

View GetListItemMarkerHost(View listItem)
{
  return listItem && listItem.GetChildViewCount() > 0u ? listItem.GetChildViewAt(0u) : View();
}

View GetShapeMarker(View listItem)
{
  View markerHost = GetListItemMarkerHost(listItem);
  return markerHost && markerHost.GetChildViewCount() > 0u ? markerHost.GetChildViewAt(0u) : View();
}

View GetListItemTaskRow(View listItem)
{
  if(!listItem || listItem.GetChildViewCount() < 2u)
  {
    return View();
  }

  View contentHost = listItem.GetChildViewAt(1u);
  if(!contentHost || contentHost.GetChildViewCount() == 0u)
  {
    return View();
  }

  View firstChild = contentHost.GetChildViewAt(0u);
  return firstChild && !Label::DownCast(firstChild) ? firstChild : View();
}

CheckBox GetListItemTaskCheckBox(View listItem)
{
  View taskRow = GetListItemTaskRow(listItem);
  return taskRow && taskRow.GetChildViewCount() > 0u
           ? CheckBox::DownCast(taskRow.GetChildViewAt(0u))
           : CheckBox();
}

View GetCheckBoxIconView(CheckBox checkBox)
{
  for(uint32_t index = 0u; checkBox && index < checkBox.GetChildViewCount(); ++index)
  {
    View child = checkBox.GetChildViewAt(index);
    if(child && !Label::DownCast(child))
    {
      return child;
    }
  }
  return View();
}

Label GetListItemTextLabel(View listItem)
{
  if(!listItem || listItem.GetChildViewCount() < 2u)
  {
    return Label();
  }

  View contentHost = listItem.GetChildViewAt(1u);
  if(!contentHost || contentHost.GetChildViewCount() == 0u)
  {
    return Label();
  }

  View  firstChild = contentHost.GetChildViewAt(0u);
  Label textLabel  = Label::DownCast(firstChild);
  if(textLabel)
  {
    return textLabel;
  }
  return firstChild && firstChild.GetChildViewCount() > 1u
           ? Label::DownCast(firstChild.GetChildViewAt(1u))
           : Label();
}

void ArrangeMarkdownView(MarkdownView view, float width)
{
  const MeasuredSize measured = view.Measure(width, 1000.0f);
  view.Arrange(LayoutRect(0.0f, 0.0f, width, measured.GetHeight()));
}

void CheckShapeMarkerFirstLineAlignment(View listItem, float lineHeight)
{
  View  markerHost = GetListItemMarkerHost(listItem);
  View  marker     = GetShapeMarker(listItem);
  Label textLabel  = GetListItemTextLabel(listItem);
  DALI_TEST_CHECK(markerHost);
  DALI_TEST_CHECK(marker);
  DALI_TEST_CHECK(textLabel);
  if(!markerHost || !marker || !textLabel)
  {
    return;
  }

  StackLayoutParams markerHostParams;
  DALI_TEST_CHECK(markerHost.TryGetLayoutParams(markerHostParams));
  DALI_TEST_CHECK(markerHostParams.GetAlignment() == LayoutAlignment::START);
  DALI_TEST_EQUALS(markerHost.GetRequestedHeight(), lineHeight, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(markerHost.GetSize().height, lineHeight, 0.01f, TEST_LOCATION);

  View        contentHost      = listItem.GetChildViewAt(1u);
  View        firstLineHost    = contentHost.GetChildViewAt(0u);
  const float markerCenterY    = markerHost.GetPositionY() +
                              marker.GetPositionY() +
                              marker.GetSize().height * 0.5f;
  const float textOffsetY      = Label::DownCast(firstLineHost)
                                   ? textLabel.GetPositionY()
                                   : firstLineHost.GetPositionY() + textLabel.GetPositionY();
  const float firstLineCenterY = contentHost.GetPositionY() + textOffsetY + lineHeight * 0.5f;
  DALI_TEST_EQUALS(markerCenterY, firstLineCenterY, 0.01f, TEST_LOCATION);
}

void CheckDefaultUnorderedMarkerGeometry(View listItem)
{
  View markerHost = GetListItemMarkerHost(listItem);
  View marker     = GetShapeMarker(listItem);
  DALI_TEST_CHECK(markerHost);
  DALI_TEST_CHECK(marker);
  if(!markerHost || !marker)
  {
    return;
  }

  DALI_TEST_EQUALS(markerHost.GetRequestedWidth(), 32.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(markerHost.GetRequestedHeight(), 32.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(markerHost.GetMargin().end, 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(marker.GetRequestedWidth(), 6.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(marker.GetRequestedHeight(), 6.0f, TEST_LOCATION);

  const Insets margin = marker.GetMargin();
  DALI_TEST_EQUALS(margin.start, 26.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(margin.end, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(margin.top, 13.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(margin.bottom, 13.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(margin.top + marker.GetRequestedHeight() + margin.bottom, 32.0f, TEST_LOCATION);
}

void CheckResponsiveListMarkerGeometry(float fontSize)
{
  constexpr float BODY_LINE_HEIGHT_RATIO = 1.6f;
  constexpr float BULLET_SIZE_RATIO      = 0.3f;
  constexpr float ORDERED_DIGIT_RATIO    = 0.55f;

  const float lineHeight        = fontSize * BODY_LINE_HEIGHT_RATIO;
  const float markerColumnWidth = std::max(32.0f, lineHeight);
  MarkdownViewStyle style = MarkdownViewStyle::Builder()
                              .SetTextFontFamily("SamsungOneUI_400")
                              .SetTextFontSize(fontSize)
                              .Build();

  MarkdownView unordered = MarkdownView::New(style);
  unordered.SetMarkdown("- bullet");
  ArrangeMarkdownView(unordered, 1000.0f);
  View  unorderedItem = GetListItemAtDepth(unordered, 1u);
  View  markerHost    = GetListItemMarkerHost(unorderedItem);
  View  bullet        = GetShapeMarker(unorderedItem);
  Label textLabel     = GetListItemTextLabel(unorderedItem);
  DALI_TEST_EQUALS(markerHost.GetRequestedWidth(), markerColumnWidth, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(markerHost.GetRequestedHeight(), lineHeight, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(bullet.GetRequestedWidth(), fontSize * BULLET_SIZE_RATIO, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(bullet.GetRequestedHeight(), fontSize * BULLET_SIZE_RATIO, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(bullet.GetMargin().start + bullet.GetRequestedWidth(), markerColumnWidth, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(bullet.GetMargin().top + bullet.GetRequestedHeight() + bullet.GetMargin().bottom, lineHeight, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(textLabel.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(textLabel.GetLineHeight(), BODY_LINE_HEIGHT_RATIO, TEST_LOCATION);
  DALI_TEST_CHECK(!textLabel.IsSystemFontSizeScaleEnabled());
  CheckShapeMarkerFirstLineAlignment(unorderedItem, lineHeight);

  unordered.SetMarkdown("- bullet\n  - nested");
  View hollowBullet = GetShapeMarker(GetListItemAtDepth(unordered, 2u));
  DALI_TEST_EQUALS(hollowBullet.GetBorderlineWidth(), std::clamp(fontSize / 16.0f, 1.0f, 2.5f), 0.01f, TEST_LOCATION);

  MarkdownView ordered = MarkdownView::New(style);
  ordered.SetMarkdown("100. ordered");
  ArrangeMarkdownView(ordered, 1000.0f);
  View  orderedItem   = GetListItemAtDepth(ordered, 1u);
  View  orderedHost   = GetListItemMarkerHost(orderedItem);
  Label orderedMarker = Label::DownCast(GetShapeMarker(orderedItem));
  DALI_TEST_CHECK(orderedMarker);
  const Vector3 orderedNaturalSize  = orderedMarker.GetNaturalSize();
  const float   orderedMinimumWidth = std::max(markerColumnWidth, 4.0f * fontSize * ORDERED_DIGIT_RATIO);
  const float   orderedWidth        = std::max(orderedMinimumWidth, orderedNaturalSize.width);
  const float   orderedHeight       = std::max(lineHeight, orderedNaturalSize.height);
  DALI_TEST_EQUALS(orderedMarker.GetRequestedWidth(), orderedWidth, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(orderedHost.GetRequestedWidth(), orderedWidth, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(orderedHost.GetRequestedHeight(), orderedHeight, 0.01f, TEST_LOCATION);
  DALI_TEST_CHECK(orderedMarker.GetSize().width + 0.01f >= orderedNaturalSize.width);
  DALI_TEST_CHECK(orderedMarker.GetSize().height + 0.01f >= orderedNaturalSize.height);
  DALI_TEST_CHECK(orderedHost.GetSize().width + 0.01f >= orderedNaturalSize.width);
  DALI_TEST_CHECK(orderedHost.GetSize().height + 0.01f >= orderedNaturalSize.height);
  DALI_TEST_EQUALS(orderedMarker.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(orderedMarker.GetLineHeight(), BODY_LINE_HEIGHT_RATIO, TEST_LOCATION);
  DALI_TEST_CHECK(!orderedMarker.IsSystemFontSizeScaleEnabled());

  MarkdownView task = MarkdownView::New(style);
  task.SetMarkdown("- [x] task");
  ArrangeMarkdownView(task, 1000.0f);
  View     taskItem     = GetListItemAtDepth(task, 1u);
  View     taskHost     = GetListItemMarkerHost(taskItem);
  View     taskBullet   = GetShapeMarker(taskItem);
  View     taskRow      = GetListItemTaskRow(taskItem);
  CheckBox taskCheckBox = GetListItemTaskCheckBox(taskItem);
  Label    taskText     = GetListItemTextLabel(taskItem);
  DALI_TEST_CHECK(taskBullet);
  DALI_TEST_CHECK(!Label::DownCast(taskBullet));
  DALI_TEST_CHECK(taskRow);
  DALI_TEST_CHECK(taskCheckBox);
  DALI_TEST_CHECK(taskText);
  DALI_TEST_EQUALS(taskBullet.GetMargin().start + taskBullet.GetRequestedWidth(), markerColumnWidth, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskCheckBox.GetIconWidth(), fontSize, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskCheckBox.GetIconHeight(), fontSize, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskCheckBox.GetRequestedWidth(), fontSize, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskCheckBox.GetRequestedHeight(), lineHeight, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskCheckBox.GetPadding(), Insets(), TEST_LOCATION);
  DALI_TEST_EQUALS(taskHost.GetRequestedWidth(), markerColumnWidth, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskHost.GetRequestedHeight(), lineHeight, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskCheckBox.GetSize().width, fontSize, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskCheckBox.GetSize().height, lineHeight, 0.01f, TEST_LOCATION);
  DALI_TEST_CHECK(taskCheckBox.IsSelected());
  DALI_TEST_EQUALS(taskCheckBox.GetAccessibilityName(), Dali::String("task"), TEST_LOCATION);
  View taskContentHost      = taskItem.GetChildViewAt(1u);
  View unorderedContentHost = unorderedItem.GetChildViewAt(1u);
  DALI_TEST_EQUALS(taskContentHost.GetPositionX(), unorderedContentHost.GetPositionX(), 0.01f, TEST_LOCATION);
  DALI_TEST_CHECK(taskCheckBox.GetPositionX() + taskCheckBox.GetSize().width + 12.0f <= taskText.GetPositionX() + 0.01f);
  CheckShapeMarkerFirstLineAlignment(taskItem, lineHeight);

  MarkdownView normalNested = MarkdownView::New(style);
  normalNested.SetMarkdown("- parent\n  1. nested");
  ArrangeMarkdownView(normalNested, 1000.0f);
  MarkdownView taskNested = MarkdownView::New(style);
  taskNested.SetMarkdown("- [ ] parent\n  1. [x] nested");
  ArrangeMarkdownView(taskNested, 1000.0f);
  View normalParentContent = GetListItemAtDepth(normalNested, 1u).GetChildViewAt(1u);
  View taskParentContent   = GetListItemAtDepth(taskNested, 1u).GetChildViewAt(1u);
  DALI_TEST_EQUALS(normalParentContent.GetPositionX(), taskParentContent.GetPositionX(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(normalParentContent.GetChildViewAt(1u).GetPositionX(),
                   taskParentContent.GetChildViewAt(1u).GetPositionX(),
                   0.01f,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(GetListItemMarkerHost(GetListItemAtDepth(normalNested, 2u)).GetRequestedWidth(),
                   GetListItemMarkerHost(GetListItemAtDepth(taskNested, 2u)).GetRequestedWidth(),
                   0.01f,
                   TEST_LOCATION);
}

Label GetOnlyRenderedLabel(MarkdownView view)
{
  std::vector<Label> labels;
  CollectLabels(view, labels);
  DALI_TEST_EQUALS(labels.size(), 1u, TEST_LOCATION);
  return labels.size() == 1u ? labels[0u] : Label();
}

void CheckNoAnchorSpans(const Text::StyledText& styledText)
{
  for(uint32_t index = 0u; index < styledText.GetSpanCount(); ++index)
  {
    DALI_TEST_CHECK(!Text::AnchorSpan::DownCast(styledText.GetSpanAt(index)));
  }
}

void CheckNoUnderlineSpans(const Text::StyledText& styledText)
{
  for(uint32_t index = 0u; index < styledText.GetSpanCount(); ++index)
  {
    DALI_TEST_CHECK(!Text::UnderlineSpan::DownCast(styledText.GetSpanAt(index)));
  }
}

Text::StyledText CheckStyledLinkPresentation(Label label)
{
  const Text::StyledText styledText = label.GetStyledText();
  DALI_TEST_CHECK(styledText);
  if(styledText)
  {
    CheckNoAnchorSpans(styledText);
    CheckNoUnderlineSpans(styledText);
  }
  return styledText;
}

void CheckRenderedUrlText(MarkdownView view, const char* markdown, const char* expected)
{
  view.SetMarkdown(markdown);
  Label label = GetOnlyRenderedLabel(view);
  DALI_TEST_CHECK(label);
  if(label)
  {
    DALI_TEST_EQUALS(label.GetText(), expected, TEST_LOCATION);
    const Text::StyledText styledText = label.GetStyledText();
    if(styledText)
    {
      CheckNoAnchorSpans(styledText);
      CheckNoUnderlineSpans(styledText);
    }
  }
  CheckPlainText(markdown, expected);
}

bool HasLabelText(const std::vector<Label>& labels, const char* expected)
{
  for(const auto& label : labels)
  {
    if(label.GetText() == expected)
    {
      return true;
    }
  }
  return false;
}

Label FindLabel(Ui::View view, const char* expected)
{
  Label label = Label::DownCast(view);
  if(label && label.GetText() == expected)
  {
    return label;
  }

  for(uint32_t index = 0u; index < view.GetChildViewCount(); ++index)
  {
    Label found = FindLabel(view.GetChildViewAt(index), expected);
    if(found)
    {
      return found;
    }
  }
  return Label();
}

Text::ForegroundColorSpan FindForegroundSpan(const Text::StyledText& styledText, uint32_t start, uint32_t end)
{
  for(uint32_t index = 0u; index < styledText.GetSpanCount(); ++index)
  {
    Text::ForegroundColorSpan span = Text::ForegroundColorSpan::DownCast(styledText.GetSpanAt(index));
    if(span && styledText.GetSpanStartIndexAt(index) == start && styledText.GetSpanEndIndexAt(index) == end)
    {
      return span;
    }
  }
  return Text::ForegroundColorSpan();
}

Text::BackgroundColorSpan FindBackgroundSpan(const Text::StyledText& styledText, uint32_t start, uint32_t end)
{
  for(uint32_t index = 0u; index < styledText.GetSpanCount(); ++index)
  {
    Text::BackgroundColorSpan span = Text::BackgroundColorSpan::DownCast(styledText.GetSpanAt(index));
    if(span && styledText.GetSpanStartIndexAt(index) == start && styledText.GetSpanEndIndexAt(index) == end)
    {
      return span;
    }
  }
  return Text::BackgroundColorSpan();
}

Text::FontSpan FindFontSpan(const Text::StyledText& styledText, uint32_t start, uint32_t end)
{
  for(uint32_t index = 0u; index < styledText.GetSpanCount(); ++index)
  {
    Text::FontSpan span = Text::FontSpan::DownCast(styledText.GetSpanAt(index));
    if(span && styledText.GetSpanStartIndexAt(index) == start && styledText.GetSpanEndIndexAt(index) == end)
    {
      return span;
    }
  }
  return Text::FontSpan();
}

Text::LineThroughSpan FindLineThroughSpan(const Text::StyledText& styledText, uint32_t start, uint32_t end)
{
  for(uint32_t index = 0u; index < styledText.GetSpanCount(); ++index)
  {
    Text::LineThroughSpan span = Text::LineThroughSpan::DownCast(styledText.GetSpanAt(index));
    if(span && styledText.GetSpanStartIndexAt(index) == start && styledText.GetSpanEndIndexAt(index) == end)
    {
      return span;
    }
  }
  return Text::LineThroughSpan();
}

bool HasFontSpanRange(const Text::StyledText& styledText, uint32_t start, uint32_t end)
{
  for(uint32_t index = 0u; index < styledText.GetSpanCount(); ++index)
  {
    if(Text::FontSpan::DownCast(styledText.GetSpanAt(index)) &&
       styledText.GetSpanStartIndexAt(index) == start &&
       styledText.GetSpanEndIndexAt(index) == end)
    {
      return true;
    }
  }
  return false;
}

bool HasBackgroundSpanRange(const Text::StyledText& styledText, uint32_t start, uint32_t end)
{
  for(uint32_t index = 0u; index < styledText.GetSpanCount(); ++index)
  {
    if(Text::BackgroundColorSpan::DownCast(styledText.GetSpanAt(index)) &&
       styledText.GetSpanStartIndexAt(index) == start &&
       styledText.GetSpanEndIndexAt(index) == end)
    {
      return true;
    }
  }
  return false;
}

bool HasFontStyleAt(const Text::StyledText& styledText, uint32_t position)
{
  for(uint32_t index = 0u; index < styledText.GetSpanCount(); ++index)
  {
    if(Text::FontSpan::DownCast(styledText.GetSpanAt(index)) &&
       styledText.GetSpanStartIndexAt(index) <= position &&
       position < styledText.GetSpanEndIndexAt(index))
    {
      return true;
    }
  }
  return false;
}

void CheckFontStyleCoverage(const Text::StyledText& styledText, uint32_t start, uint32_t end, bool expected)
{
  for(uint32_t position = start; position < end; ++position)
  {
    DALI_TEST_EQUALS(HasFontStyleAt(styledText, position), expected, TEST_LOCATION);
  }
}

} // namespace

int UtcDaliMarkdownParserSnapshotP(void)
{
  MarkdownParser parser;
  auto           snapshot = parser.Parse("# Title\n\nHello **world** and *DALi*\n\n- [x] task\n\n[link](https://example.com \"title\")\n\n![alt](image.png \"caption\")\n\n| A | B |\n|---|:--:|\n| 1 | 2 |\n\n~~gone~~\n\n```cpp\nint x;\n```\n",
                                         1u);

  DALI_TEST_CHECK(snapshot.parseSucceeded);
  DALI_TEST_CHECK(snapshot.nodes.size() > 10u);

  auto heading = FindFirstRole(snapshot, MarkdownRenderRole::HEADING);
  DALI_TEST_CHECK(heading);
  DALI_TEST_EQUALS(heading->headingLevel, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(heading->text, std::string("Title"), TEST_LOCATION);

  auto paragraph = FindText(snapshot, "Hello world and DALi");
  DALI_TEST_CHECK(paragraph);
  DALI_TEST_EQUALS(paragraph->styleRuns.size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK((paragraph->styleRuns[0].flags & MARKDOWN_TEXT_STYLE_STRONG) != 0u);
  DALI_TEST_CHECK((paragraph->styleRuns[1].flags & MARKDOWN_TEXT_STYLE_EMPHASIS) != 0u);

  auto listItem = FindFirstRole(snapshot, MarkdownRenderRole::LIST_ITEM);
  DALI_TEST_CHECK(listItem);
  DALI_TEST_CHECK(listItem->taskListItem);
  DALI_TEST_CHECK(listItem->taskChecked);
  DALI_TEST_CHECK(FindText(snapshot, "task"));

  auto link = FindText(snapshot, "link");
  DALI_TEST_CHECK(link);
  DALI_TEST_EQUALS(link->linkRanges.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(link->linkRanges[0].href, std::string("https://example.com"), TEST_LOCATION);
  DALI_TEST_EQUALS(link->linkRanges[0].title, std::string("title"), TEST_LOCATION);

  auto image = FindFirstRole(snapshot, MarkdownRenderRole::BLOCK_IMAGE);
  DALI_TEST_CHECK(image);
  DALI_TEST_EQUALS(image->sourceUrl, std::string("image.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(image->altText, std::string("alt"), TEST_LOCATION);

  auto code = FindFirstRole(snapshot, MarkdownRenderRole::CODE_BLOCK);
  DALI_TEST_CHECK(code);
  DALI_TEST_EQUALS(code->language, std::string("cpp"), TEST_LOCATION);
  DALI_TEST_CHECK(code->text.find("int x;") != std::string::npos);

  auto strike = FindText(snapshot, "gone");
  DALI_TEST_CHECK(strike);
  DALI_TEST_EQUALS(strike->styleRuns.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK((strike->styleRuns[0].flags & MARKDOWN_TEXT_STYLE_STRIKETHROUGH) != 0u);

  auto table = FindFirstRole(snapshot, MarkdownRenderRole::TABLE);
  DALI_TEST_CHECK(table);
  END_TEST;
}

int UtcDaliMarkdownParserEntitiesAndUnicodeP(void)
{
  MarkdownParser parser;
  auto           snapshot = parser.Parse("Tom &amp; Jerry &copy; &ndash; &hellip; &rarr;\n\n안녕하세요 **DALi**", 2u);

  DALI_TEST_CHECK(snapshot.parseSucceeded);
  DALI_TEST_CHECK(FindText(snapshot, "Tom & Jerry © – … →"));
  auto korean = FindText(snapshot, "안녕하세요 DALi");
  DALI_TEST_CHECK(korean);
  DALI_TEST_EQUALS(korean->utf32Length, 10u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownParserSoftAndHardBreakP(void)
{
  MarkdownParser parser;
  auto           soft = parser.Parse("soft\nbreak", 3u);
  DALI_TEST_CHECK(FindText(soft, "soft break"));

  auto hard = parser.Parse("hard  \nbreak", 4u);
  DALI_TEST_CHECK(FindText(hard, "hard\nbreak"));

  auto code  = parser.Parse("```text\na\nb\n```", 5u);
  auto block = FindFirstRole(code, MarkdownRenderRole::CODE_BLOCK);
  DALI_TEST_CHECK(block);
  DALI_TEST_CHECK(block->text.find("a\nb") != std::string::npos);
  END_TEST;
}

int UtcDaliMarkdownParserNumericEntitiesP(void)
{
  MarkdownParser parser;
  auto           snapshot = parser.Parse("&#169; &#x2192; &#0; &#xD800; &#x110000;", 6u);
  DALI_TEST_CHECK(snapshot.parseSucceeded);
  DALI_TEST_CHECK(FindText(snapshot, "© → � � �"));
  END_TEST;
}

int UtcDaliMarkdownParserNestedSpansAndImagesP(void)
{
  MarkdownParser parser;
  auto           snapshot = parser.Parse("***bold italic***\n\n**outer *inner* outer**\n\n[**bold link**](https://example.com)\n\n![*styled alt*](image.png)\n\nText ![alt](icon.png) continues\n\n~~**nested**~~", 7u);

  DALI_TEST_CHECK(snapshot.parseSucceeded);
  auto first = FindText(snapshot, "bold italic");
  DALI_TEST_CHECK(first);
  DALI_TEST_EQUALS(first->styleRuns.size(), 2u, TEST_LOCATION);

  auto link = FindText(snapshot, "bold link");
  DALI_TEST_CHECK(link);
  DALI_TEST_EQUALS(link->linkRanges.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!link->styleRuns.empty());

  auto blockImage = FindFirstRole(snapshot, MarkdownRenderRole::BLOCK_IMAGE);
  DALI_TEST_CHECK(blockImage);
  DALI_TEST_EQUALS(blockImage->sourceUrl, std::string("image.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(blockImage->altText, std::string("styled alt"), TEST_LOCATION);

  auto inlineImage = FindText(snapshot, "Text alt continues");
  DALI_TEST_CHECK(inlineImage);
  DALI_TEST_EQUALS(inlineImage->inlineObjects.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(inlineImage->inlineObjects[0].position, 5u, TEST_LOCATION);

  auto nested = FindText(snapshot, "nested");
  DALI_TEST_CHECK(nested);
  DALI_TEST_EQUALS(nested->styleRuns.size(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownParserBlockImageWhitespaceP(void)
{
  MarkdownParser parser;
  auto           snapshot = parser.Parse("  ![alt](image.png \"caption\")  \n\nText ![inline](icon.png) continues\n\n![one](one.png) ![two](two.png)\n", 9u);

  DALI_TEST_CHECK(snapshot.parseSucceeded);
  DALI_TEST_EQUALS(CountRole(snapshot, MarkdownRenderRole::BLOCK_IMAGE), 1u, TEST_LOCATION);

  auto blockImage = FindFirstRole(snapshot, MarkdownRenderRole::BLOCK_IMAGE);
  DALI_TEST_CHECK(blockImage);
  DALI_TEST_EQUALS(blockImage->sourceUrl, std::string("image.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(blockImage->altText, std::string("alt"), TEST_LOCATION);

  auto inlineImage = FindText(snapshot, "Text inline continues");
  DALI_TEST_CHECK(inlineImage);
  DALI_TEST_EQUALS(inlineImage->inlineObjects.size(), 1u, TEST_LOCATION);

  auto twoImages = FindText(snapshot, "one two");
  DALI_TEST_CHECK(twoImages);
  DALI_TEST_EQUALS(twoImages->inlineObjects.size(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownParserTableTaskSetextRawHtmlP(void)
{
  MarkdownParser parser;
  auto           snapshot = parser.Parse("Title\n-----\n\n- [ ] todo\n- [x] done\n\n| A | B | C |\n|:--|:-:|--:|\n| 1 | 2 | 3 |\n\n<div>raw</div>", 8u);

  auto heading = FindFirstRole(snapshot, MarkdownRenderRole::HEADING);
  DALI_TEST_CHECK(heading);
  DALI_TEST_EQUALS(heading->headingLevel, 2u, TEST_LOCATION);

  DALI_TEST_EQUALS(CountRole(snapshot, MarkdownRenderRole::LIST_ITEM), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(CountRole(snapshot, MarkdownRenderRole::TABLE_CELL), 6u, TEST_LOCATION);

  bool sawLeft   = false;
  bool sawCenter = false;
  bool sawRight  = false;
  for(const auto& node : snapshot.nodes)
  {
    if(node.role == MarkdownRenderRole::TABLE_CELL)
    {
      sawLeft   = sawLeft || node.tableAlignment == MarkdownTableAlignment::LEFT;
      sawCenter = sawCenter || node.tableAlignment == MarkdownTableAlignment::CENTER;
      sawRight  = sawRight || node.tableAlignment == MarkdownTableAlignment::RIGHT;
    }
  }
  DALI_TEST_CHECK(sawLeft);
  DALI_TEST_CHECK(sawCenter);
  DALI_TEST_CHECK(sawRight);
  DALI_TEST_CHECK(FindText(snapshot, "<div>raw</div>") || FindText(snapshot, "raw"));
  END_TEST;
}

int UtcDaliMarkdownParserOrderedListMarkerColumnP(void)
{
  MarkdownParser parser;
  auto           snapshot = parser.Parse("8. eight\n9. nine\n10. ten\n", 10u);

  uint32_t orderedItemCount = 0u;
  for(const auto& node : snapshot.nodes)
  {
    if(node.role == MarkdownRenderRole::LIST_ITEM)
    {
      ++orderedItemCount;
      DALI_TEST_CHECK(node.listKind == MarkdownListKind::ORDERED);
      DALI_TEST_EQUALS(node.listMarkerColumnLength, 3u, TEST_LOCATION);
    }
  }
  DALI_TEST_EQUALS(orderedItemCount, 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownParserPlainTextModeIsolationP(void)
{
  MarkdownParser parser;
  const char*    markdown = "Line one\nLine two\n\n[]()\n\n![](empty.png)\n\nBefore <span>HTML</span> after\n\n<div>\nHTML block\n</div>\n\nAfter";

  const auto defaultSnapshot   = parser.Parse(markdown, 11u);
  const auto plainTextSnapshot = parser.Parse(markdown, 12u, MarkdownParser::Options::PlainText());

  DALI_TEST_CHECK(defaultSnapshot.parseSucceeded);
  DALI_TEST_CHECK(plainTextSnapshot.parseSucceeded);
  DALI_TEST_CHECK(defaultSnapshot.plainTextLineBreaks.empty());
  DALI_TEST_CHECK(!plainTextSnapshot.plainTextLineBreaks.empty());
  DALI_TEST_CHECK(!HasPlainTextLineBreakMetadata(defaultSnapshot));
  DALI_TEST_CHECK(HasPlainTextLineBreakMetadata(plainTextSnapshot));
  CheckPlainTextLineBreakEvents(plainTextSnapshot);

  // Link and image metadata are retained for rendering so an empty label can
  // still project its URL. Plain-text-only line-break metadata stays isolated.
  DALI_TEST_EQUALS(CountZeroLengthLinks(defaultSnapshot), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(CountZeroLengthInlineObjects(defaultSnapshot) > 0u);
  DALI_TEST_CHECK(CountZeroLengthLinks(plainTextSnapshot) > 0u);
  DALI_TEST_CHECK(CountZeroLengthInlineObjects(plainTextSnapshot) > 0u);

  DALI_TEST_CHECK(FindText(defaultSnapshot, "Before <span>HTML</span> after"));
  DALI_TEST_CHECK(FindText(plainTextSnapshot, "Before HTML after"));
  DALI_TEST_EQUALS(CountRole(defaultSnapshot, MarkdownRenderRole::RAW_HTML), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(CountRole(plainTextSnapshot, MarkdownRenderRole::RAW_HTML) > 0u);

  END_TEST;
}

int UtcDaliMarkdownParserPlainTextDialectParityP(void)
{
  MarkdownParser parser;
  const char*    markdown = "Title\n-----\n\n- [ ] todo\n- [x] done\n\n~~gone~~\n\n<https://example.com>\n\n| A | B |\n|---|---|\n| 1 | 2 |\n";

  const auto defaultSnapshot   = parser.Parse(markdown, 13u);
  const auto plainTextSnapshot = parser.Parse(markdown, 14u, MarkdownParser::Options::PlainText());

  DALI_TEST_CHECK(defaultSnapshot.parseSucceeded);
  DALI_TEST_CHECK(plainTextSnapshot.parseSucceeded);
  DALI_TEST_EQUALS(CountRole(defaultSnapshot, MarkdownRenderRole::HEADING), CountRole(plainTextSnapshot, MarkdownRenderRole::HEADING), TEST_LOCATION);
  DALI_TEST_EQUALS(CountRole(defaultSnapshot, MarkdownRenderRole::TABLE), CountRole(plainTextSnapshot, MarkdownRenderRole::TABLE), TEST_LOCATION);
  DALI_TEST_EQUALS(CountRole(defaultSnapshot, MarkdownRenderRole::TABLE_CELL), CountRole(plainTextSnapshot, MarkdownRenderRole::TABLE_CELL), TEST_LOCATION);
  DALI_TEST_EQUALS(CountTaskListItems(defaultSnapshot), CountTaskListItems(plainTextSnapshot), TEST_LOCATION);
  DALI_TEST_EQUALS(CountStyleFlag(defaultSnapshot, MARKDOWN_TEXT_STYLE_STRIKETHROUGH), CountStyleFlag(plainTextSnapshot, MARKDOWN_TEXT_STYLE_STRIKETHROUGH), TEST_LOCATION);
  DALI_TEST_EQUALS(CountAutolinks(defaultSnapshot), CountAutolinks(plainTextSnapshot), TEST_LOCATION);

  END_TEST;
}

int UtcDaliMarkdownParserPlainTextSparseLineBreakLookupP(void)
{
  MarkdownParser parser;
  auto           snapshot = parser.Parse("A\nB  \nC\n\nD\nE", 15u, MarkdownParser::Options::PlainText());

  DALI_TEST_CHECK(snapshot.parseSucceeded);
  DALI_TEST_CHECK(snapshot.plainTextLineBreaks.size() >= 3u);

  MarkdownPlainTextLineBreak firstNodeFirst;
  MarkdownPlainTextLineBreak firstNodeSecond;
  MarkdownPlainTextLineBreak otherNode;
  bool                       foundFirstNodeFirst  = false;
  bool                       foundFirstNodeSecond = false;
  bool                       foundOtherNode       = false;

  for(const auto& event : snapshot.plainTextLineBreaks)
  {
    if(!foundFirstNodeFirst)
    {
      firstNodeFirst     = event;
      foundFirstNodeFirst = true;
      continue;
    }

    if(event.nodeIndex == firstNodeFirst.nodeIndex && !foundFirstNodeSecond)
    {
      firstNodeSecond     = event;
      foundFirstNodeSecond = true;
    }
    else if(event.nodeIndex != firstNodeFirst.nodeIndex && !foundOtherNode)
    {
      otherNode     = event;
      foundOtherNode = true;
    }
  }

  DALI_TEST_CHECK(foundFirstNodeFirst);
  DALI_TEST_CHECK(foundFirstNodeSecond);
  DALI_TEST_CHECK(foundOtherNode);

  if(foundFirstNodeFirst && foundFirstNodeSecond && foundOtherNode)
  {
    snapshot.plainTextLineBreaks.clear();
    snapshot.plainTextLineBreaks.push_back(firstNodeSecond);
    snapshot.plainTextLineBreaks.push_back(otherNode);
    snapshot.plainTextLineBreaks.push_back(firstNodeFirst);

    DALI_TEST_EQUALS(MarkdownSnapshotToPlainText(snapshot), Dali::String("A\nB\nC\nD\nE"), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliMarkdownViewConstructorP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliMarkdownViewNewAndDownCastP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(MarkdownView::DownCast(BaseHandle(view)));
  DALI_TEST_CHECK(DownCast<MarkdownView>(BaseHandle(view)));
  END_TEST;
}

int UtcDaliMarkdownViewStyleDefaultAndBuilderP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  MarkdownViewStyle style = MarkdownViewStyle::Default();
  DALI_TEST_CHECK(style);
  MarkdownViewStyle registeredStyle = UiConfig::GetCurrent().StyleSheet().GetStyle(MarkdownViewStyle::DefaultKey());
  DALI_TEST_CHECK(registeredStyle);
  DALI_TEST_CHECK(registeredStyle == style);
  DALI_TEST_CHECK(MarkdownViewStyle::DownCast(style));
  DALI_TEST_CHECK(MarkdownViewStyle::StaticDownCast(style));
  DALI_TEST_CHECK(!MarkdownViewStyle::DownCast(BaseHandle()));

  DALI_TEST_EQUALS(style.GetTextFontFamily(), Dali::String("SamsungOneUI_400"), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetHeadingFontFamily(), Dali::String("SamsungOneUI_700"), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetCodeFontFamily(), Dali::String("SamsungOneUI_300"), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetTextFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetHeading1FontSize(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetHeading2FontSize(), 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetHeading3FontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetHeading4FontSize(), 16.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetHeading5FontSize(), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetHeading6FontSize(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetCodeBlockFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetCodeBlockTitleFontSize(), 16.0f, TEST_LOCATION);

  DALI_TEST_CHECK(style.GetTextColor() == UiColor(0x000000));
  DALI_TEST_CHECK(style.GetHeadingTextColor() == UiColor(0x000000));
  DALI_TEST_CHECK(style.GetQuoteTextColor() == UiColor(0x2F2F2F));
  DALI_TEST_CHECK(style.GetCodeTextColor() == UiColor(0x121212));
  DALI_TEST_CHECK(style.GetCodeBlockTitleTextColor() == UiColor(0x454545));
  DALI_TEST_CHECK(style.GetInlineCodeBackgroundColor() == UiColor(0x000000u, 0.0f));
  DALI_TEST_CHECK(style.GetCodeBlockBackgroundColor() == UiColor(0xCCCCCCu, 0x33u / 255.0f));
  DALI_TEST_CHECK(style.GetCodeBlockTitleBackgroundColor() == UiColor(0xCCCCCCu, 0x55u / 255.0f));
  DALI_TEST_CHECK(style.GetQuoteBarColor() == UiColor(0xDFDFDF));
  DALI_TEST_CHECK(style.GetThematicBreakColor() == UiColor(0xDFDFDF));
  DALI_TEST_CHECK(style.GetTableRuleColor() == UiColor(0x000000));

  const CheckBoxStyle defaultCheckBoxStyle = CheckBoxStyle::Default();
  DALI_TEST_CHECK(style.GetTaskCheckBoxIconColor() == defaultCheckBoxStyle.GetIconColor());
  DALI_TEST_CHECK(style.GetTaskCheckBoxSelectedIconColor() == defaultCheckBoxStyle.GetSelectedIconColor());
  SelectableImageInterface defaultTaskCheckBoxIcon = style.CreateTaskCheckBoxIcon();
  DALI_TEST_CHECK(defaultTaskCheckBoxIcon);
  defaultTaskCheckBoxIcon.Reset();

  const UiColor customTextColor(UiColor::None());
  const UiColor customHeadingColor(0x102030u, 0.25f);
  const UiColor customQuoteColor(UiColor::PRIMARY);
  const UiColor customCodeColor(UiColor("MarkdownCodeText").WithAlpha(0.70f));
  const UiColor customTitleColor(UiColor::ON_SURFACE);
  const UiColor customInlineBackground(0x203040u, 0.35f);
  const UiColor customCodeBackground(UiColor::SURFACE);
  const UiColor customTitleBackground("MarkdownCodeTitleBackground");
  const UiColor customQuoteBar(UiColor::OUTLINE);
  const UiColor customThematicBreak(0x304050u, 0.45f);
  const UiColor customTableRule(UiColor("MarkdownTableRule").ScaleAlpha(0.55f));
  const UiColor customTaskCheckBoxIconColor(0x102030);
  const UiColor customTaskCheckBoxSelectedIconColor(0x405060);

  MarkdownViewStyle iconColorOnlyStyle = style.Configure()
                                           .SetTaskCheckBoxIconColor(customTaskCheckBoxIconColor)
                                           .Build();
  DALI_TEST_CHECK(iconColorOnlyStyle.GetTaskCheckBoxIconColor() == customTaskCheckBoxIconColor);
  DALI_TEST_CHECK(iconColorOnlyStyle.GetTaskCheckBoxSelectedIconColor() == defaultCheckBoxStyle.GetSelectedIconColor());
  SelectableImageInterface inheritedTaskCheckBoxIcon = iconColorOnlyStyle.CreateTaskCheckBoxIcon();
  DALI_TEST_CHECK(inheritedTaskCheckBoxIcon);
  inheritedTaskCheckBoxIcon.Reset();

  MarkdownViewStyle configured = style.Configure()
                                   .SetTextFontFamily("BodyFamily")
                                   .SetHeadingFontFamily("HeadingFamily")
                                   .SetCodeFontFamily("CodeFamily")
                                   .SetTextFontSize(30.0f)
                                   .SetHeading1FontSize(42.0f)
                                   .SetHeading2FontSize(36.0f)
                                   .SetHeading3FontSize(32.0f)
                                   .SetHeading4FontSize(28.0f)
                                   .SetHeading5FontSize(24.0f)
                                   .SetHeading6FontSize(20.0f)
                                   .SetCodeBlockFontSize(24.0f)
                                   .SetCodeBlockTitleFontSize(18.0f)
                                   .SetTextColor(customTextColor)
                                   .SetHeadingTextColor(customHeadingColor)
                                   .SetQuoteTextColor(customQuoteColor)
                                   .SetCodeTextColor(customCodeColor)
                                   .SetCodeBlockTitleTextColor(customTitleColor)
                                   .SetInlineCodeBackgroundColor(customInlineBackground)
                                   .SetCodeBlockBackgroundColor(customCodeBackground)
                                   .SetCodeBlockTitleBackgroundColor(customTitleBackground)
                                   .SetQuoteBarColor(customQuoteBar)
                                   .SetThematicBreakColor(customThematicBreak)
                                   .SetTableRuleColor(customTableRule)
                                   .SetTaskCheckBoxIconGenerator(
                                     Ui::Callback<SelectableImageInterface()>::New(&CreateTaskCheckBoxIcon))
                                   .SetTaskCheckBoxIconColor(customTaskCheckBoxIconColor)
                                   .SetTaskCheckBoxSelectedIconColor(customTaskCheckBoxSelectedIconColor)
                                   .Build();

  DALI_TEST_EQUALS(configured.GetTextFontFamily(), Dali::String("BodyFamily"), TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetHeadingFontFamily(), Dali::String("HeadingFamily"), TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetCodeFontFamily(), Dali::String("CodeFamily"), TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetTextFontSize(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetHeading1FontSize(), 42.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetHeading2FontSize(), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetHeading3FontSize(), 32.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetHeading4FontSize(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetHeading5FontSize(), 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetHeading6FontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetCodeBlockFontSize(), 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetCodeBlockTitleFontSize(), 18.0f, TEST_LOCATION);
  DALI_TEST_CHECK(configured.GetTextColor() == customTextColor);
  DALI_TEST_CHECK(configured.GetHeadingTextColor() == customHeadingColor);
  DALI_TEST_CHECK(configured.GetQuoteTextColor() == customQuoteColor);
  DALI_TEST_CHECK(configured.GetCodeTextColor() == customCodeColor);
  DALI_TEST_CHECK(configured.GetCodeBlockTitleTextColor() == customTitleColor);
  DALI_TEST_CHECK(configured.GetInlineCodeBackgroundColor() == customInlineBackground);
  DALI_TEST_CHECK(configured.GetCodeBlockBackgroundColor() == customCodeBackground);
  DALI_TEST_CHECK(configured.GetCodeBlockTitleBackgroundColor() == customTitleBackground);
  DALI_TEST_CHECK(configured.GetQuoteBarColor() == customQuoteBar);
  DALI_TEST_CHECK(configured.GetThematicBreakColor() == customThematicBreak);
  DALI_TEST_CHECK(configured.GetTableRuleColor() == customTableRule);
  DALI_TEST_CHECK(configured.GetTaskCheckBoxIconColor() == customTaskCheckBoxIconColor);
  DALI_TEST_CHECK(configured.GetTaskCheckBoxSelectedIconColor() == customTaskCheckBoxSelectedIconColor);

  Test::ObjectDestructionTracker customTaskCheckBoxIconTracker(application.GetCore().GetObjectRegistry());
  SelectableImageInterface customTaskCheckBoxIcon = configured.CreateTaskCheckBoxIcon();
  DALI_TEST_CHECK(customTaskCheckBoxIcon);
  View customTaskCheckBoxIconView = customTaskCheckBoxIcon.GetView();
  DALI_TEST_CHECK(customTaskCheckBoxIconView);
  customTaskCheckBoxIconTracker.Start(customTaskCheckBoxIconView);
  DALI_TEST_EQUALS(gTaskCheckBoxIconCreateCount, 1, TEST_LOCATION);
  customTaskCheckBoxIcon.Reset();
  customTaskCheckBoxIconView.Reset();
  DALI_TEST_CHECK(customTaskCheckBoxIconTracker.IsDestroyed());

  MarkdownViewStyle configuredCopy = configured;
  DALI_TEST_CHECK(configuredCopy.GetCodeTextColor() == customCodeColor);
  DALI_TEST_CHECK(configuredCopy.GetTableRuleColor() == customTableRule);
  DALI_TEST_CHECK(configuredCopy.GetTaskCheckBoxIconColor() == customTaskCheckBoxIconColor);
  DALI_TEST_CHECK(configuredCopy.GetTaskCheckBoxSelectedIconColor() == customTaskCheckBoxSelectedIconColor);

  MarkdownViewStyle changedConfigured = configured.Configure()
                                          .SetTextFontFamily("ChangedBodyFamily")
                                          .SetTextFontSize(45.0f)
                                          .SetCodeTextColor(UiColor("ChangedCodeText"))
                                          .Build();
  DALI_TEST_EQUALS(changedConfigured.GetTextFontFamily(), Dali::String("ChangedBodyFamily"), TEST_LOCATION);
  DALI_TEST_EQUALS(changedConfigured.GetTextFontSize(), 45.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetTextFontFamily(), Dali::String("BodyFamily"), TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetTextFontSize(), 30.0f, TEST_LOCATION);
  DALI_TEST_CHECK(configured.GetCodeTextColor() == customCodeColor);
  DALI_TEST_CHECK(changedConfigured.GetTaskCheckBoxIconColor() == customTaskCheckBoxIconColor);
  DALI_TEST_CHECK(changedConfigured.GetTaskCheckBoxSelectedIconColor() == customTaskCheckBoxSelectedIconColor);
  DALI_TEST_EQUALS(style.GetTextFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_CHECK(style.GetTextColor() == UiColor(0x000000));

  gTaskCheckBoxIconCreateCount = 0;
  MarkdownView styledTaskView = MarkdownView::New(configured);
  styledTaskView.SetMarkdown("- [ ] task");
  DALI_TEST_EQUALS(gTaskCheckBoxIconCreateCount, 1, TEST_LOCATION);
  CheckBox styledTaskCheckBox = GetListItemTaskCheckBox(GetListItemAtDepth(styledTaskView, 1u));
  DALI_TEST_CHECK(styledTaskCheckBox);
  DALI_TEST_EQUALS(styledTaskCheckBox.GetIconWidth(), configured.GetTextFontSize(), TEST_LOCATION);
  DALI_TEST_EQUALS(styledTaskCheckBox.GetIconHeight(), configured.GetTextFontSize(), TEST_LOCATION);

  DALI_TEST_ASSERTION(MarkdownViewStyle::Builder().SetTextFontSize(0.0f), "finite and greater than zero");
  DALI_TEST_ASSERTION(MarkdownViewStyle::Builder().SetHeading1FontSize(std::numeric_limits<float>::infinity()), "finite and greater than zero");
  DALI_TEST_ASSERTION(MarkdownViewStyle::Builder().SetTaskCheckBoxIconGenerator(
                        Ui::Callback<SelectableImageInterface()>()),
                      "must be initialized");
  MarkdownViewStyle emptyIconStyle = MarkdownViewStyle::Builder()
                                       .SetTaskCheckBoxIconGenerator(
                                         Ui::Callback<SelectableImageInterface()>::New(&CreateEmptyTaskCheckBoxIcon))
                                       .Build();
  DALI_TEST_ASSERTION(emptyIconStyle.CreateTaskCheckBoxIcon(), "returned an empty SelectableImageInterface");
  DALI_TEST_ASSERTION(MarkdownView::New(MarkdownViewStyle()), "MarkdownViewStyle must be initialized");

  MarkdownViewStyle::Builder consumedBuilder;
  MarkdownViewStyle          consumedStyle = std::move(consumedBuilder).Build();
  DALI_TEST_CHECK(consumedStyle);
  DALI_TEST_ASSERTION(std::move(consumedBuilder).Build(), "Builder has already been consumed");
  END_TEST;
}

int UtcDaliMarkdownViewDefaultGeometryP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  DALI_TEST_EQUALS(view.GetPadding(), Insets(10.0f, 10.0f, 10.0f, 10.0f), TEST_LOCATION);

  view.SetMarkdown("> Quote\n\n```cpp\ncode\n```\n\n| A |\n|---|\n| B |\n\n---");
  DALI_TEST_EQUALS(view.GetChildViewCount(), 4u, TEST_LOCATION);

  View quote = view.GetChildViewAt(0u);
  DALI_TEST_EQUALS(quote.GetPadding(), Insets(10.0f, 10.0f, 10.0f, 10.0f), TEST_LOCATION);
  View quoteBar = quote.GetChildViewAt(0u);
  DALI_TEST_EQUALS(quoteBar.GetRequestedWidth(), 6.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(quoteBar.GetCornerRadius(), Vector4(3.0f, 3.0f, 3.0f, 3.0f), TEST_LOCATION);

  View code = view.GetChildViewAt(1u);
  DALI_TEST_CHECK(code.GetBackgroundColor() == UiColor(0xCCCCCCu, 0x33u / 255.0f));
  DALI_TEST_EQUALS(code.GetCornerRadius(), Vector4(12.0f, 12.0f, 12.0f, 12.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(code.GetChildViewCount(), 2u, TEST_LOCATION);
  View codeTitle = code.GetChildViewAt(0u);
  View codeBody  = code.GetChildViewAt(1u);
  DALI_TEST_CHECK(codeTitle.GetBackgroundColor() == UiColor(0xCCCCCCu, 0x55u / 255.0f));
  DALI_TEST_EQUALS(codeTitle.GetPadding(), Insets(10.0f, 10.0f, 10.0f, 10.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(codeBody.GetPadding(), Insets(10.0f, 10.0f, 10.0f, 10.0f), TEST_LOCATION);

  View table     = view.GetChildViewAt(2u);
  View tableHead = table.GetChildViewAt(0u);
  View tableRule = tableHead.GetChildViewAt(1u);
  View tableRow  = tableHead.GetChildViewAt(0u).GetChildViewAt(0u);
  View tableCell = tableRow.GetChildViewAt(0u);
  DALI_TEST_EQUALS(tableRule.GetRequestedHeight(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tableCell.GetPadding(), Insets(5.0f, 5.0f, 5.0f, 5.0f), TEST_LOCATION);

  View thematicBreak = view.GetChildViewAt(3u);
  DALI_TEST_EQUALS(thematicBreak.GetRequestedHeight(), 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownViewWrapContentWidthPolicyP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();
  view.SetMarkdown("> Quote\n\n```cpp\ncode\n```");

  DALI_TEST_EQUALS(view.GetRequestedWidth(), WRAP_CONTENT, TEST_LOCATION);

  MeasuredSize measured = view.Measure(360.0f, 1000.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 360.0f, TEST_LOCATION);
  view.Arrange(LayoutRect(0.0f, 0.0f, measured.GetWidth(), measured.GetHeight()));
  DALI_TEST_EQUALS(view.GetChildViewAt(0u).GetSize().width, 340.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetChildViewAt(1u).GetSize().width, 340.0f, TEST_LOCATION);

  measured = view.Measure(240.0f, 1000.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 240.0f, TEST_LOCATION);
  view.Arrange(LayoutRect(0.0f, 0.0f, measured.GetWidth(), measured.GetHeight()));
  DALI_TEST_EQUALS(view.GetChildViewAt(0u).GetSize().width, 220.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetChildViewAt(1u).GetSize().width, 220.0f, TEST_LOCATION);

  view.SetMaximumWidth(200.0f);
  measured = view.Measure(360.0f, 1000.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 200.0f, TEST_LOCATION);

  view.SetRequestedWidth(180.0f);
  measured = view.Measure(360.0f, 1000.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 180.0f, TEST_LOCATION);

  MarkdownView unbounded = MarkdownView::New();
  unbounded.SetMarkdown("> Quote");
  measured = unbounded.Measure(std::numeric_limits<float>::max(), 1000.0f);
  DALI_TEST_CHECK(measured.GetWidth() < 1000.0f);
  END_TEST;
}

int UtcDaliMarkdownViewUnboundedWidthAtFractionalUiScaleP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  UiScaleManager scaleManager  = UiScaleManager::Get();
  const float    originalScale = scaleManager.GetScale();
  scaleManager.SetScale(1.3f);

  MarkdownView view = MarkdownView::New();
  view.SetMarkdown("> Quote");
  const MeasuredSize measured = view.Measure(std::numeric_limits<float>::max(), 1000.0f);

  scaleManager.SetScale(originalScale);

  DALI_TEST_CHECK(std::isfinite(measured.GetWidth()));
  DALI_TEST_CHECK(measured.GetWidth() < 1000.0f);
  END_TEST;
}

int UtcDaliMarkdownViewStyleTokenIdentityP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  const UiColor textToken("CustomMarkdownText");
  const UiColor headingToken("CustomMarkdownHeading");
  MarkdownViewStyle style = MarkdownViewStyle::Builder()
                              .SetTextColor(textToken)
                              .SetHeadingTextColor(headingToken)
                              .Build();
  MarkdownViewStyle copy = style;

  DALI_TEST_CHECK(style.GetTextColor().HasColorId());
  DALI_TEST_EQUALS(style.GetTextColor().GetColorId(), Dali::String("CustomMarkdownText"), TEST_LOCATION);
  DALI_TEST_CHECK(style.GetTextColor() == textToken);
  DALI_TEST_CHECK(copy.GetTextColor() == textToken);
  DALI_TEST_CHECK(copy.GetHeadingTextColor() == headingToken);
  END_TEST;
}

int UtcDaliMarkdownViewDefaultStyleSheetOverrideP(void)
{
  gMarkdownStyleOverrideCreateCount = 0;
  Components::UiConfig config = Components::UiConfig::New();
  config.StyleSheet().SetStyle(MarkdownViewStyle::DefaultKey(), &CreateMarkdownStyleOverride);
  UiTestApplication application(config);

  MarkdownView view = MarkdownView::New();
  view.SetMarkdown("Default style lookup");
  Label label = GetOnlyRenderedLabel(view);
  DALI_TEST_CHECK(label);
  DALI_TEST_EQUALS(label.GetFontSize(), 31.0f, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetTextColor() == UiColor("MarkdownDefaultText"));

  MarkdownView secondView = MarkdownView::New();
  secondView.SetMarkdown("Cached default style");
  DALI_TEST_EQUALS(GetOnlyRenderedLabel(secondView).GetFontSize(), 31.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gMarkdownStyleOverrideCreateCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownViewCustomTypographyP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  MarkdownViewStyle style = MarkdownViewStyle::Builder()
                              .SetTextFontFamily("BodyFamily")
                              .SetHeadingFontFamily("HeadingFamily")
                              .SetCodeFontFamily("CodeFamily")
                              .SetTextFontSize(30.0f)
                              .SetHeading1FontSize(42.0f)
                              .SetHeading2FontSize(38.0f)
                              .SetHeading3FontSize(36.0f)
                              .SetHeading4FontSize(34.0f)
                              .SetHeading5FontSize(32.0f)
                              .SetHeading6FontSize(28.0f)
                              .SetCodeBlockFontSize(24.0f)
                              .SetCodeBlockTitleFontSize(18.0f)
                              .Build();
  MarkdownView view = MarkdownView::New(style);
  view.SetMarkdown("# H1\n\n## H2\n\n### H3\n\n#### H4\n\n##### H5\n\n###### H6\n\nBody\n\n> Quote\n\n| Cell |\n|---|\n| Data |\n\n```cpp\ncode\n```\n");

  Label body = FindLabel(view, "Body");
  DALI_TEST_CHECK(body);
  DALI_TEST_EQUALS(body.GetFontFamily(), Dali::String("BodyFamily"), TEST_LOCATION);
  DALI_TEST_EQUALS(body.GetFontSize(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(body.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(body.GetLineHeight(), 1.6f, TEST_LOCATION);
  DALI_TEST_CHECK(!body.IsSystemFontSizeScaleEnabled());

  const std::vector<std::pair<const char*, float>> headingSizes = {
    {"H1", 42.0f}, {"H2", 38.0f}, {"H3", 36.0f}, {"H4", 34.0f}, {"H5", 32.0f}, {"H6", 28.0f}};
  for(const auto& entry : headingSizes)
  {
    Label heading = FindLabel(view, entry.first);
    DALI_TEST_CHECK(heading);
    DALI_TEST_EQUALS(heading.GetFontFamily(), Dali::String("HeadingFamily"), TEST_LOCATION);
    DALI_TEST_EQUALS(heading.GetFontSize(), entry.second, TEST_LOCATION);
    const float expectedLineHeight = std::max(32.0f, entry.second * 32.0f / 28.0f);
    DALI_TEST_EQUALS(heading.GetLineHeightMode(), Text::LineHeightMode::ABSOLUTE, TEST_LOCATION);
    DALI_TEST_EQUALS(heading.GetLineHeight(), expectedLineHeight, TEST_LOCATION);
    DALI_TEST_CHECK(!heading.IsSystemFontSizeScaleEnabled());
  }

  Label quote = FindLabel(view, "Quote");
  Label cell  = FindLabel(view, "Cell");
  Label title = FindLabel(view, "cpp");
  Label code  = FindLabel(view, "code");
  DALI_TEST_EQUALS(quote.GetFontFamily(), Dali::String("BodyFamily"), TEST_LOCATION);
  DALI_TEST_EQUALS(quote.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(quote.GetLineHeight(), 1.6f, TEST_LOCATION);
  DALI_TEST_EQUALS(cell.GetFontFamily(), Dali::String("BodyFamily"), TEST_LOCATION);
  DALI_TEST_EQUALS(cell.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(cell.GetLineHeight(), 1.6f, TEST_LOCATION);
  DALI_TEST_EQUALS(title.GetFontFamily(), Dali::String("CodeFamily"), TEST_LOCATION);
  DALI_TEST_EQUALS(title.GetFontSize(), 18.0f, TEST_LOCATION);
  DALI_TEST_CHECK(!title.IsSystemFontSizeScaleEnabled());
  DALI_TEST_EQUALS(code.GetFontFamily(), Dali::String("CodeFamily"), TEST_LOCATION);
  DALI_TEST_EQUALS(code.GetFontSize(), 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(code.GetLineHeightMode(), Text::LineHeightMode::ABSOLUTE, TEST_LOCATION);
  DALI_TEST_EQUALS(code.GetLineHeight(), -1.0f, TEST_LOCATION);
  DALI_TEST_CHECK(!code.IsSystemFontSizeScaleEnabled());

  MarkdownViewStyle changedCopy = style.Configure().SetTextFontSize(45.0f).Build();
  DALI_TEST_EQUALS(changedCopy.GetTextFontSize(), 45.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(body.GetFontSize(), 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownViewHeadingLineHeightUpdateP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  MarkdownViewStyle style = MarkdownViewStyle::Builder()
                              .SetHeading1FontSize(42.0f)
                              .SetHeading2FontSize(21.0f)
                              .Build();
  MarkdownView view = MarkdownView::New(style);

  view.SetMarkdown("# Heading");
  Label heading = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(heading.GetFontSize(), 42.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(heading.GetLineHeight(), 48.0f, TEST_LOCATION);

  view.SetMarkdown("## Heading");
  Label reusedHeading = GetOnlyRenderedLabel(view);
  DALI_TEST_CHECK(reusedHeading == heading);
  DALI_TEST_EQUALS(reusedHeading.GetFontSize(), 21.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(reusedHeading.GetLineHeight(), 32.0f, TEST_LOCATION);

  view.SetMarkdown("# Heading");
  reusedHeading = GetOnlyRenderedLabel(view);
  DALI_TEST_CHECK(reusedHeading == heading);
  DALI_TEST_EQUALS(reusedHeading.GetFontSize(), 42.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(reusedHeading.GetLineHeight(), 48.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownViewInlineCodeStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  const UiColor bodyToken("MarkdownInlineBody");
  const UiColor codeToken("MarkdownInlineCode");
  const UiColor backgroundToken("MarkdownInlineBackground");
  MarkdownViewStyle style = MarkdownViewStyle::Builder()
                              .SetTextFontFamily("BodyFamily")
                              .SetCodeFontFamily("CodeFamily")
                              .SetTextColor(bodyToken)
                              .SetCodeTextColor(codeToken)
                              .SetInlineCodeBackgroundColor(backgroundToken)
                              .Build();
  MarkdownView view = MarkdownView::New(style);
  view.SetMarkdown("normal `code` normal");

  Label label = GetOnlyRenderedLabel(view);
  DALI_TEST_CHECK(label.GetTextColor() == bodyToken);
  Text::StyledText styledText = label.GetStyledText();
  DALI_TEST_CHECK(styledText);

  Text::FontSpan fontSpan = FindFontSpan(styledText, 7u, 11u);
  Text::ForegroundColorSpan foregroundSpan = FindForegroundSpan(styledText, 7u, 11u);
  Text::BackgroundColorSpan backgroundSpan = FindBackgroundSpan(styledText, 7u, 11u);
  DALI_TEST_CHECK(fontSpan);
  DALI_TEST_CHECK(foregroundSpan);
  DALI_TEST_CHECK(backgroundSpan);
  DALI_TEST_EQUALS(fontSpan.GetFontAttributes().GetFamily(), Dali::String("CodeFamily"), TEST_LOCATION);
  DALI_TEST_CHECK(foregroundSpan.GetColor() == codeToken);
  DALI_TEST_CHECK(backgroundSpan.GetColor() == backgroundToken);
  DALI_TEST_CHECK(!FindForegroundSpan(styledText, 0u, 7u));
  DALI_TEST_CHECK(!FindForegroundSpan(styledText, 11u, 18u));
  END_TEST;
}

int UtcDaliMarkdownViewEffectiveDecorationColorP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  const UiColor bodyToken("MarkdownDecorationBody");
  const UiColor codeToken(UiColor("MarkdownDecorationCode").WithAlpha(0.65f));
  MarkdownViewStyle style = MarkdownViewStyle::Builder()
                              .SetTextColor(bodyToken)
                              .SetCodeTextColor(codeToken)
                              .Build();

  MarkdownView view = MarkdownView::New(style);
  view.SetMarkdown("~~body~~ ~~`code`~~");
  Label label = GetOnlyRenderedLabel(view);
  DALI_TEST_CHECK(label);

  Text::StyledText styledText = label.GetStyledText();
  DALI_TEST_CHECK(styledText);
  Text::LineThroughSpan bodyLineThrough = FindLineThroughSpan(styledText, 0u, 4u);
  Text::LineThroughSpan codeLineThrough = FindLineThroughSpan(styledText, 5u, 9u);
  DALI_TEST_CHECK(bodyLineThrough);
  DALI_TEST_CHECK(codeLineThrough);
  DALI_TEST_CHECK(bodyLineThrough.GetLineThrough().GetColor() == bodyToken);
  DALI_TEST_CHECK(codeLineThrough.GetLineThrough().GetColor() == codeToken);
  END_TEST;
}

int UtcDaliMarkdownViewCustomListMarkerStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  for(const float fontSize : {10.0f, 20.0f, 50.0f, 100.0f})
  {
    CheckResponsiveListMarkerGeometry(fontSize);
  }

  const UiColor markerToken("MarkdownMarker");
  MarkdownViewStyle style = MarkdownViewStyle::Builder()
                              .SetTextFontFamily("SamsungOneUI_400")
                              .SetTextFontSize(30.0f)
                              .SetTextColor(markerToken)
                              .Build();

  MarkdownView unordered = MarkdownView::New(style);
  unordered.SetMarkdown("- first\n  - second\n    - third");
  View filled = GetShapeMarker(GetListItemAtDepth(unordered, 1u));
  View hollow = GetShapeMarker(GetListItemAtDepth(unordered, 2u));
  View square = GetShapeMarker(GetListItemAtDepth(unordered, 3u));
  DALI_TEST_EQUALS(filled.GetRequestedWidth(), 9.0f, TEST_LOCATION);
  DALI_TEST_CHECK(filled.GetBackgroundColor() == markerToken);
  DALI_TEST_EQUALS(filled.GetMargin().top + filled.GetRequestedHeight() + filled.GetMargin().bottom, 48.0f, TEST_LOCATION);
  DALI_TEST_CHECK(hollow.GetBackgroundColor().IsNone());
  DALI_TEST_CHECK(hollow.GetBorderlineColor() == markerToken);
  DALI_TEST_CHECK(square.GetBackgroundColor() == markerToken);

  MarkdownView wrapped = MarkdownView::New(style);
  wrapped.SetMarkdown("- This is a long unordered item that wraps onto another line when constrained to the test width.\n"
                      "  - nested item");
  ArrangeMarkdownView(wrapped, 320.0f);
  View wrappedItem = GetListItemAtDepth(wrapped, 1u);
  DALI_TEST_CHECK(GetListItemTextLabel(wrappedItem).GetSize().height > 48.0f);
  CheckShapeMarkerFirstLineAlignment(wrappedItem, 48.0f);
  CheckShapeMarkerFirstLineAlignment(GetListItemAtDepth(wrapped, 2u), 48.0f);

  MarkdownView ordered = MarkdownView::New(style);
  ordered.SetMarkdown("1. ordered");
  View  orderedItem   = GetListItemAtDepth(ordered, 1u);
  View  orderedHost   = GetListItemMarkerHost(orderedItem);
  Label orderedMarker = Label::DownCast(GetShapeMarker(orderedItem));
  DALI_TEST_CHECK(orderedMarker);
  DALI_TEST_EQUALS(orderedMarker.GetFontFamily(), Dali::String("SamsungOneUI_400"), TEST_LOCATION);
  DALI_TEST_EQUALS(orderedMarker.GetFontSize(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(orderedMarker.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(orderedMarker.GetLineHeight(), 1.6f, TEST_LOCATION);
  DALI_TEST_CHECK(!orderedMarker.IsSystemFontSizeScaleEnabled());
  DALI_TEST_CHECK(orderedMarker.GetTextColor() == markerToken);
  DALI_TEST_CHECK(orderedMarker.GetHorizontalTextAlignment() == Text::Alignment::END);
  DALI_TEST_CHECK(orderedMarker.GetRequestedWidth() >= orderedMarker.GetNaturalSize().width);
  DALI_TEST_EQUALS(orderedHost.GetRequestedWidth(), orderedMarker.GetRequestedWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(orderedHost.GetMargin().end, 12.0f, TEST_LOCATION);
  const float oneDigitWidth = orderedMarker.GetRequestedWidth();

  ordered.SetMarkdown("10. ordered");
  orderedItem = GetListItemAtDepth(ordered, 1u);
  DALI_TEST_CHECK(GetListItemMarkerHost(orderedItem) == orderedHost);
  DALI_TEST_CHECK(Label::DownCast(GetShapeMarker(orderedItem)) == orderedMarker);
  DALI_TEST_EQUALS(orderedMarker.GetText(), Dali::String("10."), TEST_LOCATION);
  DALI_TEST_CHECK(orderedMarker.GetRequestedWidth() >= orderedMarker.GetNaturalSize().width);
  DALI_TEST_CHECK(orderedMarker.GetRequestedWidth() >= oneDigitWidth);
  const float twoDigitWidth = orderedMarker.GetRequestedWidth();

  ordered.SetMarkdown("100. ordered");
  orderedItem = GetListItemAtDepth(ordered, 1u);
  DALI_TEST_CHECK(GetListItemMarkerHost(orderedItem) == orderedHost);
  DALI_TEST_CHECK(Label::DownCast(GetShapeMarker(orderedItem)) == orderedMarker);
  DALI_TEST_EQUALS(orderedMarker.GetText(), Dali::String("100."), TEST_LOCATION);
  DALI_TEST_CHECK(orderedMarker.GetRequestedWidth() >= orderedMarker.GetNaturalSize().width);
  DALI_TEST_CHECK(orderedMarker.GetRequestedWidth() >= twoDigitWidth);
  DALI_TEST_EQUALS(orderedHost.GetRequestedWidth(), orderedMarker.GetRequestedWidth(), TEST_LOCATION);
  ArrangeMarkdownView(ordered, 320.0f);
  DALI_TEST_CHECK(orderedMarker.GetSize().width >= orderedMarker.GetNaturalSize().width);
  DALI_TEST_CHECK(orderedHost.GetSize().width >= orderedMarker.GetNaturalSize().width);

  ordered.SetMarkdown("- [ ] task");
  orderedItem     = GetListItemAtDepth(ordered, 1u);
  View taskBullet = GetShapeMarker(orderedItem);
  DALI_TEST_CHECK(taskBullet);
  DALI_TEST_CHECK(!Label::DownCast(taskBullet));
  CheckBox taskCheckBox = GetListItemTaskCheckBox(orderedItem);
  DALI_TEST_CHECK(taskCheckBox);
  DALI_TEST_CHECK(!taskCheckBox.IsSelected());
  DALI_TEST_EQUALS(taskCheckBox.GetIconWidth(), 30.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskCheckBox.GetIconHeight(), 30.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskCheckBox.GetRequestedWidth(), 30.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskCheckBox.GetRequestedHeight(), 48.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(orderedHost.GetRequestedWidth(), 48.0f, 0.01f, TEST_LOCATION);

  ordered.SetMarkdown("- unordered");
  orderedItem = GetListItemAtDepth(ordered, 1u);
  DALI_TEST_CHECK(GetListItemMarkerHost(orderedItem) == orderedHost);
  DALI_TEST_CHECK(!Label::DownCast(GetShapeMarker(orderedItem)));

  ordered.SetMarkdown("100. ordered again");
  orderedItem = GetListItemAtDepth(ordered, 1u);
  DALI_TEST_CHECK(GetListItemMarkerHost(orderedItem) == orderedHost);
  DALI_TEST_CHECK(Label::DownCast(GetShapeMarker(orderedItem)) == orderedMarker);
  DALI_TEST_EQUALS(orderedMarker.GetText(), Dali::String("100."), TEST_LOCATION);
  DALI_TEST_CHECK(orderedMarker.GetRequestedWidth() >= orderedMarker.GetNaturalSize().width);
  END_TEST;
}

int UtcDaliMarkdownViewCustomTextColorsP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  const UiColor textColor(Color::RED);
  const UiColor headingColor("MarkdownHeading");
  const UiColor quoteColor(Color::GREEN);
  const UiColor codeColor(Color::BLUE);
  const UiColor titleColor(Color::CYAN);
  MarkdownViewStyle style = MarkdownViewStyle::Builder()
                              .SetTextColor(textColor)
                              .SetHeadingTextColor(headingColor)
                              .SetQuoteTextColor(quoteColor)
                              .SetCodeTextColor(codeColor)
                              .SetCodeBlockTitleTextColor(titleColor)
                              .Build();
  MarkdownView view = MarkdownView::New(style);
  view.SetMarkdown("# Heading\n\nBody [link](url)\n\n> Quote\n\n```cpp\ncode\n```\n");

  DALI_TEST_CHECK(FindLabel(view, "Heading").GetTextColor() == headingColor);
  DALI_TEST_CHECK(FindLabel(view, "Body link [url]").GetTextColor() == textColor);
  DALI_TEST_CHECK(FindLabel(view, "Quote").GetTextColor() == quoteColor);
  DALI_TEST_CHECK(FindLabel(view, "cpp").GetTextColor() == titleColor);
  DALI_TEST_CHECK(FindLabel(view, "code").GetTextColor() == codeColor);
  END_TEST;
}

int UtcDaliMarkdownViewCustomDecorationColorsP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  const UiColor codeBackground("MarkdownCodeBlock");
  const UiColor titleBackground(Color::YELLOW);
  const UiColor titleText("MarkdownCodeTitleText");
  const UiColor quoteBar("MarkdownQuoteBar");
  const UiColor thematic(Color::MAGENTA);
  const UiColor tableRule("MarkdownTableRule");
  MarkdownViewStyle style = MarkdownViewStyle::Builder()
                              .SetCodeFontFamily("LazyCodeFamily")
                              .SetCodeBlockTitleFontSize(18.0f)
                              .SetCodeBlockTitleTextColor(titleText)
                              .SetCodeBlockBackgroundColor(codeBackground)
                              .SetCodeBlockTitleBackgroundColor(titleBackground)
                              .SetQuoteBarColor(quoteBar)
                              .SetThematicBreakColor(thematic)
                              .SetTableRuleColor(tableRule)
                              .Build();

  MarkdownView code = MarkdownView::New(style);
  code.SetMarkdown("```cpp\ncode\n```");
  View codeRoot = code.GetChildViewAt(0u);
  DALI_TEST_CHECK(codeRoot.GetBackgroundColor() == codeBackground);
  DALI_TEST_CHECK(codeRoot.GetChildViewAt(0u).GetBackgroundColor() == titleBackground);

  MarkdownView lazyTitle = MarkdownView::New(style);
  lazyTitle.SetMarkdown("```\ncode\n```");
  View lazyCodeRoot = lazyTitle.GetChildViewAt(0u);
  DALI_TEST_EQUALS(lazyCodeRoot.GetChildViewCount(), 1u, TEST_LOCATION);
  View  codeContentHost = lazyCodeRoot.GetChildViewAt(0u);
  Label codeBodyLabel   = FindLabel(lazyTitle, "code");
  lazyTitle.SetMarkdown("```cpp\ncode\n```");
  DALI_TEST_CHECK(lazyTitle.GetChildViewAt(0u) == lazyCodeRoot);
  DALI_TEST_EQUALS(lazyCodeRoot.GetChildViewCount(), 2u, TEST_LOCATION);
  View  languageHost  = lazyCodeRoot.GetChildViewAt(0u);
  Label languageLabel = FindLabel(lazyTitle, "cpp");
  DALI_TEST_CHECK(languageHost.GetBackgroundColor() == titleBackground);
  DALI_TEST_CHECK(lazyCodeRoot.GetChildViewAt(1u) == codeContentHost);
  DALI_TEST_CHECK(FindLabel(lazyTitle, "code") == codeBodyLabel);
  DALI_TEST_EQUALS(languageLabel.GetFontFamily(), Dali::String("LazyCodeFamily"), TEST_LOCATION);
  DALI_TEST_EQUALS(languageLabel.GetFontSize(), 18.0f, TEST_LOCATION);
  DALI_TEST_CHECK(languageLabel.GetTextColor() == titleText);

  lazyTitle.SetMarkdown("```cpp\ncode\n```");
  DALI_TEST_CHECK(lazyCodeRoot.GetChildViewAt(0u) == languageHost);
  DALI_TEST_CHECK(FindLabel(lazyTitle, "cpp") == languageLabel);
  DALI_TEST_CHECK(lazyCodeRoot.GetChildViewAt(1u) == codeContentHost);

  lazyTitle.SetMarkdown("```python\ncode\n```");
  DALI_TEST_CHECK(lazyCodeRoot.GetChildViewAt(0u) == languageHost);
  DALI_TEST_CHECK(FindLabel(lazyTitle, "python") == languageLabel);
  DALI_TEST_CHECK(lazyCodeRoot.GetChildViewAt(1u) == codeContentHost);

  lazyTitle.SetMarkdown("```\ncode\n```");
  DALI_TEST_EQUALS(lazyCodeRoot.GetChildViewCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(lazyCodeRoot.GetChildViewAt(0u) == codeContentHost);
  DALI_TEST_CHECK(FindLabel(lazyTitle, "code") == codeBodyLabel);

  lazyTitle.SetMarkdown("```cpp\ncode\n```");
  DALI_TEST_EQUALS(lazyCodeRoot.GetChildViewCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(lazyCodeRoot.GetChildViewAt(0u) == languageHost);
  DALI_TEST_CHECK(FindLabel(lazyTitle, "cpp") == languageLabel);
  DALI_TEST_CHECK(lazyCodeRoot.GetChildViewAt(1u) == codeContentHost);

  MarkdownView quote = MarkdownView::New(style);
  quote.SetMarkdown("> Quote");
  DALI_TEST_CHECK(quote.GetChildViewAt(0u).GetChildViewAt(0u).GetBackgroundColor() == quoteBar);

  MarkdownView rule = MarkdownView::New(style);
  rule.SetMarkdown("---");
  DALI_TEST_CHECK(rule.GetChildViewAt(0u).GetBackgroundColor() == thematic);

  MarkdownView table = MarkdownView::New(style);
  table.SetMarkdown("| A |\n|---|\n| B |");
  View tableHead = table.GetChildViewAt(0u).GetChildViewAt(0u);
  DALI_TEST_CHECK(tableHead.GetChildViewAt(1u).GetBackgroundColor() == tableRule);
  END_TEST;
}

int UtcDaliMarkdownViewThemeBindingP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  UiColorManager    manager = UiColorManager::Get();

  gUseAlternateMarkdownColors = false;
  ScopedMarkdownColorOverride colorOverride(manager, OverrideMarkdownColors);
  MarkdownViewStyle style = MarkdownViewStyle::Builder()
                              .SetTextColor(UiColor("MarkdownBody"))
                              .SetCodeBlockBackgroundColor(UiColor("MarkdownCodeBackground"))
                              .SetQuoteBarColor(UiColor("MarkdownQuoteBar"))
                              .Build();

  MarkdownView body = MarkdownView::New(style);
  body.SetMarkdown("Body");
  Label bodyLabel = GetOnlyRenderedLabel(body);
  DALI_TEST_EQUALS(bodyLabel.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);

  MarkdownView code = MarkdownView::New(style);
  code.SetMarkdown("```\ncode\n```");
  View codeRoot = code.GetChildViewAt(0u);
  DALI_TEST_EQUALS(codeRoot.GetBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  MarkdownView quote = MarkdownView::New(style);
  quote.SetMarkdown("> Quote");
  View quoteBar = quote.GetChildViewAt(0u).GetChildViewAt(0u);
  DALI_TEST_EQUALS(quoteBar.GetBackgroundColor().GetRgba(), Color::MAGENTA, TEST_LOCATION);

  MarkdownView list = MarkdownView::New(style);
  list.SetMarkdown("- item");
  View marker = GetShapeMarker(GetListItemAtDepth(list, 1u));
  DALI_TEST_EQUALS(marker.GetBackgroundColor().GetRgba(), Color::RED, TEST_LOCATION);

  gUseAlternateMarkdownColors = true;
  manager.SetColorOverride(OverrideMarkdownColors);
  DALI_TEST_EQUALS(bodyLabel.GetTextColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(codeRoot.GetBackgroundColor().GetRgba(), Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(quoteBar.GetBackgroundColor().GetRgba(), Color::CYAN, TEST_LOCATION);
  DALI_TEST_EQUALS(marker.GetBackgroundColor().GetRgba(), Color::GREEN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMarkdownViewSetGetClearP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("Hello **world**");
  DALI_TEST_EQUALS(view.GetMarkdown(), Dali::String("Hello **world**"), TEST_LOCATION);
  DALI_TEST_CHECK(view.GetChildCount() > 0u);

  view.Clear();
  DALI_TEST_CHECK(view.GetMarkdown().Empty());
  DALI_TEST_EQUALS(view.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownViewTaskCheckBoxUpdatesSourceP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("1. [ ] first\n2. [X] second");
  View list = view.GetChildViewAt(0u);
  DALI_TEST_CHECK(list);
  DALI_TEST_EQUALS(list.GetChildViewCount(), 2u, TEST_LOCATION);

  CheckBox first  = GetListItemTaskCheckBox(list.GetChildViewAt(0u));
  CheckBox second = GetListItemTaskCheckBox(list.GetChildViewAt(1u));
  DALI_TEST_CHECK(first);
  DALI_TEST_CHECK(second);
  DALI_TEST_CHECK(!first.IsSelected());
  DALI_TEST_CHECK(second.IsSelected());

  Property::Map actionAttributes;
  DALI_TEST_CHECK(first.DoAction("activate", actionAttributes));
  DALI_TEST_EQUALS(view.GetMarkdown(), Dali::String("1. [x] first\n2. [X] second"), TEST_LOCATION);
  DALI_TEST_EQUALS(MarkdownView::ToPlainText(view.GetMarkdown()), Dali::String("1. [x] first\n2. [x] second"), TEST_LOCATION);
  DALI_TEST_CHECK(GetListItemTaskCheckBox(list.GetChildViewAt(0u)) == first);

  DALI_TEST_CHECK(second.DoAction("activate", actionAttributes));
  DALI_TEST_EQUALS(view.GetMarkdown(), Dali::String("1. [x] first\n2. [ ] second"), TEST_LOCATION);
  DALI_TEST_EQUALS(MarkdownView::ToPlainText(view.GetMarkdown()), Dali::String("1. [x] first\n2. [ ] second"), TEST_LOCATION);
  DALI_TEST_CHECK(GetListItemTaskCheckBox(list.GetChildViewAt(1u)) == second);

  // Moving unchanged rendered content must refresh each checkbox's source offset.
  view.SetMarkdown("1.  [x] first\n2.  [ ] second");
  list = view.GetChildViewAt(0u);
  DALI_TEST_CHECK(GetListItemTaskCheckBox(list.GetChildViewAt(0u)) == first);
  first.SetSelected(false);
  DALI_TEST_EQUALS(view.GetMarkdown(), Dali::String("1.  [ ] first\n2.  [ ] second"), TEST_LOCATION);

  view.SetMarkdown("- [ ] parent\n  1. [ ] child");
  CheckBox nested = GetListItemTaskCheckBox(GetListItemAtDepth(view, 2u));
  DALI_TEST_CHECK(nested);
  DALI_TEST_CHECK(nested.DoAction("activate", actionAttributes));
  DALI_TEST_EQUALS(view.GetMarkdown(), Dali::String("- [ ] parent\n  1. [x] child"), TEST_LOCATION);

  view.Clear();
  DALI_TEST_CHECK(nested.DoAction("activate", actionAttributes));
  DALI_TEST_CHECK(view.GetMarkdown().Empty());
  END_TEST;
}

int UtcDaliMarkdownViewTaskCheckBoxLifecycleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownViewStyle markdownStyle = MarkdownViewStyle::Default()
                                      .Configure()
                                      .SetTaskCheckBoxIconGenerator(
                                        Ui::Callback<SelectableImageInterface()>::New(&CreateTaskCheckBoxIcon))
                                      .Build();
  MarkdownView view = MarkdownView::New(markdownStyle);

  gTaskCheckBoxIconCreateCount = 0;

  // A task-list icon is stateful, so each simultaneously rendered task item must
  // receive and release its own generator result.
  view.SetMarkdown("- [ ] first\n- [x] second");
  View list = view.GetChildViewAt(0u);
  DALI_TEST_CHECK(list);
  DALI_TEST_EQUALS(list.GetChildViewCount(), 2u, TEST_LOCATION);

  CheckBox firstCheckBox  = GetListItemTaskCheckBox(list.GetChildViewAt(0u));
  CheckBox secondCheckBox = GetListItemTaskCheckBox(list.GetChildViewAt(1u));
  View     firstIcon      = GetCheckBoxIconView(firstCheckBox);
  View     secondIcon     = GetCheckBoxIconView(secondCheckBox);
  DALI_TEST_CHECK(firstCheckBox);
  DALI_TEST_CHECK(secondCheckBox);
  DALI_TEST_CHECK(firstIcon);
  DALI_TEST_CHECK(secondIcon);
  DALI_TEST_CHECK(firstIcon != secondIcon);
  DALI_TEST_EQUALS(gTaskCheckBoxIconCreateCount, 2, TEST_LOCATION);

  Test::ObjectDestructionTracker firstCheckBoxTracker(application.GetCore().GetObjectRegistry());
  Test::ObjectDestructionTracker secondCheckBoxTracker(application.GetCore().GetObjectRegistry());
  Test::ObjectDestructionTracker firstIconTracker(application.GetCore().GetObjectRegistry());
  Test::ObjectDestructionTracker secondIconTracker(application.GetCore().GetObjectRegistry());
  firstCheckBoxTracker.Start(firstCheckBox);
  secondCheckBoxTracker.Start(secondCheckBox);
  firstIconTracker.Start(firstIcon);
  secondIconTracker.Start(secondIcon);

  view.Clear();
  list.Reset();
  firstCheckBox.Reset();
  secondCheckBox.Reset();
  firstIcon.Reset();
  secondIcon.Reset();
  DALI_TEST_CHECK(firstCheckBoxTracker.IsDestroyed());
  DALI_TEST_CHECK(secondCheckBoxTracker.IsDestroyed());
  DALI_TEST_CHECK(firstIconTracker.IsDestroyed());
  DALI_TEST_CHECK(secondIconTracker.IsDestroyed());

  gTaskCheckBoxIconCreateCount = 0;

  constexpr uint32_t REPEAT_COUNT = 64u;
  for(uint32_t iteration = 0u; iteration < REPEAT_COUNT; ++iteration)
  {
    Test::ObjectDestructionTracker checkBoxTracker(application.GetCore().GetObjectRegistry());
    Test::ObjectDestructionTracker iconViewTracker(application.GetCore().GetObjectRegistry());

    view.SetMarkdown((iteration % 2u == 0u) ? "- [ ] task" : "- [x] task");
    CheckBox checkBox = GetListItemTaskCheckBox(GetListItemAtDepth(view, 1u));
    View     iconView = GetCheckBoxIconView(checkBox);
    DALI_TEST_CHECK(checkBox);
    DALI_TEST_CHECK(iconView);
    checkBoxTracker.Start(checkBox);
    iconViewTracker.Start(iconView);

    view.Clear();
    DALI_TEST_EQUALS(view.GetChildViewCount(), 0u, TEST_LOCATION);

    checkBox.Reset();
    iconView.Reset();
    DALI_TEST_CHECK(checkBoxTracker.IsDestroyed());
    DALI_TEST_CHECK(iconViewTracker.IsDestroyed());
    DALI_TEST_EQUALS(gTaskCheckBoxIconCreateCount, static_cast<int>(iteration + 1u), TEST_LOCATION);
  }

  DALI_TEST_EQUALS(gTaskCheckBoxIconCreateCount, static_cast<int>(REPEAT_COUNT), TEST_LOCATION);

  // Also cover the production Lottie icon used by the default CheckBoxStyle. If its
  // SelectableImageInterface remained alive, it would retain the icon view as well.
  Test::ObjectDestructionTracker defaultCheckBoxTracker(application.GetCore().GetObjectRegistry());
  Test::ObjectDestructionTracker defaultIconViewTracker(application.GetCore().GetObjectRegistry());
  MarkdownView                   defaultIconView = MarkdownView::New();
  defaultIconView.SetMarkdown("- [ ] task");
  CheckBox defaultCheckBox = GetListItemTaskCheckBox(GetListItemAtDepth(defaultIconView, 1u));
  View     defaultIcon     = GetCheckBoxIconView(defaultCheckBox);
  DALI_TEST_CHECK(defaultCheckBox);
  DALI_TEST_CHECK(defaultIcon);
  defaultCheckBoxTracker.Start(defaultCheckBox);
  defaultIconViewTracker.Start(defaultIcon);

  defaultIconView.Clear();
  defaultCheckBox.Reset();
  defaultIcon.Reset();
  DALI_TEST_CHECK(defaultCheckBoxTracker.IsDestroyed());
  DALI_TEST_CHECK(defaultIconViewTracker.IsDestroyed());
  END_TEST;
}

int UtcDaliMarkdownViewImageTextSubtreeLifecycleP(void)
{
  UiTestApplication              application(Components::UiConfig::New());
  Test::ObjectDestructionTracker rootTracker(application.GetCore().GetObjectRegistry());
  Test::ObjectDestructionTracker labelTracker(application.GetCore().GetObjectRegistry());
  MarkdownView                   view = MarkdownView::New();

  view.SetMarkdown("![alt](one.png)");
  DALI_TEST_EQUALS(view.GetChildViewCount(), 1u, TEST_LOCATION);

  View imageRoot = view.GetChildViewAt(0u);
  DALI_TEST_EQUALS(imageRoot.GetChildViewCount(), 1u, TEST_LOCATION);

  Label imageLabel = Label::DownCast(imageRoot.GetChildViewAt(0u));
  DALI_TEST_CHECK(imageLabel);
  DALI_TEST_EQUALS(imageLabel.GetText(), "alt [one.png]", TEST_LOCATION);
  DALI_TEST_CHECK(!ImageView::DownCast(imageRoot.GetChildViewAt(0u)));
  rootTracker.Start(imageRoot);
  labelTracker.Start(imageLabel);

  view.SetMarkdown("![alt](two.png)");
  DALI_TEST_CHECK(view.GetChildViewAt(0u) == imageRoot);
  DALI_TEST_CHECK(Label::DownCast(imageRoot.GetChildViewAt(0u)) == imageLabel);
  DALI_TEST_EQUALS(imageLabel.GetText(), "alt [two.png]", TEST_LOCATION);

  view.SetMarkdown("![next](two.png)");
  DALI_TEST_CHECK(view.GetChildViewAt(0u) == imageRoot);
  DALI_TEST_CHECK(Label::DownCast(imageRoot.GetChildViewAt(0u)) == imageLabel);
  DALI_TEST_EQUALS(imageLabel.GetText(), "next [two.png]", TEST_LOCATION);

  view.Clear();
  DALI_TEST_EQUALS(view.GetChildViewCount(), 0u, TEST_LOCATION);

  imageLabel.Reset();
  imageRoot.Reset();
  DALI_TEST_CHECK(rootTracker.IsDestroyed());
  DALI_TEST_CHECK(labelTracker.IsDestroyed());
  END_TEST;
}

int UtcDaliMarkdownViewStreamingSetReuseP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("안녕");
  DALI_TEST_EQUALS(view.GetChildViewCount(), 1u, TEST_LOCATION);
  View firstChild = view.GetChildViewAt(0u);

  view.SetMarkdown("안녕하세요");
  DALI_TEST_EQUALS(view.GetMarkdown(), Dali::String("안녕하세요"), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetChildViewCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(firstChild == view.GetChildViewAt(0u));
  END_TEST;
}

int UtcDaliMarkdownViewRepeatedSetDoesNotChurnTreeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view     = MarkdownView::New();
  const char*       markdown = "- item\n\n> quote\n\n| A | B |\n|---|---|\n| 1 | 2 |\n";

  view.SetMarkdown(markdown);
  DALI_TEST_EQUALS(view.GetChildViewCount(), 3u, TEST_LOCATION);
  View firstChild  = view.GetChildViewAt(0u);
  View secondChild = view.GetChildViewAt(1u);
  View thirdChild  = view.GetChildViewAt(2u);

  view.SetMarkdown(markdown);
  DALI_TEST_EQUALS(view.GetChildViewCount(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(firstChild == view.GetChildViewAt(0u));
  DALI_TEST_CHECK(secondChild == view.GetChildViewAt(1u));
  DALI_TEST_CHECK(thirdChild == view.GetChildViewAt(2u));
  END_TEST;
}

int UtcDaliMarkdownViewMiddleInsertionKeepsExistingLayoutOrderP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("A\n\nC");
  DALI_TEST_EQUALS(view.GetChildViewCount(), 2u, TEST_LOCATION);
  View firstChild  = view.GetChildViewAt(0u);
  View secondChild = view.GetChildViewAt(1u);

  view.SetMarkdown("A\n\nB\n\nC");
  DALI_TEST_EQUALS(view.GetChildViewCount(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(firstChild == view.GetChildViewAt(0u));
  DALI_TEST_CHECK(secondChild != view.GetChildViewAt(1u));
  END_TEST;
}

int UtcDaliMarkdownViewSamePathStyleChangeReusesLeafP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("This is **impor");
  DALI_TEST_EQUALS(view.GetChildViewCount(), 1u, TEST_LOCATION);
  View firstChild = view.GetChildViewAt(0u);

  view.SetMarkdown("This is **important**");

  DALI_TEST_EQUALS(view.GetChildViewCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(firstChild == view.GetChildViewAt(0u));
  END_TEST;
}

int UtcDaliMarkdownViewContainerHierarchyP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("- item\n  - nested\n\n> quote\n\n| A | B |\n|---|---|\n| 1 | 2 |\n");

  const auto& snapshot = Dali::Ui::Internal::GetImpl(view).GetSnapshot();
  DALI_TEST_CHECK(FindFirstRole(snapshot, MarkdownRenderRole::LIST));
  DALI_TEST_CHECK(FindFirstRole(snapshot, MarkdownRenderRole::QUOTE));
  DALI_TEST_CHECK(FindFirstRole(snapshot, MarkdownRenderRole::TABLE));
  DALI_TEST_CHECK(FindText(snapshot, "item"));
  DALI_TEST_CHECK(FindText(snapshot, "nested"));
  DALI_TEST_EQUALS(view.GetChildViewCount(), 3u, TEST_LOCATION);

  View list = view.GetChildViewAt(0u);
  DALI_TEST_CHECK(list);
  DALI_TEST_EQUALS(list.GetChildViewCount(), 1u, TEST_LOCATION);
  View listItem = list.GetChildViewAt(0u);
  DALI_TEST_CHECK(listItem);
  DALI_TEST_EQUALS(listItem.GetChildViewCount(), 2u, TEST_LOCATION);
  View listContent = listItem.GetChildViewAt(1u);
  DALI_TEST_CHECK(listContent);
  DALI_TEST_CHECK(listContent.GetChildViewCount() >= 2u);
  DALI_TEST_CHECK(Label::DownCast(listContent.GetChildViewAt(0u)));

  View quote = view.GetChildViewAt(1u);
  DALI_TEST_CHECK(quote);
  DALI_TEST_EQUALS(quote.GetChildViewCount(), 2u, TEST_LOCATION);
  View quoteContent = quote.GetChildViewAt(1u);
  DALI_TEST_CHECK(quoteContent);
  DALI_TEST_CHECK(quoteContent.GetChildViewCount() > 0u);

  View table = view.GetChildViewAt(2u);
  DALI_TEST_CHECK(table);
  DALI_TEST_EQUALS(table.GetChildViewCount(), 2u, TEST_LOCATION);
  View tableHead = table.GetChildViewAt(0u);
  DALI_TEST_CHECK(tableHead);
  DALI_TEST_EQUALS(tableHead.GetChildViewCount(), 2u, TEST_LOCATION);
  View tableHeadContent = tableHead.GetChildViewAt(0u);
  DALI_TEST_CHECK(tableHeadContent);
  DALI_TEST_EQUALS(tableHeadContent.GetChildViewCount(), 1u, TEST_LOCATION);
  View tableHeaderRow = tableHeadContent.GetChildViewAt(0u);
  DALI_TEST_CHECK(tableHeaderRow);
  DALI_TEST_EQUALS(tableHeaderRow.GetChildViewCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(tableHead.GetChildViewAt(1u));
  View tableCell = tableHeaderRow.GetChildViewAt(0u);
  DALI_TEST_CHECK(tableCell);
  DALI_TEST_CHECK(tableCell.GetChildViewCount() > 0u);
  END_TEST;
}

int UtcDaliMarkdownViewUnorderedListMarkerGeometryP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();
  view.SetMarkdown("- level 1\n  - level 2\n    - level 3\n      - level 4");

  View depth1 = GetShapeMarker(GetListItemAtDepth(view, 1u));
  View depth2 = GetShapeMarker(GetListItemAtDepth(view, 2u));
  View depth3 = GetShapeMarker(GetListItemAtDepth(view, 3u));
  View depth4 = GetShapeMarker(GetListItemAtDepth(view, 4u));
  DALI_TEST_CHECK(depth1);
  DALI_TEST_CHECK(depth2);
  DALI_TEST_CHECK(depth3);
  DALI_TEST_CHECK(depth4);

  for(uint32_t depth = 1u; depth <= 4u; ++depth)
  {
    CheckDefaultUnorderedMarkerGeometry(GetListItemAtDepth(view, depth));
  }

  DALI_TEST_EQUALS(depth1.GetBackgroundColor().GetRgba(), Color::BLACK, TEST_LOCATION);
  DALI_TEST_EQUALS(depth1.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(depth1.GetCornerRadius(), Vector4(0.5f, 0.5f, 0.5f, 0.5f), TEST_LOCATION);

  DALI_TEST_CHECK(depth2.GetBackgroundColor().IsNone());
  DALI_TEST_EQUALS(depth2.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(depth2.GetCornerRadius(), Vector4(0.5f, 0.5f, 0.5f, 0.5f), TEST_LOCATION);
  DALI_TEST_EQUALS(depth2.GetBorderlineWidth(), 1.25f, TEST_LOCATION);
  DALI_TEST_EQUALS(depth2.GetBorderlineColor().GetRgba(), Color::BLACK, TEST_LOCATION);

  DALI_TEST_EQUALS(depth3.GetBackgroundColor().GetRgba(), Color::BLACK, TEST_LOCATION);
  DALI_TEST_EQUALS(depth3.GetCornerRadius(), Vector4::ZERO, TEST_LOCATION);

  DALI_TEST_EQUALS(depth4.GetBackgroundColor().GetRgba(), Color::BLACK, TEST_LOCATION);
  DALI_TEST_EQUALS(depth4.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(depth4.GetCornerRadius(), Vector4(0.5f, 0.5f, 0.5f, 0.5f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownViewUnorderedListMarkerLifecycleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("- first line  \n  second line");
  View listItem   = GetListItemAtDepth(view, 1u);
  View marker     = GetShapeMarker(listItem);
  View markerHost = GetListItemMarkerHost(listItem);
  DALI_TEST_CHECK(marker);
  DALI_TEST_CHECK(markerHost);
  CheckDefaultUnorderedMarkerGeometry(listItem);

  View contentHost = listItem.GetChildViewAt(1u);
  DALI_TEST_CHECK(contentHost);
  Label textLabel = Label::DownCast(contentHost.GetChildViewAt(0u));
  DALI_TEST_CHECK(textLabel);
  DALI_TEST_EQUALS(textLabel.GetFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(textLabel.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(textLabel.GetLineHeight(), 1.6f, TEST_LOCATION);
  DALI_TEST_CHECK(!textLabel.IsSystemFontSizeScaleEnabled());

  view.SetMarkdown("- first line  \n  second line");
  DALI_TEST_CHECK(marker == GetShapeMarker(GetListItemAtDepth(view, 1u)));

  view.SetMarkdown("- first line extended  \n  second line");
  listItem = GetListItemAtDepth(view, 1u);
  DALI_TEST_CHECK(marker == GetShapeMarker(listItem));
  DALI_TEST_CHECK(markerHost == GetListItemMarkerHost(listItem));

  view.SetMarkdown("1. ordered");
  listItem          = GetListItemAtDepth(view, 1u);
  markerHost        = GetListItemMarkerHost(listItem);
  Label markerLabel = Label::DownCast(markerHost.GetChildViewAt(0u));
  DALI_TEST_CHECK(markerLabel);
  DALI_TEST_EQUALS(markerHost.GetRequestedWidth(), 32.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(markerLabel.GetRequestedWidth(), 32.0f, TEST_LOCATION);

  view.SetMarkdown("1. [ ] todo\n2. [x] done");
  View taskList = view.GetChildViewAt(0u);
  DALI_TEST_CHECK(taskList);
  DALI_TEST_EQUALS(taskList.GetChildViewCount(), 2u, TEST_LOCATION);
  View  firstTaskItem    = taskList.GetChildViewAt(0u);
  View  secondTaskItem   = taskList.GetChildViewAt(1u);
  Label firstTaskMarker  = Label::DownCast(GetShapeMarker(firstTaskItem));
  Label secondTaskMarker = Label::DownCast(GetShapeMarker(secondTaskItem));
  DALI_TEST_CHECK(firstTaskMarker);
  DALI_TEST_CHECK(secondTaskMarker);
  DALI_TEST_EQUALS(firstTaskMarker.GetText(), Dali::String("1."), TEST_LOCATION);
  DALI_TEST_EQUALS(secondTaskMarker.GetText(), Dali::String("2."), TEST_LOCATION);
  CheckBox firstTaskCheckbox  = GetListItemTaskCheckBox(firstTaskItem);
  CheckBox secondTaskCheckbox = GetListItemTaskCheckBox(secondTaskItem);
  DALI_TEST_CHECK(firstTaskCheckbox);
  DALI_TEST_CHECK(secondTaskCheckbox);
  DALI_TEST_CHECK(!firstTaskCheckbox.IsSelected());
  DALI_TEST_CHECK(secondTaskCheckbox.IsSelected());
  Label firstTaskText  = GetListItemTextLabel(firstTaskItem);
  Label secondTaskText = GetListItemTextLabel(secondTaskItem);
  DALI_TEST_CHECK(firstTaskText);
  DALI_TEST_CHECK(secondTaskText);
  DALI_TEST_EQUALS(firstTaskText.GetText(), Dali::String("todo"), TEST_LOCATION);
  DALI_TEST_EQUALS(secondTaskText.GetText(), Dali::String("done"), TEST_LOCATION);
  DALI_TEST_EQUALS(firstTaskItem.GetMargin().start, 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(secondTaskItem.GetMargin().start, 12.0f, TEST_LOCATION);

  view.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(firstTaskMarker.GetText(), Dali::String(".1"), TEST_LOCATION);
  DALI_TEST_EQUALS(secondTaskMarker.GetText(), Dali::String(".2"), TEST_LOCATION);
  DALI_TEST_CHECK(!firstTaskCheckbox.IsSelected());
  DALI_TEST_CHECK(secondTaskCheckbox.IsSelected());
  view.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
  DALI_TEST_EQUALS(firstTaskMarker.GetText(), Dali::String("1."), TEST_LOCATION);
  DALI_TEST_EQUALS(secondTaskMarker.GetText(), Dali::String("2."), TEST_LOCATION);

  view.SetMarkdown("- [ ] task");
  listItem        = GetListItemAtDepth(view, 1u);
  markerHost      = GetListItemMarkerHost(listItem);
  View taskBullet = GetShapeMarker(listItem);
  DALI_TEST_CHECK(taskBullet);
  DALI_TEST_CHECK(!Label::DownCast(taskBullet));
  CheckBox taskCheckBox = GetListItemTaskCheckBox(listItem);
  DALI_TEST_CHECK(taskCheckBox);
  DALI_TEST_CHECK(!taskCheckBox.IsSelected());
  DALI_TEST_EQUALS(taskCheckBox.GetRequestedWidth(), 20.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(taskCheckBox.GetRequestedHeight(), 32.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(markerHost.GetRequestedWidth(), 32.0f, 0.01f, TEST_LOCATION);

  View  taskRow  = GetListItemTaskRow(listItem);
  Label taskText = GetListItemTextLabel(listItem);
  view.SetMarkdown("- [x] task");
  listItem = GetListItemAtDepth(view, 1u);
  DALI_TEST_CHECK(GetListItemMarkerHost(listItem) == markerHost);
  DALI_TEST_CHECK(GetShapeMarker(listItem) == taskBullet);
  DALI_TEST_CHECK(GetListItemTaskRow(listItem) == taskRow);
  DALI_TEST_CHECK(GetListItemTaskCheckBox(listItem) == taskCheckBox);
  DALI_TEST_CHECK(GetListItemTextLabel(listItem) == taskText);
  DALI_TEST_CHECK(taskCheckBox.IsSelected());
  DALI_TEST_CHECK(taskText.IsAccessibilityHidden());

  view.SetMarkdown("- [ ]\n  - nested");
  listItem = GetListItemAtDepth(view, 1u);
  taskRow  = GetListItemTaskRow(listItem);
  DALI_TEST_CHECK(taskRow);
  DALI_TEST_EQUALS(taskRow.GetChildViewCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(GetListItemTaskCheckBox(listItem));
  DALI_TEST_CHECK(!GetListItemTextLabel(listItem));
  View taskContentHost = listItem.GetChildViewAt(1u);
  DALI_TEST_EQUALS(taskContentHost.GetChildViewCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(taskContentHost.GetChildViewAt(0u) == taskRow);
  DALI_TEST_CHECK(taskContentHost.GetChildViewAt(1u));

  view.SetMarkdown("- unordered again");
  listItem = GetListItemAtDepth(view, 1u);
  DALI_TEST_CHECK(GetShapeMarker(listItem));
  DALI_TEST_CHECK(!Label::DownCast(GetShapeMarker(listItem)));
  DALI_TEST_CHECK(!GetListItemTextLabel(listItem).IsAccessibilityHidden());
  END_TEST;
}

int UtcDaliMarkdownViewLinkTextProjectionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  const std::vector<std::pair<const char*, const char*>> cases = {
    {"[DALi](https://example.com)", "DALi [https://example.com]"},
    {"See [DALi](https://example.com) for details.", "See DALi [https://example.com] for details."},
    {"[empty]()", "empty []"},
    {"[](https://example.com)", " [https://example.com]"},
    {"[DALi](https://example.com \"title\")", "DALi [https://example.com]"},
    {"<https://example.com>", "https://example.com [https://example.com]"},
    {"[A](a) and [B](b)", "A [a] and B [b]"},
    {"[A](a)[B](b)", "A [a]B [b]"},
    {"[링크](url)", "링크 [url]"},
    {"[DALi](https://예시.테스트/문서)", "DALi [https://예시.테스트/문서]"},
  };

  for(const auto& testCase : cases)
  {
    CheckRenderedUrlText(view, testCase.first, testCase.second);
  }
  END_TEST;
}

int UtcDaliMarkdownViewImageTextProjectionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  const std::vector<std::pair<const char*, const char*>> cases = {
    {"![diagram](diagram.png)", "diagram [diagram.png]"},
    {"Text ![icon](icon.png) continues", "Text icon [icon.png] continues"},
    {"![one](one.png) ![two](two.png)", "one [one.png] two [two.png]"},
    {"![](empty.png)", " [empty.png]"},
    {"![empty]()", "empty []"},
    {"[![diagram](diagram.png)](page.html)", "diagram [diagram.png] [page.html]"},
    {"x[](link)![](image)", "x [link] [image]"},
    {"x![](image)[](link)", "x [image] [link]"},
    {"한글 ![그림](이미지.png) 끝", "한글 그림 [이미지.png] 끝"},
  };

  for(const auto& testCase : cases)
  {
    CheckRenderedUrlText(view, testCase.first, testCase.second);
  }

  view.SetMarkdown("Before ![*icon*](icon.png) after");
  Label label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "Before icon [icon.png] after", TEST_LOCATION);
  const Text::StyledText styledText = label.GetStyledText();
  DALI_TEST_CHECK(styledText);
  DALI_TEST_CHECK(HasFontSpanRange(styledText, 7u, 11u));
  DALI_TEST_CHECK(!HasFontSpanRange(styledText, 11u, 22u));

  view.SetMarkdown("Before ![icon](one.png) after");
  View  inlineRoot  = view.GetChildViewAt(0u);
  Label inlineLabel = GetOnlyRenderedLabel(view);
  view.SetMarkdown("Before ![icon](two.png) after");
  DALI_TEST_CHECK(view.GetChildViewAt(0u) == inlineRoot);
  DALI_TEST_CHECK(GetOnlyRenderedLabel(view) == inlineLabel);
  DALI_TEST_EQUALS(inlineLabel.GetText(), "Before icon [two.png] after", TEST_LOCATION);

  view.SetMarkdown("- ![](list.png)\n\n| Image |\n|---|\n| ![](cell.png) |");
  std::vector<Label> labels;
  CollectLabels(view, labels);
  DALI_TEST_CHECK(HasLabelText(labels, " [list.png]"));
  DALI_TEST_CHECK(HasLabelText(labels, " [cell.png]"));

  DALI_TEST_EQUALS(MarkdownView::ToPlainText("x[](link)![](image)"), Dali::String("x [link] [image]"), TEST_LOCATION);
  DALI_TEST_EQUALS(MarkdownView::ToPlainText("x![](image)[](link)"), Dali::String("x [image] [link]"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownViewLinkStyleProjectionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("[**bold link**](url)");
  Label label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "bold link [url]", TEST_LOCATION);
  Text::StyledText styledText = CheckStyledLinkPresentation(label);
  DALI_TEST_CHECK(HasFontSpanRange(styledText, 0u, 9u));

  view.SetMarkdown("[*italic*](url)");
  label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "italic [url]", TEST_LOCATION);
  styledText = CheckStyledLinkPresentation(label);
  DALI_TEST_CHECK(HasFontSpanRange(styledText, 0u, 6u));

  view.SetMarkdown("[***styled***](url)");
  label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "styled [url]", TEST_LOCATION);
  styledText = CheckStyledLinkPresentation(label);
  DALI_TEST_CHECK(HasFontSpanRange(styledText, 0u, 6u));

  view.SetMarkdown("[`code`](url)");
  label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "code [url]", TEST_LOCATION);
  styledText = CheckStyledLinkPresentation(label);
  DALI_TEST_CHECK(HasFontSpanRange(styledText, 0u, 4u));
  DALI_TEST_CHECK(HasBackgroundSpanRange(styledText, 0u, 4u));

  view.SetMarkdown("[link](url) **after**");
  label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "link [url] after", TEST_LOCATION);
  styledText = CheckStyledLinkPresentation(label);
  DALI_TEST_CHECK(HasFontSpanRange(styledText, 11u, 16u));

  view.SetMarkdown("**before** [link](url)");
  label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "before link [url]", TEST_LOCATION);
  styledText = CheckStyledLinkPresentation(label);
  DALI_TEST_CHECK(HasFontSpanRange(styledText, 0u, 6u));

  view.SetMarkdown("[A](a) **middle** [B](b)");
  label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "A [a] middle B [b]", TEST_LOCATION);
  styledText = CheckStyledLinkPresentation(label);
  DALI_TEST_CHECK(HasFontSpanRange(styledText, 6u, 12u));
  END_TEST;
}

int UtcDaliMarkdownViewLinkOuterStyleBoundaryP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("**before [link](url) after**");
  Label label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "before link [url] after", TEST_LOCATION);

  const Text::StyledText styledText = CheckStyledLinkPresentation(label);
  CheckFontStyleCoverage(styledText, 0u, 11u, true);
  CheckFontStyleCoverage(styledText, 11u, 17u, false);
  CheckFontStyleCoverage(styledText, 17u, 23u, true);

  view.SetMarkdown("**x[](url)y**");
  label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "x [url]y", TEST_LOCATION);

  const Text::StyledText zeroLengthStyledText = CheckStyledLinkPresentation(label);
  CheckFontStyleCoverage(zeroLengthStyledText, 0u, 1u, true);
  CheckFontStyleCoverage(zeroLengthStyledText, 1u, 7u, false);
  CheckFontStyleCoverage(zeroLengthStyledText, 7u, 8u, true);
  END_TEST;
}

int UtcDaliMarkdownViewLinkMultipleZeroLengthBoundariesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("**a[](u)b[](v)c**");
  const Label label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "a [u]b [v]c", TEST_LOCATION);

  const Text::StyledText styledText = CheckStyledLinkPresentation(label);
  CheckFontStyleCoverage(styledText, 0u, 1u, true);
  CheckFontStyleCoverage(styledText, 1u, 5u, false);
  CheckFontStyleCoverage(styledText, 5u, 6u, true);
  CheckFontStyleCoverage(styledText, 6u, 10u, false);
  CheckFontStyleCoverage(styledText, 10u, 11u, true);
  END_TEST;
}

int UtcDaliMarkdownViewLinkComponentContextsP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();
  view.SetMarkdown("# [Heading](heading-url)\n\n- [Item](item-url)\n\n> [Quote](quote-url)\n\n| Link |\n|---|\n| [Cell](cell-url) |");

  std::vector<Label> labels;
  CollectLabels(view, labels);
  DALI_TEST_CHECK(HasLabelText(labels, "Heading [heading-url]"));
  DALI_TEST_CHECK(HasLabelText(labels, "Item [item-url]"));
  DALI_TEST_CHECK(HasLabelText(labels, "Quote [quote-url]"));
  DALI_TEST_CHECK(HasLabelText(labels, "Cell [cell-url]"));
  END_TEST;
}

int UtcDaliMarkdownViewLinkReconciliationP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("[DALi](url-a)");
  View  firstChild = view.GetChildViewAt(0u);
  Label label      = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "DALi [url-a]", TEST_LOCATION);

  view.SetMarkdown("[DALi](url-a)");
  DALI_TEST_CHECK(firstChild == view.GetChildViewAt(0u));
  DALI_TEST_EQUALS(GetOnlyRenderedLabel(view).GetText(), "DALi [url-a]", TEST_LOCATION);

  view.SetMarkdown("[DALi](url-b)");
  DALI_TEST_CHECK(firstChild == view.GetChildViewAt(0u));
  DALI_TEST_EQUALS(GetOnlyRenderedLabel(view).GetText(), "DALi [url-b]", TEST_LOCATION);

  view.SetMarkdown("[Docs](url-b)");
  DALI_TEST_CHECK(firstChild == view.GetChildViewAt(0u));
  DALI_TEST_EQUALS(GetOnlyRenderedLabel(view).GetText(), "Docs [url-b]", TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownViewLinkNonLinkRegressionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  MarkdownView      view = MarkdownView::New();

  view.SetMarkdown("plain text");
  Label label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "plain text", TEST_LOCATION);
  DALI_TEST_CHECK(!label.GetStyledText());

  view.SetMarkdown("`[not a link](url)`");
  label = GetOnlyRenderedLabel(view);
  DALI_TEST_EQUALS(label.GetText(), "[not a link](url)", TEST_LOCATION);
  DALI_TEST_CHECK(label.GetStyledText());
  CheckNoAnchorSpans(label.GetStyledText());
  CheckNoUnderlineSpans(label.GetStyledText());

  view.SetMarkdown("Before <a href=\"url\">HTML</a> after");
  label = GetOnlyRenderedLabel(view);
  DALI_TEST_CHECK(std::string(label.GetText().CStr()).find(" [url]") == std::string::npos);
  END_TEST;
}

int UtcDaliMarkdownViewToPlainTextGoldenP(void)
{
  const char* markdown = R"MARKDOWN(# What is Self-Supervised Learning?

Self-supervised learning is a branch of machine learning where a model learns to predict part of its input from other parts of the input, **without requiring explicit labels**. This is in contrast to traditional supervised learning, which depends on labeled datasets.

---

## 🔍 Key Characteristics

- **No Manual Labels:** The model generates its own "labels" from the data.
- **Pretext Tasks:** Typical pretext tasks include:
  - *Predicting missing words* in a sentence
  - *Colorizing grayscale images*
  - *Predicting the next frame* in a video

---

> "Self-supervised learning unlocks the ability to learn from vast amounts of unlabeled data.")MARKDOWN"
"  \n"
R"MARKDOWN(> — Yann LeCun

---

## 🚀 Popular Approaches

| Approach   | Domain        | Example           |
|------------|---------------|------------------|
| Masked LM  | NLP           | BERT             |
| Contrastive| Vision        | SimCLR, MoCo     |
| Autoencoders | General     | Variational AE   |

### Sample Code (PyTorch)

```python
import torch.nn as nn

class AutoEncoder(nn.Module):
    def __init__(self):
        super().__init__()
        self.enc = nn.Linear(784, 64)
        self.dec = nn.Linear(64, 784)
    def forward(self, x):
        z = self.enc(x)
        return self.dec(z)
```

### 📝 Summary

- Self-supervised learning = **labels from data itself**
- Core to large language models (LLMs) and modern AI

For more info, see [LeCun's talk](https://youtu.be/MkN6a6C1b_w).
)MARKDOWN";

  const char* expected = R"PLAIN(What is Self-Supervised Learning?
Self-supervised learning is a branch of machine learning where a model learns to predict part of its input from other parts of the input, without requiring explicit labels. This is in contrast to traditional supervised learning, which depends on labeled datasets.

🔍 Key Characteristics
No Manual Labels: The model generates its own "labels" from the data.
Pretext Tasks: Typical pretext tasks include:
Predicting missing words in a sentence
Colorizing grayscale images
Predicting the next frame in a video

"Self-supervised learning unlocks the ability to learn from vast amounts of unlabeled data."
— Yann LeCun

🚀 Popular Approaches
Approach
Domain
Example
Masked LM
NLP
BERT
Contrastive
Vision
SimCLR, MoCo
Autoencoders
General
Variational AE
Sample Code (PyTorch)
python
import torch.nn as nn

class AutoEncoder(nn.Module):
    def __init__(self):
        super().__init__()
        self.enc = nn.Linear(784, 64)
        self.dec = nn.Linear(64, 784)
    def forward(self, x):
        z = self.enc(x)
        return self.dec(z)
📝 Summary
Self-supervised learning = labels from data itself
Core to large language models (LLMs) and modern AI
For more info, see LeCun's talk [https://youtu.be/MkN6a6C1b_w].)PLAIN";

  CheckPlainText(markdown, expected);
  END_TEST;
}

int UtcDaliMarkdownViewToPlainTextBasicsP(void)
{
  CheckPlainText("", "");
  CheckPlainText(" \t\r\n", "");
  CheckPlainText("plain paragraph", "plain paragraph");
  CheckPlainText("First paragraph.\n\nSecond paragraph.", "First paragraph.\nSecond paragraph.");
  CheckPlainText("# Heading 1\n\n## Heading 2", "Heading 1\nHeading 2");
  CheckPlainText("This is **bold**, *italic*, ~~strike~~ and `code`.", "This is bold, italic, strike and code.");
  CheckPlainText("First line\nSecond line", "First line\nSecond line");
  CheckPlainText("A &amp; B &#169; &#x2192;", "A & B © →");
  CheckPlainText("\\*literal\\* 한글 😀", "*literal* 한글 😀");
  CheckPlainText("First\n\n---\n\nSecond", "First\n\nSecond");
  CheckPlainText("---\n\nOnly text\n\n---\n\n", "\nOnly text");
  END_TEST;
}

int UtcDaliMarkdownViewToPlainTextListsAndQuotesP(void)
{
  CheckPlainText("- Alpha\n- Beta", "Alpha\nBeta");
  CheckPlainText("- Parent\n  - Child\n    - Grandchild", "Parent\nChild\nGrandchild");
  CheckPlainText("3. Alpha\n4. Beta", "3. Alpha\n4. Beta");
  CheckPlainText("1. Parent\n   1. Child\n   2. Next", "1. Parent\n1. Child\n2. Next");
  CheckPlainText("1. Ordered\n   - Unordered\n\n1. Reset", "1. Ordered\nUnordered\n2. Reset");
  CheckPlainText("1. First paragraph\n\n   Second paragraph", "1. First paragraph\n1. Second paragraph");
  CheckPlainText("- [x] done\n- [ ] todo", "[x] done\n[ ] todo");
  CheckPlainText("> First line\n> Second line", "First line\nSecond line");
  CheckPlainText("> - item\n>   1. ordered", "item\n1. ordered");
  END_TEST;
}

int UtcDaliMarkdownViewToPlainTextLinksImagesCodeAndTablesP(void)
{
  CheckPlainText("See [DALi](https://example.com \"title\").", "See DALi [https://example.com].");
  CheckPlainText("[empty]()", "empty []");
  CheckPlainText("<https://example.com>", "https://example.com [https://example.com]");
  CheckPlainText("![diagram](diagram.png \"title\")", "diagram [diagram.png]");
  CheckPlainText("![](empty.png)", " [empty.png]");
  CheckPlainText("A ![icon](icon.png) B", "A icon [icon.png] B");
  CheckPlainText("[![diagram](diagram.png)](page.html)", "diagram [diagram.png] [page.html]");
  CheckPlainText("```python\nprint(\"Hello\")\n```", "python\nprint(\"Hello\")");
  CheckPlainText("```\nplain code\n```", "\nplain code");
  CheckPlainText("```text\n  indented\n\n  next\n```\n\n# After", "text\n  indented\n\n  next\nAfter");
  CheckPlainText("```text\nalpha\r\nbeta\r\n```", "text\nalpha\nbeta");
  CheckPlainText("```\n```", "");
  CheckPlainText("| A | B |\n|---|---|\n| 1 | 2 |", "A\nB\n1\n2");
  CheckPlainText("| A |  | **B** |\n|---|---|---|\n| [C](url) | D | E |", "A\n\nB\nC [url]\nD\nE");
  CheckPlainText("Before <span>HTML</span> after", "Before HTML after");
  CheckPlainText("<div>\nHTML block\n</div>\n\nAfter", "\nAfter");
  CheckPlainText("Open **strong", "Open **strong");
  CheckPlainText("`unterminated", "`unterminated");

  const Dali::String first  = MarkdownView::ToPlainText("Repeated [call](url)");
  const Dali::String second = MarkdownView::ToPlainText("Repeated [call](url)");
  DALI_TEST_EQUALS(first, "Repeated call [url]", TEST_LOCATION);
  DALI_TEST_EQUALS(second, "Repeated call [url]", TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownViewToPlainTextLargeSparseEventsP(void)
{
  const std::string filler(12000u, 'a');
  const std::string markdown = filler + " [![diagram](diagram.png)](page.html) tail";
  const std::string expected = filler + " diagram [diagram.png] [page.html] tail";

  const Dali::String plainText = MarkdownView::ToPlainText(Dali::String(markdown.c_str()));
  DALI_TEST_EQUALS(plainText, Dali::String(expected.c_str()), TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkdownViewToPlainTextCrlfP(void)
{
  CheckPlainText("First\r\nSecond\r\n\r\nThird", "First\nSecond\nThird");
  CheckPlainText("- Alpha\r\n- Beta\r\n\r\n| A | B |\r\n|---|---|\r\n| 1 | 2 |\r\n", "Alpha\nBeta\nA\nB\n1\n2");
  CheckPlainText("```text\r\nalpha\r\nbeta\r\n```\r\n\r\nAfter", "text\nalpha\nbeta\nAfter");
  END_TEST;
}

int UtcDaliMarkdownViewToPlainTextUtf8BoundariesP(void)
{
  CheckPlainText("한글 😀 [링크](url) ![그림](img.png)", "한글 😀 링크 [url] 그림 [img.png]");
  CheckPlainText("😀  \n다음", "😀\n다음");
  CheckPlainText("한글\n😀", "한글\n😀");
  END_TEST;
}
