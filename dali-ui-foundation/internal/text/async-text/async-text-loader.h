#ifndef DALI_UI_TEXT_ASYNC_TEXT_LOADER_H
#define DALI_UI_TEXT_ASYNC_TEXT_LOADER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/async-text/async-text-module.h>
#include <dali-ui-foundation/internal/text/text-enumerations.h>
#include <dali-ui-foundation/internal/text/text-model-interface.h>
#include <dali-ui-foundation/public-api/text/fit/text-fit-candidate.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/rendering/visual-renderer.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class AsyncTextLoader;

} // namespace Internal DALI_INTERNAL

namespace Async
{
enum RequestType
{
  RENDER_FIXED_SIZE,
  RENDER_FIXED_WIDTH,
  RENDER_FIXED_HEIGHT,
  RENDER_CONSTRAINT,
  COMPUTE_NATURAL_SIZE,
  COMPUTE_HEIGHT_FOR_WIDTH,
};
const char* const RequestTypeName[] = {"RENDER_FIXED_SIZE", "RENDER_FIXED_WIDTH", "RENDER_FIXED_HEIGHT",
                                       "RENDER_CONSTRAINT", "COMPUTE_NATURAL_SIZE", "COMPUTE_HEIGHT_FOR_WIDTH"};
} // namespace Async

struct AsyncTextParameters
{
  AsyncTextParameters()
  : text{},
    fontFamily{},
    textColor{Color::BLACK},
    underlineColor{Color::BLACK},
    strikethroughColor{Color::BLACK},
    shadowColor{Color::BLACK},
    outlineColor{Color::WHITE},
    backgroundColorWithCutout{Color::TRANSPARENT},
    textBackgroundColor{Color::TRANSPARENT},
    embossLightColor{Color::TRANSPARENT},
    embossShadowColor{Color::TRANSPARENT},
    shadowOffset{},
    outlineOffset{},
    embossDirection{},
    padding{0u, 0u, 0u, 0u},
    variationsMap{},
    textFitCandidates{},
    fontSize{0.f},
    minLineSize{0.f},
    relativeLineSize{1.f},
    characterSpacing{0.f},
    fontSizeScale{1.f},
    textWidth{0.f},
    textHeight{0.f},
    originWidth{0.f},
    originHeight{0.f},
    underlineHeight{0.f},
    dashedUnderlineWidth{2.f},
    dashedUnderlineGap{1.f},
    strikethroughHeight{0.f},
    shadowBlurRadius{0.f},
    outlineBlurRadius{0.f},
    embossStrength{0.f},
    textFitMinSize{10.f},
    textFitMaxSize{100.f},
    textFitStepSize{1.f},
    marqueeLoopDelay{0.0f},
    renderScale{1.0f},
    renderScaleWidth{0.f},
    renderScaleHeight{0.f},
    maxTextureSize{0},
    marqueeSpeed{1},
    marqueeLoopCount{1},
    marqueeGap{0},
    outlineWidth{0u},
    requestType{Async::RENDER_FIXED_SIZE},
    horizontalAlignment{Alignment::START},
    verticalAlignment{Alignment::START},
    lineWrapMode{LineWrapMode::WORD},
    underlineType{Text::Underline::Type::SOLID},
    layoutDirection{Dali::LayoutDirection::LEFT_TO_RIGHT},
    verticalLineAlignment{Alignment::START},
    layoutDirectionPolicy{LayoutDirectionMode::CONTENTS},
    ellipsisPosition{Text::EllipsisPosition::END},
    ellipsisMode{Text::Ellipsize::TRUNCATE},
    marqueeOrientation{Text::MarqueeOrientation::HORIZONTAL},
    marqueeStopMode{Text::MarqueeStopMode::FINISH_LOOP},
    fontWeight{FontWeight::NONE},
    fontWidth{FontWidth::NONE},
    fontSlant{FontSlant::NONE},
    isMultiLine{false},
    ellipsis{true},
    enableMarkup{false},
    isUnderlineEnabled{false},
    isStrikethroughEnabled{false},
    isTextBackgroundEnabled{false},
    isTextFitEnabled{false},
    isTextFitCandidatesEnabled{false},
    isMarqueeEnabled{false},
    isMarqueeMaxTextureExceeded{false},
    isCutoutEnabled{false},
    isBackgroundWithCutoutEnabled{false},
    isEmbossEnabled{false}
  {
  }

