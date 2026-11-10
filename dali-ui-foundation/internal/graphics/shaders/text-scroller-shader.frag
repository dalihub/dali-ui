//@name text-scroller-shader.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
#if defined(IS_REQUIRED_TEXT_GRADIENT) || defined(IS_REQUIRED_TEXT_GRADIENT_OVERLAY)
INPUT highp vec2 vTextGradientCoord;
#endif
#ifdef IS_REQUIRED_TEXT_GRADIENT_MIXED
UNIFORM sampler2D sTextGradientMask;
#endif
#ifdef IS_REQUIRED_TEXT_GRADIENT
UNIFORM sampler2D sGradientLookup;
#endif
#ifdef IS_REQUIRED_TEXT_GRADIENT_OVERLAY
UNIFORM sampler2D sGradientOverlayLookup;
#endif
#ifdef IS_REQUIRED_TEXT_STYLE
UNIFORM sampler2D sStyle;
#endif
#ifdef IS_REQUIRED_TEXT_OVERLAY_STYLE
UNIFORM sampler2D sOverlayStyle;
#endif

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
#ifdef IS_REQUIRED_TEXT_GRADIENT
  UNIFORM highp vec2 uTextGradientStartPosition;
  UNIFORM highp vec2 uTextGradientEndPosition;
  UNIFORM highp float uTextGradientEvaluationMode;
  UNIFORM highp vec2 uTextGradientRadialCenter;
  UNIFORM highp vec2 uTextGradientEvaluationScale;
  UNIFORM highp vec2 uTextGradientConicCenter;
  UNIFORM highp vec2 uTextGradientConicScale;
  UNIFORM highp float uTextGradientConicStartAngle;
  UNIFORM highp float uTextGradientStartOffset;
  UNIFORM highp vec4 uTextGradientBounds;
#endif
#ifdef IS_REQUIRED_TEXT_GRADIENT_OVERLAY
  UNIFORM highp vec2 uTextGradientOverlayStartPosition;
  UNIFORM highp vec2 uTextGradientOverlayEndPosition;
  UNIFORM highp float uTextGradientOverlayEvaluationMode;
  UNIFORM highp vec2 uTextGradientOverlayRadialCenter;
  UNIFORM highp vec2 uTextGradientOverlayEvaluationScale;
  UNIFORM highp vec2 uTextGradientOverlayConicCenter;
  UNIFORM highp vec2 uTextGradientOverlayConicScale;
  UNIFORM highp float uTextGradientOverlayConicStartAngle;
  UNIFORM highp float uTextGradientOverlayStartOffset;
  UNIFORM highp vec4 uTextGradientOverlayBounds;
  UNIFORM highp float uTextGradientOverlayMode;
#endif
};

#ifdef IS_REQUIRED_TEXT_GRADIENT
highp vec2 GetTextGradientCoordinate()
{
#ifdef IS_REQUIRED_TEXT_GRADIENT_CONTENT_BOUND
  return vec2(fract(vTexCoord.x), vTexCoord.y);
#else
  return vTextGradientCoord;
#endif
}

highp float EvaluateTextGradientPosition(highp vec2 textGradientCoord)
{
  const highp float TEXT_GRADIENT_EVALUATION_RADIAL = 2.0;
  const highp float TEXT_GRADIENT_EVALUATION_CONIC  = 3.0;
  const highp float TEXT_GRADIENT_INV_TWO_PI = 0.15915494309189533576888376337251;
  if(abs(uTextGradientEvaluationMode - TEXT_GRADIENT_EVALUATION_RADIAL) < 0.5)
  {
    return length((textGradientCoord - uTextGradientRadialCenter) * uTextGradientEvaluationScale);
  }
  // Conic and authored-space linear evaluations share this dispatch.
  // Normalized linear evaluation keeps its original projection and branch count.
  if(uTextGradientEvaluationMode > TEXT_GRADIENT_EVALUATION_CONIC - 0.5)
  {
    if(uTextGradientEvaluationMode > TEXT_GRADIENT_EVALUATION_CONIC + 0.5)
    {
      return dot(textGradientCoord - uTextGradientStartPosition, uTextGradientEvaluationScale);
    }
    highp vec2 conicVector = (textGradientCoord - uTextGradientConicCenter) * uTextGradientConicScale;
    highp float angle = atan(conicVector.y, conicVector.x) - uTextGradientConicStartAngle;
    return fract(angle * TEXT_GRADIENT_INV_TWO_PI);
  }

  highp vec2 gradientVector = uTextGradientEndPosition - uTextGradientStartPosition;
  highp float gradientLengthSquared = max(dot(gradientVector, gradientVector), 0.000001);
  return dot(textGradientCoord - uTextGradientStartPosition, gradientVector) / gradientLengthSquared;
}
#endif

#ifdef IS_REQUIRED_TEXT_GRADIENT_OVERLAY
highp vec2 GetTextGradientOverlayCoordinate()
{
#ifdef IS_REQUIRED_TEXT_GRADIENT_OVERLAY_CONTENT_BOUND
  return vec2(fract(vTexCoord.x), vTexCoord.y);
#else
  return vTextGradientCoord;
#endif
}

