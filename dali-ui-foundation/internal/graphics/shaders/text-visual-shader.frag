//@name text-visual-shader.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
#ifdef IS_REQUIRED_TEXT_GRADIENT_MIXED
UNIFORM sampler2D sTextGradientMask;
#endif
#ifdef IS_REQUIRED_TEXT_GRADIENT
UNIFORM sampler2D sGradientLookup;
#endif
#ifdef IS_REQUIRED_STYLE
UNIFORM sampler2D sStyle;
#endif
#ifdef IS_REQUIRED_OVERLAY
UNIFORM sampler2D sOverlayStyle;
#endif

#ifndef IS_REQUIRED_TEXT_GRADIENT
  #ifdef IS_REQUIRED_MULTI_COLOR
  #elif defined(IS_REQUIRED_EMOJI)
  // Single color with emoji.
  UNIFORM sampler2D sMask;
  #endif
#endif

UNIFORM_BLOCK FragBlock
{
  #ifndef IS_REQUIRED_TEXT_GRADIENT
    #ifdef IS_REQUIRED_MULTI_COLOR
    #elif defined(IS_REQUIRED_EMOJI)
    // Single color with emoji.
    UNIFORM lowp float uHasMultipleTextColors;
    #endif
  #endif
  UNIFORM lowp vec4 uTextColorAnimatable;
  UNIFORM lowp vec4 uColor;

  #ifdef IS_REQUIRED_TEXT_GRADIENT
  UNIFORM highp float uTextGradientType;
  UNIFORM highp vec2 uTextGradientStartPosition;
  UNIFORM highp vec2 uTextGradientEndPosition;
  UNIFORM highp vec2 uTextGradientRadialCenter;
  UNIFORM highp vec2 uTextGradientRadialScale;
  UNIFORM highp float uTextGradientStartOffset;
  UNIFORM highp vec4 uTextGradientBounds;
  #endif

  #ifdef IS_REQUIRED_EMBOSS
  UNIFORM lowp vec2 uEmbossSize;
  UNIFORM lowp vec2 uEmbossDirection;
  UNIFORM lowp float uEmbossStrength;
  UNIFORM lowp vec4 uEmbossLightColor;
  UNIFORM lowp vec4 uEmbossShadowColor;
  #endif
};

#ifdef IS_REQUIRED_TEXT_GRADIENT
highp float EvaluateTextGradientPosition(highp vec2 textGradientCoord)
{
  const highp float TEXT_GRADIENT_TYPE_RADIAL = 2.0;
  if(abs(uTextGradientType - TEXT_GRADIENT_TYPE_RADIAL) < 0.5)
  {
    return length((textGradientCoord - uTextGradientRadialCenter) * uTextGradientRadialScale);
  }

  highp vec2 gradientVector = uTextGradientEndPosition - uTextGradientStartPosition;
  highp float gradientLengthSquared = max(dot(gradientVector, gradientVector), 0.000001);
  return dot(textGradientCoord - uTextGradientStartPosition, gradientVector) / gradientLengthSquared;
}
#endif

