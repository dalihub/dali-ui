#ifndef DALI_UI_TEXT_VISUALIZER_ATLAS_RENDERER_BRIDGE_H
#define DALI_UI_TEXT_VISUALIZER_ATLAS_RENDERER_BRIDGE_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/rendering/text-renderer.h>
#include <dali-ui-foundation/internal/text/text-visualizer/rendering/text-visualizer-glyph-renderer.h>
#include <dali-ui-foundation/internal/text/text-visualizer/text-visualizer-view-interface.h>
#include <dali/public-api/actors/actor.h>

namespace Dali::Ui::Internal::TextVisualizer
{
class AtlasViewAdapter;

/**
 * @brief Bridge object that owns a renderer instance and references atlas-ready adapter data.
 *
 * This class intentionally stops short of scene attachment and geometry upload. It only tracks
 * whether AtlasRenderer creation is possible and keeps a non-owning adapter pointer for future
 * render integration.
 */
class AtlasRendererBridge
{
public:
  AtlasRendererBridge();
  ~AtlasRendererBridge();

  void SetAdapter(const AtlasViewAdapter* adapter);
  void Clear();

  bool     HasRenderableGlyphs() const;
  bool     IsRendererCreated() const;
  bool     HasRenderHost() const;
  bool     HasTextControlActor() const;
  bool     HasRendererOutput() const;
  bool     IsRendererAttached() const;
  uint32_t GetRendererOutputChildCount() const
  {
    return mRendererOutput ? mRendererOutput.GetChildCount() : 0u;
  }
  uint32_t GetRendererOutputRendererCount() const
  {
    return mRendererOutput ? mRendererOutput.GetRendererCount() : 0u;
  }
  Vector3 GetRendererOutputSize() const
  {
    return mRendererOutput ? mRendererOutput.GetProperty<Vector3>(Actor::Property::SIZE) : Vector3::ZERO;
  }
  Vector3 GetRenderHostSize() const
  {
    return mRenderHost ? mRenderHost.GetProperty<Vector3>(Actor::Property::SIZE) : Vector3::ZERO;
  }
  bool IsRendererOutputVisible() const
  {
    return mRendererOutput && mRendererOutput.GetProperty<bool>(Actor::Property::VISIBLE);
  }
  bool IsRendererOutputParentedToHost() const
  {
    return mRendererOutput &&
           mRenderHost &&
           mRendererOutput.GetParent() &&
           (mRendererOutput.GetParent() == mRenderHost);
  }
  bool                                             IsRenderReady() const;
  bool                                             HasViewInterfaceAdapter() const;
  Actor                                            GetRendererOutput() const;
  uint32_t                                         GetRendererOutputDescendantCount() const;
  uint32_t                                         GetRendererOutputTotalRendererCount() const;
  bool                                             HasRendererOutputRenderableDescendant() const;
  Vector3                                          GetFirstRendererOutputChildSize() const;
  bool                                             IsFirstRendererOutputChildVisible() const;
  uint32_t                                         GetViewInterfaceGetGlyphsCallCount() const;
  uint32_t                                         GetLastRequestedGlyphCount() const;
  uint32_t                                         GetLastReturnedGlyphCount() const;
  uint32_t                                         GetLastGlyphStartIndex() const;
  uint32_t                                         GetRenderCallCount() const;
  uint32_t                                         GetAttachCallCount() const;
  uint32_t                                         GetValidatedGlyphCount() const;
  bool                                             HasValidatedRenderData() const;
  uint32_t                                         GetLightweightRenderAttemptCount() const;
  uint32_t                                         GetLightweightRenderSuccessCount() const;
  uint32_t                                         GetLightweightRenderFallbackCount() const;
  uint32_t                                         GetLightweightGeometryOnlyUpdateCount() const;
  uint32_t                                         GetLightweightFullMeshRebuildCount() const;
  uint32_t                                         GetLightweightGlyphCacheEntryCount() const;
  uint32_t                                         GetLightweightMeshRecordCount() const;
  uint64_t                                         GetLightweightMeshTopologySignature() const;
  bool                                             HasLightweightMeshTopologySignature() const;
  TextVisualizerGlyphRenderer::RenderFailureReason GetLightweightLastFailureReason() const;
  uint32_t                                         GetLightweightFailureEmptyInputCount() const;
  uint32_t                                         GetLightweightFailureNoRenderableGlyphsCount() const;
  uint32_t                                         GetLightweightFailureNoPositionCacheCount() const;
  uint32_t                                         GetLightweightFailureSignatureCount() const;
  uint32_t                                         GetLightweightFailureNoGlyphManagerCount() const;
  uint32_t                                         GetLightweightFailureGlyphPlacementCount() const;
  uint32_t                                         GetLightweightFailureGlyphInfoCount() const;
  uint32_t                                         GetLightweightFailureGlyphPositionCount() const;
  uint32_t                                         GetLightweightFailureCacheMissCount() const;
  uint32_t                                         GetLightweightFailureEmptyMeshCount() const;
  uint32_t                                         GetLightweightFailureNoOutputActorCount() const;
  uint32_t                                         GetLightweightFailureNoTextureSetCount() const;
  uint32_t                                         GetLightweightLastFailedPlacementIndex() const;
  uint32_t                                         GetLightweightLastFailedGlyphIndex() const;
  Text::FontId                                     GetLightweightLastFailedFontId() const;
  Text::GlyphIndex                                 GetLightweightLastFailedGlyphId() const;
  float                                            GetLightweightLastFailedGlyphAdvance() const;
  float                                            GetLightweightLastFailedGlyphWidth() const;
  float                                            GetLightweightLastFailedGlyphHeight() const;

  void  EnsureRenderer();
  void  ResetRenderer();
  bool  UpdateRenderData();
  void  SetLightweightRendererAllowed(bool allowed);
  bool  IsLightweightRendererAllowed() const;
  void  SetRenderHost(Actor renderHost);
  Actor GetRenderHost() const;
  void  SetTextControlActor(Actor textControlActor);
  Actor GetTextControlActor() const;
  bool  AttachRendererToHost();
  void  DetachRendererFromHost();

private:
  struct Impl;

  const AtlasViewAdapter*     mAdapter;
  Text::RendererPtr           mRenderer;
  Actor                       mRenderHost;
  Actor                       mTextControlActor;
  Actor                       mRendererOutput;
  bool                        mRendererAttached;
  uint32_t                    mRenderCallCount;
  uint32_t                    mAttachCallCount;
  uint32_t                    mLightweightRenderAttemptCount;
  uint32_t                    mLightweightRenderSuccessCount;
  uint32_t                    mLightweightRenderFallbackCount;
  bool                        mLightweightRendererAllowed;
  Property::Index             mAnimatablePropertyIndex;
  float                       mAlignmentOffset;
  int                         mDepth;
  TextVisualizerViewInterface mViewInterface;
  TextVisualizerGlyphRenderer mGlyphRenderer;
  Impl*                       mImpl;
};

} // namespace Dali::Ui::Internal::TextVisualizer

#endif // DALI_UI_TEXT_VISUALIZER_ATLAS_RENDERER_BRIDGE_H
