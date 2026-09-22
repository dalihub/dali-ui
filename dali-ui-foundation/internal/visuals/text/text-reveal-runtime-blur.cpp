/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali-ui-foundation/internal/visuals/text/text-reveal-runtime-blur.h>

#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-data.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/views/view/view-renderers.h>
#include <dali-ui-foundation/internal/visuals/text/text-reveal-blur-renderer.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali/devel-api/adaptor-framework/graphics-backend.h>
#include <dali/devel-api/adaptor-framework/image-loading-devel.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/rendering/visual-renderer.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/actors/custom-actor.h>
#include <dali/public-api/animation/constraint-source.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/vertex-buffer.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>

namespace DALI_NAMESPACE::Ui::Internal
{
namespace
{
// H preserves Y until the vertical filter runs. Source remains full resolution
// for the final sharp handoff; this policy does not change the Reveal schedule.
constexpr float QUARTER_BLUR_SCALE = 0.25f;

struct BlurStrength
{
  float start;
  float duration;

  float Evaluate(float progress) const
  {
    const float p = Ui::Text::Internal::Reveal::ResolveRenderProgress(progress);
    const float q = p >= 1.0f ? 1.0f : std::clamp((p - start) / duration, 0.0f, 1.0f);
    return 1.0f - q * q * (3.0f - 2.0f * q);
  }

  void operator()(float& value, const PropertyInputContainer& inputs)
  {
    value = Evaluate(inputs[0]->GetFloat());
  }
};

template<typename T>
void MirrorProperty(Renderer target, Property::Index targetIndex, Renderer source, Property::Index sourceIndex)
{
  if(!Dali::Adaptor::IsAvailable())
  {
    return;
  }
  auto constraint = Constraint::New<T>(target, targetIndex, [](T& value, const PropertyInputContainer& inputs)
  {
    if constexpr(std::is_same_v<T, float>)
    {
      value = inputs[0]->GetFloat();
    }
    else if constexpr(std::is_same_v<T, Vector2>)
    {
      value = inputs[0]->GetVector2();
    }
    else if constexpr(std::is_same_v<T, Vector3>)
    {
      value = inputs[0]->GetVector3();
    }
    else if constexpr(std::is_same_v<T, Vector4>)
    {
      value = inputs[0]->GetVector4();
    }
  });
  constraint.AddSource(Source(source, sourceIndex));
  constraint.Apply();
}

/**
 * @brief Reuses immutable renderer property descriptions during one publication.
 *
 * Values and types remain live reads: object-created and property-set callbacks
 * may modify the source while line renderers are constructed. Renderer property
 * names and access modes do not change; newly registered properties invalidate
 * this local list before the next clone, after its object-created callback.
 */
struct ForegroundProperties
{
  struct Entry
  {
    Property::Index index;
    Dali::String    name;
    bool            writable;
    bool            constraintInput;
  };

  void Prepare(Renderer source)
  {
    const auto count = source.GetPropertyCount();
    if(count != entries.size())
    {
      Property::IndexContainer indices;
      source.GetPropertyIndices(indices);
      entries.clear();
      entries.reserve(indices.Size());
      for(auto index : indices)
      {
        entries.push_back({index, source.GetPropertyName(index), source.IsPropertyWritable(index), source.IsPropertyAConstraintInput(index)});
      }
    }
  }

  std::vector<Entry> entries;
};

Renderer CloneForeground(Renderer source, ForegroundProperties& properties, TextureSet textures, const Vector4& rectangle, const Vector2& controlSize,
                         const Vector2& captureOffset = Vector2::ZERO)
{
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  using P = VisualRenderer::Property;

  const bool cropped  = rectangle != Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  const bool absolute = cropped || captureOffset != Vector2::ZERO;
  Geometry   geometry = source.GetGeometry();
  Shader     shader   = source.GetShader();
  Renderer   target   = VisualRenderer::New(geometry, shader);
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  properties.Prepare(source);
  for(const auto& property : properties.entries)
  {
    const auto index = property.index;
    if(!property.writable)
    {
      continue;
    }
    if(absolute && ((index >= P::TRANSFORM_OFFSET && index <= P::EXTRA_SIZE_HEIGHT) ||
                    (index >= P::TRANSFORM_PIVOT && index <= P::TRANSFORM_SIZE_HEIGHT_POLICY)))
    {
      continue;
    }
    const auto& name = property.name;
    // RegisterProperty also sets an existing property; avoid looking up each
    // custom name twice. Keep target lookup live for object-created callbacks.
    const auto targetIndex = target.RegisterProperty(name, source.GetProperty(index));
    if(!Dali::Adaptor::IsAvailable())
    {
      return {};
    }
    if(property.constraintInput && target.IsPropertyAnimatable(targetIndex))
    {
      // Keep the original renderer's color, gradient and Reveal bindings as
      // the authority. Update-side dependencies also cover reverse and seeks.
      switch(source.GetPropertyType(index))
      {
        case Property::FLOAT:
          MirrorProperty<float>(target, targetIndex, source, index);
          break;
        case Property::VECTOR2:
          MirrorProperty<Vector2>(target, targetIndex, source, index);
          break;
        case Property::VECTOR3:
          MirrorProperty<Vector3>(target, targetIndex, source, index);
          break;
        case Property::VECTOR4:
          if(cropped && (name == "uTextGradientBounds" || name == "uTextGradientOverlayBounds"))
          {
            auto remap = [rectangle](Vector4& value, const PropertyInputContainer& inputs)
            {
              const auto& bounds = inputs[0]->GetVector4();
              value              = Vector4((bounds.x - rectangle.x) / rectangle.z, (bounds.y - rectangle.y) / rectangle.w,
                                           bounds.z / rectangle.z, bounds.w / rectangle.w);
            };
            auto constraint = Constraint::New<Vector4>(target, targetIndex, remap);
            constraint.AddSource(Source(source, index));
            constraint.Apply();
            break;
          }
          MirrorProperty<Vector4>(target, targetIndex, source, index);
          break;
        default:
          break;
      }
    }
    if(!Dali::Adaptor::IsAvailable())
    {
      return {};
    }
  }
  if(absolute)
  {
    // Retain the ordinary shader and unit quad. Resolve the original transform
    // into absolute cropped coordinates; large pivot values would lose mediump
    // precision on long paragraphs. Gradient bounds are remapped above, while
    // PIXEL metadata keeps its original encoded per-pixel schedule unchanged.
    target.SetProperty(P::TRANSFORM_OFFSET_SIZE_MODE, Vector4::ONE);
    target.SetProperty(P::TRANSFORM_PIVOT, Vector2(0.5f, 0.5f));
    target.SetProperty(P::EXTRA_SIZE, Vector2::ZERO);
    target.RegisterProperty("runtimeBlurTextureRect", rectangle);
    if(!Dali::Adaptor::IsAvailable())
    {
      return {};
    }
    // These two policies are event-side properties, not constraint inputs.
    // Layout changes rebuild this companion with the new transform policy.
    const auto mode   = source.GetProperty<Vector4>(P::TRANSFORM_OFFSET_SIZE_MODE);
    const auto pivot  = source.GetProperty<Vector2>(P::TRANSFORM_PIVOT);
    auto       extent = [controlSize, mode](const PropertyInputContainer& inputs)
    {
      const auto& size  = inputs[0]->GetVector2();
      const auto& extra = inputs[1]->GetVector2();
      return Vector2(size.x * (controlSize.x * (1.0f - mode.z) + mode.z) + extra.x,
                     size.y * (controlSize.y * (1.0f - mode.w) + mode.w) + extra.y);
    };
    auto size = Constraint::New<Vector2>(target, P::TRANSFORM_SIZE, [rectangle, extent](Vector2& value, const PropertyInputContainer& inputs)
    {
      const auto original = extent(inputs);
      value               = Vector2(original.x * rectangle.z, original.y * rectangle.w);
    });
    if(!Dali::Adaptor::IsAvailable())
    {
      return {};
    }
    auto offset = Constraint::New<Vector2>(target, P::TRANSFORM_OFFSET, [rectangle, controlSize, extent, mode, pivot, captureOffset](Vector2& value, const PropertyInputContainer& inputs)
    {
      const auto  original     = extent(inputs);
      const auto& sourceOffset = inputs[2]->GetVector2();
      value                    = Vector2(sourceOffset.x * (controlSize.x * (1.0f - mode.x) + mode.x) + (rectangle.x + pivot.x - 0.5f) * original.x,
                                         sourceOffset.y * (controlSize.y * (1.0f - mode.y) + mode.y) + (rectangle.y + pivot.y - 0.5f) * original.y) +
              captureOffset;
    });
    for(auto* constraint : {&size, &offset})
    {
      constraint->AddSource(Source(source, P::TRANSFORM_SIZE));
      constraint->AddSource(Source(source, P::EXTRA_SIZE));
    }
    offset.AddSource(Source(source, P::TRANSFORM_OFFSET));
    size.Apply();
    offset.Apply();
  }
  target.SetTextures(textures);
  return target;
}

void BindTexture(Renderer renderer, Texture texture)
{
  if(!Dali::Adaptor::IsAvailable() || !renderer)
  {
    return;
  }
  TextureSet textures = TextureSet::New();
  if(!Dali::Adaptor::IsAvailable())
  {
    return;
  }
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);
  sampler.SetWrapMode(WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE);
  textures.SetTexture(0u, texture);
  textures.SetSampler(0u, sampler);
  renderer.SetTextures(textures);
}

Actor NewPassActor(const char* name, const Vector2& size)
{
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, name);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
  actor.SetProperty(Actor::Property::SIZE, size);
  // Capture foreground color exactly once; final composition inherits owner color.
  actor.SetProperty(Actor::Property::COLOR_MODE, ColorMode::USE_OWN_COLOR);
  return actor;
}

Shader& GetAlphaSourceShader(bool atlased = false)
{
  thread_local std::array<Shader, 2u> shaders;
  auto&                               shader = shaders[atlased ? 1u : 0u];
  if(!shader)
  {
    // This variant is selected only from the resolved single-color feature
    // state. Do not inspect or rewrite the original shader's source at runtime.
    const std::string prefix   = atlased ? "#define TEXT_REVEAL_SOURCE_ATLAS\n" : "";
    const std::string vertex   = prefix + SHADER_TEXT_VISUAL_SHADER_VERT.data();
    const std::string fragment = prefix + "#define IS_REQUIRED_TEXT_REVEAL\n#define TEXT_REVEAL_BLUR_ALPHA_SOURCE\n" + SHADER_TEXT_VISUAL_SHADER_FRAG.data();
    shader                     = Shader::New(Dali::Integration::ToDaliStringView(vertex),
                                             Dali::Integration::ToDaliStringView(fragment), atlased ? Shader::Hint::MODIFIES_GEOMETRY : Shader::Hint::NONE,
                         atlased ? "TEXT_REVEAL_BLUR_ALPHA_ATLAS" : "TEXT_REVEAL_BLUR_ALPHA_SOURCE");
    if(!Dali::Adaptor::IsAvailable())
    {
      return shader;
    }
    shader.RegisterProperty("viewEffectiveScale", 1.0f);
    shader.RegisterProperty("visualTransformUseEffectiveScale", 1.0f);
    shader.RegisterProperty("pixelSnapFactor", 0.0f);
  }
  return shader;
}

