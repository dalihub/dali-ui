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
#include "view-visual-data.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/common/stage.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/constraint-integ.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/animation/constraint-source.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>

#include <algorithm>
#include <unordered_set>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/ui-constraint-tag-ranges.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

using Dali::Integration::ToStdString;

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

static constexpr uint32_t DEFAULT_CORNER_RADIUS_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START +
                                                               8);
static constexpr uint32_t DEFAULT_CORNER_SQUARENESS_CONSTRAINT_TAG(
  Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 9);

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VIEW_VISUALS");
#endif

template<typename T>
void Remove(Dictionary<T>& keyValues, const std::string& name)
{
  keyValues.Remove(name);
}

void Remove(DictionaryKeys& keys, const std::string& name)
{
  DictionaryKeys::iterator iter = std::find(keys.begin(), keys.end(), name);
  if(iter != keys.end())
  {
    keys.erase(iter);
  }
}

/**
 *  Finds visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual(Property::Index targetIndex, const RegisteredVisualContainer& visuals,
                RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    if((*iter)->index == targetIndex)
    {
      return true;
    }
  }
  return false;
}

/**
 *  Finds visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual(std::string visualName, const RegisteredVisualContainer& visuals,
                RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Ui::Visual::Base visual = (*iter)->visual;
    if(visual && visual.GetName() == visualName)
    {
      return true;
    }
  }
  return false;
}

/**
 *  Finds visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual(const Ui::Visual::Base findVisual, const RegisteredVisualContainer& visuals,
                RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Ui::Visual::Base visual = (*iter)->visual;
    if(visual && visual == findVisual)
    {
      return true;
    }
  }
  return false;
}

/**
 *  Finds internal visual in given array, returning true if found along with the iterator for that visual as a out
 * parameter
 */
bool FindVisual(const Visual::Base& findInternalVisual, const RegisteredVisualContainer& visuals,
                RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Visual::Base& visual = Ui::GetImplementation((*iter)->visual);
    if((&visual == &findInternalVisual))
    {
      return true;
    }
  }
  return false;
}

void FindChangeableVisuals(Dictionary<Property::Map>& stateVisualsToAdd, Dictionary<Property::Map>& stateVisualsToChange,
                           DictionaryKeys& stateVisualsToRemove)
{
  DictionaryKeys copyOfStateVisualsToRemove = stateVisualsToRemove;

  for(DictionaryKeys::iterator iter = copyOfStateVisualsToRemove.begin(); iter != copyOfStateVisualsToRemove.end();
      ++iter)
  {
    const std::string& visualName = (*iter);
    Property::Map*     toMap      = stateVisualsToAdd.Find(visualName);
    if(toMap)
    {
      stateVisualsToChange.Add(visualName, *toMap);
      stateVisualsToAdd.Remove(visualName);
      Remove(stateVisualsToRemove, visualName);
    }
  }
}

Ui::Visual::Base GetVisualByName(const RegisteredVisualContainer& visuals, const std::string& visualName)
{
  Ui::Visual::Base visualHandle;

  RegisteredVisualContainer::Iterator iter;
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Ui::Visual::Base visual = (*iter)->visual;
    if(visual && visual.GetName() == visualName)
    {
      visualHandle = visual;
      break;
    }
  }
  return visualHandle;
}

Ui::Visual::Base GetVisualByIndex(const RegisteredVisualContainer& visuals, Property::Index index)
{
  Ui::Visual::Base visualHandle;

  RegisteredVisualContainer::Iterator iter;
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    if((*iter)->index == index)
    {
      visualHandle = (*iter)->visual;
      break;
    }
  }
  return visualHandle;
}

/**
 * Move visual from source to destination container
 */
void MoveVisual(RegisteredVisualContainer::Iterator sourceIter, RegisteredVisualContainer& source,
                RegisteredVisualContainer& destination)
{
  Ui::Visual::Base visual = (*sourceIter)->visual;
  if(visual)
  {
    RegisteredVisual* rv = source.Release(sourceIter);
    destination.PushBack(rv);
  }
}

/**
 * Discard visual from source to visual factory.
 */
void DiscardVisual(RegisteredVisualContainer::Iterator sourceIter, RegisteredVisualContainer& source)
{
  Ui::Visual::Base visual = (*sourceIter)->visual;
  if(visual)
  {
    if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
    {
      Ui::VisualFactory::Get().DiscardVisual(visual);
    }
  }

  source.Erase(sourceIter);
}

/**
 * @brief Set visual on scene
 * @param[in] visualImpl The visual
 * @param[in] viewImpl Actor with renderers
 * @note When offscreen rendering is on, visuals drawn out of view's area(depth index in range of BACKGROUND_EFFECT
 * or FOREGROUND_EFFECT) should swap its renderer to cache renderers. So changing visual's depth index may not apply
 * instantaneously. Turn offscreen rendering off and on again.
 */
void SetVisualOnScene(Internal::Visual::Base& visualImpl, Integration::ViewImpl& viewImpl)
{
  Actor self = viewImpl.Self();
  visualImpl.SetOnScene(self);

  Ui::View                         handle = Ui::View(viewImpl.GetOwner());
  Ui::View::OffScreenRenderingType offscreenRenderingType =
    Ui::View::OffScreenRenderingType(handle.GetProperty<int32_t>(Ui::View::Property::OFFSCREEN_RENDERING));
  if(offscreenRenderingType != Ui::View::OffScreenRenderingType::NONE)
  {
    if(!visualImpl.IsOffscreenRenderingCaptureEnabled())
    {
      Renderer renderer = visualImpl.GetRenderer();
      self.RemoveRenderer(renderer);
      self.AddCacheRenderer(renderer);
    }
  }
}

/**
 * @brief Remove visual's renderer from cache renderers(offscreen renderers)
 * @param[in] visualImpl The visual
 * @param[in] viewImpl Actor with renderers
 */
void SetVisualOffScene(Internal::Visual::Base& visualImpl, Integration::ViewImpl& viewImpl)
{
  Actor self = viewImpl.Self();

  Ui::View                         handle = Ui::View(viewImpl.GetOwner());
  Ui::View::OffScreenRenderingType offscreenRenderingType =
    Ui::View::OffScreenRenderingType(handle.GetProperty<int32_t>(Ui::View::Property::OFFSCREEN_RENDERING));
  if(offscreenRenderingType != Ui::View::OffScreenRenderingType::NONE)
  {
    if(!visualImpl.IsOffscreenRenderingCaptureEnabled())
    {
      VisualRenderer renderer = visualImpl.GetRenderer();
      self.RemoveCacheRenderer(renderer);

      // Don't need to call self.AddRenderer(renderer) again, because SetOffScene() will remove renderer automatically.
    }
  }
  visualImpl.SetOffScene(self);
}

/**
 * @brief Iterate through given container and setOffScene any visual found
 *
 * @param[in] container Container of visuals
 * @param[in] viewImpl Actor to remove visuals from
 */
