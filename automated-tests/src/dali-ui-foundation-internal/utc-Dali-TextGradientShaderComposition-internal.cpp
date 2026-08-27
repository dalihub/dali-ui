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

#include <dali-ui-foundation/integration-api/text/text-scroller-interface.h>
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/text/marquee/marquee-builder.h>
#include <dali-ui-foundation/internal/text/text-gradient-bounds.h>
#include <dali-ui-foundation/internal/text/text-gradient-helper.h>
#include <dali-ui-foundation/internal/text/text-gradient-marquee-helper.h>
#include <dali-ui-foundation/internal/text/text-scroller.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual-shader-factory.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <mesh-builder.h>

#include <string>
#include <vector>

using namespace Dali;

namespace
{

namespace TextFeature = Dali::Ui::Internal::TextVisualShaderFeature;
namespace TextInternal = Dali::Ui::Text::Internal;
namespace UiIntegrationText = Dali::Ui::Integration::Text;
namespace UiText      = Dali::Ui::Text;
namespace UiInternal  = Dali::Ui::Internal;

constexpr const char* TEXT_GRADIENT_DEFINE         = "#define IS_REQUIRED_TEXT_GRADIENT\n";
constexpr const char* TEXT_GRADIENT_MIXED_DEFINE   = "#define IS_REQUIRED_TEXT_GRADIENT_MIXED\n";
constexpr const char* TEXT_GRADIENT_OVERLAY_DEFINE = "#define IS_REQUIRED_TEXT_GRADIENT_OVERLAY\n";
constexpr const char* TEXT_REVEAL_DEFINE          = "#define IS_REQUIRED_TEXT_REVEAL\n";
constexpr const char* TEXT_STYLE_DEFINE            = "#define IS_REQUIRED_TEXT_STYLE\n";
constexpr const char* TEXT_OVERLAY_STYLE_DEFINE    = "#define IS_REQUIRED_TEXT_OVERLAY_STYLE\n";
constexpr float       EPSILON                      = 0.0001f;

std::string GetFragmentPrefix(const TextFeature::FeatureBuilder& builder)
{
  std::string vertexPrefix;
  std::string fragmentPrefix;

  builder.GetVertexShaderPrefixList(vertexPrefix);
  builder.GetFragmentShaderPrefixList(fragmentPrefix);

  DALI_TEST_EQUALS(vertexPrefix.empty(), true, TEST_LOCATION);
  return fragmentPrefix;
}

void ExpectNoTextGradientDefine(const std::string& fragmentPrefix)
{
  DALI_TEST_EQUALS(fragmentPrefix.find(TEXT_GRADIENT_DEFINE) == std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find(TEXT_GRADIENT_MIXED_DEFINE) == std::string::npos, true, TEST_LOCATION);
}

void ExpectNoTextGradientOverlayDefine(const std::string& fragmentPrefix)
{
  DALI_TEST_EQUALS(fragmentPrefix.find(TEXT_GRADIENT_OVERLAY_DEFINE) == std::string::npos, true, TEST_LOCATION);
}

size_t CountOccurrences(const std::string& text, const std::string& token)
{
  size_t count = 0u;
  for(size_t position = text.find(token); position != std::string::npos;
      position = text.find(token, position + token.size()))
  {
    ++count;
  }
  return count;
}

void ExpectTextGradientDefine(const std::string& fragmentPrefix)
{
  DALI_TEST_EQUALS(fragmentPrefix.find(TEXT_GRADIENT_DEFINE) != std::string::npos, true, TEST_LOCATION);
}

void ExpectTextGradientOverlayDefine(const std::string& fragmentPrefix)
{
  DALI_TEST_EQUALS(fragmentPrefix.find(TEXT_GRADIENT_OVERLAY_DEFINE) != std::string::npos, true, TEST_LOCATION);
}

void ExpectTextGradientMixedDefine(const std::string& fragmentPrefix)
{
  ExpectTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find(TEXT_GRADIENT_MIXED_DEFINE) != std::string::npos, true, TEST_LOCATION);
}

TextInternal::Gradient::Style CreateLinearGradientStyle(const Vector2& start = Vector2::ZERO,
                                                        const Vector2& end   = Vector2::ONE)
{
  TextInternal::Gradient::Style style;
  style.enabled     = true;
  style.type        = Dali::Ui::Gradient::Type::LINEAR;
  style.linearStart = start;
  style.linearEnd   = end;
  style.stops.PushBack({0.0f, Color::RED});
  style.stops.PushBack({1.0f, Color::BLUE});
  return style;
}

PixelData CreatePixelData(uint32_t width, uint32_t height, Pixel::Format pixelFormat)
{
  const uint32_t bufferSize = width * height * Pixel::GetBytesPerPixel(pixelFormat);
  return PixelData::New(new uint8_t[bufferSize](), bufferSize, width, height, pixelFormat, PixelData::DELETE_ARRAY);
}

void ExpectBounds(const Vector4& bounds, const Vector4& expected)
{
  DALI_TEST_EQUALS(bounds.x, expected.x, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(bounds.y, expected.y, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(bounds.z, expected.z, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(bounds.w, expected.w, EPSILON, TEST_LOCATION);
}

void ExpectPosition(const Vector2& position, const Vector2& expected)
{
  DALI_TEST_EQUALS(position.x, expected.x, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, expected.y, EPSILON, TEST_LOCATION);
}

UiText::LineRun MakeLine(float width,
                         float alignmentOffset,
                         float ascender = 8.0f,
                         float descender = -2.0f,
                         float lineSpacing = 0.0f)
{
  UiText::LineRun line;
  line.width           = width;
  line.alignmentOffset = alignmentOffset;
  line.ascender        = ascender;
  line.descender       = descender;
  line.lineSpacing     = lineSpacing;
  return line;
}

void ExpectNoTextGradientRendererProperties(Renderer renderer)
{
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientStartPosition"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientEndPosition"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientStartOffset"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientBounds"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientType"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientRadialCenter"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientRadialScale"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientConicCenter"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientConicScale"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientConicStartAngle"), Property::INVALID_INDEX, TEST_LOCATION);
}

void ExpectNoTextGradientOverlayRendererProperties(Renderer renderer)
{
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayStartPosition"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayEndPosition"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayStartOffset"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayBounds"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayType"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayRadialCenter"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayRadialScale"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayConicCenter"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayConicScale"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayConicStartAngle"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientOverlayMode"), Property::INVALID_INDEX, TEST_LOCATION);
}

class TestScrollerInterface : public UiIntegrationText::ScrollerInterface
{
public:
  void ScrollingFinished() override
  {
  }
};

void ExpectMarqueeCompositionResult(bool hasMultipleTextColors,
                                    bool containsColorGlyph,
                                    bool styleTextureEnabled,
                                    bool isOverlayStyle,
                                    bool embossEnabled,
                                    bool cutoutEnabled,
                                    bool expectedSupported,
                                    TextInternal::GradientMarquee::CompositionUnsupportedReason expectedReason,
                                    uint32_t expectedResourceFlags =
                                      static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionResourceFlag::TEXT_TEXTURE),
                                    uint32_t expectedShaderFeatureFlags =
                                      static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionShaderFeatureFlag::NONE))
{
  TextInternal::GradientMarquee::CompositionPolicy policy;
  policy.hasMultipleTextColors = hasMultipleTextColors;
  policy.containsColorGlyph    = containsColorGlyph;
  policy.styleTextureEnabled   = styleTextureEnabled;
  policy.isOverlayStyle        = isOverlayStyle;
  policy.embossEnabled         = embossEnabled;
  policy.cutoutEnabled         = cutoutEnabled;

  const TextInternal::GradientMarquee::CompositionResult result =
    TextInternal::GradientMarquee::GetCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, expectedSupported, TEST_LOCATION);
  DALI_TEST_EQUALS(result.unsupportedReason == expectedReason, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.requiredResourceFlags, expectedResourceFlags, TEST_LOCATION);
  DALI_TEST_EQUALS(result.shaderFeatureFlags, expectedShaderFeatureFlags, TEST_LOCATION);
  DALI_TEST_EQUALS(TextInternal::GradientMarquee::IsCompositionSupported(hasMultipleTextColors,
                                                                         containsColorGlyph,
                                                                         styleTextureEnabled,
                                                                         isOverlayStyle,
                                                                         embossEnabled,
                                                                         cutoutEnabled),
                   expectedSupported,
                   TEST_LOCATION);
}

void ExpectMarqueeMixedColorCompositionResult(bool baseGradientEnabled,
                                              bool overlayGradientEnabled,
                                              bool hasMultipleTextColors,
                                              bool containsColorGlyph,
                                              bool styleTextureEnabled,
                                              bool isOverlayStyle,
                                              bool embossEnabled,
                                              bool cutoutEnabled,
                                              bool expectedSupported)
{
  TextInternal::GradientMarquee::CompositionPolicy policy;
  policy.hasMultipleTextColors  = hasMultipleTextColors;
  policy.containsColorGlyph     = containsColorGlyph;
  policy.styleTextureEnabled    = styleTextureEnabled;
  policy.isOverlayStyle         = isOverlayStyle;
  policy.embossEnabled          = embossEnabled;
  policy.cutoutEnabled          = cutoutEnabled;
  policy.baseGradientEnabled    = baseGradientEnabled;
  policy.overlayGradientEnabled = overlayGradientEnabled;

  const TextInternal::GradientMarquee::CompositionResult result =
    TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, expectedSupported, TEST_LOCATION);
  DALI_TEST_EQUALS(TextInternal::GradientMarquee::IsMixedColorCompositionSupported(policy), expectedSupported, TEST_LOCATION);
  if(expectedSupported)
  {
    const uint32_t expectedResourceFlags =
      static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionResourceFlag::PRESERVED_COLOR_TEXTURE) |
      static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionResourceFlag::GRADIENT_MASK_TEXTURE) |
      static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionResourceFlag::TEXT_GRADIENT_LOOKUP_TEXTURE) |
      (styleTextureEnabled ? static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionResourceFlag::STYLE_TEXTURE) : 0u) |
      (overlayGradientEnabled ? static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionResourceFlag::TEXT_GRADIENT_OVERLAY_LOOKUP_TEXTURE) : 0u) |
      (isOverlayStyle ? static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionResourceFlag::OVERLAY_STYLE_TEXTURE) : 0u);
    const uint32_t expectedShaderFeatureFlags =
      static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionShaderFeatureFlag::TEXT_GRADIENT) |
      static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionShaderFeatureFlag::TEXT_GRADIENT_MIXED) |
      (styleTextureEnabled ? static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionShaderFeatureFlag::STYLE_TEXTURE) : 0u) |
      (overlayGradientEnabled ? static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionShaderFeatureFlag::TEXT_GRADIENT_OVERLAY) : 0u) |
      (isOverlayStyle ? static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionShaderFeatureFlag::OVERLAY_STYLE) : 0u);
    DALI_TEST_EQUALS(result.requiredResourceFlags, expectedResourceFlags, TEST_LOCATION);
    DALI_TEST_EQUALS(result.shaderFeatureFlags, expectedShaderFeatureFlags, TEST_LOCATION);
  }
  else
  {
    DALI_TEST_EQUALS(result.requiredResourceFlags,
                     static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionResourceFlag::TEXT_TEXTURE),
                     TEST_LOCATION);
    DALI_TEST_EQUALS(result.shaderFeatureFlags,
                     static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionShaderFeatureFlag::NONE),
                     TEST_LOCATION);
  }
}

uint32_t ResourceFlag(TextInternal::GradientMarquee::CompositionResourceFlag flag)
{
  return static_cast<uint32_t>(flag);
}

uint32_t ShaderFeatureFlag(TextInternal::GradientMarquee::CompositionShaderFeatureFlag flag)
{
  return static_cast<uint32_t>(flag);
}

void ExpectMarqueeSimpleStyleCompositionResult(bool baseGradientEnabled,
                                               bool overlayGradientEnabled,
                                               bool hasMultipleTextColors,
                                               bool containsColorGlyph,
                                               bool isOverlayStyle,
                                               bool embossEnabled,
                                               bool cutoutEnabled,
                                               bool expectedSupported,
                                               TextInternal::GradientMarquee::CompositionUnsupportedReason expectedReason,
                                               uint32_t expectedResourceFlags,
                                               uint32_t expectedShaderFeatureFlags)
{
  TextInternal::GradientMarquee::CompositionPolicy policy;
  policy.hasMultipleTextColors  = hasMultipleTextColors;
  policy.containsColorGlyph     = containsColorGlyph;
  policy.styleTextureEnabled    = true;
  policy.isOverlayStyle         = isOverlayStyle;
  policy.embossEnabled          = embossEnabled;
  policy.cutoutEnabled          = cutoutEnabled;
  policy.baseGradientEnabled    = baseGradientEnabled;
  policy.overlayGradientEnabled = overlayGradientEnabled;

  const TextInternal::GradientMarquee::CompositionResult result =
    TextInternal::GradientMarquee::GetCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, expectedSupported, TEST_LOCATION);
  DALI_TEST_EQUALS(result.unsupportedReason == expectedReason, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.requiredResourceFlags, expectedResourceFlags, TEST_LOCATION);
  DALI_TEST_EQUALS(result.shaderFeatureFlags, expectedShaderFeatureFlags, TEST_LOCATION);
}