// Source geometry carries no per-line uniforms. Bound indices independently
// of atlas chunking and blur page packing.
constexpr uint32_t MAX_SOURCE_LINES_PER_DRAW = TextRevealBlurRenderer::MAX_LINES_PER_DRAW;
static_assert(MAX_SOURCE_LINES_PER_DRAW <= std::numeric_limits<uint16_t>::max() / 4u);

struct SourceBatchVertex
{
  Vector2 position;
  Vector4 crop;
  Vector2 captureOffset;
  Vector4 atlas;
};

Geometry CreateSourceBatchGeometry(const std::vector<SourceBatchVertex>& vertices)
{
  Property::Map format;
  format["aPosition"]     = Property::VECTOR2;
  format["aSourceCrop"]   = Property::VECTOR4;
  format["aSourceOffset"] = Property::VECTOR2;
  format["aSourceAtlas"]  = Property::VECTOR4;
  auto buffer             = VertexBuffer::New(format);
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  buffer.SetData(vertices.data(), static_cast<uint32_t>(vertices.size()));
  std::vector<uint16_t> indices;
  indices.reserve(vertices.size() / 4u * 6u);
  for(size_t first = 0u; first < vertices.size(); first += 4u)
  {
    for(uint32_t corner : {0u, 3u, 1u, 0u, 2u, 3u})
    {
      indices.push_back(static_cast<uint16_t>(first + corner));
    }
  }
  auto geometry = Geometry::New();
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  geometry.AddVertexBuffer(buffer);
  geometry.SetIndexBuffer(indices.data(), static_cast<uint32_t>(indices.size()));
  return geometry;
}

Shader& GetDecorationShader()
{
  thread_local Shader shader;
  if(!shader)
  {
    const char* fragment = R"SHADER(
//@version 100
precision highp float;
INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
void main()
{
  gl_FragColor = TEXTURE(sTexture, vTexCoord);
}
)SHADER";
    shader               = Shader::New(Dali::Integration::ToDaliStringView(SHADER_TEXT_VISUAL_SHADER_VERT), fragment,
                                       Shader::Hint::NONE, "TEXT_REVEAL_UNBLURRED_DECORATION");
    shader.RegisterProperty("viewEffectiveScale", 1.0f);
    shader.RegisterProperty("visualTransformUseEffectiveScale", 1.0f);
    shader.RegisterProperty("pixelSnapFactor", 0.0f);
  }
  return shader;
}

Renderer CreateBlurOutput(Renderer foreground, Geometry geometry, bool alphaOnly, bool batched, bool quarterSource = false)
{
  if(!Dali::Adaptor::IsAvailable() || !geometry)
  {
    return {};
  }
  const bool          batchStrength = quarterSource && batched;
  thread_local Shader wholeAlphaShader;
  thread_local Shader batchShaders[2];
  thread_local Shader quarterShaders[2][2];
  auto&               shader = quarterSource ? quarterShaders[batched ? 1u : 0u][alphaOnly ? 1u : 0u]
                               : batched     ? batchShaders[alphaOnly ? 1u : 0u]
                                             : wholeAlphaShader;
  if(!shader)
  {
    std::string vertex;
    if(batchStrength)
    {
      vertex = "#define TEXT_REVEAL_OUTPUT_BATCH\n#define NUM_REVEAL_LINES " + std::to_string(TextRevealBlurRenderer::MAX_LINES_PER_DRAW) + "\n";
    }
    vertex += R"SHADER(
//@version 100
precision highp float;
INPUT highp vec2 aPosition;
INPUT highp vec2 aTexCoord;
INPUT highp vec4 aRevealRectangle;
OUTPUT highp vec2 vTexCoord;
OUTPUT highp vec4 vRevealRectangle;
#ifdef TEXT_REVEAL_OUTPUT_BATCH
INPUT highp float aRevealLineIndex;
OUTPUT highp float vRevealBlurStrength;
#endif
UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
#ifdef TEXT_REVEAL_OUTPUT_BATCH
  UNIFORM highp float uRevealBlurStrength[NUM_REVEAL_LINES];
#endif
};
void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition, 0.0, 1.0);
  vTexCoord = aTexCoord;
  vRevealRectangle = aRevealRectangle;
#ifdef TEXT_REVEAL_OUTPUT_BATCH
  // All four vertices retain the same CPU-generated line strength.
  vRevealBlurStrength = uRevealBlurStrength[int(aRevealLineIndex)];
#endif
}
)SHADER";
    std::string fragment = alphaOnly ? "#define ALPHA_ONLY\n" : "";
    if(quarterSource)
    {
      fragment += "#define QUARTER_BLUR_SHARP_TAKEOVER\n";
    }
    if(batchStrength)
    {
      fragment += "#define TEXT_REVEAL_OUTPUT_BATCH\n";
    }
    if(!batched)
    {
      fragment += "#define WHOLE_TEXT_OUTPUT\n";
    }
    fragment += R"SHADER(
//@version 100
precision highp float;
INPUT highp vec2 vTexCoord;
#ifndef WHOLE_TEXT_OUTPUT
INPUT highp vec4 vRevealRectangle;
#endif
UNIFORM sampler2D sTexture;
#ifdef QUARTER_BLUR_SHARP_TAKEOVER
UNIFORM sampler2D sSharpSource;
#ifdef TEXT_REVEAL_OUTPUT_BATCH
INPUT highp float vRevealBlurStrength;
#define uAnimationRatio vRevealBlurStrength
#endif
UNIFORM_BLOCK RevealBlurHandoff
{
#ifndef TEXT_REVEAL_OUTPUT_BATCH
  UNIFORM highp float uAnimationRatio;
#endif
  UNIFORM highp float uQuarterAuthoredRadius;
};
#endif
UNIFORM_BLOCK FragColor
{
  UNIFORM vec4 uColor;
#ifdef ALPHA_ONLY
  UNIFORM vec4 uTextColorAnimatable;
#endif
};
#ifdef QUARTER_BLUR_SHARP_TAKEOVER
highp float SmootherStep(highp float t)
{
  t = clamp(t, 0.0, 1.0);
  return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}
#endif
void main()
{
#ifdef WHOLE_TEXT_OUTPUT
  highp vec4 color = TEXTURE(sTexture, vTexCoord);
#else
  highp vec4 color = TEXTURE(sTexture, vRevealRectangle.xy + vTexCoord * vRevealRectangle.zw);
#endif
#ifdef QUARTER_BLUR_SHARP_TAKEOVER
  // H/V use the sequence's changing strength. Restore sharp detail as the
  // effective radius falls below the reduced-resolution reconstruction range.
#ifdef WHOLE_TEXT_OUTPUT
  highp vec4 sharpColor = TEXTURE(sSharpSource, vTexCoord);
#else
  highp vec4 sharpColor = TEXTURE(sSharpSource, vRevealRectangle.xy + vTexCoord * vRevealRectangle.zw);
#endif
  // The existing output constraint supplies this line's actual blur strength.
  // Only handoff uses authored radius; H/V sampling and timing stay unchanged.
  const highp float SHARP_ONLY_EFFECTIVE_RADIUS = 2.0;
  const highp float BLUR_ONLY_EFFECTIVE_RADIUS = 8.0;
  highp float effectiveRadius = uQuarterAuthoredRadius * uAnimationRatio;
  highp float t = (effectiveRadius - SHARP_ONLY_EFFECTIVE_RADIUS) / (BLUR_ONLY_EFFECTIVE_RADIUS - SHARP_ONLY_EFFECTIVE_RADIUS);
  highp float blurMix = SmootherStep(t);
  // RGBA captures are premultiplied: interpolate color and coverage together.
  // The alpha-only path reconstructs text color from the mixed red channel.
  color = t <= 0.0 ? sharpColor : mix(sharpColor, color, blurMix);
#endif
#ifdef ALPHA_ONLY
  highp vec3 rgb = uTextColorAnimatable.a > 0.0 ? uTextColorAnimatable.rgb / uTextColorAnimatable.a : vec3(0.0);
  color = vec4(rgb * color.r, color.r);
#endif
  gl_FragColor = color * uColor;
}
)SHADER";
    shader = Shader::New(Dali::Integration::ToDaliStringView(batched ? std::string_view(vertex) : BASIC_VERTEX_SOURCE), Dali::Integration::ToDaliStringView(fragment),
                         Shader::Hint::NONE, quarterSource ? (batched ? "TEXT_REVEAL_QUARTER_BLUR_BATCH_OUTPUT" : "TEXT_REVEAL_QUARTER_BLUR_OUTPUT") : !batched  ? "TEXT_REVEAL_BLUR_WHOLE_ALPHA_OUTPUT"
                                                                                                                                                     : alphaOnly ? "TEXT_REVEAL_BLUR_ALPHA_OUTPUT"
                                                                                                                                                                 : "TEXT_REVEAL_BLUR_BATCH_OUTPUT");
    shader.RegisterProperty("viewEffectiveScale", 1.0f);
  }
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  Renderer output = Renderer::New(geometry, shader);
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  if(alphaOnly)
  {
    // Alpha was captured once. Read the same update-side color as that capture
    // and restore its unpremultiplied RGB; owner color is applied only at output.
    const auto source = foreground.GetPropertyIndex("uTextColorAnimatable");
    const auto target = output.RegisterProperty("uTextColorAnimatable", foreground.GetProperty(source));
    MirrorProperty<Vector4>(output, target, foreground, source);
  }
  return output;
}

Renderer CreatePlainOutput()
{
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  auto shader = Shader::New(Dali::Integration::ToDaliStringView(BASIC_VERTEX_SOURCE),
                            Dali::Integration::ToDaliStringView(BASIC_FRAGMENT_SOURCE));
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  shader.ReserveCustomProperties(1u);
  shader.RegisterUniqueProperty("viewEffectiveScale", 1.0f);
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  auto geometry = VisualFactoryCache::CreateGridGeometry(Uint16Pair(1, 1), true);
  return Dali::Adaptor::IsAvailable() ? Renderer::New(geometry, shader) : Renderer{};
}

struct BlurBatchVertex
{
  Vector2 position;
  Vector2 texCoord;
  Vector4 rectangle;
  Vector2 inverseSize;
  float   lineIndex;
};

// Full-resolution composition only samples the atlas. Keep its stride and format
// free of the inverse-size and line-index attributes used by the H/V passes.
struct BlurBatchOutputVertex
{
  Vector2 position;
  Vector2 texCoord;
  Vector4 rectangle;
};

// PERFORMANCE output selects line-local strength for the sharp-source handoff.
// QUALITY keeps its existing vertex format, without unused shader attributes.
struct BlurBatchHandoffVertex
{
  Vector2 position;
  Vector2 texCoord;
  Vector4 rectangle;
  float   lineIndex;
};

