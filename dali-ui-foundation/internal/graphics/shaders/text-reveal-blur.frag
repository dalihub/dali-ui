//@name text-reveal-blur.frag

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

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;

UNIFORM_BLOCK FragBlock
{
#ifndef TEXT_REVEAL_DRAW_BATCH
  UNIFORM highp float uAnimationRatio;
#endif
  UNIFORM highp float uOpacity;
#ifndef TEXT_REVEAL_DRAW_BATCH
  UNIFORM highp vec2  uOffsetDirection;
#endif
  UNIFORM highp float uTextRevealBlurProgress;
#ifndef TEXT_REVEAL_DRAW_BATCH
  UNIFORM highp float uRevealSequenceStart;
  UNIFORM highp vec4 uRevealBatchRect;
#endif
  UNIFORM highp vec2 uRevealBatchInvSize;
};

UNIFORM_BLOCK SharedBlock
{
  UNIFORM highp vec3 uSize;
  UNIFORM highp float viewEffectiveScale;
};

UNIFORM_BLOCK GaussianBlurSampleBlock
{
  UNIFORM highp float uSampleOffsets[NUM_SAMPLES];
  UNIFORM highp float uSampleWeights[NUM_SAMPLES];
};

#ifdef TEXT_REVEAL_DRAW_BATCH
INPUT highp vec4 vRevealRectangle;
INPUT highp vec2 vRevealOffsetDirection;
INPUT highp vec2 vRevealBlurState;
#define uRevealBatchRect vRevealRectangle
#define uOffsetDirection vRevealOffsetDirection
#define uAnimationRatio vRevealBlurState.x
#define uRevealSequenceStart vRevealBlurState.y
#endif

highp vec4 ReadRevealBlurTexture(highp vec2 uv)
{
  // Each quad keeps line-local UVs and its own radius/timing. Clamp within
  // its padded tile, not the page edge, so adjacent lines cannot bleed.
  highp vec2 point = uRevealBatchRect.xy + uv * uRevealBatchRect.zw;
  highp vec2 low = uRevealBatchRect.xy + uRevealBatchInvSize * 0.5;
  highp vec2 high = uRevealBatchRect.xy + uRevealBatchRect.zw - uRevealBatchInvSize * 0.5;
  return TEXTURE(sTexture, clamp(point, low, high));
}
#define READ_BLUR_TEXTURE(uv) ReadRevealBlurTexture(uv)

void main()
{
  // Allow for 16-bit rounding of start metadata while keeping sequences
  // transparent before their start, including STEP and PIXEL sequences.
  if(uTextRevealBlurProgress <= 0.0 ||
     uTextRevealBlurProgress < uRevealSequenceStart - 1.0 / 65535.0)
  {
    gl_FragColor = vec4(0.0);
    return;
  }
  if(uAnimationRatio == 0.0)
  {
    // Reveal may still be fading after blur ends. Copy the current source,
    // not a cached completed frame, so seeks and reverse remain symmetric.
    gl_FragColor = READ_BLUR_TEXTURE(vTexCoord) * uOpacity;
    return;
  }
  highp vec4 col = vec4(0.0);

  for (int i=0; i<NUM_SAMPLES; ++i)
  {
    col += (READ_BLUR_TEXTURE(vTexCoord + uSampleOffsets[i] * uOffsetDirection * uAnimationRatio * viewEffectiveScale) + READ_BLUR_TEXTURE(vTexCoord - uSampleOffsets[i] * uOffsetDirection * uAnimationRatio * viewEffectiveScale)) * uSampleWeights[i];
  }
  col *= uOpacity;
  gl_FragColor = col;
}
