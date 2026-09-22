//@name text-visual-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
OUTPUT highp vec2 vTexCoord;

#ifdef TEXT_REVEAL_SOURCE_ATLAS
INPUT highp vec4 aSourceCrop;
INPUT highp vec2 aSourceOffset;
INPUT highp vec4 aSourceAtlas;
OUTPUT highp vec4 vSourceAtlas;
OUTPUT highp vec4 vSourceCrop;
#endif

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
  UNIFORM highp float viewEffectiveScale;
  UNIFORM highp vec3 uScale;
  UNIFORM highp float pixelSnapFactor;
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

vec4 ComputeVertexPosition()
{
#ifdef TEXT_REVEAL_SOURCE_ATLAS
  // Match CloneForeground's absolute crop transform. Capture owns scale 1;
  // preserve the original control's uSize/origin and add only page placement.
  vec2 original = size * (uSize.xy * (vec2(1.0) - offsetSizeMode.zw) + offsetSizeMode.zw) + extraSize;
  vec2 croppedSize = original * aSourceCrop.zw;
  vec2 croppedOffset = offset * (uSize.xy * (vec2(1.0) - offsetSizeMode.xy) + offsetSizeMode.xy)
                     + (aSourceCrop.xy + pivot - vec2(0.5)) * original + aSourceOffset;
  vec4 result = vec4((aPosition + vec2(0.5)) * croppedSize + croppedOffset + origin * uSize.xy, 0.0, 1.0);
#else
  highp float effectiveScale = mix(1.0, viewEffectiveScale, visualTransformUseEffectiveScale);
  vec2 visualSize = mix(size * uSize.xy, size * effectiveScale, offsetSizeMode.zw ) + extraSize * effectiveScale;
  vec2 visualOffset = mix(offset * uSize.xy, offset * effectiveScale, offsetSizeMode.xy);
  vec4 result = vec4( (aPosition + pivot) * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0 );
#endif

  vec2 snappedPosition = result.xy;
  snappedPosition.x = floor(snappedPosition.x * uScale.x + 0.5) / uScale.x;
  snappedPosition.y = floor(snappedPosition.y * uScale.y + 0.5) / uScale.y;

  snappedPosition.x = snappedPosition.x + (1.0 - abs(mod(uSize.x, 2.0) - 1.0)) * 0.5;
  snappedPosition.y = snappedPosition.y + (1.0 - abs(mod(uSize.y, 2.0) - 1.0)) * 0.5;

  result.xy = mix(result.xy, snappedPosition, pixelSnapFactor);

  return result;
}

void main()
{
  vTexCoord = aPosition + vec2(0.5);
#ifdef TEXT_REVEAL_SOURCE_ATLAS
  vSourceAtlas = aSourceAtlas;
  vSourceCrop = aSourceCrop;
#endif
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}
