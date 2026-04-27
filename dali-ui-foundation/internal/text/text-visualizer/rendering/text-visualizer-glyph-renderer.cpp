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

// CLASS HEADER
#include <dali-ui-foundation/internal/text/text-visualizer/rendering/text-visualizer-glyph-renderer.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/rendering/texture-set.h>

#include <algorithm>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/text/rendering/atlas/atlas-glyph-manager.h>
#include <dali-ui-foundation/internal/text/rendering/atlas/atlas-manager.h>
#include <dali-ui-foundation/internal/text/rendering/atlas/atlas-mesh-factory.h>
#include <dali-ui-foundation/internal/text/text-visualizer/atlas-view-adapter.h>
#include <dali-ui-foundation/internal/text/text-visualizer/layout-types.h>
#include <dali-ui-foundation/internal/text/text-visualizer/prepared-text.h>

namespace Dali::Ui::Internal::TextVisualizer
{
namespace
{
struct PendingMesh
{
  uint32_t                       atlasId{0u};
  Dali::Ui::AtlasManager::Mesh2D mesh;
};

Dali::Ui::AtlasGlyphManager::GlyphStyle CreateGlyphStyle(const Text::GlyphInfo& glyphInfo)
{
  Dali::Ui::AtlasGlyphManager::GlyphStyle style;
  style.outline  = 0u;
  style.isItalic = glyphInfo.isItalicRequired;
  style.isBold   = glyphInfo.isBoldRequired;
  return style;
}

bool IsRenderableGlyph(const Text::GlyphInfo& glyphInfo)
{
  return (glyphInfo.width > 0.0f) || (glyphInfo.height > 0.0f);
}

void HashGlyphCacheValue(uint64_t& hash, uint64_t value)
{
  hash ^= value + 0x9e3779b97f4a7c15ULL + (hash << 6u) + (hash >> 2u);
}

void HashMeshTopologyValue(uint64_t& hash, uint64_t value)
{
  HashGlyphCacheValue(hash, value);
}

Property::Map CreateQuadVertexFormat()
{
  Property::Map format;
  format["aPosition"] = Property::VECTOR2;
  format["aTexCoord"] = Property::VECTOR2;
  format["aColor"]    = Property::VECTOR4;
  return format;
}

Vector2 GetMeshActorSize(const AtlasViewAdapter& adapter)
{
  const Vector2 controlSize = adapter.GetControlSize();
  const Vector2 layoutSize  = adapter.GetLayoutSize();

  Vector2 actorSize = layoutSize;
  if(actorSize.x <= 0.0f)
  {
    actorSize.x = controlSize.x;
  }

  if(actorSize.y <= 0.0f)
  {
    actorSize.y = controlSize.y;
  }

  actorSize.x = std::max(actorSize.x, controlSize.x);
  actorSize.y = std::max(actorSize.y, controlSize.y);

  return actorSize;
}

PendingMesh* FindPendingMesh(std::vector<PendingMesh>& pendingMeshes, uint32_t atlasId)
{
  for(PendingMesh& pendingMesh : pendingMeshes)
  {
    if(pendingMesh.atlasId == atlasId)
    {
      return &pendingMesh;
    }
  }

  return nullptr;
}

uint64_t CalculateMeshTopologySignature(const std::vector<PendingMesh>& pendingMeshes, Dali::Ui::AtlasGlyphManager glyphManager)
{
  uint64_t hash = 1469598103934665603ULL;
  HashMeshTopologyValue(hash, pendingMeshes.size());

  for(const PendingMesh& pendingMesh : pendingMeshes)
  {
    const bool isColorShader = Pixel::BGRA8888 == glyphManager.GetPixelFormat(pendingMesh.atlasId);
    HashMeshTopologyValue(hash, pendingMesh.atlasId);
    HashMeshTopologyValue(hash, pendingMesh.mesh.mVertices.Size());
    HashMeshTopologyValue(hash, pendingMesh.mesh.mIndices.Size());
    HashMeshTopologyValue(hash, isColorShader ? 1u : 0u);
  }

  return hash;
}
} // unnamed namespace

TextVisualizerGlyphRenderer::TextVisualizerGlyphRenderer()
: mRenderHost(),
  mOutputActor(),
  mAttached(false),
  mGlyphCacheEntries(),
  mGlyphCacheSignature(0u),
  mHasGlyphCacheSignature(false),
  mMeshRecords(),
  mMeshTopologySignature(0u),
  mHasMeshTopologySignature(false),
  mGeometryOnlyUpdateCount(0u),
  mFullMeshRebuildCount(0u),
  mLastFailureReason(RenderFailureReason::NONE),
  mFailureEmptyInputCount(0u),
  mFailureNoRenderableGlyphsCount(0u),
  mFailureNoPositionCacheCount(0u),
  mFailureSignatureCount(0u),
  mFailureNoGlyphManagerCount(0u),
  mFailureGlyphPlacementCount(0u),
  mFailureGlyphInfoCount(0u),
  mFailureGlyphPositionCount(0u),
  mFailureCacheMissCount(0u),
  mFailureEmptyMeshCount(0u),
  mFailureNoOutputActorCount(0u),
  mFailureNoTextureSetCount(0u),
  mLastFailedPlacementIndex(std::numeric_limits<uint32_t>::max()),
  mLastFailedGlyphIndex(std::numeric_limits<uint32_t>::max()),
  mLastFailedFontId(0u),
  mLastFailedGlyphId(0u),
  mLastFailedGlyphAdvance(0.0f),
  mLastFailedGlyphWidth(0.0f),
  mLastFailedGlyphHeight(0.0f),
  mLastFailedGlyphXBearing(0.0f),
  mLastFailedGlyphYBearing(0.0f),
  mShaderL8(),
  mShaderRgba()
{
}

TextVisualizerGlyphRenderer::~TextVisualizerGlyphRenderer()
{
  Clear();
}

void TextVisualizerGlyphRenderer::Clear()
{
  ClearMeshes();
  ClearGlyphCache();
  DetachOutputFromHost();
  mOutputActor.Reset();
  mRenderHost.Reset();
  mAttached                 = false;
  mMeshTopologySignature    = 0u;
  mHasMeshTopologySignature = false;
  mGeometryOnlyUpdateCount  = 0u;
  mFullMeshRebuildCount     = 0u;
  ResetFailureDiagnostics();
  mShaderL8.Reset();
  mShaderRgba.Reset();
}

bool TextVisualizerGlyphRenderer::HasOutputActor() const
{
  return static_cast<bool>(mOutputActor);
}

Actor TextVisualizerGlyphRenderer::GetOutputActor() const
{
  return mOutputActor;
}

bool TextVisualizerGlyphRenderer::IsAttached() const
{
  return mAttached;
}

void TextVisualizerGlyphRenderer::SetRenderHost(Actor renderHost)
{
  if(!renderHost)
  {
    DetachOutputFromHost();
  }

  mRenderHost = renderHost;
}

Actor TextVisualizerGlyphRenderer::GetRenderHost() const
{
  return mRenderHost;
}

bool TextVisualizerGlyphRenderer::HasRenderHost() const
{
  return static_cast<bool>(mRenderHost);
}

void TextVisualizerGlyphRenderer::EnsureOutputActor()
{
  if(mOutputActor)
  {
    return;
  }

  mOutputActor = Actor::New();
  mOutputActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  mOutputActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  mOutputActor.SetProperty(Actor::Property::POSITION, Vector3::ZERO);
  mOutputActor.SetProperty(Actor::Property::VISIBLE, true);
}

bool TextVisualizerGlyphRenderer::AttachOutputToHost()
{
  if(!mRenderHost)
  {
    return false;
  }

  EnsureOutputActor();
  if(!mOutputActor)
  {
    return false;
  }

  Actor outputParent = mOutputActor.GetParent();
  if(outputParent && outputParent != mRenderHost)
  {
    return false;
  }

  if(!outputParent)
  {
    mRenderHost.Add(mOutputActor);
  }

  mAttached = true;
  return true;
}

void TextVisualizerGlyphRenderer::DetachOutputFromHost()
{
  if(mOutputActor)
  {
    Actor parent = mOutputActor.GetParent();
    if(parent)
    {
      mOutputActor.Unparent();
    }
  }

  mAttached = false;
}

void TextVisualizerGlyphRenderer::ClearMeshes()
{
  for(MeshRecord& record : mMeshRecords)
  {
    if(record.actor)
    {
      Actor parent = record.actor.GetParent();
      if(parent)
      {
        record.actor.Unparent();
      }
    }

    record.actor.Reset();
    record.renderer.Reset();
    record.geometry.Reset();
    record.vertexBuffer.Reset();
    record.vertexCount = 0u;
    record.indexCount  = 0u;
    record.atlasId     = 0u;
  }

  mMeshRecords.clear();
  mMeshTopologySignature    = 0u;
  mHasMeshTopologySignature = false;
}

bool TextVisualizerGlyphRenderer::HasMeshRecords() const
{
  return !mMeshRecords.empty();
}

uint32_t TextVisualizerGlyphRenderer::GetMeshRecordCount() const
{
  return static_cast<uint32_t>(mMeshRecords.size());
}

bool TextVisualizerGlyphRenderer::HasMeshTopologySignature() const
{
  return mHasMeshTopologySignature;
}

uint64_t TextVisualizerGlyphRenderer::GetMeshTopologySignature() const
{
  return mMeshTopologySignature;
}

uint32_t TextVisualizerGlyphRenderer::GetGeometryOnlyUpdateCount() const
{
  return mGeometryOnlyUpdateCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFullMeshRebuildCount() const
{
  return mFullMeshRebuildCount;
}

void TextVisualizerGlyphRenderer::ClearGlyphCache()
{
  ReleaseGlyphReferences(mGlyphCacheEntries);
  mGlyphCacheSignature    = 0u;
  mHasGlyphCacheSignature = false;
}

bool TextVisualizerGlyphRenderer::HasGlyphCacheEntries() const
{
  return !mGlyphCacheEntries.empty();
}

uint32_t TextVisualizerGlyphRenderer::GetGlyphCacheEntryCount() const
{
  return static_cast<uint32_t>(mGlyphCacheEntries.size());
}

bool TextVisualizerGlyphRenderer::HasGlyphCacheSignature() const
{
  return mHasGlyphCacheSignature;
}

uint64_t TextVisualizerGlyphRenderer::GetGlyphCacheSignature() const
{
  return mGlyphCacheSignature;
}

TextVisualizerGlyphRenderer::RenderFailureReason TextVisualizerGlyphRenderer::GetLastFailureReason() const
{
  return mLastFailureReason;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureEmptyInputCount() const
{
  return mFailureEmptyInputCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureNoRenderableGlyphsCount() const
{
  return mFailureNoRenderableGlyphsCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureNoPositionCacheCount() const
{
  return mFailureNoPositionCacheCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureSignatureCount() const
{
  return mFailureSignatureCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureNoGlyphManagerCount() const
{
  return mFailureNoGlyphManagerCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureGlyphPlacementCount() const
{
  return mFailureGlyphPlacementCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureGlyphInfoCount() const
{
  return mFailureGlyphInfoCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureGlyphPositionCount() const
{
  return mFailureGlyphPositionCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureCacheMissCount() const
{
  return mFailureCacheMissCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureEmptyMeshCount() const
{
  return mFailureEmptyMeshCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureNoOutputActorCount() const
{
  return mFailureNoOutputActorCount;
}

uint32_t TextVisualizerGlyphRenderer::GetFailureNoTextureSetCount() const
{
  return mFailureNoTextureSetCount;
}

uint32_t TextVisualizerGlyphRenderer::GetLastFailedPlacementIndex() const
{
  return mLastFailedPlacementIndex;
}

uint32_t TextVisualizerGlyphRenderer::GetLastFailedGlyphIndex() const
{
  return mLastFailedGlyphIndex;
}

Text::FontId TextVisualizerGlyphRenderer::GetLastFailedFontId() const
{
  return mLastFailedFontId;
}

Text::GlyphIndex TextVisualizerGlyphRenderer::GetLastFailedGlyphId() const
{
  return mLastFailedGlyphId;
}

float TextVisualizerGlyphRenderer::GetLastFailedGlyphAdvance() const
{
  return mLastFailedGlyphAdvance;
}

float TextVisualizerGlyphRenderer::GetLastFailedGlyphWidth() const
{
  return mLastFailedGlyphWidth;
}

float TextVisualizerGlyphRenderer::GetLastFailedGlyphHeight() const
{
  return mLastFailedGlyphHeight;
}

float TextVisualizerGlyphRenderer::GetLastFailedGlyphXBearing() const
{
  return mLastFailedGlyphXBearing;
}

float TextVisualizerGlyphRenderer::GetLastFailedGlyphYBearing() const
{
  return mLastFailedGlyphYBearing;
}

void TextVisualizerGlyphRenderer::ResetFailureDiagnostics()
{
  mLastFailureReason              = RenderFailureReason::NONE;
  mFailureEmptyInputCount         = 0u;
  mFailureNoRenderableGlyphsCount = 0u;
  mFailureNoPositionCacheCount    = 0u;
  mFailureSignatureCount          = 0u;
  mFailureNoGlyphManagerCount     = 0u;
  mFailureGlyphPlacementCount     = 0u;
  mFailureGlyphInfoCount          = 0u;
  mFailureGlyphPositionCount      = 0u;
  mFailureCacheMissCount          = 0u;
  mFailureEmptyMeshCount          = 0u;
  mFailureNoOutputActorCount      = 0u;
  mFailureNoTextureSetCount       = 0u;
  mLastFailedPlacementIndex       = std::numeric_limits<uint32_t>::max();
  mLastFailedGlyphIndex           = std::numeric_limits<uint32_t>::max();
  mLastFailedFontId               = 0u;
  mLastFailedGlyphId              = 0u;
  mLastFailedGlyphAdvance         = 0.0f;
  mLastFailedGlyphWidth           = 0.0f;
  mLastFailedGlyphHeight          = 0.0f;
  mLastFailedGlyphXBearing        = 0.0f;
  mLastFailedGlyphYBearing        = 0.0f;
}

void TextVisualizerGlyphRenderer::RecordFailure(RenderFailureReason reason, uint32_t placementIndex, uint32_t glyphIndex, const Text::GlyphInfo* glyphInfo)
{
  mLastFailureReason        = reason;
  mLastFailedPlacementIndex = placementIndex;
  mLastFailedGlyphIndex     = glyphIndex;
  mLastFailedFontId         = 0u;
  mLastFailedGlyphId        = 0u;
  mLastFailedGlyphAdvance   = 0.0f;
  mLastFailedGlyphWidth     = 0.0f;
  mLastFailedGlyphHeight    = 0.0f;
  mLastFailedGlyphXBearing  = 0.0f;
  mLastFailedGlyphYBearing  = 0.0f;

  if(nullptr != glyphInfo)
  {
    mLastFailedFontId        = glyphInfo->fontId;
    mLastFailedGlyphId       = glyphInfo->index;
    mLastFailedGlyphAdvance  = glyphInfo->advance;
    mLastFailedGlyphWidth    = glyphInfo->width;
    mLastFailedGlyphHeight   = glyphInfo->height;
    mLastFailedGlyphXBearing = glyphInfo->xBearing;
    mLastFailedGlyphYBearing = glyphInfo->yBearing;
  }

  switch(reason)
  {
    case RenderFailureReason::EMPTY_INPUT:
    {
      ++mFailureEmptyInputCount;
      break;
    }
    case RenderFailureReason::NO_RENDERABLE_GLYPHS:
    {
      ++mFailureNoRenderableGlyphsCount;
      break;
    }
    case RenderFailureReason::NO_POSITION_CACHE:
    {
      ++mFailureNoPositionCacheCount;
      break;
    }
    case RenderFailureReason::SIGNATURE_FAILED:
    {
      ++mFailureSignatureCount;
      break;
    }
    case RenderFailureReason::NO_GLYPH_MANAGER:
    {
      ++mFailureNoGlyphManagerCount;
      break;
    }
    case RenderFailureReason::GLYPH_PLACEMENT_FAILED:
    {
      ++mFailureGlyphPlacementCount;
      break;
    }
    case RenderFailureReason::GLYPH_INFO_FAILED:
    {
      ++mFailureGlyphInfoCount;
      break;
    }
    case RenderFailureReason::GLYPH_POSITION_FAILED:
    {
      ++mFailureGlyphPositionCount;
      break;
    }
    case RenderFailureReason::GLYPH_CACHE_MISS:
    {
      ++mFailureCacheMissCount;
      break;
    }
    case RenderFailureReason::EMPTY_MESH:
    {
      ++mFailureEmptyMeshCount;
      break;
    }
    case RenderFailureReason::NO_OUTPUT_ACTOR:
    {
      ++mFailureNoOutputActorCount;
      break;
    }
    case RenderFailureReason::NO_TEXTURE_SET:
    {
      ++mFailureNoTextureSetCount;
      break;
    }
    case RenderFailureReason::NONE:
    {
      break;
    }
  }
}

bool TextVisualizerGlyphRenderer::CalculateGlyphCacheSignature(const AtlasViewAdapter& adapter, uint64_t& signature)
{
  const uint32_t placementCount = adapter.GetGlyphPlacementCount();
  if(0u == placementCount)
  {
    RecordFailure(RenderFailureReason::SIGNATURE_FAILED);
    return false;
  }

  uint64_t hash = 1469598103934665603ULL;
  HashGlyphCacheValue(hash, placementCount);

  for(uint32_t placementIndex = 0u; placementIndex < placementCount; ++placementIndex)
  {
    GlyphPlacement  placement;
    Text::GlyphInfo glyphInfo;

    if(!adapter.GetGlyphPlacement(placementIndex, placement))
    {
      RecordFailure(RenderFailureReason::GLYPH_PLACEMENT_FAILED, placementIndex);
      return false;
    }

    if(!adapter.GetGlyphInfo(placement.glyphIndex, glyphInfo))
    {
      RecordFailure(RenderFailureReason::GLYPH_INFO_FAILED, placementIndex, placement.glyphIndex);
      return false;
    }

    const Dali::Ui::AtlasGlyphManager::GlyphStyle style = CreateGlyphStyle(glyphInfo);
    HashGlyphCacheValue(hash, glyphInfo.fontId);
    HashGlyphCacheValue(hash, glyphInfo.index);
    HashGlyphCacheValue(hash, style.outline);
    HashGlyphCacheValue(hash, style.isItalic ? 1u : 0u);
    HashGlyphCacheValue(hash, style.isBold ? 1u : 0u);
  }

  signature = hash;
  return true;
}

bool TextVisualizerGlyphRenderer::CanReuseGlyphCache(uint64_t signature) const
{
  return mHasGlyphCacheSignature &&
         (mGlyphCacheSignature == signature) &&
         !mGlyphCacheEntries.empty();
}

bool TextVisualizerGlyphRenderer::AcquireGlyphReferences(const AtlasViewAdapter& adapter, std::vector<GlyphCacheEntry>& entries)
{
  entries.clear();

  Dali::Ui::AtlasGlyphManager glyphManager = Dali::Ui::AtlasGlyphManager::Get();
  if(!glyphManager)
  {
    RecordFailure(RenderFailureReason::NO_GLYPH_MANAGER);
    return false;
  }

  const uint32_t placementCount = adapter.GetGlyphPlacementCount();
  entries.reserve(placementCount);

  for(uint32_t placementIndex = 0u; placementIndex < placementCount; ++placementIndex)
  {
    GlyphPlacement  placement;
    Text::GlyphInfo glyphInfo;

    if(!adapter.GetGlyphPlacement(placementIndex, placement))
    {
      RecordFailure(RenderFailureReason::GLYPH_PLACEMENT_FAILED, placementIndex);
      ReleaseGlyphReferences(entries);
      return false;
    }

    if(!adapter.GetGlyphInfo(placement.glyphIndex, glyphInfo))
    {
      RecordFailure(RenderFailureReason::GLYPH_INFO_FAILED, placementIndex, placement.glyphIndex);
      ReleaseGlyphReferences(entries);
      return false;
    }

    if(!IsRenderableGlyph(glyphInfo))
    {
      continue;
    }

    const Dali::Ui::AtlasGlyphManager::GlyphStyle style = CreateGlyphStyle(glyphInfo);

    Dali::Ui::AtlasManager::AtlasSlot slot;
    if(!glyphManager.IsCached(glyphInfo.fontId, glyphInfo.index, style, slot) ||
       (0u == slot.mImageId) ||
       (0u == slot.mAtlasId))
    {
      RecordFailure(RenderFailureReason::GLYPH_CACHE_MISS, placementIndex, placement.glyphIndex, &glyphInfo);
      ReleaseGlyphReferences(entries);
      return false;
    }

    glyphManager.AdjustReferenceCount(glyphInfo.fontId, glyphInfo.index, style, 1);

    GlyphCacheEntry entry;
    entry.fontId     = glyphInfo.fontId;
    entry.glyphIndex = glyphInfo.index;
    entry.style      = style;
    entry.atlasId    = slot.mAtlasId;
    entry.imageId    = slot.mImageId;
    entries.push_back(entry);
  }

  if(entries.empty())
  {
    RecordFailure(RenderFailureReason::NO_RENDERABLE_GLYPHS);
    return false;
  }

  return true;
}

void TextVisualizerGlyphRenderer::ReleaseGlyphReferences(std::vector<GlyphCacheEntry>& entries) const
{
  if(entries.empty())
  {
    return;
  }

  Dali::Ui::AtlasGlyphManager glyphManager = Dali::Ui::AtlasGlyphManager::Get();
  if(glyphManager)
  {
    for(const GlyphCacheEntry& entry : entries)
    {
      if(entry.fontId != 0u)
      {
        glyphManager.AdjustReferenceCount(entry.fontId, entry.glyphIndex, entry.style, -1);
      }
    }
  }

  entries.clear();
}

bool TextVisualizerGlyphRenderer::Render(const PreparedText& preparedText, const LayoutResult& layoutResult, const AtlasViewAdapter& adapter, const Vector4& textColor)
{
  if(preparedText.Empty() ||
     !preparedText.HasGlyphData() ||
     !preparedText.HasGlyphMetrics() ||
     layoutResult.glyphPlacements.Empty())
  {
    RecordFailure(RenderFailureReason::EMPTY_INPUT);
    ClearMeshes();
    return false;
  }

  return RenderAdapter(adapter, textColor);
}

bool TextVisualizerGlyphRenderer::Render(const AtlasViewAdapter& adapter)
{
  return RenderAdapter(adapter, adapter.GetTextColor());
}

bool TextVisualizerGlyphRenderer::RenderAdapter(const AtlasViewAdapter& adapter, const Vector4& textColor)
{
  if(!adapter.HasRenderableGlyphs())
  {
    RecordFailure(RenderFailureReason::NO_RENDERABLE_GLYPHS);
    ClearMeshes();
    return false;
  }

  if(0u == adapter.GetRendererGlyphPositionCacheCount())
  {
    RecordFailure(RenderFailureReason::NO_POSITION_CACHE);
    return false;
  }

  uint64_t newGlyphCacheSignature = 0u;
  if(!CalculateGlyphCacheSignature(adapter, newGlyphCacheSignature))
  {
    if(RenderFailureReason::SIGNATURE_FAILED != mLastFailureReason)
    {
      ++mFailureSignatureCount;
    }
    return false;
  }

  Dali::Ui::AtlasGlyphManager glyphManager = Dali::Ui::AtlasGlyphManager::Get();
  if(!glyphManager)
  {
    RecordFailure(RenderFailureReason::NO_GLYPH_MANAGER);
    return false;
  }

  const bool                   reuseGlyphCache = CanReuseGlyphCache(newGlyphCacheSignature);
  std::vector<GlyphCacheEntry> newGlyphCacheEntries;

  if(!reuseGlyphCache &&
     !AcquireGlyphReferences(adapter, newGlyphCacheEntries))
  {
    return false;
  }

  auto failRender = [&]() -> bool
  {
    if(!reuseGlyphCache)
    {
      ReleaseGlyphReferences(newGlyphCacheEntries);
    }
    return false;
  };

  const Vector2 actorSize     = GetMeshActorSize(adapter);
  const Vector2 halfActorSize = actorSize * 0.5f;

  std::vector<PendingMesh> pendingMeshes;
  pendingMeshes.reserve(adapter.GetRenderableGlyphCount());

  for(uint32_t placementIndex = 0u, placementCount = adapter.GetGlyphPlacementCount(); placementIndex < placementCount; ++placementIndex)
  {
    GlyphPlacement  placement;
    Text::GlyphInfo glyphInfo;
    Vector2         position;

    if(!adapter.GetGlyphPlacement(placementIndex, placement))
    {
      RecordFailure(RenderFailureReason::GLYPH_PLACEMENT_FAILED, placementIndex);
      return failRender();
    }

    if(!adapter.GetGlyphInfo(placement.glyphIndex, glyphInfo))
    {
      RecordFailure(RenderFailureReason::GLYPH_INFO_FAILED, placementIndex, placement.glyphIndex);
      return failRender();
    }

    if(!IsRenderableGlyph(glyphInfo))
    {
      continue;
    }

    if(!adapter.GetRendererGlyphPosition(placementIndex, position))
    {
      RecordFailure(RenderFailureReason::GLYPH_POSITION_FAILED, placementIndex, placement.glyphIndex, &glyphInfo);
      return failRender();
    }

    // Atlas glyph mesh vertices use actor-center local coordinates. Match Text::AtlasRenderer by converting from
    // layout top-left coordinates before generating the glyph quad.
    position = Vector2(std::round(position.x), position.y) - halfActorSize;

    const Dali::Ui::AtlasGlyphManager::GlyphStyle style = CreateGlyphStyle(glyphInfo);

    Dali::Ui::AtlasManager::AtlasSlot slot;
    if(!glyphManager.IsCached(glyphInfo.fontId, glyphInfo.index, style, slot) ||
       (0u == slot.mImageId) ||
       (0u == slot.mAtlasId))
    {
      RecordFailure(RenderFailureReason::GLYPH_CACHE_MISS, placementIndex, placement.glyphIndex, &glyphInfo);
      return failRender();
    }

    Dali::Ui::AtlasManager::Mesh2D glyphMesh;
    glyphManager.GenerateMeshData(slot.mImageId, position, glyphMesh);
    if(glyphMesh.mVertices.Empty() || glyphMesh.mIndices.Empty())
    {
      RecordFailure(RenderFailureReason::EMPTY_MESH, placementIndex, placement.glyphIndex, &glyphInfo);
      return failRender();
    }

    for(Vector<Dali::Ui::AtlasManager::Vertex2D>::Iterator it = glyphMesh.mVertices.Begin(), endIt = glyphMesh.mVertices.End(); it != endIt; ++it)
    {
      it->mColor = textColor;
    }

    PendingMesh* pendingMesh = FindPendingMesh(pendingMeshes, slot.mAtlasId);
    if(nullptr != pendingMesh)
    {
      Internal::AtlasMeshFactory::AppendMesh(pendingMesh->mesh, glyphMesh);
    }
    else
    {
      PendingMesh newPendingMesh;
      newPendingMesh.atlasId = slot.mAtlasId;
      newPendingMesh.mesh    = glyphMesh;
      pendingMeshes.push_back(newPendingMesh);
    }
  }

  if(pendingMeshes.empty())
  {
    RecordFailure(RenderFailureReason::NO_RENDERABLE_GLYPHS);
    return failRender();
  }

  EnsureOutputActor();
  if(!mOutputActor)
  {
    RecordFailure(RenderFailureReason::NO_OUTPUT_ACTOR);
    return failRender();
  }

  const uint64_t newMeshTopologySignature = CalculateMeshTopologySignature(pendingMeshes, glyphManager);
  mOutputActor.SetProperty(Actor::Property::SIZE, Vector3(actorSize.x, actorSize.y, 0.0f));

  auto canUpdateGeometryOnly = [&]() -> bool
  {
    if(!CanReuseGlyphCache(newGlyphCacheSignature) ||
       !mHasMeshTopologySignature ||
       (mMeshTopologySignature != newMeshTopologySignature) ||
       (mMeshRecords.size() != pendingMeshes.size()))
    {
      return false;
    }

    for(size_t index = 0u; index < pendingMeshes.size(); ++index)
    {
      const MeshRecord&  record      = mMeshRecords[index];
      const PendingMesh& pendingMesh = pendingMeshes[index];

      if((record.atlasId != pendingMesh.atlasId) ||
         !record.actor ||
         !record.renderer ||
         !record.geometry ||
         !record.vertexBuffer ||
         (record.vertexCount != pendingMesh.mesh.mVertices.Size()) ||
         (record.indexCount != pendingMesh.mesh.mIndices.Size()))
      {
        return false;
      }

      Actor parent = record.actor.GetParent();
      if(parent != mOutputActor)
      {
        return false;
      }
    }

    return true;
  };

  auto updateGeometryOnly = [&]() -> bool
  {
    if(!canUpdateGeometryOnly())
    {
      return false;
    }

    for(size_t index = 0u; index < pendingMeshes.size(); ++index)
    {
      MeshRecord&        record      = mMeshRecords[index];
      const PendingMesh& pendingMesh = pendingMeshes[index];

      record.vertexBuffer.SetData(const_cast<Dali::Ui::AtlasManager::Vertex2D*>(pendingMesh.mesh.mVertices.Begin()), pendingMesh.mesh.mVertices.Size());
      TextureSet textureSet = glyphManager.GetTextures(record.atlasId);
      if(!textureSet)
      {
        RecordFailure(RenderFailureReason::NO_TEXTURE_SET);
        return false;
      }
      record.renderer.SetTextures(textureSet);
      record.actor.SetProperty(Actor::Property::SIZE, Vector3(actorSize.x, actorSize.y, 0.0f));
      record.vertexCount = pendingMesh.mesh.mVertices.Size();
      record.indexCount  = pendingMesh.mesh.mIndices.Size();
    }

    mMeshTopologySignature    = newMeshTopologySignature;
    mHasMeshTopologySignature = true;
    ++mGeometryOnlyUpdateCount;
    return true;
  };

  if(updateGeometryOnly())
  {
    return true;
  }

  for(const PendingMesh& pendingMesh : pendingMeshes)
  {
    if(pendingMesh.mesh.mVertices.Empty() || pendingMesh.mesh.mIndices.Empty())
    {
      RecordFailure(RenderFailureReason::EMPTY_MESH);
      return failRender();
    }

    TextureSet textureSet = glyphManager.GetTextures(pendingMesh.atlasId);
    if(!textureSet)
    {
      RecordFailure(RenderFailureReason::NO_TEXTURE_SET);
      return failRender();
    }
  }

  ClearMeshes();

  Property::Map vertexFormat = CreateQuadVertexFormat();

  for(const PendingMesh& pendingMesh : pendingMeshes)
  {
    VertexBuffer vertexBuffer = VertexBuffer::New(vertexFormat);
    vertexBuffer.SetData(const_cast<Dali::Ui::AtlasManager::Vertex2D*>(pendingMesh.mesh.mVertices.Begin()), pendingMesh.mesh.mVertices.Size());

    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer(vertexBuffer);
    geometry.SetIndexBuffer(pendingMesh.mesh.mIndices.Begin(), pendingMesh.mesh.mIndices.Size());

    TextureSet textureSet = glyphManager.GetTextures(pendingMesh.atlasId);

    const bool isColorShader = Pixel::BGRA8888 == glyphManager.GetPixelFormat(pendingMesh.atlasId);
    Shader     shader;
    if(isColorShader)
    {
      if(!mShaderRgba)
      {
        mShaderRgba = Shader::New(Integration::ToDaliStringView(SHADER_TEXT_ATLAS_SHADER_VERT),
                                  Integration::ToDaliStringView(SHADER_TEXT_ATLAS_RGBA_SHADER_FRAG),
                                  static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL),
                                  "TEXT_VISUALIZER_GLYPH_RENDERER_RGBA");
      }
      shader = mShaderRgba;
    }
    else
    {
      if(!mShaderL8)
      {
        mShaderL8 = Shader::New(Integration::ToDaliStringView(SHADER_TEXT_ATLAS_SHADER_VERT),
                                Integration::ToDaliStringView(SHADER_TEXT_ATLAS_L8_SHADER_FRAG),
                                static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL),
                                "TEXT_VISUALIZER_GLYPH_RENDERER_L8");
      }
      shader = mShaderL8;
    }

    shader.RegisterProperty("textColorAnimatable", Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    Renderer renderer = Renderer::New(geometry, shader);
    renderer.SetTextures(textureSet);
    renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
    renderer.SetProperty(Renderer::Property::DEPTH_INDEX, DepthIndex::CONTENT);

    Actor meshActor = Actor::New();
    meshActor.AddRenderer(renderer);
    meshActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    meshActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    meshActor.SetProperty(Actor::Property::SIZE, Vector3(actorSize.x, actorSize.y, 0.0f));
    meshActor.RegisterProperty("uOffset", Vector2::ZERO);
    meshActor.SetProperty(Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_COLOR);

    mOutputActor.Add(meshActor);

    MeshRecord meshRecord;
    meshRecord.atlasId      = pendingMesh.atlasId;
    meshRecord.actor        = meshActor;
    meshRecord.renderer     = renderer;
    meshRecord.geometry     = geometry;
    meshRecord.vertexBuffer = vertexBuffer;
    meshRecord.vertexCount  = pendingMesh.mesh.mVertices.Size();
    meshRecord.indexCount   = pendingMesh.mesh.mIndices.Size();
    mMeshRecords.push_back(meshRecord);
  }

  mMeshTopologySignature    = newMeshTopologySignature;
  mHasMeshTopologySignature = true;
  ++mFullMeshRebuildCount;

  if(!reuseGlyphCache)
  {
    ReleaseGlyphReferences(mGlyphCacheEntries);
    mGlyphCacheEntries.swap(newGlyphCacheEntries);
    mGlyphCacheSignature    = newGlyphCacheSignature;
    mHasGlyphCacheSignature = true;
  }

  return HasMeshRecords();
}

} // namespace Dali::Ui::Internal::TextVisualizer