void SetVisualsOffScene(const RegisteredVisualContainer& container, Integration::ViewImpl& viewImpl)
{
  for(auto iter = container.Begin(), end = container.End(); iter != end; iter++)
  {
    if((*iter)->visual)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::SetOffScene Setting visual(%d) off stage\n", (*iter)->index);
      SetVisualOffScene(Ui::GetImplementation((*iter)->visual), viewImpl);
    }
  }
}
} // unnamed namespace

ViewDataImpl::VisualData::VisualData(ViewDataImpl& outer)
: mVisualEventSignal(),
  mOuter(outer),
  mOffscreenRenderingEnabled(false),
  mCornerRadiusValueAdded(false),
  mCornerSquarenessValueAdded(false)
{
}

ViewDataImpl::VisualData::~VisualData()
{
}

void ViewDataImpl::VisualData::ConnectScene(Actor parent)
{
  for(RegisteredVisualContainer::Iterator iter = mVisuals.Begin(); iter != mVisuals.End(); iter++)
  {
    // Check whether the visual is empty and enabled
    if((*iter)->visual && (*iter)->enabled)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::OnSceneConnection Setting visual(%d) on scene\n",
                    (*iter)->index);
      SetVisualOnScene(Ui::GetImplementation((*iter)->visual), mOuter.mViewImpl);
    }
  }
}

void ViewDataImpl::VisualData::ClearScene(Actor parent)
{
  SetVisualsOffScene(mVisuals, mOuter.mViewImpl);

  if(!mRemoveVisuals.Empty())
  {
    std::reverse(mRemoveVisuals.Begin(), mRemoveVisuals.End());

    while(!mRemoveVisuals.Empty())
    {
      auto removalIter = mRemoveVisuals.End() - 1u;
      SetVisualOffScene(Ui::GetImplementation((*removalIter)->visual), mOuter.mViewImpl);

      // Discard removed visual. It will be destroyed at next Idle time.
      DiscardVisual(removalIter, mRemoveVisuals);
    }
  }

  for(auto replacedIter = mVisuals.Begin(), end = mVisuals.End(); replacedIter != end; replacedIter++)
  {
    (*replacedIter)->pending = false;
  }
}

// Called by a Visual when it's resource is ready
void ViewDataImpl::VisualData::ResourceReady(Visual::Base& object)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "ViewDataImpl::VisualData::ResourceReady() replacements pending[%d]\n",
                mRemoveVisuals.Count());

  RegisteredVisualContainer::Iterator registeredIter;

  // A resource is ready, find resource in the registered visuals container and get its index
  if(!FindVisual(object, mVisuals, registeredIter))
  {
    return;
  }

  RegisteredVisualContainer::Iterator visualToRemoveIter;
  // Find visual with the same index in the removal container
  // Set if off stage as it's replacement is now ready.
  // Remove if from removal list as now removed from stage.
  // Set Pending flag on the ready visual to false as now ready.
  if(FindVisual((*registeredIter)->index, mRemoveVisuals, visualToRemoveIter))
  {
    (*registeredIter)->pending = false;
    if(!((*visualToRemoveIter)->overrideReadyTransition))
    {
      SetVisualOffScene(Ui::GetImplementation((*visualToRemoveIter)->visual), mOuter.mViewImpl);
    }

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(visualToRemoveIter, mRemoveVisuals);
  }

  // A visual is ready so view may need relayouting if staged
  RelayoutRequest(object);

  // Called by a Visual when it's resource is ready
  if(((*registeredIter)->enabled))
  {
    mOuter.ResourceReady();
  }
}

void ViewDataImpl::VisualData::NotifyVisualEvent(Visual::Base& object, Property::Index signalId)
{
  for(auto registeredIter = mVisuals.Begin(), end = mVisuals.End(); registeredIter != end; ++registeredIter)
  {
    Internal::Visual::Base& registeredVisualImpl = Ui::GetImplementation((*registeredIter)->visual);
    if(&object == &registeredVisualImpl)
    {
      Dali::Ui::View handle(mOuter.mViewImpl.GetOwner());
      mVisualEventSignal.Emit(handle, (*registeredIter)->index, signalId);
      break;
    }
  }
}

void ViewDataImpl::VisualData::RelayoutRequest(Visual::Base& object)
{
  if(mOuter.mViewImpl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    mOuter.mViewImpl.RelayoutRequestToView();
  }
}

// Called by a Visual
bool ViewDataImpl::VisualData::IsAnyPropertyAnimate(const std::unordered_set<Property::Index>& properties) const
{
  for(const auto& index : properties)
  {
    if(mPropertyOnAnimation.find(index) != mPropertyOnAnimation.end())
    {
      return true;
    }
  }
  return false;
}

bool ViewDataImpl::VisualData::IsResourceReady() const
{
  // Iterate through and check all the enabled visuals are ready
  for(auto visualIter = mVisuals.Begin(); visualIter != mVisuals.End(); ++visualIter)
  {
    const Ui::Visual::Base        visual     = (*visualIter)->visual;
    const Internal::Visual::Base& visualImpl = Ui::GetImplementation(visual);

    // one of the enabled visuals is not ready
    if(!visualImpl.IsResourceReady() && (*visualIter)->enabled)
    {
      return false;
    }
  }
  return true;
}

Ui::Visual::ResourceStatus ViewDataImpl::VisualData::GetVisualResourceStatus(Property::Index index) const
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    if((*iter)->visual)
    {
      const Internal::Visual::Base& visualImpl = Ui::GetImplementation((*iter)->visual);
      return visualImpl.GetResourceStatus();
    }
  }

  return Ui::Visual::ResourceStatus::PREPARING;
}

void ViewDataImpl::VisualData::CopyInstancedProperties(RegisteredVisualContainer& visuals,
                                                       Dictionary<Property::Map>& instancedProperties)
{
  for(RegisteredVisualContainer::Iterator iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    if((*iter)->visual)
    {
      Property::Map instanceMap;
      Ui::GetImplementation((*iter)->visual).CreateInstancePropertyMap(instanceMap);
      instancedProperties.Add((*iter)->visual.GetName(), instanceMap);
    }
  }
}

void ViewDataImpl::VisualData::RegisterVisual(Property::Index index, Ui::Visual::Base& visual)
{
  RegisterVisual(index, visual, VisualState::ENABLED, DepthIndexValue::NOT_SET);
}

void ViewDataImpl::VisualData::RegisterVisual(Property::Index index, Ui::Visual::Base& visual, int depthIndex)
{
  RegisterVisual(index, visual, VisualState::ENABLED, DepthIndexValue::SET, depthIndex);
}

void ViewDataImpl::VisualData::RegisterVisual(Property::Index index, Ui::Visual::Base& visual, bool enabled)
{
  RegisterVisual(index, visual, (enabled ? VisualState::ENABLED : VisualState::DISABLED), DepthIndexValue::NOT_SET);
}

void ViewDataImpl::VisualData::RegisterVisual(Property::Index index, Ui::Visual::Base& visual, bool enabled,
                                              int depthIndex)
{
  RegisterVisual(index, visual, (enabled ? VisualState::ENABLED : VisualState::DISABLED), DepthIndexValue::SET,
                 depthIndex);
}