  ~AsyncTextParameters() = default;

  std::string text;       ///< The text to be rendered encoded in utf8.
  std::string fontFamily; ///< The font's family.

  Vector4 textColor; ///< The default text's color. Default is white.
  Vector4 underlineColor;
  Vector4 strikethroughColor;
  Vector4 shadowColor;
  Vector4 outlineColor;
  Vector4 backgroundColorWithCutout; ///< Background color with cutout.
  Vector4 textBackgroundColor;
  Vector4 embossLightColor;
  Vector4 embossShadowColor;

  Vector2 shadowOffset;
  Vector2 outlineOffset;
  Vector2 embossDirection;

  Extents padding; ///< The padding of the boundaries where the text is going to be laid-out.

  Property::Map                    variationsMap; ///< The map for variable fonts. it might be replaced by variable map run.
  Dali::Vector<Text::FitCandidate> textFitCandidates;

  float fontSize;         ///< The font's size (in pixels).
  float minLineSize;      ///< The line's minimum size (in pixels).
  float relativeLineSize; ///< The relative height of the line (a factor that will be multiplied by text height).
  float characterSpacing; ///< The space between characters.
  float fontSizeScale;    ///< The font's size scale.
  float textWidth;        ///< The width in pixels of the boundaries where the text is going to be laid-out.
  float textHeight;       ///< The height in pixels of the boundaries where the text is going to be laid-out.
  float originWidth;
  float originHeight;
  float underlineHeight;
  float dashedUnderlineWidth;
  float dashedUnderlineGap;
  float strikethroughHeight;
  float shadowBlurRadius;
  float outlineBlurRadius;
  float embossStrength;
  float textFitMinSize;
  float textFitMaxSize;
  float textFitStepSize;
  float marqueeLoopDelay;
  float renderScale;       ///< The render scale.
  float renderScaleWidth;  ///< The requested original textWidth when using render scale.
  float renderScaleHeight; ///< The requested original textHeight when using render scale.

  int maxTextureSize; ///< The maximum size of texture.
  int marqueeSpeed;   ///< marquee properties.
  int marqueeLoopCount;
  int marqueeGap;

  uint16_t outlineWidth; ///< The width of the outline, if it is greater than 1, it is enabled.

  Async::RequestType          requestType;
  Alignment                   horizontalAlignment;   ///< The horizontal alignment: one of {START, CENTER, END}.
  Alignment                   verticalAlignment;     ///< The vertical alignment: one of {START, CENTER, END}.
  LineWrapMode                lineWrapMode;          ///< The line wrap mode: one of {WORD, CHARACTER, HYPHENATION, MIXED}.
  Text::Underline::Type       underlineType;         ///< The type of underline: one of {SOLID, DASHED, DOUBLE}.
  Dali::LayoutDirection::Type layoutDirection;       ///< The layout direction: one of {LEFT_TO_RIGHT, RIGHT_TO_LEFT}.
  Alignment                   verticalLineAlignment; ///< The vertical line alignment: one of {START, CENTER, END}.
  LayoutDirectionMode         layoutDirectionPolicy; ///< The policy used to set the text layout direction : one of {INHERIT, LOCALE, CONTENTS}.
  Text::EllipsisPosition::Type
                           ellipsisPosition;   ///< The position of the ellipsis glyph: one of {END, START, MIDDLE}.
  Text::Ellipsize::Mode    ellipsisMode;       ///< The mode of the ellipsis: one of {TRUNCATE, MARQUEE}.
  Text::MarqueeOrientation marqueeOrientation; ///< The orientation of the marquee {HORIZONTAL, VERTICAL}.
  Text::MarqueeStopMode    marqueeStopMode;    ///< The marquee stop mode: one of {FINISH_LOOP, IMMEDIATE}.
  FontWeight               fontWeight;         ///< The font's weight.
  FontWidth                fontWidth;          ///< The font's width.
  FontSlant                fontSlant;          ///< The font's slant.

