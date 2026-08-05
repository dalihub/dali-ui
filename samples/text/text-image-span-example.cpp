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

// EXTERNAL INCLUDES
#include <algorithm>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr std::size_t CASE_COUNT           = 32u;
constexpr float       RELATIVE_LINE_HEIGHT = 1.6f;
constexpr const char* REMOTE_IMAGE_URL =
  "https://www.w3.org/assets/logos/w3c-2025-transitional/w3c-72x48.png";

struct ImageSpec
{
  uint32_t                              start{0u};
  uint32_t                              end{0u};
  const char*                           source{nullptr};
  Vector2                               size{24.0f, 24.0f};
  Text::ImageAttributes::InlineAlignment alignment{Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM};
  float                                 verticalOffset{0.0f};
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
  bool                            scrollCase{false};
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

std::string ReplacementText(std::string text)
{
  constexpr char MARKER[] = "[image]";
  std::size_t    position = 0u;
  while((position = text.find(MARKER, position)) != std::string::npos)
  {
    text.replace(position,
                 sizeof(MARKER) - 1u,
                 Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
    position += sizeof(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER) - 1u;
  }
  return text;
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
  return ImageAt(text,
                 Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER,
                 source,
                 size,
                 alignment,
                 verticalOffset,
                 occurrence);
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
        const std::string text = ReplacementText("A[image]B");
        return {"1. One U+FFFC", "The object replacement character is replaced by one TEXT_BOTTOM image.",
                text, {ObjectAt(text, 0u, "flag_kr.png", Vector2(24.0f, 24.0f))}, false};
      }
      case 1u:
        return {"2. Multi-character exact range", "[icon] disappears as one atomic unit; toggling replacement restores it.",
                "Press [icon] to continue", {{6u, 12u, "flag_kr.png", Vector2(32.0f, 24.0f)}}};
      case 2u:
      {
        const std::string text = ReplacementText("X[image][image][image]Y");
        return {"3. Multiple and adjacent", "Three U+FFFC occurrences use three ImageSpan handles; adjacent boxes remain distinct.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(24.0f, 24.0f)),
                 ObjectAt(text, 1u, "flag_kr.png", Vector2(24.0f, 24.0f)),
                 ObjectAt(text, 2u, "flag_us.png", Vector2(24.0f, 24.0f))}};
      }
      case 3u:
      {
        const std::string text = ReplacementText("baseline [image] bottom [image] center [image] up [image] down [image]");
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
        const std::string text = ReplacementText("before [image] after\n[image]\nend");
        return {"5. Multiline and replacement-only line", "Window resizing wraps around a wide box and retains the image-only line.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(120.0f, 60.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(40.0f, 40.0f))}};
      }
      case 5u:
      {
        const std::string text = ReplacementText(
          "This is a long paragraph with an inline [image] image placed between ordinary words. "
          "Resize the preview to observe how the sentence wraps before and after the image. "
          "A second [image] marker appears later while several more words continue across multiple lines.");
        return {"6. Long multiline prose", "Two U+FFFC images remain in flow while the paragraph wraps across many lines.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(36.0f, 26.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(36.0f, 26.0f))}};
      }
      case 6u:
      {
        const std::string text = ReplacementText(
          "Text before the oversized inline image explains the first line. "
          "The next phrase contains [image] in continuous prose, followed by enough text to wrap onto several more lines "
          "and show the expanded line box clearly.");
        return {"7. Large image inside multiline text", "The 180x130 box expands its line while surrounding prose continues above and below it.",
                text,
                {ObjectAt(text, 0u, "flag_us.png", Vector2(180.0f, 130.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)}};
      }
      case 7u:
      {
        const std::string text = ReplacementText(
          "Sizes [image] 8x8, [image] 12x12, [image] 16x16, [image] 24x24, [image] 32x20, "
          "[image] 40x40, [image] 64x32, [image] 80x48, and [image] 120x60 are distributed through multiline prose "
          "so every box participates in wrapping.");
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
        const std::string text = ReplacementText("LTR אבג [image] العربية end");
        return {"9. RTL and mixed bidi", "The U+FFFC image remains one visual unit between Hebrew and Arabic text.",
                text, {ObjectAt(text, 0u, "flag_ae.png", Vector2(34.0f, 24.0f))}, true, false, true};
      }
      case 9u:
      {
        const std::string text = ReplacementText("Long prefix words [image] trailing text");
        return {"10. END ellipsis atomicity", "Window resizing places END ellipsis before or after the whole image, never through it.",
                text, {ObjectAt(text, 0u, "flag_kr.png", Vector2(70.0f, 28.0f))}, false, true};
      }
      case 10u:
      {
        const std::string text = ReplacementText("Atomic[image]suffix without spaces still has a stable END ellipsis boundary");
        return {"11. END ellipsis without spaces", "An adjacent image remains atomic without whitespace on either side.",
                text, {ObjectAt(text, 0u, "flag_us.png", Vector2(64.0f, 32.0f))}, false, true, false,
                Text::Alignment::START, Text::Alignment::CENTER, Text::LineWrapMode::CHARACTER};
      }
      case 11u:
      {
        const std::string text = ReplacementText("[image] image starts a long single line whose remaining words overflow the preview");
        return {"12. END ellipsis with leading image", "The leading image is retained or removed as one complete unit.",
                text, {ObjectAt(text, 0u, "flag_ae.png", Vector2(92.0f, 38.0f))}, false, true, false,
                Text::Alignment::START};
      }
      case 12u:
      {
        const std::string text = ReplacementText("A long single line ends with one final replacement [image]");
        return {"13. END ellipsis with trailing image", "The final image cannot remain after ellipsis or become partially clipped.",
                text, {ObjectAt(text, 0u, "flag_kr.png", Vector2(72.0f, 30.0f))}, false, true, false,
                Text::Alignment::END};
      }
      case 13u:
      {
        const std::string text = ReplacementText("Marquee [image] must remain static even when the surrounding line is deliberately long");
        CaseData data{"14. Marquee blocked", "Starting marquee must leave replacement content in its static layout.",
                      text, {ObjectAt(text, 0u, "flag_us.png", Vector2(58.0f, 28.0f))}, false};
        data.marqueeCase = true;
        return data;
      }
      case 14u:
      {
        const std::string text = ReplacementText("Sizes [image] then [image] then [image] and ordinary trailing words");
        return {"15. END ellipsis with mixed sizes", "Window widths exercise ellipsis before, on and after differently sized boxes.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(8.0f, 8.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(120.0f, 60.0f)),
                 ObjectAt(text, 2u, "flag_ae.png", Vector2(40.0f, 40.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)},
                false, true, false, Text::Alignment::START};
      }
      case 15u:
      {
        const std::string text = ReplacementText("One [image] two [image] three [image] followed by ordinary text before the end");
        return {"16. END ellipsis between images", "Every threshold exposes a monotonic prefix with no stale visual after ellipsis.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(48.0f, 28.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(68.0f, 32.0f)),
                 ObjectAt(text, 2u, "flag_ae.png", Vector2(52.0f, 36.0f))},
                false, true, false, Text::Alignment::END};
      }
      case 16u:
      {
        const std::string text = ReplacementText("Readable prefix [image] followed by a sentence that will be elided cleanly");
        return {"17. END ellipsis on image boundary", "At the threshold, U+FFFC changes directly into ellipsis without an image-sized blank gap.",
                text, {ObjectAt(text, 0u, "flag_kr.png", Vector2(82.0f, 30.0f))}, false, true};
      }
      case 17u:
      {
        const std::string text = ReplacementText("[image]");
        return {"18. Image-only END ellipsis", "The only image is either fully visible or replaced by ellipsis without a residual box.",
                text, {ObjectAt(text, 0u, "flag_us.png", Vector2(120.0f, 60.0f))}, false, true};
      }
      case 18u:
      {
        const std::string text = ReplacementText("ordinary text before the final [image]");
        return {"19. Text plus image END ellipsis", "The trailing image is never retained to the right of the ellipsis.",
                text, {ObjectAt(text, 0u, "flag_ae.png", Vector2(80.0f, 48.0f))}, false, true, false,
                Text::Alignment::START};
      }
      case 19u:
      {
        const std::string text = ReplacementText("[image] ordinary text after the leading replacement keeps extending");
        return {"20. Image plus text END ellipsis", "A visible leading image remains before ellipsis while only its following text is removed.",
                text, {ObjectAt(text, 0u, "flag_kr.png", Vector2(64.0f, 32.0f))}, false, true, false,
                Text::Alignment::START};
      }
      case 20u:
      {
        const std::string text = ReplacementText("sizes [image] text [image][image] text [image] end");
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
        const std::string text = ReplacementText("אבג LTR words [image] continue inside an RTL paragraph until the END boundary");
        return {"22. RTL paragraph plus LTR END ellipsis", "Paragraph direction changes the physical END edge without changing replacement visibility.",
                text, {ObjectAt(text, 0u, "flag_ae.png", Vector2(56.0f, 30.0f))}, false, true, true,
                Text::Alignment::START};
      }
      case 22u:
      {
        const std::string text = ReplacementText("resource lifecycle [image] keeps the same 64x32 reservation after success or failure");
        CaseData data{"23. Load failure and lifecycle", "Switch Source to alternate a valid and missing source; layout must not move.",
                      text,
                      {ObjectAt(text, 0u, mLifecycleAlternate ? "flag_kr.png" : "missing-image.png", Vector2(64.0f, 32.0f))},
                      false};
        data.lifecycleCase = true;
        return data;
      }
      case 23u:
      {
        const std::string text = ReplacementText("LTR prefix [image] אבג العربية mixed direction trailing words for ellipsis");
        return {"24. END ellipsis in mixed bidi text", "RTL layout and mixed-direction text keep the replacement on the correct side of END ellipsis.",
                text, {ObjectAt(text, 0u, "flag_ae.png", Vector2(56.0f, 30.0f))}, false, true, true,
                Text::Alignment::START};
      }
      case 24u:
      {
        const std::string text = ReplacementText("A multiline paragraph begins with enough ordinary prose to create several wrapped lines. "
          "The first inline [image] image should remain visible while more sentences continue through the preview. "
          "Additional words deliberately repeat the wrapping pressure so that vertical overflow selects a final visible line. "
          "Near that boundary a second [image] image may either fit completely before the ellipsis or disappear completely. "
          "Everything after the boundary, including this third [image] image and the remaining paragraph, must stay hidden. "
          "The final sentences provide enough length for both wide and narrow window sizes to exercise multiline END ellipsis.");
        return {"25. Multiline END ellipsis line boundary", "Only replacements before the final visible ellipsis boundary remain; later-line images are hidden.",
                text,
                {ObjectAt(text, 0u, "flag_kr.png", Vector2(42.0f, 28.0f)),
                 ObjectAt(text, 1u, "flag_us.png", Vector2(76.0f, 32.0f)),
                 ObjectAt(text, 2u, "flag_ae.png", Vector2(48.0f, 30.0f))},
                true, true, false, Text::Alignment::START, Text::Alignment::START};
      }
      case 25u:
      {
        const std::string text = ReplacementText("Centered multiline ellipsis begins with ordinary words and a small [image] image. "
          "Several phrases follow to build multiple lines before a very wide [image] replacement approaches the last visible line. "
          "More text and a tall [image] replacement continue beyond it so resize can move the final boundary across different box sizes. "
          "This deliberately long tail keeps wrapping through additional sentences and verifies that no image can appear below or after ellipsis. "
          "One more sequence of ordinary words makes the vertical overflow deterministic at the wider preview size as well.");
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
        const std::string text = ReplacementText("An oversized replacement follows wrapped introductory prose and tests vertical END ellipsis. "
          "More words place [image] near a constrained line before many trailing sentences continue. "
          "The large reserved box must be fully visible only when its whole line participates in the visible layout. "
          "Otherwise the renderer must choose a text ellipsis boundary without flashing, cropping or retaining the large image. "
          "Repeated trailing words add stable overflow for wide and narrow resize verification.");
        CaseData data{"27. RenderScale 2x oversized END ellipsis",
                      "While text is truncated, exactly one END ellipsis remains visible; the 210x120 image is wholly visible or wholly elided.",
                      text,
                      {ObjectAt(text, 0u, "flag_us.png", Vector2(210.0f, 120.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)},
                      true, true, false, Text::Alignment::END, Text::Alignment::START};
        data.renderScale = 2.0f;
        return data;
      }
      case 27u:
      {
        const std::string text = ReplacementText("line one before [image]\n"
          "[image]\n"
          "line three contains [image] and more words that wrap\n"
          "line four is intentionally verbose and continues beyond the available height\n"
          "line five has [image] and must be completely elided\n"
          "line six ends the explicit-newline scenario");
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
        const std::string text = ReplacementText("Remote HTTPS image [image] downloaded between ordinary text");
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
        const std::string text = ReplacementText("Portrait source in a square reservation [image] followed by a landscape source in a tall reservation [image]. "
          "Both images must keep their natural ratio while ordinary multiline text wraps around the fixed boxes.");
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
      {
        const std::string text = ReplacementText("Cold source [image] and same-source cache reuse [image] remain in fixed boxes while the source switches repeatedly.");
        const char* source = mLifecycleAlternate ? "../../image-view/res/sample.jpg" : "flag_us.png";
        CaseData data{"31. Cold, warm-cache and source switching",
                      "Switch Source repeatedly: both Labels and both occurrences must reveal only aspect-fitted pixels, with no full-box flash or stale source.",
                      text,
                      {ObjectAt(text, 0u, source, Vector2(110.0f, 90.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER),
                       ObjectAt(text, 1u, source, Vector2(70.0f, 120.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)},
                      true, false, false, Text::Alignment::CENTER, Text::Alignment::CENTER};
        data.lifecycleCase = true;
        return data;
      }
      case 31u:
      default:
      {
        const std::string text = ReplacementText(
          "TOP CHECKPOINT [image] The first image begins a deliberately tall multiline Label. "
          "Drag vertically inside either preview and verify that the image moves with this paragraph and is clipped only by the ScrollView edge.\n\n"
          "Section 1. Ordinary prose creates several wrapped lines before the next checkpoint. "
          "Resize the window while this section is visible; wrapping may change, but no image may detach from its reserved box or remain fixed over the viewport. "
          "A small baseline image [image] finishes the first section.\n\n"
          "Section 2. Continue downward through enough text to move the top images completely outside the clipped region. "
          "The ScrollView must not leave stale pixels behind after a fast drag or fling. Reversing direction should reveal the same images again without a flash.\n\n"
          "QUARTER CHECKPOINT [image] This wide image exercises partial clipping at both the top and bottom viewport edges. "
          "Stop with half of the reserved box outside the viewport, then continue scrolling until it is fully hidden.\n\n"
          "Section 3. Mixed sizes are distributed throughout the content rather than clustered in one line. "
          "Text before and after every object should keep its wrapping position while the containing Label translates. "
          "A compact centered image [image] marks the approach to the middle.\n\n"
          "MIDDLE CHECKPOINT [image] This taller replacement expands its own line. "
          "Scroll slowly across this point and compare the synchronous Label on the left with the asynchronous Label on the right. "
          "Their image geometry and clipping should remain identical.\n\n"
          "Section 4. More wrapped prose supplies distance below the middle. "
          "Repeated scrolling should reuse the loaded resources without changing aspect ratio, reserved size, alignment, or vertical offset. "
          "The next marker uses TEXT_BOTTOM alignment [image] before the final quarter.\n\n"
          "THREE-QUARTER CHECKPOINT [image] Fling past this image and return to it. "
          "No previously visible image should hover over later text, and no newly visible image should wait for an unrelated Label relayout.\n\n"
          "Section 5. The final long paragraph keeps the Label taller than the viewport at every supported window size. "
          "Use the Scroll Both action to compare matching offsets, or drag each column independently to stress clipping during motion.\n\n"
          "BOTTOM CHECKPOINT [image] The last image must be fully recoverable after scrolling to the end and back to the top. "
          "This final tail provides visible text below it so the image can cross the viewport edge in both directions.");
        CaseData data{"32. Tall multiline Label inside ScrollView",
                      "Drag or use Scroll Both: all eight images move with the Label, clip at viewport edges, and reappear without stale pixels.",
                      text,
                      {ObjectAt(text, 0u, "flag_kr.png", Vector2(72.0f, 48.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER),
                       ObjectAt(text, 1u, "flag_us.png", Vector2(36.0f, 24.0f), Text::ImageAttributes::InlineAlignment::TEXT_BASELINE),
                       ObjectAt(text, 2u, "flag_ae.png", Vector2(150.0f, 72.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER),
                       ObjectAt(text, 3u, "flag_kr.png", Vector2(44.0f, 32.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER),
                       ObjectAt(text, 4u, "flag_us.png", Vector2(96.0f, 120.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER),
                       ObjectAt(text, 5u, "flag_ae.png", Vector2(64.0f, 40.0f)),
                       ObjectAt(text, 6u, "flag_kr.png", Vector2(132.0f, 64.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER),
                       ObjectAt(text, 7u, "flag_us.png", Vector2(88.0f, 56.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)},
                      true, false, false, Text::Alignment::START, Text::Alignment::START,
                      Text::LineWrapMode::WORD, 23.0f};
        data.scrollCase = true;
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
        // A new handle per occurrence is intentional, including same-source cases.
        DALI_ASSERT_ALWAYS(builder.SetSpan(Text::ImageSpan::New(attributes), image.start, image.end) &&
                           "ImageSpan sample range must be valid");
      }
    }
    return builder.Build();
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
    mActionStatus.clear();
    mHorizontalAlignmentOverride = NextAlignmentOverride(mHorizontalAlignmentOverride);
    ApplyCase();
  }

  void CycleVerticalAlignment()
  {
    mActionStatus.clear();
    mVerticalAlignmentOverride = NextAlignmentOverride(mVerticalAlignmentOverride);
    ApplyCase();
  }

  void CycleOverflowMode()
  {
    mActionStatus.clear();
    mOverflowOverride = NextToggleOverride(mOverflowOverride);
    ApplyCase();
  }

  void CycleMultiline()
  {
    mActionStatus.clear();
    mMultilineOverride = NextToggleOverride(mMultilineOverride);
    ApplyCase();
  }

  void ToggleLineHeight()
  {
    mRelativeLineHeightEnabled = !mRelativeLineHeightEnabled;
    mActionStatus              = "Line height changed";
    ApplyCase();
  }

  void TogglePreviewHeight()
  {
    mWrapContentHeightEnabled = !mWrapContentHeightEnabled;
    mActionStatus             = "Preview height changed";
    ApplyCase();
  }

  void ResetLayoutOverrides()
  {
    mHorizontalAlignmentOverride = AlignmentOverride::START;
    mVerticalAlignmentOverride   = AlignmentOverride::START;
    mOverflowOverride            = ToggleOverride::ON;
    mMultilineOverride           = ToggleOverride::CASE_DEFAULT;
    mRelativeLineHeightEnabled   = false;
    mWrapContentHeightEnabled    = false;
    mReplacementEnabled          = true;
    mLifecycleAlternate          = false;
    mActionStatus                = "Case settings restored";
    ApplyCase();
  }

  void UpdateLayoutControls(const CaseData& data)
  {
    const Text::Alignment horizontal = ResolveAlignment(mHorizontalAlignmentOverride, data.horizontalAlignment);
    const Text::Alignment vertical   = ResolveAlignment(mVerticalAlignmentOverride, data.verticalAlignment);
    const bool            ellipsis   = ResolveToggle(mOverflowOverride, data.ellipsis);
    const bool            multiline  = ResolveToggle(mMultilineOverride, data.multiline);

    mHorizontalAlignmentControl.SetText((std::string("Horizontal: ") + TextAlignmentName(horizontal)).c_str());
    mVerticalAlignmentControl.SetText((std::string("Vertical: ") + TextAlignmentName(vertical)).c_str());
    mOverflowControl.SetText((std::string("Overflow: ") + (ellipsis ? "ELLIPSIS" : "CLIP")).c_str());
    mMultilineControl.SetText((std::string("Multiline: ") + (multiline ? "ON" : "OFF")).c_str());
    mLineHeightControl.SetText((std::string("Line Height: ") + (mRelativeLineHeightEnabled ? "RELATIVE 1.6" : "AUTO")).c_str());
    mPreviewHeightControl.SetText((std::string("Height: ") +
                                   (data.scrollCase
                                      ? "WRAP_CONTENT"
                                      : (mWrapContentHeightEnabled ? "WRAP_CONTENT" : "FILL"))).c_str());
    mReplacementControl.SetText((std::string("Replacement: ") + (mReplacementEnabled ? "ON" : "OFF")).c_str());
    mSourceControl.SetText((std::string("Source: ") + (mLifecycleAlternate ? "B" : "A")).c_str());
  }

  void ConfigurePreview(Label label, bool async) const
  {
    const CaseData data = GetCase(mCaseIndex);
    label.SetAsyncRendering(async);
    label.SetMultiLine(ResolveToggle(mMultilineOverride, data.multiline));
    label.SetTextOverflowMode(ResolveToggle(mOverflowOverride, data.ellipsis) ? Text::OverflowMode::ELLIPSIS : Text::OverflowMode::CLIP);
    label.SetLineWrapMode(data.lineWrapMode);
    label.SetFontSize(data.fontSize);
    label.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
    label.SetLineHeight(mRelativeLineHeightEnabled ? RELATIVE_LINE_HEIGHT : Text::LINE_HEIGHT_AUTO);
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
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(data.scrollCase
                               ? WRAP_CONTENT
                               : (mWrapContentHeightEnabled ? WRAP_CONTENT : MATCH_PARENT));
    label.SetLayoutParams(StackLayoutParams::New()
                            .SetWeight((data.scrollCase || mWrapContentHeightEnabled) ? 0.0f : 1.0f)
                            .SetAlignment(LayoutAlignment::FILL));
    label.SetMaximumWidth(10000.0f);
  }

  std::string StatusText(const CaseData& data, const Text::StyledText& styledText) const
  {
    std::ostringstream status;
    bool advancedRange = false;
    for(const ImageSpec& image : data.images)
    {
      advancedRange |= image.end - image.start != 1u;
    }
    const Text::Alignment horizontal = ResolveAlignment(mHorizontalAlignmentOverride, data.horizontalAlignment);
    const Text::Alignment vertical   = ResolveAlignment(mVerticalAlignmentOverride, data.verticalAlignment);
    const bool            ellipsis   = ResolveToggle(mOverflowOverride, data.ellipsis);
    const bool            multiline  = ResolveToggle(mMultilineOverride, data.multiline);
    status << "Layout: " << (multiline ? "multiline" : "single line") << ", "
           << (ellipsis ? "END ellipsis" : "clip") << ", horizontal=" << TextAlignmentName(horizontal)
           << ", vertical=" << TextAlignmentName(vertical) << "\n";
    status << "Text: " << (data.rtl ? "RTL" : "LTR") << ", font=" << data.fontSize
           << ", line height=" << (mRelativeLineHeightEnabled ? "RELATIVE 1.6" : "AUTO")
           << ", height=" << (data.scrollCase
                                  ? "WRAP_CONTENT"
                                  : (mWrapContentHeightEnabled ? "WRAP_CONTENT" : "FILL"))
           << ", render scale=" << data.renderScale << ", text fit=" << (data.textFit ? "ON" : "OFF")
           << ", authoring=" << (advancedRange ? "multi-character range" : "U+FFFC")
           << ", replacement=" << (mReplacementEnabled ? "ON" : "OFF") << "\n";
    status << "Images (" << styledText.GetSpanCount() << "): ";
    for(std::size_t i = 0u; i < data.images.size(); ++i)
    {
      const ImageSpec& image = data.images[i];
      if(i > 0u)
      {
        status << ", ";
      }
      status << image.size.width << "x" << image.size.height << " " << AlignmentName(image.alignment);
      if(image.verticalOffset != 0.0f)
      {
        status << " offset=" << image.verticalOffset;
      }
    }
    status << "\n";
    if(data.marqueeCase)
    {
      status << "Marquee: " << (mPrimary.IsMarqueeRunning() ? "RUNNING" : "STOPPED") << "\n";
    }
    if(data.lifecycleCase)
    {
      status << "Resource source variant: " << (mLifecycleAlternate ? "B" : "A") << "\n";
    }
    if(data.scrollCase)
    {
      const Vector2 syncPosition  = mPrimaryScroll.GetScrollPosition();
      const Vector2 asyncPosition = mParityScroll.GetScrollPosition();
      status << "Scroll Y: sync=" << syncPosition.y << ", async=" << asyncPosition.y
             << ". Drag either preview or press Scroll Both.\n";
    }
    if(!mActionStatus.empty())
    {
      status << mActionStatus;
    }
    return status.str();
  }

  void ApplyCase()
  {
    const CaseData        data       = GetCase(mCaseIndex);
    const Text::StyledText styledText = BuildStyledText(data, mReplacementEnabled);
    mPrimary.StopMarquee();
    mParity.StopMarquee();
    ConfigurePreview(mPrimary, false);
    ConfigurePreview(mParity, true);
    mPrimary.SetStyledText(styledText);
    mParity.SetStyledText(styledText); // Same immutable source; runtime remains control-owned.
    mPrimaryScroll.ScrollTo(Vector2::ZERO, false);
    mParityScroll.ScrollTo(Vector2::ZERO, false);
    mScrollStep = 0u;
    std::string displayTitle(data.title);
    const std::size_t numberSeparator = displayTitle.find(". ");
    if(numberSeparator != std::string::npos)
    {
      displayTitle.erase(0u, numberSeparator + 2u);
    }
    mTitle.SetText(("Case " + std::to_string(mCaseIndex + 1u) + " / " + std::to_string(CASE_COUNT) + " — " + displayTitle).c_str());
    mDescription.SetText(data.expected);
    mExpected.SetText(data.scrollCase
                        ? "Expected: sync and async Labels keep identical image geometry while their ScrollViews clip moving content."
                        : "Expected: sync and async output match; every replacement remains one complete layout unit.");
    mActionControl.SetText(data.scrollCase ? "Scroll Both" : "Start Marquee");
    mStatus.SetText(StatusText(data, styledText).c_str());
    UpdateLayoutControls(data);
  }

  void RefreshStatus()
  {
    const CaseData data = GetCase(mCaseIndex);
    mStatus.SetText(StatusText(data, mPrimary.GetStyledText()).c_str());
  }

  void RunPreviewAction()
  {
    const CaseData data = GetCase(mCaseIndex);
    if(!data.scrollCase)
    {
      mPrimary.StartMarquee();
      mParity.StartMarquee();
      mActionStatus = "Marquee requested for both columns";
      RefreshStatus();
      return;
    }

    mScrollStep              = (mScrollStep + 1u) % 4u;
    const float progress     = static_cast<float>(mScrollStep) / 3.0f;
    const float primaryRange = std::max(0.0f, mPrimary.GetCurrentSize().height - mPrimaryScroll.GetCurrentSize().height);
    const float parityRange  = std::max(0.0f, mParity.GetCurrentSize().height - mParityScroll.GetCurrentSize().height);
    mPrimaryScroll.ScrollTo(Vector2(0.0f, primaryRange * progress), false);
    mParityScroll.ScrollTo(Vector2(0.0f, parityRange * progress), false);
    mActionStatus = "Both ScrollViews moved to the next checkpoint";
    RefreshStatus();
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

    mTitle       = NewHudLabel("", 48.0f, 0x1D4ED8);
    mDescription = NewHudLabel("", 64.0f, 0x334155);
    mStatus      = NewHudLabel("", 142.0f, 0x1E293B);
    mExpected    = NewHudLabel("", 50.0f, 0x334155);
    mStatus.SetHorizontalTextAlignment(Text::Alignment::START);
    mStatus.SetFontSize(14.0f);
    mDescription.SetFontSize(15.0f);

    StackLayout actions = StackLayout::New(StackOrientation::HORIZONTAL);
    actions.SetSpacing(6.0f);
    actions.SetRequestedWidth(MATCH_PARENT);
    actions.SetRequestedHeight(44.0f);
    Label previous = NewHudLabel("Previous Case", 44.0f, 0x1D4ED8, true);
    Label next     = NewHudLabel("Next Case", 44.0f, 0x1D4ED8, true);
    Label reset    = NewHudLabel("Reset Case", 44.0f, 0x334155, true);
    mActionControl = NewHudLabel("Start Marquee", 44.0f, 0x475569, true);
    for(Label button : {previous, next, reset, mActionControl})
    {
      actions.Add(button);
    }
    previous.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) {
      mActionStatus.clear();
      mCaseIndex = (mCaseIndex + CASE_COUNT - 1u) % CASE_COUNT;
      ApplyCase();
    });
    next.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) {
      mActionStatus.clear();
      mCaseIndex = (mCaseIndex + 1u) % CASE_COUNT;
      ApplyCase();
    });
    reset.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { ResetLayoutOverrides(); });
    mActionControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { RunPreviewAction(); });

    StackLayout layoutActions = StackLayout::New(StackOrientation::HORIZONTAL);
    layoutActions.SetSpacing(6.0f);
    layoutActions.SetRequestedWidth(MATCH_PARENT);
    layoutActions.SetRequestedHeight(44.0f);
    StackLayout contentActions = StackLayout::New(StackOrientation::HORIZONTAL);
    contentActions.SetSpacing(6.0f);
    contentActions.SetRequestedWidth(MATCH_PARENT);
    contentActions.SetRequestedHeight(44.0f);
    mHorizontalAlignmentControl = NewHudLabel("", 44.0f, 0x475569, true);
    mVerticalAlignmentControl   = NewHudLabel("", 44.0f, 0x475569, true);
    mOverflowControl            = NewHudLabel("", 44.0f, 0x475569, true);
    mMultilineControl           = NewHudLabel("", 44.0f, 0x475569, true);
    mLineHeightControl          = NewHudLabel("", 44.0f, 0x475569, true);
    mPreviewHeightControl       = NewHudLabel("", 44.0f, 0x475569, true);
    mReplacementControl         = NewHudLabel("", 44.0f, 0x475569, true);
    mSourceControl              = NewHudLabel("", 44.0f, 0x475569, true);
    for(Label control : {mHorizontalAlignmentControl, mVerticalAlignmentControl, mOverflowControl})
    {
      layoutActions.Add(control);
    }
    for(Label control : {mMultilineControl, mLineHeightControl, mPreviewHeightControl, mReplacementControl, mSourceControl})
    {
      contentActions.Add(control);
    }
    mHorizontalAlignmentControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { CycleHorizontalAlignment(); });
    mVerticalAlignmentControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { CycleVerticalAlignment(); });
    mOverflowControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { CycleOverflowMode(); });
    mMultilineControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { CycleMultiline(); });
    mLineHeightControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { ToggleLineHeight(); });
    mPreviewHeightControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { TogglePreviewHeight(); });
    mReplacementControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) {
      mActionStatus.clear();
      mReplacementEnabled = !mReplacementEnabled;
      ApplyCase();
    });
    mSourceControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) {
      mActionStatus.clear();
      mLifecycleAlternate = !mLifecycleAlternate;
      ApplyCase();
    });

    StackLayout previews = StackLayout::New(StackOrientation::HORIZONTAL);
    previews.SetSpacing(8.0f);
    previews.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mPrimary       = Label::New();
    mParity        = Label::New();
    mPrimaryScroll = ScrollView::New();
    mParityScroll  = ScrollView::New();
    StackLayout syncColumn       = StackLayout::New(StackOrientation::VERTICAL);
    StackLayout asyncColumn      = StackLayout::New(StackOrientation::VERTICAL);
    StackLayout syncPreviewSlot  = StackLayout::New(StackOrientation::VERTICAL);
    StackLayout asyncPreviewSlot = StackLayout::New(StackOrientation::VERTICAL);
    for(StackLayout column : {syncColumn, asyncColumn})
    {
      column.SetSpacing(6.0f);
      column.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
      previews.Add(column);
    }
    for(StackLayout slot : {syncPreviewSlot, asyncPreviewSlot})
    {
      slot.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    }
    for(ScrollView scroll : {mPrimaryScroll, mParityScroll})
    {
      scroll.SetScrollDirection(ScrollDirection::Vertical);
      scroll.SetOverScrollMode(OverScrollMode::ContentScrolls);
      scroll.SetRequestedWidth(MATCH_PARENT);
      scroll.SetRequestedHeight(MATCH_PARENT);
      scroll.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    }
    syncColumn.Add(NewHudLabel("SYNC", 34.0f, 0x0F766E));
    asyncColumn.Add(NewHudLabel("ASYNC", 34.0f, 0x7C3AED));
    syncColumn.Add(syncPreviewSlot);
    asyncColumn.Add(asyncPreviewSlot);
    for(Label label : {mPrimary, mParity})
    {
      label.SetFontSize(28.0f);
      label.SetTextColor(UiColor(0x111827));
      label.SetBackgroundColor(UiColor(0xF8FAFC));
      label.SetPadding(Extents(14, 14, 14, 14));
      label.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    }
    mPrimaryScroll.SetContent(mPrimary);
    mParityScroll.SetContent(mParity);
    syncPreviewSlot.Add(mPrimaryScroll);
    asyncPreviewSlot.Add(mParityScroll);

    root.Add(mTitle);
    root.Add(mDescription);
    root.Add(actions);
    root.Add(layoutActions);
    root.Add(contentActions);
    root.Add(previews);
    root.Add(mStatus);
    root.Add(mExpected);
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
      mActionStatus.clear();
      mCaseIndex = (mCaseIndex + CASE_COUNT - 1u) % CASE_COUNT;
      ApplyCase();
    }
    else if(event.GetKeyName() == "Right")
    {
      mActionStatus.clear();
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
    else if(event.GetKeyName() == "g" || event.GetKeyName() == "G")
    {
      ToggleLineHeight();
    }
    else if(event.GetKeyName() == "w" || event.GetKeyName() == "W")
    {
      TogglePreviewHeight();
    }
    else if(event.GetKeyName() == "d" || event.GetKeyName() == "D")
    {
      ResetLayoutOverrides();
    }
    else if(event.GetKeyName() == "l" || event.GetKeyName() == "L")
    {
      mActionStatus.clear();
      mLifecycleAlternate = !mLifecycleAlternate;
      ApplyCase();
    }
    else if(event.GetKeyName() == "1")
    {
      UiScaleManager::Get().SetScale(0.8f);
    }
    else if(event.GetKeyName() == "2")
    {
      UiScaleManager::Get().SetScale(1.0f);
    }
    else if(event.GetKeyName() == "3")
    {
      UiScaleManager::Get().SetScale(1.2f);
    }
    else if(event.GetKeyName() == "4")
    {
      UiScaleManager::Get().SetScale(1.5f);
    }
    else if(event.GetKeyName() == "5")
    {
      UiScaleManager::Get().SetScale(2.0f);
    }
  }

