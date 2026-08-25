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
#include <dali-ui-foundation/internal/text/anchor/anchor-interaction-data.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/math/rect.h>
#include <algorithm>
#include <cstdint>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/traits/attachment-id.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

namespace
{

const AttachmentId ANCHOR_INTERACTION_DATA_ATTACHMENT_ID = AttachmentId::Alloc();

struct AnchorActorSnapshot
{
  Dali::Ui::Text::CharacterIndex characterIndex{0u};
  Dali::Ui::Text::Length         numberOfCharacters{0u};
  std::string                    href{};
  std::string                    name{};
  Rect<float>                    bounds{};
};

bool ContainsPoint(const Rect<float>& rectangle, const Vector2& point)
{
  const bool insideX = point.x >= rectangle.x && point.x <= rectangle.x + rectangle.width;
  const bool insideY = point.y >= rectangle.y && point.y <= rectangle.y + rectangle.height;
  return insideX && insideY;
}

Rect<float> UnionAnchorRectangles(const std::vector<Rect<float> >& rectangles)
{
  if(rectangles.empty())
  {
    return Rect<float>();
  }

  float minX      = rectangles[0u].x;
  float minY      = rectangles[0u].y;
  float maxRight  = rectangles[0u].x + rectangles[0u].width;
  float maxBottom = rectangles[0u].y + rectangles[0u].height;

  for(uint32_t index = 1u; index < rectangles.size(); ++index)
  {
    const Rect<float>& rectangle = rectangles[index];
    minX                         = std::min(minX, rectangle.x);
    minY                         = std::min(minY, rectangle.y);
    maxRight                     = std::max(maxRight, rectangle.x + rectangle.width);
    maxBottom                    = std::max(maxBottom, rectangle.y + rectangle.height);
  }

  return Rect<float>(minX, minY, maxRight - minX, maxBottom - minY);
}

std::vector<AnchorActorSnapshot> BuildAnchorActorSnapshotsFromHitRegions(const std::vector<Dali::Ui::Text::AsyncAnchorHitRegion>& hitRegions)
{
  std::vector<AnchorActorSnapshot> snapshots;
  snapshots.reserve(hitRegions.size());

  for(const auto& hitRegion : hitRegions)
  {
    if(hitRegion.rectangles.empty())
    {
      continue;
    }

    const Rect<float> bounds = UnionAnchorRectangles(hitRegion.rectangles);
    if(bounds.width <= 0.0f || bounds.height <= 0.0f)
    {
      continue;
    }

    AnchorActorSnapshot snapshot;
    snapshot.characterIndex     = hitRegion.characterIndex;
    snapshot.numberOfCharacters = hitRegion.numberOfCharacters;
    snapshot.href               = hitRegion.href;
    snapshot.name               = hitRegion.href;
    snapshot.bounds             = bounds;
    snapshots.push_back(std::move(snapshot));
  }

  return snapshots;
}

Dali::Ui::TextAnchor CreateTextAnchorActor(const AnchorActorSnapshot& snapshot, const Vector2& contentOffset)
{
  Dali::Ui::TextAnchor anchorActor = Dali::Ui::TextAnchor::New();
  anchorActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  anchorActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  anchorActor.SetProperty(Actor::Property::POSITION,
                          Vector2(snapshot.bounds.x + contentOffset.x, snapshot.bounds.y + contentOffset.y));
  anchorActor.SetProperty(Actor::Property::SIZE, Vector2(snapshot.bounds.width, snapshot.bounds.height));
  anchorActor.SetProperty(Actor::Property::NAME, Dali::Integration::ToPropertyValue(snapshot.name));
  anchorActor.SetProperty(Dali::Ui::TextAnchor::Property::START_CHARACTER_INDEX,
                          static_cast<int32_t>(snapshot.characterIndex));
  anchorActor.SetProperty(Dali::Ui::TextAnchor::Property::END_CHARACTER_INDEX,
                          static_cast<int32_t>(snapshot.characterIndex + snapshot.numberOfCharacters));
  anchorActor.SetProperty(Dali::Ui::TextAnchor::Property::URI, Dali::Integration::ToPropertyValue(snapshot.href));
  return anchorActor;
}

Dali::Ui::Text::AsyncAnchorClickedState MakeClickedState(const Dali::Ui::Text::AsyncAnchorHitRegion& region)
{
  return Dali::Ui::Text::AsyncAnchorClickedState{
    region.characterIndex,
    region.numberOfCharacters,
    region.href};
}

bool MatchesAnchorKey(const Dali::Ui::Text::AsyncAnchorClickedState& clickedAnchor,
                      const Dali::Ui::Text::AsyncAnchorHitRegion&    region)
{
  return clickedAnchor.characterIndex == region.characterIndex &&
         clickedAnchor.numberOfCharacters == region.numberOfCharacters &&
         clickedAnchor.href == region.href;
}

} // namespace