  bool isMultiLine : 1;                   ///< Whether the multi-line layout is enabled.
  bool ellipsis : 1;                      ///< Whether the ellipsis layout option is enabled.
  bool enableMarkup : 1;                  ///< Whether the mark-up processor is enabled.
  bool isUnderlineEnabled : 1;            ///< Underline enabeld flag.
  bool isStrikethroughEnabled : 1;        ///< Strikethrough enabeld flag.
  bool isTextBackgroundEnabled : 1;       ///< Text background flag.
  bool isTextFitEnabled : 1;              ///< TextFit enabeld flag.
  bool isTextFitCandidatesEnabled : 1;    ///< TextFit Candidates enabeld flag.
  bool isMarqueeEnabled : 1;              ///< Marquee enabeld flag.
  bool isMarqueeMaxTextureExceeded : 1;   ///< Whether the marquee texture size exceeds the maximum texture width.
  bool isCutoutEnabled : 1;               ///< Cutout enabled flag.
  bool isBackgroundWithCutoutEnabled : 1; ///< Background with cutout enabled flag.
  bool isEmbossEnabled : 1;               ///< Emboss enabled flag.
};

struct AsyncTextRenderInfo
{
  AsyncTextRenderInfo()
  : requestType(Async::RENDER_FIXED_SIZE),
    textPixelData(),
    stylePixelData(),
    overlayStylePixelData(),
    maskPixelData(),
    marqueePixelData(),
    size(),
    controlSize(),
    renderedSize(),
    lineCount(0),
    marqueeWrapGap(0.f),
    hasMultipleTextColors(false),
    containsColorGlyph(false),
    styleEnabled(false),
    isOverlayStyle(false),
    isTextDirectionRTL(false),
    isCutoutEnabled(false),
    isEmbossEnabled(false)
  {
  }

  ~AsyncTextRenderInfo()
  {
  }
  Async::RequestType requestType;
  PixelData          textPixelData;
  PixelData          stylePixelData;
  PixelData          overlayStylePixelData;
  PixelData          maskPixelData;
  PixelData          marqueePixelData;
  Size               size;         ///< Actual rendered buffer size. For marquee, this is the scrolling texture size.
  Size               controlSize;  ///< View size used to display the rendered text.
  Size               renderedSize; ///< Final displayed size reported back to the caller.
  int                lineCount;
  float              marqueeWrapGap;
  bool               hasMultipleTextColors : 1;
  bool               containsColorGlyph : 1;
  bool               styleEnabled : 1;
  bool               isOverlayStyle : 1;
  bool               isTextDirectionRTL : 1;
  bool               isCutoutEnabled : 1;
  bool               isEmbossEnabled : 1;
};

/**
 * AsyncTextLoader
 *
 */
class AsyncTextLoader : public BaseHandle
{
public:
  /**
   * @brief Create an uninitialized AsyncTextLoader handle.
   *
   */
  AsyncTextLoader();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AsyncTextLoader();

  /**
   * @brief This constructor is used by AsyncTextLoader::Get().
   *
   * @param[in] implementation a pointer to the internal async text loader object.
   */
  explicit DALI_INTERNAL AsyncTextLoader(Internal::AsyncTextLoader* implementation);

  /**
   * @brief Create a handle to the new AsyncTextLoader instance.
   *
   * @return A handle to the AsyncTextLoader.
   */
  static AsyncTextLoader New();

  /**
   * @brief Sets the locale.
   *
   * @param[in] locale The locale.
   */
  void SetLocale(const std::string& locale);

  /**
   * @brief Sets a flag indicating that module's locale updating is needed.
   *
   * When the async text loader is available, update is processed on the main thread.
   *
   * @param[in] update Whether to update the locale or not.
   */
  void SetLocaleUpdateNeeded(bool update);

  /**
   * @brief Whether module's locale updating is needed.
   *
   * @return A flag that indicates whether the locale should be updated or not.
   */
  bool IsLocaleUpdateNeeded();

  /**
   * @brief Clear the cache of the async text module.
   */
  void ClearModule();

  /**
   * @brief Sets custom fonts directories.
   *
   * @param[in] customFontDirectories List of the custom font paths.
   */
  void SetCustomFontDirectories(const TextAbstraction::FontPathList& customFontDirectories);

