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
#include <dali-ui-foundation/internal/text/text-visualizer/atlas-renderer-bridge.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/rendering/atlas/text-atlas-renderer.h>
#include <dali-ui-foundation/internal/text/text-visualizer/atlas-view-adapter.h>

namespace Dali::Ui::Internal::TextVisualizer
{
namespace
{
constexpr bool ENABLE_TEXT_VISUALIZER_LIGHTWEIGHT_RENDERER = false;

uint32_t CountDescendants(const Actor& actor)
{
  if(!actor)
  {
    return 0u;
  }

  uint32_t count = actor.GetChildCount();
  for(uint32_t index = 0u; index < actor.GetChildCount(); ++index)
  {
    count += CountDescendants(actor.GetChildAt(index));
  }
  return count;
}

uint32_t CountRenderersRecursive(const Actor& actor)
{
  if(!actor)
  {
    return 0u;
  }

  uint32_t count = actor.GetRendererCount();
  for(uint32_t index = 0u; index < actor.GetChildCount(); ++index)
  {
    count += CountRenderersRecursive(actor.GetChildAt(index));
  }
  return count;
}
} // unnamed namespace

struct AtlasRendererBridge::Impl
{
  void Clear()
  {
    mValidatedGlyphCount = 0u;
    mHasValidatedGlyphs  = false;
  }

