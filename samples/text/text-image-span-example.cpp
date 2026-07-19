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
 */

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr std::size_t CASE_COUNT = 31u;
constexpr std::array<float, 5u> WIDTH_SWEEP = {180.0f, 220.0f, 260.0f, 340.0f, 0.0f};
constexpr uint32_t LIFECYCLE_INTERVAL_MS = 250u;
constexpr uint32_t LIFECYCLE_PHASE_COUNT = 12u;
constexpr const char* REMOTE_IMAGE_URL =
  "https://www.w3.org/assets/logos/w3c-2025-transitional/w3c-72x48.png";

struct MemorySnapshot
{
  long rssKb{0};
  long hwmKb{0};
};

MemorySnapshot ReadMemory()
{
  MemorySnapshot snapshot;
  if(FILE* file = std::fopen("/proc/self/status", "r"))
  {
    char line[128];
    while(std::fgets(line, sizeof(line), file))
    {
      if(std::strncmp(line, "VmRSS:", 6u) == 0)
      {
        std::sscanf(line + 6u, "%ld", &snapshot.rssKb);
      }
      else if(std::strncmp(line, "VmHWM:", 6u) == 0)
      {
        std::sscanf(line + 6u, "%ld", &snapshot.hwmKb);
      }
    }
    std::fclose(file);
  }
  return snapshot;
}

const char* LifecyclePhaseName(uint32_t phase)
{
  switch(phase)
  {
    case 0u: return "persistent set (local+web)";
    case 1u: return "next sample-case update";
    case 2u: return "builder ClearSpans";
    case 3u: return "plain/empty source clear";
    case 4u: return "transient Label add";
    case 5u: return "shared StyledText update";
    case 6u: return "transient Label detach";
    case 7u: return "active transient destroy";
    case 8u: return "markup-composed set";
    case 9u: return "markup-composed update";
    case 10u: return "markup clear/active branch";
    case 11u:
    default: return "markup Label detach/destroy";
  }
}

struct ImageSpec
{
  uint32_t                              start{0u};
  uint32_t                              end{0u};
  const char*                           source{nullptr};
  Vector2                               size{24.0f, 24.0f};
  Text::ImageAttributes::InlineAlignment alignment{Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM};
  float                                 verticalOffset{0.0f};
  const char*                           alternativeText{nullptr};
};

struct CaseData
{
  const char*                     title{nullptr};
  const char*                     expected{nullptr};
  std::string                     logicalText;
  std::vector<ImageSpec>          images;
  bool                            multiline{true};
  bool                            ellipsis{false};
  bool                            rtl{false};
  Text::Alignment                 horizontalAlignment{Text::Alignment::CENTER};
  Text::Alignment                 verticalAlignment{Text::Alignment::CENTER};
  Text::LineWrapMode              lineWrapMode{Text::LineWrapMode::WORD};
  float                           fontSize{28.0f};
  float                           renderScale{1.0f};
  bool                            textFit{false};
  bool                            marqueeCase{false};
  bool                            lifecycleCase{false};
};

enum class AlignmentOverride
{
  CASE_DEFAULT,
  START,
  CENTER,
  END
};

enum class ToggleOverride
{
  CASE_DEFAULT,
  OFF,
  ON
};

AlignmentOverride NextAlignmentOverride(AlignmentOverride value)
{
  switch(value)
  {
    case AlignmentOverride::CASE_DEFAULT: return AlignmentOverride::START;
    case AlignmentOverride::START: return AlignmentOverride::CENTER;
    case AlignmentOverride::CENTER: return AlignmentOverride::END;
    case AlignmentOverride::END:
    default: return AlignmentOverride::CASE_DEFAULT;
  }
}

ToggleOverride NextToggleOverride(ToggleOverride value)
{
  switch(value)
  {
    case ToggleOverride::CASE_DEFAULT: return ToggleOverride::OFF;
    case ToggleOverride::OFF: return ToggleOverride::ON;
    case ToggleOverride::ON:
    default: return ToggleOverride::CASE_DEFAULT;
  }
}

ImageSpec ImageAt(const std::string&                       text,
                  const char*                              marker,
                  const char*                              source,
                  Vector2                                  size,
                  Text::ImageAttributes::InlineAlignment alignment = Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM,
                  float                                    verticalOffset = 0.0f,
                  std::size_t                              occurrence = 0u)
{
  const std::string markerText(marker);
  std::size_t       start      = 0u;
  for(std::size_t index = 0u; index <= occurrence; ++index)
  {
    start = text.find(markerText, index == 0u ? 0u : start + markerText.size());
  }
  DALI_ASSERT_ALWAYS(start != std::string::npos && "ImageSpan sample marker is missing");

  auto countUtf8Characters = [](const char* bytes, std::size_t byteCount)
  {
    uint32_t count = 0u;
    for(std::size_t index = 0u; index < byteCount; ++index)
    {
      if((static_cast<uint8_t>(bytes[index]) & 0xC0u) != 0x80u)
      {
        ++count;
      }
    }
    return count;
  };

  const uint32_t characterStart  = countUtf8Characters(text.data(), start);
  const uint32_t characterLength = countUtf8Characters(markerText.data(), markerText.size());
  return {characterStart,
          characterStart + characterLength,
          source,
          size,
          alignment,
          verticalOffset};
}

ImageSpec ObjectAt(const std::string&                       text,
                   std::size_t                              occurrence,
                   const char*                              source,
                   Vector2                                  size,
                   Text::ImageAttributes::InlineAlignment alignment = Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM,
                   float                                    verticalOffset = 0.0f)
{
  return ImageAt(text, "\uFFFC", source, size, alignment, verticalOffset, occurrence);
}

const char* AlignmentName(Text::ImageAttributes::InlineAlignment alignment)
{
  switch(alignment)
  {
    case Text::ImageAttributes::InlineAlignment::TEXT_BASELINE: return "TEXT_BASELINE";
    case Text::ImageAttributes::InlineAlignment::TEXT_CENTER: return "TEXT_CENTER";
    case Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM:
    default: return "TEXT_BOTTOM";
  }
}

const char* TextAlignmentName(Text::Alignment alignment)
{
  switch(alignment)
  {
    case Text::Alignment::START: return "START";
    case Text::Alignment::END: return "END";
    case Text::Alignment::CENTER:
    default: return "CENTER";
  }
}

const char* OverrideName(bool caseDefault)
{
  return caseDefault ? "case" : "override";
}

Label NewHudLabel(const char* text, float height, uint32_t background, bool interactive = false)
{
  Label label = Label::New(text);
  label.SetFontSize(interactive ? 15.0f : 17.0f);
  label.SetTextColor(UiColor(0xF8FAFC));
  label.SetBackgroundColor(UiColor(background));
  label.SetPadding(Extents(10, 10, 5, 5));
  label.SetMultiLine(true);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(height);
  label.SetCornerRadius(6.0f);
  if(interactive)
  {
    label.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  }
  return label;
}
} // unnamed namespace

class TextImageSpanController : public ConnectionTracker
{
public:
  explicit TextImageSpanController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextImageSpanController::OnInit);
  }