AnchorInteractionData::~AnchorInteractionData()
{
  ClearA11yAnchors();
}

void AnchorInteractionData::StartTouch(const Vector2& position)
{
  mTouchPosition = position;
  mIsTouchDown   = true;
}

bool AnchorInteractionData::IsTouchDown() const
{
  return mIsTouchDown;
}

const Vector2& AnchorInteractionData::GetTouchPosition() const
{
  return mTouchPosition;
}

void AnchorInteractionData::EndTouch()
{
  mIsTouchDown = false;
}

bool AnchorInteractionData::HasHitRegions() const
{
  return !mHitRegions.empty();
}

void AnchorInteractionData::Clear()
{
  ClearA11yAnchors();
  mHitRegions.clear();
  mClickedAnchors.clear();
  EndTouch();
}

void AnchorInteractionData::SetHitRegions(std::vector<Dali::Ui::Text::AsyncAnchorHitRegion>&& hitRegions)
{
  ClearA11yAnchors();
  mHitRegions = std::move(hitRegions);
}

void AnchorInteractionData::ClearA11yAnchors()
{
  for(auto& anchorActor : mTextAnchorActors)
  {
    if(anchorActor)
    {
      anchorActor.Unparent();
    }
  }
  mTextAnchorActors.clear();
}

const std::vector<Dali::Ui::TextAnchor>& AnchorInteractionData::GetA11yAnchors() const
{
  return mTextAnchorActors;
}

bool AnchorInteractionData::SetA11yAnchors(Dali::Ui::View owner, std::vector<Dali::Ui::TextAnchor>&& anchorActors)
{
  ClearA11yAnchors();

  if(!owner || anchorActors.empty())
  {
    return false;
  }

  mTextAnchorActors.reserve(anchorActors.size());
  for(auto& anchorActor : anchorActors)
  {
    if(anchorActor)
    {
      owner.Add(anchorActor);
      mTextAnchorActors.push_back(std::move(anchorActor));
    }
  }
  return !mTextAnchorActors.empty();
}

bool AnchorInteractionData::UpdateA11yAnchorsFromHitRegions(Dali::Ui::View owner, const Vector2& contentOffset)
{
  ClearA11yAnchors();

  if(!owner || mHitRegions.empty())
  {
    return false;
  }

  const std::vector<AnchorActorSnapshot> snapshots = BuildAnchorActorSnapshotsFromHitRegions(mHitRegions);
  if(snapshots.empty())
  {
    return false;
  }

  mTextAnchorActors.reserve(snapshots.size());

  for(const auto& snapshot : snapshots)
  {
    Dali::Ui::TextAnchor anchorActor = CreateTextAnchorActor(snapshot, contentOffset);
    owner.Add(anchorActor);
    mTextAnchorActors.push_back(anchorActor);
  }
  return !mTextAnchorActors.empty();
}

Dali::Ui::Text::AsyncAnchorHitRegion* AnchorInteractionData::FindHitRegion(const Vector2& point)
{
  for(auto regionIt = mHitRegions.rbegin(); regionIt != mHitRegions.rend(); ++regionIt)
  {
    Dali::Ui::Text::AsyncAnchorHitRegion& region = *regionIt;
    for(const auto& rectangle : region.rectangles)
    {
      if(ContainsPoint(rectangle, point))
      {
        return &region;
      }
    }
  }

  return nullptr;
}

Dali::Ui::Text::AsyncAnchorHitRegion* AnchorInteractionData::FindHitRegion(Dali::Ui::Text::CharacterIndex characterIndex)
{
  for(auto regionIt = mHitRegions.rbegin(); regionIt != mHitRegions.rend(); ++regionIt)
  {
    Dali::Ui::Text::AsyncAnchorHitRegion& region = *regionIt;
    if(characterIndex >= region.characterIndex &&
       characterIndex < region.characterIndex + region.numberOfCharacters)
    {
      return &region;
    }
  }

  return nullptr;
}

bool AnchorInteractionData::MarkClicked(Dali::Ui::Text::AsyncAnchorHitRegion& region)
{
  const auto clickedIt = std::find_if(mClickedAnchors.begin(),
                                      mClickedAnchors.end(),
                                      [&region](const Dali::Ui::Text::AsyncAnchorClickedState& clickedAnchor)
  {
    return MatchesAnchorKey(clickedAnchor, region);
  });

  region.isClicked = true;
  if(clickedIt != mClickedAnchors.end())
  {
    return false;
  }

  mClickedAnchors.push_back(MakeClickedState(region));
  return true;
}

void AnchorInteractionData::PruneClickedAnchors()
{
  mClickedAnchors.erase(std::remove_if(mClickedAnchors.begin(),
                                       mClickedAnchors.end(),
                                       [this](const Dali::Ui::Text::AsyncAnchorClickedState& clickedAnchor)
  {
    const auto hitRegionIt = std::find_if(mHitRegions.begin(),
                                          mHitRegions.end(),
                                          [&clickedAnchor](const Dali::Ui::Text::AsyncAnchorHitRegion& region)
    {
      return MatchesAnchorKey(clickedAnchor, region);
    });

    return hitRegionIt == mHitRegions.end();
  }),
                        mClickedAnchors.end());
}

