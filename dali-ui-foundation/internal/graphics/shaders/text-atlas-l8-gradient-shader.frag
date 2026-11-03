//@name text-atlas-l8-gradient-shader.frag

//@version 100

precision highp float;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM lowp vec4 textColorAnimatable;
  UNIFORM highp float uTextGradientEvaluationMode;
  UNIFORM highp vec2 uTextGradientStartPosition;
  UNIFORM highp vec2 uTextGradientEndPosition;
  UNIFORM highp vec2 uTextGradientRadialCenter;
  UNIFORM highp vec2 uTextGradientEvaluationScale;
  UNIFORM highp vec2 uTextGradientConicCenter;
  UNIFORM highp vec2 uTextGradientConicScale;
  UNIFORM highp float uTextGradientConicStartAngle;
  UNIFORM highp float uTextGradientStartOffset;
  UNIFORM highp vec4 uTextGradientBounds;
};

UNIFORM sampler2D sTexture;
UNIFORM sampler2D sGradientLookup;
INPUT highp vec2 vTexCoord;
INPUT highp vec4 vColor;
INPUT highp vec2 vTextGradientCoord;
INPUT highp float vGradientFill;

highp float EvaluateTextGradientPosition(highp vec2 coord)
{
  const highp float EVALUATION_RADIAL = 2.0;
  const highp float EVALUATION_CONIC  = 3.0;
  const highp float INV_TWO_PI = 0.15915494309189533577;
  if(abs(uTextGradientEvaluationMode - EVALUATION_RADIAL) < 0.5)
  {
    return length((coord - uTextGradientRadialCenter) * uTextGradientEvaluationScale);
  }
  // Conic and authored-space linear evaluations share this dispatch.
  // Normalized linear evaluation keeps its original projection and branch count.
  if(uTextGradientEvaluationMode > EVALUATION_CONIC - 0.5)
  {
    if(uTextGradientEvaluationMode > EVALUATION_CONIC + 0.5)
    {
      return dot(coord - uTextGradientStartPosition, uTextGradientEvaluationScale);
    }
    highp vec2 vector = (coord - uTextGradientConicCenter) * uTextGradientConicScale;
    return fract((atan(vector.y, vector.x) - uTextGradientConicStartAngle) * INV_TWO_PI);
  }

  highp vec2 vector = uTextGradientEndPosition - uTextGradientStartPosition;
  return dot(coord - uTextGradientStartPosition, vector) /
         max(dot(vector, vector), 0.000001);
}

void main()
{
  mediump float coverage = TEXTURE(sTexture, vTexCoord).r;
  if(vGradientFill > 0.5)
  {
    highp vec2 coord = (vTextGradientCoord - uTextGradientBounds.xy) /
                       max(uTextGradientBounds.zw, vec2(0.000001));
    highp float position = EvaluateTextGradientPosition(coord) + uTextGradientStartOffset;
    mediump vec4 gradient = TEXTURE(sGradientLookup, vec2(position, 0.5));

    // Gradient lookup RGB is premultiplied. Atlas L8 output is straight alpha,
    // so recover straight RGB independently of the default foreground color.
    mediump vec3 straightRgb = gradient.a > 0.000001 ? gradient.rgb / gradient.a : vec3(0.0);
    gl_FragColor = vec4(straightRgb * uColor.rgb,
                        uColor.a * gradient.a * coverage);
  }
  else
  {
    gl_FragColor = vec4(vColor.rgb * uColor.rgb * textColorAnimatable.rgb,
                        uColor.a * vColor.a * textColorAnimatable.a * coverage);
  }
}
