//@name text-cutout-shader.frag
//@version 100
precision highp float;
INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM sampler2D sStyle;
#ifdef CUTOUT_OVERLAY
UNIFORM sampler2D sOverlayStyle;
#endif
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM lowp vec4 uTextColorAnimatable;
  UNIFORM highp vec2 uCutoutControlSize;
  UNIFORM highp vec4 uCutoutRect;
  UNIFORM highp vec4 uCutoutBackground;
};
void main()
{
  highp vec2 uv = (vTexCoord * uCutoutControlSize - uCutoutRect.xy) / uCutoutRect.zw;
  highp float inside = step(0.0, uv.x) * step(0.0, uv.y) * step(uv.x, 1.0) * step(uv.y, 1.0);
#ifdef CUTOUT_COLOR_GLYPHS
  highp vec4 glyph = TEXTURE(sTexture, uv) * inside;
  highp float coverage = glyph.a;
  highp vec4 fill = glyph * uTextColorAnimatable.a;
#else
  highp float coverage = TEXTURE(sTexture, uv).r * inside;
  highp vec4 fill = uTextColorAnimatable * coverage;
#endif
  highp vec4 style = TEXTURE(sStyle, uv) * inside;
  highp vec4 underlay = style + uCutoutBackground * (1.0 - style.a);
  highp vec4 color = fill + underlay * (1.0 - coverage);
#ifdef CUTOUT_OVERLAY
  highp vec4 overlay = TEXTURE(sOverlayStyle, uv) * inside;
  color = overlay + color * (1.0 - overlay.a);
#endif
  gl_FragColor = uColor * color;
}
