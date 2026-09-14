//@name text-reveal-blur.vert

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

// Batched Reveal blur passes use this vertex shader; scalar passes reuse control-renderers.vert.
precision highp float;
INPUT highp vec2 aPosition;
INPUT highp vec2 aTexCoord;
INPUT highp vec4 aRevealRectangle;
INPUT highp vec2 aRevealInverseSize;
INPUT highp float aRevealLineIndex;
OUTPUT highp vec2 vTexCoord;
OUTPUT highp vec4 vRevealRectangle;
OUTPUT highp vec2 vRevealOffsetDirection;
OUTPUT highp vec2 vRevealBlurState;
UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec2 uOffsetDirection;
  UNIFORM highp vec2 uRevealBlurState[NUM_REVEAL_LINES];
};
void main()
{
  // Coordinates are already page-local pixels. Each quad has constant bounds
  // and the exact update-side strength; no timing equation is re-evaluated here.
  gl_Position = uMvpMatrix * vec4(aPosition, 0.0, 1.0);
  vTexCoord = aTexCoord;
  vRevealRectangle = aRevealRectangle;
  vRevealOffsetDirection = aRevealInverseSize * uOffsetDirection;
  vRevealBlurState = uRevealBlurState[int(aRevealLineIndex)];
}