UiText::MarqueeBuilder::SimpleStyleContentRequest CreateSimpleStyleContentRequest(bool baseGradientEnabled,
                                                                                  bool overlayGradientEnabled)
{
  UiText::MarqueeBuilder::SimpleStyleContentRequest request;
  request.fillPixelData  = CreatePixelData(2u, 2u, Pixel::RGBA8888);
  request.stylePixelData = CreatePixelData(2u, 2u, Pixel::RGBA8888);
  request.sampler        = Sampler::New();
  request.verifiedSize   = Size(2.0f, 2.0f);

  request.gradientState.baseRenderable         = baseGradientEnabled;
  request.gradientState.overlayRenderable      = overlayGradientEnabled;
  request.gradientState.baseStyle              = CreateLinearGradientStyle();
  request.gradientState.overlayStyle           = CreateLinearGradientStyle(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  request.gradientState.baseStyleRenderable    = baseGradientEnabled;
  request.gradientState.overlayStyleRenderable = overlayGradientEnabled;

  request.compositionPolicy.styleTextureEnabled    = true;
  request.compositionPolicy.baseGradientEnabled    = baseGradientEnabled;
  request.compositionPolicy.overlayGradientEnabled = overlayGradientEnabled;

  if(baseGradientEnabled)
  {
    request.baseBoundsResolved        = true;
    request.baseBounds.bounds         = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    request.baseBounds.coordinateSize = Vector2(2.0f, 2.0f);
  }

  if(overlayGradientEnabled)
  {
    request.overlayBoundsResolved        = true;
    request.overlayBounds.bounds         = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    request.overlayBounds.coordinateSize = Vector2(2.0f, 2.0f);
    request.overlayMode                  = Dali::Ui::Text::GradientOverlayMode::SCREEN;
  }

  return request;
}

UiText::MarqueeBuilder::SimpleGradientContentRequest CreateSimpleGradientContentRequest(bool baseGradientEnabled,
                                                                                       bool overlayGradientEnabled)
{
  UiText::MarqueeBuilder::SimpleGradientContentRequest request;
  request.gradientState.baseRenderable         = baseGradientEnabled;
  request.gradientState.overlayRenderable      = overlayGradientEnabled;
  request.gradientState.baseStyle              = CreateLinearGradientStyle();
  request.gradientState.overlayStyle           = CreateLinearGradientStyle(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  request.gradientState.baseStyleRenderable    = baseGradientEnabled;
  request.gradientState.overlayStyleRenderable = overlayGradientEnabled;
  request.overlayMode                          = Dali::Ui::Text::GradientOverlayMode::SCREEN;

  if(baseGradientEnabled)
  {
    request.baseBoundsResolved        = true;
    request.baseBounds.bounds         = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    request.baseBounds.coordinateSize = Vector2(2.0f, 2.0f);
  }

  if(overlayGradientEnabled)
  {
    request.overlayBoundsResolved        = true;
    request.overlayBounds.bounds         = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    request.overlayBounds.coordinateSize = Vector2(2.0f, 2.0f);
  }

  return request;
}

UiText::MarqueeBuilder::MixedGradientContentRequest CreateMixedGradientContentRequest(bool overlayGradientEnabled,
                                                                                     bool styleTextureEnabled)
{
  UiText::MarqueeBuilder::MixedGradientContentRequest request;
  request.preservedPixelData = CreatePixelData(2u, 2u, Pixel::RGBA8888);
  request.maskPixelData      = CreatePixelData(2u, 2u, Pixel::L8);
  request.sampler            = Sampler::New();
  request.verifiedSize       = Size(2.0f, 2.0f);

  request.gradientState.baseRenderable         = true;
  request.gradientState.overlayRenderable      = overlayGradientEnabled;
  request.gradientState.baseStyle              = CreateLinearGradientStyle();
  request.gradientState.overlayStyle           = CreateLinearGradientStyle(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  request.gradientState.baseStyleRenderable    = true;
  request.gradientState.overlayStyleRenderable = overlayGradientEnabled;

  request.compositionPolicy.hasMultipleTextColors  = true;
  request.compositionPolicy.styleTextureEnabled    = styleTextureEnabled;
  request.compositionPolicy.baseGradientEnabled    = true;
  request.compositionPolicy.overlayGradientEnabled = overlayGradientEnabled;

  request.baseBounds.bounds         = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  request.baseBounds.coordinateSize = Vector2(2.0f, 2.0f);

  if(overlayGradientEnabled)
  {
    request.overlayBoundsResolved        = true;
    request.overlayBounds.bounds         = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    request.overlayBounds.coordinateSize = Vector2(2.0f, 2.0f);
    request.overlayMode                  = Dali::Ui::Text::GradientOverlayMode::SCREEN;
  }

  if(styleTextureEnabled)
  {
    request.styleTextureEnabled = true;
    request.stylePixelData      = CreatePixelData(2u, 2u, Pixel::RGBA8888);
  }

  return request;
}

TextFeature::FeatureBuilder BuildAsyncFeature(bool textGradientSupported,
                                              bool hasMultipleTextColors = false,
                                              bool containsColorGlyph    = false,
                                              bool styleEnabled          = false,
                                              bool isOverlayStyle        = false,
                                              bool isEmbossEnabled       = false,
                                              bool textGradientMixedSupported = false)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableMultiColor(hasMultipleTextColors)
    .EnableEmoji(containsColorGlyph)
    .EnableStyle(styleEnabled)
    .EnableOverlay(isOverlayStyle)
    .EnableEmboss(isEmbossEnabled)
    .EnableTextGradient(textGradientSupported)
    .EnableTextGradientMixed(textGradientMixedSupported);
  return builder;
}

TextFeature::FeatureBuilder BuildSeparatedStyleFeature(bool textGradientSupported,
                                                       bool textGradientMixedSupported,
                                                       bool hasMultipleTextColors,
                                                       bool containsColorGlyph,
                                                       bool legacyStyleEnabled,
                                                       bool isOverlayStyle = false,
                                                       bool isEmbossEnabled = false)
{
  const bool featureStyleEnabled =
    (textGradientSupported || textGradientMixedSupported) ? false : legacyStyleEnabled;

  TextFeature::FeatureBuilder builder;
  builder.EnableMultiColor(hasMultipleTextColors)
    .EnableEmoji(containsColorGlyph)
    .EnableStyle(featureStyleEnabled)
    .EnableOverlay(isOverlayStyle)
    .EnableEmboss(isEmbossEnabled)
    .EnableTextGradient(textGradientSupported)
    .EnableTextGradientMixed(textGradientMixedSupported);
  return builder;
}

} // namespace

void utc_dali_text_gradient_shader_composition_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_gradient_shader_composition_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextGradientShaderCompositionDisabledFeatureUnchangedP(void)
{
  TextFeature::FeatureBuilder builder;

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientOverlay(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.empty(), true, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  ExpectNoTextGradientOverlayDefine(fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientOverlayShaderFeatureDisabledP(void)
{
  TextFeature::FeatureBuilder builder;

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradientOverlay(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT, TEST_LOCATION);
  ExpectNoTextGradientOverlayDefine(fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientOverlayShaderFeatureOnlyP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientOverlay(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientOverlay(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT_OVERLAY, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  ExpectTextGradientOverlayDefine(fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientOverlayShaderFeatureWithBaseGradientP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradient(true);
  builder.EnableTextGradientOverlay(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientOverlay(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT_AND_TEXT_GRADIENT_OVERLAY, TEST_LOCATION);
  ExpectTextGradientDefine(fragmentPrefix);
  ExpectTextGradientOverlayDefine(fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientOverlayShaderFeatureWithMixedBaseGradientP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);
  builder.EnableTextGradientOverlay(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientOverlay(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED_WITH_TEXT_GRADIENT_OVERLAY, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  ExpectTextGradientOverlayDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientOverlayShaderFeatureWithMultiColorAndEmojiP(void)
{
  TextFeature::FeatureBuilder multiColorBuilder;
  multiColorBuilder.EnableMultiColor(true);
  multiColorBuilder.EnableTextGradientOverlay(true);

  std::string multiColorPrefix = GetFragmentPrefix(multiColorBuilder);

  DALI_TEST_EQUALS(multiColorBuilder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_TEXT_GRADIENT_OVERLAY, TEST_LOCATION);
  ExpectNoTextGradientDefine(multiColorPrefix);
  ExpectTextGradientOverlayDefine(multiColorPrefix);
  DALI_TEST_EQUALS(multiColorPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") != std::string::npos, true, TEST_LOCATION);

  TextFeature::FeatureBuilder emojiBuilder;
  emojiBuilder.EnableEmoji(true);
  emojiBuilder.EnableTextGradientOverlay(true);

  std::string emojiPrefix = GetFragmentPrefix(emojiBuilder);

  DALI_TEST_EQUALS(emojiBuilder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI_AND_TEXT_GRADIENT_OVERLAY, TEST_LOCATION);
  ExpectNoTextGradientDefine(emojiPrefix);
  ExpectTextGradientOverlayDefine(emojiPrefix);
  DALI_TEST_EQUALS(emojiPrefix.find("#define IS_REQUIRED_EMOJI\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientOverlayShaderUniformSymbolsP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientOverlay(true);

  const std::string fragmentPrefix = GetFragmentPrefix(builder);
  const std::string fragmentShader = fragmentPrefix + std::string(SHADER_TEXT_VISUAL_SHADER_FRAG);

  ExpectTextGradientOverlayDefine(fragmentPrefix);
  DALI_TEST_CHECK(fragmentShader.find("UNIFORM sampler2D sGradientOverlayLookup;") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("uTextGradientOverlayType") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("uTextGradientOverlayStartPosition") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("uTextGradientOverlayEndPosition") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("uTextGradientOverlayRadialCenter") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("uTextGradientOverlayRadialScale") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("uTextGradientOverlayConicCenter") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("uTextGradientOverlayConicScale") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("uTextGradientOverlayConicStartAngle") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("uTextGradientOverlayStartOffset") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("uTextGradientOverlayBounds") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("uTextGradientOverlayMode") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("ApplyTextGradientOverlay(textColor, vTexCoord)") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("mediump float glyphAlpha = baseFill.a") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("if(glyphAlpha <= 0.000001)") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("return baseFill;") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("mediump vec3 baseRgb = baseFill.rgb / max(glyphAlpha, 0.000001)") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("return vec4(blendedRgb * glyphAlpha, glyphAlpha)") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("result.rgb = overlayColor.rgb * overlayColor.a + baseFill.rgb * (1.0 - overlayColor.a)") == std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("result.a = baseFill.a") == std::string::npos);
  END_TEST;
}

int UtcDaliTextRevealForegroundShaderCompositionP(void)
{
  auto ExpectRevealVariant = [](TextFeature::FeatureBuilder builder)
  {
    builder.EnableTextReveal(true);
    const std::string prefix = GetFragmentPrefix(builder);
    DALI_TEST_CHECK(prefix.find(TEXT_REVEAL_DEFINE) != std::string::npos);
  };

  // Every resolved foreground family must be able to select the reveal
  // variant: solid, multi-color, emoji, base/mixed gradients, overlay gradient,
  // separated underlay/overlay styles, and bevel.
  ExpectRevealVariant(TextFeature::FeatureBuilder());

  TextFeature::FeatureBuilder multiColor;
  multiColor.EnableMultiColor(true);
  ExpectRevealVariant(multiColor);

  TextFeature::FeatureBuilder emoji;
  emoji.EnableEmoji(true);
  ExpectRevealVariant(emoji);

  TextFeature::FeatureBuilder gradientAndEmoji;
  gradientAndEmoji.EnableEmoji(true).EnableTextGradientMixed(true);
  ExpectRevealVariant(gradientAndEmoji);

  TextFeature::FeatureBuilder baseGradient;
  baseGradient.EnableTextGradient(true);
  ExpectRevealVariant(baseGradient);

  TextFeature::FeatureBuilder mixedGradient;
  mixedGradient.EnableMultiColor(true).EnableTextGradientMixed(true);
  ExpectRevealVariant(mixedGradient);

  TextFeature::FeatureBuilder overlayGradient;
  overlayGradient.EnableTextGradientOverlay(true);
  ExpectRevealVariant(overlayGradient);

  TextFeature::FeatureBuilder baseAndOverlayGradient;
  baseAndOverlayGradient.EnableTextGradient(true).EnableTextGradientOverlay(true);
  ExpectRevealVariant(baseAndOverlayGradient);

  TextFeature::FeatureBuilder mixedAndOverlayGradient;
  mixedAndOverlayGradient.EnableMultiColor(true).EnableEmoji(true).EnableTextGradientMixed(true).EnableTextGradientOverlay(true);
  ExpectRevealVariant(mixedAndOverlayGradient);

  TextFeature::FeatureBuilder underlayStyle;
  underlayStyle.EnableStyle(true);
  ExpectRevealVariant(underlayStyle);

  TextFeature::FeatureBuilder overlayStyle;
  overlayStyle.EnableOverlay(true);
  ExpectRevealVariant(overlayStyle);

  TextFeature::FeatureBuilder bevel;
  bevel.EnableEmboss(true);
  ExpectRevealVariant(bevel);

  TextFeature::FeatureBuilder revealOnly;
  revealOnly.EnableTextReveal(true);
  const std::string revealOnlyPrefix = GetFragmentPrefix(revealOnly);
  DALI_TEST_CHECK(revealOnlyPrefix.find(TEXT_REVEAL_DEFINE) != std::string::npos);
  DALI_TEST_CHECK(revealOnlyPrefix.find("#define IS_REQUIRED_EMBOSS\n") == std::string::npos);

  TextFeature::FeatureBuilder revealBevel;
  revealBevel.EnableTextReveal(true).EnableEmboss(true);
  const std::string revealBevelPrefix = GetFragmentPrefix(revealBevel);
  DALI_TEST_CHECK(revealBevelPrefix.find(TEXT_REVEAL_DEFINE) != std::string::npos);
  DALI_TEST_CHECK(revealBevelPrefix.find("#define IS_REQUIRED_EMBOSS\n") != std::string::npos);

  const std::string fragmentShader(SHADER_TEXT_VISUAL_SHADER_FRAG);
  const size_t      gradientOverlay = fragmentShader.find("textColor = ApplyTextGradientOverlay(textColor, vTexCoord)");
  const size_t      revealMultiply  = fragmentShader.find("textColor *= ResolveTextRevealOpacity(vTexCoord)");
  const size_t      styleComposition = fragmentShader.find("gl_FragColor = uColor * (");
  DALI_TEST_CHECK(gradientOverlay != std::string::npos);
  DALI_TEST_CHECK(revealMultiply != std::string::npos);
  DALI_TEST_CHECK(styleComposition != std::string::npos);
  DALI_TEST_CHECK(gradientOverlay < revealMultiply);
  DALI_TEST_CHECK(revealMultiply < styleComposition);
  DALI_TEST_CHECK(fragmentShader.find("+ styleTexture * (1.0 - textColor.a)") > revealMultiply);
  DALI_TEST_CHECK(fragmentShader.find(") * (1.0 - overlayStyleTexture.a) + overlayStyleTexture") > revealMultiply);

  // Bevel samples displaced foreground coverage, so each component uses the
  // timing metadata at the matching source coordinate before composition.
  DALI_TEST_CHECK(fragmentShader.find("textAlpha *= ResolveTextRevealOpacity(vTexCoord)") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("light *= ResolveTextRevealOpacity(clamp(vTexCoord - offset, 0.0, 1.0))") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("shadow *= ResolveTextRevealOpacity(clamp(vTexCoord + offset, 0.0, 1.0))") != std::string::npos);
  DALI_TEST_EQUALS(CountOccurrences(fragmentShader, "textColor *= ResolveTextRevealOpacity(vTexCoord)"), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(CountOccurrences(fragmentShader, "textAlpha *= ResolveTextRevealOpacity(vTexCoord)"), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(CountOccurrences(fragmentShader, "light *= ResolveTextRevealOpacity(clamp(vTexCoord - offset, 0.0, 1.0))"), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(CountOccurrences(fragmentShader, "shadow *= ResolveTextRevealOpacity(clamp(vTexCoord + offset, 0.0, 1.0))"), 2u, TEST_LOCATION);

  // The preprocessor combinations above fix the runtime metadata sampling
  // count: Reveal off compiles no resolver call, Reveal without emboss keeps
  // the single textColor call, and Reveal with emboss selects one of the two
  // foreground branches with center/light/shadow calls while excluding the
  // final textColor call through TEXT_REVEAL_RESOLVED_IN_EMBOSS.
  END_TEST;
}

int UtcDaliTextRevealShaderCacheSeparatesEffectiveVariantsP(void)
{
  UiTestApplication application;
  UiInternal::VisualFactoryCache     cache(false);
  UiInternal::TextVisualShaderFactory factory;
  std::vector<Shader>                  revealShaders;

  auto AddDistinctRevealVariant = [&](TextFeature::FeatureBuilder builder)
  {
    builder.EnableTextReveal(true);
    Shader shader       = factory.GetShader(cache, builder);
    Shader cachedShader = factory.GetShader(cache, builder);
    DALI_TEST_CHECK(shader);
    DALI_TEST_CHECK(shader == cachedShader);
    for(const Shader& previous : revealShaders)
    {
      DALI_TEST_CHECK(shader != previous);
    }
    revealShaders.push_back(shader);
  };

  AddDistinctRevealVariant(TextFeature::FeatureBuilder());

  TextFeature::FeatureBuilder multiColor;
  multiColor.EnableMultiColor(true);
  AddDistinctRevealVariant(multiColor);

  TextFeature::FeatureBuilder emoji;
  emoji.EnableEmoji(true);
  AddDistinctRevealVariant(emoji);

  TextFeature::FeatureBuilder baseGradient;
  baseGradient.EnableTextGradient(true);
  AddDistinctRevealVariant(baseGradient);

  TextFeature::FeatureBuilder mixedGradient;
  mixedGradient.EnableMultiColor(true).EnableTextGradientMixed(true);
  AddDistinctRevealVariant(mixedGradient);

  TextFeature::FeatureBuilder overlayGradient;
  overlayGradient.EnableTextGradientOverlay(true);
  AddDistinctRevealVariant(overlayGradient);

  TextFeature::FeatureBuilder baseAndOverlayGradient;
  baseAndOverlayGradient.EnableTextGradient(true).EnableTextGradientOverlay(true);
  AddDistinctRevealVariant(baseAndOverlayGradient);

  TextFeature::FeatureBuilder mixedAndOverlayGradient;
  mixedAndOverlayGradient.EnableMultiColor(true).EnableTextGradientMixed(true).EnableTextGradientOverlay(true);
  AddDistinctRevealVariant(mixedAndOverlayGradient);

  TextFeature::FeatureBuilder style;
  style.EnableStyle(true);
  AddDistinctRevealVariant(style);

  TextFeature::FeatureBuilder overlayStyle;
  overlayStyle.EnableOverlay(true);
  AddDistinctRevealVariant(overlayStyle);

  TextFeature::FeatureBuilder styleAndOverlay;
  styleAndOverlay.EnableStyle(true).EnableOverlay(true);
  AddDistinctRevealVariant(styleAndOverlay);

  TextFeature::FeatureBuilder emboss;
  emboss.EnableEmboss(true);
  AddDistinctRevealVariant(emboss);

  TextFeature::FeatureBuilder ordinary;
  Shader ordinaryShader = factory.GetShader(cache, ordinary);
  DALI_TEST_CHECK(ordinaryShader);
  DALI_TEST_CHECK(ordinaryShader != revealShaders.front());
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionSimpleFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradient(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT, TEST_LOCATION);
  ExpectTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") == std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_EMOJI\n") == std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") == std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionEnabledThenDisabledP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradient(true);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT, TEST_LOCATION);

  builder.EnableTextGradient(false);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionDisabledThenEnabledP(void)
{
  TextFeature::FeatureBuilder builder;

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT, TEST_LOCATION);

  builder.EnableTextGradient(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT, TEST_LOCATION);
  ExpectTextGradientDefine(fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMultiColorFallbackP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradient(true);
  builder.EnableMultiColor(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMultiColorThenSimpleP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradient(true);
  builder.EnableMultiColor(true);

  std::string multiColorPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT, TEST_LOCATION);
  ExpectNoTextGradientDefine(multiColorPrefix);

  builder.EnableMultiColor(false);

  std::string simplePrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT, TEST_LOCATION);
  ExpectTextGradientDefine(simplePrefix);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionColorGlyphFallbackP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradient(true);
  builder.EnableEmoji(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_EMOJI\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMixedMultiColorFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);
  std::string fragmentShader = fragmentPrefix + std::string(SHADER_TEXT_VISUAL_SHADER_FRAG);
  const auto  maskSampler    = fragmentShader.find("UNIFORM sampler2D sTextGradientMask;");
  const auto  lookupSampler  = fragmentShader.find("UNIFORM sampler2D sGradientLookup;");
  const auto  maskLookup     = fragmentShader.find("TEXTURE(sTextGradientMask, vTexCoord).r");
  const auto  gradientLookup = fragmentShader.find("TEXTURE(sGradientLookup, vec2(gradientPosition + uTextGradientStartOffset, 0.5))");
  const auto  alphaOver      = fragmentShader.find("gradientFill + preservedColor * (1.0 - gradientFill.a)");

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  DALI_TEST_CHECK(maskSampler != std::string::npos);
  DALI_TEST_CHECK(lookupSampler != std::string::npos);
  DALI_TEST_CHECK(maskSampler < lookupSampler);
  DALI_TEST_CHECK(maskLookup != std::string::npos);
  DALI_TEST_CHECK(gradientLookup != std::string::npos);
  DALI_TEST_CHECK(alphaOver != std::string::npos);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMixedEmojiFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableEmoji(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_EMOJI\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionDisabledColorOnlyStyledTextKeepsStyleFeatureP(void)
{
  TextFeature::FeatureBuilder builder =
    BuildSeparatedStyleFeature(false, false, true, false, true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(),
                   UiInternal::VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE,
                   TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionColorOnlyStyledTextMixedKeepsStyleTextureOffP(void)
{
  TextFeature::FeatureBuilder builder =
    BuildSeparatedStyleFeature(false, true, true, false, false);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(),
                   UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED,
                   TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") == std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientVisualMixedStyleShaderFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);
  builder.EnableStyle(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);
  std::string fragmentShader = fragmentPrefix + std::string(SHADER_TEXT_VISUAL_SHADER_FRAG);
  const auto  mixedFill      = fragmentShader.find("textColor = gradientFill + preservedColor * (1.0 - gradientFill.a);");
  const auto  styleApply     = fragmentShader.find("+ styleTexture * (1.0 - textColor.a)");

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED_WITH_STYLE, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_CHECK(fragmentShader.find("UNIFORM sampler2D sTextGradientMask;") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("UNIFORM sampler2D sStyle;") != std::string::npos);
  DALI_TEST_CHECK(mixedFill != std::string::npos);
  DALI_TEST_CHECK(styleApply != std::string::npos);
  DALI_TEST_CHECK(mixedFill < styleApply);
  END_TEST;
}

int UtcDaliTextGradientVisualMixedStyleOverlayShaderFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);
  builder.EnableStyle(true);
  builder.EnableTextGradientOverlay(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);
  std::string fragmentShader = fragmentPrefix + std::string(SHADER_TEXT_VISUAL_SHADER_FRAG);
  const auto  mixedFill      = fragmentShader.find("textColor = gradientFill + preservedColor * (1.0 - gradientFill.a);");
  const auto  overlayApply   = fragmentShader.find("textColor = ApplyTextGradientOverlay(textColor, vTexCoord);");
  const auto  styleApply     = fragmentShader.find("+ styleTexture * (1.0 - textColor.a)");

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientOverlay(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED_WITH_STYLE_AND_TEXT_GRADIENT_OVERLAY, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  ExpectTextGradientOverlayDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_CHECK(mixedFill != std::string::npos);
  DALI_TEST_CHECK(overlayApply != std::string::npos);
  DALI_TEST_CHECK(styleApply != std::string::npos);
  DALI_TEST_CHECK(mixedFill < overlayApply);
  DALI_TEST_CHECK(overlayApply < styleApply);
  END_TEST;
}

int UtcDaliTextGradientVisualMixedOverlayStyleShaderFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);
  builder.EnableOverlay(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);
  std::string fragmentShader = fragmentPrefix + std::string(SHADER_TEXT_VISUAL_SHADER_FRAG);
  const auto  mixedFill      = fragmentShader.find("textColor = gradientFill + preservedColor * (1.0 - gradientFill.a);");
  const auto  overlayStyle   = fragmentShader.find(") * (1.0 - overlayStyleTexture.a) + overlayStyleTexture");

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED_WITH_OVERLAY, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_OVERLAY\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_CHECK(fragmentShader.find("UNIFORM sampler2D sOverlayStyle;") != std::string::npos);
  DALI_TEST_CHECK(mixedFill != std::string::npos);
  DALI_TEST_CHECK(overlayStyle != std::string::npos);
  DALI_TEST_CHECK(mixedFill < overlayStyle);
  END_TEST;
}

int UtcDaliTextGradientVisualMixedStyleOverlayStyleShaderFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);
  builder.EnableStyle(true);
  builder.EnableOverlay(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);
  std::string fragmentShader = fragmentPrefix + std::string(SHADER_TEXT_VISUAL_SHADER_FRAG);
  const auto  mixedFill      = fragmentShader.find("textColor = gradientFill + preservedColor * (1.0 - gradientFill.a);");
  const auto  styleApply     = fragmentShader.find("+ styleTexture * (1.0 - textColor.a)");
  const auto  overlayStyle   = fragmentShader.find(") * (1.0 - overlayStyleTexture.a) + overlayStyleTexture");

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED_WITH_STYLE_AND_OVERLAY, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_OVERLAY\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_CHECK(mixedFill != std::string::npos);
  DALI_TEST_CHECK(styleApply != std::string::npos);
  DALI_TEST_CHECK(overlayStyle != std::string::npos);
  DALI_TEST_CHECK(mixedFill < styleApply);
  DALI_TEST_CHECK(styleApply < overlayStyle);
  END_TEST;
}

int UtcDaliTextGradientVisualMixedOverlayStyleAndGradientOverlayShaderFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);
  builder.EnableOverlay(true);
  builder.EnableTextGradientOverlay(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);
  std::string fragmentShader = fragmentPrefix + std::string(SHADER_TEXT_VISUAL_SHADER_FRAG);
  const auto  mixedFill      = fragmentShader.find("textColor = gradientFill + preservedColor * (1.0 - gradientFill.a);");
  const auto  overlayApply   = fragmentShader.find("textColor = ApplyTextGradientOverlay(textColor, vTexCoord);");
  const auto  overlayStyle   = fragmentShader.find(") * (1.0 - overlayStyleTexture.a) + overlayStyleTexture");

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientOverlay(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED_WITH_OVERLAY_AND_TEXT_GRADIENT_OVERLAY, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  ExpectTextGradientOverlayDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_OVERLAY\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_CHECK(mixedFill != std::string::npos);
  DALI_TEST_CHECK(overlayApply != std::string::npos);
  DALI_TEST_CHECK(overlayStyle != std::string::npos);
  DALI_TEST_CHECK(mixedFill < overlayApply);
  DALI_TEST_CHECK(overlayApply < overlayStyle);
  END_TEST;
}

int UtcDaliTextGradientVisualMixedStyleOverlayStyleAndGradientOverlayShaderFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);
  builder.EnableStyle(true);
  builder.EnableOverlay(true);
  builder.EnableTextGradientOverlay(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);
  std::string fragmentShader = fragmentPrefix + std::string(SHADER_TEXT_VISUAL_SHADER_FRAG);
  const auto  mixedFill      = fragmentShader.find("textColor = gradientFill + preservedColor * (1.0 - gradientFill.a);");
  const auto  overlayApply   = fragmentShader.find("textColor = ApplyTextGradientOverlay(textColor, vTexCoord);");
  const auto  styleApply     = fragmentShader.find("+ styleTexture * (1.0 - textColor.a)");
  const auto  overlayStyle   = fragmentShader.find(") * (1.0 - overlayStyleTexture.a) + overlayStyleTexture");

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientOverlay(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED_WITH_STYLE_AND_OVERLAY_AND_TEXT_GRADIENT_OVERLAY, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  ExpectTextGradientOverlayDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_OVERLAY\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_CHECK(mixedFill != std::string::npos);
  DALI_TEST_CHECK(overlayApply != std::string::npos);
  DALI_TEST_CHECK(styleApply != std::string::npos);
  DALI_TEST_CHECK(overlayStyle != std::string::npos);
  DALI_TEST_CHECK(mixedFill < overlayApply);
  DALI_TEST_CHECK(overlayApply < styleApply);
  DALI_TEST_CHECK(styleApply < overlayStyle);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMixedEmbossFallbackP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);
  builder.EnableEmboss(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_EMBOSS, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_EMBOSS\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionSimpleThenMixedP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradient(true);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), false, TEST_LOCATION);

  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMixedThenSimpleP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);

  builder.EnableMultiColor(false);
  builder.EnableTextGradient(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT, TEST_LOCATION);
  ExpectTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find(TEXT_GRADIENT_MIXED_DEFINE) == std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMixedThenDisabledP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);

  builder.EnableTextGradientMixed(false);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionSimpleStyleFeatureP(void)
{
  TextFeature::FeatureBuilder styleBuilder;
  styleBuilder.EnableTextGradient(true);
  styleBuilder.EnableStyle(true);

  std::string stylePrefix = GetFragmentPrefix(styleBuilder);

  DALI_TEST_EQUALS(styleBuilder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(styleBuilder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT_AND_STYLE, TEST_LOCATION);
  ExpectTextGradientDefine(stylePrefix);
  DALI_TEST_EQUALS(stylePrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(stylePrefix.find(TEXT_GRADIENT_MIXED_DEFINE) == std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionSimpleOverlayFeatureP(void)
{
  TextFeature::FeatureBuilder overlayBuilder;
  overlayBuilder.EnableTextGradient(true);
  overlayBuilder.EnableOverlay(true);

  std::string overlayPrefix = GetFragmentPrefix(overlayBuilder);

  DALI_TEST_EQUALS(overlayBuilder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(overlayBuilder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT_AND_OVERLAY, TEST_LOCATION);
  ExpectTextGradientDefine(overlayPrefix);
  DALI_TEST_EQUALS(overlayPrefix.find("#define IS_REQUIRED_OVERLAY\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(overlayPrefix.find(TEXT_GRADIENT_MIXED_DEFINE) == std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionSimpleStyleAndOverlayFeatureP(void)
{
  TextFeature::FeatureBuilder overlayBuilder;
  overlayBuilder.EnableTextGradient(true);
  overlayBuilder.EnableStyle(true);
  overlayBuilder.EnableOverlay(true);

  std::string overlayPrefix = GetFragmentPrefix(overlayBuilder);

  DALI_TEST_EQUALS(overlayBuilder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(overlayBuilder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT_AND_STYLE_AND_OVERLAY, TEST_LOCATION);
  ExpectTextGradientDefine(overlayPrefix);
  DALI_TEST_EQUALS(overlayPrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(overlayPrefix.find("#define IS_REQUIRED_OVERLAY\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(overlayPrefix.find(TEXT_GRADIENT_MIXED_DEFINE) == std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionSimpleEmbossFallbackP(void)
{
  TextFeature::FeatureBuilder embossBuilder;
  embossBuilder.EnableTextGradient(true);
  embossBuilder.EnableEmboss(true);

  std::string embossPrefix = GetFragmentPrefix(embossBuilder);

  DALI_TEST_EQUALS(embossBuilder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(embossBuilder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMBOSS, TEST_LOCATION);
  ExpectNoTextGradientDefine(embossPrefix);
  DALI_TEST_EQUALS(embossPrefix.find("#define IS_REQUIRED_EMBOSS\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionSkipsSingleColorFallbackP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradient(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);
  std::string fragmentShader = fragmentPrefix + std::string(SHADER_TEXT_VISUAL_SHADER_FRAG);

  ExpectTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentShader.find("#elif defined(IS_REQUIRED_TEXT_GRADIENT)\n  mediump float textTexture = TEXTURE(sTexture, vTexCoord).r;") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionBoundsUniformP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradient(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);
  std::string fragmentShader = fragmentPrefix + std::string(SHADER_TEXT_VISUAL_SHADER_FRAG);

  ExpectTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientBounds") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientType") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientRadialCenter") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientRadialScale") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientConicCenter") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientConicScale") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientConicStartAngle") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("textGradientCoord") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("EvaluateTextGradientPosition") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("TEXT_GRADIENT_TYPE_RADIAL") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("TEXT_GRADIENT_TYPE_CONIC") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("TEXT_GRADIENT_INV_TWO_PI") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientType > 1.5") == std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("length((textGradientCoord - uTextGradientRadialCenter) * uTextGradientRadialScale)") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("atan(conicVector.y, conicVector.x) - uTextGradientConicStartAngle") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionObjectBoundingBoxPositionP(void)
{
  const Vector4 bounds(0.25f, 0.25f, 0.5f, 0.5f);
  const Vector2 textureSize(200.0f, 100.0f);

  ExpectPosition(TextInternal::ResolveGradientPosition(Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX,
                                                       Vector2(-0.5f, -0.5f),
                                                       bounds,
                                                       textureSize),
                 Vector2(0.0f, 0.0f));
  ExpectPosition(TextInternal::ResolveGradientPosition(Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX,
                                                       Vector2(0.5f, 0.5f),
                                                       bounds,
                                                       textureSize),
                 Vector2(1.0f, 1.0f));
  ExpectPosition(TextInternal::ResolveGradientPosition(Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX,
                                                       Vector2(0.0f, 0.0f),
                                                       bounds,
                                                       textureSize),
                 Vector2(0.5f, 0.5f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionUserSpacePositionP(void)
{
  const Vector4 bounds(0.25f, 0.25f, 0.5f, 0.5f);
  const Vector2 textureSize(200.0f, 100.0f);

  ExpectPosition(TextInternal::ResolveGradientPosition(Dali::Ui::Gradient::Units::USER_SPACE,
                                                       Vector2(50.0f, 25.0f),
                                                       bounds,
                                                       textureSize),
                 Vector2(0.5f, 0.5f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionUserSpaceRadialScaleP(void)
{
  const Vector4 bounds(0.25f, 0.25f, 0.5f, 0.5f);
  const Vector2 textureSize(200.0f, 100.0f);

  ExpectPosition(TextInternal::ResolveGradientPosition(Dali::Ui::Gradient::Units::USER_SPACE,
                                                       Vector2(50.0f, 25.0f),
                                                       bounds,
                                                       textureSize),
                 Vector2(0.5f, 0.5f));
  ExpectPosition(TextInternal::ResolveRadialGradientScale(Dali::Ui::Gradient::Units::USER_SPACE,
                                                          25.0f,
                                                          bounds,
                                                          textureSize),
                 Vector2(4.0f, 2.0f));
  ExpectPosition(TextInternal::ResolveRadialGradientScale(Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX,
                                                          0.5f,
                                                          bounds,
                                                          textureSize),
                 Vector2(2.0f, 2.0f));
  ExpectPosition(TextInternal::ResolveConicGradientScale(Dali::Ui::Gradient::Units::USER_SPACE,
                                                         bounds,
                                                         textureSize),
                 Vector2(100.0f, 50.0f));
  ExpectPosition(TextInternal::ResolveConicGradientScale(Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX,
                                                         bounds,
                                                         textureSize),
                 Vector2::ONE);
  END_TEST;
}

int UtcDaliRenderDataLinearP(void)
{
  TextInternal::Gradient::Style style;
  style.enabled      = true;
  style.type         = Dali::Ui::Gradient::Type::LINEAR;
  style.units        = Dali::Ui::Gradient::Units::USER_SPACE;
  style.linearStart  = Vector2(25.0f, 20.0f);
  style.linearEnd    = Vector2(75.0f, 40.0f);
  style.startOffset  = 0.25f;
  style.stops.PushBack({0.0f, Color::RED});
  style.stops.PushBack({1.0f, Color::BLUE});

  const Vector4 bounds(0.25f, 0.2f, 0.5f, 0.4f);
  const Vector2 coordinateSize(200.0f, 100.0f);

  const TextInternal::Gradient::RenderData renderData =
    TextInternal::Gradient::ResolveRenderData(style, bounds, coordinateSize);

  DALI_TEST_EQUALS(renderData.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(renderData.type, Dali::Ui::Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_EQUALS(renderData.startOffset, 0.25f, EPSILON, TEST_LOCATION);
  ExpectBounds(renderData.bounds, bounds);
  ExpectPosition(renderData.startPosition,
                 TextInternal::ResolveGradientPosition(style.units, style.linearStart, bounds, coordinateSize));
  ExpectPosition(renderData.endPosition,
                 TextInternal::ResolveGradientPosition(style.units, style.linearEnd, bounds, coordinateSize));
  END_TEST;
}

int UtcDaliRenderDataRadialP(void)
{
  TextInternal::Gradient::Style style;
  style.enabled      = true;
  style.type         = Dali::Ui::Gradient::Type::RADIAL;
  style.units        = Dali::Ui::Gradient::Units::USER_SPACE;
  style.radialCenter = Vector2(50.0f, 20.0f);
  style.radialRadius = 25.0f;
  style.startOffset  = 0.35f;
  style.stops.PushBack({0.0f, Color::RED});
  style.stops.PushBack({1.0f, Color::BLUE});

  const Vector4 bounds(0.25f, 0.0f, 0.5f, 1.0f);
  const Vector2 coordinateSize(200.0f, 40.0f);

  const TextInternal::Gradient::RenderData renderData =
    TextInternal::Gradient::ResolveRenderData(style, bounds, coordinateSize);

  DALI_TEST_EQUALS(renderData.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(renderData.type, Dali::Ui::Gradient::Type::RADIAL, TEST_LOCATION);
  DALI_TEST_EQUALS(renderData.startOffset, 0.35f, EPSILON, TEST_LOCATION);
  ExpectBounds(renderData.bounds, bounds);
  ExpectPosition(renderData.radialCenter,
                 TextInternal::ResolveGradientPosition(style.units, style.radialCenter, bounds, coordinateSize));
  ExpectPosition(renderData.radialScale,
                 TextInternal::ResolveRadialGradientScale(style.units, style.radialRadius, bounds, coordinateSize));
  END_TEST;
}

int UtcDaliRenderDataConicP(void)
{
  TextInternal::Gradient::Style style;
  style.enabled         = true;
  style.type            = Dali::Ui::Gradient::Type::CONIC;
  style.units           = Dali::Ui::Gradient::Units::USER_SPACE;
  style.conicCenter     = Vector2(50.0f, 20.0f);
  style.conicStartAngle = Radian(0.75f);
  style.startOffset     = 0.45f;
  style.stops.PushBack({0.0f, Color::RED});
  style.stops.PushBack({1.0f, Color::BLUE});

  const Vector4 bounds(0.25f, 0.0f, 0.5f, 1.0f);
  const Vector2 coordinateSize(200.0f, 40.0f);

  const TextInternal::Gradient::RenderData renderData =
    TextInternal::Gradient::ResolveRenderData(style, bounds, coordinateSize);

  DALI_TEST_EQUALS(renderData.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(renderData.type, Dali::Ui::Gradient::Type::CONIC, TEST_LOCATION);
  DALI_TEST_EQUALS(renderData.startOffset, 0.45f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderData.conicStartAngle, 0.75f, EPSILON, TEST_LOCATION);
  ExpectBounds(renderData.bounds, bounds);
  ExpectPosition(renderData.conicCenter,
                 TextInternal::ResolveGradientPosition(style.units, style.conicCenter, bounds, coordinateSize));
  ExpectPosition(renderData.conicScale,
                 TextInternal::ResolveConicGradientScale(style.units, bounds, coordinateSize));
  END_TEST;
}

int UtcDaliRenderDataUnsupportedP(void)
{
  const Vector4 bounds(0.25f, 0.0f, 0.5f, 1.0f);
  const Vector2 coordinateSize(200.0f, 40.0f);

  TextInternal::Gradient::Style noneStyle;
  TextInternal::Gradient::RenderData renderData =
    TextInternal::Gradient::ResolveRenderData(noneStyle, bounds, coordinateSize);

  DALI_TEST_EQUALS(renderData.enabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(renderData.type, Dali::Ui::Gradient::Type::NONE, TEST_LOCATION);

  TextInternal::Gradient::Style oneStopStyle;
  oneStopStyle.enabled     = true;
  oneStopStyle.type        = Dali::Ui::Gradient::Type::LINEAR;
  oneStopStyle.linearStart = Vector2(-0.5f, 0.0f);
  oneStopStyle.linearEnd   = Vector2(0.5f, 0.0f);
  oneStopStyle.stops.PushBack({0.0f, Color::RED});

  renderData = TextInternal::Gradient::ResolveRenderData(oneStopStyle, bounds, coordinateSize);

  DALI_TEST_EQUALS(renderData.enabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(renderData.type, Dali::Ui::Gradient::Type::NONE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderDataBoundsOriginIndependenceP(void)
{
  const Vector2 coordinateSize(300.0f, 120.0f);
  const Vector4 firstBounds(0.0f, 0.0f, 0.5f, 0.75f);
  const Vector4 scrolledBounds(0.2f, -0.15f, 0.5f, 0.75f);

  TextInternal::Gradient::Style linear;
  linear.enabled     = true;
  linear.type        = Dali::Ui::Gradient::Type::LINEAR;
  linear.units       = Dali::Ui::Gradient::Units::USER_SPACE;
  linear.linearStart = Vector2(20.0f, 10.0f);
  linear.linearEnd   = Vector2(140.0f, 70.0f);
  linear.stops.PushBack({0.0f, Color::RED});
  linear.stops.PushBack({1.0f, Color::BLUE});

  const auto linearFirst = TextInternal::Gradient::ResolveRenderData(linear, firstBounds, coordinateSize);
  const auto linearScrolled = TextInternal::Gradient::ResolveRenderData(linear, scrolledBounds, coordinateSize);
  ExpectPosition(linearScrolled.startPosition, linearFirst.startPosition);
  ExpectPosition(linearScrolled.endPosition, linearFirst.endPosition);
  ExpectBounds(linearScrolled.bounds, scrolledBounds);

  TextInternal::Gradient::Style radial = linear;
  radial.type         = Dali::Ui::Gradient::Type::RADIAL;
  radial.radialCenter = Vector2(75.0f, 45.0f);
  radial.radialRadius = 30.0f;
  const auto radialFirst = TextInternal::Gradient::ResolveRenderData(radial, firstBounds, coordinateSize);
  const auto radialScrolled = TextInternal::Gradient::ResolveRenderData(radial, scrolledBounds, coordinateSize);
  ExpectPosition(radialScrolled.radialCenter, radialFirst.radialCenter);
  ExpectPosition(radialScrolled.radialScale, radialFirst.radialScale);
  ExpectBounds(radialScrolled.bounds, scrolledBounds);

  TextInternal::Gradient::Style conic = linear;
  conic.type            = Dali::Ui::Gradient::Type::CONIC;
  conic.conicCenter     = Vector2(60.0f, 35.0f);
  conic.conicStartAngle = Radian(0.6f);
  const auto conicFirst = TextInternal::Gradient::ResolveRenderData(conic, firstBounds, coordinateSize);
  const auto conicScrolled = TextInternal::Gradient::ResolveRenderData(conic, scrolledBounds, coordinateSize);
  ExpectPosition(conicScrolled.conicCenter, conicFirst.conicCenter);
  ExpectPosition(conicScrolled.conicScale, conicFirst.conicScale);
  DALI_TEST_EQUALS(conicScrolled.conicStartAngle, conicFirst.conicStartAngle, EPSILON, TEST_LOCATION);
  ExpectBounds(conicScrolled.bounds, scrolledBounds);

  linear.units = Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX;
  linear.linearStart = Vector2(-0.5f, 0.0f);
  linear.linearEnd   = Vector2(0.5f, 0.0f);
  const auto objectFirst = TextInternal::Gradient::ResolveRenderData(linear, firstBounds, coordinateSize);
  const auto objectScrolled = TextInternal::Gradient::ResolveRenderData(linear, scrolledBounds, coordinateSize);
  ExpectPosition(objectScrolled.startPosition, objectFirst.startPosition);
  ExpectPosition(objectScrolled.endPosition, objectFirst.endPosition);
  ExpectBounds(objectScrolled.bounds, scrolledBounds);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMarqueeHorizontalFeatureP(void)
{
  std::string vertexShader   = std::string(TEXT_GRADIENT_DEFINE) + std::string(SHADER_TEXT_SCROLLER_SHADER_VERT);
  std::string fragmentShader = std::string(TEXT_GRADIENT_DEFINE) + std::string(SHADER_TEXT_SCROLLER_SHADER_FRAG);

  DALI_TEST_EQUALS(vertexShader.find("OUTPUT highp vec2 vTextGradientCoord;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(vertexShader.find("vTextGradientCoord = aPosition + vec2(0.5);") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("UNIFORM sampler2D sGradientLookup;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("INPUT highp vec2 vTextGradientCoord;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientBounds") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientRadialCenter") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientRadialScale") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientConicCenter") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientConicScale") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientConicStartAngle") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("TEXT_GRADIENT_TYPE_CONIC") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("(vTextGradientCoord - uTextGradientBounds.xy)") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("fract(") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("TEXTURE(sGradientLookup, vec2(gradientPosition + uTextGradientStartOffset, 0.5))") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMarqueeVerticalFeatureP(void)
{
  std::string vertexShader   = std::string(TEXT_GRADIENT_DEFINE) + std::string(SHADER_TEXT_SCROLLER_VERTICAL_SHADER_VERT);
  std::string fragmentShader = std::string(TEXT_GRADIENT_DEFINE) + std::string(SHADER_TEXT_SCROLLER_VERTICAL_SHADER_FRAG);

  DALI_TEST_EQUALS(vertexShader.find("OUTPUT highp vec2 vTextGradientCoord;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(vertexShader.find("vTextGradientCoord = aPosition + vec2(0.5);") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("UNIFORM sampler2D sGradientLookup;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("INPUT highp vec2 vTextGradientCoord;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientBounds") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientRadialCenter") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientRadialScale") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientConicCenter") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientConicScale") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientConicStartAngle") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("TEXT_GRADIENT_TYPE_CONIC") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("(vTextGradientCoord - uTextGradientBounds.xy)") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("fract(") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("TEXTURE(sGradientLookup, vec2(gradientPosition + uTextGradientStartOffset, 0.5))") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMarqueeMixedFeatureP(void)
{
  std::string horizontalFragmentShader = std::string(TEXT_GRADIENT_DEFINE) +
                                         std::string(TEXT_GRADIENT_MIXED_DEFINE) +
                                         std::string(SHADER_TEXT_SCROLLER_SHADER_FRAG);
  std::string verticalFragmentShader = std::string(TEXT_GRADIENT_DEFINE) +
                                       std::string(TEXT_GRADIENT_MIXED_DEFINE) +
                                       std::string(SHADER_TEXT_SCROLLER_VERTICAL_SHADER_FRAG);

  DALI_TEST_EQUALS(horizontalFragmentShader.find("UNIFORM sampler2D sTextGradientMask;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(horizontalFragmentShader.find("mediump vec4 preservedColor = textTexture;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(horizontalFragmentShader.find("TEXTURE(sTextGradientMask, vTexCoord).r") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(horizontalFragmentShader.find("gradientFill + preservedColor * (1.0 - gradientFill.a)") != std::string::npos, true, TEST_LOCATION);

  DALI_TEST_EQUALS(verticalFragmentShader.find("UNIFORM sampler2D sTextGradientMask;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(verticalFragmentShader.find("mediump vec4 preservedColor = textTexture;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(verticalFragmentShader.find("TEXTURE(sTextGradientMask, vTexCoord).r") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(verticalFragmentShader.find("gradientFill + preservedColor * (1.0 - gradientFill.a)") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMarqueeMixedOverlayFeatureP(void)
{
  std::string fragmentShader = std::string(TEXT_GRADIENT_DEFINE) +
                               std::string(TEXT_GRADIENT_MIXED_DEFINE) +
                               std::string(TEXT_GRADIENT_OVERLAY_DEFINE) +
                               std::string(SHADER_TEXT_SCROLLER_SHADER_FRAG);

  const auto mixedFill = fragmentShader.find("textTexture = gradientFill + preservedColor * (1.0 - gradientFill.a);");
  const auto overlayApply = fragmentShader.find("textTexture = ApplyTextGradientOverlay(textTexture);");

  DALI_TEST_CHECK(fragmentShader.find("UNIFORM sampler2D sTextGradientMask;") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("UNIFORM sampler2D sGradientLookup;") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("UNIFORM sampler2D sGradientOverlayLookup;") != std::string::npos);
  DALI_TEST_CHECK(mixedFill != std::string::npos);
  DALI_TEST_CHECK(overlayApply != std::string::npos);
  DALI_TEST_CHECK(mixedFill < overlayApply);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMarqueeStyleFeatureP(void)
{
  std::string horizontalFragmentShader = std::string(TEXT_GRADIENT_DEFINE) +
                                         std::string(TEXT_GRADIENT_OVERLAY_DEFINE) +
                                         std::string(TEXT_STYLE_DEFINE) +
                                         std::string(SHADER_TEXT_SCROLLER_SHADER_FRAG);
  std::string verticalFragmentShader = std::string(TEXT_GRADIENT_DEFINE) +
                                       std::string(TEXT_GRADIENT_OVERLAY_DEFINE) +
                                       std::string(TEXT_STYLE_DEFINE) +
                                       std::string(SHADER_TEXT_SCROLLER_VERTICAL_SHADER_FRAG);

  const auto horizontalOverlayApply = horizontalFragmentShader.find("textTexture = ApplyTextGradientOverlay(textTexture);");
  const auto horizontalStyleApply   = horizontalFragmentShader.find("textTexture = textTexture + styleTexture * (1.0 - textTexture.a);");
  const auto verticalOverlayApply   = verticalFragmentShader.find("textTexture = ApplyTextGradientOverlay(textTexture);");
  const auto verticalStyleApply     = verticalFragmentShader.find("textTexture = textTexture + styleTexture * (1.0 - textTexture.a);");

  DALI_TEST_CHECK(horizontalFragmentShader.find("UNIFORM sampler2D sStyle;") != std::string::npos);
  DALI_TEST_CHECK(horizontalFragmentShader.find("mediump vec4 styleTexture = TEXTURE(sStyle, vTexCoord);") != std::string::npos);
  DALI_TEST_CHECK(horizontalOverlayApply != std::string::npos);
  DALI_TEST_CHECK(horizontalStyleApply != std::string::npos);
  DALI_TEST_CHECK(horizontalOverlayApply < horizontalStyleApply);

  DALI_TEST_CHECK(verticalFragmentShader.find("UNIFORM sampler2D sStyle;") != std::string::npos);
  DALI_TEST_CHECK(verticalFragmentShader.find("mediump vec4 styleTexture = TEXTURE(sStyle, vTexCoord);") != std::string::npos);
  DALI_TEST_CHECK(verticalOverlayApply != std::string::npos);
  DALI_TEST_CHECK(verticalStyleApply != std::string::npos);
  DALI_TEST_CHECK(verticalOverlayApply < verticalStyleApply);
  END_TEST;
}

int UtcDaliTextScrollerOverlayStyleShaderFeatureP(void)
{
  std::string horizontalFragmentShader = std::string(TEXT_OVERLAY_STYLE_DEFINE) +
                                         std::string(SHADER_TEXT_SCROLLER_SHADER_FRAG);
  std::string verticalFragmentShader = std::string(TEXT_OVERLAY_STYLE_DEFINE) +
                                       std::string(SHADER_TEXT_SCROLLER_VERTICAL_SHADER_FRAG);

  const auto horizontalFillSample   = horizontalFragmentShader.find("mediump vec4 textTexture = TEXTURE( sTexture, vTexCoord );");
  const auto horizontalOverlayStyle = horizontalFragmentShader.find("textTexture = textTexture * (1.0 - overlayStyleTexture.a) + overlayStyleTexture;");
  const auto horizontalOutput       = horizontalFragmentShader.find("gl_FragColor = textTexture * uColor;");
  const auto verticalFillSample     = verticalFragmentShader.find("mediump vec4 textTexture = TEXTURE( sTexture, vTexCoord );");
  const auto verticalOverlayStyle   = verticalFragmentShader.find("textTexture = textTexture * (1.0 - overlayStyleTexture.a) + overlayStyleTexture;");
  const auto verticalOutput         = verticalFragmentShader.find("gl_FragColor = textTexture * uColor;");

  DALI_TEST_CHECK(horizontalFragmentShader.find("UNIFORM sampler2D sOverlayStyle;") != std::string::npos);
  DALI_TEST_CHECK(horizontalFragmentShader.find("mediump vec4 overlayStyleTexture = TEXTURE(sOverlayStyle, vTexCoord);") != std::string::npos);
  DALI_TEST_CHECK(horizontalFillSample != std::string::npos);
  DALI_TEST_CHECK(horizontalOverlayStyle != std::string::npos);
  DALI_TEST_CHECK(horizontalOutput != std::string::npos);
  DALI_TEST_CHECK(horizontalFillSample < horizontalOverlayStyle);
  DALI_TEST_CHECK(horizontalOverlayStyle < horizontalOutput);

  DALI_TEST_CHECK(verticalFragmentShader.find("UNIFORM sampler2D sOverlayStyle;") != std::string::npos);
  DALI_TEST_CHECK(verticalFragmentShader.find("mediump vec4 overlayStyleTexture = TEXTURE(sOverlayStyle, vTexCoord);") != std::string::npos);
  DALI_TEST_CHECK(verticalFillSample != std::string::npos);
  DALI_TEST_CHECK(verticalOverlayStyle != std::string::npos);
  DALI_TEST_CHECK(verticalOutput != std::string::npos);
  DALI_TEST_CHECK(verticalFillSample < verticalOverlayStyle);
  DALI_TEST_CHECK(verticalOverlayStyle < verticalOutput);
  END_TEST;
}

int UtcDaliTextScrollerStyleThenOverlayStyleShaderFeatureP(void)
{
  std::string fragmentShader = std::string(TEXT_STYLE_DEFINE) +
                               std::string(TEXT_OVERLAY_STYLE_DEFINE) +
                               std::string(SHADER_TEXT_SCROLLER_SHADER_FRAG);

  const auto styleApply        = fragmentShader.find("textTexture = textTexture + styleTexture * (1.0 - textTexture.a);");
  const auto overlayStyleApply = fragmentShader.find("textTexture = textTexture * (1.0 - overlayStyleTexture.a) + overlayStyleTexture;");

  DALI_TEST_CHECK(styleApply != std::string::npos);
  DALI_TEST_CHECK(overlayStyleApply != std::string::npos);
  DALI_TEST_CHECK(styleApply < overlayStyleApply);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMarqueeMixedStyleFeatureP(void)
{
  std::string horizontalFragmentShader = std::string(TEXT_GRADIENT_DEFINE) +
                                         std::string(TEXT_GRADIENT_MIXED_DEFINE) +
                                         std::string(TEXT_GRADIENT_OVERLAY_DEFINE) +
                                         std::string(TEXT_STYLE_DEFINE) +
                                         std::string(SHADER_TEXT_SCROLLER_SHADER_FRAG);
  std::string verticalFragmentShader = std::string(TEXT_GRADIENT_DEFINE) +
                                       std::string(TEXT_GRADIENT_MIXED_DEFINE) +
                                       std::string(TEXT_GRADIENT_OVERLAY_DEFINE) +
                                       std::string(TEXT_STYLE_DEFINE) +
                                       std::string(SHADER_TEXT_SCROLLER_VERTICAL_SHADER_FRAG);

  const auto horizontalMixedFill    = horizontalFragmentShader.find("textTexture = gradientFill + preservedColor * (1.0 - gradientFill.a);");
  const auto horizontalOverlayApply = horizontalFragmentShader.find("textTexture = ApplyTextGradientOverlay(textTexture);");
  const auto horizontalStyleApply   = horizontalFragmentShader.find("textTexture = textTexture + styleTexture * (1.0 - textTexture.a);");
  const auto verticalMixedFill      = verticalFragmentShader.find("textTexture = gradientFill + preservedColor * (1.0 - gradientFill.a);");
  const auto verticalOverlayApply   = verticalFragmentShader.find("textTexture = ApplyTextGradientOverlay(textTexture);");
  const auto verticalStyleApply     = verticalFragmentShader.find("textTexture = textTexture + styleTexture * (1.0 - textTexture.a);");

  DALI_TEST_CHECK(horizontalFragmentShader.find("UNIFORM sampler2D sTextGradientMask;") != std::string::npos);
  DALI_TEST_CHECK(horizontalFragmentShader.find("UNIFORM sampler2D sStyle;") != std::string::npos);
  DALI_TEST_CHECK(horizontalMixedFill != std::string::npos);
  DALI_TEST_CHECK(horizontalOverlayApply != std::string::npos);
  DALI_TEST_CHECK(horizontalStyleApply != std::string::npos);
  DALI_TEST_CHECK(horizontalMixedFill < horizontalOverlayApply);
  DALI_TEST_CHECK(horizontalOverlayApply < horizontalStyleApply);

  DALI_TEST_CHECK(verticalFragmentShader.find("UNIFORM sampler2D sTextGradientMask;") != std::string::npos);
  DALI_TEST_CHECK(verticalFragmentShader.find("UNIFORM sampler2D sStyle;") != std::string::npos);
  DALI_TEST_CHECK(verticalMixedFill != std::string::npos);
  DALI_TEST_CHECK(verticalOverlayApply != std::string::npos);
  DALI_TEST_CHECK(verticalStyleApply != std::string::npos);
  DALI_TEST_CHECK(verticalMixedFill < verticalOverlayApply);
  DALI_TEST_CHECK(verticalOverlayApply < verticalStyleApply);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMarqueeHorizontalOverlayFeatureP(void)
{
  std::string vertexShader   = std::string(TEXT_GRADIENT_OVERLAY_DEFINE) + std::string(SHADER_TEXT_SCROLLER_SHADER_VERT);
  std::string fragmentShader = std::string(TEXT_GRADIENT_OVERLAY_DEFINE) + std::string(SHADER_TEXT_SCROLLER_SHADER_FRAG);

  DALI_TEST_EQUALS(vertexShader.find("OUTPUT highp vec2 vTextGradientCoord;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(vertexShader.find("vTextGradientCoord = aPosition + vec2(0.5);") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("UNIFORM sampler2D sGradientOverlayLookup;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("INPUT highp vec2 vTextGradientCoord;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayBounds") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayMode") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayRadialCenter") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayRadialScale") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayConicCenter") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayConicScale") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayConicStartAngle") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("(vTextGradientCoord - uTextGradientOverlayBounds.xy)") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("TEXTURE(sGradientOverlayLookup, vec2(gradientPosition + uTextGradientOverlayStartOffset, 0.5))") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("baseFill.rgb / max(glyphAlpha, 0.000001)") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("vec4(blendedRgb * glyphAlpha, glyphAlpha)") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMarqueeVerticalOverlayFeatureP(void)
{
  std::string vertexShader   = std::string(TEXT_GRADIENT_OVERLAY_DEFINE) + std::string(SHADER_TEXT_SCROLLER_VERTICAL_SHADER_VERT);
  std::string fragmentShader = std::string(TEXT_GRADIENT_OVERLAY_DEFINE) + std::string(SHADER_TEXT_SCROLLER_VERTICAL_SHADER_FRAG);

  DALI_TEST_EQUALS(vertexShader.find("OUTPUT highp vec2 vTextGradientCoord;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(vertexShader.find("vTextGradientCoord = aPosition + vec2(0.5);") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("UNIFORM sampler2D sGradientOverlayLookup;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("INPUT highp vec2 vTextGradientCoord;") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayBounds") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayMode") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayRadialCenter") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayRadialScale") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayConicCenter") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayConicScale") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("uTextGradientOverlayConicStartAngle") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("(vTextGradientCoord - uTextGradientOverlayBounds.xy)") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("TEXTURE(sGradientOverlayLookup, vec2(gradientPosition + uTextGradientOverlayStartOffset, 0.5))") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("baseFill.rgb / max(glyphAlpha, 0.000001)") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("vec4(blendedRgb * glyphAlpha, glyphAlpha)") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionCalculateContentBoundsStartP(void)
{
  UiText::LineRun line;
  line.width           = 40.0f;
  line.alignmentOffset = 0.0f;

  const Vector4 bounds = TextInternal::CalculateGradientContentBounds(Vector2(100.0f, 50.0f),
                                                                      Vector2(40.0f, 20.0f),
                                                                      &line,
                                                                      1u,
                                                                      UiText::Alignment::START);

  ExpectBounds(bounds, Vector4(0.0f, 0.0f, 0.4f, 0.4f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionCalculateContentBoundsCenterP(void)
{
  UiText::LineRun line;
  line.width           = 40.0f;
  line.alignmentOffset = 30.0f;

  const Vector4 bounds = TextInternal::CalculateGradientContentBounds(Vector2(100.0f, 50.0f),
                                                                      Vector2(40.0f, 20.0f),
                                                                      &line,
                                                                      1u,
                                                                      UiText::Alignment::CENTER);

  ExpectBounds(bounds, Vector4(0.3f, 0.3f, 0.4f, 0.4f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionCalculateContentBoundsEndP(void)
{
  UiText::LineRun line;
  line.width           = 40.0f;
  line.alignmentOffset = 60.0f;

  const Vector4 bounds = TextInternal::CalculateGradientContentBounds(Vector2(100.0f, 50.0f),
                                                                      Vector2(40.0f, 20.0f),
                                                                      &line,
                                                                      1u,
                                                                      UiText::Alignment::END);

  ExpectBounds(bounds, Vector4(0.6f, 0.6f, 0.4f, 0.4f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAtlasVariantP(void)
{
  const std::string vertexShader(SHADER_TEXT_ATLAS_GRADIENT_SHADER_VERT);
  const std::string fragmentShader(SHADER_TEXT_ATLAS_L8_GRADIENT_SHADER_FRAG);

  DALI_TEST_CHECK(vertexShader.find("INPUT highp float aGradientFill;") != std::string::npos);
  DALI_TEST_CHECK(vertexShader.find("aPosition + 0.5 * uTextGradientLayoutSize") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("UNIFORM sampler2D sGradientLookup;") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("if(vGradientFill > 0.5)") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("gradient.rgb / gradient.a") != std::string::npos);
  DALI_TEST_CHECK(fragmentShader.find("vColor.a * textColorAnimatable.a * gradient.a * coverage") != std::string::npos);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAtlasContentBoundsUnionP(void)
{
  UiText::LineRun lines[2] = {MakeLine(80.0f, 20.0f, 15.0f, -5.0f),
                              MakeLine(40.0f, 50.0f, 12.0f, -4.0f)};

  const Vector4 bounds = TextInternal::CalculateAtlasGradientContentBounds(
    Vector2(200.0f, 100.0f), lines, 2u, 10.0f);

  ExpectBounds(bounds, Vector4(0.05f, 0.0f, 0.4f, 0.36f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAtlasContentBoundsEmptyLinePolicyP(void)
{
  UiText::LineRun firstEmpty[2] = {MakeLine(0.0f, 150.0f, 9.0f, -3.0f),
                                   MakeLine(40.0f, 20.0f, 9.0f, -3.0f)};
  ExpectBounds(TextInternal::CalculateAtlasGradientContentBounds(
                 Vector2(200.0f, 60.0f), firstEmpty, 2u, 0.0f),
               Vector4(0.1f, 0.0f, 0.2f, 0.4f));

  UiText::LineRun trailingEmpty[2] = {MakeLine(40.0f, 20.0f, 9.0f, -3.0f),
                                      MakeLine(0.0f, -70.0f, 9.0f, -3.0f)};
  ExpectBounds(TextInternal::CalculateAtlasGradientContentBounds(
                 Vector2(200.0f, 60.0f), trailingEmpty, 2u, 0.0f),
               Vector4(0.1f, 0.0f, 0.2f, 0.4f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAtlasContentBoundsAllEmptyP(void)
{
  UiText::LineRun newlineOnly[2] = {MakeLine(0.0f, 100.0f, 8.0f, -2.0f),
                                    MakeLine(0.0f, -50.0f, 9.0f, -3.0f)};
  const float safeWidth = Math::MACHINE_EPSILON_1000 / 200.0f;
  ExpectBounds(TextInternal::CalculateAtlasGradientContentBounds(
                 Vector2(200.0f, 50.0f), newlineOnly, 2u, 0.0f),
               Vector4(0.0f, 0.0f, safeWidth, 22.0f / 50.0f));

  UiText::LineRun zeroMetric = MakeLine(0.0f, 90.0f, 0.0f, 0.0f);
  const float safeHeight = Math::MACHINE_EPSILON_1000 / 50.0f;
  ExpectBounds(TextInternal::CalculateAtlasGradientContentBounds(
                 Vector2(200.0f, 50.0f), &zeroMetric, 1u, 0.0f),
               Vector4(0.0f, 0.0f, safeWidth, safeHeight));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAtlasContentBoundsWhitespaceAndAlignmentP(void)
{
  UiText::LineRun whitespace = MakeLine(30.0f, 85.0f);
  ExpectBounds(TextInternal::CalculateAtlasGradientContentBounds(
                 Vector2(200.0f, 40.0f), &whitespace, 1u, 5.0f),
               Vector4(0.4f, 0.0f, 0.15f, 0.25f));

  UiText::LineRun centered = MakeLine(60.0f, 70.0f);
  ExpectBounds(TextInternal::CalculateAtlasGradientContentBounds(
                 Vector2(200.0f, 40.0f), &centered, 1u, 10.0f),
               Vector4(0.3f, 0.0f, 0.3f, 0.25f));

  UiText::LineRun rightAligned = MakeLine(40.0f, 160.0f);
  ExpectBounds(TextInternal::CalculateAtlasGradientContentBounds(
                 Vector2(200.0f, 40.0f), &rightAligned, 1u, 0.0f),
               Vector4(0.8f, 0.0f, 0.2f, 0.25f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAtlasContentBoundsNegativeAndDifferentWidthsP(void)
{
  UiText::LineRun lines[3] = {MakeLine(20.0f, 15.0f),
                              MakeLine(60.0f, -10.0f),
                              MakeLine(30.0f, 90.0f)};
  ExpectBounds(TextInternal::CalculateAtlasGradientContentBounds(
                 Vector2(200.0f, 60.0f), lines, 3u, 10.0f),
               Vector4(-0.1f, 0.0f, 0.65f, 0.5f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAtlasContentBoundsHeightRangeAndInvalidLayoutP(void)
{
  UiText::LineRun tall[2] = {MakeLine(40.0f, 0.0f, 15.0f, -5.0f),
                             MakeLine(40.0f, 0.0f, 15.0f, -5.0f)};
  ExpectBounds(TextInternal::CalculateAtlasGradientContentBounds(
                 Vector2(100.0f, 20.0f), tall, 2u, 0.0f),
               Vector4(0.0f, 0.0f, 0.4f, 2.0f));

  UiText::LineRun shortLine = MakeLine(40.0f, 0.0f, 4.0f, -1.0f);
  ExpectBounds(TextInternal::CalculateAtlasGradientContentBounds(
                 Vector2(100.0f, 20.0f), &shortLine, 1u, 0.0f),
               Vector4(0.0f, 0.0f, 0.4f, 0.25f));

  ExpectBounds(TextInternal::CalculateAtlasGradientContentBounds(
                 Vector2::ZERO, &shortLine, 1u, 0.0f),
               Vector4(0.0f, 0.0f, 1.0f, 1.0f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAtlasAndLabelContentBoundsParityP(void)
{
  UiText::LineRun lines[2] = {MakeLine(60.0f, 70.0f, 15.0f, -5.0f),
                              MakeLine(30.0f, 85.0f, 15.0f, -5.0f)};
  const Vector2 size(200.0f, 40.0f);
  const Vector4 labelBounds = TextInternal::CalculateGradientContentBounds(
    size, size, lines, 2u, UiText::Alignment::START);
  const Vector4 atlasBounds = TextInternal::CalculateAtlasGradientContentBounds(
    size, lines, 2u, 0.0f);
  ExpectBounds(atlasBounds, labelBounds);

  END_TEST;
}

int UtcDaliTextGradientShaderCompositionLabelEmptyFallbackP(void)
{
  const Vector2 textureSize(100.0f, 50.0f);
  const Vector2 layoutSize(40.0f, 20.0f);
  const Vector4 fullLayoutFallback(0.0f, 0.0f, 0.4f, 0.4f);

  ExpectBounds(TextInternal::CalculateGradientContentBounds(
                 textureSize, layoutSize, nullptr, 0u, UiText::Alignment::START),
               fullLayoutFallback);

  UiText::LineRun ignored = MakeLine(30.0f, 5.0f);
  ExpectBounds(TextInternal::CalculateGradientContentBounds(
                 textureSize, layoutSize, &ignored, 0u, UiText::Alignment::START),
               fullLayoutFallback);

  UiText::LineRun empty[2] = {MakeLine(0.0f, 100.0f, 15.0f, -5.0f),
                              MakeLine(0.0f, -80.0f, 15.0f, -5.0f)};
  ExpectBounds(TextInternal::CalculateGradientContentBounds(
                 textureSize, layoutSize, empty, 2u, UiText::Alignment::START),
               fullLayoutFallback);

  UiText::LineRun trailingNewline[2] = {MakeLine(30.0f, 5.0f), MakeLine(0.0f, 80.0f)};
  ExpectBounds(TextInternal::CalculateGradientContentBounds(
                 textureSize, layoutSize, trailingNewline, 2u, UiText::Alignment::START),
               Vector4(0.05f, 0.0f, 0.3f, 0.4f));

  const float safeWidth  = Math::MACHINE_EPSILON_1000 / textureSize.width;
  const float safeHeight = Math::MACHINE_EPSILON_1000 / textureSize.height;
  ExpectBounds(TextInternal::CalculateGradientContentBounds(
                 textureSize, Vector2::ZERO, nullptr, 0u, UiText::Alignment::START),
               Vector4(0.0f, 0.0f, safeWidth, safeHeight));
  ExpectBounds(TextInternal::CalculateGradientContentBounds(
                 Vector2::ZERO, layoutSize, nullptr, 0u, UiText::Alignment::START),
               Vector4(0.0f, 0.0f, 1.0f, 1.0f));

  const Vector4 atlasEmpty = TextInternal::CalculateAtlasGradientContentBounds(
    layoutSize, empty, 2u, 0.0f);
  DALI_TEST_EQUALS(atlasEmpty.z, Math::MACHINE_EPSILON_1000 / layoutSize.width, EPSILON, TEST_LOCATION);
  DALI_TEST_CHECK(atlasEmpty.z < fullLayoutFallback.z);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAtlasViewBoundsScrollP(void)
{
  const Vector4 bounds = TextInternal::CalculateGradientViewBounds(
    Vector2(300.0f, 100.0f), Vector2(200.0f, 80.0f), Vector2(12.0f, -10.0f));

  ExpectBounds(bounds, Vector4(-0.04f, 0.1f, 2.0f / 3.0f, 0.8f));
  END_TEST;
}

int UtcDaliTextGradientCalculateMarqueeGradientViewportBoundsHorizontalOverflowP(void)
{
  UiText::LineRun line;
  line.width           = 200.0f;
  line.alignmentOffset = 0.0f;
  line.ascender        = 15.0f;
  line.descender       = -5.0f;
  line.lineSpacing     = 0.0f;

  const Vector4 bounds = TextInternal::CalculateMarqueeGradientViewportBounds(Vector2(100.0f, 40.0f),
                                                                              Vector2(200.0f, 20.0f),
                                                                              &line,
                                                                              1u,
                                                                              UiText::Alignment::START,
                                                                              UiText::Alignment::CENTER);

  ExpectBounds(bounds, Vector4(0.0f, 0.25f, 1.0f, 0.5f));
  END_TEST;
}

int UtcDaliTextGradientCalculateMarqueeGradientViewportBoundsHorizontalShortEndP(void)
{
  UiText::LineRun line;
  line.width           = 40.0f;
  line.alignmentOffset = 0.0f;
  line.ascender        = 15.0f;
  line.descender       = -5.0f;
  line.lineSpacing     = 0.0f;

  const Vector4 bounds = TextInternal::CalculateMarqueeGradientViewportBounds(Vector2(100.0f, 40.0f),
                                                                              Vector2(40.0f, 20.0f),
                                                                              &line,
                                                                              1u,
                                                                              UiText::Alignment::END,
                                                                              UiText::Alignment::END);

  ExpectBounds(bounds, Vector4(0.6f, 0.5f, 0.4f, 0.5f));
  END_TEST;
}

int UtcDaliTextGradientCalculateMarqueeGradientViewportBoundsVerticalOverflowP(void)
{
  UiText::LineRun line;
  line.width           = 50.0f;
  line.alignmentOffset = 0.0f;
  line.ascender        = 160.0f;
  line.descender       = 0.0f;
  line.lineSpacing     = 0.0f;

  const Vector4 bounds = TextInternal::CalculateMarqueeGradientViewportBounds(Vector2(100.0f, 80.0f),
                                                                              Vector2(50.0f, 160.0f),
                                                                              &line,
                                                                              1u,
                                                                              UiText::Alignment::CENTER,
                                                                              UiText::Alignment::START);

  ExpectBounds(bounds, Vector4(0.25f, 0.0f, 0.5f, 1.0f));
  END_TEST;
}

int UtcDaliTextGradientCalculateMarqueeGradientViewportBoundsVerticalShortEndP(void)
{
  UiText::LineRun line;
  line.width           = 50.0f;
  line.alignmentOffset = 0.0f;
  line.ascender        = 15.0f;
  line.descender       = -5.0f;
  line.lineSpacing     = 0.0f;

  const Vector4 bounds = TextInternal::CalculateMarqueeGradientViewportBounds(Vector2(100.0f, 80.0f),
                                                                              Vector2(50.0f, 20.0f),
                                                                              &line,
                                                                              1u,
                                                                              UiText::Alignment::END,
                                                                              UiText::Alignment::END);

  ExpectBounds(bounds, Vector4(0.5f, 0.75f, 0.5f, 0.25f));
  END_TEST;
}

int UtcDaliTextGradientMarqueeCompositionPolicyReasonsP(void)
{
  using Reason = TextInternal::GradientMarquee::CompositionUnsupportedReason;
  using Resource = TextInternal::GradientMarquee::CompositionResourceFlag;
  using ShaderFeature = TextInternal::GradientMarquee::CompositionShaderFeatureFlag;

  ExpectMarqueeCompositionResult(false, false, false, false, false, false, true, Reason::NONE);
  ExpectMarqueeCompositionResult(true, false, false, false, false, false, false, Reason::MULTIPLE_TEXT_COLORS);
  ExpectMarqueeCompositionResult(false, true, false, false, false, false, false, Reason::COLOR_GLYPH);
  ExpectMarqueeCompositionResult(false, false, true, false, false, false, false, Reason::STYLE_TEXTURE);
  ExpectMarqueeCompositionResult(false, false, false, true, false, false, true, Reason::NONE,
                                 ResourceFlag(Resource::TEXT_TEXTURE) |
                                   ResourceFlag(Resource::OVERLAY_STYLE_TEXTURE),
                                 ShaderFeatureFlag(ShaderFeature::OVERLAY_STYLE));
  ExpectMarqueeCompositionResult(false, false, false, false, false, true, false, Reason::CUTOUT_FALLBACK);
  ExpectMarqueeCompositionResult(false, false, false, false, true, false, false, Reason::EMBOSS_SHADER_FEATURE);
  ExpectMarqueeCompositionResult(true, true, true, true, true, true, false, Reason::CUTOUT_FALLBACK);
  END_TEST;
}

int UtcDaliTextGradientMarqueeMixedColorCompositionPolicyP(void)
{
  ExpectMarqueeMixedColorCompositionResult(true, false, true, false, false, false, false, false, true);
  ExpectMarqueeMixedColorCompositionResult(true, false, false, true, false, false, false, false, true);
  ExpectMarqueeMixedColorCompositionResult(true, false, true, true, false, false, false, false, true);
  ExpectMarqueeMixedColorCompositionResult(true, true, true, false, false, false, false, false, true);
  ExpectMarqueeMixedColorCompositionResult(true, true, false, true, false, false, false, false, true);
  ExpectMarqueeMixedColorCompositionResult(true, true, true, true, false, false, false, false, true);

  ExpectMarqueeMixedColorCompositionResult(false, false, true, false, false, false, false, false, false);
  ExpectMarqueeMixedColorCompositionResult(true, false, false, false, false, false, false, false, false);
  ExpectMarqueeMixedColorCompositionResult(true, false, true, false, true, false, false, false, true);
  ExpectMarqueeMixedColorCompositionResult(true, false, true, false, false, true, false, false, true);
  ExpectMarqueeMixedColorCompositionResult(true, false, true, false, false, false, true, false, false);
  ExpectMarqueeMixedColorCompositionResult(true, false, true, false, false, false, false, true, false);
  ExpectMarqueeMixedColorCompositionResult(true, true, true, false, true, false, false, false, true);
  ExpectMarqueeMixedColorCompositionResult(true, true, true, false, false, true, false, false, true);
  ExpectMarqueeMixedColorCompositionResult(true, true, true, false, false, false, true, false, false);
  ExpectMarqueeMixedColorCompositionResult(true, true, true, false, false, false, false, true, false);
  ExpectMarqueeMixedColorCompositionResult(false, false, false, true, false, false, false, false, false);
  ExpectMarqueeMixedColorCompositionResult(true, false, false, true, true, false, false, false, true);
  ExpectMarqueeMixedColorCompositionResult(true, false, false, true, false, true, false, false, true);
  ExpectMarqueeMixedColorCompositionResult(true, false, false, true, false, false, true, false, false);
  ExpectMarqueeMixedColorCompositionResult(true, false, false, true, false, false, false, true, false);
  END_TEST;
}

int UtcDaliTextGradientMarqueeMixedStyleCompositionPolicyP(void)
{
  using Reason = TextInternal::GradientMarquee::CompositionUnsupportedReason;
  using Resource = TextInternal::GradientMarquee::CompositionResourceFlag;
  using ShaderFeature = TextInternal::GradientMarquee::CompositionShaderFeatureFlag;

  TextInternal::GradientMarquee::CompositionPolicy policy;
  policy.styleTextureEnabled   = true;
  policy.hasMultipleTextColors = true;
  policy.baseGradientEnabled   = true;

  TextInternal::GradientMarquee::CompositionResult result =
    TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.unsupportedReason == Reason::NONE, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.requiredResourceFlags,
                   ResourceFlag(Resource::PRESERVED_COLOR_TEXTURE) |
                     ResourceFlag(Resource::GRADIENT_MASK_TEXTURE) |
                     ResourceFlag(Resource::STYLE_TEXTURE) |
                     ResourceFlag(Resource::TEXT_GRADIENT_LOOKUP_TEXTURE),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(result.shaderFeatureFlags,
                   ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT) |
                     ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_MIXED) |
                     ShaderFeatureFlag(ShaderFeature::STYLE_TEXTURE),
                   TEST_LOCATION);

  policy.overlayGradientEnabled = true;
  result = TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.requiredResourceFlags,
                   ResourceFlag(Resource::PRESERVED_COLOR_TEXTURE) |
                     ResourceFlag(Resource::GRADIENT_MASK_TEXTURE) |
                     ResourceFlag(Resource::STYLE_TEXTURE) |
                     ResourceFlag(Resource::TEXT_GRADIENT_LOOKUP_TEXTURE) |
                     ResourceFlag(Resource::TEXT_GRADIENT_OVERLAY_LOOKUP_TEXTURE),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(result.shaderFeatureFlags,
                   ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT) |
                     ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_MIXED) |
                     ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_OVERLAY) |
                     ShaderFeatureFlag(ShaderFeature::STYLE_TEXTURE),
                   TEST_LOCATION);

  policy.baseGradientEnabled    = false;
  policy.overlayGradientEnabled = true;
  result = TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, false, TEST_LOCATION);

  policy.baseGradientEnabled = true;
  policy.isOverlayStyle      = true;
  result = TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.unsupportedReason == Reason::NONE, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.requiredResourceFlags,
                   ResourceFlag(Resource::PRESERVED_COLOR_TEXTURE) |
                     ResourceFlag(Resource::GRADIENT_MASK_TEXTURE) |
                     ResourceFlag(Resource::STYLE_TEXTURE) |
                     ResourceFlag(Resource::TEXT_GRADIENT_LOOKUP_TEXTURE) |
                     ResourceFlag(Resource::TEXT_GRADIENT_OVERLAY_LOOKUP_TEXTURE) |
                     ResourceFlag(Resource::OVERLAY_STYLE_TEXTURE),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(result.shaderFeatureFlags,
                   ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT) |
                     ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_MIXED) |
                     ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_OVERLAY) |
                     ShaderFeatureFlag(ShaderFeature::STYLE_TEXTURE) |
                     ShaderFeatureFlag(ShaderFeature::OVERLAY_STYLE),
                   TEST_LOCATION);

  policy.isOverlayStyle = false;
  policy.cutoutEnabled  = true;
  result = TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, false, TEST_LOCATION);
  DALI_TEST_EQUALS(result.unsupportedReason == Reason::CUTOUT_FALLBACK, true, TEST_LOCATION);

  policy.cutoutEnabled = false;
  policy.embossEnabled = true;
  result = TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, false, TEST_LOCATION);
  DALI_TEST_EQUALS(result.unsupportedReason == Reason::EMBOSS_SHADER_FEATURE, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeMixedOverlayStyleCompositionPolicyP(void)
{
  using Reason = TextInternal::GradientMarquee::CompositionUnsupportedReason;
  using Resource = TextInternal::GradientMarquee::CompositionResourceFlag;
  using ShaderFeature = TextInternal::GradientMarquee::CompositionShaderFeatureFlag;

  TextInternal::GradientMarquee::CompositionPolicy policy;
  policy.hasMultipleTextColors = true;
  policy.baseGradientEnabled   = true;
  policy.isOverlayStyle        = true;

  TextInternal::GradientMarquee::CompositionResult result =
    TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.unsupportedReason == Reason::NONE, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.requiredResourceFlags,
                   ResourceFlag(Resource::PRESERVED_COLOR_TEXTURE) |
                     ResourceFlag(Resource::GRADIENT_MASK_TEXTURE) |
                     ResourceFlag(Resource::TEXT_GRADIENT_LOOKUP_TEXTURE) |
                     ResourceFlag(Resource::OVERLAY_STYLE_TEXTURE),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(result.shaderFeatureFlags,
                   ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT) |
                     ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_MIXED) |
                     ShaderFeatureFlag(ShaderFeature::OVERLAY_STYLE),
                   TEST_LOCATION);

  policy.overlayGradientEnabled = true;
  result = TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.requiredResourceFlags,
                   ResourceFlag(Resource::PRESERVED_COLOR_TEXTURE) |
                     ResourceFlag(Resource::GRADIENT_MASK_TEXTURE) |
                     ResourceFlag(Resource::TEXT_GRADIENT_LOOKUP_TEXTURE) |
                     ResourceFlag(Resource::TEXT_GRADIENT_OVERLAY_LOOKUP_TEXTURE) |
                     ResourceFlag(Resource::OVERLAY_STYLE_TEXTURE),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(result.shaderFeatureFlags,
                   ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT) |
                     ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_MIXED) |
                     ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_OVERLAY) |
                     ShaderFeatureFlag(ShaderFeature::OVERLAY_STYLE),
                   TEST_LOCATION);

  policy.styleTextureEnabled = true;
  result = TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.requiredResourceFlags,
                   ResourceFlag(Resource::PRESERVED_COLOR_TEXTURE) |
                     ResourceFlag(Resource::GRADIENT_MASK_TEXTURE) |
                     ResourceFlag(Resource::STYLE_TEXTURE) |
                     ResourceFlag(Resource::TEXT_GRADIENT_LOOKUP_TEXTURE) |
                     ResourceFlag(Resource::TEXT_GRADIENT_OVERLAY_LOOKUP_TEXTURE) |
                     ResourceFlag(Resource::OVERLAY_STYLE_TEXTURE),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(result.shaderFeatureFlags,
                   ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT) |
                     ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_MIXED) |
                     ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_OVERLAY) |
                     ShaderFeatureFlag(ShaderFeature::STYLE_TEXTURE) |
                     ShaderFeatureFlag(ShaderFeature::OVERLAY_STYLE),
                   TEST_LOCATION);

  policy.baseGradientEnabled    = false;
  policy.overlayGradientEnabled = true;
  result = TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, false, TEST_LOCATION);
  DALI_TEST_EQUALS(result.unsupportedReason == Reason::OVERLAY_STYLE, true, TEST_LOCATION);

  policy.baseGradientEnabled = true;
  policy.cutoutEnabled       = true;
  result = TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, false, TEST_LOCATION);
  DALI_TEST_EQUALS(result.unsupportedReason == Reason::CUTOUT_FALLBACK, true, TEST_LOCATION);

  policy.cutoutEnabled = false;
  policy.embossEnabled = true;
  result = TextInternal::GradientMarquee::GetMixedColorCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, false, TEST_LOCATION);
  DALI_TEST_EQUALS(result.unsupportedReason == Reason::EMBOSS_SHADER_FEATURE, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeSimpleStyleCompositionPolicyP(void)
{
  using Reason = TextInternal::GradientMarquee::CompositionUnsupportedReason;
  using Resource = TextInternal::GradientMarquee::CompositionResourceFlag;
  using ShaderFeature = TextInternal::GradientMarquee::CompositionShaderFeatureFlag;

  const uint32_t textAndStyle = ResourceFlag(Resource::TEXT_TEXTURE) |
                                ResourceFlag(Resource::STYLE_TEXTURE);
  const uint32_t styleShader = ShaderFeatureFlag(ShaderFeature::STYLE_TEXTURE);

  ExpectMarqueeSimpleStyleCompositionResult(true, false, false, false, false, false, false, true, Reason::NONE,
                                            textAndStyle | ResourceFlag(Resource::TEXT_GRADIENT_LOOKUP_TEXTURE),
                                            styleShader | ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT));
  ExpectMarqueeSimpleStyleCompositionResult(false, true, false, false, false, false, false, true, Reason::NONE,
                                            textAndStyle | ResourceFlag(Resource::TEXT_GRADIENT_OVERLAY_LOOKUP_TEXTURE),
                                            styleShader | ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_OVERLAY));
  ExpectMarqueeSimpleStyleCompositionResult(true, true, false, false, false, false, false, true, Reason::NONE,
                                            textAndStyle |
                                              ResourceFlag(Resource::TEXT_GRADIENT_LOOKUP_TEXTURE) |
                                              ResourceFlag(Resource::TEXT_GRADIENT_OVERLAY_LOOKUP_TEXTURE),
                                            styleShader |
                                              ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT) |
                                              ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT_OVERLAY));

  ExpectMarqueeSimpleStyleCompositionResult(false, false, false, false, false, false, false, false, Reason::STYLE_TEXTURE,
                                            ResourceFlag(Resource::TEXT_TEXTURE),
                                            ShaderFeatureFlag(ShaderFeature::NONE));
  ExpectMarqueeSimpleStyleCompositionResult(true, false, true, false, false, false, false, false, Reason::MULTIPLE_TEXT_COLORS,
                                            ResourceFlag(Resource::TEXT_TEXTURE),
                                            ShaderFeatureFlag(ShaderFeature::NONE));
  ExpectMarqueeSimpleStyleCompositionResult(true, false, false, true, false, false, false, false, Reason::COLOR_GLYPH,
                                            ResourceFlag(Resource::TEXT_TEXTURE),
                                            ShaderFeatureFlag(ShaderFeature::NONE));
  ExpectMarqueeSimpleStyleCompositionResult(true, false, false, false, true, false, false, true, Reason::NONE,
                                            textAndStyle |
                                              ResourceFlag(Resource::TEXT_GRADIENT_LOOKUP_TEXTURE) |
                                              ResourceFlag(Resource::OVERLAY_STYLE_TEXTURE),
                                            styleShader |
                                              ShaderFeatureFlag(ShaderFeature::TEXT_GRADIENT) |
                                              ShaderFeatureFlag(ShaderFeature::OVERLAY_STYLE));
  ExpectMarqueeSimpleStyleCompositionResult(true, false, false, false, false, false, true, false, Reason::CUTOUT_FALLBACK,
                                            ResourceFlag(Resource::TEXT_TEXTURE),
                                            ShaderFeatureFlag(ShaderFeature::NONE));
  ExpectMarqueeSimpleStyleCompositionResult(true, false, false, false, false, true, false, false, Reason::EMBOSS_SHADER_FEATURE,
                                            ResourceFlag(Resource::TEXT_TEXTURE),
                                            ShaderFeatureFlag(ShaderFeature::NONE));
  END_TEST;
}

int UtcDaliTextGradientMarqueeOverlayOnlyMixedTargetCompositionPolicyP(void)
{
  TextInternal::GradientMarquee::CompositionPolicy policy;
  policy.overlayGradientEnabled = true;

  policy.hasMultipleTextColors = true;
  TextInternal::GradientMarquee::CompositionResult result =
    TextInternal::GradientMarquee::GetCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.requiredResourceFlags,
                   static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionResourceFlag::TEXT_TEXTURE),
                   TEST_LOCATION);

  policy.hasMultipleTextColors = false;
  policy.containsColorGlyph    = true;
  result = TextInternal::GradientMarquee::GetCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.requiredResourceFlags,
                   static_cast<uint32_t>(TextInternal::GradientMarquee::CompositionResourceFlag::TEXT_TEXTURE),
                   TEST_LOCATION);

  policy.styleTextureEnabled = true;
  result = TextInternal::GradientMarquee::GetCompositionResult(policy);
  DALI_TEST_EQUALS(result.supported, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeAsyncUsesViewportBoundsP(void)
{
  UiText::LineRun line;
  line.width           = 200.0f;
  line.alignmentOffset = 0.0f;
  line.ascender        = 40.0f;
  line.descender       = 0.0f;
  line.lineSpacing     = 0.0f;

  const Vector4 viewportBounds = TextInternal::CalculateMarqueeGradientViewportBounds(Vector2(100.0f, 40.0f),
                                                                                      Vector2(200.0f, 40.0f),
                                                                                      &line,
                                                                                      1u,
                                                                                      UiText::Alignment::START,
                                                                                      UiText::Alignment::START);

  UiText::AsyncTextRenderInfo renderInfo;
  renderInfo.textLogicalBounds                 = Vector4(0.0f, 0.0f, 200.0f / 260.0f, 1.0f);
  renderInfo.textGradientMarqueeViewportBounds = viewportBounds;

  ExpectBounds(renderInfo.textLogicalBounds, Vector4(0.0f, 0.0f, 200.0f / 260.0f, 1.0f));
  ExpectBounds(renderInfo.textGradientMarqueeViewportBounds, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
  END_TEST;
}

int UtcDaliTextGradientMarqueeLookupTexturesUseSequentialSlotsP(void)
{
  TestApplication application;

  TextInternal::Gradient::Style baseStyle;
  baseStyle.enabled     = true;
  baseStyle.type        = Dali::Ui::Gradient::Type::LINEAR;
  baseStyle.linearStart = Vector2::ZERO;
  baseStyle.linearEnd   = Vector2::ONE;
  baseStyle.stops.PushBack({0.0f, Color::RED});
  baseStyle.stops.PushBack({1.0f, Color::BLUE});

  TextInternal::Gradient::Style overlayStyle;
  overlayStyle.enabled     = true;
  overlayStyle.type        = Dali::Ui::Gradient::Type::LINEAR;
  overlayStyle.linearStart = Vector2(-0.5f, 0.0f);
  overlayStyle.linearEnd   = Vector2(0.5f, 0.0f);
  overlayStyle.stops.PushBack({0.0f, Color::TRANSPARENT});
  overlayStyle.stops.PushBack({1.0f, Color::WHITE});

  TextureSet textureSet      = TextureSet::New();
  uint32_t   textureSetIndex = 1u;
  TextInternal::Gradient::AddLookupTexture(textureSet, textureSetIndex, baseStyle);
  TextInternal::Gradient::AddLookupTexture(textureSet, textureSetIndex, overlayStyle);

  DALI_TEST_EQUALS(textureSetIndex, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(textureSet.GetTexture(1u));
  DALI_TEST_CHECK(textureSet.GetTexture(2u));
  END_TEST;
}

int UtcDaliTextGradientMarqueeMixedLookupTextureUsesSlotTwoP(void)
{
  TestApplication application;

  TextInternal::Gradient::Style baseStyle = CreateLinearGradientStyle();

  TextureSet textureSet      = TextureSet::New();
  uint32_t   textureSetIndex = 2u;
  TextInternal::Gradient::AddLookupTexture(textureSet, textureSetIndex, baseStyle);

  DALI_TEST_EQUALS(textureSetIndex, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(textureSet.GetTexture(2u));
  END_TEST;
}

int UtcDaliTextGradientMarqueeMixedOverlayLookupTextureUsesSlotThreeP(void)
{
  TestApplication application;

  UiText::MarqueeBuilder::PreparedContent content;
  content.textureSet = TextureSet::New();

  UiText::MarqueeBuilder::MixedGradientContentRequest request;
  request.preservedPixelData = CreatePixelData(2u, 2u, Pixel::RGBA8888);
  request.maskPixelData      = CreatePixelData(2u, 2u, Pixel::L8);
  request.sampler            = Sampler::New();
  request.verifiedSize       = Size(2.0f, 2.0f);

  request.gradientState.baseRenderable         = true;
  request.gradientState.overlayRenderable      = true;
  request.gradientState.baseStyle              = CreateLinearGradientStyle();
  request.gradientState.overlayStyle           = CreateLinearGradientStyle(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  request.gradientState.baseStyleRenderable    = true;
  request.gradientState.overlayStyleRenderable = true;

  request.compositionPolicy.hasMultipleTextColors  = true;
  request.compositionPolicy.baseGradientEnabled    = true;
  request.compositionPolicy.overlayGradientEnabled = true;

  request.baseBounds.bounds            = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  request.baseBounds.coordinateSize    = Vector2(2.0f, 2.0f);
  request.overlayBoundsResolved        = true;
  request.overlayBounds.bounds         = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  request.overlayBounds.coordinateSize = Vector2(2.0f, 2.0f);
  request.overlayMode                  = Dali::Ui::Text::GradientOverlayMode::SCREEN;

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplyMixedGradientContent(content, request), true, TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(3u));
  DALI_TEST_EQUALS(content.textGradient.mixedTextGradient, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayMode, Dali::Ui::Text::GradientOverlayMode::SCREEN, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerPlainMarqueeCompositionPlanKeepsFastPathP(void)
{
  TestApplication application;

  Sampler sampler = Sampler::New();
  UiText::MarqueeBuilder::PreparedContent content =
    UiText::MarqueeBuilder::CreateTextContent(CreatePixelData(2u, 2u, Pixel::RGBA8888), sampler);

  UiText::MarqueeBuilder::CompositionRequest request;
  request.sampler      = sampler;
  request.verifiedSize = Size(2.0f, 2.0f);

  const UiText::MarqueeBuilder::CompositionPlan plan =
    UiText::MarqueeBuilder::GetCompositionPlan(request);
  DALI_TEST_EQUALS(plan.HasWork(), false, TEST_LOCATION);

  UiText::MarqueeBuilder::PixelDataBundle pixels;
  UiText::MarqueeBuilder::ApplyPreparedComposition(content, request, pixels);

  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(!content.textureSet.GetTexture(1u));
  DALI_TEST_EQUALS(content.textGradient.enabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.mixedTextGradient, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayStyleTextureEnabled, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerPlainOverlayStyleCompositionPlanUsesOverlayOnlyP(void)
{
  TestApplication application;

  Sampler sampler = Sampler::New();
  UiText::MarqueeBuilder::PreparedContent content =
    UiText::MarqueeBuilder::CreateTextContent(CreatePixelData(2u, 2u, Pixel::RGBA8888), sampler);

  UiText::MarqueeBuilder::CompositionRequest request;
  request.sampler                    = sampler;
  request.verifiedSize               = Size(2.0f, 2.0f);
  request.featureInfo.isOverlayStyle = true;

  const UiText::MarqueeBuilder::CompositionPlan plan =
    UiText::MarqueeBuilder::GetCompositionPlan(request);
  DALI_TEST_EQUALS(plan.HasWork(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.needsOverlayStylePixelData, true, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.needsBaseBounds, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.needsOverlayBounds, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.needsPreservedMaskPixelData, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.needsStylePixelData, false, TEST_LOCATION);

  UiText::MarqueeBuilder::PixelDataBundle pixels;
  pixels.overlayStylePixelData = CreatePixelData(2u, 2u, Pixel::RGBA8888);
  UiText::MarqueeBuilder::ApplyPreparedComposition(content, request, pixels);

  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_EQUALS(content.textGradient.enabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.mixedTextGradient, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayStyleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerOverlayStyleTextureSlotP(void)
{
  TestApplication application;

  const Size verifiedSize(2.0f, 2.0f);
  Sampler    sampler = Sampler::New();
  UiText::MarqueeBuilder::PreparedContent content =
    UiText::MarqueeBuilder::CreateTextContent(CreatePixelData(2u, 2u, Pixel::RGBA8888), sampler);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryAppendPlainOverlayStyleContent(content,
                                                                             CreatePixelData(2u, 2u, Pixel::RGBA8888),
                                                                             sampler,
                                                                             verifiedSize),
                   true,
                   TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_EQUALS(content.textGradient.enabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.mixedTextGradient, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayStyleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerGradientOverlayStyleTextureSlotP(void)
{
  TestApplication application;

  const Size verifiedSize(2.0f, 2.0f);
  Sampler    sampler = Sampler::New();
  UiText::MarqueeBuilder::PreparedContent content =
    UiText::MarqueeBuilder::CreateTextContent(CreatePixelData(2u, 2u, Pixel::RGBA8888), sampler);
  const auto request = CreateSimpleGradientContentRequest(true, false);

  UiText::MarqueeBuilder::ApplySimpleGradientContent(content, request);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryAppendSimpleOverlayStyleContent(content,
                                                                              CreatePixelData(2u, 2u, Pixel::RGBA8888),
                                                                              sampler,
                                                                              verifiedSize),
                   true,
                   TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_EQUALS(content.textGradient.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayStyleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerGradientOverlayOverlayStyleTextureSlotP(void)
{
  TestApplication application;

  const Size verifiedSize(2.0f, 2.0f);
  Sampler    sampler = Sampler::New();
  UiText::MarqueeBuilder::PreparedContent content =
    UiText::MarqueeBuilder::CreateTextContent(CreatePixelData(2u, 2u, Pixel::RGBA8888), sampler);
  const auto request = CreateSimpleGradientContentRequest(true, true);

  UiText::MarqueeBuilder::ApplySimpleGradientContent(content, request);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryAppendSimpleOverlayStyleContent(content,
                                                                              CreatePixelData(2u, 2u, Pixel::RGBA8888),
                                                                              sampler,
                                                                              verifiedSize),
                   true,
                   TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(3u));
  DALI_TEST_EQUALS(content.textGradient.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayMode, Dali::Ui::Text::GradientOverlayMode::SCREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayStyleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerStyleOverlayStyleTextureSlotP(void)
{
  TestApplication application;

  const Size verifiedSize(2.0f, 2.0f);
  Sampler    sampler = Sampler::New();
  UiText::MarqueeBuilder::PreparedContent content;
  const auto request = CreateSimpleStyleContentRequest(true, true);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplySimpleStyleContent(content, request), true, TEST_LOCATION);
  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryAppendSimpleOverlayStyleContent(content,
                                                                              CreatePixelData(2u, 2u, Pixel::RGBA8888),
                                                                              sampler,
                                                                              verifiedSize),
                   true,
                   TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(3u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(4u));
  DALI_TEST_EQUALS(content.textGradient.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayStyleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerSimpleOverlayStyleDoesNotAppendToMixedContentP(void)
{
  TestApplication application;

  const Size verifiedSize(2.0f, 2.0f);
  Sampler    sampler = Sampler::New();
  UiText::MarqueeBuilder::PreparedContent content;
  const auto request = CreateMixedGradientContentRequest(true, false);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplyMixedGradientContent(content, request), true, TEST_LOCATION);
  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryAppendSimpleOverlayStyleContent(content,
                                                                              CreatePixelData(2u, 2u, Pixel::RGBA8888),
                                                                              sampler,
                                                                              verifiedSize),
                   false,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.mixedTextGradient, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayStyleTextureEnabled, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerMixedOverlayStyleTextureSlotP(void)
{
  TestApplication application;

  const Size verifiedSize(2.0f, 2.0f);
  Sampler    sampler = Sampler::New();
  UiText::MarqueeBuilder::PreparedContent content;
  const auto request = CreateMixedGradientContentRequest(false, false);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplyMixedGradientContent(content, request), true, TEST_LOCATION);
  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryAppendMixedOverlayStyleContent(content,
                                                                             CreatePixelData(2u, 2u, Pixel::RGBA8888),
                                                                             sampler,
                                                                             verifiedSize),
                   true,
                   TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(3u));
  DALI_TEST_EQUALS(content.textGradient.mixedTextGradient, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayStyleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerMixedGradientOverlayOverlayStyleTextureSlotP(void)
{
  TestApplication application;

  const Size verifiedSize(2.0f, 2.0f);
  Sampler    sampler = Sampler::New();
  UiText::MarqueeBuilder::PreparedContent content;
  const auto request = CreateMixedGradientContentRequest(true, false);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplyMixedGradientContent(content, request), true, TEST_LOCATION);
  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryAppendMixedOverlayStyleContent(content,
                                                                             CreatePixelData(2u, 2u, Pixel::RGBA8888),
                                                                             sampler,
                                                                             verifiedSize),
                   true,
                   TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(3u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(4u));
  DALI_TEST_EQUALS(content.textGradient.mixedTextGradient, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayStyleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerMixedStyleOverlayStyleTextureSlotP(void)
{
  TestApplication application;

  const Size verifiedSize(2.0f, 2.0f);
  Sampler    sampler = Sampler::New();
  UiText::MarqueeBuilder::PreparedContent content;
  const auto request = CreateMixedGradientContentRequest(false, true);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplyMixedGradientContent(content, request), true, TEST_LOCATION);
  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryAppendMixedOverlayStyleContent(content,
                                                                             CreatePixelData(2u, 2u, Pixel::RGBA8888),
                                                                             sampler,
                                                                             verifiedSize),
                   true,
                   TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(3u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(4u));
  DALI_TEST_EQUALS(content.textGradient.mixedTextGradient, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayStyleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerMixedStyleGradientOverlayOverlayStyleTextureSlotP(void)
{
  TestApplication application;

  const Size verifiedSize(2.0f, 2.0f);
  Sampler    sampler = Sampler::New();
  UiText::MarqueeBuilder::PreparedContent content;
  const auto request = CreateMixedGradientContentRequest(true, true);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplyMixedGradientContent(content, request), true, TEST_LOCATION);
  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryAppendMixedOverlayStyleContent(content,
                                                                             CreatePixelData(2u, 2u, Pixel::RGBA8888),
                                                                             sampler,
                                                                             verifiedSize),
                   true,
                   TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(3u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(4u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(5u));
  DALI_TEST_EQUALS(content.textGradient.mixedTextGradient, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayStyleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeMixedStyleBaseLookupAndStyleTextureSlotsP(void)
{
  TestApplication application;

  UiText::MarqueeBuilder::PreparedContent content;
  const auto request = CreateMixedGradientContentRequest(false, true);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplyMixedGradientContent(content, request), true, TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(3u));
  DALI_TEST_EQUALS(content.textGradient.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.mixedTextGradient, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeMixedStyleBaseOverlayLookupAndStyleTextureSlotsP(void)
{
  TestApplication application;

  UiText::MarqueeBuilder::PreparedContent content;
  const auto request = CreateMixedGradientContentRequest(true, true);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplyMixedGradientContent(content, request), true, TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(3u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(4u));
  DALI_TEST_EQUALS(content.textGradient.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.mixedTextGradient, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayMode, Dali::Ui::Text::GradientOverlayMode::SCREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeStyleBaseLookupAndStyleTextureSlotsP(void)
{
  TestApplication application;

  UiText::MarqueeBuilder::PreparedContent content;
  const auto request = CreateSimpleStyleContentRequest(true, false);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplySimpleStyleContent(content, request), true, TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_EQUALS(content.textGradient.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeStyleOverlayLookupAndStyleTextureSlotsP(void)
{
  TestApplication application;

  UiText::MarqueeBuilder::PreparedContent content;
  const auto request = CreateSimpleStyleContentRequest(false, true);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplySimpleStyleContent(content, request), true, TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_EQUALS(content.textGradient.enabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayMode, Dali::Ui::Text::GradientOverlayMode::SCREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeStyleBaseOverlayLookupAndStyleTextureSlotsP(void)
{
  TestApplication application;

  UiText::MarqueeBuilder::PreparedContent content;
  const auto request = CreateSimpleStyleContentRequest(true, true);

  DALI_TEST_EQUALS(UiText::MarqueeBuilder::TryApplySimpleStyleContent(content, request), true, TEST_LOCATION);
  DALI_TEST_CHECK(content.textureSet.GetTexture(0u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(1u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(2u));
  DALI_TEST_CHECK(content.textureSet.GetTexture(3u));
  DALI_TEST_EQUALS(content.textGradient.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.overlayMode, Dali::Ui::Text::GradientOverlayMode::SCREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(content.textGradient.styleTextureEnabled, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeScrollerUpdatesRendererBoundsP(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  const Vector4 staleBounds(0.0f, 0.0f, 0.25f, 1.0f);
  const Vector4 marqueeBounds(0.25f, 0.0f, 0.5f, 1.0f);
  const Property::Index boundsIndex = renderer.RegisterProperty("uTextGradientBounds", staleBounds);
  const Property::Index typeIndex = renderer.RegisterProperty("uTextGradientType", 1.0f);
  const Property::Index radialCenterIndex = renderer.RegisterProperty("uTextGradientRadialCenter", Vector2::ZERO);
  const Property::Index radialScaleIndex = renderer.RegisterProperty("uTextGradientRadialScale", Vector2::ZERO);
  const Property::Index conicCenterIndex = renderer.RegisterProperty("uTextGradientConicCenter", Vector2::ZERO);
  const Property::Index conicScaleIndex = renderer.RegisterProperty("uTextGradientConicScale", Vector2::ZERO);
  const Property::Index conicStartAngleIndex = renderer.RegisterProperty("uTextGradientConicStartAngle", 0.0f);

  UiText::TextScrollerGradient textGradient;
  textGradient.enabled       = true;
  textGradient.type          = Dali::Ui::Gradient::Type::CONIC;
  textGradient.startPosition = Vector2::ZERO;
  textGradient.endPosition   = Vector2::ONE;
  textGradient.radialCenter  = Vector2(0.5f, 0.5f);
  textGradient.radialScale   = Vector2(2.0f, 2.0f);
  textGradient.conicCenter     = Vector2(0.25f, 0.75f);
  textGradient.conicScale      = Vector2(100.0f, 40.0f);
  textGradient.conicStartAngle = 0.75f;
  textGradient.startOffset   = 0.0f;
  textGradient.bounds        = marqueeBounds;

  TestScrollerInterface     scrollerInterface;
  UiText::TextScrollerPtr   scroller = UiText::TextScroller::New(scrollerInterface);
  const Actor               actor    = Actor::New();
  const TextureSet          textureSet = TextureSet::New();

  scroller->SetParameters(actor,
                          renderer,
                          textureSet,
                          Size(100.0f, 40.0f),
                          Size(200.0f, 40.0f),
                          20.0f,
                          true,
                          false,
                          UiText::Alignment::CENTER,
                          UiText::Alignment::CENTER,
                          true,
                          textGradient);

  Vector4 actualBounds;
  renderer.GetProperty(boundsIndex).Get(actualBounds);
  ExpectBounds(actualBounds, marqueeBounds);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(typeIndex), 3.0f, EPSILON, TEST_LOCATION);
  Vector2 actualRadialCenter;
  Vector2 actualRadialScale;
  renderer.GetProperty(radialCenterIndex).Get(actualRadialCenter);
  renderer.GetProperty(radialScaleIndex).Get(actualRadialScale);
  ExpectPosition(actualRadialCenter, Vector2(0.5f, 0.5f));
  ExpectPosition(actualRadialScale, Vector2(2.0f, 2.0f));
  Vector2 actualConicCenter;
  Vector2 actualConicScale;
  renderer.GetProperty(conicCenterIndex).Get(actualConicCenter);
  renderer.GetProperty(conicScaleIndex).Get(actualConicScale);
  ExpectPosition(actualConicCenter, Vector2(0.25f, 0.75f));
  ExpectPosition(actualConicScale, Vector2(100.0f, 40.0f));
  DALI_TEST_EQUALS(renderer.GetProperty<float>(conicStartAngleIndex), 0.75f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeScrollerInitializesPixelSnapFactorP(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  TestScrollerInterface   scrollerInterface;
  UiText::TextScrollerPtr scroller = UiText::TextScroller::New(scrollerInterface);
  scroller->SetParameters(Actor::New(),
                          renderer,
                          TextureSet::New(),
                          Size(100.0f, 40.0f),
                          Size(200.0f, 40.0f),
                          20.0f,
                          true,
                          false,
                          UiText::Alignment::CENTER,
                          UiText::Alignment::CENTER,
                          true,
                          UiText::TextScrollerGradient{});

  Shader                marqueeShader        = renderer.GetShader();
  const Property::Index pixelSnapFactorIndex = marqueeShader.GetPropertyIndex("pixelSnapFactor");
  DALI_TEST_CHECK(pixelSnapFactorIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(marqueeShader.GetProperty<float>(pixelSnapFactorIndex), 0.0f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextScrollerHorizontalAlignmentIgnoresWrapGapP(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  TestScrollerInterface   scrollerInterface;
  UiText::TextScrollerPtr scroller   = UiText::TextScroller::New(scrollerInterface);
  const Actor             actor      = Actor::New();
  const TextureSet        textureSet = TextureSet::New();

  auto getHorizontalAlignment = [&](const Size&       textureSize,
                                    float             wrapGap,
                                    bool              isTextContentOverflow,
                                    bool              isTextDirectionRtl,
                                    UiText::Alignment requestedAlignment)
  {
    scroller->SetParameters(actor,
                            renderer,
                            textureSet,
                            Size(100.0f, 40.0f),
                            textureSize,
                            wrapGap,
                            isTextContentOverflow,
                            isTextDirectionRtl,
                            requestedAlignment,
                            UiText::Alignment::CENTER);

    Shader                marqueeShader = renderer.GetShader();
    const Property::Index index         = marqueeShader.GetPropertyIndex("uHorizontalAlign");
    DALI_TEST_CHECK(index != Property::INVALID_INDEX);
    return marqueeShader.GetProperty<float>(index);
  };

  // The texture includes the wrap gap. A fitting text width must retain the requested alignment.
  DALI_TEST_EQUALS(getHorizontalAlignment(Size(130.0f, 40.0f), 40.0f, false, false, UiText::Alignment::CENTER),
                   0.0f,
                   EPSILON,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(getHorizontalAlignment(Size(140.0f, 40.0f), 40.0f, false, false, UiText::Alignment::END),
                   0.5f,
                   EPSILON,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(getHorizontalAlignment(Size(130.0f, 40.0f), 40.0f, false, true, UiText::Alignment::END),
                   -0.5f,
                   EPSILON,
                   TEST_LOCATION);

  // Invalid fitting geometry keeps the legacy zero-delta fallback.
  scroller->SetParameters(actor,
                          renderer,
                          textureSet,
                          Size(100.0f, 40.0f),
                          Size(130.0f, 40.0f),
                          40.0f,
                          false,
                          false,
                          UiText::Alignment::CENTER,
                          UiText::Alignment::CENTER,
                          false,
                          UiText::TextScrollerGradient(),
                          UiText::MarqueeInitialDelta{-10.0f, false});
  const Property::Index fittingDeltaIndex = renderer.GetShader().GetPropertyIndex("uDelta");
  DALI_TEST_CHECK(fittingDeltaIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(renderer.GetShader().GetProperty<float>(fittingDeltaIndex), 0.0f, EPSILON, TEST_LOCATION);

  // Overflow classification selects the viewport independently. A valid
  // fitting transition delta is applied without changing requested CENTER.
  scroller->SetParameters(actor,
                          renderer,
                          textureSet,
                          Size(100.0f, 40.0f),
                          Size(130.0f, 40.0f),
                          40.0f,
                          false,
                          false,
                          UiText::Alignment::CENTER,
                          UiText::Alignment::CENTER,
                          false,
                          UiText::TextScrollerGradient(),
                          UiText::MarqueeInitialDelta{-10.0f, true});
  Shader                fittingShader          = renderer.GetShader();
  const Property::Index fittingAlignIndex      = fittingShader.GetPropertyIndex("uHorizontalAlign");
  const Property::Index validFittingDeltaIndex = fittingShader.GetPropertyIndex("uDelta");
  DALI_TEST_CHECK(fittingAlignIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(validFittingDeltaIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(fittingShader.GetProperty<float>(fittingAlignIndex), 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(fittingShader.GetProperty<float>(validFittingDeltaIndex), -10.0f, EPSILON, TEST_LOCATION);

  // Actual overflow keeps the legacy logical START alignment in both directions.
  DALI_TEST_EQUALS(getHorizontalAlignment(Size(150.0f, 40.0f), 40.0f, true, false, UiText::Alignment::CENTER),
                   -0.5f,
                   EPSILON,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(getHorizontalAlignment(Size(150.0f, 40.0f), 40.0f, true, true, UiText::Alignment::CENTER),
                   0.5f,
                   EPSILON,
                   TEST_LOCATION);

  const UiText::MarqueeInitialDelta ltrInitialDelta{-10.0f, true};
  scroller->SetParameters(actor,
                          renderer,
                          textureSet,
                          Size(100.0f, 40.0f),
                          Size(150.0f, 40.0f),
                          20.0f,
                          true,
                          false,
                          UiText::Alignment::END,
                          UiText::Alignment::CENTER,
                          false,
                          UiText::TextScrollerGradient(),
                          ltrInitialDelta);
  Shader                ltrMarqueeShader = renderer.GetShader();
  const Property::Index ltrDeltaIndex    = ltrMarqueeShader.GetPropertyIndex("uDelta");
  DALI_TEST_CHECK(ltrDeltaIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(ltrMarqueeShader.GetProperty<float>(ltrDeltaIndex), -10.0f, EPSILON, TEST_LOCATION);
  const Property::Index ltrAlignIndex = ltrMarqueeShader.GetPropertyIndex("uHorizontalAlign");
  DALI_TEST_CHECK(ltrAlignIndex != Property::INVALID_INDEX);
  const float ltrViewportStart = ltrMarqueeShader.GetProperty<float>(ltrDeltaIndex) +
                                 UiText::ResolveLegacyHorizontalMarqueeViewportOrigin(
                                   ltrMarqueeShader.GetProperty<float>(ltrAlignIndex),
                                   150.0f,
                                   100.0f,
                                   20.0f);
  DALI_TEST_EQUALS(ltrViewportStart, -10.0f, EPSILON, TEST_LOCATION);

  // The continuity correction is relative to the direction-resolved logical START.
  // It must not cancel the RTL alignment already applied by the shader.
  const UiText::MarqueeInitialDelta rtlInitialDelta{-10.0f, true};
  scroller->SetParameters(actor,
                          renderer,
                          textureSet,
                          Size(100.0f, 40.0f),
                          Size(150.0f, 40.0f),
                          20.0f,
                          true,
                          true,
                          UiText::Alignment::CENTER,
                          UiText::Alignment::CENTER,
                          false,
                          UiText::TextScrollerGradient(),
                          rtlInitialDelta);
  Shader                rtlMarqueeShader = renderer.GetShader();
  const Property::Index rtlDeltaIndex    = rtlMarqueeShader.GetPropertyIndex("uDelta");
  DALI_TEST_CHECK(rtlDeltaIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(rtlMarqueeShader.GetProperty<float>(rtlDeltaIndex), -10.0f, EPSILON, TEST_LOCATION);
  const Property::Index rtlAlignIndex = rtlMarqueeShader.GetPropertyIndex("uHorizontalAlign");
  DALI_TEST_CHECK(rtlAlignIndex != Property::INVALID_INDEX);
  const float rtlViewportStart = rtlMarqueeShader.GetProperty<float>(rtlDeltaIndex) +
                                 UiText::ResolveLegacyHorizontalMarqueeViewportOrigin(
                                   rtlMarqueeShader.GetProperty<float>(rtlAlignIndex),
                                   150.0f,
                                   100.0f,
                                   20.0f);
  DALI_TEST_EQUALS(rtlViewportStart, 20.0f, EPSILON, TEST_LOCATION);

  // Horizontal continuity metadata is ignored by the frozen vertical marquee path.
  scroller->SetOrientation(UiText::MarqueeOrientation::VERTICAL);
  scroller->SetParameters(actor,
                          renderer,
                          textureSet,
                          Size(100.0f, 40.0f),
                          Size(100.0f, 80.0f),
                          20.0f,
                          true,
                          false,
                          UiText::Alignment::CENTER,
                          UiText::Alignment::CENTER,
                          false,
                          UiText::TextScrollerGradient(),
                          UiText::MarqueeInitialDelta{-10.0f, true});
  const Property::Index verticalDeltaIndex = renderer.GetShader().GetPropertyIndex("uDelta");
  DALI_TEST_CHECK(verticalDeltaIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(renderer.GetShader().GetProperty<float>(verticalDeltaIndex), 0.0f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFittingMarqueeLabelLifecycleP(void)
{
  UiTestApplication application;

  Dali::Ui::Label label = Dali::Ui::Label::New("Fitting marquee lifecycle text");
  label.SetFontSize(18.0f);
  label.SetHorizontalTextAlignment(UiText::Alignment::CENTER);
  label.SetMarqueeTriggerPolicy(UiText::MarqueeTriggerPolicy::MANUAL);
  label.SetMarqueeStopMode(UiText::MarqueeStopMode::IMMEDIATE);
  label.SetMarqueeLoopCount(0);
  label.SetMarqueeLoopDelay(10.0f);

  const float naturalWidth = label.GetNaturalSize().width;
  label.SetRequestedWidth(naturalWidth + 169.0f);
  label.SetRequestedHeight(40.0f);
  application.GetScene().Add(label);

  const auto render = [&]
  {
    application.SendNotification();
    application.Render(16u);
    application.SendNotification();
  };
  const auto getMarqueeShader = [&]
  {
    DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    Shader shader = label.GetRendererAt(0u).GetShader();
    DALI_TEST_CHECK(shader.GetPropertyIndex("uDelta") != Property::INVALID_INDEX);
    return shader;
  };

  render();
  label.StartMarquee();
  render();
  Shader firstShader = getMarqueeShader();
  DALI_TEST_EQUALS(std::fabs(firstShader.GetProperty<float>(firstShader.GetPropertyIndex("uDelta"))),
                   0.5f,
                   EPSILON,
                   TEST_LOCATION);

  label.StopMarquee();
  render();
  label.StartMarquee();
  render();
  Shader replayShader = getMarqueeShader();
  DALI_TEST_EQUALS(std::fabs(replayShader.GetProperty<float>(replayShader.GetPropertyIndex("uDelta"))),
                   0.5f,
                   EPSILON,
                   TEST_LOCATION);

  label.StopMarquee();
  label.SetRequestedWidth(naturalWidth + 170.0f);
  render();
  label.StartMarquee();
  render();
  Shader resizedShader = getMarqueeShader();
  DALI_TEST_EQUALS(resizedShader.GetProperty<float>(resizedShader.GetPropertyIndex("uDelta")),
                   0.0f,
                   EPSILON,
                   TEST_LOCATION);

  label.StopMarquee();
  label.SetRequestedWidth(naturalWidth + 169.0f);
  label.SetHorizontalTextAlignment(UiText::Alignment::END);
  render();
  label.StartMarquee();
  render();
  Shader endShader = getMarqueeShader();
  DALI_TEST_EQUALS(endShader.GetProperty<float>(endShader.GetPropertyIndex("uHorizontalAlign")),
                   0.5f,
                   EPSILON,
                   TEST_LOCATION);
  const float endDelta = endShader.GetProperty<float>(endShader.GetPropertyIndex("uDelta"));
  DALI_TEST_CHECK(std::fabs(endDelta) < EPSILON || std::fabs(endDelta + 1.0f) < EPSILON);

  label.StopMarquee();
  label.SetText("Changed fitting marquee text");
  label.SetHorizontalTextAlignment(UiText::Alignment::CENTER);
  const float changedNaturalWidth = label.GetNaturalSize().width;
  label.SetRequestedWidth(changedNaturalWidth + 169.0f);
  render();
  label.StartMarquee();
  render();
  Shader changedShader = getMarqueeShader();
  DALI_TEST_EQUALS(std::fabs(changedShader.GetProperty<float>(changedShader.GetPropertyIndex("uDelta"))),
                   0.5f,
                   EPSILON,
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliAsyncTextMarqueeReportsContentOverflowP(void)
{
  TestApplication application;

  auto makeParameters = []
  {
    // Exercise the render-scale path where texture and wrap-gap publication cannot be used to infer overflow.
    UiText::AsyncTextParameters parameters;
    parameters.text               = "marquee overflow state";
    parameters.textWidth          = 200.0f;
    parameters.textHeight         = 80.0f;
    parameters.renderScale        = 2.0f;
    parameters.renderScaleWidth   = 100.0f;
    parameters.renderScaleHeight  = 40.0f;
    parameters.maxTextureSize     = 2048;
    parameters.requestType        = UiIntegrationText::Async::RENDER_FIXED_SIZE;
    parameters.isMarqueeEnabled   = true;
    parameters.marqueeGap         = 80;
    parameters.marqueeOrientation = UiText::MarqueeOrientation::HORIZONTAL;
    return parameters;
  };

  UiText::AsyncTextLoader loader = UiText::AsyncTextLoader::New();

  UiText::AsyncTextParameters       fittingParameters = makeParameters();
  const UiText::AsyncTextRenderInfo fittingInfo =
    loader.RenderMarquee(fittingParameters, true, Size(180.0f, 40.0f));
  DALI_TEST_EQUALS(fittingInfo.isMarqueeContentOverflow, false, TEST_LOCATION);

  UiText::AsyncTextParameters       overflowingParameters = makeParameters();
  const UiText::AsyncTextRenderInfo overflowingInfo =
    loader.RenderMarquee(overflowingParameters, true, Size(220.0f, 40.0f));
  DALI_TEST_EQUALS(overflowingInfo.isMarqueeContentOverflow, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeScrollerUpdatesBaseAnimSourceP(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  const Property::Index startOffsetIndex = renderer.RegisterProperty("uTextGradientStartOffset", 0.0f);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  UiText::TextScrollerGradient textGradient;
  textGradient.enabled                   = true;
  textGradient.type                      = Dali::Ui::Gradient::Type::LINEAR;
  textGradient.startPosition             = Vector2::ZERO;
  textGradient.endPosition               = Vector2::ONE;
  textGradient.startOffset               = 0.15f;
  textGradient.bounds                    = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  textGradient.startOffsetPropertyIndex  = Property::INVALID_INDEX;
  textGradient.applyConstraintsAlways    = true;

  TestScrollerInterface   scrollerInterface;
  UiText::TextScrollerPtr scroller   = UiText::TextScroller::New(scrollerInterface);
  const TextureSet        textureSet = TextureSet::New();

  scroller->SetParameters(actor,
                          renderer,
                          textureSet,
                          Size(100.0f, 40.0f),
                          Size(200.0f, 40.0f),
                          20.0f,
                          true,
                          false,
                          UiText::Alignment::CENTER,
                          UiText::Alignment::CENTER,
                          true,
                          textGradient);

  application.SendNotification();
  application.Render(16u);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(startOffsetIndex), 0.15f, EPSILON, TEST_LOCATION);

  const Property::Index sourceStartOffsetIndex = actor.RegisterProperty("uTextGradientStartOffset", 0.45f);
  scroller->SetGradientAnimProperties(sourceStartOffsetIndex);

  application.SendNotification();
  application.Render(16u);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(startOffsetIndex), 0.45f, EPSILON, TEST_LOCATION);

  actor.SetProperty(sourceStartOffsetIndex, 0.8f);
  application.SendNotification();
  application.Render(16u);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(startOffsetIndex), 0.8f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeScrollerUpdatesOverlayRendererPropertiesP(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  const Vector4 staleBounds(0.0f, 0.0f, 0.25f, 1.0f);
  const Vector4 overlayBounds(0.0f, 0.25f, 1.0f, 0.5f);
  const Property::Index boundsIndex = renderer.RegisterProperty("uTextGradientOverlayBounds", staleBounds);
  const Property::Index typeIndex = renderer.RegisterProperty("uTextGradientOverlayType", 1.0f);
  const Property::Index startOffsetIndex = renderer.RegisterProperty("uTextGradientOverlayStartOffset", 0.0f);
  const Property::Index radialCenterIndex = renderer.RegisterProperty("uTextGradientOverlayRadialCenter", Vector2::ZERO);
  const Property::Index radialScaleIndex = renderer.RegisterProperty("uTextGradientOverlayRadialScale", Vector2::ZERO);
  const Property::Index conicCenterIndex = renderer.RegisterProperty("uTextGradientOverlayConicCenter", Vector2::ZERO);
  const Property::Index conicScaleIndex = renderer.RegisterProperty("uTextGradientOverlayConicScale", Vector2::ZERO);
  const Property::Index conicStartAngleIndex = renderer.RegisterProperty("uTextGradientOverlayConicStartAngle", 0.0f);
  const Property::Index modeIndex = renderer.RegisterProperty("uTextGradientOverlayMode", 0.0f);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);
  const Property::Index sourceStartOffsetIndex = actor.RegisterProperty("uTextGradientOverlayStartOffset", 0.35f);

  UiText::TextScrollerGradient textGradient;
  textGradient.overlayEnabled                  = true;
  textGradient.overlayType                     = Dali::Ui::Gradient::Type::CONIC;
  textGradient.overlayStartPosition            = Vector2::ZERO;
  textGradient.overlayEndPosition              = Vector2::ONE;
  textGradient.overlayRadialCenter             = Vector2(0.5f, 0.5f);
  textGradient.overlayRadialScale              = Vector2(2.0f, 2.0f);
  textGradient.overlayConicCenter              = Vector2(0.25f, 0.75f);
  textGradient.overlayConicScale               = Vector2(100.0f, 40.0f);
  textGradient.overlayConicStartAngle          = 0.75f;
  textGradient.overlayStartOffset              = 0.0f;
  textGradient.overlayBounds                   = overlayBounds;
  textGradient.overlayMode                     = Dali::Ui::Text::GradientOverlayMode::SCREEN;
  textGradient.overlayStartOffsetPropertyIndex = sourceStartOffsetIndex;
  textGradient.overlayApplyConstraintsAlways   = true;

  TestScrollerInterface   scrollerInterface;
  UiText::TextScrollerPtr scroller   = UiText::TextScroller::New(scrollerInterface);
  const TextureSet        textureSet = TextureSet::New();

  scroller->SetParameters(actor,
                          renderer,
                          textureSet,
                          Size(100.0f, 40.0f),
                          Size(200.0f, 40.0f),
                          20.0f,
                          true,
                          false,
                          UiText::Alignment::CENTER,
                          UiText::Alignment::CENTER,
                          true,
                          textGradient);

  Vector4 actualBounds;
  renderer.GetProperty(boundsIndex).Get(actualBounds);
  ExpectBounds(actualBounds, overlayBounds);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(typeIndex), 3.0f, EPSILON, TEST_LOCATION);
  Vector2 actualRadialCenter;
  Vector2 actualRadialScale;
  renderer.GetProperty(radialCenterIndex).Get(actualRadialCenter);
  renderer.GetProperty(radialScaleIndex).Get(actualRadialScale);
  ExpectPosition(actualRadialCenter, Vector2(0.5f, 0.5f));
  ExpectPosition(actualRadialScale, Vector2(2.0f, 2.0f));
  Vector2 actualConicCenter;
  Vector2 actualConicScale;
  renderer.GetProperty(conicCenterIndex).Get(actualConicCenter);
  renderer.GetProperty(conicScaleIndex).Get(actualConicScale);
  ExpectPosition(actualConicCenter, Vector2(0.25f, 0.75f));
  ExpectPosition(actualConicScale, Vector2(100.0f, 40.0f));
  DALI_TEST_EQUALS(renderer.GetProperty<float>(conicStartAngleIndex), 0.75f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(modeIndex), 1.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextGradientStartOffset"), Property::INVALID_INDEX, TEST_LOCATION);

  application.SendNotification();
  application.Render(16u);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(startOffsetIndex), 0.35f, EPSILON, TEST_LOCATION);

  actor.SetProperty(sourceStartOffsetIndex, 0.8f);
  application.SendNotification();
  application.Render(16u);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(startOffsetIndex), 0.8f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeScrollerKeepsBaseAndOverlayIndependentP(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  const Vector4 baseBounds(0.1f, 0.0f, 0.8f, 1.0f);
  const Vector4 overlayBounds(0.0f, 0.2f, 1.0f, 0.6f);
  const Property::Index baseBoundsIndex = renderer.RegisterProperty("uTextGradientBounds", Vector4::ZERO);
  const Property::Index overlayBoundsIndex = renderer.RegisterProperty("uTextGradientOverlayBounds", Vector4::ZERO);
  const Property::Index baseStartOffsetIndex = renderer.RegisterProperty("uTextGradientStartOffset", 0.0f);
  const Property::Index overlayStartOffsetIndex = renderer.RegisterProperty("uTextGradientOverlayStartOffset", 0.0f);
  const Property::Index overlayModeIndex = renderer.RegisterProperty("uTextGradientOverlayMode", 0.0f);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  const Property::Index baseSourceIndex = actor.RegisterProperty("baseGradientStartOffset", 0.2f);
  const Property::Index overlaySourceIndex = actor.RegisterProperty("overlayGradientStartOffset", 0.4f);
  DALI_TEST_CHECK(baseSourceIndex != overlaySourceIndex);

  UiText::TextScrollerGradient textGradient;
  textGradient.enabled                   = true;
  textGradient.type                      = Dali::Ui::Gradient::Type::LINEAR;
  textGradient.startPosition             = Vector2::ZERO;
  textGradient.endPosition               = Vector2::ONE;
  textGradient.startOffset               = 0.0f;
  textGradient.bounds                    = baseBounds;
  textGradient.startOffsetPropertyIndex  = baseSourceIndex;
  textGradient.applyConstraintsAlways    = true;
  textGradient.overlayEnabled            = true;
  textGradient.overlayType               = Dali::Ui::Gradient::Type::LINEAR;
  textGradient.overlayStartPosition      = Vector2(-0.5f, 0.0f);
  textGradient.overlayEndPosition        = Vector2(0.5f, 0.0f);
  textGradient.overlayStartOffset        = 0.0f;
  textGradient.overlayBounds             = overlayBounds;
  textGradient.overlayMode               = Dali::Ui::Text::GradientOverlayMode::SCREEN;
  textGradient.overlayStartOffsetPropertyIndex = overlaySourceIndex;
  textGradient.overlayApplyConstraintsAlways   = true;

  DALI_TEST_EQUALS(textGradient.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(textGradient.overlayEnabled, true, TEST_LOCATION);

  TestScrollerInterface   scrollerInterface;
  UiText::TextScrollerPtr scroller   = UiText::TextScroller::New(scrollerInterface);
  const TextureSet        textureSet = TextureSet::New();

  scroller->SetParameters(actor,
                          renderer,
                          textureSet,
                          Size(100.0f, 40.0f),
                          Size(200.0f, 40.0f),
                          20.0f,
                          true,
                          false,
                          UiText::Alignment::CENTER,
                          UiText::Alignment::CENTER,
                          true,
                          textGradient);

  DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextGradientStartOffset") != Property::INVALID_INDEX);
  DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextGradientOverlayStartOffset") != Property::INVALID_INDEX);
  DALI_TEST_CHECK(baseStartOffsetIndex != overlayStartOffsetIndex);

  Vector4 actualBaseBounds;
  Vector4 actualOverlayBounds;
  renderer.GetProperty(baseBoundsIndex).Get(actualBaseBounds);
  renderer.GetProperty(overlayBoundsIndex).Get(actualOverlayBounds);
  ExpectBounds(actualBaseBounds, baseBounds);
  ExpectBounds(actualOverlayBounds, overlayBounds);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(overlayModeIndex), 1.0f, EPSILON, TEST_LOCATION);

  application.SendNotification();
  application.Render(16u);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(baseStartOffsetIndex), 0.2f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(overlayStartOffsetIndex), 0.4f, EPSILON, TEST_LOCATION);

  actor.SetProperty(baseSourceIndex, 0.25f);
  application.SendNotification();
  application.Render(16u);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(baseStartOffsetIndex), 0.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(overlayStartOffsetIndex), 0.4f, EPSILON, TEST_LOCATION);

  actor.SetProperty(overlaySourceIndex, 0.85f);
  application.SendNotification();
  application.Render(16u);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(baseStartOffsetIndex), 0.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(overlayStartOffsetIndex), 0.85f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientMarqueeScrollerDisabledKeepsRendererCleanP(void)
{
  TestApplication application;

  Geometry rendererGeometry = CreateQuadGeometry();
  Shader   rendererShader   = CreateShader();
  Renderer renderer         = Renderer::New(rendererGeometry, rendererShader);

  TestScrollerInterface   scrollerInterface;
  UiText::TextScrollerPtr scroller   = UiText::TextScroller::New(scrollerInterface);
  const Actor             actor      = Actor::New();
  const TextureSet        textureSet = TextureSet::New();

  scroller->SetParameters(actor,
                          renderer,
                          textureSet,
                          Size(100.0f, 40.0f),
                          Size(200.0f, 40.0f),
                          20.0f,
                          true,
                          false,
                          UiText::Alignment::CENTER,
                          UiText::Alignment::CENTER,
                          true);

  ExpectNoTextGradientRendererProperties(renderer);
  ExpectNoTextGradientOverlayRendererProperties(renderer);
  END_TEST;
}

int UtcDaliTextGradientMarqueeCreatesRadialUniformValuesP(void)
{
  TextInternal::Gradient::Style style;
  style.enabled      = true;
  style.type         = Dali::Ui::Gradient::Type::RADIAL;
  style.units        = Dali::Ui::Gradient::Units::USER_SPACE;
  style.radialCenter = Vector2(50.0f, 20.0f);
  style.radialRadius = 25.0f;
  style.stops.PushBack({0.0f, Color::RED});
  style.stops.PushBack({1.0f, Color::BLUE});

  const Vector4 bounds(0.25f, 0.0f, 0.5f, 1.0f);
  const Vector2 coordinateSize(200.0f, 40.0f);

  UiText::TextScrollerGradient textGradient =
    TextInternal::GradientMarquee::CreateScrollerGradient(style, bounds, coordinateSize);

  DALI_TEST_EQUALS(textGradient.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(textGradient.type, Dali::Ui::Gradient::Type::RADIAL, TEST_LOCATION);
  ExpectPosition(textGradient.radialCenter, Vector2(0.5f, 0.5f));
  ExpectPosition(textGradient.radialScale, Vector2(4.0f, 1.6f));
  END_TEST;
}

int UtcDaliTextGradientMarqueeCreatesConicUniformValuesP(void)
{
  TextInternal::Gradient::Style style;
  style.enabled         = true;
  style.type            = Dali::Ui::Gradient::Type::CONIC;
  style.units           = Dali::Ui::Gradient::Units::USER_SPACE;
  style.conicCenter     = Vector2(50.0f, 20.0f);
  style.conicStartAngle = Radian(0.75f);
  style.stops.PushBack({0.0f, Color::RED});
  style.stops.PushBack({1.0f, Color::BLUE});

  const Vector4 bounds(0.25f, 0.0f, 0.5f, 1.0f);
  const Vector2 coordinateSize(200.0f, 40.0f);

  UiText::TextScrollerGradient textGradient =
    TextInternal::GradientMarquee::CreateScrollerGradient(style, bounds, coordinateSize);

  DALI_TEST_EQUALS(textGradient.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(textGradient.type, Dali::Ui::Gradient::Type::CONIC, TEST_LOCATION);
  ExpectPosition(textGradient.conicCenter, Vector2(0.5f, 0.5f));
  ExpectPosition(textGradient.conicScale, Vector2(100.0f, 40.0f));
  DALI_TEST_EQUALS(textGradient.conicStartAngle, 0.75f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAsyncSimpleFeatureP(void)
{
  TextFeature::FeatureBuilder builder = BuildAsyncFeature(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT, TEST_LOCATION);
  ExpectTextGradientDefine(fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAsyncMultiColorFallbackP(void)
{
  TextFeature::FeatureBuilder builder = BuildAsyncFeature(true, true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAsyncMixedMultiColorFeatureP(void)
{
  TextFeature::FeatureBuilder builder = BuildAsyncFeature(false, true, false, false, false, false, true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAsyncMixedStyleFeatureP(void)
{
  TextFeature::FeatureBuilder builder = BuildAsyncFeature(false, true, false, true, false, false, true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED_WITH_STYLE, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAsyncMixedOverlayStyleFeatureP(void)
{
  TextFeature::FeatureBuilder builder = BuildAsyncFeature(false, true, false, false, true, false, true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_TEXT_GRADIENT_MIXED_WITH_OVERLAY, TEST_LOCATION);
  ExpectTextGradientMixedDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_OVERLAY\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAsyncStyleFeatureP(void)
{
  TextFeature::FeatureBuilder builder = BuildAsyncFeature(true, false, false, true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT_AND_STYLE, TEST_LOCATION);
  ExpectTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAsyncStyleAndOverlayFeatureP(void)
{
  TextFeature::FeatureBuilder builder = BuildAsyncFeature(true, false, false, true, true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_TEXT_GRADIENT_AND_STYLE_AND_OVERLAY, TEST_LOCATION);
  ExpectTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_OVERLAY\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionAsyncTextVisualDisabledFallbackP(void)
{
  TextFeature::FeatureBuilder builder = BuildAsyncFeature(false);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  END_TEST;
}