private:
  std::string Resource(const char* file) const
  {
    return std::string(RESOURCES_DIR) + file;
  }

  std::string ResolveImageSource(const char* source) const
  {
    const std::string sourceText(source ? source : "");
    if(sourceText.rfind("https://", 0u) == 0u || sourceText.rfind("http://", 0u) == 0u)
    {
      return sourceText;
    }
    return Resource(sourceText.c_str());
  }

  CaseData GetCase(std::size_t index) const
  {
    switch(index)
    {
      case 0u:
      {
        const std::string text = "A\uFFFCB";
        return {"1. One U+FFFC", "The object replacement character is replaced by one TEXT_BOTTOM image.",
                text, {ObjectAt(text, 0u, "flag_kr.png", Vector2(24.0f, 24.0f))}, false};
      }
      case 1u:
        return {"2. Multi-character exact range", "[icon] disappears as one atomic unit; toggling replacement restores it.",
                "Press [icon] to continue", {{6u, 12u, "flag_kr.png", Vector2(32.0f, 24.0f)}}};
      case 2u:
      {
        const std::string text = "X\uFFFC\uFFFC\uFFFCY";
        return {"3. Multiple and adjacent", "Three U+FFFC occurrences use three ImageSpan handles; adjacent boxes remain distinct.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(24.0f, 24.0f)),
                 ObjectAt(text, 1u, "flag_kr.png", Vector2(24.0f, 24.0f)),
                 ObjectAt(text, 2u, "flag_us.png", Vector2(24.0f, 24.0f))}};
      }
      case 3u:
      {
        const std::string text = "baseline \uFFFC bottom \uFFFC center \uFFFC up \uFFFC down \uFFFC";
        return {"4. Alignment and vertical offset", "All alignments are visible; positive offset moves down and negative offset moves up.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(30.0f, 38.0f), Text::ImageAttributes::InlineAlignment::TEXT_BASELINE),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(30.0f, 24.0f), Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM),
                 ObjectAt(text, 2u, "flag_ae.png", Vector2(30.0f, 30.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER),
                 ObjectAt(text, 3u, "flag_kr.png", Vector2(24.0f, 24.0f), Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM, -8.0f),
                 ObjectAt(text, 4u, "flag_us.png", Vector2(24.0f, 24.0f), Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM, 8.0f)}};
      }
      case 4u:
      {
        const std::string text = "before \uFFFC after\n\uFFFC\nend";
        return {"5. Multiline and replacement-only line", "Width sweep wraps around a wide box and retains the image-only line.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(120.0f, 60.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(40.0f, 40.0f))}};
      }
      case 5u:
      {
        const std::string text =
          "This is a long paragraph with an inline \uFFFC image placed between ordinary words. Resize the preview to observe how the sentence wraps before and after the image. A second \uFFFC marker appears later while several more words continue across multiple lines.";
        return {"6. Long multiline prose", "Two U+FFFC images remain in flow while the paragraph wraps across many lines.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(36.0f, 26.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(36.0f, 26.0f))}};
      }
      case 6u:
      {
        const std::string text =
          "Text before the oversized inline image explains the first line. The next phrase contains \uFFFC in continuous prose, followed by enough text to wrap onto several more lines and show the expanded line box clearly.";
        return {"7. Large image inside multiline text", "The 180x130 box expands its line while surrounding prose continues above and below it.",
                text,
                {ObjectAt(text, 0u, "flag_us.png", Vector2(180.0f, 130.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)}};
      }
      case 7u:
      {
        const std::string text =
          "Sizes \uFFFC 8x8, \uFFFC 12x12, \uFFFC 16x16, \uFFFC 24x24, \uFFFC 32x20, \uFFFC 40x40, \uFFFC 64x32, \uFFFC 80x48, and \uFFFC 120x60 are distributed through multiline prose so every box participates in wrapping.";
        return {"8. Nine image sizes", "Required tiny, rectangular, square and large boxes exercise same-line and multiline layout.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(8.0f, 8.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(12.0f, 12.0f)),
                 ObjectAt(text, 2u, "flag_ae.png", Vector2(16.0f, 16.0f)),
                 ObjectAt(text, 3u, "flag_kr.png", Vector2(24.0f, 24.0f)),
                 ObjectAt(text, 4u, "flag_us.png", Vector2(32.0f, 20.0f)),
                 ObjectAt(text, 5u, "flag_ae.png", Vector2(40.0f, 40.0f)),
                 ObjectAt(text, 6u, "flag_kr.png", Vector2(64.0f, 32.0f)),
                 ObjectAt(text, 7u, "flag_us.png", Vector2(80.0f, 48.0f)),
                 ObjectAt(text, 8u, "flag_ae.png", Vector2(120.0f, 60.0f))}};
      }
      case 8u:
      {
        const std::string text = "LTR אבג \uFFFC العربية end";
        return {"9. RTL and mixed bidi", "The U+FFFC image remains one visual unit between Hebrew and Arabic text.",
                text, {ObjectAt(text, 0u, "flag_ae.png", Vector2(34.0f, 24.0f))}, true, false, true};
      }
      case 9u:
      {
        const std::string text = "Long prefix words \uFFFC trailing text";
        return {"10. END ellipsis atomicity", "Width sweep places END ellipsis before or after the whole image, never through it.",
                text, {ObjectAt(text, 0u, "flag_kr.png", Vector2(70.0f, 28.0f))}, false, true};
      }
      case 10u:
      {
        const std::string text = "Atomic\uFFFCsuffix without spaces still has a stable END ellipsis boundary";
        return {"11. END ellipsis without spaces", "An adjacent image remains atomic without whitespace on either side.",
                text, {ObjectAt(text, 0u, "flag_us.png", Vector2(64.0f, 32.0f))}, false, true, false,
                Text::Alignment::START, Text::Alignment::CENTER, Text::LineWrapMode::CHARACTER};
      }
      case 11u:
      {
        const std::string text = "\uFFFC image starts a long single line whose remaining words overflow the preview";
        return {"12. END ellipsis with leading image", "The leading image is retained or removed as one complete unit.",
                text, {ObjectAt(text, 0u, "flag_ae.png", Vector2(92.0f, 38.0f))}, false, true, false,
                Text::Alignment::START};
      }
      case 12u:
      {
        const std::string text = "A long single line ends with one final replacement \uFFFC";
        return {"13. END ellipsis with trailing image", "The final image cannot remain after ellipsis or become partially clipped.",
                text, {ObjectAt(text, 0u, "flag_kr.png", Vector2(72.0f, 30.0f))}, false, true, false,
                Text::Alignment::END};
      }
      case 13u:
      {
        const std::string text = "Marquee \uFFFC must remain static even when the surrounding line is deliberately long";
        CaseData data{"14. Marquee blocked", "Starting marquee must leave replacement content in its static layout.",
                      text, {ObjectAt(text, 0u, "flag_us.png", Vector2(58.0f, 28.0f))}, false};
        data.marqueeCase = true;
        return data;
      }
      case 14u:
      {
        const std::string text = "Sizes \uFFFC then \uFFFC then \uFFFC and ordinary trailing words";
        return {"15. END ellipsis with mixed sizes", "Widths exercise ellipsis before, on and after differently sized boxes.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(8.0f, 8.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(120.0f, 60.0f)),
                 ObjectAt(text, 2u, "flag_ae.png", Vector2(40.0f, 40.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)},
                false, true, false, Text::Alignment::START};
      }
      case 15u:
      {
        const std::string text = "One \uFFFC two \uFFFC three \uFFFC followed by ordinary text before the end";
        return {"16. END ellipsis between images", "Every threshold exposes a monotonic prefix with no stale visual after ellipsis.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(48.0f, 28.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(68.0f, 32.0f)),
                 ObjectAt(text, 2u, "flag_ae.png", Vector2(52.0f, 36.0f))},
                false, true, false, Text::Alignment::END};
      }
      case 16u:
      {
        const std::string text = "Readable prefix \uFFFC followed by a sentence that will be elided cleanly";
        return {"17. END ellipsis on image boundary", "At the threshold, U+FFFC changes directly into ellipsis without an image-sized blank gap.",
                text, {ObjectAt(text, 0u, "flag_kr.png", Vector2(82.0f, 30.0f))}, false, true};
      }
      case 17u:
      {
        const std::string text = "\uFFFC";
        return {"18. Image-only END ellipsis", "The only image is either fully visible or replaced by ellipsis without a residual box.",
                text, {ObjectAt(text, 0u, "flag_us.png", Vector2(120.0f, 60.0f))}, false, true};
      }
      case 18u:
      {
        const std::string text = "ordinary text before the final \uFFFC";
        return {"19. Text plus image END ellipsis", "The trailing image is never retained to the right of the ellipsis.",
                text, {ObjectAt(text, 0u, "flag_ae.png", Vector2(80.0f, 48.0f))}, false, true, false,
                Text::Alignment::START};
      }
      case 19u:
      {
        const std::string text = "\uFFFC ordinary text after the leading replacement keeps extending";
        return {"20. Image plus text END ellipsis", "A visible leading image remains before ellipsis while only its following text is removed.",
                text, {ObjectAt(text, 0u, "flag_kr.png", Vector2(64.0f, 32.0f))}, false, true, false,
                Text::Alignment::START};
      }
      case 20u:
      {
        const std::string text = "sizes \uFFFC text \uFFFC\uFFFC text \uFFFC end";
        return {"21. Mixed-size END ellipsis sweep", "The 8x8, 24x24, 48x32 and 80x48 boxes cross the boundary independently and atomically.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(8.0f, 8.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(24.0f, 24.0f)),
                 ObjectAt(text, 2u, "flag_ae.png", Vector2(48.0f, 32.0f)),
                 ObjectAt(text, 3u, "flag_kr.png", Vector2(80.0f, 48.0f))},
                false, true};
      }
      case 21u:
      {
        const std::string text = "אבג LTR words \uFFFC continue inside an RTL paragraph until the END boundary";
        return {"22. RTL paragraph plus LTR END ellipsis", "Paragraph direction changes the physical END edge without changing replacement visibility.",
                text, {ObjectAt(text, 0u, "flag_ae.png", Vector2(56.0f, 30.0f))}, false, true, true,
                Text::Alignment::START};
      }
      case 22u:
      {
        const std::string text = "resource lifecycle \uFFFC keeps the same 64x32 reservation after success or failure";
        CaseData data{"23. Load failure and lifecycle", "Press L to alternate a valid and missing source; layout must not move.",
                      text,
                      {ObjectAt(text, 0u, mLifecycleAlternate ? "flag_kr.png" : "missing-image.png", Vector2(64.0f, 32.0f))},
                      false};
        data.lifecycleCase = true;
        return data;
      }
      case 23u:
      {
        const std::string text = "LTR prefix \uFFFC אבג العربية mixed direction trailing words for ellipsis";
        return {"24. END ellipsis in mixed bidi text", "RTL layout and mixed-direction text keep the replacement on the correct side of END ellipsis.",
                text, {ObjectAt(text, 0u, "flag_ae.png", Vector2(56.0f, 30.0f))}, false, true, true,
                Text::Alignment::START};
      }
      case 24u:
      {
        const std::string text =
          "A multiline paragraph begins with enough ordinary prose to create several wrapped lines. "
          "The first inline \uFFFC image should remain visible while more sentences continue through the preview. "
          "Additional words deliberately repeat the wrapping pressure so that vertical overflow selects a final visible line. "
          "Near that boundary a second \uFFFC image may either fit completely before the ellipsis or disappear completely. "
          "Everything after the boundary, including this third \uFFFC image and the remaining paragraph, must stay hidden. "
          "The final sentences provide enough length for both wide and narrow window sizes to exercise multiline END ellipsis.";
        return {"25. Multiline END ellipsis line boundary", "Only replacements before the final visible ellipsis boundary remain; later-line images are hidden.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(42.0f, 28.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(76.0f, 32.0f)),
                 ObjectAt(text, 2u, "flag_ae.png", Vector2(48.0f, 30.0f))},
                true, true, false, Text::Alignment::START, Text::Alignment::START};
      }
      case 25u:
      {
        const std::string text =
          "Centered multiline ellipsis begins with ordinary words and a small \uFFFC image. "
          "Several phrases follow to build multiple lines before a very wide \uFFFC replacement approaches the last visible line. "
          "More text and a tall \uFFFC replacement continue beyond it so resize can move the final boundary across different box sizes. "
          "This deliberately long tail keeps wrapping through additional sentences and verifies that no image can appear below or after ellipsis. "
          "One more sequence of ordinary words makes the vertical overflow deterministic at the wider preview size as well.";
        CaseData data{"26. Text-fit multiline END ellipsis", "Text-fit must preserve atomic mixed-size images at the final ellipsis boundary.",
                      text,
                      {ObjectAt(text, 0u, "flag_kr.png", Vector2(26.0f, 20.0f)),
                       ObjectAt(text, 1u, "flag_us.png", Vector2(150.0f, 38.0f)),
                       ObjectAt(text, 2u, "flag_ae.png", Vector2(44.0f, 88.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)},
                      true, true};
        data.textFit = true;
        return data;
      }
      case 26u:
      {
        const std::string text =
          "An oversized replacement follows wrapped introductory prose and tests vertical END ellipsis. "
          "More words place \uFFFC near a constrained line before many trailing sentences continue. "
          "The large reserved box must be fully visible only when its whole line participates in the visible layout. "
          "Otherwise the renderer must choose a text ellipsis boundary without flashing, cropping or retaining the large image. "
          "Repeated trailing words add stable overflow for wide and narrow resize verification.";
        CaseData data{"27. RenderScale 2x oversized END ellipsis", "While text is truncated, exactly one END ellipsis remains visible; the 210x120 image is wholly visible or wholly elided.",
                      text,
                      {ObjectAt(text, 0u, "flag_us.png", Vector2(210.0f, 120.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)},
                      true, true, false, Text::Alignment::END, Text::Alignment::START};
        data.renderScale = 2.0f;
        return data;
      }
      case 27u:
      {
        const std::string text =
          "line one before \uFFFC\n"
          "\uFFFC\n"
          "line three contains \uFFFC and more words that wrap\n"
          "line four is intentionally verbose and continues beyond the available height\n"
          "line five has \uFFFC and must be completely elided\n"
          "line six ends the explicit-newline scenario";
        return {"28. Multiline ellipsis with image-only lines", "Explicit newlines, an image-only line and later replacements remain atomic at the vertical cutoff.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(44.0f, 28.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(90.0f, 46.0f)),
                 ObjectAt(text, 2u, "flag_ae.png", Vector2(56.0f, 34.0f)),
                 ObjectAt(text, 3u, "flag_kr.png", Vector2(70.0f, 36.0f))},
                true, true, false, Text::Alignment::START, Text::Alignment::START};
      }
      case 28u:
      {
        const std::string text = "Remote HTTPS image \uFFFC downloaded between ordinary text";
        return {"29. Remote HTTPS image download",
                "Network required: both previews keep a stable 144x96 box while downloading, then display the W3C PNG.",
                text,
                {ObjectAt(text, 0u, REMOTE_IMAGE_URL, Vector2(144.0f, 96.0f),
                          Text::ImageAttributes::InlineAlignment::TEXT_CENTER)},
                true, false, false, Text::Alignment::CENTER, Text::Alignment::CENTER,
                Text::LineWrapMode::WORD, 28.0f};
      }
      case 29u:
      {
        const std::string text =
          "Portrait source in a square reservation \uFFFC followed by a landscape source in a tall reservation \uFFFC. "
          "Both images must keep their natural ratio while ordinary multiline text wraps around the fixed boxes.";
        return {"30. Opposite aspect ratios and boxes",
                "The portrait photo stays narrow inside 96x96; the landscape flag stays letterboxed inside 64x120 without a stretched frame.",
                text,
                {ObjectAt(text,
                          0u,
                          "../../image-view/res/sample.jpg",
                          Vector2(96.0f, 96.0f),
                          Text::ImageAttributes::InlineAlignment::TEXT_CENTER),
                 ObjectAt(text,
                          1u,
                          "flag_us.png",
                          Vector2(64.0f, 120.0f),
                          Text::ImageAttributes::InlineAlignment::TEXT_CENTER)},
                true, false, false, Text::Alignment::CENTER, Text::Alignment::CENTER};
      }
      case 30u:
      default:
      {
        const std::string text =
          "Cold source \uFFFC and same-source cache reuse \uFFFC remain in fixed boxes while the source switches repeatedly.";
        const char* source = mLifecycleAlternate ? "../../image-view/res/sample.jpg" : "flag_us.png";
        CaseData data{"31. Cold, warm-cache and source switching",
                      "Press L repeatedly: both Labels and both occurrences must reveal only aspect-fitted pixels, with no full-box flash or stale source.",
                      text,
                      {ObjectAt(text, 0u, source, Vector2(110.0f, 90.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER),
                       ObjectAt(text, 1u, source, Vector2(70.0f, 120.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)},
                      true, false, false, Text::Alignment::CENTER, Text::Alignment::CENTER};
        data.lifecycleCase = true;
        return data;
      }
    }
  }

  Text::StyledText BuildStyledText(const CaseData& data, bool replacements) const
  {
    Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(data.logicalText.c_str());
    if(replacements)
    {
      for(const ImageSpec& image : data.images)
      {
        const std::string source = ResolveImageSource(image.source);
        Text::ImageAttributes attributes(source.c_str(), image.size);
        attributes.SetAlignment(image.alignment);
        attributes.SetVerticalOffset(image.verticalOffset);
        if(image.alternativeText)
        {
          attributes.SetAlternativeText(image.alternativeText);
        }
        else
        {
          attributes.SetAlternativeText(""); // Explicit decorative image.
        }
        // A new handle per occurrence is intentional, including same-source cases.
        DALI_ASSERT_ALWAYS(builder.SetSpan(Text::ImageSpan::New(attributes), image.start, image.end) &&
                           "ImageSpan sample range must be valid");
      }
    }
    return builder.Build();
  }

  void AppendLifecycleImage(Text::StyledTextBuilder&                 builder,
                            const char*                              source,
                            Vector2                                  size,
                            Text::ImageAttributes::InlineAlignment alignment = Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM,
                            float                                    verticalOffset = 0.0f) const
  {
    const uint32_t begin = builder.GetUtf32Length();
    builder.AppendText("\uFFFC");

    const std::string sourceUrl = ResolveImageSource(source);
    Text::ImageAttributes attributes(sourceUrl.c_str(), size);
    attributes.SetAlignment(alignment);
    attributes.SetVerticalOffset(verticalOffset);
    attributes.SetAlternativeText("");
    DALI_ASSERT_ALWAYS(builder.SetSpan(Text::ImageSpan::New(attributes), begin, begin + 1u) &&
                       "Lifecycle ImageSpan range must be valid");
  }

  void AppendLifecycleRange(Text::StyledTextBuilder& builder, const char* source, Vector2 size) const
  {
    const uint32_t begin = builder.GetUtf32Length();
    builder.AppendText("[range]");

    const std::string sourceUrl = ResolveImageSource(source);
    Text::ImageAttributes attributes(sourceUrl.c_str(), size);
    attributes.SetAlignment(Text::ImageAttributes::InlineAlignment::TEXT_CENTER);
    attributes.SetAlternativeText("");
    DALI_ASSERT_ALWAYS(builder.SetSpan(Text::ImageSpan::New(attributes), begin, builder.GetUtf32Length()) &&
                       "Lifecycle multi-character ImageSpan range must be valid");
  }

  Text::StyledText BuildLifecycleStyledText(bool alternate) const
  {
    Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
    builder.AppendText(alternate ? "Updated local " : "Persistent local ");
    AppendLifecycleImage(builder,
                         alternate ? "flag_us.png" : "flag_kr.png",
                         alternate ? Vector2(54.0f, 30.0f) : Vector2(32.0f, 24.0f));
    builder.AppendText(" web ");
    AppendLifecycleImage(builder,
                         REMOTE_IMAGE_URL,
                         alternate ? Vector2(96.0f, 58.0f) : Vector2(72.0f, 48.0f),
                         Text::ImageAttributes::InlineAlignment::TEXT_CENTER);
    builder.AppendText(" adjacent ");
    AppendLifecycleImage(builder, "flag_ae.png", Vector2(18.0f, 18.0f));
    AppendLifecycleImage(builder, "flag_ae.png", Vector2(30.0f, 20.0f));
    builder.AppendText(" exact-range ");
    AppendLifecycleRange(builder, alternate ? "missing-image-lifecycle.png" : "flag_kr.png", Vector2(64.0f, 34.0f));
    builder.AppendText(" followed by enough ordinary words to exercise END ellipsis and multiline overflow while resources change.");
    return builder.Build();
  }

  Text::StyledText BuildLifecycleMarkupStyledText(bool alternate) const
  {
    Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup(
      alternate ? "<color value='#FCA5A5'>Updated markup</color> <u>local</u> "
                : "<color value='#93C5FD'>Markup source</color> <u>local</u> ");
    AppendLifecycleImage(builder,
                         alternate ? "flag_ae.png" : "flag_us.png",
                         alternate ? Vector2(48.0f, 34.0f) : Vector2(30.0f, 22.0f));
    builder.AppendText(" <s>web</s> ");
    AppendLifecycleImage(builder,
                         REMOTE_IMAGE_URL,
                         alternate ? Vector2(110.0f, 64.0f) : Vector2(76.0f, 48.0f),
                         Text::ImageAttributes::InlineAlignment::TEXT_CENTER);
    builder.AppendText(" with styled trailing words that remain long enough for ellipsis.");
    return builder.Build();
  }

  Text::StyledText BuildLifecycleMarkupWithoutImages() const
  {
    return Text::StyledText::FromMarkup(
      "<color value='#86EFAC'>Markup remains, ImageSpan attachments are cleared.</color> "
      "The next cycle recreates local and web resources.");
  }

  void ConfigureLifecycleLabel(Label label, bool multiline, bool async) const
  {
    label.SetAsyncRendering(async);
    label.SetMultiLine(multiline);
    label.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
    label.SetLineWrapMode(Text::LineWrapMode::WORD);
    label.SetFontSize(15.0f);
    label.SetRenderScale(1.0f);
    label.SetTextFit(Text::Fit::None());
    label.SetTextColor(UiColor(0x111827));
    label.SetBackgroundColor(UiColor(0xE2E8F0));
    label.SetPadding(Extents(4, 4, 2, 2));
    label.SetHorizontalTextAlignment(Text::Alignment::START);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
    label.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  }

  void ConfigureLifecycleCaseLabel(Label label, const CaseData& data, bool async) const
  {
    ConfigureLifecycleLabel(label, data.multiline, async);
    label.SetTextOverflowMode(data.ellipsis ? Text::OverflowMode::ELLIPSIS : Text::OverflowMode::CLIP);
    label.SetLineWrapMode(data.lineWrapMode);
    label.SetFontSize(data.fontSize);
    label.SetRenderScale(data.renderScale);
    if(data.textFit)
    {
      label.SetTextFit(Text::Fit::Range(14.0f, 28.0f, 2.0f));
    }
    else
    {
      label.SetTextFit(Text::Fit::None());
    }
    label.SetHorizontalTextAlignment(data.horizontalAlignment);
    label.SetVerticalTextAlignment(data.verticalAlignment);
    label.SetLayoutDirection(data.rtl ? LayoutDirection::RIGHT_TO_LEFT : LayoutDirection::LEFT_TO_RIGHT);
  }

  Label NewLifecycleLabel(bool multiline, bool async) const
  {
    Label label = Label::New();
    ConfigureLifecycleLabel(label, multiline, async);
    return label;
  }

  Text::Alignment ResolveAlignment(AlignmentOverride value, Text::Alignment caseDefault) const
  {
    switch(value)
    {
      case AlignmentOverride::START: return Text::Alignment::START;
      case AlignmentOverride::CENTER: return Text::Alignment::CENTER;
      case AlignmentOverride::END: return Text::Alignment::END;
      case AlignmentOverride::CASE_DEFAULT:
      default: return caseDefault;
    }
  }

  bool ResolveToggle(ToggleOverride value, bool caseDefault) const
  {
    switch(value)
    {
      case ToggleOverride::OFF: return false;
      case ToggleOverride::ON: return true;
      case ToggleOverride::CASE_DEFAULT:
      default: return caseDefault;
    }
  }

  void CycleHorizontalAlignment()
  {
    mHorizontalAlignmentOverride = NextAlignmentOverride(mHorizontalAlignmentOverride);
    ApplyCase();
  }

  void CycleVerticalAlignment()
  {
    mVerticalAlignmentOverride = NextAlignmentOverride(mVerticalAlignmentOverride);
    ApplyCase();
  }

  void CycleOverflowMode()
  {
    mOverflowOverride = NextToggleOverride(mOverflowOverride);
    ApplyCase();
  }

  void CycleMultiline()
  {
    mMultilineOverride = NextToggleOverride(mMultilineOverride);
    ApplyCase();
  }

  void ResetLayoutOverrides()
  {
    mHorizontalAlignmentOverride = AlignmentOverride::CASE_DEFAULT;
    mVerticalAlignmentOverride   = AlignmentOverride::CASE_DEFAULT;
    mOverflowOverride            = ToggleOverride::CASE_DEFAULT;
    mMultilineOverride           = ToggleOverride::CASE_DEFAULT;
    ApplyCase();
  }

  void UpdateLayoutControls(const CaseData& data)
  {
    const Text::Alignment horizontal = ResolveAlignment(mHorizontalAlignmentOverride, data.horizontalAlignment);
    const Text::Alignment vertical   = ResolveAlignment(mVerticalAlignmentOverride, data.verticalAlignment);
    const bool            ellipsis   = ResolveToggle(mOverflowOverride, data.ellipsis);
    const bool            multiline  = ResolveToggle(mMultilineOverride, data.multiline);

    mHorizontalAlignmentControl.SetText((std::string("H: ") + TextAlignmentName(horizontal) + " [" +
                                         OverrideName(mHorizontalAlignmentOverride == AlignmentOverride::CASE_DEFAULT) + "]").c_str());
    mVerticalAlignmentControl.SetText((std::string("V: ") + TextAlignmentName(vertical) + " [" +
                                       OverrideName(mVerticalAlignmentOverride == AlignmentOverride::CASE_DEFAULT) + "]").c_str());
    mOverflowControl.SetText((std::string("O: ") + (ellipsis ? "ELLIPSIS" : "CLIP") + " [" +
                              OverrideName(mOverflowOverride == ToggleOverride::CASE_DEFAULT) + "]").c_str());
    mMultilineControl.SetText((std::string("M: ") + (multiline ? "ON" : "OFF") + " [" +
                               OverrideName(mMultilineOverride == ToggleOverride::CASE_DEFAULT) + "]").c_str());
  }

  void ConfigurePreview(Label label, bool async) const
  {
    const CaseData data = GetCase(mCaseIndex);
    label.SetAsyncRendering(async);
    label.SetMultiLine(ResolveToggle(mMultilineOverride, data.multiline));
    label.SetTextOverflowMode(ResolveToggle(mOverflowOverride, data.ellipsis) ? Text::OverflowMode::ELLIPSIS : Text::OverflowMode::CLIP);
    label.SetLineWrapMode(data.lineWrapMode);
    label.SetFontSize(data.fontSize);
    label.SetRenderScale(data.renderScale);
    if(data.textFit)
    {
      label.SetTextFit(Text::Fit::Range(14.0f, 28.0f, 2.0f));
    }
    else
    {
      label.SetTextFit(Text::Fit::None());
    }
    label.SetHorizontalTextAlignment(ResolveAlignment(mHorizontalAlignmentOverride, data.horizontalAlignment));
    label.SetVerticalTextAlignment(ResolveAlignment(mVerticalAlignmentOverride, data.verticalAlignment));
    label.SetLayoutDirection(data.rtl ? LayoutDirection::RIGHT_TO_LEFT : LayoutDirection::LEFT_TO_RIGHT);
    const float previewWidth = WIDTH_SWEEP[mWidthIndex];
    label.SetRequestedWidth(previewWidth > 0.0f ? previewWidth : MATCH_PARENT);
    label.SetMaximumWidth(previewWidth > 0.0f ? previewWidth : 10000.0f);
  }

  std::string StatusText(const CaseData& data, const Text::StyledText& styledText) const
  {
    std::ostringstream status;
    bool advancedRange = false;
    for(const ImageSpec& image : data.images)
    {
      advancedRange |= image.end - image.start != 1u;
    }
    const float previewWidth = WIDTH_SWEEP[mWidthIndex];
    const Text::Alignment horizontal = ResolveAlignment(mHorizontalAlignmentOverride, data.horizontalAlignment);
    const Text::Alignment vertical   = ResolveAlignment(mVerticalAlignmentOverride, data.verticalAlignment);
    const bool            ellipsis   = ResolveToggle(mOverflowOverride, data.ellipsis);
    const bool            multiline  = ResolveToggle(mMultilineOverride, data.multiline);
    status << "logical: " << data.logicalText << "\nreplacement: " << (mReplacementEnabled ? "ON" : "OFF")
           << " | primary: " << (mUseAsync ? "async" : "sync")
           << " | width: " << (previewWidth > 0.0f ? std::to_string(static_cast<int>(previewWidth)) : "wide")
           << " | authoring: " << (advancedRange ? "multi-character compact" : "U+FFFC identity")
           << " | renderScale: " << data.renderScale << " | textFit: " << (data.textFit ? "ON" : "OFF")
           << " | span count: " << styledText.GetSpanCount() << "\n";
    status << "layout: H=" << TextAlignmentName(horizontal) << " ("
           << OverrideName(mHorizontalAlignmentOverride == AlignmentOverride::CASE_DEFAULT) << ") | V="
           << TextAlignmentName(vertical) << " ("
           << OverrideName(mVerticalAlignmentOverride == AlignmentOverride::CASE_DEFAULT) << ") | overflow="
           << (ellipsis ? "ELLIPSIS" : "CLIP") << " ("
           << OverrideName(mOverflowOverride == ToggleOverride::CASE_DEFAULT) << ") | multiline="
           << (multiline ? "ON" : "OFF") << " ("
           << OverrideName(mMultilineOverride == ToggleOverride::CASE_DEFAULT) << ")\n";
    for(std::size_t i = 0u; i < data.images.size(); ++i)
    {
      const ImageSpec& image = data.images[i];
      status << "#" << i + 1u << " [" << image.start << "," << image.end << ") " << image.source
             << " " << image.size.width << "x" << image.size.height << " " << AlignmentName(image.alignment);
      if(image.verticalOffset != 0.0f)
      {
        status << " offset=" << image.verticalOffset;
      }
      status << "\n";
    }
    const Vector3 natural = mPrimary.GetNaturalSize();
    status << "primary natural size: " << natural.width << " x " << natural.height;
    if(data.marqueeCase)
    {
      const bool marqueeRunning = mPrimary.IsMarqueeRunning();
      status << " | marquee running: " << (marqueeRunning ? "YES" : "NO");
      if(mReplacementEnabled && marqueeRunning)
      {
        status << " (BUG)";
      }
    }
    if(data.lifecycleCase)
    {
      status << " | lifecycle variant: " << (mLifecycleAlternate ? "B" : "A") << " (press L)";
    }
    status << "\nellipsis policy: END supported | START/MIDDLE unsupported (replacement-preserving CLIP fallback)";
    if(!mStressStatus.empty())
    {
      status << "\n" << mStressStatus;
    }
    return status.str();
  }

  void ApplyCase()
  {
    const CaseData        data       = GetCase(mCaseIndex);
    const Text::StyledText styledText = BuildStyledText(data, mReplacementEnabled);
    ConfigurePreview(mPrimary, mUseAsync);
    ConfigurePreview(mParity, true);
    mPrimary.SetStyledText(styledText);
    mParity.SetStyledText(styledText); // Same immutable source; runtime remains control-owned.
    mTitle.SetText((std::to_string(mCaseIndex + 1u) + "/" + std::to_string(CASE_COUNT) + "  " + data.title).c_str());
    mExpected.SetText(data.expected);
    mStatus.SetText(StatusText(data, styledText).c_str());
    UpdateLayoutControls(data);
  }

  void RefreshStatus()
  {
    const CaseData data = GetCase(mCaseIndex);
    mStatus.SetText(StatusText(data, mPrimary.GetStyledText()).c_str());
  }

  void RemoveLifecycleLabel(Label& label)
  {
    if(label && label.GetParent())
    {
      mStressRoot.Remove(label, RemovePolicy::IMMEDIATE);
    }
  }

  void EnsureLifecycleSetClearLabel()
  {
    if(!mLifecycleSetClearLabel)
    {
      mLifecycleSetClearLabel = NewLifecycleLabel(false, false);
      mStressRoot.Add(mLifecycleSetClearLabel);
    }
  }

  void EnsureLifecycleMarkupLabel()
  {
    if(!mLifecycleMarkupLabel)
    {
      mLifecycleMarkupLabel = NewLifecycleLabel(false, true);
      mStressRoot.Add(mLifecycleMarkupLabel);
    }
  }

  void UpdateLifecycleStatus(uint32_t phase)
  {
    const MemorySnapshot memory = ReadMemory();
    if(memory.rssKb > mLifecyclePeakRssKb)
    {
      mLifecyclePeakRssKb = memory.rssKb;
    }

    std::ostringstream status;
    status << "lifecycle loop: " << (mLifecycleRunning ? "RUNNING" : "STOPPED")
           << " (F/button) | interval: " << LIFECYCLE_INTERVAL_MS << " ms"
           << " | step: " << mLifecycleStep
           << " | cycles: " << (mLifecycleStep / LIFECYCLE_PHASE_COUNT)
           << " | phase: " << LifecyclePhaseName(phase)
           << " | sample case: " << (mLifecycleCaseIndex + 1u) << "/" << CASE_COUNT;
    if(memory.rssKb > 0 && mLifecycleBaselineMemory.rssKb > 0)
    {
      status << " | RSS: " << memory.rssKb << " KiB (delta "
             << memory.rssKb - mLifecycleBaselineMemory.rssKb << ", peak "
             << mLifecyclePeakRssKb - mLifecycleBaselineMemory.rssKb << ")"
             << " | HWM: " << memory.hwmKb << " KiB";
    }
    mStressStatus = status.str();
    RefreshStatus();
  }

  bool OnLifecycleTimerTick()
  {
    if(!mLifecycleRunning)
    {
      return false;
    }

    const uint32_t phase = static_cast<uint32_t>(mLifecycleStep % LIFECYCLE_PHASE_COUNT);
    const uint64_t cycle = mLifecycleStep / LIFECYCLE_PHASE_COUNT;
    switch(phase)
    {
      case 0u:
      {
        EnsureLifecycleSetClearLabel();
        ConfigureLifecycleLabel(mLifecycleSetClearLabel, false, (cycle & 1u) != 0u);
        mLifecycleSetClearLabel.SetStyledText(BuildLifecycleStyledText(false));
        break;
      }
      case 1u:
      {
        EnsureLifecycleSetClearLabel();
        mLifecycleCaseIndex      = static_cast<std::size_t>(cycle % CASE_COUNT);
        const CaseData caseData = GetCase(mLifecycleCaseIndex);
        ConfigureLifecycleCaseLabel(mLifecycleSetClearLabel, caseData, (cycle & 1u) == 0u);
        mLifecycleSetClearLabel.SetStyledText(BuildStyledText(caseData, true));
        break;
      }
      case 2u:
      {
        if(mLifecycleSetClearLabel)
        {
          Text::StyledText current = mLifecycleSetClearLabel.GetStyledText();
          if(current)
          {
            Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromStyledText(current);
            builder.ClearSpans();
            mLifecycleSetClearLabel.SetStyledText(builder.Build());
          }
        }
        break;
      }
      case 3u:
      {
        if(mLifecycleSetClearLabel)
        {
          if((cycle & 1u) == 0u)
          {
            mLifecycleSetClearLabel.SetText("Plain text clears all replacement runtime and pending image loads.");
          }
          else
          {
            mLifecycleSetClearLabel.SetStyledText(Text::StyledText());
          }
        }
        break;
      }
      case 4u:
      {
        RemoveLifecycleLabel(mLifecycleTransientLabel);
        mLifecycleTransientLabel.Reset();
        mLifecycleTransientLabel = NewLifecycleLabel(true, (cycle & 1u) == 0u);
        mStressRoot.Add(mLifecycleTransientLabel);
        mLifecycleTransientLabel.SetStyledText(BuildLifecycleStyledText((cycle & 1u) != 0u));
        break;
      }
      case 5u:
      {
        const Text::StyledText shared = BuildLifecycleStyledText((cycle & 1u) == 0u);
        if(mLifecycleTransientLabel)
        {
          mLifecycleTransientLabel.SetStyledText(shared);
        }
        EnsureLifecycleSetClearLabel();
        mLifecycleSetClearLabel.SetStyledText(shared);
        break;
      }
      case 6u:
      {
        RemoveLifecycleLabel(mLifecycleTransientLabel);
        if(mLifecycleSetClearLabel)
        {
          mLifecycleSetClearLabel.SetText("Shared StyledText removed from the surviving Label.");
        }
        break;
      }
      case 7u:
      {
        mLifecycleTransientLabel.Reset();
        break;
      }
      case 8u:
      {
        EnsureLifecycleMarkupLabel();
        ConfigureLifecycleLabel(mLifecycleMarkupLabel, false, (cycle & 1u) != 0u);
        mLifecycleMarkupLabel.SetStyledText(BuildLifecycleMarkupStyledText(false));
        break;
      }
      case 9u:
      {
        EnsureLifecycleMarkupLabel();
        ConfigureLifecycleLabel(mLifecycleMarkupLabel, (cycle & 1u) != 0u, (cycle & 1u) == 0u);
        mLifecycleMarkupLabel.SetStyledText(BuildLifecycleMarkupStyledText(true));
        break;
      }
      case 10u:
      {
        if(mLifecycleMarkupLabel)
        {
          if((cycle & 1u) == 0u)
          {
            mLifecycleMarkupLabel.SetStyledText(BuildLifecycleMarkupWithoutImages());
          }
          else
          {
            // Keep active remote ImageVisuals on odd cycles so the next phase destroys the owner mid-load.
            mLifecycleMarkupLabel.SetStyledText(BuildLifecycleMarkupStyledText(false));
          }
        }
        break;
      }
      case 11u:
      default:
      {
        RemoveLifecycleLabel(mLifecycleMarkupLabel);
        mLifecycleMarkupLabel.Reset();
        break;
      }
    }

    ++mLifecycleStep;
    UpdateLifecycleStatus(phase);
    return true;
  }

  void StopLifecycleLoop()
  {
    if(mLifecycleTimer)
    {
      mLifecycleTimer.Stop();
      mLifecycleTimer.Reset();
    }
    mLifecycleRunning = false;

    RemoveLifecycleLabel(mLifecycleTransientLabel);
    RemoveLifecycleLabel(mLifecycleMarkupLabel);
    RemoveLifecycleLabel(mLifecycleSetClearLabel);
    mLifecycleTransientLabel.Reset();
    mLifecycleMarkupLabel.Reset();
    mLifecycleSetClearLabel.Reset();
    mStressRoot.RemoveAllChildren();
    mStressRoot.SetRequestedHeight(1.0f);
    UpdateLifecycleStatus(static_cast<uint32_t>(mLifecycleStep % LIFECYCLE_PHASE_COUNT));
  }

  void ToggleLifecycleLoop()
  {
    if(mLifecycleRunning)
    {
      StopLifecycleLoop();
      return;
    }

    mStressRoot.RemoveAllChildren();
    mStressLabels.clear();
    mStressRoot.SetRequestedHeight(96.0f);
    mLifecycleStep           = 0u;
    mLifecycleCaseIndex      = 0u;
    mLifecycleBaselineMemory = ReadMemory();
    mLifecyclePeakRssKb      = mLifecycleBaselineMemory.rssKb;
    mLifecycleRunning        = true;
    mLifecycleTimer          = Timer::New(LIFECYCLE_INTERVAL_MS);
    mLifecycleTimer.TickSignal().Connect(this, &TextImageSpanController::OnLifecycleTimerTick);
    OnLifecycleTimerTick();
    mLifecycleTimer.Start();
  }

  void RunStress()
  {
    if(mLifecycleRunning)
    {
      StopLifecycleLoop();
    }
    mStressRoot.RemoveAllChildren();
    mStressLabels.clear();
    const uint32_t imagesPerLabel = mStressTenImages ? 10u : 1u;
    const auto start = std::chrono::steady_clock::now();
    for(uint32_t labelIndex = 0u; labelIndex < 100u; ++labelIndex)
    {
      Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
      for(uint32_t imageIndex = 0u; imageIndex < imagesPerLabel; ++imageIndex)
      {
        const uint32_t begin = builder.GetUtf32Length();
        builder.AppendText("\uFFFC");
        const char* file = (imageIndex % 2u == 0u) ? "flag_kr.png" : "flag_us.png";
        Text::ImageAttributes attributes(Resource(file).c_str(), Vector2(10.0f, 10.0f));
        DALI_ASSERT_ALWAYS(builder.SetSpan(Text::ImageSpan::New(attributes), begin, begin + 1u) &&
                           "Stress ImageSpan range must be valid");
      }
      Label label = Label::New();
      label.SetFontSize(8.0f);
      label.SetStyledText(builder.Build());
      label.SetRequestedWidth(120.0f);
      label.SetRequestedHeight(12.0f);
      mStressRoot.Add(label);
      mStressLabels.push_back(label);
    }
    const double milliseconds = std::chrono::duration<double, std::milli>(
      std::chrono::steady_clock::now() - start).count();
    std::ostringstream status;
    status << "stress: 100 Labels x " << imagesPerLabel << " image(s), build/add " << milliseconds << " ms";
    mStressStatus = status.str();
    mStressTenImages = !mStressTenImages;
    ApplyCase();
  }

  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, 1080, 820));
    window.SetBackgroundColor(UiColor(0x0F172A));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(8.0f);
    root.SetPadding(Extents(16, 16, 16, 16));

    mTitle    = NewHudLabel("", 48.0f, 0x1D4ED8);
    mExpected = NewHudLabel("", 58.0f, 0x334155);
    mStatus   = NewHudLabel("", 150.0f, 0x1E293B);
    mStatus.SetHorizontalTextAlignment(Text::Alignment::START);

    StackLayout previews = StackLayout::New(StackOrientation::HORIZONTAL);
    previews.SetSpacing(8.0f);
    previews.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mPrimary = Label::New();
    mParity  = Label::New();
    for(Label label : {mPrimary, mParity})
    {
      label.SetFontSize(28.0f);
      label.SetTextColor(UiColor(0x111827));
      label.SetBackgroundColor(UiColor(0xF8FAFC));
      label.SetPadding(Extents(14, 14, 14, 14));
      label.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
      previews.Add(label);
    }

    StackLayout actions = StackLayout::New(StackOrientation::HORIZONTAL);
    actions.SetSpacing(6.0f);
    actions.SetRequestedWidth(MATCH_PARENT);
    actions.SetRequestedHeight(44.0f);
    const std::array<const char*, 8u> names = {"Previous", "Next", "Sync/Async", "Width", "Replacement", "Marquee", "Stress", "Loop (F)"};
    std::array<Label, 8u> buttons;
    for(std::size_t i = 0u; i < names.size(); ++i)
    {
      buttons[i] = NewHudLabel(names[i], 44.0f, i < 2u ? 0x1D4ED8 : 0x475569, true);
      actions.Add(buttons[i]);
    }
    buttons[0].AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { mCaseIndex = (mCaseIndex + CASE_COUNT - 1u) % CASE_COUNT; ApplyCase(); });
    buttons[1].AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { mCaseIndex = (mCaseIndex + 1u) % CASE_COUNT; ApplyCase(); });
    buttons[2].AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { mUseAsync = !mUseAsync; ApplyCase(); });
    buttons[3].AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { mWidthIndex = (mWidthIndex + 1u) % WIDTH_SWEEP.size(); ApplyCase(); });
    buttons[4].AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { mReplacementEnabled = !mReplacementEnabled; ApplyCase(); });
    buttons[5].AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) {
      mPrimary.StartMarquee();
      mParity.StartMarquee();
      mStatus.SetText(StatusText(GetCase(mCaseIndex), mPrimary.GetStyledText()).c_str());
    });
    buttons[6].AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { RunStress(); });
    buttons[7].AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { ToggleLifecycleLoop(); });

    StackLayout layoutActions = StackLayout::New(StackOrientation::HORIZONTAL);
    layoutActions.SetSpacing(6.0f);
    layoutActions.SetRequestedWidth(MATCH_PARENT);
    layoutActions.SetRequestedHeight(44.0f);
    mHorizontalAlignmentControl = NewHudLabel("", 44.0f, 0x475569, true);
    mVerticalAlignmentControl   = NewHudLabel("", 44.0f, 0x475569, true);
    mOverflowControl            = NewHudLabel("", 44.0f, 0x475569, true);
    mMultilineControl           = NewHudLabel("", 44.0f, 0x475569, true);
    Label resetLayout           = NewHudLabel("Defaults (D)", 44.0f, 0x334155, true);
    layoutActions.Add(mHorizontalAlignmentControl);
    layoutActions.Add(mVerticalAlignmentControl);
    layoutActions.Add(mOverflowControl);
    layoutActions.Add(mMultilineControl);
    layoutActions.Add(resetLayout);
    mHorizontalAlignmentControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { CycleHorizontalAlignment(); });
    mVerticalAlignmentControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { CycleVerticalAlignment(); });
    mOverflowControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { CycleOverflowMode(); });
    mMultilineControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { CycleMultiline(); });
    resetLayout.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { ResetLayoutOverrides(); });

    mStressRoot = StackLayout::New(StackOrientation::VERTICAL);
    mStressRoot.SetRequestedWidth(1.0f);
    mStressRoot.SetRequestedHeight(1.0f);

    root.Add(mTitle);
    root.Add(mExpected);
    root.Add(previews);
    root.Add(mStatus);
    root.Add(actions);
    root.Add(layoutActions);
    root.Add(mStressRoot);
    window.Add(root);
    window.KeyEventSignal().Connect(this, &TextImageSpanController::OnKey);
    ApplyCase();
  }

  void OnKey(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyName() == "Left")
    {
      mCaseIndex = (mCaseIndex + CASE_COUNT - 1u) % CASE_COUNT;
      ApplyCase();
    }
    else if(event.GetKeyName() == "Right")
    {
      mCaseIndex = (mCaseIndex + 1u) % CASE_COUNT;
      ApplyCase();
    }
    else if(event.GetKeyName() == "h" || event.GetKeyName() == "H")
    {
      CycleHorizontalAlignment();
    }
    else if(event.GetKeyName() == "v" || event.GetKeyName() == "V")
    {
      CycleVerticalAlignment();
    }
    else if(event.GetKeyName() == "o" || event.GetKeyName() == "O")
    {
      CycleOverflowMode();
    }
    else if(event.GetKeyName() == "m" || event.GetKeyName() == "M")
    {
      CycleMultiline();
    }
    else if(event.GetKeyName() == "d" || event.GetKeyName() == "D")
    {
      ResetLayoutOverrides();
    }
    else if(event.GetKeyName() == "l" || event.GetKeyName() == "L")
    {
      mLifecycleAlternate = !mLifecycleAlternate;
      ApplyCase();
    }
    else if(event.GetKeyName() == "f" || event.GetKeyName() == "F")
    {
      ToggleLifecycleLoop();
    }
    else if(event.GetKeyName() == "q")
    {
      UiScaleManager::Get().SetScale(0.8f);
    }
    else if(event.GetKeyName() == "w")
    {
      UiScaleManager::Get().SetScale(1.0f);
    }
    else if(event.GetKeyName() == "e")
    {
      UiScaleManager::Get().SetScale(1.2f);
    }
    else if(event.GetKeyName() == "r")
    {
      UiScaleManager::Get().SetScale(1.5f);
    }
    else if(event.GetKeyName() == "t")
    {
      UiScaleManager::Get().SetScale(2.0f);
    }
  }

