//@name text-atlas-gradient-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
INPUT highp vec2 aTexCoord;
INPUT highp vec4 aColor;
INPUT highp float aGradientFill;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp vec2 uOffset;
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec2 uTextGradientLayoutSize;
};

OUTPUT highp vec2 vTexCoord;
OUTPUT highp vec4 vColor;
OUTPUT highp vec2 vTextGradientCoord;
OUTPUT highp float vGradientFill;

void main()
{
  highp vec4 position = vec4(aPosition.xy + uOffset, 0.0, 1.0);
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
  vColor = aColor;
  vTextGradientCoord = (aPosition + 0.5 * uTextGradientLayoutSize) /
                       max(uTextGradientLayoutSize, vec2(0.000001));
  vGradientFill = aGradientFill;
}