template<typename Vertex>
Geometry CreateBlurBatchGeometry(const Vertex* vertices, uint32_t lineCount)
{
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  static_assert(std::is_same_v<Vertex, BlurBatchVertex> || std::is_same_v<Vertex, BlurBatchOutputVertex> || std::is_same_v<Vertex, BlurBatchHandoffVertex>);
  Property::Map format;
  format["aPosition"]        = Property::VECTOR2;
  format["aTexCoord"]        = Property::VECTOR2;
  format["aRevealRectangle"] = Property::VECTOR4;
  if constexpr(std::is_same_v<Vertex, BlurBatchVertex>)
  {
    format["aRevealInverseSize"] = Property::VECTOR2;
    format["aRevealLineIndex"]   = Property::FLOAT;
  }
  else if constexpr(std::is_same_v<Vertex, BlurBatchHandoffVertex>)
  {
    format["aRevealLineIndex"] = Property::FLOAT;
  }
  VertexBuffer buffer = VertexBuffer::New(format);
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  buffer.SetData(vertices, lineCount * 4u);
  std::vector<uint16_t> indices;
  indices.reserve(lineCount * 6u);
  for(uint32_t line = 0u; line < lineCount; ++line)
  {
    for(uint32_t corner : {0u, 3u, 1u, 0u, 2u, 3u})
    {
      indices.push_back(static_cast<uint16_t>(line * 4u + corner));
    }
  }
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(buffer);
  geometry.SetIndexBuffer(indices.data(), static_cast<uint32_t>(indices.size()));
  return geometry;
}

/**
 * @brief Owns the offscreen resources for Text::Reveal blur.
 *
 * WHOLE_TEXT reuses the TextVisual foreground renderer. PER_LINE batches
 * compatible atlas entries and retains isolated sub-draws where required.
 * Both paths preserve local capture coordinates, resolved gradient/color-glyph
 * composition and update-side Reveal progress.
 */
class RuntimeBlurActor : public CustomActorImpl
{
  struct ImageCapture
  {
    struct Binding
    {
      RevealBlurImage placement;
      Actor           target;
      Vector2         offset;
      Renderer        source;
      Renderer        proxy;
    };
    uint64_t             sourceRevision{0u};
    bool                 released{false};
    Actor                retention;
    std::vector<Binding> bindings;
  };
  struct DecorationComposition
  {
    explicit DecorationComposition(std::unique_ptr<RuntimeRevealBlurDecorations> decorationPlanes)
    : planes(std::move(decorationPlanes))
    {
    }
    std::unique_ptr<RuntimeRevealBlurDecorations> planes;
    TextureSet                                    originalTextures;
    Actor                                         source;
    Actor                                         overlay;
    FrameBuffer                                   buffer;
    RenderTask                                    task;
  };

public:
  RuntimeBlurActor(Actor owner, Renderer foreground, const Vector2& size,
                   Property::Index progressIndex, uint32_t radius, float blurDuration,
                   std::vector<RuntimeRevealBlurSequence> sequences, bool singleColor,
                   std::unique_ptr<RuntimeRevealBlurDecorations> decorations,
                   std::unique_ptr<RuntimeRevealBlurImages>      images,
                   RuntimeRevealBlurSettings                     settings)
  : mForeground(foreground),
    mSourceTextures(foreground.GetTextures()),
    mSourceShader(foreground.GetShader()),
    mContentSize(size),
    mTargetSize(std::ceil(size.x) + 2.0f * static_cast<float>(radius + 2u),
                std::ceil(size.y) + 2.0f * static_cast<float>(radius + 2u)),
    mRadius(radius),
    mBlurDuration(blurDuration),
    mSettings(settings),
    mSingleColor(singleColor),
    mProgressIndex(progressIndex),
    mOwner(owner),
    mSequences(std::move(sequences)),
    mDecorations(decorations ? std::make_unique<DecorationComposition>(std::move(decorations)) : nullptr)
  {
    if(images && !images->placements.empty())
    {
      mImages                 = std::make_unique<ImageCapture>();
      mImages->sourceRevision = images->sourceRevision;
      for(const auto& placement : images->placements)
      {
        mImages->bindings.push_back({placement});
      }
    }
  }

  Renderer ReleaseForeground()
  {
    if(mReleased)
    {
      return {};
    }
    mReleased = true;
    if(mImages)
    {
      mImages->released = true;
      if(auto* data = Text::GetInlineReplacementData(Ui::View::DownCast(mOwner.GetHandle())))
      {
        data->manager.ReleaseBlurCapture(Self());
      }
    }
    if(!mForegroundBorrowed)
    {
      return {};
    }
    mForegroundBorrowed = false;
    mForegroundActor.RemoveRenderer(mForeground);
    if(mForegroundShader)
    {
      // WHOLE_TEXT borrows the original renderer for foreground capture. Async
      // publication may already have installed its next shader before cleanup.
      if(mForeground.GetShader() == mCaptureShader)
      {
        mForeground.SetShader(mForegroundShader);
      }
      mForegroundShader.Reset();
    }
    if(mDecorations && mDecorations->originalTextures &&
       mForeground.GetTextures() == mDecorations->planes->foregroundTextures)
    {
      mForeground.SetTextures(mDecorations->originalTextures);
    }
    return mForeground;
  }

  bool IsActive() const
  {
    return Dali::Adaptor::IsAvailable() && !mReleased && static_cast<bool>(mTaskList);
  }

  bool HasCurrentForeground() const
  {
    return !mReleased && mForegroundBorrowed &&
           mForeground.GetShader() == (mCaptureShader ? mCaptureShader : mSourceShader) &&
           mForeground.GetTextures() == (mDecorations && mDecorations->originalTextures
                                           ? mDecorations->planes->foregroundTextures
                                           : mSourceTextures);
  }

  bool BorrowForeground(Actor owner)
  {
    if(mReleased || mForegroundBorrowed || owner != mOwner.GetHandle())
    {
      return false;
    }
    // Core renderer attachment and shader/texture setters enqueue messages;
    // they do not emit application signals. Claim ownership before transfer.
    mForegroundBorrowed = true;
    owner.RemoveRenderer(mForeground);
    if(mCaptureShader)
    {
      mForegroundShader = mForeground.GetShader();
      mForeground.SetShader(mCaptureShader);
    }
    if(mDecorations && mSequences.size() == 1u && !mPerLine)
    {
      mDecorations->originalTextures = mForeground.GetTextures();
      mForeground.SetTextures(mDecorations->planes->foregroundTextures);
    }
    mForegroundActor.AddRenderer(mForeground);
    return true;
  }

  void RefreshImages()
  {
    if(!Dali::Adaptor::IsAvailable() || !mImages || mImages->released)
    {
      return;
    }
    auto  owner = mOwner.GetHandle();
    auto* data  = Text::GetInlineReplacementData(Ui::View::DownCast(owner));
    if(!data)
    {
      return;
    }
    const auto sources = data->manager.GetReadyBlurCaptureSources(mImages->sourceRevision);
    for(auto& binding : mImages->bindings)
    {
      const auto found = std::find_if(sources.begin(), sources.end(), [&](const auto& source)
      {
        return source.occurrenceIdentity == binding.placement.occurrenceIdentity &&
               source.lineIndex == binding.placement.lineIndex;
      });
      if(found == sources.end())
      {
        if(binding.proxy)
        {
          binding.target.RemoveRenderer(binding.proxy);
          binding.proxy.Reset();
          binding.source.Reset();
        }
        continue;
      }
      if(binding.source != found->renderer || !binding.proxy ||
         binding.proxy.GetTextures() != found->renderer.GetTextures() ||
         binding.proxy.GetShader() != found->renderer.GetShader())
      {
        if(binding.proxy)
        {
          binding.target.RemoveRenderer(binding.proxy);
        }
        ForegroundProperties properties;
        auto                 proxy = CloneForeground(found->renderer, properties, found->renderer.GetTextures(),
                                                     Vector4(0.0f, 0.0f, 1.0f, 1.0f), mContentSize, binding.offset);
        // Object-created callbacks may retire this publication during cloning.
        if(!Dali::Adaptor::IsAvailable() || mImages->released || !proxy)
        {
          return;
        }
        binding.source = found->renderer;
        binding.proxy  = proxy;
        binding.target.AddRenderer(proxy);
      }
      data = Text::GetInlineReplacementData(Ui::View::DownCast(owner));
      if(!data || !data->manager.CaptureBlurRenderer(Self(), mImages->sourceRevision, *found, mImages->retention))
      {
        binding.target.RemoveRenderer(binding.proxy);
        binding.proxy.Reset();
        binding.source.Reset();
      }
    }
  }

  void GetOffScreenRenderTasks(Dali::Vector<RenderTask>& tasks, bool isForward) override
  {
    if(isForward)
    {
      for(const auto& pass : mPasses)
      {
        for(auto task : pass.tasks)
        {
          if(task)
          {
            tasks.PushBack(task);
          }
        }
      }
      if(mDecorations && mDecorations->task)
      {
        tasks.PushBack(mDecorations->task);
      }
    }
  }