void ViewDataImpl::VisualData::RegisterVisual(Property::Index index, Ui::Visual::Base& visual,
                                              VisualState::Type enabled, DepthIndexValue::Type depthIndexValueSet,
                                              int depthIndex)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  DALI_LOG_INFO(gLogFilter, Debug::Concise, "RegisterVisual:%d \n", index);

  bool  visualReplaced(false);
  Actor self = mOuter.mViewImpl.Self();

  // Set the depth index, if not set by caller this will be either the current visual depth, max depth of all visuals
  // or zero.
  int requiredDepthIndex = visual.GetDepthIndex();

  if(depthIndexValueSet == DepthIndexValue::SET)
  {
    requiredDepthIndex = depthIndex;
  }

  // Change the depth index value automatically if the visual has DepthIndex to AUTO_INDEX
  // or if RegisterVisual set DepthIndex to AUTO_INDEX.
  const bool requiredDepthIndexChanged = (requiredDepthIndex == DepthIndex::AUTO_INDEX);

  // Visual replacement, existing visual should only be removed from stage when replacement ready.
  if(!mVisuals.Empty())
  {
    RegisteredVisualContainer::Iterator registeredVisualsiter;
    // Check if visual (index) is already registered, this is the current visual.
    if(FindVisual(index, mVisuals, registeredVisualsiter))
    {
      Ui::Visual::Base& currentRegisteredVisual = (*registeredVisualsiter)->visual;
      if(currentRegisteredVisual)
      {
        // Store current visual depth index as may need to set the replacement visual to same depth
        const int currentDepthIndex = (*registeredVisualsiter)->visual.GetDepthIndex();

        // No longer required to know if the replaced visual's resources are ready
        StopObservingVisual(currentRegisteredVisual);

        // If view staged and visual enabled then visuals will be swapped once ready
        if(self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) && enabled)
        {
          // Check if visual is currently in the process of being replaced ( is in removal container )
          RegisteredVisualContainer::Iterator visualQueuedForRemoval;
          if(FindVisual(index, mRemoveVisuals, visualQueuedForRemoval))
          {
            // Visual with same index is already in removal container so current visual pending
            // Only the the last requested visual will be displayed so remove current visual which is staged but not
            // ready.
            SetVisualOffScene(Ui::GetImplementation(currentRegisteredVisual), mOuter.mViewImpl);

            mVisuals.Erase(registeredVisualsiter);
          }
          else
          {
            // current visual not already in removal container so add now.
            DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                          "RegisterVisual Move current registered visual to removal Queue: %d \n", index);
            MoveVisual(registeredVisualsiter, mVisuals, mRemoveVisuals);
          }
        }
        else
        {
          // View not staged or visual disabled so can just erase from registered visuals and new visual will be
          // added later.
          mVisuals.Erase(registeredVisualsiter);
        }

        // If the visual have a depth index as AUTO_INDEX and the new visual does not have a depth index applied to it,
        // then use the previously set depth-index for this index
        if(requiredDepthIndexChanged)
        {
          requiredDepthIndex = currentDepthIndex;
          DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                        "Use replaced visual index. VisualDepthIndex AUTO_INDEX set as: %d\n", requiredDepthIndex);
        }
      }

      visualReplaced = true;
    }
  }

  // If not set, set the name of the visual to the same name as the view's property.
  // ( If the view has been type registered )
  if(visual.GetName().empty())
  {
    // returns empty string if index is not found as long as index is not -1
    std::string visualName = ToStdString(self.GetPropertyName(index));
    if(!visualName.empty())
    {
      DALI_LOG_INFO(gLogFilter, Debug::Concise, "Setting visual name for property %d to %s\n", index,
                    visualName.c_str());
      visual.SetName(visualName);
    }
  }

  if(!visualReplaced) // New registration entry
  {
    // If we have more than one visual and the visual have a depth index as AUTO_INDEX, then set it to be the highest
    if((mVisuals.Size() > 0) && requiredDepthIndexChanged)
    {
      int maxDepthIndex =
        static_cast<int>(DepthIndex::CONTENT) - 1; // Start at DepthIndex::CONTENT if maxDepth index belongs to a
                                                   // background or no visuals have been added yet.

      RegisteredVisualContainer::ConstIterator       iter;
      const RegisteredVisualContainer::ConstIterator endIter = mVisuals.End();
      for(iter = mVisuals.Begin(); iter != endIter; iter++)
      {
        const int visualDepthIndex = (*iter)->visual.GetDepthIndex();
        if(visualDepthIndex > maxDepthIndex)
        {
          maxDepthIndex = visualDepthIndex;
        }
      }
      requiredDepthIndex =
        ++maxDepthIndex; // Add one to the current maximum depth index so that our added visual appears on top.
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Use top of all visuals. VisualDepthIndex AUTO_INDEX set as: %d\n",
                    requiredDepthIndex);
    }
  }

  if(visual)
  {
    // If required depth index still DepthIndex::AUTO_INDEX, Make it as DepthIndex::CONTENT now
    if(requiredDepthIndex == static_cast<int>(DepthIndex::AUTO_INDEX))
    {
      requiredDepthIndex = static_cast<int>(DepthIndex::CONTENT);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Some strange cases. VisualDepthIndex AUTO_INDEX set as: %d\n",
                    requiredDepthIndex);
    }

    // Set determined depth index
    visual.SetDepthIndex(requiredDepthIndex);

    // Monitor when the visual resources are ready
    StartObservingVisual(visual);

    DALI_LOG_INFO(gLogFilter, Debug::Concise, "New Visual registration index[%d] depth[%d]\n", index,
                  requiredDepthIndex);
    RegisteredVisual* newRegisteredVisual = new RegisteredVisual(
      index, visual, (enabled == VisualState::ENABLED ? true : false), (visualReplaced && enabled));
    mVisuals.PushBack(newRegisteredVisual);

    Internal::Visual::Base& visualImpl = Ui::GetImplementation(visual);
    // Put on stage if enabled and the view is already on the stage
    if((enabled == VisualState::ENABLED) && self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
    {
      SetVisualOnScene(visualImpl, mOuter.mViewImpl);
    }
    else if(enabled &&
            visualImpl.IsResourceReady()) // When not being staged, check if visual already 'ResourceReady' before it
                                          // was Registered. ( Resource may have been loaded already )
    {
      ResourceReady(visualImpl);
    }
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::RegisterVisual() Registered %s(%d), enabled:%s\n",
                visual.GetName().c_str(), index, enabled ? "true" : "false");
}

void ViewDataImpl::VisualData::UnregisterVisual(Property::Index index)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    // stop observing visual
    StopObservingVisual((*iter)->visual);

    SetVisualOffScene(Ui::GetImplementation((*iter)->visual), mOuter.mViewImpl);

    (*iter)->visual.Reset();
    mVisuals.Erase(iter);
  }

  if(FindVisual(index, mRemoveVisuals, iter))
  {
    SetVisualOffScene(Ui::GetImplementation((*iter)->visual), mOuter.mViewImpl);

    (*iter)->pending = false;

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(iter, mRemoveVisuals);
  }
}