highp float EvaluateTextGradientOverlayPosition(highp vec2 textGradientCoord)
{
  const highp float TEXT_GRADIENT_EVALUATION_RADIAL = 2.0;
  const highp float TEXT_GRADIENT_EVALUATION_CONIC  = 3.0;
  const highp float TEXT_GRADIENT_INV_TWO_PI = 0.15915494309189533576888376337251;
  if(abs(uTextGradientOverlayEvaluationMode - TEXT_GRADIENT_EVALUATION_RADIAL) < 0.5)
  {
    return length((textGradientCoord - uTextGradientOverlayRadialCenter) * uTextGradientOverlayEvaluationScale);
  }
  // Conic and authored-space linear evaluations share this dispatch.
  // Normalized linear evaluation keeps its original projection and branch count.
  if(uTextGradientOverlayEvaluationMode > TEXT_GRADIENT_EVALUATION_CONIC - 0.5)
  {
    if(uTextGradientOverlayEvaluationMode > TEXT_GRADIENT_EVALUATION_CONIC + 0.5)
    {
      return dot(textGradientCoord - uTextGradientOverlayStartPosition, uTextGradientOverlayEvaluationScale);
    }
    highp vec2 conicVector = (textGradientCoord - uTextGradientOverlayConicCenter) * uTextGradientOverlayConicScale;
    highp float angle = atan(conicVector.y, conicVector.x) - uTextGradientOverlayConicStartAngle;
    return fract(angle * TEXT_GRADIENT_INV_TWO_PI);
  }

  highp vec2 gradientVector = uTextGradientOverlayEndPosition - uTextGradientOverlayStartPosition;
  highp float gradientLengthSquared = max(dot(gradientVector, gradientVector), 0.000001);
  return dot(textGradientCoord - uTextGradientOverlayStartPosition, gradientVector) / gradientLengthSquared;
}

mediump vec4 ApplyTextGradientOverlay(mediump vec4 baseFill)
{
  mediump float foregroundAlpha = baseFill.a;
  if(foregroundAlpha <= 0.0)
  {
    return baseFill;
  }

  highp vec2 textGradientOverlayCoord =
    (GetTextGradientOverlayCoordinate() - uTextGradientOverlayBounds.xy) / max(uTextGradientOverlayBounds.zw, vec2(0.000001));
  highp float gradientPosition = EvaluateTextGradientOverlayPosition(textGradientOverlayCoord);
  mediump vec4 overlayColor =
    TEXTURE(sGradientOverlayLookup, vec2(gradientPosition + uTextGradientOverlayStartOffset, 0.5));
  // Both inputs are premultiplied. Overlay changes color within the existing
  // foreground alpha; it does not add glyph coverage or increase opacity.
  mediump vec3 blendedRgb;
  const highp float TEXT_GRADIENT_OVERLAY_MODE_SCREEN = 1.0;
  if(abs(uTextGradientOverlayMode - TEXT_GRADIENT_OVERLAY_MODE_SCREEN) < 0.5)
  {
    blendedRgb = baseFill.rgb + overlayColor.rgb * (foregroundAlpha - baseFill.rgb);
  }
  else
  {
    blendedRgb = overlayColor.rgb * foregroundAlpha + baseFill.rgb * (1.0 - overlayColor.a);
  }
  return vec4(blendedRgb, foregroundAlpha);
}
#endif

void main()
{
  if ( vTexCoord.y > 1.0 )
    discard;

  mediump vec4 textTexture = TEXTURE( sTexture, vTexCoord );

#ifdef IS_REQUIRED_TEXT_GRADIENT_MIXED
  mediump vec4 preservedColor = textTexture;
  mediump float textGradientMask = TEXTURE(sTextGradientMask, vTexCoord).r;
  highp vec2 textGradientCoord =
    (GetTextGradientCoordinate() - uTextGradientBounds.xy) / max(uTextGradientBounds.zw, vec2(0.000001));
  highp float gradientPosition = EvaluateTextGradientPosition(textGradientCoord);
  mediump vec4 gradientColor = TEXTURE(sGradientLookup, vec2(gradientPosition + uTextGradientStartOffset, 0.5));
  mediump vec4 gradientFill = vec4(gradientColor.rgb * textGradientMask, gradientColor.a * textGradientMask);
  textTexture = gradientFill + preservedColor * (1.0 - gradientFill.a);
#elif defined(IS_REQUIRED_TEXT_GRADIENT)
  highp vec2 textGradientCoord =
    (GetTextGradientCoordinate() - uTextGradientBounds.xy) / max(uTextGradientBounds.zw, vec2(0.000001));
  highp float gradientPosition = EvaluateTextGradientPosition(textGradientCoord);
  mediump vec4 gradientColor = TEXTURE(sGradientLookup, vec2(gradientPosition + uTextGradientStartOffset, 0.5));
  textTexture = vec4(gradientColor.rgb * textTexture.a, gradientColor.a * textTexture.a);
#endif

#ifdef IS_REQUIRED_TEXT_GRADIENT_OVERLAY
  textTexture = ApplyTextGradientOverlay(textTexture);
#endif

#ifdef IS_REQUIRED_TEXT_STYLE
  mediump vec4 styleTexture = TEXTURE(sStyle, vTexCoord);
  textTexture = textTexture + styleTexture * (1.0 - textTexture.a);
#endif

#ifdef IS_REQUIRED_TEXT_OVERLAY_STYLE
  mediump vec4 overlayStyleTexture = TEXTURE(sOverlayStyle, vTexCoord);
  textTexture = textTexture * (1.0 - overlayStyleTexture.a) + overlayStyleTexture;
#endif

  gl_FragColor = textTexture * uColor;
}