  void Initialize()
  {
    Actor self = Self();
    self.SetProperty(Actor::Property::SIZE, mTargetSize);
    self.SetProperty(Actor::Property::POSITION, Vector2((mContentSize.x - mTargetSize.x) * 0.5f,
                                                        (mContentSize.y - mTargetSize.y) * 0.5f));
    self.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    self.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    self.SetProperty(Actor::Property::NAME, "TextRevealRuntimeGaussian");
    RegisterOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
    if(!Dali::Adaptor::IsAvailable())
    {
      return;
    }

    if(mImages)
    {
      // A renderer used as a constraint source must remain scene-connected.
      // One hidden core actor retains all originals; only proxies are captured.
      mImages->retention = NewPassActor("RevealGaussianRetainedImages", mContentSize);
      if(!Dali::Adaptor::IsAvailable())
      {
        return;
      }
      mImages->retention.SetProperty(Actor::Property::VISIBLE, false);
      self.Add(mImages->retention);
      if(!Dali::Adaptor::IsAvailable())
      {
        return;
      }
    }

    Actor outputParent = self;
    if(mDecorations)
    {
      auto& decoration  = *mDecorations;
      decoration.source = NewPassActor("RevealGaussianDecorationComposition", mTargetSize);
      if(!Dali::Adaptor::IsAvailable())
      {
        return;
      }
      self.Add(decoration.source);
      outputParent = decoration.source;
      ForegroundProperties properties;
      for(bool overlay : {false, true})
      {
        const auto textures = overlay ? decoration.planes->overlay : decoration.planes->background;
        if(textures)
        {
          Actor planes = NewPassActor(overlay ? "RevealGaussianOverlay" : "RevealGaussianBackground", mContentSize);
          planes.RegisterProperty("viewEffectiveScale", 1.0f);
          if(!Dali::Adaptor::IsAvailable())
          {
            return;
          }
          auto renderer = CloneForeground(mForeground, properties, textures, Vector4(0.0f, 0.0f, 1.0f, 1.0f), mContentSize);
          if(!Dali::Adaptor::IsAvailable() || !renderer)
          {
            return;
          }
          renderer.SetShader(GetDecorationShader());
          renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
          renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
          planes.AddRenderer(renderer);
          if(!Dali::Adaptor::IsAvailable())
          {
            return;
          }
          if(overlay)
          {
            decoration.overlay = planes;
          }
          else
          {
            decoration.source.Add(planes);
          }
        }
      }
      decoration.buffer = FrameBuffer::New(static_cast<uint32_t>(mTargetSize.x), static_cast<uint32_t>(mTargetSize.y), FrameBuffer::Attachment::NONE);
      if(!Dali::Adaptor::IsAvailable())
      {
        return;
      }
      Texture compositeTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888,
                                              static_cast<uint32_t>(mTargetSize.x), static_cast<uint32_t>(mTargetSize.y));
      if(!Dali::Adaptor::IsAvailable())
      {
        return;
      }
      decoration.buffer.AttachColorTexture(compositeTexture);
      Renderer output = CreatePlainOutput();
      if(!Dali::Adaptor::IsAvailable() || !output)
      {
        return;
      }
      BindTexture(output, decoration.buffer.GetColorTexture());
      output.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
      output.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
      if(!Dali::Adaptor::IsAvailable())
      {
        return;
      }
      Actor presentation = NewPassActor("RevealGaussianDecoratedOutput", mTargetSize);
      presentation.SetProperty(Actor::Property::COLOR_MODE, ColorMode::USE_PARENT_COLOR);
      presentation.AddRenderer(output);
      self.Add(presentation);
      if(!Dali::Adaptor::IsAvailable())
      {
        return;
      }
    }

    mForegroundActor = NewPassActor("RevealGaussianForeground", mContentSize);
    mForegroundActor.RegisterProperty("viewEffectiveScale", 1.0f);
    if(!Dali::Adaptor::IsAvailable())
    {
      return;
    }
    const bool perLine = !mSequences.empty();
    mPerLine           = perLine;
    if(perLine)
    {
      mForegroundActor.SetProperty(Actor::Property::VISIBLE, false);
      self.Add(mForegroundActor);
    }
    else
    {
      mSequences.emplace_back();
    }