Ui::Visual::Base ViewDataImpl::VisualData::GetVisual(Property::Index index) const
{
  return Ui::Visual::Base(GetVisualImplPtr(index));
}

Ui::Internal::Visual::Base* ViewDataImpl::VisualData::GetVisualImplPtr(Property::Index index) const
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    return (*iter)->visual ? &Ui::GetImplementation((*iter)->visual) : nullptr;
  }
  return nullptr;
}

Ui::Visual::Base ViewDataImpl::VisualData::GetVisual(const std::string& name) const
{
  return GetVisualByName(mVisuals, name);
}

void ViewDataImpl::VisualData::EnableVisual(Property::Index index, bool enable)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "View::EnableVisual(%d, %s)\n", index, enable ? "T" : "F");

  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    if((*iter)->enabled == enable)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::EnableVisual Visual %s(%d) already %s\n",
                    (*iter)->visual.GetName().c_str(), index, enable ? "enabled" : "disabled");
      return;
    }

    (*iter)->enabled = enable;
    if(mOuter.mViewImpl.Self().GetProperty<bool>(
         Actor::Property::CONNECTED_TO_SCENE)) // If view not on Scene then Visual will be added when
                                               // SceneConnection is called.
    {
      if(enable)
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::EnableVisual Setting %s(%d) on stage \n",
                      (*iter)->visual.GetName().c_str(), index);
        SetVisualOnScene(Ui::GetImplementation((*iter)->visual), mOuter.mViewImpl);
      }
      else
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::EnableVisual Setting %s(%d) off stage \n",
                      (*iter)->visual.GetName().c_str(), index);

        SetVisualOffScene(Ui::GetImplementation((*iter)->visual),
                          mOuter.mViewImpl); // No need to call if view not staged.
      }
    }
  }
  else
  {
    DALI_LOG_ERROR("View::EnableVisual(%d, %s) FAILED - NO SUCH VISUAL\n", index, enable ? "T" : "F");
  }
}

void ViewDataImpl::VisualData::EnableReadyTransitionOverridden(Ui::Visual::Base& visual, bool enable)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "View::EnableReadyTransitionOverridden(%p, %s)\n", &visual,
                enable ? "T" : "F");

  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(visual, mVisuals, iter))
  {
    if((*iter)->overrideReadyTransition == enable)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::EnableReadyTransitionOverridden Visual %s(%p) already %s\n",
                    (*iter)->visual.GetName().c_str(), &visual, enable ? "enabled" : "disabled");
      return;
    }

    (*iter)->overrideReadyTransition = enable;
  }
}

void ViewDataImpl::VisualData::EnableCornerPropertiesOverridden(Ui::Visual::Base& visual, bool enable,
                                                                Dali::Constraint cornerRadiusConstraint)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "View::EnableCornerPropertiesOverridden(%p, %s)\n", &visual,
                enable ? "T" : "F");

  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(visual, mVisuals, iter))
  {
    if((*iter)->overrideCornerProperties == enable)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::EnableCornerPropertiesOverridden Visual %s(%p) already %s\n",
                    (*iter)->visual.GetName().c_str(), &visual, enable ? "enabled" : "disabled");
      return;
    }

    (*iter)->overrideCornerProperties = enable;

    if(enable)
    {
      auto self = mOuter.mViewImpl.Self();

      Ui::Internal::Visual::Base& visualImpl = Ui::GetImplementation(visual);

      if(mCornerRadiusValueAdded || mCornerSquarenessValueAdded)
      {
        // TODO This condition is to cover utc failtures. Remove this after updating them.
        // e.g Setting view's corner radius and then setting background visual: Changing visual's corner radius
        // crashes utc.
        const Vector4 cornerRadius = self.GetProperty<Vector4>(Ui::View::Property::CORNER_RADIUS);

        Property::Map map;
        // Use corner radius ZERO when offscreen rendering with capture is enabled to avoid issues with anti-aliasing.
        map.Insert(Ui::DevelVisual::Property::CORNER_RADIUS,
                   (mOffscreenRenderingEnabled && visualImpl.IsCornerRadiusIgnoredAtOffscreenRendering())
                     ? Vector4::ZERO
                     : cornerRadius);
        map.Insert(Ui::DevelVisual::Property::CORNER_RADIUS_POLICY,
                   self.GetProperty<int>(Ui::View::Property::CORNER_RADIUS_POLICY));
        map.Insert(Ui::DevelVisual::Property::CORNER_SQUARENESS,
                   self.GetProperty<Vector4>(Ui::View::Property::CORNER_SQUARENESS));

        visual.DoAction(Ui::DevelVisual::Action::UPDATE_PROPERTY, map);
      }

      auto visualCornerRadiusProperty = visualImpl.GetPropertyObject(DevelVisual::Property::CORNER_RADIUS, false);
      auto visualCornerSquarenessProperty =
        visualImpl.GetPropertyObject(DevelVisual::Property::CORNER_SQUARENESS, false);
      if(DALI_LIKELY(visualCornerRadiusProperty.propertyIndex != Property::INVALID_INDEX &&
                     visualCornerRadiusProperty.object) &&
         DALI_LIKELY(visualCornerSquarenessProperty.propertyIndex != Property::INVALID_INDEX &&
                     visualCornerSquarenessProperty.object))
      {
        if(cornerRadiusConstraint)
        {
          DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                        "View::EnableCornerPropertiesOverridden Visual %s(%p) use own corner radius constraint\n",
                        (*iter)->visual.GetName().c_str(), &visual);
          std::unordered_set<Property::Index> relativeProperties;

          const auto sourceCount = cornerRadiusConstraint.GetSourceCount();
          for(uint32_t i = 0; i < sourceCount; ++i)
          {
            auto source = cornerRadiusConstraint.GetSourceAt(i);
            if(source.sourceType == Dali::SourceType::OBJECT_PROPERTY)
            {
              relativeProperties.insert(source.propertyIndex);

              // Special case for Actor::Property::SIZE.
              if(source.propertyIndex == Actor::Property::SIZE)
              {
                relativeProperties.insert(Actor::Property::SIZE_WIDTH);
                relativeProperties.insert(Actor::Property::SIZE_HEIGHT);
              }
            }
          }
          visualImpl.AddConstraintFeature(cornerRadiusConstraint, std::move(relativeProperties));
        }
        else
        {
          DALI_LOG_INFO(
            gLogFilter, Debug::Verbose,
            "View::EnableCornerPropertiesOverridden Visual %s(%p) use default equal corner radius constraint\n",
            (*iter)->visual.GetName().c_str(), &visual);
          cornerRadiusConstraint = Constraint::New<Vector4>(
            visualCornerRadiusProperty.object, visualCornerRadiusProperty.propertyIndex, EqualToConstraint());
          cornerRadiusConstraint.AddSource(Source(self, Ui::View::Property::CORNER_RADIUS));
          Dali::Integration::ConstraintSetInternalTag(cornerRadiusConstraint, DEFAULT_CORNER_RADIUS_CONSTRAINT_TAG);
          visualImpl.AddConstraintFeature(cornerRadiusConstraint, {Ui::View::Property::CORNER_RADIUS});
        }
        if(mCornerRadiusValueAdded &&
           !(mOffscreenRenderingEnabled && visualImpl.IsCornerRadiusIgnoredAtOffscreenRendering()))
        {
          cornerRadiusConstraint.Apply();
        }

        auto cornerSquarenessEqualConstraint = Constraint::New<Vector4>(
          visualCornerSquarenessProperty.object, visualCornerSquarenessProperty.propertyIndex, EqualToConstraint());
        cornerSquarenessEqualConstraint.AddSource(Source(self, Ui::View::Property::CORNER_SQUARENESS));
        Dali::Integration::ConstraintSetInternalTag(cornerSquarenessEqualConstraint,
                                                    DEFAULT_CORNER_SQUARENESS_CONSTRAINT_TAG);
        visualImpl.AddConstraintFeature(cornerSquarenessEqualConstraint, {Ui::View::Property::CORNER_SQUARENESS});
        if(mCornerSquarenessValueAdded)
        {
          cornerSquarenessEqualConstraint.Apply();
        }
      }
    }
    else
    {
      Ui::Internal::Visual::Base& visualImpl = Ui::GetImplementation(visual);
      visualImpl.RemoveConstraintFeatureByIndex(Ui::View::Property::CORNER_RADIUS);
      visualImpl.RemoveConstraintFeatureByIndex(Ui::View::Property::CORNER_SQUARENESS);
    }
  }
}

