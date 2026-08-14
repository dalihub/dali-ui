#ifndef DALI_UI_TEXT_VISUAL_SHADER_FACTORY_H
#define DALI_UI_TEXT_VISUAL_SHADER_FACTORY_H

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
 */

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/shader-precompiler.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-shader-factory-interface.h>
#include <cstdint>
#include <string_view>
#include <unordered_map>

namespace Dali
{
namespace Ui
{
namespace Internal
{
/**
 * TextVisualShaderFeature contains feature lists what text visual shader need to know.
 */
namespace TextVisualShaderFeature
{
namespace TextMultiColor
{
/**
 * @brief Whether text contains single color or not.
 */
enum Type
{
  SINGLE_COLOR_TEXT = 0, ///< The text contains single color only.
  MULTI_COLOR_TEXT       ///< The text contains multiple colorｓ.
};
} // namespace TextMultiColor

namespace TextEmoji
{
/**
 * @brief Whether text contains emoji or not.
 */
enum Type
{
  NO_EMOJI = 0, ///< The text contains no emoji.
  HAS_EMOJI     ///< The text contains emoji.
};
} // namespace TextEmoji

namespace TextStyle
{
/**
 * @brief Whether text contains styles (like shadow or background color) or not.
 */
enum Type
{
  NO_STYLES = 0, ///< The text contains no styles.
  HAS_STYLES     ///< The text contains styles.
};
} // namespace TextStyle

namespace TextOverlay
{
/**
 * @brief Whether text contains overlay styles (like markdown) or not.
 */
enum Type
{
  NO_OVERLAY = 0, ///< The text contains no overlay.
  HAS_OVERLAY     ///< The text contains overlay.
};
} // namespace TextOverlay

namespace TextEmboss
{
/**
 * @brief Whether text contains emboss or not.
 */
enum Type
{
  NO_EMBOSS = 0, ///< The text contains no emboss.
  HAS_EMBOSS     ///< The text contains emboss.
};
} // namespace TextEmboss

namespace TextGradient
{
/**
 * @brief Whether text uses TextGradient fill or not.
 */
enum Type : uint8_t
{
  NO_TEXT_GRADIENT = 0,   ///< The text uses the existing text color fill.
  HAS_TEXT_GRADIENT,      ///< The single-color text uses TextGradient fill.
  HAS_TEXT_GRADIENT_MIXED ///< The default-color glyphs in mixed text use TextGradient fill.
};
} // namespace TextGradient

namespace TextGradientOverlay
{
/**
 * @brief Whether text uses TextGradientOverlay or not.
 */
enum Type
{
  NO_TEXT_GRADIENT_OVERLAY = 0, ///< The text uses no TextGradientOverlay.
  HAS_TEXT_GRADIENT_OVERLAY     ///< The resolved text fill uses TextGradientOverlay.
};
} // namespace TextGradientOverlay

namespace TextReveal
{
enum Type
{
  NO_TEXT_REVEAL = 0,
  HAS_TEXT_REVEAL
};
} // namespace TextReveal

/**
 * @brief Collection of current text visual feature.
 */
class FeatureBuilder
{
public:
  FeatureBuilder();
  FeatureBuilder& EnableMultiColor(bool enableMultiColor);
  FeatureBuilder& EnableEmoji(bool enableEmoji);
  FeatureBuilder& EnableStyle(bool enableStyle);
  FeatureBuilder& EnableOverlay(bool enableOverlay);
  FeatureBuilder& EnableEmboss(bool enableEmboss);
  FeatureBuilder& EnableTextGradient(bool enableTextGradient);
  FeatureBuilder& EnableTextGradientMixed(bool enableTextGradientMixed);
  FeatureBuilder& EnableTextGradientOverlay(bool enableTextGradientOverlay);
  FeatureBuilder& EnableTextReveal(bool enableTextReveal);

  VisualFactoryCache::ShaderType GetShaderType() const;
  void                           GetVertexShaderPrefixList(std::string& vertexShaderPrefixList) const;
  void                           GetFragmentShaderPrefixList(std::string& fragmentShaderPrefixList) const;