    Actor      owner   = mOwner.GetHandle();
    const bool batched = perLine;
    // A8 maps to R8_UNORM. Restrict this path to GLES 3+, whose red-channel
    // render targets are supported; unvalidated backends retain RGBA.
    const bool alphaCapable = mSingleColor &&
                              mSourceTextures.GetTexture(0u).GetPixelFormat() == Pixel::L8 &&
                              mForeground.GetPropertyIndex("uTextColorAnimatable") != Property::INVALID_INDEX &&
                              Dali::Graphics::GetCurrentGraphicsBackend() == Dali::Graphics::Backend::GLES &&
                              Shader::GetShaderLanguageVersion() >= 300u;
    if(mDecorations && !perLine)
    {
      mCaptureShader = mDecorations->planes->foregroundShader;
    }
    if(alphaCapable && !mImages && !perLine)
    {
      // Reuse the existing foreground and its bindings without another clone.
      mCaptureShader = GetAlphaSourceShader();
    }
    // Only image-bearing sequences need RGBA. Pack the two formats separately;
    // presentation is restored to original logical order below, including halos.
    // No image means the previous batch path and allocation counts are intact.
    std::vector<size_t> sequenceOrder;
    size_t              alphaSequenceCount = mSequences.size();
    if(mImages && batched && alphaCapable)
    {
      auto hasImage = [&](size_t index)
      {
        return std::any_of(mImages->bindings.begin(), mImages->bindings.end(), [&](const auto& image)
        {
          return image.placement.lineIndex == mSequences[index].lineIndex;
        });
      };
      for(size_t i = 0u; i < mSequences.size(); ++i)
      {
        if(!hasImage(i))
        {
          sequenceOrder.push_back(i);
        }
      }
      alphaSequenceCount = sequenceOrder.size();
      for(size_t i = 0u; i < mSequences.size(); ++i)
      {
        if(hasImage(i))
        {
          sequenceOrder.push_back(i);
        }
      }
      std::vector<RuntimeRevealBlurSequence> ordered;
      ordered.reserve(mSequences.size());
      for(auto index : sequenceOrder)
      {
        ordered.push_back(std::move(mSequences[index]));
      }
      mSequences = std::move(ordered);
    }
    std::vector<Vector2> lineSizes;
    std::vector<Vector2> lineOffsets;
    std::vector<Vector2> horizontalBands;
    lineSizes.reserve(mSequences.size());
    lineOffsets.reserve(mSequences.size());
    if(batched)
    {
      horizontalBands.reserve(mSequences.size());
    }
    for(const auto& timing : mSequences)
    {
      const auto sourceTexture = mSourceTextures.GetTexture(0u);
      Vector2    horizontalBand;
      auto       bounds = batched
                            ? ResolveRuntimeRevealBlurTarget(mForeground, mContentSize,
                                                             Vector2(static_cast<float>(sourceTexture.GetWidth()), static_cast<float>(sourceTexture.GetHeight())), timing.coverage, mRadius, &horizontalBand)
                            : Rect<int32_t>(0, 0, static_cast<int32_t>(mTargetSize.x), static_cast<int32_t>(mTargetSize.y));
      if(batched && mImages)
      {
        bool hasBounds = timing.hasTextForeground;
        for(const auto& image : mImages->bindings)
        {
          if(image.placement.lineIndex != timing.lineIndex)
          {
            continue;
          }
          Vector2    imageBand;
          const auto area = ResolveRuntimeRevealBlurTarget(mForeground, mContentSize,
                                                           Vector2(static_cast<float>(sourceTexture.GetWidth()), static_cast<float>(sourceTexture.GetHeight())),
                                                           image.placement.coverage, mRadius, &imageBand);
          if(!hasBounds)
          {
            bounds         = area;
            hasBounds      = true;
            horizontalBand = imageBand;
          }
          else
          {
            const int32_t left = std::min(bounds.x, area.x);
            const int32_t top  = std::min(bounds.y, area.y);
            bounds             = Rect<int32_t>(left, top, std::max(bounds.x + bounds.width, area.x + area.width) - left,
                                               std::max(bounds.y + bounds.height, area.y + area.height) - top);
            horizontalBand.x   = std::min(horizontalBand.x, imageBand.x);
            horizontalBand.y   = std::max(horizontalBand.y, imageBand.y);
          }
        }
      }
      const Vector2 size(static_cast<float>(bounds.width), static_cast<float>(bounds.height));
      if(batched)
      {
        // Include every reserved image slot, even before its renderer is READY.
        // Normalize only after the text/image union and original target clamp.
        horizontalBand   = (horizontalBand - Vector2(static_cast<float>(bounds.y), static_cast<float>(bounds.y))) / size.y;
        horizontalBand.x = std::clamp(horizontalBand.x, 0.0f, 1.0f);
        horizontalBand.y = std::clamp(horizontalBand.y, 0.0f, 1.0f);
        if(!std::isfinite(horizontalBand.x) || !std::isfinite(horizontalBand.y) || horizontalBand.x >= horizontalBand.y)
        {
          horizontalBand = Vector2(0.0f, 1.0f);
        }
        horizontalBands.push_back(horizontalBand);
      }
      lineSizes.push_back(size);
      lineOffsets.emplace_back(static_cast<float>(bounds.x) + (size.x - mTargetSize.x) * 0.5f,
                               static_cast<float>(bounds.y) + (size.y - mTargetSize.y) * 0.5f);
    }
    std::vector<RuntimeRevealBlurBatch> batches;
    if(batched)
    {
      if(!sequenceOrder.empty() && alphaSequenceCount > 0u && alphaSequenceCount < lineSizes.size())
      {
        for(const auto& range : {std::pair<size_t, size_t>(0u, alphaSequenceCount), {alphaSequenceCount, lineSizes.size()}})
        {
          const std::vector<Vector2> sizes(lineSizes.begin() + static_cast<std::vector<Vector2>::difference_type>(range.first),
                                           lineSizes.begin() + static_cast<std::vector<Vector2>::difference_type>(range.second));
          for(auto batch : BuildRuntimeRevealBlurBatches(sizes, Dali::GetMaxTextureSize()))
          {
            batch.first += range.first;
            batches.push_back(batch);
          }
        }
      }
      else
      {
        batches = BuildRuntimeRevealBlurBatches(lineSizes, Dali::GetMaxTextureSize());
      }
    }
    else
    {
      for(size_t index = 0u; index < lineSizes.size(); ++index)
      {
        batches.push_back({index, 1u, lineSizes[index]});
      }
    }
    const bool                            quarterBlur           = mSettings.path == RuntimeRevealBlurPath::AXIS_AWARE_QUARTER;
    const float                           quarterAuthoredRadius = mSettings.authoredRadius;
    ForegroundProperties                  foregroundProperties;
    std::vector<std::pair<size_t, Actor>> orderedOutputs;
    mPasses.resize(batches.size());
    for(size_t batchIndex = 0u; batchIndex < batches.size(); ++batchIndex)
    {
      if(!Dali::Adaptor::IsAvailable())
      {
        return;
      }
      const auto& batch        = batches[batchIndex];
      const bool  alphaOnly    = alphaCapable && (!mImages || (batched && batch.first < alphaSequenceCount));
      const auto  targetFormat = alphaOnly ? Pixel::A8 : Pixel::RGBA8888;
      auto&       pass         = mPasses[batchIndex];
      pass.size                = batch.size;
      pass.source              = NewPassActor(batched ? "RevealGaussianBatchSource" : "RevealGaussianSource", pass.size);
      if(!Dali::Adaptor::IsAvailable())
      {
        return;
      }
      self.Add(pass.source);
      for(uint32_t i = 0u; i < 2u; ++i)
      {
        pass.blurActors[i] = NewPassActor(batched ? (i == 0u ? "RevealGaussianBatchH" : "RevealGaussianBatchV")
                                                  : (i == 0u ? "RevealGaussianH" : "RevealGaussianV"),
                                          pass.size);
        if(!Dali::Adaptor::IsAvailable())
        {
          return;
        }
        self.Add(pass.blurActors[i]);
      }
      if(batched)
      {
        // Full resolution consumes source/H before the next page overwrites it.
        // Late Smooth also samples source at composition, so only H is scratch
        // in that path. Never share textures across companions or formats.
        for(size_t previous = 0u; previous < batchIndex; ++previous)
        {
          if(mPasses[previous].size == pass.size &&
             mPasses[previous].buffers[0u].GetColorTexture().GetPixelFormat() == targetFormat)
          {
            if(!quarterBlur)
            {
              pass.buffers[0u] = mPasses[previous].buffers[0u];
            }
            pass.buffers[1u] = mPasses[previous].buffers[1u];
            break;
          }
        }
      }
      for(size_t bufferIndex = 0u; bufferIndex < pass.buffers.size(); ++bufferIndex)
      {
        auto& buffer = pass.buffers[bufferIndex];
        if(!buffer)
        {
          auto width  = static_cast<uint32_t>(pass.size.x);
          auto height = static_cast<uint32_t>(pass.size.y);
          if(quarterBlur && bufferIndex > 0u)
          {
            width = std::max(1u, static_cast<uint32_t>(std::ceil(pass.size.x * QUARTER_BLUR_SCALE)));
            if(bufferIndex == 2u)
            {
              height = std::max(1u, static_cast<uint32_t>(std::ceil(pass.size.y * QUARTER_BLUR_SCALE)));
            }
          }
          buffer = FrameBuffer::New(width, height, FrameBuffer::Attachment::NONE);
          if(!Dali::Adaptor::IsAvailable())
          {
            return;
          }
          Texture texture = Texture::New(TextureType::TEXTURE_2D, targetFormat, width, height);
          if(!Dali::Adaptor::IsAvailable())
          {
            return;
          }
          buffer.AttachColorTexture(texture);
        }
      }
      std::vector<BlurBatchVertex>        vertices;
      std::vector<BlurBatchVertex>        horizontalVertices;
      std::vector<BlurBatchOutputVertex>  outputVertices;
      std::vector<BlurBatchHandoffVertex> handoffVertices;
      std::vector<SourceBatchVertex>      sourceVertices;
      TextureSet                          sourceTextures;
      Shader                              sourceShader;
      Actor                               sharedForeground;
      if(batched)
      {
        vertices.reserve(batch.count * 4u);
        horizontalVertices.reserve(batch.count * 4u);
        if(quarterBlur)
        {
          handoffVertices.reserve(batch.count * 4u);
        }
        else
        {
          outputVertices.reserve(batch.count * 4u);
        }
        // Keep the text shader's original uSize and origin reference. Only
        // page placement moves into each renderer; textures and timing remain
        // line-local. The source task and its camera keep the page dimensions.
        sharedForeground = NewPassActor("RevealGaussianLineForeground", mContentSize);
        sharedForeground.RegisterProperty("viewEffectiveScale", 1.0f);
        if(!Dali::Adaptor::IsAvailable())
        {
          return;
        }
        pass.source.Add(sharedForeground);
      }
      // A TextureSet identifies the atlas format, masks, shared lookups and
      // sampler state of this publication. All scalar bindings still mirror
      // mForeground. Never combine across an image slot or a legacy sub-draw.
      auto flushSource = [&]()
      {
        if(sourceVertices.empty())
        {
          return true;
        }
        auto renderer = CloneForeground(mForeground, foregroundProperties, sourceTextures,
                                        Vector4(0.0f, 0.0f, 1.0f, 1.0f), mContentSize);
        if(!Dali::Adaptor::IsAvailable() || !renderer)
        {
          return false;
        }
        auto geometry = CreateSourceBatchGeometry(sourceVertices);
        if(!Dali::Adaptor::IsAvailable() || !geometry)
        {
          return false;
        }
        renderer.SetGeometry(geometry);
        renderer.SetShader(sourceShader);
        sharedForeground.AddRenderer(renderer);
        sourceVertices.clear();
        return true;
      };
      if(perLine)
      {
        sourceVertices.reserve(std::min(batch.count, static_cast<size_t>(MAX_SOURCE_LINES_PER_DRAW)) * 4u);
      }
      float y = 0.0f;
      for(size_t sequence = batch.first; sequence < batch.first + batch.count; ++sequence)
      {
        const auto&   timing = mSequences[sequence];
        const auto&   size   = lineSizes[sequence];
        const auto&   offset = lineOffsets[sequence];
        const Vector2 center((size.x - pass.size.x) * 0.5f, y + (size.y - pass.size.y) * 0.5f);
        const Vector4 rectangle(0.0f, y / pass.size.y, size.x / pass.size.x, size.y / pass.size.y);
        if(batched)
        {
          const float line = static_cast<float>((sequence - batch.first) % TextRevealBlurRenderer::MAX_LINES_PER_DRAW);
          for(const Vector2 uv : {Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f)})
          {
            vertices.push_back({center + (uv - Vector2(0.5f, 0.5f)) * size, uv, rectangle,
                                Vector2(1.0f / size.x, 1.0f / size.y), line});
            const auto&   band = horizontalBands[sequence];
            const Vector2 horizontalUv(uv.x, uv.y == 0.0f ? band.x : band.y);
            horizontalVertices.push_back({center + (horizontalUv - Vector2(0.5f, 0.5f)) * size, horizontalUv, rectangle,
                                          Vector2(1.0f / size.x, 1.0f / size.y), line});
            // Retain line order and the original owner-local position. Halos
            // may overlap at output and must keep the same source-over order.
            if(quarterBlur)
            {
              handoffVertices.push_back({offset + (uv - Vector2(0.5f, 0.5f)) * size, uv, rectangle, line});
            }
            else
            {
              outputVertices.push_back({offset + (uv - Vector2(0.5f, 0.5f)) * size, uv, rectangle});
            }
          }
        }
        Actor source = pass.source;
        if(!batched)
        {
          source.SetProperty(Actor::Property::POSITION, offset);
        }
        Actor foreground = batched ? sharedForeground : mForegroundActor;
        if(perLine)
        {
          const bool imageSlot = mImages && std::any_of(mImages->bindings.begin(), mImages->bindings.end(), [&](const auto& image)
          {
            return image.placement.lineIndex == timing.lineIndex;
          });
          // Existing A8/RGBA page packing may place non-neighboring logical
          // lines beside one another. Keep source groups contiguous in the
          // original sequence as well as in the packed page.
          const bool sequenceBoundary = !sequenceOrder.empty() && sequence > batch.first &&
                                        sequenceOrder[sequence] != sequenceOrder[sequence - 1u] + 1u;
          if((imageSlot || sequenceBoundary) && !flushSource())
          {
            return;
          }
          if(timing.hasTextForeground && timing.atlasShader)
          {
            const auto shader = alphaOnly ? GetAlphaSourceShader(true) : timing.atlasShader;
            if(!Dali::Adaptor::IsAvailable())
            {
              return;
            }
            if((sourceTextures != timing.textures || sourceShader != shader ||
                sourceVertices.size() == MAX_SOURCE_LINES_PER_DRAW * 4u) &&
               !flushSource())
            {
              return;
            }
            sourceTextures = timing.textures;
            sourceShader   = shader;
            for(const Vector2 uv : {Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f)})
            {
              sourceVertices.push_back({uv - Vector2(0.5f, 0.5f), timing.textureRect, center - offset, timing.atlasRectangle});
            }
          }
          else if(timing.hasTextForeground)
          {
            if(!flushSource())
            {
              return;
            }
            Renderer renderer = CloneForeground(mForeground, foregroundProperties, timing.textures, timing.textureRect, mContentSize,
                                                batched ? center - offset : Vector2::ZERO);
            if(!Dali::Adaptor::IsAvailable() || !renderer)
            {
              return;
            }
            if(mDecorations)
            {
              renderer.SetShader(mDecorations->planes->foregroundShader);
            }
            if(alphaOnly)
            {
              renderer.SetShader(GetAlphaSourceShader());
            }
            foreground.AddRenderer(renderer);
          }
          if(imageSlot && !flushSource())
          {
            return;
          }
        }
        if(!batched)
        {
          foreground.SetProperty(Actor::Property::POSITION, -offset);
          source.Add(foreground);
        }
        if(mImages)
        {
          for(auto& image : mImages->bindings)
          {
            if(!perLine || image.placement.lineIndex == timing.lineIndex)
            {
              image.target = foreground;
              image.offset = batched ? center - offset : Vector2::ZERO;
            }
          }
        }
        if(!batched)
        {
          for(uint32_t i = 0u; i < 2u; ++i)
          {
            Actor blurActor = pass.blurActors[i];
            blurActor.SetProperty(Actor::Property::POSITION, offset);
            Renderer renderer = TextRevealBlurRenderer::Create(mRadius);
            if(!Dali::Adaptor::IsAvailable() || !renderer)
            {
              return;
            }
            BindTexture(renderer, pass.buffers[i].GetColorTexture());
            renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::OFF);
            renderer.RegisterProperty("uOpacity", 1.0f);
            renderer.RegisterProperty("uOffsetDirection", i == 0u ? Vector2(1.0f / size.x, 0.0f)
                                                                  : Vector2(0.0f, 1.0f / size.y));
            if(quarterBlur)
            {
              // Keep full-size camera/quads and normalized Gaussian offsets:
              // H reads full Source; V reads reduced H but still offsets by
              // sample/fullHeight, NOT sample/quarterHeight. Kernel unchanged.
              const auto input = pass.buffers[i].GetColorTexture();
              renderer.SetProperty(renderer.GetPropertyIndex("uRevealBatchInvSize"),
                                   Vector2(1.0f / input.GetWidth(), 1.0f / input.GetHeight()));
            }
            renderer.RegisterProperty("uRevealSequenceStart", timing.start);
            // Use the same update-side progress as the source and blur strength.
            // Do not gate tasks: their clears and shared-scratch ordering remain
            // necessary even when this sequence currently contributes no color.
            const auto progress = renderer.RegisterProperty("uTextRevealBlurProgress", owner.GetCurrentProperty<float>(mProgressIndex));
            if(!Dali::Adaptor::IsAvailable())
            {
              return;
            }
            auto mirror = Constraint::New<float>(renderer, progress, [](float& value, const PropertyInputContainer& inputs)
            {
              value = inputs[0]->GetFloat();
            });
            mirror.AddSource(Source(owner, mProgressIndex));
            mirror.Apply();
            BlurStrength strength{timing.start, mBlurDuration};
            auto         index = renderer.RegisterProperty("uAnimationRatio", strength.Evaluate(owner.GetCurrentProperty<float>(mProgressIndex)));
            if(!Dali::Adaptor::IsAvailable())
            {
              return;
            }
            auto constraint = Constraint::New<float>(renderer, index, strength);
            constraint.AddSource(Source(owner, mProgressIndex));
            constraint.Apply();
            blurActor.AddRenderer(renderer);
          }
        }
        if(!batched)
        {
          Renderer output = alphaOnly || quarterBlur ? CreateBlurOutput(mForeground, mForeground.GetGeometry(), alphaOnly, false, quarterBlur)
                                                     : CreatePlainOutput();
          if(!Dali::Adaptor::IsAvailable() || !output)
          {
            return;
          }
          BindTexture(output, pass.buffers[2].GetColorTexture());
          if(quarterBlur)
          {
            output.RegisterProperty("uQuarterAuthoredRadius", quarterAuthoredRadius);
            auto textures = output.GetTextures();
            textures.SetTexture(1u, pass.buffers[0u].GetColorTexture());
            textures.SetSampler(1u, textures.GetSampler(0u));
            BlurStrength strength{timing.start, mBlurDuration};
            const auto   index      = output.RegisterProperty("uAnimationRatio", strength.Evaluate(owner.GetCurrentProperty<float>(mProgressIndex)));
            auto         constraint = Constraint::New<float>(output, index, strength);
            constraint.AddSource(Source(owner, mProgressIndex));
            constraint.Apply();
          }
          output.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
          output.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
          if(!Dali::Adaptor::IsAvailable())
          {
            return;
          }
          Actor outputActor = NewPassActor("RevealGaussianOutput", size);
          outputActor.SetProperty(Actor::Property::POSITION, offset);
          outputActor.SetProperty(Actor::Property::COLOR_MODE, ColorMode::USE_PARENT_COLOR);
          outputActor.AddRenderer(output);
          outputParent.Add(outputActor);
        }
        y += size.y;
      }
      if(!flushSource())
      {
        return;
      }
      if(batched)
      {
        // Uniform limits bound a draw, not a page. Even large pages retain the
        // same tasks, scratch sharing and isolated output rectangles.
        for(size_t first = 0u; first < batch.count; first += TextRevealBlurRenderer::MAX_LINES_PER_DRAW)
        {
          const auto count    = static_cast<uint32_t>(std::min(batch.count - first,
                                                               static_cast<size_t>(TextRevealBlurRenderer::MAX_LINES_PER_DRAW)));
          Geometry   geometry = CreateBlurBatchGeometry(vertices.data() + first * 4u, count);
          if(!Dali::Adaptor::IsAvailable() || !geometry)
          {
            return;
          }
          // H changes X support only: it cannot populate a transparent source
          // row. Draw just its filtered Y coverage; V retains the full padded
          // quads and creates the vertical halo. The full H task clear below
          // remains essential, including when pages share scratch storage.
          Geometry   horizontalGeometry = geometry;
          const auto bandFirst          = horizontalBands.begin() + static_cast<std::vector<Vector2>::difference_type>(batch.first + first);
          if(std::any_of(bandFirst, bandFirst + count, [](const Vector2& band)
          {
            return band.x > 0.0f || band.y < 1.0f;
          }))
          {
            auto candidate = CreateBlurBatchGeometry(horizontalVertices.data() + first * 4u, count);
            if(!Dali::Adaptor::IsAvailable())
            {
              return;
            }
            if(candidate)
            {
              horizontalGeometry = candidate;
            }
          }
          for(uint32_t i = 0u; i < 2u; ++i)
          {
            Renderer renderer = TextRevealBlurRenderer::CreateBatch(mRadius, i == 0u ? horizontalGeometry : geometry);
            if(!Dali::Adaptor::IsAvailable() || !renderer)
            {
              return;
            }
            BindTexture(renderer, pass.buffers[i].GetColorTexture());
            renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::OFF);
            renderer.RegisterProperty("uOpacity", 1.0f);
            renderer.RegisterProperty("uOffsetDirection", i == 0u ? Vector2(1.0f, 0.0f) : Vector2(0.0f, 1.0f));
            renderer.SetProperty(renderer.GetPropertyIndex("uRevealBatchInvSize"), Vector2(1.0f / pass.size.x, 1.0f / pass.size.y));
            if(quarterBlur)
            {
              // Vertex offsets still use each full-resolution line extent;
              // only the input tile's half-texel clamp follows its resolution.
              const auto input = pass.buffers[i].GetColorTexture();
              renderer.SetProperty(renderer.GetPropertyIndex("uRevealBatchInvSize"),
                                   Vector2(1.0f / input.GetWidth(), 1.0f / input.GetHeight()));
            }
            const float progressValue = owner.GetCurrentProperty<float>(mProgressIndex);
            const auto  progress      = renderer.RegisterProperty("uTextRevealBlurProgress", progressValue);
            if(!Dali::Adaptor::IsAvailable())
            {
              return;
            }
            auto mirror = Constraint::New<float>(renderer, progress, [](float& value, const PropertyInputContainer& inputs)
            {
              value = inputs[0]->GetFloat();
            });
            mirror.AddSource(Source(owner, mProgressIndex));
            mirror.Apply();
            for(uint32_t line = 0u; line < count; ++line)
            {
              const auto&        timing = mSequences[batch.first + first + line];
              const BlurStrength strength{timing.start, mBlurDuration};
              const std::string  name  = "uRevealBlurState[" + std::to_string(line) + "]";
              const auto         index = renderer.RegisterProperty(name.c_str(), Vector2(strength.Evaluate(progressValue), timing.start));
              if(!Dali::Adaptor::IsAvailable())
              {
                return;
              }
              auto constraint = Constraint::New<Vector2>(renderer, index, [strength](Vector2& value, const PropertyInputContainer& inputs)
              {
                value = Vector2(strength.Evaluate(inputs[0]->GetFloat()), strength.start);
              });
              constraint.AddSource(Source(owner, mProgressIndex));
              constraint.Apply();
            }
            // The task source already has the page's size and coordinate
            // system. Draw renderers do not need their own child actors.
            pass.blurActors[i].AddRenderer(renderer);
          }
          size_t outputFirst = first;
          while(outputFirst < first + count)
          {
            size_t outputEnd = outputFirst + 1u;
            // Combine only consecutive presentation lines in this page. The
            // handoff keeps each quad's strength and the original halo order.
            while(outputEnd < first + count &&
                  (sequenceOrder.empty() || sequenceOrder[batch.first + outputEnd] == sequenceOrder[batch.first + outputEnd - 1u] + 1u))
            {
              ++outputEnd;
            }
            const auto outputCount    = static_cast<uint32_t>(outputEnd - outputFirst);
            auto       outputGeometry = quarterBlur ? CreateBlurBatchGeometry(handoffVertices.data() + outputFirst * 4u, outputCount)
                                                    : CreateBlurBatchGeometry(outputVertices.data() + outputFirst * 4u, outputCount);
            Renderer   output         = CreateBlurOutput(mForeground, outputGeometry, alphaOnly, true, quarterBlur);
            if(!Dali::Adaptor::IsAvailable() || !output)
            {
              return;
            }
            BindTexture(output, pass.buffers[2].GetColorTexture());
            if(quarterBlur)
            {
              output.RegisterProperty("uQuarterAuthoredRadius", quarterAuthoredRadius);
              auto textures = output.GetTextures();
              textures.SetTexture(1u, pass.buffers[0u].GetColorTexture());
              textures.SetSampler(1u, textures.GetSampler(0u));
              for(size_t line = outputFirst; line < outputEnd; ++line)
              {
                const auto&        timing = mSequences[batch.first + line];
                const BlurStrength strength{timing.start, mBlurDuration};
                // Indices are local to the H/V draw, not to a split output
                // range (mixed-format pages can have gaps in logical order).
                const std::string name  = "uRevealBlurStrength[" + std::to_string(line - first) + "]";
                const auto        index = output.RegisterProperty(name.c_str(), strength.Evaluate(owner.GetCurrentProperty<float>(mProgressIndex)));
                if(!Dali::Adaptor::IsAvailable())
                {
                  return;
                }
                auto constraint = Constraint::New<float>(output, index, strength);
                constraint.AddSource(Source(owner, mProgressIndex));
                constraint.Apply();
              }
            }
            output.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
            output.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
            if(!Dali::Adaptor::IsAvailable())
            {
              return;
            }
            Actor outputActor = NewPassActor("RevealGaussianOutput", mTargetSize);
            outputActor.SetProperty(Actor::Property::COLOR_MODE, ColorMode::USE_PARENT_COLOR);
            outputActor.AddRenderer(output);
            if(sequenceOrder.empty())
            {
              outputParent.Add(outputActor);
            }
            else
            {
              orderedOutputs.emplace_back(sequenceOrder[batch.first + outputFirst], outputActor);
            }
            outputFirst = outputEnd;
          }
        }
      }
    }
    std::sort(orderedOutputs.begin(), orderedOutputs.end(), [](const auto& left, const auto& right)
    {
      return left.first < right.first;
    });
    for(auto& output : orderedOutputs)
    {
      outputParent.Add(output.second);
    }
    if(mDecorations && mDecorations->overlay)
    {
      // Actor traversal orders the overlay after every blurred line output.
      mDecorations->source.Add(mDecorations->overlay);
    }
  }

  void OnChildAdd(Actor&) override
  {
  }
  void OnChildRemove(Actor&) override
  {
  }
  void OnSizeSet(const Vector3&) override
  {
  }
  void OnSizeAnimation(Animation&, const Vector3&) override
  {
  }