bool ViewDataImpl::VisualData::IsVisualEnabled(Property::Index index) const
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    return (*iter)->enabled;
  }
  return false;
}

void ViewDataImpl::VisualData::RemoveVisual(RegisteredVisualContainer& visuals, const std::string& visualName)
{
  for(RegisteredVisualContainer::Iterator visualIter = visuals.Begin(); visualIter != visuals.End(); ++visualIter)
  {
    Ui::Visual::Base visual = (*visualIter)->visual;
    if(visual && visual.GetName() == visualName)
    {
      SetVisualOffScene(Ui::GetImplementation(visual), mOuter.mViewImpl);

      (*visualIter)->visual.Reset();
      visuals.Erase(visualIter);
      break;
    }
  }
}

void ViewDataImpl::VisualData::RemoveVisuals(RegisteredVisualContainer& visuals, DictionaryKeys& removeVisuals)
{
  for(DictionaryKeys::iterator iter = removeVisuals.begin(); iter != removeVisuals.end(); ++iter)
  {
    const std::string visualName = *iter;
    RemoveVisual(visuals, visualName);
  }
}

void ViewDataImpl::VisualData::RecreateChangedVisuals(Dictionary<Property::Map>& stateVisualsToChange,
                                                      Dictionary<Property::Map>& instancedProperties)
{
  Dali::CustomActor handle(mOuter.mViewImpl.GetOwner());
  for(Dictionary<Property::Map>::iterator iter = stateVisualsToChange.Begin(); iter != stateVisualsToChange.End();
      ++iter)
  {
    const std::string&   visualName = (*iter).key;
    const Property::Map& toMap      = (*iter).entry;

    RegisteredVisualContainer::Iterator registeredVisualsiter;
    // Check if visual (visualName) is already registered, this is the current visual.
    if(FindVisual(visualName, mVisuals, registeredVisualsiter))
    {
      Ui::Visual::Base& visual = (*registeredVisualsiter)->visual;
      if(visual)
      {
        // No longer required to know if the replaced visual's resources are ready
        StopObservingVisual(visual);

        // If view staged then visuals will be swapped once ready
        Actor self = mOuter.mViewImpl.Self();
        if(self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
        {
          // Check if visual is currently in the process of being replaced ( is in removal container )
          RegisteredVisualContainer::Iterator visualQueuedForRemoval;
          if(FindVisual(visualName, mRemoveVisuals, visualQueuedForRemoval))
          {
            // Visual with same visual name is already in removal container so current visual pending
            // Only the the last requested visual will be displayed so remove current visual which is staged but not
            // ready.
            SetVisualOffScene(Ui::GetImplementation(visual), mOuter.mViewImpl);

            (*registeredVisualsiter)->visual.Reset();
            mVisuals.Erase(registeredVisualsiter);
          }
          else
          {
            // current visual not already in removal container so add now.
            DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                          "RegisterVisual Move current registered visual to removal Queue: %s \n", visualName.c_str());
            MoveVisual(registeredVisualsiter, mVisuals, mRemoveVisuals);
          }
        }
        else
        {
          // View not staged or visual disabled so can just erase from registered visuals and new visual will be
          // added later.
          (*registeredVisualsiter)->visual.Reset();
          mVisuals.Erase(registeredVisualsiter);
        }
      }

      const Property::Map* instancedMap = instancedProperties.FindConst(visualName);
      Style::ApplyVisual(handle, visualName, toMap, instancedMap);
    }
  }
}

void ViewDataImpl::VisualData::ReplaceStateVisualsAndProperties(const StylePtr oldState, const StylePtr newState,
                                                                const std::string& subState)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  // Collect all old visual names
  DictionaryKeys stateVisualsToRemove;
  if(oldState)
  {
    oldState->visuals.GetKeys(stateVisualsToRemove);
    if(!subState.empty())
    {
      const StylePtr* oldSubState = oldState->subStates.FindConst(subState);
      if(oldSubState)
      {
        DictionaryKeys subStateVisualsToRemove;
        (*oldSubState)->visuals.GetKeys(subStateVisualsToRemove);
        Merge(stateVisualsToRemove, subStateVisualsToRemove);
      }
    }
  }

  // Collect all new visual properties
  Dictionary<Property::Map> stateVisualsToAdd;
  if(newState)
  {
    stateVisualsToAdd = newState->visuals;
    if(!subState.empty())
    {
      const StylePtr* newSubState = newState->subStates.FindConst(subState);
      if(newSubState)
      {
        stateVisualsToAdd.Merge((*newSubState)->visuals);
      }
    }
  }

  // If a name is in both add/remove, move it to change list.
  Dictionary<Property::Map> stateVisualsToChange;
  FindChangeableVisuals(stateVisualsToAdd, stateVisualsToChange, stateVisualsToRemove);

  // Copy instanced properties (e.g. text label) of current visuals
  Dictionary<Property::Map> instancedProperties;
  CopyInstancedProperties(mVisuals, instancedProperties);

  // For each visual in remove list, remove from mVisuals
  RemoveVisuals(mVisuals, stateVisualsToRemove);

  // For each visual in add list, create and add to mVisuals
  Dali::CustomActor handle(mOuter.mViewImpl.GetOwner());
  Style::ApplyVisuals(handle, stateVisualsToAdd, instancedProperties);

  // For each visual in change list, if it requires a new visual,
  // remove old visual, create and add to mVisuals
  RecreateChangedVisuals(stateVisualsToChange, instancedProperties);
}

Ui::View::VisualEventSignalType& ViewDataImpl::VisualData::VisualEventSignal()
{
  return mVisualEventSignal;
}