std::vector<Dali::Ui::Text::AsyncAnchorClickedState> AnchorInteractionData::GetClickedAnchors() const
{
  return mClickedAnchors;
}

AnchorInteractionData* GetAnchorInteractionData(Dali::Ui::View owner)
{
  if(!owner)
  {
    return nullptr;
  }

  return owner.GetAttachment<AnchorInteractionData>(ANCHOR_INTERACTION_DATA_ATTACHMENT_ID);
}

AnchorInteractionData& GetOrCreateAnchorInteractionData(Dali::Ui::View owner)
{
  DALI_ASSERT_ALWAYS(owner && "Anchor attachment requires a valid owner");

  AnchorInteractionData* data = GetAnchorInteractionData(owner);
  if(!data)
  {
    owner.SetAttachment(ANCHOR_INTERACTION_DATA_ATTACHMENT_ID, Dali::MakeUnique<AnchorInteractionData>());
    data = GetAnchorInteractionData(owner);
  }

  DALI_ASSERT_ALWAYS(data && "Anchor attachment creation failed");
  return *data;
}

void ClearAnchorInteractionData(Dali::Ui::View owner)
{
  AnchorInteractionData* data = GetAnchorInteractionData(owner);
  if(data)
  {
    data->Clear();
  }
}

void ClearA11yAnchors(Dali::Ui::View owner)
{
  AnchorInteractionData* data = GetAnchorInteractionData(owner);
  if(data)
  {
    data->ClearA11yAnchors();
  }
}

const std::vector<Dali::Ui::TextAnchor>& GetA11yAnchors(Dali::Ui::View owner)
{
  static const std::vector<Dali::Ui::TextAnchor> EMPTY_ANCHORS;

  auto* data = GetAnchorInteractionData(owner);
  return data ? data->GetA11yAnchors() : EMPTY_ANCHORS;
}

bool SetAnchorHitRegions(Dali::Ui::View                                      owner,
                         std::vector<Dali::Ui::Text::AsyncAnchorHitRegion>&& hitRegions)
{
  if(!owner)
  {
    return false;
  }

  if(hitRegions.empty())
  {
    ClearAnchorInteractionData(owner);
    return false;
  }

  AnchorInteractionData& data = GetOrCreateAnchorInteractionData(owner);
  data.SetHitRegions(std::move(hitRegions));
  data.PruneClickedAnchors();
  return true;
}

bool SetA11yAnchors(Dali::Ui::View owner, std::vector<Dali::Ui::TextAnchor>&& anchorActors)
{
  if(!owner)
  {
    return false;
  }

  AnchorInteractionData* data = GetAnchorInteractionData(owner);
  if(anchorActors.empty())
  {
    if(data)
    {
      data->ClearA11yAnchors();
    }
    return false;
  }

  return GetOrCreateAnchorInteractionData(owner).SetA11yAnchors(owner, std::move(anchorActors));
}

bool UpdateA11yAnchorsFromHitRegions(Dali::Ui::View owner, const Vector2& contentOffset)
{
  AnchorInteractionData* data = GetAnchorInteractionData(owner);
  if(data)
  {
    return data->UpdateA11yAnchorsFromHitRegions(owner, contentOffset);
  }
  return false;
}

std::vector<Dali::Ui::Text::AsyncAnchorClickedState> GetAnchorClickedStates(Dali::Ui::View owner)
{
  AnchorInteractionData* data = GetAnchorInteractionData(owner);
  return data ? data->GetClickedAnchors() : std::vector<Dali::Ui::Text::AsyncAnchorClickedState>();
}

AnchorHitResult HitTestAnchor(Dali::Ui::View owner, const Vector2& point)
{
  AnchorInteractionData* data = GetAnchorInteractionData(owner);
  if(!data)
  {
    return AnchorHitResult{};
  }

  Dali::Ui::Text::AsyncAnchorHitRegion* region = data->FindHitRegion(point);
  if(!region)
  {
    return AnchorHitResult{};
  }

  return AnchorHitResult{
    true,
    data->MarkClicked(*region),
    region->href};
}

AnchorHitResult ActivateAnchor(Dali::Ui::View owner, Dali::Ui::Text::CharacterIndex characterIndex)
{
  AnchorInteractionData* data = GetAnchorInteractionData(owner);
  if(!data)
  {
    return AnchorHitResult{};
  }

  Dali::Ui::Text::AsyncAnchorHitRegion* region = data->FindHitRegion(characterIndex);
  if(!region)
  {
    return AnchorHitResult{};
  }

  return AnchorHitResult{
    true,
    data->MarkClicked(*region),
    region->href};
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