protected:
  void OnSceneConnection(int /*depth*/) override
  {
    auto sceneHolder = Dali::Integration::SceneHolder::Get(Self());
    if(!Dali::Adaptor::IsAvailable() || !sceneHolder || mReleased)
    {
      return;
    }
    mTaskList     = sceneHolder.GetRenderTaskList();
    auto taskList = mTaskList;
    for(auto& pass : mPasses)
    {
      for(uint32_t i = 0u; i < 3u; ++i)
      {
        auto task = taskList.CreateTask();
        // CreateTask/its camera can emit ObjectCreated before the returned
        // handle is stored. A disconnect only removes this candidate's tasks.
        if(!Dali::Adaptor::IsAvailable() || mReleased || !mTaskList)
        {
          taskList.RemoveTask(task);
          return;
        }
        pass.tasks[i] = task;
        pass.tasks[i].SetSourceActor(i == 0u ? pass.source : pass.blurActors[i - 1u]);
        pass.tasks[i].SetBuiltinCameraActor(RenderTask::BuiltinCameraType::ATTACHED_TO_SOURCE_ACTOR, pass.size,
                                            Property::Map().Add(CameraActor::Property::INVERT_Y_AXIS, true));
        if(!Dali::Adaptor::IsAvailable() || mReleased || !mTaskList)
        {
          return;
        }
        pass.tasks[i].SetExclusive(true);
        pass.tasks[i].SetInputEnabled(false);
        pass.tasks[i].SetFrameBuffer(pass.buffers[i]);
        pass.tasks[i].SetClearEnabled(true);
        pass.tasks[i].SetClearColor(Color::TRANSPARENT);
        pass.tasks[i].SetRefreshRate(RenderTask::REFRESH_ALWAYS);
      }
    }
    if(mDecorations)
    {
      auto task = taskList.CreateTask();
      if(!Dali::Adaptor::IsAvailable() || mReleased || !mTaskList)
      {
        taskList.RemoveTask(task);
        return;
      }
      mDecorations->task = task;
      task.SetSourceActor(mDecorations->source);
      task.SetBuiltinCameraActor(RenderTask::BuiltinCameraType::ATTACHED_TO_SOURCE_ACTOR, mTargetSize,
                                 Property::Map().Add(CameraActor::Property::INVERT_Y_AXIS, true));
      if(!Dali::Adaptor::IsAvailable() || mReleased || !mTaskList)
      {
        return;
      }
      task.SetExclusive(true);
      task.SetInputEnabled(false);
      task.SetFrameBuffer(mDecorations->buffer);
      task.SetClearEnabled(true);
      task.SetClearColor(Color::TRANSPARENT);
      task.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
    }
    RequestRenderTaskReorder();
  }

  void OnSceneDisconnection() override
  {
    if(mDecorations && mDecorations->task)
    {
      mTaskList.RemoveTask(mDecorations->task);
      mDecorations->task.Reset();
    }
    for(auto& pass : mPasses)
    {
      for(auto& task : pass.tasks)
      {
        if(task)
        {
          mTaskList.RemoveTask(task);
          task.Reset();
        }
      }
    }
    mTaskList.Reset();
  }

