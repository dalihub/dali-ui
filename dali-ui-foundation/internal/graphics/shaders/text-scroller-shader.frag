//@name text-scroller-shader.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
#ifdef IS_REQUIRED_TEXT_GRADIENT
INPUT highp vec2 vTextGradientCoord;
UNIFORM sampler2D sGradientLookup;
#endif

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
#ifdef IS_REQUIRED_TEXT_GRADIENT
  UNIFORM highp vec2 uTextGradientStartPosition;
  UNIFORM highp vec2 uTextGradientEndPosition;
  UNIFORM highp float uTextGradientType;
  UNIFORM highp vec2 uTextGradientRadialCenter;
  UNIFORM highp vec2 uTextGradientRadialScale;
  UNIFORM highp vec2 uTextGradientConicCenter;
  UNIFORM highp vec2 uTextGradientConicScale;
  UNIFORM highp float uTextGradientConicStartAngle;
  UNIFORM highp float uTextGradientStartOffset;
  UNIFORM highp vec4 uTextGradientBounds;
#endif
};

#ifdef IS_REQUIRED_TEXT_GRADIENT
highp float EvaluateTextGradientPosition(highp vec2 textGradientCoord)
{
  const highp float TEXT_GRADIENT_TYPE_RADIAL = 2.0;
  const highp float TEXT_GRADIENT_TYPE_CONIC = 3.0;
  const highp float TEXT_GRADIENT_INV_TWO_PI = 0.15915494309189533576888376337251;
  if(abs(uTextGradientType - TEXT_GRADIENT_TYPE_RADIAL) < 0.5)
  {
    return length((textGradientCoord - uTextGradientRadialCenter) * uTextGradientRadialScale);
  }
  if(abs(uTextGradientType - TEXT_GRADIENT_TYPE_CONIC) < 0.5)
  {
    highp vec2 conicVector = (textGradientCoord - uTextGradientConicCenter) * uTextGradientConicScale;
    highp float angle = atan(conicVector.y, conicVector.x) - uTextGradientConicStartAngle;
    return fract(angle * TEXT_GRADIENT_INV_TWO_PI);
  }

  highp vec2 gradientVector = uTextGradientEndPosition - uTextGradientStartPosition;
  highp float gradientLengthSquared = max(dot(gradientVector, gradientVector), 0.000001);
  return dot(textGradientCoord - uTextGradientStartPosition, gradientVector) / gradientLengthSquared;
}
#endif

void main()
{
  if ( vTexCoord.y > 1.0 )
    discard;

  mediump vec4 textTexture = TEXTURE( sTexture, vTexCoord );

#ifdef IS_REQUIRED_TEXT_GRADIENT
  highp vec2 textGradientCoord =
    (vTextGradientCoord - uTextGradientBounds.xy) / max(uTextGradientBounds.zw, vec2(0.000001));
  highp float gradientPosition = EvaluateTextGradientPosition(textGradientCoord);
  mediump vec4 gradientColor = TEXTURE(sGradientLookup, vec2(gradientPosition + uTextGradientStartOffset, 0.5));
  textTexture = vec4(gradientColor.rgb * textTexture.a, gradientColor.a * textTexture.a);
#endif

  gl_FragColor = textTexture * uColor;
}
