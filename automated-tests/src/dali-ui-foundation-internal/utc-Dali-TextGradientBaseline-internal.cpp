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

#include <dali-ui-foundation/internal/text/color-segmentation.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual-shader-factory.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

#include <initializer_list>
#include <string>

using namespace Dali;

namespace
{

namespace TextFeature = Dali::Ui::Internal::TextVisualShaderFeature;
namespace UiInternal  = Dali::Ui::Internal;
namespace UiText      = Dali::Ui::Text;

Vector<UiText::GlyphIndex> MakeGlyphMap(std::initializer_list<UiText::GlyphIndex> values)
{
  Vector<UiText::GlyphIndex> glyphMap;
  glyphMap.Reserve(static_cast<uint32_t>(values.size()));

  for(auto value : values)
  {
    glyphMap.PushBack(value);
  }

  return glyphMap;
}

Vector<UiText::Length> MakeGlyphsPerCharacter(std::initializer_list<UiText::Length> values)
{
  Vector<UiText::Length> glyphsPerCharacter;
  glyphsPerCharacter.Reserve(static_cast<uint32_t>(values.size()));

  for(auto value : values)
  {
    glyphsPerCharacter.PushBack(value);
  }

  return glyphsPerCharacter;
}

UiText::ColorRun MakeColorRun(UiText::CharacterIndex characterIndex, UiText::Length numberOfCharacters, const Vector4& color)
{
  UiText::ColorRun run;
  run.characterRun.characterIndex     = characterIndex;
  run.characterRun.numberOfCharacters = numberOfCharacters;
  run.color                           = color;
  return run;
}

void ExpectNoGradientPrefix(const std::string& vertexPrefix, const std::string& fragmentPrefix)
{
  DALI_TEST_EQUALS(vertexPrefix.find("GRADIENT") == std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("GRADIENT") == std::string::npos, true, TEST_LOCATION);
}

} // namespace

void utc_dali_text_gradient_baseline_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_gradient_baseline_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextGradientBaselineTextVisualDefaultShaderFeatureP(void)
{
  TextFeature::FeatureBuilder builder;

  std::string vertexPrefix;
  std::string fragmentPrefix;
  builder.GetVertexShaderPrefixList(vertexPrefix);
  builder.GetFragmentShaderPrefixList(fragmentPrefix);

  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT, TEST_LOCATION);
  DALI_TEST_EQUALS(vertexPrefix.empty(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.empty(), true, TEST_LOCATION);
  ExpectNoGradientPrefix(vertexPrefix, fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientBaselineTextVisualEmojiShaderFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableEmoji(true);

  std::string vertexPrefix;
  std::string fragmentPrefix;
  builder.GetVertexShaderPrefixList(vertexPrefix);
  builder.GetFragmentShaderPrefixList(fragmentPrefix);

  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI, TEST_LOCATION);
  DALI_TEST_EQUALS(vertexPrefix.empty(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_EMOJI\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") == std::string::npos, true, TEST_LOCATION);
  ExpectNoGradientPrefix(vertexPrefix, fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientBaselineTextVisualMultiColorSuppressesEmojiFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableEmoji(true);
  builder.EnableMultiColor(true);

  std::string vertexPrefix;
  std::string fragmentPrefix;
  builder.GetVertexShaderPrefixList(vertexPrefix);
  builder.GetFragmentShaderPrefixList(fragmentPrefix);

  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT, TEST_LOCATION);
  DALI_TEST_EQUALS(vertexPrefix.empty(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_MULTI_COLOR\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_EMOJI\n") == std::string::npos, true, TEST_LOCATION);
  ExpectNoGradientPrefix(vertexPrefix, fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientBaselineTextVisualStyleOverlayEmbossFeatureP(void)
{
  TextFeature::FeatureBuilder builder;
  builder.EnableStyle(true);
  builder.EnableOverlay(true);
  builder.EnableEmboss(true);

  std::string vertexPrefix;
  std::string fragmentPrefix;
  builder.GetVertexShaderPrefixList(vertexPrefix);
  builder.GetFragmentShaderPrefixList(fragmentPrefix);

  DALI_TEST_EQUALS(builder.GetShaderType(), UiInternal::VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_OVERLAY_AND_EMBOSS, TEST_LOCATION);
  DALI_TEST_EQUALS(vertexPrefix.empty(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_STYLE\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_OVERLAY\n") != std::string::npos, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.find("#define IS_REQUIRED_EMBOSS\n") != std::string::npos, true, TEST_LOCATION);
  ExpectNoGradientPrefix(vertexPrefix, fragmentPrefix);
  END_TEST;
}

int UtcDaliTextGradientBaselineColorSegmentationDefaultIndicesP(void)
{
  Vector<UiText::ColorRun>   colorRuns;
  Vector<Vector4>            colors;
  Vector<UiText::ColorIndex> colorIndices;

  UiText::SetColorSegmentationInfo(
    colorRuns,
    MakeGlyphMap({0u, 1u, 2u}),
    MakeGlyphsPerCharacter({1u, 1u, 1u}),
    0u,
    0u,
    3u,
    colors,
    colorIndices);

  DALI_TEST_EQUALS(colors.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices[0], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices[1], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices[2], 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientBaselineColorSegmentationExplicitIndicesP(void)
{
  Vector<UiText::ColorRun> colorRuns;
  colorRuns.PushBack(MakeColorRun(1u, 2u, Color::RED));

  Vector<Vector4>            colors;
  Vector<UiText::ColorIndex> colorIndices;

  UiText::SetColorSegmentationInfo(
    colorRuns,
    MakeGlyphMap({0u, 1u, 2u}),
    MakeGlyphsPerCharacter({1u, 1u, 1u}),
    0u,
    0u,
    3u,
    colors,
    colorIndices);

  DALI_TEST_EQUALS(colors.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(colors[0], Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices[0], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices[1], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices[2], 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientBaselineColorSegmentationReusesColorIndicesP(void)
{
  Vector<UiText::ColorRun> colorRuns;
  colorRuns.PushBack(MakeColorRun(0u, 1u, Color::GREEN));
  colorRuns.PushBack(MakeColorRun(1u, 1u, Color::BLUE));
  colorRuns.PushBack(MakeColorRun(2u, 1u, Color::GREEN));

  Vector<Vector4>            colors;
  Vector<UiText::ColorIndex> colorIndices;

  UiText::SetColorSegmentationInfo(
    colorRuns,
    MakeGlyphMap({0u, 1u, 2u}),
    MakeGlyphsPerCharacter({1u, 1u, 1u}),
    0u,
    0u,
    3u,
    colors,
    colorIndices);

  DALI_TEST_EQUALS(colors.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(colors[0], Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(colors[1], Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices[0], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices[1], 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(colorIndices[2], 1u, TEST_LOCATION);
  END_TEST;
}
