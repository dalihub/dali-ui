//@name npatch-visual-3x3-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
OUTPUT highp vec2 vTexCoord;
OUTPUT highp vec2 vMaskTexCoord;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
  UNIFORM highp float viewEffectiveScale;
  UNIFORM highp vec2 uFixed[3];
  UNIFORM highp vec2 uStretchTotal;
  UNIFORM lowp  float visualTransformUseEffectiveScale;
};

UNIFORM_BLOCK VisualVertBlock
{
  //Visual size and offset
  UNIFORM highp vec2 offset;
  UNIFORM highp vec2 size;
  UNIFORM highp vec2 extraSize;
  UNIFORM mediump vec4 offsetSizeMode;
  UNIFORM mediump vec2 origin;
  UNIFORM mediump vec2 pivot;
};

void main()
{
  highp float effectiveScale = mix(1.0, viewEffectiveScale, visualTransformUseEffectiveScale);
  highp vec2 visualSize = mix(size * uSize.xy, size * effectiveScale, offsetSizeMode.zw) + extraSize * effectiveScale;
  highp vec2 visualOffset = mix(offset * uSize.xy, offset * effectiveScale, offsetSizeMode.xy);

  highp vec2 fixedFactor = vec2(uFixed[int((aPosition.x + 1.0) * 0.5)].x, uFixed[int((aPosition.y  + 1.0) * 0.5)].y);
  highp vec2 stretch  = floor(aPosition * 0.5);
  highp vec2 fixedTotal = uFixed[2];

  // The fixed frame is in source pixels, which are natural units like the natural size, so it is drawn
  // at the view's effective scale; scale it down further if it is bigger than visualSize.
  highp float fixedScale = min(viewEffectiveScale, min(visualSize.x / fixedTotal.x, visualSize.y / fixedTotal.y));

  highp vec4 gridPosition = vec4(fixedFactor * fixedScale + (visualSize - fixedTotal * fixedScale) * stretch, 0.0, 1.0);
  highp vec4 vertexPosition = gridPosition;
  vertexPosition.xy -= visualSize * vec2(0.5, 0.5);
  vertexPosition.xy += pivot * visualSize + visualOffset + origin * uSize.xy;

  vertexPosition = uMvpMatrix * vertexPosition;

  vTexCoord = (fixedFactor + stretch * uStretchTotal) / (fixedTotal + uStretchTotal);

  vMaskTexCoord = gridPosition.xy / size;
  gl_Position = vertexPosition;
}
