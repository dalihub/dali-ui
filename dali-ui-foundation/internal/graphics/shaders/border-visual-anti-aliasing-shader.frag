//@name border-visual-anti-aliasing-shader.frag

//@version 100

precision highp float;

INPUT highp float vAlpha;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM lowp vec4 borderColor;
  UNIFORM highp float borderSize;
  UNIFORM highp float viewEffectiveScale;
};

void main()
{
  gl_FragColor = borderColor * uColor;
  gl_FragColor.a *= smoothstep(0.0, 1.5, vAlpha) * smoothstep( borderSize * viewEffectiveScale + 1.5, borderSize * viewEffectiveScale, vAlpha );
}