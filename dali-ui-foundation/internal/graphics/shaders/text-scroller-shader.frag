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
  UNIFORM highp float uTextGradientStartOffset;
  UNIFORM highp vec4 uTextGradientBounds;
#endif
};

void main()
{
  if ( vTexCoord.y > 1.0 )
    discard;

  mediump vec4 textTexture = TEXTURE( sTexture, vTexCoord );

#ifdef IS_REQUIRED_TEXT_GRADIENT
  highp vec2 textGradientCoord =
    (vTextGradientCoord - uTextGradientBounds.xy) / max(uTextGradientBounds.zw, vec2(0.000001));
  highp vec2 gradientVector = uTextGradientEndPosition - uTextGradientStartPosition;
  highp float gradientLengthSquared = max(dot(gradientVector, gradientVector), 0.000001);
  highp float gradientPosition =
    dot(textGradientCoord - uTextGradientStartPosition, gradientVector) / gradientLengthSquared;
  mediump vec4 gradientColor = TEXTURE(sGradientLookup, vec2(gradientPosition + uTextGradientStartOffset, 0.5));
  textTexture = vec4(gradientColor.rgb * textTexture.a, gradientColor.a * textTexture.a);
#endif

  gl_FragColor = textTexture * uColor;
}