void ViewDataImpl::VisualData::DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                                        const Dali::Property::Value& attributes)
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(visualIndex, mVisuals, iter))
  {
    Ui::GetImplementation((*iter)->visual).DoAction(actionId, attributes);
  }
}

void ViewDataImpl::VisualData::DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                                                 const Dali::Any& attributes)
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(visualIndex, mVisuals, iter))
  {
    Ui::GetImplementation((*iter)->visual).DoActionExtension(actionId, attributes);
  }
}

void ViewDataImpl::VisualData::ClearVisuals()
{
  while(!mVisuals.Empty())
  {
    auto iter = mVisuals.End() - 1u;
    StopObservingVisual((*iter)->visual);

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(iter, mVisuals);
  }

  while(!mRemoveVisuals.Empty())
  {
    auto removalIter = mRemoveVisuals.End() - 1u;
    StopObservingVisual((*removalIter)->visual);

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(removalIter, mRemoveVisuals);
  }
}

Dali::Property ViewDataImpl::VisualData::GetVisualProperty(Dali::Property::Index index,
                                                           Dali::Property::Key   visualPropertyKey)
{
  Ui::Visual::Base visual = GetVisualByIndex(mVisuals, index);
  if(visual)
  {
    return visual.GetPropertyObject(std::move(visualPropertyKey));
  }

  Handle handle;
  return Dali::Property(handle, Property::INVALID_INDEX);
}

void ViewDataImpl::VisualData::StopObservingVisual(Ui::Visual::Base& visual)
{
  Internal::Visual::Base& visualImpl = Ui::GetImplementation(visual);

  // Stop observing the visual
  visualImpl.RemoveEventObserver(*this);
  visualImpl.RemoveConstraintObserver(*this);
}

void ViewDataImpl::VisualData::StartObservingVisual(Ui::Visual::Base& visual)
{
  Internal::Visual::Base& visualImpl = Ui::GetImplementation(visual);

  // start observing the visual for events
  visualImpl.AddEventObserver(*this);
  visualImpl.AddConstraintObserver(*this);
}

void ViewDataImpl::VisualData::CreateAnimationConstraints(const Dali::BaseObject& animationObject,
                                                          Property::Index         index)
{
  if(index == Ui::View::Property::CORNER_RADIUS || index == Ui::View::Property::CORNER_SQUARENESS ||
     index == Ui::View::Property::BORDERLINE_WIDTH || index == Ui::View::Property::BORDERLINE_COLOR ||
     index == Ui::View::Property::BORDERLINE_OFFSET || index == Actor::Property::SIZE ||
     index == Actor::Property::SIZE_WIDTH || index == Actor::Property::SIZE_HEIGHT)
  {
    bool notifyConstraints = false;
    if(mPropertyOnAnimation.find(index) == mPropertyOnAnimation.end())
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::CreateAnimationConstraints property animated from now [%d]\n",
                    index);
      notifyConstraints = true;
    }

    // Get or create counter
    auto& animationCounter = mPropertyOnAnimation[index];

    const auto* animationObjectPtr = &static_cast<const Dali::RefObject&>(animationObject);

    auto iter = animationCounter.find(animationObjectPtr);
    if(iter == animationCounter.end())
    {
      animationCounter.insert({animationObjectPtr, 1});
    }
    else
    {
      ++(iter->second);
    }

    if(notifyConstraints)
    {
      NotifyConstraintPropertyChanged(index, true);
    }
  }
}

void ViewDataImpl::VisualData::ClearAnimationConstraints(const Dali::BaseObject& animationObject,
                                                         Property::Index         index)
{
  auto indexIter = mPropertyOnAnimation.find(index);
  if(indexIter != mPropertyOnAnimation.end())
  {
    auto& animationCounter = indexIter->second;

    const auto* animationObjectPtr = &static_cast<const Dali::RefObject&>(animationObject);

    auto iter = animationCounter.find(animationObjectPtr);
    if(DALI_LIKELY(iter != animationCounter.end()))
    {
      if(iter->second == 0 || --(iter->second) == 0)
      {
        animationCounter.erase(iter);
        if(animationCounter.empty())
        {
          DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                        "View::ClearAnimationConstraints property not animated anymore [%d]\n", index);

          mPropertyOnAnimation.erase(index);
          NotifyConstraintPropertyChanged(index, false);
        }
      }
    }
  }
}