  uint32_t mValidatedGlyphCount{0u};
  bool     mHasValidatedGlyphs{false};
};

AtlasRendererBridge::AtlasRendererBridge()
: mAdapter(nullptr),
  mRenderer(),
  mRenderHost(),
  mTextControlActor(),
  mRendererOutput(),
  mRendererAttached(false),
  mRenderCallCount(0u),
  mAttachCallCount(0u),
  mLightweightRenderAttemptCount(0u),
  mLightweightRenderSuccessCount(0u),
  mLightweightRenderFallbackCount(0u),
  mLightweightRendererAllowed(false),
  mAnimatablePropertyIndex(Property::INVALID_INDEX),
  mAlignmentOffset(0.0f),
  mDepth(0),
  mViewInterface(),
  mGlyphRenderer(),
  mImpl(new Impl())
{
}

AtlasRendererBridge::~AtlasRendererBridge()
{
  delete mImpl;
}

void AtlasRendererBridge::SetAdapter(const AtlasViewAdapter* adapter)
{
  if(nullptr == adapter)
  {
    DetachRendererFromHost();
    mGlyphRenderer.Clear();
    mImpl->Clear();
    mAdapter = nullptr;
    mViewInterface.Clear();
    return;
  }

  mAdapter = adapter;
  mImpl->Clear();
  if(mRenderHost)
  {
    mGlyphRenderer.SetRenderHost(mRenderHost);
  }
  mViewInterface.SetAdapter(adapter);
}

void AtlasRendererBridge::Clear()
{
  DetachRendererFromHost();
  mAdapter = nullptr;
  mViewInterface.Clear();
  mImpl->Clear();
  mRenderHost.Reset();
  mTextControlActor.Reset();
  mRenderCallCount                = 0u;
  mAttachCallCount                = 0u;
  mLightweightRenderAttemptCount  = 0u;
  mLightweightRenderSuccessCount  = 0u;
  mLightweightRenderFallbackCount = 0u;
  mLightweightRendererAllowed     = false;
  mGlyphRenderer.Clear();
  ResetRenderer();
}

bool AtlasRendererBridge::HasRenderableGlyphs() const
{
  return (nullptr != mAdapter) && mAdapter->HasRenderableGlyphs();
}

bool AtlasRendererBridge::IsRendererCreated() const
{
  return static_cast<bool>(mRenderer);
}

bool AtlasRendererBridge::HasRenderHost() const
{
  return static_cast<bool>(mRenderHost);
}

bool AtlasRendererBridge::HasTextControlActor() const
{
  return static_cast<bool>(mTextControlActor);
}

bool AtlasRendererBridge::HasRendererOutput() const
{
  return static_cast<bool>(mRendererOutput);
}

bool AtlasRendererBridge::IsRendererAttached() const
{
  return mRendererAttached;
}

bool AtlasRendererBridge::IsRenderReady() const
{
  return IsRendererCreated() &&
         HasRenderHost() &&
         HasRendererOutput() &&
         IsRendererAttached() &&
         HasViewInterfaceAdapter() &&
         HasRenderableGlyphs();
}

bool AtlasRendererBridge::HasViewInterfaceAdapter() const
{
  return mViewInterface.HasAdapter();
}

Actor AtlasRendererBridge::GetRendererOutput() const
{
  return mRendererOutput;
}

uint32_t AtlasRendererBridge::GetRendererOutputDescendantCount() const
{
  return CountDescendants(mRendererOutput);
}

uint32_t AtlasRendererBridge::GetRendererOutputTotalRendererCount() const
{
  return CountRenderersRecursive(mRendererOutput);
}

bool AtlasRendererBridge::HasRendererOutputRenderableDescendant() const
{
  return GetRendererOutputTotalRendererCount() > 0u;
}

Vector3 AtlasRendererBridge::GetFirstRendererOutputChildSize() const
{
  if(mRendererOutput && mRendererOutput.GetChildCount() > 0u)
  {
    return mRendererOutput.GetChildAt(0u).GetProperty<Vector3>(Actor::Property::SIZE);
  }

  return Vector3::ZERO;
}

bool AtlasRendererBridge::IsFirstRendererOutputChildVisible() const
{
  return mRendererOutput &&
         (mRendererOutput.GetChildCount() > 0u) &&
         mRendererOutput.GetChildAt(0u).GetProperty<bool>(Actor::Property::VISIBLE);
}

uint32_t AtlasRendererBridge::GetViewInterfaceGetGlyphsCallCount() const
{
  return mViewInterface.GetDiagnostics().getGlyphsCallCount;
}

uint32_t AtlasRendererBridge::GetLastRequestedGlyphCount() const
{
  return mViewInterface.GetDiagnostics().lastRequestedGlyphCount;
}

uint32_t AtlasRendererBridge::GetLastReturnedGlyphCount() const
{
  return mViewInterface.GetDiagnostics().lastReturnedGlyphCount;
}

uint32_t AtlasRendererBridge::GetLastGlyphStartIndex() const
{
  return mViewInterface.GetDiagnostics().lastGlyphStartIndex;
}

uint32_t AtlasRendererBridge::GetRenderCallCount() const
{
  return mRenderCallCount;
}

uint32_t AtlasRendererBridge::GetAttachCallCount() const
{
  return mAttachCallCount;
}

uint32_t AtlasRendererBridge::GetValidatedGlyphCount() const
{
  return mImpl->mValidatedGlyphCount;
}

bool AtlasRendererBridge::HasValidatedRenderData() const
{
  return mImpl->mHasValidatedGlyphs;
}

uint32_t AtlasRendererBridge::GetLightweightRenderAttemptCount() const
{
  return mLightweightRenderAttemptCount;
}

uint32_t AtlasRendererBridge::GetLightweightRenderSuccessCount() const
{
  return mLightweightRenderSuccessCount;
}

uint32_t AtlasRendererBridge::GetLightweightRenderFallbackCount() const
{
  return mLightweightRenderFallbackCount;
}

uint32_t AtlasRendererBridge::GetLightweightGeometryOnlyUpdateCount() const
{
  return mGlyphRenderer.GetGeometryOnlyUpdateCount();
}

uint32_t AtlasRendererBridge::GetLightweightFullMeshRebuildCount() const
{
  return mGlyphRenderer.GetFullMeshRebuildCount();
}

uint32_t AtlasRendererBridge::GetLightweightGlyphCacheEntryCount() const
{
  return mGlyphRenderer.GetGlyphCacheEntryCount();
}

uint32_t AtlasRendererBridge::GetLightweightMeshRecordCount() const
{
  return mGlyphRenderer.GetMeshRecordCount();
}

uint64_t AtlasRendererBridge::GetLightweightMeshTopologySignature() const
{
  return mGlyphRenderer.GetMeshTopologySignature();
}

bool AtlasRendererBridge::HasLightweightMeshTopologySignature() const
{
  return mGlyphRenderer.HasMeshTopologySignature();
}

TextVisualizerGlyphRenderer::RenderFailureReason AtlasRendererBridge::GetLightweightLastFailureReason() const
{
  return mGlyphRenderer.GetLastFailureReason();
}

uint32_t AtlasRendererBridge::GetLightweightFailureEmptyInputCount() const
{
  return mGlyphRenderer.GetFailureEmptyInputCount();
}

uint32_t AtlasRendererBridge::GetLightweightFailureNoRenderableGlyphsCount() const
{
  return mGlyphRenderer.GetFailureNoRenderableGlyphsCount();
}

uint32_t AtlasRendererBridge::GetLightweightFailureNoPositionCacheCount() const
{
  return mGlyphRenderer.GetFailureNoPositionCacheCount();
}

uint32_t AtlasRendererBridge::GetLightweightFailureSignatureCount() const
{
  return mGlyphRenderer.GetFailureSignatureCount();
}

uint32_t AtlasRendererBridge::GetLightweightFailureNoGlyphManagerCount() const
{
  return mGlyphRenderer.GetFailureNoGlyphManagerCount();
}

uint32_t AtlasRendererBridge::GetLightweightFailureGlyphPlacementCount() const
{
  return mGlyphRenderer.GetFailureGlyphPlacementCount();
}

uint32_t AtlasRendererBridge::GetLightweightFailureGlyphInfoCount() const
{
  return mGlyphRenderer.GetFailureGlyphInfoCount();
}

uint32_t AtlasRendererBridge::GetLightweightFailureGlyphPositionCount() const
{
  return mGlyphRenderer.GetFailureGlyphPositionCount();
}

uint32_t AtlasRendererBridge::GetLightweightFailureCacheMissCount() const
{
  return mGlyphRenderer.GetFailureCacheMissCount();
}

uint32_t AtlasRendererBridge::GetLightweightFailureEmptyMeshCount() const
{
  return mGlyphRenderer.GetFailureEmptyMeshCount();
}

uint32_t AtlasRendererBridge::GetLightweightFailureNoOutputActorCount() const
{
  return mGlyphRenderer.GetFailureNoOutputActorCount();
}

uint32_t AtlasRendererBridge::GetLightweightFailureNoTextureSetCount() const
{
  return mGlyphRenderer.GetFailureNoTextureSetCount();
}

uint32_t AtlasRendererBridge::GetLightweightLastFailedPlacementIndex() const
{
  return mGlyphRenderer.GetLastFailedPlacementIndex();
}

uint32_t AtlasRendererBridge::GetLightweightLastFailedGlyphIndex() const
{
  return mGlyphRenderer.GetLastFailedGlyphIndex();
}

Text::FontId AtlasRendererBridge::GetLightweightLastFailedFontId() const
{
  return mGlyphRenderer.GetLastFailedFontId();
}

Text::GlyphIndex AtlasRendererBridge::GetLightweightLastFailedGlyphId() const
{
  return mGlyphRenderer.GetLastFailedGlyphId();
}

float AtlasRendererBridge::GetLightweightLastFailedGlyphAdvance() const
{
  return mGlyphRenderer.GetLastFailedGlyphAdvance();
}

float AtlasRendererBridge::GetLightweightLastFailedGlyphWidth() const
{
  return mGlyphRenderer.GetLastFailedGlyphWidth();
}

float AtlasRendererBridge::GetLightweightLastFailedGlyphHeight() const
{
  return mGlyphRenderer.GetLastFailedGlyphHeight();
}

void AtlasRendererBridge::EnsureRenderer()
{
  if(!mRenderer && HasRenderableGlyphs())
  {
    mRenderer = Text::AtlasRenderer::New();
  }
}

void AtlasRendererBridge::ResetRenderer()
{
  DetachRendererFromHost();
  mRenderer.Reset();
}

bool AtlasRendererBridge::UpdateRenderData()
{
  if(nullptr == mAdapter)
  {
    mImpl->Clear();
    return false;
  }

  const uint32_t renderableGlyphCount = mAdapter->GetRenderableGlyphCount();
  if(0u == renderableGlyphCount)
  {
    mImpl->Clear();
    return false;
  }

  if(!mRenderer)
  {
    mRenderer = Text::AtlasRenderer::New();
  }

  if(!IsRendererCreated())
  {
    mImpl->Clear();
    return false;
  }

  const uint32_t indicesToValidate[] = {0u, renderableGlyphCount - 1u};
  for(uint32_t index = 0u; index < 2u; ++index)
  {
    const uint32_t  placementIndex = indicesToValidate[index];
    GlyphPlacement  placement;
    Text::GlyphInfo glyph;
    Vector2         position;

    if(!mAdapter->GetGlyphPlacement(placementIndex, placement) ||
       !mAdapter->GetGlyphInfo(placement.glyphIndex, glyph) ||
       !mAdapter->GetRendererGlyphPosition(placementIndex, position))
    {
      mImpl->Clear();
      return false;
    }
  }

  mImpl->mValidatedGlyphCount = renderableGlyphCount;
  mImpl->mHasValidatedGlyphs  = true;
  return true;
}

void AtlasRendererBridge::SetLightweightRendererAllowed(bool allowed)
{
  if(mLightweightRendererAllowed == allowed)
  {
    return;
  }

  mLightweightRendererAllowed = allowed;

  if(!allowed)
  {
    mGlyphRenderer.Clear();
    if(mRenderHost)
    {
      mGlyphRenderer.SetRenderHost(mRenderHost);
    }
  }
}

bool AtlasRendererBridge::IsLightweightRendererAllowed() const
{
  return mLightweightRendererAllowed;
}

void AtlasRendererBridge::SetRenderHost(Actor renderHost)
{
  if(!renderHost)
  {
    DetachRendererFromHost();
  }

  mRenderHost = renderHost;
  mGlyphRenderer.SetRenderHost(renderHost);
}

Actor AtlasRendererBridge::GetRenderHost() const
{
  return mRenderHost;
}

void AtlasRendererBridge::SetTextControlActor(Actor textControlActor)
{
  mTextControlActor = textControlActor;
}

Actor AtlasRendererBridge::GetTextControlActor() const
{
  return mTextControlActor;
}

bool AtlasRendererBridge::AttachRendererToHost()
{
  if(!HasRenderHost() || !HasRenderableGlyphs() || !HasViewInterfaceAdapter())
  {
    return false;
  }

  if(ENABLE_TEXT_VISUALIZER_LIGHTWEIGHT_RENDERER && mLightweightRendererAllowed && (nullptr != mAdapter))
  {
    ++mLightweightRenderAttemptCount;

    mGlyphRenderer.SetRenderHost(mRenderHost);
    if(mGlyphRenderer.Render(*mAdapter) && mGlyphRenderer.AttachOutputToHost())
    {
      Actor output = mGlyphRenderer.GetOutputActor();
      if(output)
      {
        if(mRendererOutput && mRendererOutput != output)
        {
          Actor previousParent = mRendererOutput.GetParent();
          if(previousParent == mRenderHost)
          {
            mRendererOutput.Unparent();
          }
        }

        mRendererOutput   = output;
        mRendererAttached = true;
        ++mRenderCallCount;
        ++mLightweightRenderSuccessCount;
        return true;
      }
    }

    ++mLightweightRenderFallbackCount;
  }

  EnsureRenderer();
  if(!IsRendererCreated())
  {
    return false;
  }

  mAlignmentOffset = 0.0f;
  mViewInterface.ResetDiagnostics();
  ++mRenderCallCount;

  Actor output = mRenderer->Render(mViewInterface,
                                   HasTextControlActor() ? mTextControlActor : mRenderHost,
                                   mAnimatablePropertyIndex,
                                   mAlignmentOffset,
                                   mDepth);
  if(!output)
  {
    return false;
  }

  Actor outputParent = output.GetParent();
  if(outputParent && outputParent != mRenderHost)
  {
    return false;
  }

  if(mRendererOutput && mRendererOutput != output)
  {
    Actor previousParent = mRendererOutput.GetParent();
    if(previousParent == mRenderHost)
    {
      mRendererOutput.Unparent();
    }
  }

  if(!output.GetParent())
  {
    mRenderHost.Add(output);
    ++mAttachCallCount;
  }

  mRendererOutput   = output;
  mRendererAttached = true;
  return true;
}

void AtlasRendererBridge::DetachRendererFromHost()
{
  if(mRendererOutput)
  {
    Actor parent = mRendererOutput.GetParent();
    if(parent)
    {
      mRendererOutput.Unparent();
    }
    mRendererOutput.Reset();
  }

  mGlyphRenderer.DetachOutputFromHost();
  mRendererAttached = false;
}

} // namespace Dali::Ui::Internal::TextVisualizer
