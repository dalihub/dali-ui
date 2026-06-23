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

void ExpectTextGradientDefine(const std::string& fragmentPrefix)
{
  DALI_TEST_EQUALS(fragmentPrefix.find(TEXT_GRADIENT_DEFINE) != std::string::npos, true, TEST_LOCATION);
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
  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.empty(), true, TEST_LOCATION);
  ExpectNoTextGradientDefine(fragmentPrefix);
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
  DALI_TEST_EQUALS(fragmentShader.find("textGradientCoord") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionObjectBoundingBoxPositionP(void)
{
  const Vector4 bounds(0.25f, 0.25f, 0.5f, 0.5f);
  const Vector2 textureSize(200.0f, 100.0f);

  ExpectPosition(TextInternal::ResolveTextGradientPosition(Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX,
                                                           Vector2(-0.5f, -0.5f),
                                                           bounds,
                                                           textureSize),
                 Vector2(0.0f, 0.0f));
  ExpectPosition(TextInternal::ResolveTextGradientPosition(Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX,
                                                           Vector2(0.5f, 0.5f),
                                                           bounds,
                                                           textureSize),
                 Vector2(1.0f, 1.0f));
  ExpectPosition(TextInternal::ResolveTextGradientPosition(Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX,
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

  ExpectPosition(TextInternal::ResolveTextGradientPosition(Dali::Ui::Gradient::Units::USER_SPACE,
                                                           Vector2(50.0f, 25.0f),
                                                           bounds,
                                                           textureSize),
                 Vector2(0.5f, 0.5f));
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
  DALI_TEST_EQUALS(fragmentShader.find("(vTextGradientCoord - uTextGradientBounds.xy)") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("fract(") == std::string::npos, true, TEST_LOCATION);
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
  DALI_TEST_EQUALS(fragmentShader.find("(vTextGradientCoord - uTextGradientBounds.xy)") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("fract(") == std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentShader.find("TEXTURE(sGradientLookup, vec2(gradientPosition + uTextGradientStartOffset, 0.5))") != std::string::npos, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionCalculateBoundsStartP(void)
{
  UiText::LineRun line;
  line.width           = 40.0f;
  line.alignmentOffset = 0.0f;

  const Vector4 bounds = TextInternal::CalculateTextGradientBounds(Vector2(100.0f, 50.0f),
                                                                   Vector2(40.0f, 20.0f),
                                                                   &line,
                                                                   1u,
                                                                   UiText::Alignment::START);

  ExpectBounds(bounds, Vector4(0.0f, 0.0f, 0.4f, 0.4f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionCalculateBoundsCenterP(void)
{
  UiText::LineRun line;
  line.width           = 40.0f;
  line.alignmentOffset = 30.0f;

  const Vector4 bounds = TextInternal::CalculateTextGradientBounds(Vector2(100.0f, 50.0f),
                                                                   Vector2(40.0f, 20.0f),
                                                                   &line,
                                                                   1u,
                                                                   UiText::Alignment::CENTER);

  ExpectBounds(bounds, Vector4(0.3f, 0.3f, 0.4f, 0.4f));
  END_TEST;
}

int UtcDaliTextGradientShaderCompositionCalculateBoundsEndP(void)
{
  UiText::LineRun line;
  line.width           = 40.0f;
  line.alignmentOffset = 60.0f;

  const Vector4 bounds = TextInternal::CalculateTextGradientBounds(Vector2(100.0f, 50.0f),
                                                                   Vector2(40.0f, 20.0f),
                                                                   &line,
                                                                   1u,
                                                                   UiText::Alignment::END);

  ExpectBounds(bounds, Vector4(0.6f, 0.6f, 0.4f, 0.4f));
  END_TEST;
}

int UtcDaliTextGradientCalculateMarqueeViewportBoundsHorizontalOverflowP(void)
{
  UiText::LineRun line;
  line.width           = 200.0f;
  line.alignmentOffset = 0.0f;
  line.ascender        = 15.0f;
  line.descender       = -5.0f;
  line.lineSpacing     = 0.0f;

  const Vector4 bounds = TextInternal::CalculateMarqueeTextGradientViewportBounds(Vector2(100.0f, 40.0f),
                                                                                  Vector2(200.0f, 20.0f),
                                                                                  &line,
                                                                                  1u,
                                                                                  UiText::Alignment::START,
                                                                                  UiText::Alignment::CENTER);

  ExpectBounds(bounds, Vector4(0.0f, 0.25f, 1.0f, 0.5f));
  END_TEST;
}

int UtcDaliTextGradientCalculateMarqueeViewportBoundsHorizontalShortEndP(void)
{
  UiText::LineRun line;
  line.width           = 40.0f;
  line.alignmentOffset = 0.0f;
  line.ascender        = 15.0f;
  line.descender       = -5.0f;
  line.lineSpacing     = 0.0f;

  const Vector4 bounds = TextInternal::CalculateMarqueeTextGradientViewportBounds(Vector2(100.0f, 40.0f),
                                                                                  Vector2(40.0f, 20.0f),
                                                                                  &line,
                                                                                  1u,
                                                                                  UiText::Alignment::END,
                                                                                  UiText::Alignment::END);

  ExpectBounds(bounds, Vector4(0.6f, 0.5f, 0.4f, 0.5f));
  END_TEST;
}

int UtcDaliTextGradientCalculateMarqueeViewportBoundsVerticalOverflowP(void)
{
  UiText::LineRun line;
  line.width           = 50.0f;
  line.alignmentOffset = 0.0f;
  line.ascender        = 160.0f;
  line.descender       = 0.0f;
  line.lineSpacing     = 0.0f;

  const Vector4 bounds = TextInternal::CalculateMarqueeTextGradientViewportBounds(Vector2(100.0f, 80.0f),
                                                                                  Vector2(50.0f, 160.0f),
                                                                                  &line,
                                                                                  1u,
                                                                                  UiText::Alignment::CENTER,
                                                                                  UiText::Alignment::START);

  ExpectBounds(bounds, Vector4(0.25f, 0.0f, 0.5f, 1.0f));
  END_TEST;
}

int UtcDaliTextGradientCalculateMarqueeViewportBoundsVerticalShortEndP(void)
{
  UiText::LineRun line;
  line.width           = 50.0f;
  line.alignmentOffset = 0.0f;
  line.ascender        = 15.0f;
  line.descender       = -5.0f;
  line.lineSpacing     = 0.0f;

  const Vector4 bounds = TextInternal::CalculateMarqueeTextGradientViewportBounds(Vector2(100.0f, 80.0f),
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

  const Vector4 viewportBounds = TextInternal::CalculateMarqueeTextGradientViewportBounds(Vector2(100.0f, 40.0f),
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

int UtcDaliTextGradientMarqueeScrollerUpdatesRendererBoundsP(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  const Vector4 staleBounds(0.0f, 0.0f, 0.25f, 1.0f);
  const Vector4 marqueeBounds(0.25f, 0.0f, 0.5f, 1.0f);
  const Property::Index boundsIndex = renderer.RegisterProperty("uTextGradientBounds", staleBounds);

  UiText::TextScrollerTextGradient textGradient;
  textGradient.enabled       = true;
  textGradient.startPosition = Vector2::ZERO;
  textGradient.endPosition   = Vector2::ONE;
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