void ViewDataImpl::VisualData::NotifyConstraintPropertyChanged(Property::Index index, bool notifyFromAnimation)
{
  for(auto registeredVisual : mVisuals)
  {
    if(registeredVisual->visual && registeredVisual->enabled)
    {
      // TODO : Might need to use some flags
      Internal::Visual::Base& visualImpl = Ui::GetImplementation(registeredVisual->visual);

      DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                    "View::NotifyConstraintPropertyChanged(%d) Visual %s(%p) notify property changed [%d]\n",
                    notifyFromAnimation, registeredVisual->visual.GetName().c_str(), &visualImpl, index);
      visualImpl.UpdateApplyRate(index);
    }
  }

  switch(index)
  {
    case Ui::View::Property::CORNER_RADIUS:
    {
      const Vector4 cornerRadius =
        mOuter.mViewImpl.Self().GetProperty<Vector4>(Ui::View::Property::CORNER_RADIUS);

      DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                    "View::NotifyConstraintPropertyChanged(%d) set CornerRadius Value: %f, %f, %f, %f\n",
                    notifyFromAnimation, cornerRadius.x, cornerRadius.y, cornerRadius.z, cornerRadius.w);

      for(auto registeredVisual : mVisuals)
      {
        if(registeredVisual->overrideCornerProperties)
        {
          auto& visualImpl = Ui::GetImplementation(registeredVisual->visual);
          if(mOffscreenRenderingEnabled && visualImpl.IsCornerRadiusIgnoredAtOffscreenRendering())
          {
            // Skip offscreen captured visuals start constraints.
            continue;
          }

          // Ensure to add uniforms
          if(notifyFromAnimation)
          {
            // Need to make ensure that we are using corner radius now.
            // It will change shader if need.
            [[maybe_unused]] auto visualCornerRadiusProperty =
              visualImpl.GetPropertyObject(DevelVisual::Property::CORNER_RADIUS, true);
          }
          visualImpl.DoAction(Ui::DevelVisual::Action::UPDATE_PROPERTY,
                              Property::Map().Add(Ui::DevelVisual::Property::CORNER_RADIUS, cornerRadius));

          // Apply corner radius to other visuals
          if(DALI_UNLIKELY(!mCornerRadiusValueAdded))
          {
            visualImpl.DoAction(
              Ui::DevelVisual::Action::UPDATE_PROPERTY,
              Property::Map()
                .Add(Ui::DevelVisual::Property::CORNER_RADIUS_POLICY,
                     mOuter.mViewImpl.Self().GetProperty<int>(Ui::View::Property::CORNER_RADIUS_POLICY))
                .Add(Ui::DevelVisual::Property::CORNER_SQUARENESS,
                     mOuter.mViewImpl.Self().GetProperty<Vector4>(Ui::View::Property::CORNER_SQUARENESS)));
            visualImpl.StartConstraintFeature(Ui::View::Property::CORNER_RADIUS);
          }
        }
      }

      if(DALI_UNLIKELY(!mCornerRadiusValueAdded))
      {
        // First time corner radius animated, or setted. Need to apply corner radius constraint to visuals
        mCornerRadiusValueAdded = true;
      }
      break;
    }
    case Ui::View::Property::CORNER_RADIUS_POLICY:
    {
      const int cornerRadiusPolicy =
        mOuter.mViewImpl.Self().GetProperty<int>(Ui::View::Property::CORNER_RADIUS_POLICY);

      DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                    "View::NotifyConstraintPropertyChanged(%d) set CornerRadiusPolicy Value: %d\n",
                    notifyFromAnimation, cornerRadiusPolicy);

      for(auto registeredVisual : mVisuals)
      {
        if(registeredVisual->overrideCornerProperties)
        {
          auto& visualImpl = Ui::GetImplementation(registeredVisual->visual);

          visualImpl.DoAction(Ui::DevelVisual::Action::UPDATE_PROPERTY,
                              Property::Map().Add(Ui::DevelVisual::Property::CORNER_RADIUS_POLICY, cornerRadiusPolicy));
        }
      }
      break;
    }
    case Ui::View::Property::CORNER_SQUARENESS:
    {
      Vector4 cornerSquareness =
        mOuter.mViewImpl.Self().GetProperty<Vector4>(Ui::View::Property::CORNER_SQUARENESS);

      DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                    "View::NotifyConstraintPropertyChanged(%d) set CornerSquareness Value: %f, %f, %f, %f\n",
                    notifyFromAnimation, cornerSquareness.x, cornerSquareness.y, cornerSquareness.z,
                    cornerSquareness.w);

      for(auto registeredVisual : mVisuals)
      {
        if(registeredVisual->overrideCornerProperties)
        {
          auto& visualImpl = Ui::GetImplementation(registeredVisual->visual);

          // Ensure to add uniforms
          if(notifyFromAnimation)
          {
            // Need to make ensure that we are using corner squreness now.
            // It will change shader if need.
            [[maybe_unused]] auto visualCornerSqurenessProperty =
              visualImpl.GetPropertyObject(DevelVisual::Property::CORNER_SQUARENESS, true);
          }
          visualImpl.DoAction(Ui::DevelVisual::Action::UPDATE_PROPERTY,
                              Property::Map().Add(Ui::DevelVisual::Property::CORNER_SQUARENESS, cornerSquareness));

          if(DALI_UNLIKELY(!mCornerSquarenessValueAdded))
          {
            // Apply corner squareness to other visuals
            visualImpl.DoAction(
              Ui::DevelVisual::Action::UPDATE_PROPERTY,
              Property::Map()
                .Add(Ui::DevelVisual::Property::CORNER_RADIUS,
                     mOuter.mViewImpl.Self().GetProperty<Vector4>(Ui::View::Property::CORNER_RADIUS))
                .Add(Ui::DevelVisual::Property::CORNER_RADIUS_POLICY,
                     mOuter.mViewImpl.Self().GetProperty<int>(Ui::View::Property::CORNER_RADIUS_POLICY)));
            visualImpl.StartConstraintFeature(Ui::View::Property::CORNER_SQUARENESS);
          }
        }
      }

      if(DALI_UNLIKELY(!mCornerSquarenessValueAdded))
      {
        // First time corner squareness animated, or setted. Need to apply corner squareness constraint to visuals
        mCornerSquarenessValueAdded = true;
      }
      break;
    }
  }
}

void ViewDataImpl::VisualData::OffscreenRenderingEnabled(bool enabled)
{
  if(mOffscreenRenderingEnabled == enabled)
  {
    return;
  }
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::OffscreenRenderingEnabled(%d)\n", enabled);

  if(DALI_UNLIKELY(!mCornerRadiusValueAdded))
  {
    DALI_LOG_INFO(
      gLogFilter, Debug::Verbose,
      "View::OffscreenRenderingEnabled First type to set View::CORNER_RADIUS by OffscreenRendering view\n");
  }

  mOffscreenRenderingEnabled = enabled;
  for(auto iter = mVisuals.begin(); iter != mVisuals.end(); ++iter)
  {
    if((*iter)->overrideCornerProperties)
    {
      auto& visualImpl = Ui::GetImplementation((*iter)->visual);
      if(visualImpl.IsOffscreenRenderingCaptureEnabled())
      {
        if(enabled && visualImpl.IsCornerRadiusIgnoredAtOffscreenRendering())
        {
          // Stop corner radius constraint if offscreen rendering is enabled
          // Use corner radius ZERO when offscreen rendering with capture is enabled to avoid issues with anti-aliasing.
          visualImpl.StopConstraintFeature(Ui::View::Property::CORNER_RADIUS);
          visualImpl.DoAction(Ui::DevelVisual::Action::UPDATE_PROPERTY,
                              Property::Map().Add(Ui::DevelVisual::Property::CORNER_RADIUS, Vector4::ZERO));
        }
        else
        {
          // Re-apply corner radius to other visuals
          visualImpl.DoAction(Ui::DevelVisual::Action::UPDATE_PROPERTY,
                              Property::Map().Add(Ui::DevelVisual::Property::CORNER_RADIUS,
                                                  mOuter.mViewImpl.Self().GetProperty<Vector4>(
                                                    Ui::View::Property::CORNER_RADIUS)));
          visualImpl.StartConstraintFeature(Ui::View::Property::CORNER_RADIUS);
        }
      }
      else
      {
        if(visualImpl.IsOnScene())
        {
          Dali::Ui::View handle(mOuter.mViewImpl.GetOwner());

          Renderer renderer = visualImpl.GetRenderer();
          if(enabled)
          {
            handle.RemoveRenderer(renderer);
            handle.AddCacheRenderer(renderer);
          }
          else
          {
            handle.RemoveCacheRenderer(renderer);
            handle.AddRenderer(renderer);
          }
        }
        if(DALI_UNLIKELY(!mCornerRadiusValueAdded))
        {
          // First time corner radius animated, or setted. Need to apply corner radius constraint to visuals
          visualImpl.DoAction(Ui::DevelVisual::Action::UPDATE_PROPERTY,
                              Property::Map().Add(Ui::DevelVisual::Property::CORNER_RADIUS,
                                                  mOuter.mViewImpl.Self().GetProperty<Vector4>(
                                                    Ui::View::Property::CORNER_RADIUS)));
          visualImpl.StartConstraintFeature(Ui::View::Property::CORNER_RADIUS);
        }
      }
    }
  }
  // Do not re-start constraint features for corner radius.
  mCornerRadiusValueAdded = true;
}

