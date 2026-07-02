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

#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/text/text-scroller.h>
#include <dali-ui-foundation/internal/text/text-scroller-interface.h>
#include <dali-ui-foundation/internal/text/text-gradient-bounds.h>
#include <dali-ui-foundation/internal/text/text-gradient-helper.h>
#include <dali-ui-foundation/internal/text/text-gradient-marquee-helper.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual-shader-factory.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <mesh-builder.h>

#include <string>

using namespace Dali;

namespace
{

namespace TextFeature = Dali::Ui::Internal::TextVisualShaderFeature;
namespace TextInternal = Dali::Ui::Text::Internal;
namespace UiText      = Dali::Ui::Text;
namespace UiInternal  = Dali::Ui::Internal;

constexpr const char* TEXT_GRADIENT_DEFINE = "#define IS_REQUIRED_TEXT_GRADIENT\n";
constexpr const char* TEXT_GRADIENT_MIXED_DEFINE = "#define IS_REQUIRED_TEXT_GRADIENT_MIXED\n";
constexpr const char* TEXT_GRADIENT_OVERLAY_DEFINE = "#define IS_REQUIRED_TEXT_GRADIENT_OVERLAY\n";
constexpr float       EPSILON              = 0.0001f;

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

class TestScrollerInterface : public UiText::ScrollerInterface
{
public:
  void ScrollingFinished() override
  {
  }
};

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

int UtcDaliTextGradientOverlayShaderFeatureWithMarkupAndEmojiP(void)
{
  TextFeature::FeatureBuilder markupBuilder;
  markupBuilder.EnableMultiColor(true);
  markupBuilder.EnableTextGradientOverlay(true);

  std::string markupPrefix = GetFragmentPrefix(markupBuilder);

  DALI_TEST_EQUALS(markupBuilder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_TEXT_GRADIENT_OVERLAY, TEST_LOCATION);
  ExpectNoTextGradientDefine(markupPrefix);
  ExpectTextGradientOverlayDefine(markupPrefix);
  DALI_TEST_EQUALS(markupPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") != std::string::npos, true, TEST_LOCATION);

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

int UtcDaliTextGradientShaderCompositionDisabledColorOnlyMarkupKeepsLegacyStyleFeatureP(void)
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

int UtcDaliTextGradientShaderCompositionColorOnlyMarkupMixedIgnoresLegacyStyleFeatureP(void)
{
  TextFeature::FeatureBuilder builder =
    BuildSeparatedStyleFeature(false, true, true, false, true);

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

int UtcDaliTextGradientShaderCompositionMixedStyleFallbackP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);
  builder.EnableStyle(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionMixedOverlayFallbackP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableTextGradientMixed(true);
  builder.EnableMultiColor(true);
  builder.EnableOverlay(true);

  std::string fragmentPrefix = GetFragmentPrefix(builder);

  DALI_TEST_EQUALS(builder.IsEnabledTextGradient(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.IsEnabledTextGradientMixed(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_OVERLAY, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_OVERLAY\n") != std::string::npos, true, TEST_LOCATION);
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

int UtcDaliTextGradientRenderDataLinearP(void)
{
  TextInternal::TextGradientStyle style;
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

  const TextInternal::TextGradient::TextGradientRenderData renderData =
    TextInternal::TextGradient::ResolveGradientRenderData(style, bounds, coordinateSize);

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

int UtcDaliTextGradientRenderDataRadialP(void)
{
  TextInternal::TextGradientStyle style;
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

  const TextInternal::TextGradient::TextGradientRenderData renderData =
    TextInternal::TextGradient::ResolveGradientRenderData(style, bounds, coordinateSize);

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

int UtcDaliTextGradientRenderDataConicP(void)
{
  TextInternal::TextGradientStyle style;
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

  const TextInternal::TextGradient::TextGradientRenderData renderData =
    TextInternal::TextGradient::ResolveGradientRenderData(style, bounds, coordinateSize);

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

int UtcDaliTextGradientRenderDataUnsupportedP(void)
{
  const Vector4 bounds(0.25f, 0.0f, 0.5f, 1.0f);
  const Vector2 coordinateSize(200.0f, 40.0f);

  TextInternal::TextGradientStyle noneStyle;
  TextInternal::TextGradient::TextGradientRenderData renderData =
    TextInternal::TextGradient::ResolveGradientRenderData(noneStyle, bounds, coordinateSize);

  DALI_TEST_EQUALS(renderData.enabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(renderData.type, Dali::Ui::Gradient::Type::NONE, TEST_LOCATION);

  TextInternal::TextGradientStyle oneStopStyle;
  oneStopStyle.enabled     = true;
  oneStopStyle.type        = Dali::Ui::Gradient::Type::LINEAR;
  oneStopStyle.linearStart = Vector2(-0.5f, 0.0f);
  oneStopStyle.linearEnd   = Vector2(0.5f, 0.0f);
  oneStopStyle.stops.PushBack({0.0f, Color::RED});

  renderData = TextInternal::TextGradient::ResolveGradientRenderData(oneStopStyle, bounds, coordinateSize);

  DALI_TEST_EQUALS(renderData.enabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(renderData.type, Dali::Ui::Gradient::Type::NONE, TEST_LOCATION);
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

  TextInternal::TextGradientStyle baseStyle;
  baseStyle.enabled     = true;
  baseStyle.type        = Dali::Ui::Gradient::Type::LINEAR;
  baseStyle.linearStart = Vector2::ZERO;
  baseStyle.linearEnd   = Vector2::ONE;
  baseStyle.stops.PushBack({0.0f, Color::RED});
  baseStyle.stops.PushBack({1.0f, Color::BLUE});

  TextInternal::TextGradientStyle overlayStyle;
  overlayStyle.enabled     = true;
  overlayStyle.type        = Dali::Ui::Gradient::Type::LINEAR;
  overlayStyle.linearStart = Vector2(-0.5f, 0.0f);
  overlayStyle.linearEnd   = Vector2(0.5f, 0.0f);
  overlayStyle.stops.PushBack({0.0f, Color::TRANSPARENT});
  overlayStyle.stops.PushBack({1.0f, Color::WHITE});

  TextureSet textureSet      = TextureSet::New();
  uint32_t   textureSetIndex = 1u;
  TextInternal::TextGradient::AddLookupTexture(textureSet, textureSetIndex, baseStyle);
  TextInternal::TextGradient::AddLookupTexture(textureSet, textureSetIndex, overlayStyle);

  DALI_TEST_EQUALS(textureSetIndex, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(textureSet.GetTexture(1u));
  DALI_TEST_CHECK(textureSet.GetTexture(2u));
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

  UiText::TextScrollerTextGradient textGradient;
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

  UiText::TextScrollerTextGradient textGradient;
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

  UiText::TextScrollerTextGradient textGradient;
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
  TextInternal::TextGradientStyle style;
  style.enabled      = true;
  style.type         = Dali::Ui::Gradient::Type::RADIAL;
  style.units        = Dali::Ui::Gradient::Units::USER_SPACE;
  style.radialCenter = Vector2(50.0f, 20.0f);
  style.radialRadius = 25.0f;
  style.stops.PushBack({0.0f, Color::RED});
  style.stops.PushBack({1.0f, Color::BLUE});

  const Vector4 bounds(0.25f, 0.0f, 0.5f, 1.0f);
  const Vector2 coordinateSize(200.0f, 40.0f);

  UiText::TextScrollerTextGradient textGradient =
    TextInternal::TextGradientMarquee::CreateMarqueeGradient(style, bounds, coordinateSize);

  DALI_TEST_EQUALS(textGradient.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(textGradient.type, Dali::Ui::Gradient::Type::RADIAL, TEST_LOCATION);
  ExpectPosition(textGradient.radialCenter, Vector2(0.5f, 0.5f));
  ExpectPosition(textGradient.radialScale, Vector2(4.0f, 1.6f));
  END_TEST;
}

int UtcDaliTextGradientMarqueeCreatesConicUniformValuesP(void)
{
  TextInternal::TextGradientStyle style;
  style.enabled         = true;
  style.type            = Dali::Ui::Gradient::Type::CONIC;
  style.units           = Dali::Ui::Gradient::Units::USER_SPACE;
  style.conicCenter     = Vector2(50.0f, 20.0f);
  style.conicStartAngle = Radian(0.75f);
  style.stops.PushBack({0.0f, Color::RED});
  style.stops.PushBack({1.0f, Color::BLUE});

  const Vector4 bounds(0.25f, 0.0f, 0.5f, 1.0f);
  const Vector2 coordinateSize(200.0f, 40.0f);

  UiText::TextScrollerTextGradient textGradient =
    TextInternal::TextGradientMarquee::CreateMarqueeGradient(style, bounds, coordinateSize);

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