private:
  Application&      mApplication;
  Label             mTitle;
  Label             mDescription;
  Label             mExpected;
  Label             mPrimary;
  Label             mParity;
  ScrollView        mPrimaryScroll;
  ScrollView        mParityScroll;
  Label             mStatus;
  Label             mActionControl;
  Label             mHorizontalAlignmentControl;
  Label             mVerticalAlignmentControl;
  Label             mOverflowControl;
  Label             mMultilineControl;
  Label             mLineHeightControl;
  Label             mPreviewHeightControl;
  Label             mReplacementControl;
  Label             mSourceControl;
  std::size_t       mCaseIndex{0u};
  bool              mReplacementEnabled{true};
  bool              mLifecycleAlternate{false};
  bool              mRelativeLineHeightEnabled{false};
  bool              mWrapContentHeightEnabled{false};
  AlignmentOverride mHorizontalAlignmentOverride{AlignmentOverride::START};
  AlignmentOverride mVerticalAlignmentOverride{AlignmentOverride::START};
  ToggleOverride    mOverflowOverride{ToggleOverride::ON};
  ToggleOverride    mMultilineOverride{ToggleOverride::CASE_DEFAULT};
  uint32_t          mScrollStep{0u};
  std::string       mActionStatus;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TextImageSpanController controller(application);
  application.MainLoop();
  return 0;
}