  /**
   * @brief Request adds a custom font directory.
   *
   * @param[in] path The path of the custom font directory.
   */
  void RequestAddCustomFont(const std::string& path);

  /**
   * @brief Sets a flag indicating that module's cache clearing is needed.
   *
   * When the async text loader is available, clear is processed on the main thread.
   *
   * @param[in] clear Whether to clear the cache or not.
   */
  void SetModuleClearNeeded(bool clear);

  /**
   * @brief Whether module's cache clearing is needed.
   *
   * @return A flag that indicates whether the cache should be cleared or not.
   */
  bool IsModuleClearNeeded();

  /**
   * @brief Setup render scale.
   * Sets the control size to be rendered to fit the given scale.
   * The scaled rendering result cannot be exactly the same as the original.
   * However, we guarantee the ellipsis result.
   * If the original is ellipsised, the scaled result will always be ellipsised.
   * If the original is not ellipsised, the scaled result will not be ellipsised.
   * Occasionally, the scaled result exceeds the size of the control.
   * Since we need to ensure the size of the control, we slightly reduce the glyph's advance to adjust the total width
   * to fit the control size. While this may cause rendering quality issues at smaller point sizes, there is almost no
   * noticeable difference at moderate sizes of 20pt or larger.
   *
   * @param[in] parameters All options required to compute size of text.
   * @param[out] cachedNaturalSize Whether the natural size has been calculated.
   *
   * @return The natural size of text.
   */
  Size SetupRenderScale(AsyncTextParameters& parameters, bool& cachedNaturalSize);

  /**
   * @brief Compute natural size of text.
   *
   * @param[in] parameters All options required to compute size of text.
   *
   * @return The natural size of text.
   */
  Size ComputeNaturalSize(AsyncTextParameters& parameters);

  /**
   * @brief Compute height for width of text.
   *
   * @param[in] parameters All options required to compute height of text.
   * @param[in] width The width of text to compute.
   * @param[in] layoutOnly If there is no need to Initialize/Update, only the Layout is performed.
   *
   * @return The height for width of text.
   */
  float ComputeHeightForWidth(AsyncTextParameters& parameters, float width, bool layoutOnly);

  /**
   * @brief Renders text into a pixel buffer.
   *
   * @param[in] parameters All options required to render text.
   * @param[in] useCachedNaturalSize Indicates whether to use the provided natural size or calculate it internally.
   * @param[in] naturalSize The natural size of the text to be used if useCachedNaturalSize is true.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo RenderText(AsyncTextParameters& parameters, bool useCachedNaturalSize, const Size& naturalSize);

  /**
   * @brief Renders text into a pixel buffer.
   *
   * @param[in] parameters All options required to render text.
   * @param[in] useCachedNaturalSize Indicates whether to use the provided natural size or calculate it internally.
   * @param[in] naturalSize The natural size of the text to be used if useCachedNaturalSize is true.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo RenderTextFit(AsyncTextParameters& parameters, bool useCachedNaturalSize,
                                    const Size& naturalSize);

  /**
   * @brief Renders text into a pixel buffer.
   *
   * @param[in] parameters All options required to render text.
   * @param[in] useCachedNaturalSize Indicates whether to use the provided natural size or calculate it internally.
   * @param[in] naturalSize The natural size of the text to be used if useCachedNaturalSize is true.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo RenderMarquee(AsyncTextParameters& parameters, bool useCachedNaturalSize,
                                    const Size& naturalSize);

  /**
   * @brief Gets the natural size of text.
   *
   * @param[in] parameters All options required to compute text.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo GetNaturalSize(AsyncTextParameters& parameters);

  /**
   * @brief Gets the height for width of text.
   *
   * @param[in] parameters All options required to compute text.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo GetHeightForWidth(AsyncTextParameters& parameters);

public:
  // Default copy and move operator
  AsyncTextLoader(const AsyncTextLoader& rhs)            = default;
  AsyncTextLoader(AsyncTextLoader&& rhs)                 = default;
  AsyncTextLoader& operator=(const AsyncTextLoader& rhs) = default;
  AsyncTextLoader& operator=(AsyncTextLoader&& rhs)      = default;
};

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_ASYNC_TEXT_LOADER_H