private:
  Application&       mApplication;
  Label              mTitle;
  Label              mExpected;
  Label              mPrimary;
  Label              mParity;
  Label              mStatus;
  Label              mHorizontalAlignmentControl;
  Label              mVerticalAlignmentControl;
  Label              mOverflowControl;
  Label              mMultilineControl;
  Label              mLifecycleSetClearLabel;
  Label              mLifecycleTransientLabel;
  Label              mLifecycleMarkupLabel;
  StackLayout        mStressRoot;
  std::vector<Label> mStressLabels;
  Timer              mLifecycleTimer;
  MemorySnapshot     mLifecycleBaselineMemory;
  std::size_t        mCaseIndex{0u};
  std::size_t        mLifecycleCaseIndex{0u};
  uint64_t           mLifecycleStep{0u};
  long               mLifecyclePeakRssKb{0};
  bool               mUseAsync{false};
  std::size_t        mWidthIndex{2u};
  bool               mReplacementEnabled{true};
  bool               mLifecycleAlternate{false};
  bool               mStressTenImages{false};
  bool               mLifecycleRunning{false};
  AlignmentOverride  mHorizontalAlignmentOverride{AlignmentOverride::CASE_DEFAULT};
  AlignmentOverride  mVerticalAlignmentOverride{AlignmentOverride::CASE_DEFAULT};
  ToggleOverride     mOverflowOverride{ToggleOverride::CASE_DEFAULT};
  ToggleOverride     mMultilineOverride{ToggleOverride::CASE_DEFAULT};
  std::string        mStressStatus;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TextImageSpanController controller(application);
  application.MainLoop();
  return 0;
}
