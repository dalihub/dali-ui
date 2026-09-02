#ifndef DALI_UI_INLINE_REPLACEMENT_IMAGE_REVEAL_SHADER_H
#define DALI_UI_INLINE_REPLACEMENT_IMAGE_REVEAL_SHADER_H

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

namespace Dali::Ui::Internal::Text
{
/**
 * @brief Fragment shader for spatial PIXEL Reveal of a texture-backed inline replacement.
 *
 * ImageVisual supplies its standard vertex shader, texture, mix color and
 * alpha mode. The manager precomposes pixel-area normalization into the
 * timing coefficients, so the fragment only consumes the final texture
 * coordinate produced by the standard vertex shader.
 */
inline constexpr char INLINE_REPLACEMENT_IMAGE_REVEAL_FRAGMENT_SHADER[] = R"SHADER(
//@name inline-replacement-image-reveal-shader.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;

UNIFORM sampler2D sTexture;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4   uColor;
  UNIFORM lowp float  premultipliedAlpha;
  UNIFORM highp float uInlineReplacementRevealProgress;
  UNIFORM highp vec3  uInlineReplacementRevealTiming;
};

void main()
{
  lowp vec4 textureColor = TEXTURE(sTexture, vTexCoord) * uColor;

  highp float progress = clamp(uInlineReplacementRevealProgress, 0.0, 1.0);
  highp float reveal = 0.0;
  if(progress >= 1.0)
  {
    reveal = 1.0;
  }
  else if(progress > 0.0)
  {
    highp float fragmentStart = uInlineReplacementRevealTiming.x +
                                uInlineReplacementRevealTiming.y * vTexCoord.x;
    highp float fadeDuration = uInlineReplacementRevealTiming.z;
    reveal = fadeDuration > 0.0
               ? clamp((progress - fragmentStart) / fadeDuration, 0.0, 1.0)
               : step(fragmentStart, progress);
  }

  textureColor.a *= reveal;
  textureColor.rgb *= mix(1.0, reveal, premultipliedAlpha);
  gl_FragColor = textureColor;
}
)SHADER";
} // namespace Dali::Ui::Internal::Text

#endif // DALI_UI_INLINE_REPLACEMENT_IMAGE_REVEAL_SHADER_H