void ViewDataImpl::VisualData::ApplyFittingMode(const Vector2& size)
{
  Actor self;
  for(RegisteredVisualContainer::Iterator iter = mVisuals.Begin(); iter != mVisuals.End(); iter++)
  {
    // Check whether the visual is empty and enabled
    if((*iter)->visual && (*iter)->enabled)
    {
      Internal::Visual::Base& visualImpl = Ui::GetImplementation((*iter)->visual);

      // If the current visual is using the transform property map, fittingMode will not be applied.
      if(visualImpl.IsIgnoreFittingMode())
      {
        continue;
      }

      Visual::FittingMode fittingMode = visualImpl.GetFittingMode();

      // If the fittingMode is DONT_CARE, we don't need to apply fittingMode, just Set the size of view
      if(fittingMode == Visual::FittingMode::DONT_CARE)
      {
        if(visualImpl.GetType() != Ui::Visual::Type::TEXT)
        {
          visualImpl.SetViewSize(size);
        }
        continue;
      }

      if(!self)
      {
        self = mOuter.mViewImpl.Self();
      }

      Extents padding = self.GetProperty<Extents>(Ui::View::Property::PADDING);

      bool zeroPadding = (padding == Extents());

      Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(
        self.GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
      if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
      {
        std::swap(padding.start, padding.end);
      }

      // remove padding from the size to know how much is left for the visual
      Vector2 finalSize   = size - Vector2(padding.start + padding.end, padding.top + padding.bottom);
      Vector2 finalOffset = Vector2(padding.start, padding.top);

      // Reset PIXEL_AREA after using OVER_FIT_KEEP_ASPECT_RATIO
      if(visualImpl.IsPixelAreaSetForFittingMode())
      {
        visualImpl.SetPixelAreaForFittingMode(FULL_TEXTURE_RECT);
      }

      Property::Map transformMap = Property::Map();

      if((!zeroPadding) || // If padding is not zero
         (fittingMode != Visual::FittingMode::FILL))
      {
        visualImpl.SetTransformMapUsageForFittingMode(true);

        Vector2 naturalSize;
        // NaturalSize will not be used for FILL fitting mode, which is default.
        // Skip GetNaturalSize
        if(fittingMode != Visual::FittingMode::FILL)
        {
          visualImpl.GetNaturalSize(naturalSize);
        }

        // If FittingMode use FIT_WIDTH or FIT_HEIGHT, it need to change proper fittingMode
        if(fittingMode == Visual::FittingMode::FIT_WIDTH || fittingMode == Visual::FittingMode::FIT_HEIGHT)
        {
          const float widthRatio = !Dali::EqualsZero(naturalSize.width) ? (finalSize.width / naturalSize.width) : 0.0f;
          const float heightRatio =
            !Dali::EqualsZero(naturalSize.height) ? (finalSize.height / naturalSize.height) : 0.0f;
          if(widthRatio < heightRatio)
          {
            // Final size has taller form than natural size.
            fittingMode = (fittingMode == Visual::FittingMode::FIT_WIDTH)
                            ? Visual::FittingMode::FIT_KEEP_ASPECT_RATIO
                            : Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO;
          }
          else
          {
            // Final size has wider form than natural size.
            fittingMode = (fittingMode == Visual::FittingMode::FIT_WIDTH)
                            ? Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO
                            : Visual::FittingMode::FIT_KEEP_ASPECT_RATIO;
          }
        }

        // Calculate size for fittingMode
        switch(fittingMode)
        {
          case Visual::FittingMode::FIT_KEEP_ASPECT_RATIO:
          {
            auto availableVisualSize = finalSize;

            // scale to fit the padded area
            finalSize =
              naturalSize *
              std::min(
                (!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0),
                (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0));

            // calculate final offset within the padded area
            finalOffset += (availableVisualSize - finalSize) * .5f;

            // populate the transform map
            transformMap.Add(Ui::Visual::Transform::Property::OFFSET, finalOffset)
              .Add(Ui::Visual::Transform::Property::SIZE, finalSize);
            break;
          }
          case Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO:
          {
            auto availableVisualSize = finalSize;
            finalSize =
              naturalSize *
              std::max(
                (!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));

            auto originalOffset = finalOffset;

            if(!visualImpl.IsPixelAreaSetForFittingMode() && !Dali::EqualsZero(finalSize.width) &&
               !Dali::EqualsZero(finalSize.height))
            {
              float   x           = abs((availableVisualSize.width - finalSize.width) / finalSize.width) * .5f;
              float   y           = abs((availableVisualSize.height - finalSize.height) / finalSize.height) * .5f;
              float   widthRatio  = 1.f - abs((availableVisualSize.width - finalSize.width) / finalSize.width);
              float   heightRatio = 1.f - abs((availableVisualSize.height - finalSize.height) / finalSize.height);
              Vector4 pixelArea   = Vector4(x, y, widthRatio, heightRatio);
              visualImpl.SetPixelAreaForFittingMode(pixelArea);
            }

            // populate the transform map
            transformMap.Add(Ui::Visual::Transform::Property::OFFSET, originalOffset)
              .Add(Ui::Visual::Transform::Property::SIZE, availableVisualSize);
            break;
          }
          case Visual::FittingMode::CENTER:
          {
            auto availableVisualSize = finalSize;
            if(availableVisualSize.width > naturalSize.width && availableVisualSize.height > naturalSize.height)
            {
              finalSize = naturalSize;
            }
            else
            {
              finalSize =
                naturalSize *
                std::min(
                  (!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                  (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height)
                                                         : 0.0f));
            }

            finalOffset += (availableVisualSize - finalSize) * .5f;

            // populate the transform map
            transformMap.Add(Ui::Visual::Transform::Property::OFFSET, finalOffset)
              .Add(Ui::Visual::Transform::Property::SIZE, finalSize);
            break;
          }
          case Visual::FittingMode::FILL:
          {
            transformMap.Add(Ui::Visual::Transform::Property::OFFSET, finalOffset)
              .Add(Ui::Visual::Transform::Property::SIZE, finalSize);
            break;
          }
          case Visual::FittingMode::FIT_WIDTH:
          case Visual::FittingMode::FIT_HEIGHT:
          case Visual::FittingMode::DONT_CARE:
          {
            // This FittingMode already converted
            break;
          }
        }

        // Set extra value for applying transformMap
        transformMap
          .Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
               Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
          .Add(Ui::Visual::Transform::Property::ORIGIN, Ui::Align::TOP_BEGIN)
          .Add(Ui::Visual::Transform::Property::ANCHOR_POINT, Ui::Align::TOP_BEGIN)
          .Add(Ui::Visual::Transform::Property::SIZE_POLICY,
               Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE));
      }
      else if(visualImpl.IsTransformMapSetForFittingMode() &&
              zeroPadding) // Reset offset to zero only if padding applied previously
      {
        visualImpl.SetTransformMapUsageForFittingMode(false);

        // Reset the transform map
        transformMap.Add(Ui::Visual::Transform::Property::OFFSET, Vector2::ZERO)
          .Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
               Vector2(Ui::Visual::Transform::Policy::RELATIVE, Ui::Visual::Transform::Policy::RELATIVE))
          .Add(Ui::Visual::Transform::Property::SIZE, Vector2::ONE)
          .Add(Ui::Visual::Transform::Property::SIZE_POLICY,
               Vector2(Ui::Visual::Transform::Policy::RELATIVE, Ui::Visual::Transform::Policy::RELATIVE));
      }

      visualImpl.SetTransformAndSize(transformMap, size);
    }
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
