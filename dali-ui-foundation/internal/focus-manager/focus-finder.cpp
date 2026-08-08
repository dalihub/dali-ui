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

/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Modified by joogab yun(joogab.yun@samsung.com)
 */

// CLASS HEADER
#include "focus-finder.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/actors/layer.h>
#include <algorithm>
#include <vector>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace FocusFinder
{
namespace
{
static constexpr float FULLY_TRANSPARENT(0.01f); ///< Alpha values must rise above this, before an object is considered to be visible.

static int MajorAxisDistanceRaw(Dali::Ui::FocusDirection direction, Dali::Bounds source, Dali::Bounds dest)
{
  switch(direction)
  {
    case Dali::Ui::FocusDirection::LEFT:
    {
      return static_cast<int>(source.Left() - dest.Right());
    }
    case Dali::Ui::FocusDirection::RIGHT:
    {
      return static_cast<int>(dest.Left() - source.Right());
    }
    case Dali::Ui::FocusDirection::UP:
    {
      return static_cast<int>(source.Top() - dest.Bottom());
    }
    case Dali::Ui::FocusDirection::DOWN:
    {
      return static_cast<int>(dest.Top() - source.Bottom());
    }
    default:
    {
      return 0;
    }
  }
}

/**
 * @return The distance from the edge furthest in the given direction
 *   of source to the edge nearest in the given direction of dest.
 *   If the dest is not in the direction from source, return 0.
 */
static int MajorAxisDistance(Dali::Ui::FocusDirection direction, Dali::Bounds source, Dali::Bounds dest)
{
  return std::max(0, MajorAxisDistanceRaw(direction, source, dest));
}

static int MajorAxisDistanceToFarEdgeRaw(Dali::Ui::FocusDirection direction, Dali::Bounds source, Dali::Bounds dest)
{
  switch(direction)
  {
    case Dali::Ui::FocusDirection::LEFT:
    {
      return static_cast<int>(source.Left() - dest.Left());
    }
    case Dali::Ui::FocusDirection::RIGHT:
    {
      return static_cast<int>(dest.Right() - source.Right());
    }
    case Dali::Ui::FocusDirection::UP:
    {
      return static_cast<int>(source.Top() - dest.Top());
    }
    case Dali::Ui::FocusDirection::DOWN:
    {
      return static_cast<int>(dest.Bottom() - source.Bottom());
    }
    default:
    {
      return 0;
    }
  }
}

/**
 * @return The distance along the major axis w.r.t the direction from the
 *   edge of source to the far edge of dest.
 *   If the dest is not in the direction from source, return 1
 */
static int MajorAxisDistanceToFarEdge(Dali::Ui::FocusDirection direction, Dali::Bounds source, Dali::Bounds dest)
{
  return std::max(1, MajorAxisDistanceToFarEdgeRaw(direction, source, dest));
}

/**
 * Find the distance on the minor axis w.r.t the direction to the nearest
 * edge of the destination rectangle.
 * @param direction the direction (up, down, left, right)
 * @param source The source rect.
 * @param dest The destination rect.
 * @return The distance.
 */
static int MinorAxisDistance(Dali::Ui::FocusDirection direction, Dali::Bounds source, Dali::Bounds dest)
{
  switch(direction)
  {
    case Dali::Ui::FocusDirection::LEFT:
    case Dali::Ui::FocusDirection::RIGHT:
    {
      // the distance between the center verticals
      return std::abs(static_cast<int>((source.Top() + (source.Bottom() - source.Top()) * 0.5f) -
                      (dest.Top() + (dest.Bottom() - dest.Top()) * 0.5f)));
    }
    case Dali::Ui::FocusDirection::UP:
    case Dali::Ui::FocusDirection::DOWN:
    {
      // the distance between the center horizontals
      return std::abs(static_cast<int>((source.Left() + (source.Right() - source.Left()) * 0.5f) -
                      (dest.Left() + (dest.Right() - dest.Left()) * 0.5f)));
    }
    default:
    {
      return 0;
    }
  }
}

/**
 * Calculate distance given major and minor axis distances.
 * @param majorAxisDistance The majorAxisDistance
 * @param minorAxisDistance The minorAxisDistance
 * @return The distance
 */
static uint64_t GetWeightedDistanceFor(int majorAxisDistance, int minorAxisDistance)
{
  return 13 * static_cast<int64_t>(majorAxisDistance) * static_cast<int64_t>(majorAxisDistance) + static_cast<int64_t>(minorAxisDistance) * static_cast<int64_t>(minorAxisDistance);
}

/**
 * Is destRect a candidate for the next focus given the direction?
 * @param srcRect The source rect.
 * @param destRect The dest rect.
 * @param direction The direction (up, down, left, right)
 * @return Whether destRect is a candidate.
 */
static bool IsCandidate(Dali::Bounds srcRect, Dali::Bounds destRect, Dali::Ui::FocusDirection direction)
{
  switch(direction)
  {
    case Dali::Ui::FocusDirection::LEFT:
    {
      return (srcRect.Right() > destRect.Right() || srcRect.Left() >= destRect.Right()) && srcRect.Left() > destRect.Left();
    }
    case Dali::Ui::FocusDirection::RIGHT:
    {
      return (srcRect.Left() < destRect.Left() || srcRect.Right() <= destRect.Left()) && srcRect.Right() < destRect.Right();
    }
    case Dali::Ui::FocusDirection::UP:
    {
      return (srcRect.Bottom() > destRect.Bottom() || srcRect.Top() >= destRect.Bottom()) && srcRect.Top() > destRect.Top();
    }
    case Dali::Ui::FocusDirection::DOWN:
    {
      return (srcRect.Top() < destRect.Top() || srcRect.Bottom() <= destRect.Top()) && srcRect.Bottom() < destRect.Bottom();
    }
    default:
    {
      return false;
    }
  }
  return false;
}

/**
 * Is dest in a given direction from src?
 * @param direction the direction (up, down, left, right)
 * @param src The source rect
 * @param dest The dest rect
 */
static bool IsToDirectionOf(Dali::Ui::FocusDirection direction, Dali::Bounds src, Dali::Bounds dest)
{
  switch(direction)
  {
    case Dali::Ui::FocusDirection::LEFT:
    {
      return src.Left() >= dest.Right();
    }
    case Dali::Ui::FocusDirection::RIGHT:
    {
      return src.Right() <= dest.Left();
    }
    case Dali::Ui::FocusDirection::UP:
    {
      return src.Top() >= dest.Bottom();
    }
    case Dali::Ui::FocusDirection::DOWN:
    {
      return src.Bottom() <= dest.Top();
    }
    default:
    {
      return false;
    }
  }
}

/**
 * Do the given direction's axis of rect1 and rect2 overlap?
 * @param direction the direction (up, down, left, right)
 * @param rect1 The first rect
 * @param rect2 The second rect
 * @return whether the beams overlap
 */
static bool BeamsOverlap(Dali::Ui::FocusDirection direction, Dali::Bounds rect1, Dali::Bounds rect2)
{
  switch(direction)
  {
    case Dali::Ui::FocusDirection::LEFT:
    case Dali::Ui::FocusDirection::RIGHT:
    {
      return (rect2.Bottom() >= rect1.Top()) && (rect2.Top() <= rect1.Bottom());
    }
    case Dali::Ui::FocusDirection::UP:
    case Dali::Ui::FocusDirection::DOWN:
    {
      return (rect2.Right() >= rect1.Left()) && (rect2.Left() <= rect1.Right());
    }
    default:
    {
      return false;
    }
  }
}

/**
 * One rectangle may be another candidate than another by virtue of being exclusively in the beam of the source rect.
 * @param direction The direction (up, down, left, right)
 * @param source The source rect
 * @param rect1 The first rect
 * @param rect2 The second rect
 * @return Whether rect1 is a better candidate than rect2 by virtue of it being in src's beam
 */
static bool BeamBeats(Dali::Ui::FocusDirection direction, Dali::Bounds source, Dali::Bounds rect1, Dali::Bounds rect2)
{
  const bool rect1InSrcBeam = BeamsOverlap(direction, source, rect1);
  const bool rect2InSrcBeam = BeamsOverlap(direction, source, rect2);
  // if rect1 isn't exclusively in the src beam, it doesn't win
  if(rect2InSrcBeam || !rect1InSrcBeam)
  {
    return false;
  }
  // we know rect1 is in the beam, and rect2 is not
  // if rect1 is to the direction of, and rect2 is not, rect1 wins.
  // for example, for direction left, if rect1 is to the left of the source
  // and rect2 is below, then we always prefer the in beam rect1, since rect2
  // could be reached by going down.
  if(!IsToDirectionOf(direction, source, rect2))
  {
    return true;
  }
  // for horizontal directions, being exclusively in beam always wins
  if((direction == Dali::Ui::FocusDirection::LEFT || direction == Dali::Ui::FocusDirection::RIGHT))
  {
    return true;
  }
  // for vertical directions, beams only beat up to a point:
  // now, as long as rect2 isn't completely closer, rect1 wins
  // e.g for direction down, completely closer means for rect2's top
  // edge to be closer to the source's top edge than rect1's bottom edge.
  return (MajorAxisDistance(direction, source, rect1) < MajorAxisDistanceToFarEdge(direction, source, rect2));
}

bool IsBetterCandidate(Ui::FocusDirection direction, Bounds& focusedRect, Bounds& candidateRect, Bounds& bestCandidateRect)
{
  // to be a better candidate, need to at least be a candidate in the first place
  if(!IsCandidate(focusedRect, candidateRect, direction))
  {
    return false;
  }
  // we know that candidateRect is a candidate.. if bestCandidateRect is not a candidate,
  // candidateRect is better
  if(!IsCandidate(focusedRect, bestCandidateRect, direction))
  {
    return true;
  }
  // if candidateRect is better by beam, it wins
  if(BeamBeats(direction, focusedRect, candidateRect, bestCandidateRect))
  {
    return true;
  }
  // if bestCandidateRect is better, then candidateRect cant' be :)
  if(BeamBeats(direction, focusedRect, bestCandidateRect, candidateRect))
  {
    return false;
  }

  // otherwise, do fudge-tastic comparison of the major and minor axis
  return (GetWeightedDistanceFor(
            MajorAxisDistance(direction, focusedRect, candidateRect),
            MinorAxisDistance(direction, focusedRect, candidateRect)) < GetWeightedDistanceFor(MajorAxisDistance(direction, focusedRect, bestCandidateRect),
                                                                                               MinorAxisDistance(direction, focusedRect, bestCandidateRect)));
}

bool IsFocusable(Actor& actor)
{
  return (actor.GetProperty<bool>(Actor::Property::FOCUSABLE) &&
          actor.GetProperty<bool>(Actor::Property::ENABLED) &&
          actor.GetProperty<bool>(Actor::Property::VISIBLE) &&
          !actor.GetCurrentProperty<bool>(DevelActor::Property::WORLD_IGNORED) &&
          actor.GetProperty<Vector4>(Actor::Property::WORLD_COLOR).a > FULLY_TRANSPARENT);
}

bool CanTraverseFocus(Actor& actor)
{
  return (actor &&
          actor.GetProperty<bool>(Actor::Property::VISIBLE) &&
          !actor.GetCurrentProperty<bool>(DevelActor::Property::WORLD_IGNORED));
}

Actor FindNextFocus(Actor& actor, Actor& focusedActor, Bounds& focusedRect, Bounds& bestCandidateRect, Ui::FocusDirection direction)
{
  Actor nearestActor;
  if(CanTraverseFocus(actor) &&
     actor.GetProperty<bool>(Actor::Property::ALLOW_DESCENDANT_FOCUS))
  {
    // Recursively children
    const auto childCount = actor.GetChildCount();
    for(auto i = childCount; i > 0u; --i)
    {
      Dali::Actor child = actor.GetChildAt(i - 1);
      if(child && child != focusedActor && IsFocusable(child))
      {
        Bounds candidateRect = DevelActor::CalculateCurrentScreenExtents(child);

        if(IsBetterCandidate(direction, focusedRect, candidateRect, bestCandidateRect))
        {
          bestCandidateRect = candidateRect;
          nearestActor      = child;
        }
      }
      Actor nextActor = FindNextFocus(child, focusedActor, focusedRect, bestCandidateRect, direction);
      if(nextActor)
      {
        nearestActor = nextActor;
      }
    }
  }
  return nearestActor;
}

struct TraversalNode
{
  Actor  actor;
  Bounds rect;
  bool   includeSelf{false};
  bool   focusable{false};
};

struct FocusableEntry
{
  Actor  actor;
  Bounds rect;
  bool   focusable{false};
};

struct FocusFinderWorkspace
{
  std::vector<TraversalNode>  traversalStack;
  std::vector<FocusableEntry> sortedChildren;
  std::vector<FocusableEntry> focusableViews;
  bool                        inUse{false};
};

struct WorkspaceGuard
{
  explicit WorkspaceGuard(FocusFinderWorkspace& workspace)
  : workspace(workspace)
  {
    workspace.inUse = true;
  }

  ~WorkspaceGuard()
  {
    workspace.inUse = false;
  }

  FocusFinderWorkspace& workspace;
};

/**
 * Collects focusable views in reading order.
 * At each level, direct children are sorted by screen position (top-to-bottom,
 * left-to-right) before traversing the child subtree.
 */
void AddFocusables(Actor rootActor, Actor& focusedActor, FocusFinderWorkspace& workspace)
{
  if(!rootActor)
  {
    return;
  }

  workspace.traversalStack.clear();
  workspace.traversalStack.push_back({rootActor, Bounds(), false, false});

  while(!workspace.traversalStack.empty())
  {
    TraversalNode node = workspace.traversalStack.back();
    workspace.traversalStack.pop_back();

    Actor actor = node.actor;
    if(!CanTraverseFocus(actor))
    {
      continue;
    }

    if(node.includeSelf && node.focusable)
    {
      workspace.focusableViews.push_back({actor, node.rect, true});
    }

    if(!actor.GetProperty<bool>(Actor::Property::ALLOW_DESCENDANT_FOCUS))
    {
      continue;
    }

    const auto childCount = actor.GetChildCount();
    if(childCount == 0u)
    {
      continue;
    }

    workspace.sortedChildren.clear();
    for(auto i = 0u; i < childCount; ++i)
    {
      Actor child = actor.GetChildAt(i);
      if(!CanTraverseFocus(child))
      {
        continue;
      }

      const bool focusable   = IsFocusable(child);
      const bool hasChildren = child.GetProperty<bool>(Actor::Property::ALLOW_DESCENDANT_FOCUS);
      if(!focusable && !hasChildren)
      {
        continue;
      }

      workspace.sortedChildren.push_back({child, DevelActor::CalculateCurrentScreenExtents(child), focusable});
    }

    View       parentView = View::DownCast(actor);
    const bool isRtl      = parentView && parentView.GetEffectiveLayoutDirection() == Dali::LayoutDirection::RIGHT_TO_LEFT;

    std::stable_sort(workspace.sortedChildren.begin(), workspace.sortedChildren.end(),
                     [isRtl](const FocusableEntry& a, const FocusableEntry& b)
    {
      if(a.rect.y != b.rect.y)
      {
        return a.rect.y < b.rect.y;
      }
      return isRtl ? a.rect.x > b.rect.x : a.rect.x < b.rect.x;
    });

    for(auto iter = workspace.sortedChildren.rbegin(); iter != workspace.sortedChildren.rend(); ++iter)
    {
      workspace.traversalStack.push_back({iter->actor, iter->rect, true, iter->focusable});
    }
  }
}

View GetNextFocusableViewInOrder(Actor rootActor, View focusedView, Ui::FocusDirection direction, FocusFinderWorkspace& workspace)
{
  if(!rootActor)
  {
    return View();
  }

  Actor focusedActor = focusedView;

  workspace.focusableViews.clear();

  AddFocusables(rootActor, focusedActor, workspace);

  if(workspace.focusableViews.empty())
  {
    return View();
  }

  // Find the focused view's position in the ordered list
  int focusedIndex = -1;
  for(size_t i = 0; i < workspace.focusableViews.size(); ++i)
  {
    if(workspace.focusableViews[i].actor == focusedActor)
    {
      focusedIndex = static_cast<int>(i);
      break;
    }
  }

  int nextIndex = -1;
  if(direction == Ui::FocusDirection::FORWARD)
  {
    nextIndex = (focusedIndex >= 0) ? focusedIndex + 1 : 0;
  }
  else // BACKWARD
  {
    nextIndex = (focusedIndex >= 0) ? focusedIndex - 1 : static_cast<int>(workspace.focusableViews.size()) - 1;
  }

  if(nextIndex >= 0 && nextIndex < static_cast<int>(workspace.focusableViews.size()))
  {
    return View::DownCast(workspace.focusableViews[nextIndex].actor);
  }

  return View();
}

} // unnamed namespace

View GetNearestFocusableView(Actor rootActor, View focusedView, Ui::FocusDirection direction)
{
  Actor focusedActor = focusedView;

  Actor nearestActor;
  if(!rootActor)
  {
    return View();
  }

  Bounds focusedRect;
  if(!focusedActor)
  {
    // If there is no currently focused view, it is searched based on the upper left corner of the current window.
    Bounds rootRect = DevelActor::CalculateCurrentScreenExtents(rootActor);
    focusedRect     = Bounds(rootRect.x, rootRect.y, 0.f, 0.f);
  }
  else
  {
    focusedRect = DevelActor::CalculateCurrentScreenExtents(focusedActor);
  }

  // initialize the best candidate to something impossible
  // (so the first plausible view will become the best choice)
  Bounds bestCandidateRect = focusedRect;
  switch(direction)
  {
    case Ui::FocusDirection::LEFT:
    {
      bestCandidateRect.x += 1;
      break;
    }
    case Ui::FocusDirection::RIGHT:
    {
      bestCandidateRect.x -= 1;
      break;
    }
    case Ui::FocusDirection::UP:
    {
      bestCandidateRect.y += 1;
      break;
    }
    case Ui::FocusDirection::DOWN:
    {
      bestCandidateRect.y -= 1;
      break;
    }
    default:
    {
      break;
    }
  }

  nearestActor = FindNextFocus(rootActor, focusedActor, focusedRect, bestCandidateRect, direction);
  return View::DownCast(nearestActor);
}

View GetNextFocusableViewInOrder(Actor rootActor, View focusedView, Ui::FocusDirection direction)
{
  static FocusFinderWorkspace reusableWorkspace;

  if(reusableWorkspace.inUse)
  {
    FocusFinderWorkspace localWorkspace;
    return GetNextFocusableViewInOrder(rootActor, focusedView, direction, localWorkspace);
  }

  WorkspaceGuard guard(reusableWorkspace);
  return GetNextFocusableViewInOrder(rootActor, focusedView, direction, reusableWorkspace);
}

} // namespace FocusFinder
} // namespace Internal
} // namespace Ui
} // namespace Dali