  bool IsEnabledMultiColor() const
  {
    return mTextMultiColor == TextMultiColor::MULTI_COLOR_TEXT;
  }
  bool IsEnabledEmoji() const
  {
    return mTextEmoji == TextEmoji::HAS_EMOJI;
  }
  bool IsEnabledStyle() const
  {
    return mTextStyle == TextStyle::HAS_STYLES;
  }
  bool IsEnabledOverlay() const
  {
    return mTextOverlay == TextOverlay::HAS_OVERLAY;
  }
  bool isEnabledEmboss() const
  {
    return mTextEmboss == TextEmboss::HAS_EMBOSS;
  }
  bool IsEnabledTextGradient() const
  {
    return mTextGradient == TextGradient::HAS_TEXT_GRADIENT &&
           mTextMultiColor == TextMultiColor::SINGLE_COLOR_TEXT &&
           mTextEmoji == TextEmoji::NO_EMOJI &&
           mTextEmboss == TextEmboss::NO_EMBOSS;
  }
  bool IsEnabledTextGradientMixed() const
  {
    return mTextGradient == TextGradient::HAS_TEXT_GRADIENT_MIXED &&
           (mTextMultiColor == TextMultiColor::MULTI_COLOR_TEXT ||
            mTextEmoji == TextEmoji::HAS_EMOJI) &&
           mTextEmboss == TextEmboss::NO_EMBOSS;
  }
  bool IsEnabledAnyTextGradient() const
  {
    return IsEnabledTextGradient() || IsEnabledTextGradientMixed();
  }
  bool IsEnabledTextGradientOverlay() const
  {
    return mTextGradientOverlay == TextGradientOverlay::HAS_TEXT_GRADIENT_OVERLAY;
  }
  bool IsEnabledTextReveal() const
  {
    return mTextReveal == TextReveal::HAS_TEXT_REVEAL;
  }

private:
  TextMultiColor::Type
                    mTextMultiColor : 2; ///< Whether text has multiple color, or not. default as TextMultiColor::SINGLE_COLOR_TEXT
  TextEmoji::Type   mTextEmoji : 2;      ///< Whether text has emoji, or not. default as TextEmoji::NO_EMOJI
  TextStyle::Type   mTextStyle : 2;      ///< Whether text has style, or not. default as TextStyle::NO_STYLES
  TextOverlay::Type mTextOverlay : 2;    ///< Whether text has overlay style, or not. default as TextOverlay::NO_OVERLAY
  TextEmboss::Type  mTextEmboss : 2;     ///< Whether text has emboss style, or not. default as TextEmboss::NO_EMBOSS
  TextGradient::Type
    mTextGradient : 2; ///< Whether text uses TextGradient fill, or not. default as TextGradient::NO_TEXT_GRADIENT
  TextGradientOverlay::Type
    mTextGradientOverlay : 2; ///< Whether text uses TextGradientOverlay, or not. default as TextGradientOverlay::NO_TEXT_GRADIENT_OVERLAY
  TextReveal::Type mTextReveal : 1;          ///< Sequential foreground reveal shader variant.
};

} // namespace TextVisualShaderFeature

/**
 * TextVisualShaderFactory is an object that provides and shares shaders for text visuals
 */
class TextVisualShaderFactory : public VisualShaderFactoryInterface
{
public:
  /**
   * @brief Constructor
   */
  TextVisualShaderFactory();

  /**
   * @brief Destructor
   */
  ~TextVisualShaderFactory();

  /**
   * @brief Get the standard text rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] featureBuilder Collection of current text shader's features
   * @return The standard text rendering shader with features.
   */
  Shader GetShader(VisualFactoryCache& factoryCache, const TextVisualShaderFeature::FeatureBuilder& featureBuilder);

public: // Implementation of VisualShaderFactoryInterface
  /**
   * @copydoc Dali::Ui::VisualShaderFactoryInterface::AddPrecompiledShader
   */
  bool AddPrecompiledShader(PrecompileShaderOption& option) override;

  /**
   * @copydoc Dali::Ui::VisualShaderFactoryInterface::GetPreCompiledShader
   */
  void GetPreCompiledShader(ShaderPreCompiler::RawShaderData& shaders) override;

private:
  std::unordered_map<uint32_t, VisualFactoryCache::ExternalShaderId> mRevealShaderIds;

  /**
   * @brief Create pre-compiled shader for image with builder and option.
   */
  void CreatePrecompileShader(TextVisualShaderFeature::FeatureBuilder& builder, const ShaderFlagList& option);

  /**
   * @brief Check if cached hash value is valid or not.
   */
  bool SavePrecompileShader(VisualFactoryCache::ShaderType shader, std::string&& vertexPrefix,
                            std::string&& fragmentPrefix);

protected:
  /**
   * Undefined copy constructor.
   */
  TextVisualShaderFactory(const TextVisualShaderFactory&);

  /**
   * Undefined assignment operator.
   */
  TextVisualShaderFactory& operator=(const TextVisualShaderFactory& rhs);
};

} // namespace Internal

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_VISUAL_SHADER_FACTORY_H
