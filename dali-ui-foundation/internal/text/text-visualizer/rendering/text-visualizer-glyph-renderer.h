#ifndef DALI_UI_TEXT_VISUALIZER_GLYPH_RENDERER_H
#define DALI_UI_TEXT_VISUALIZER_GLYPH_RENDERER_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/public-api/rendering/vertex-buffer.h>

#include <cstdint>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/rendering/atlas/atlas-glyph-manager.h>

namespace Dali::Ui::Internal::TextVisualizer
{
class AtlasViewAdapter;
class LayoutResult;
class PreparedText;

/**
 * @brief Phase 2 prototype for a TextVisualizer-only lightweight glyph renderer.
 *
 * This class is intentionally not connected to the active TextVisualizer render path yet.
 * The current mesh MVP only uses already-cached atlas glyphs and returns false on cache miss.
 * The existing Text::AtlasRenderer path remains unchanged.
 */
class TextVisualizerGlyphRenderer
{
public:
  enum class RenderFailureReason : uint32_t
  {
    NONE = 0u,
    EMPTY_INPUT,
    NO_RENDERABLE_GLYPHS,
    NO_POSITION_CACHE,
    SIGNATURE_FAILED,
    NO_GLYPH_MANAGER,
    GLYPH_PLACEMENT_FAILED,
    GLYPH_INFO_FAILED,
    GLYPH_POSITION_FAILED,
    GLYPH_CACHE_MISS,
    EMPTY_MESH,
    NO_OUTPUT_ACTOR,
    NO_TEXTURE_SET
  };

  TextVisualizerGlyphRenderer();
  ~TextVisualizerGlyphRenderer();

  void Clear();

  bool  HasOutputActor() const;
  Actor GetOutputActor() const;

  bool IsAttached() const;

  void  SetRenderHost(Actor renderHost);
  Actor GetRenderHost() const;
  bool  HasRenderHost() const;

  void EnsureOutputActor();
  bool AttachOutputToHost();
  void DetachOutputFromHost();

  void     ClearMeshes();
  bool     HasMeshRecords() const;
  uint32_t GetMeshRecordCount() const;
  bool     HasMeshTopologySignature() const;
  uint64_t GetMeshTopologySignature() const;
  uint32_t GetGeometryOnlyUpdateCount() const;
  uint32_t GetFullMeshRebuildCount() const;

  void     ClearGlyphCache();
  bool     HasGlyphCacheEntries() const;
  uint32_t GetGlyphCacheEntryCount() const;
  bool     HasGlyphCacheSignature() const;
  uint64_t GetGlyphCacheSignature() const;

  RenderFailureReason GetLastFailureReason() const;
  uint32_t            GetFailureEmptyInputCount() const;
  uint32_t            GetFailureNoRenderableGlyphsCount() const;
  uint32_t            GetFailureNoPositionCacheCount() const;
  uint32_t            GetFailureSignatureCount() const;
  uint32_t            GetFailureNoGlyphManagerCount() const;
  uint32_t            GetFailureGlyphPlacementCount() const;
  uint32_t            GetFailureGlyphInfoCount() const;
  uint32_t            GetFailureGlyphPositionCount() const;
  uint32_t            GetFailureCacheMissCount() const;
  uint32_t            GetFailureEmptyMeshCount() const;
  uint32_t            GetFailureNoOutputActorCount() const;
  uint32_t            GetFailureNoTextureSetCount() const;
  uint32_t            GetLastFailedPlacementIndex() const;
  uint32_t            GetLastFailedGlyphIndex() const;
  Text::FontId        GetLastFailedFontId() const;
  Text::GlyphIndex    GetLastFailedGlyphId() const;
  float               GetLastFailedGlyphAdvance() const;
  float               GetLastFailedGlyphWidth() const;
  float               GetLastFailedGlyphHeight() const;
  float               GetLastFailedGlyphXBearing() const;
  float               GetLastFailedGlyphYBearing() const;

  bool Render(const PreparedText& preparedText, const LayoutResult& layoutResult, const AtlasViewAdapter& adapter, const Vector4& textColor);
  bool Render(const AtlasViewAdapter& adapter);

private:
  struct GlyphCacheEntry
  {
    Text::FontId                            fontId{0u};
    Text::GlyphIndex                        glyphIndex{0u};
    Dali::Ui::AtlasGlyphManager::GlyphStyle style;
    uint32_t                                atlasId{0u};
    uint32_t                                imageId{0u};
  };

  struct MeshRecord
  {
    uint32_t     atlasId{0u};
    Actor        actor;
    Renderer     renderer;
    Geometry     geometry;
    VertexBuffer vertexBuffer;
    uint32_t     vertexCount{0u};
    uint32_t     indexCount{0u};
  };

  bool CalculateGlyphCacheSignature(const AtlasViewAdapter& adapter, uint64_t& signature);
  bool CanReuseGlyphCache(uint64_t signature) const;
  bool AcquireGlyphReferences(const AtlasViewAdapter& adapter, std::vector<GlyphCacheEntry>& entries);
  void ReleaseGlyphReferences(std::vector<GlyphCacheEntry>& entries) const;
  bool RenderAdapter(const AtlasViewAdapter& adapter, const Vector4& textColor);
  void ResetFailureDiagnostics();
  void RecordFailure(RenderFailureReason reason, uint32_t placementIndex = 0u, uint32_t glyphIndex = 0u, const Text::GlyphInfo* glyphInfo = nullptr);

  Actor mRenderHost;
  Actor mOutputActor;
  bool  mAttached;

  std::vector<GlyphCacheEntry> mGlyphCacheEntries;
  uint64_t                     mGlyphCacheSignature;
  bool                         mHasGlyphCacheSignature;

  std::vector<MeshRecord> mMeshRecords;
  uint64_t                mMeshTopologySignature;
  bool                    mHasMeshTopologySignature;
  uint32_t                mGeometryOnlyUpdateCount;
  uint32_t                mFullMeshRebuildCount;

  RenderFailureReason mLastFailureReason;
  uint32_t            mFailureEmptyInputCount;
  uint32_t            mFailureNoRenderableGlyphsCount;
  uint32_t            mFailureNoPositionCacheCount;
  uint32_t            mFailureSignatureCount;
  uint32_t            mFailureNoGlyphManagerCount;
  uint32_t            mFailureGlyphPlacementCount;
  uint32_t            mFailureGlyphInfoCount;
  uint32_t            mFailureGlyphPositionCount;
  uint32_t            mFailureCacheMissCount;
  uint32_t            mFailureEmptyMeshCount;
  uint32_t            mFailureNoOutputActorCount;
  uint32_t            mFailureNoTextureSetCount;
  uint32_t            mLastFailedPlacementIndex;
  uint32_t            mLastFailedGlyphIndex;
  Text::FontId        mLastFailedFontId;
  Text::GlyphIndex    mLastFailedGlyphId;
  float               mLastFailedGlyphAdvance;
  float               mLastFailedGlyphWidth;
  float               mLastFailedGlyphHeight;
  float               mLastFailedGlyphXBearing;
  float               mLastFailedGlyphYBearing;

  Shader mShaderL8;
  Shader mShaderRgba;
};

} // namespace Dali::Ui::Internal::TextVisualizer

#endif // DALI_UI_TEXT_VISUALIZER_GLYPH_RENDERER_H