private:
  Renderer                  mForeground;
  TextureSet                mSourceTextures;
  Shader                    mSourceShader;
  Shader                    mForegroundShader;
  Shader                    mCaptureShader;
  bool                      mForegroundBorrowed{false};
  bool                      mReleased{false};
  bool                      mPerLine{false};
  Vector2                   mContentSize;
  Vector2                   mTargetSize;
  uint32_t                  mRadius;
  float                     mBlurDuration;
  RuntimeRevealBlurSettings mSettings;
  bool                      mSingleColor;
  Property::Index           mProgressIndex;
  WeakHandle<Actor>         mOwner;
  Actor                     mForegroundActor;
  struct Pass
  {
    Vector2                     size;
    Actor                       source;
    std::array<Actor, 2u>       blurActors;
    std::array<FrameBuffer, 3u> buffers;
    std::array<RenderTask, 3u>  tasks;
  };
  std::vector<RuntimeRevealBlurSequence> mSequences;
  std::vector<Pass>                      mPasses;
  RenderTaskList                         mTaskList;
  std::unique_ptr<DecorationComposition> mDecorations;
  std::unique_ptr<ImageCapture>          mImages;
};

// Only Core actor properties are required; no UI View/layout state is created.
// This private type is not creatable through the public registry.
TypeRegistration runtimeBlurType(typeid(RuntimeBlurActor), typeid(CustomActor), []() -> BaseHandle
{
  return {};
});
} // unnamed namespace

std::vector<RuntimeRevealBlurBatch> BuildRuntimeRevealBlurBatches(const std::vector<Vector2>& sizes, uint32_t maxTextureSize)
{
  // Bound storage, not the number of independently timed lines. The 1M-pixel
  // limit caps each new page at 4 MiB in RGBA; do not enlarge already-large
  // individual targets. Page area may exceed occupied area by at most 25%.
  constexpr uint64_t                  MAX_PIXELS = 1024u * 1024u;
  std::vector<RuntimeRevealBlurBatch> batches;
  if(sizes.empty())
  {
    return batches;
  }

  // Prefer one page for the whole Label. A short prefix can pack poorly even
  // when the complete set fits, so do not reject it on prefix occupancy alone.
  RuntimeRevealBlurBatch whole{0u, sizes.size(), Vector2::ZERO};
  uint64_t               occupied = 0u;
  bool                   fits     = true;
  for(const auto& size : sizes)
  {
    whole.size.x = std::max(whole.size.x, size.x);
    whole.size.y += size.y;
    occupied += static_cast<uint64_t>(size.x) * static_cast<uint64_t>(size.y);
    if(whole.size.x > static_cast<float>(maxTextureSize) || whole.size.y > static_cast<float>(maxTextureSize) ||
       static_cast<uint64_t>(whole.size.x) * static_cast<uint64_t>(whole.size.y) > MAX_PIXELS)
    {
      fits = false;
      break;
    }
  }
  if(fits && static_cast<uint64_t>(whole.size.x) * static_cast<uint64_t>(whole.size.y) * 4u <= occupied * 5u)
  {
    batches.push_back(whole);
    return batches;
  }

  // Use the established fallback as a storage/task ceiling. Growing pages
  // independently can lose equal-sized source/H scratch reuse.
  constexpr size_t MAX_FALLBACK_LINES = 4u;
  batches.reserve(sizes.size());
  for(size_t first = 0u; first < sizes.size();)
  {
    RuntimeRevealBlurBatch batch{first, 1u, sizes[first]};
    uint64_t               used = static_cast<uint64_t>(batch.size.x) * static_cast<uint64_t>(batch.size.y);
    while(batch.count < MAX_FALLBACK_LINES && first + batch.count < sizes.size())
    {
      const auto&   next = sizes[first + batch.count];
      const Vector2 size(std::max(batch.size.x, next.x), batch.size.y + next.y);
      if(size.x > static_cast<float>(maxTextureSize) || size.y > static_cast<float>(maxTextureSize))
      {
        break;
      }
      const uint64_t pixels         = static_cast<uint64_t>(size.x) * static_cast<uint64_t>(size.y);
      const uint64_t occupiedPixels = used + static_cast<uint64_t>(next.x) * static_cast<uint64_t>(next.y);
      if(pixels > MAX_PIXELS || pixels * 4u > occupiedPixels * 5u)
      {
        break;
      }
      batch.size = size;
      ++batch.count;
      used = occupiedPixels;
    }
    first += batch.count;
    batches.push_back(batch);
  }
  if(batches.size() == 1u)
  {
    return batches;
  }

  // Each page retains V, while pages of identical dimensions share source/H.
  // All pages use the same format, so pixel counts compare actual FBO storage
  // for both the alpha-only and RGBA paths without a format-dependent planner.
  std::vector<Vector2> dimensions;
  dimensions.reserve(batches.size());
  for(const auto& page : batches)
  {
    dimensions.push_back(page.size);
  }
  const auto storagePixels = [&dimensions]()
  {
    std::sort(dimensions.begin(), dimensions.end(), [](const Vector2& left, const Vector2& right)
    {
      return left.x < right.x || (left.x == right.x && left.y < right.y);
    });
    uint64_t pixels = 0u;
    for(size_t index = 0u; index < dimensions.size(); ++index)
    {
      const uint64_t area = static_cast<uint64_t>(dimensions[index].x) * static_cast<uint64_t>(dimensions[index].y);
      pixels += area;
      if(index == 0u || dimensions[index] != dimensions[index - 1u])
      {
        pixels += 2u * area;
      }
    }
    return pixels;
  };
  const uint64_t storageBudget        = storagePixels();
  uint64_t       bestStorage          = storageBudget;
  uint64_t       unpaddedStorageLimit = storageBudget;
  // First/count are implicit in each balanced partition. Keep only dimensions
  // until a candidate wins, and reuse both size lists throughout the search.
  std::vector<Vector2> candidateSizes;
  candidateSizes.reserve(batches.size());
  const auto tryBalanced = [&](size_t pageCount)
  {
    candidateSizes.clear();
    Vector2  commonSize    = Vector2::ZERO;
    uint64_t leastOccupied = std::numeric_limits<uint64_t>::max();
    for(size_t page = 0u, first = 0u; page < pageCount; ++page)
    {
      const size_t           count = sizes.size() / pageCount + (page < sizes.size() % pageCount ? 1u : 0u);
      RuntimeRevealBlurBatch batch{first, count, Vector2::ZERO};
      uint64_t               used = 0u;
      for(size_t line = first; line < first + count; ++line)
      {
        batch.size.x = std::max(batch.size.x, sizes[line].x);
        batch.size.y += sizes[line].y;
        used += static_cast<uint64_t>(sizes[line].x) * static_cast<uint64_t>(sizes[line].y);
      }
      const uint64_t pixels = static_cast<uint64_t>(batch.size.x) * static_cast<uint64_t>(batch.size.y);
      // Leave unpadded single targets to publication checks. For combined
      // targets, check the complete page as in the whole-Label fast path.
      if(count > 1u && (batch.size.x > static_cast<float>(maxTextureSize) || batch.size.y > static_cast<float>(maxTextureSize) ||
                        pixels > MAX_PIXELS || pixels * 4u > used * 5u))
      {
        return;
      }
      commonSize.x  = std::max(commonSize.x, batch.size.x);
      commonSize.y  = std::max(commonSize.y, batch.size.y);
      leastOccupied = std::min(leastOccupied, used);
      first += count;
      candidateSizes.push_back(batch.size);
    }
    dimensions.assign(candidateSizes.begin(), candidateSizes.end());
    uint64_t storage             = storagePixels();
    unpaddedStorageLimit         = std::min(unpaddedStorageLimit, storage);
    const uint64_t commonPixels  = static_cast<uint64_t>(commonSize.x) * static_cast<uint64_t>(commonSize.y);
    const uint64_t sharedStorage = (static_cast<uint64_t>(pageCount) + 2u) * commonPixels;
    // Padding a short page can remove a separate source/H pair. Enforce the
    // occupancy limit on every padded page and keep the unpadded candidates'
    // storage ceiling; do not trade their memory savings for fewer tasks.
    const bool normalize = commonSize.x <= static_cast<float>(maxTextureSize) && commonSize.y <= static_cast<float>(maxTextureSize) &&
                           commonPixels <= MAX_PIXELS && commonPixels * 4u <= leastOccupied * 5u &&
                           sharedStorage < storage && sharedStorage <= unpaddedStorageLimit;
    if(normalize)
    {
      storage = sharedStorage;
    }
    if(storage <= storageBudget && (pageCount < batches.size() || storage < bestStorage))
    {
      bestStorage = storage;
      batches.resize(pageCount);
      for(size_t page = 0u, first = 0u; page < pageCount; ++page)
      {
        const size_t count = sizes.size() / pageCount + (page < sizes.size() % pageCount ? 1u : 0u);
        batches[page]      = {first, count, normalize ? commonSize : candidateSizes[page]};
        first += count;
      }
    }
  };

  // Equal line-count partitions often recover source/H sharing, including an
  // uneven tail in the old fallback. Keep logical order and try only distinct
  // integer quotients: O(sqrt(N)) candidates, not every possible partition.
  // Counts descend: a later unpadded candidate that lowers the storage ceiling
  // also wins on page count. Padding cannot increase the unpadded result's cost.
  tryBalanced(batches.size());
  for(size_t divisor = 1u; divisor <= sizes.size() / 2u;)
  {
    const size_t pageCount = sizes.size() / divisor;
    divisor                = sizes.size() / pageCount + 1u;
    if(pageCount < batches.size())
    {
      tryBalanced(pageCount);
    }
  }
  return batches;
}