void main()
{
#ifdef IS_REQUIRED_STYLE
  mediump vec4 styleTexture = TEXTURE( sStyle, vTexCoord );
#endif

#ifdef IS_REQUIRED_OVERLAY
  mediump vec4 overlayStyleTexture = TEXTURE( sOverlayStyle, vTexCoord );
#endif

  mediump vec4 textColor;
#ifdef IS_REQUIRED_TEXT_GRADIENT_MIXED
  mediump vec4 preservedColor = TEXTURE(sTexture, vTexCoord);
  mediump float textTexture = TEXTURE(sTextGradientMask, vTexCoord).r;
  highp vec2 textGradientCoord =
    (vTexCoord - uTextGradientBounds.xy) / max(uTextGradientBounds.zw, vec2(0.000001));
  highp float gradientPosition = EvaluateTextGradientPosition(textGradientCoord);
  mediump vec4 gradientColor = TEXTURE(sGradientLookup, vec2(gradientPosition + uTextGradientStartOffset, 0.5));
  mediump vec4 gradientFill = vec4(gradientColor.rgb * textTexture,
                                   gradientColor.a * textTexture * uTextColorAnimatable.a);
  textColor = gradientFill + preservedColor * (1.0 - gradientFill.a);
#elif defined(IS_REQUIRED_TEXT_GRADIENT)
  mediump float textTexture = TEXTURE(sTexture, vTexCoord).r;
  highp vec2 textGradientCoord =
    (vTexCoord - uTextGradientBounds.xy) / max(uTextGradientBounds.zw, vec2(0.000001));
  highp float gradientPosition = EvaluateTextGradientPosition(textGradientCoord);
  mediump vec4 gradientColor = TEXTURE(sGradientLookup, vec2(gradientPosition + uTextGradientStartOffset, 0.5));
  textColor = vec4(gradientColor.rgb * textTexture, gradientColor.a * textTexture * uTextColorAnimatable.a);
#elif defined(IS_REQUIRED_MULTI_COLOR) || defined(IS_REQUIRED_EMOJI)
  // Multiple color or use emoji.
  textColor = TEXTURE(sTexture, vTexCoord);
#ifdef IS_REQUIRED_EMBOSS
  // Multiple color or use emoji, with emboss
  mediump float textAlpha = textColor.a;
  mediump vec2 offset = normalize(uEmbossDirection) * uEmbossSize * uEmbossStrength;
  float light = TEXTURE(sTexture, clamp(vTexCoord - offset, 0.0, 1.0)).a;
  float shadow = TEXTURE(sTexture, clamp(vTexCoord + offset, 0.0, 1.0)).a;

  vec4 lightColor = vec4(uEmbossLightColor.rgb * light, light);
  vec4 shadowColor = vec4(uEmbossShadowColor.rgb * shadow, shadow);
  vec4 embossColor = lightColor + shadowColor;

  vec4 baseColor = vec4(uTextColorAnimatable.rgb * textAlpha, textAlpha);
  textColor = embossColor * (1.0 - textAlpha) + baseColor;
  textColor.a = max(textAlpha, embossColor.a);
#endif
#endif

#ifdef IS_REQUIRED_TEXT_GRADIENT
#elif defined(IS_REQUIRED_MULTI_COLOR)
#elif defined(IS_REQUIRED_EMOJI)
  // Single color with emoji.
  mediump float maskTexture = TEXTURE(sMask, vTexCoord).r;

  // Set the color of non-transparent pixel in text to what it is animated to.
  // Markup text with multiple text colors are not animated (but can be supported later on if required).
  // Emoji color are not animated.
  mediump float vstep = step( 0.0001, textColor.a );
  textColor.rgb = mix(textColor.rgb, uTextColorAnimatable.rgb, vstep * maskTexture * (1.0 - uHasMultipleTextColors));
#elif defined(IS_REQUIRED_EMBOSS)
// Single color with emboss, without emoji.
  mediump float textAlpha = TEXTURE(sTexture, vTexCoord).r;
  mediump vec2 offset = normalize(uEmbossDirection) * uEmbossSize * uEmbossStrength;
  float light = TEXTURE(sTexture, clamp(vTexCoord - offset, 0.0, 1.0)).r;
  float shadow = TEXTURE(sTexture, clamp(vTexCoord + offset, 0.0, 1.0)).r;

  vec4 lightColor = vec4(uEmbossLightColor.rgb * light, light);
  vec4 shadowColor = vec4(uEmbossShadowColor.rgb * shadow, shadow);
  vec4 embossColor = lightColor + shadowColor;

  vec4 baseColor = vec4(uTextColorAnimatable.rgb * textAlpha, textAlpha);
  textColor = embossColor * (1.0 - textAlpha) + baseColor;
  textColor.a = max(textAlpha, embossColor.a);

#else
  // Single color without emoji.
  mediump float textTexture = TEXTURE(sTexture, vTexCoord).r;
  textColor = uTextColorAnimatable * textTexture;
#endif

  // Draw the text as overlay above the style
  gl_FragColor = uColor * (
#ifdef IS_REQUIRED_OVERLAY
                   (
#endif
                     textColor
#ifdef IS_REQUIRED_STYLE
                     + styleTexture * (1.0 - textColor.a)
#endif
#ifdef IS_REQUIRED_OVERLAY
                   ) * (1.0 - overlayStyleTexture.a) + overlayStyleTexture
#endif
                 );
}