Rect<int32_t> ResolveRuntimeRevealBlurTarget(Renderer foreground, const Vector2& controlSize,
                                             const Vector2& textureSize, const Rect<uint32_t>& coverage,
                                             uint32_t radius, Vector2* horizontalBand)
{
  const int32_t       fullWidth  = static_cast<int32_t>(std::ceil(controlSize.x)) + 2 * static_cast<int32_t>(radius + 2u);
  const int32_t       fullHeight = static_cast<int32_t>(std::ceil(controlSize.y)) + 2 * static_cast<int32_t>(radius + 2u);
  const Rect<int32_t> full(0, 0, fullWidth, fullHeight);
  if(horizontalBand)
  {
    *horizontalBand = Vector2(0.0f, static_cast<float>(fullHeight));
  }
  if(coverage.width == 0u || coverage.height == 0u || textureSize.x <= 0.0f || textureSize.y <= 0.0f)
  {
    return full;
  }
  using P              = VisualRenderer::Property;
  const Vector2 size   = foreground.GetProperty<Vector2>(P::TRANSFORM_SIZE);
  const Vector2 extra  = foreground.GetProperty<Vector2>(P::EXTRA_SIZE);
  const Vector2 offset = foreground.GetProperty<Vector2>(P::TRANSFORM_OFFSET);
  const Vector2 origin = foreground.GetProperty<Vector2>(P::TRANSFORM_ORIGIN);
  const Vector2 pivot  = foreground.GetProperty<Vector2>(P::TRANSFORM_PIVOT);
  const Vector4 mode   = foreground.GetProperty<Vector4>(P::TRANSFORM_OFFSET_SIZE_MODE);
  const Vector2 fullSize(static_cast<float>(fullWidth), static_cast<float>(fullHeight));
  const Vector2 first(static_cast<float>(coverage.x), static_cast<float>(coverage.y));
  const Vector2 last(static_cast<float>(coverage.x + coverage.width), static_cast<float>(coverage.y + coverage.height));
  // TextVisual's fitting transform already includes UI scale and disables its
  // shader's extra effective-scale multiplication. Map raster coverage into
  // that displayed extent, including async supersampled render-scale textures.
  const Vector2 extent(size.x * (controlSize.x * (1.0f - mode.z) + mode.z) + extra.x,
                       size.y * (controlSize.y * (1.0f - mode.w) + mode.w) + extra.y);
  const Vector2 start(offset.x * (controlSize.x * (1.0f - mode.x) + mode.x) + origin.x * controlSize.x + (pivot.x - 0.5f) * extent.x + fullSize.x * 0.5f,
                      offset.y * (controlSize.y * (1.0f - mode.y) + mode.y) + origin.y * controlSize.y + (pivot.y - 0.5f) * extent.y + fullSize.y * 0.5f);
  if(!(extent.x > 0.0f && extent.y > 0.0f) || !std::isfinite(extent.x + extent.y + start.x + start.y))
  {
    return full;
  }
  const float guard = static_cast<float>(radius + 4u);
  auto        bound = [](float value, int32_t limit)
  {
    return static_cast<int32_t>(std::clamp(value, 0.0f, static_cast<float>(limit)));
  };
  const int32_t left   = bound(std::floor(start.x + first.x * extent.x / textureSize.x) - guard, fullWidth);
  const int32_t top    = bound(std::floor(start.y + first.y * extent.y / textureSize.y) - guard, fullHeight);
  const int32_t right  = bound(std::ceil(start.x + last.x * extent.x / textureSize.x) + guard, fullWidth);
  const int32_t bottom = bound(std::ceil(start.y + last.y * extent.y / textureSize.y) + guard, fullHeight);
  if(horizontalBand && right > left && bottom > top &&
     std::isfinite(textureSize.x) && std::isfinite(textureSize.y) &&
     static_cast<double>(coverage.x) + coverage.width <= textureSize.x &&
     static_cast<double>(coverage.y) + coverage.height <= textureSize.y)
  {
    // LINEAR source sampling extends half a source texel beyond the covered
    // pixel cells. Map that support through UI/render scale, then round out.
    // One capture texel also covers H's LINEAR footprint/texel-center rounding.
    // At 1:1 this retains two rows beyond coverage, like the source crop guard;
    // unlike a fixed pixel margin it remains conservative when magnified.
    constexpr float HORIZONTAL_PASS_FILTER_GUARD = 1.0f;
    const float     filterGuard                  = 0.5f * extent.y / textureSize.y + HORIZONTAL_PASS_FILTER_GUARD;
    const float     bandTop                      = std::floor(start.y + first.y * extent.y / textureSize.y - filterGuard);
    const float     bandBottom                   = std::ceil(start.y + last.y * extent.y / textureSize.y + filterGuard);
    if(std::isfinite(bandTop) && std::isfinite(bandBottom) && bandTop < bandBottom)
    {
      const int32_t clippedTop    = std::clamp(bound(bandTop, fullHeight), top, bottom);
      const int32_t clippedBottom = std::clamp(bound(bandBottom, fullHeight), top, bottom);
      if(clippedTop < clippedBottom)
      {
        *horizontalBand = Vector2(static_cast<float>(clippedTop), static_cast<float>(clippedBottom));
      }
    }
  }
  return right > left && bottom > top ? Rect<int32_t>(left, top, right - left, bottom - top) : full;
}

RuntimeRevealBlurSettings ResolveRuntimeRevealBlurSettings(Ui::Integration::Text::Reveal::BlurMode mode, float authoredRadius)
{
  const bool fullResolution = mode == Ui::Integration::Text::Reveal::BlurMode::QUALITY;
  return {fullResolution ? RuntimeRevealBlurPath::FULL_RESOLUTION : RuntimeRevealBlurPath::AXIS_AWARE_QUARTER,
          authoredRadius};
}

Actor PrepareRuntimeRevealBlur(Actor owner, Renderer foreground, const Vector2& size,
                               Property::Index progressIndex, uint32_t radius, float blurDuration,
                               std::vector<RuntimeRevealBlurSequence> sequences, bool singleColor,
                               std::unique_ptr<RuntimeRevealBlurDecorations> decorations,
                               std::unique_ptr<RuntimeRevealBlurImages>      images,
                               RuntimeRevealBlurSettings                     settings)
{
  if(!Dali::Adaptor::IsAvailable() || !Ui::View::DownCast(owner) || !foreground || !Dali::Integration::SceneHolder::Get(owner) ||
     !foreground.GetTextures() || foreground.GetTextures().GetTextureCount() == 0u || !foreground.GetTextures().GetTexture(0u) ||
     !foreground.GetGeometry() || !foreground.GetShader() ||
     progressIndex == Property::INVALID_INDEX || owner.GetPropertyType(progressIndex) != Property::FLOAT ||
     !owner.IsPropertyAConstraintInput(progressIndex) ||
     !(size.x > 0.0f && size.y > 0.0f && blurDuration > 0.0f) || !std::isfinite(size.x + size.y + blurDuration) ||
     radius != ResolveRevealBlurRadius(static_cast<float>(radius)) || radius == 0u)
  {
    return {};
  }
  const auto maximum = Dali::GetMaxTextureSize();
  const auto halo    = 2.0f * static_cast<float>(radius + 2u);
  if(maximum <= 0 || std::ceil(size.x) + halo >= static_cast<float>(maximum) ||
     std::ceil(size.y) + halo >= static_cast<float>(maximum))
  {
    return {};
  }
  for(const auto& sequence : sequences)
  {
    const auto& rectangle = sequence.textureRect;
    if(!std::isfinite(sequence.start) || sequence.start < 0.0f || sequence.start > 1.0f ||
       !std::isfinite(rectangle.x + rectangle.y + rectangle.z + rectangle.w) ||
       rectangle.x < 0.0f || rectangle.y < 0.0f || rectangle.z <= 0.0f || rectangle.w <= 0.0f ||
       rectangle.x + rectangle.z > 1.000001f || rectangle.y + rectangle.w > 1.000001f ||
       (sequence.hasTextForeground && (!sequence.textures || sequence.textures.GetTextureCount() == 0u || !sequence.textures.GetTexture(0u))))
    {
      return {};
    }
    if(!sequence.hasTextForeground && (!images || std::none_of(images->placements.begin(), images->placements.end(), [&](const auto& image)
    {
      return image.lineIndex == sequence.lineIndex;
    })))
    {
      return {};
    }
  }
  if(!(settings.authoredRadius > 0.0f) || !std::isfinite(settings.authoredRadius))
  {
    settings.authoredRadius = static_cast<float>(radius);
  }
  auto*       impl = new RuntimeBlurActor(owner, foreground, size, progressIndex, radius, blurDuration, std::move(sequences), singleColor, std::move(decorations), std::move(images), settings);
  CustomActor companion(*impl);
  if(!Dali::Adaptor::IsAvailable() || !Dali::Integration::SceneHolder::Get(owner))
  {
    return {};
  }
  impl->Initialize();
  if(!Dali::Adaptor::IsAvailable() || !Dali::Integration::SceneHolder::Get(owner))
  {
    return {};
  }
  return companion;
}

bool ActivateRuntimeRevealBlur(Actor companion, Actor owner)
{
  if(!Dali::Adaptor::IsAvailable() || !companion || !owner || !Dali::Integration::SceneHolder::Get(owner))
  {
    return false;
  }
  auto& impl = static_cast<RuntimeBlurActor&>(CustomActor::DownCast(companion).GetImplementation());
  if(!impl.BorrowForeground(owner))
  {
    return false;
  }
  Ui::View view = Ui::View::DownCast(owner);
  Integration::View::AllowToAddActorToChildBegin(view);
  owner.Add(companion);
  Integration::View::AllowToAddActorToChildEnd(view);
  return impl.IsActive();
}

bool HasCurrentRuntimeRevealBlurForeground(Actor companion)
{
  return companion && static_cast<RuntimeBlurActor&>(CustomActor::DownCast(companion).GetImplementation()).HasCurrentForeground();
}

Actor CreateRuntimeRevealBlur(Actor owner, Renderer foreground, const Vector2& size,
                              Property::Index progressIndex, uint32_t radius, float blurDuration,
                              std::vector<RuntimeRevealBlurSequence> sequences, bool singleColor,
                              std::unique_ptr<RuntimeRevealBlurDecorations> decorations,
                              std::unique_ptr<RuntimeRevealBlurImages>      images,
                              RuntimeRevealBlurSettings                     settings)
{
  auto companion = PrepareRuntimeRevealBlur(owner, foreground, size, progressIndex, radius, blurDuration,
                                            std::move(sequences), singleColor, std::move(decorations), std::move(images), settings);
  if(!ActivateRuntimeRevealBlur(companion, owner))
  {
    RemoveRuntimeRevealBlur(companion, owner);
  }
  return companion;
}

void RefreshRuntimeRevealBlurImages(Actor companion)
{
  if(companion)
  {
    auto& impl = static_cast<RuntimeBlurActor&>(CustomActor::DownCast(companion).GetImplementation());
    impl.RefreshImages();
  }
}

void RemoveRuntimeRevealBlur(Actor& companion, Actor owner)
{
  if(companion)
  {
    // Detach authority first: callbacks must not retire this object twice or
    // overwrite a newer companion assigned through the same reference.
    Actor    retiring   = std::move(companion);
    auto&    impl       = static_cast<RuntimeBlurActor&>(CustomActor::DownCast(retiring).GetImplementation());
    Renderer foreground = impl.ReleaseForeground();
    if(foreground && owner)
    {
      owner.AddRenderer(foreground);
    }
    retiring.Unparent();
  }
}
} //namespace DALI_NAMESPACE::Ui::Internal
