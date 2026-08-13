#pragma once

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
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali/public-api/animation/animation.h>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/dialog/dialog-container.h>
#include <dali-ui-components/public-api/navigator/navigation-transition-spec.h>
#include <dali-ui-components/public-api/navigator/navigator.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Implementation class for Ui::Navigator.
 */
class DALI_UI_COMPONENTS_API NavigatorImpl : public ViewImpl
{
public:
  static Ui::Navigator New();

  // Navigation stack
  void     Push(Ui::View page, bool animated);
  Ui::View Pop(bool animated);
  void     InsertBefore(Ui::View page, Ui::View before);
  void     Remove(Ui::View page);
  void     Clear();

  // Modal stack
  void     PushModal(Ui::View modal, bool animated);
  Ui::View PopModal(bool animated);

  // Queries
  Ui::View GetCurrentView() const;
  uint32_t GetNavigationStackCount() const;
  uint32_t GetModalStackCount() const;
  Ui::View GetNavigationStackItem(uint32_t index) const;
  Ui::View GetModalStackItem(uint32_t index) const;

  // Back navigation
  bool NavigateBack();

  // Transition customization
  void SetPageTransitionAnimationEnabled(bool enabled);
  bool IsPageTransitionAnimationEnabled() const;
  void SetModalTransitionAnimationEnabled(bool enabled);
  bool IsModalTransitionAnimationEnabled() const;
  void SetTransitionSpec(NavigationTransitionSpec spec);
  void ClearTransitionSpec();
  void SetPageTransitionSpec(Ui::View page, NavigationTransitionSpec spec);
  void ClearPageTransitionSpec(Ui::View page);
  void SetModalTransitionSpec(NavigationTransitionSpec spec);
  void ClearModalTransitionSpec();
  void SetPageModalTransitionSpec(Ui::View page, NavigationTransitionSpec spec);
  void ClearPageModalTransitionSpec(Ui::View page);

  // Signals
  Ui::Navigator::BackRequestedSignalType& BackRequestedSignal()
  {
    return mBackRequestedSignal;
  }
  Ui::Navigator::PageEventSignalType& PageWillAppearSignal()
  {
    return mPageWillAppearSignal;
  }
  Ui::Navigator::PageEventSignalType& PageDidAppearSignal()
  {
    return mPageDidAppearSignal;
  }
  Ui::Navigator::PageEventSignalType& PageWillDisappearSignal()
  {
    return mPageWillDisappearSignal;
  }
  Ui::Navigator::PageEventSignalType& PageDidDisappearSignal()
  {
    return mPageDidDisappearSignal;
  }
  Ui::Navigator::TransitionFinishedSignalType& TransitionFinishedSignal()
  {
    return mTransitionFinishedSignal;
  }

protected:
  NavigatorImpl();
  virtual ~NavigatorImpl();

  void OnInitialize() override;

private:
  Ui::Navigator GetHandle();
  Ui::View      NavTop() const;
  Ui::View      ModalTop() const;
  void          AddChildFill(Ui::View view);
  void          UpdateVisibility();
  void          RestackModals();

  // Fade transition: animate one view's opacity, then finalize.
  void RunTransition(bool animated, bool fadeIncoming);
  void OnTransitionFinished(Dali::Animation animation);
  void FinishTransition();
  void AbortTransition();
  // Finalizes any in-flight transition immediately before starting a new one.
  void SettlePendingTransition();

  void OnScrimClicked(Ui::DialogContainer container);

  bool                     EmitBackRequested(Ui::Navigator navigator, Ui::View page);
  NavigationTransitionSpec LookupAnimatorSpec(Ui::View view, bool byPop, bool isIncoming, bool byModal);
  NavigationTransitionSpec LookupSnapSpec(Ui::View view, bool isIncoming, bool byModal);
  float                    ResolveTransitionDuration(Ui::View incoming, Ui::View outgoing) const;
  void                     SnapView(Ui::View view, bool isIncoming);
  void                     SetPageSpec(std::vector<std::pair<Ui::View, NavigationTransitionSpec>>& specs, Ui::View page, NavigationTransitionSpec spec);
  void                     ClearPageSpec(std::vector<std::pair<Ui::View, NavigationTransitionSpec>>& specs, Ui::View page);
  void                     RemovePageSpec(Ui::View page);

  static bool InStack(const std::vector<Ui::View>& stack, Ui::View view);
  // True if the view is already in either the navigation or the modal stack.
  bool InAnyStack(Ui::View view) const;

  NavigatorImpl(const NavigatorImpl&)            = delete;
  NavigatorImpl(NavigatorImpl&&)                 = delete;
  NavigatorImpl& operator=(const NavigatorImpl&) = delete;
  NavigatorImpl& operator=(NavigatorImpl&&)      = delete;

private:
  std::vector<Ui::View> mNavStack;
  std::vector<Ui::View> mModalStack;

  Ui::Navigator::BackRequestedSignalType      mBackRequestedSignal;
  Ui::Navigator::PageEventSignalType          mPageWillAppearSignal;
  Ui::Navigator::PageEventSignalType          mPageDidAppearSignal;
  Ui::Navigator::PageEventSignalType          mPageWillDisappearSignal;
  Ui::Navigator::PageEventSignalType          mPageDidDisappearSignal;
  Ui::Navigator::TransitionFinishedSignalType mTransitionFinishedSignal;

  // In-flight transition state.
  Dali::Animation mTransition;
  Ui::View        mTxIncoming;
  Ui::View        mTxOutgoing;
  bool            mTxByPop{false};
  bool            mTxRemoveOutgoing{false};
  bool            mTxByModal{false};
  bool            mInvokingTransitionCallback{false};
  bool            mPageTransitionAnimationEnabled{true};
  bool            mModalTransitionAnimationEnabled{true};

  // Specifications selected when the current transition starts. Keep them
  // until completion so animator and snap callbacks use one stable snapshot.
  NavigationTransitionSpec mTxIncomingSpec;
  NavigationTransitionSpec mTxOutgoingSpec;
  NavigationTransitionSpec mTxIncomingSnapSpec;
  NavigationTransitionSpec mTxOutgoingSnapSpec;

  NavigationTransitionSpec                                   mDefaultSpec;
  std::vector<std::pair<Ui::View, NavigationTransitionSpec>> mPageSpecs;
  NavigationTransitionSpec                                   mDefaultModalSpec;
  std::vector<std::pair<Ui::View, NavigationTransitionSpec>> mModalPageSpecs;
};

} // namespace Integration

inline Integration::NavigatorImpl& GetImpl(Ui::Navigator& navigator)
{
  DALI_ASSERT_ALWAYS(navigator);
  Dali::RefObject& handle = navigator.GetImplementation();
  return static_cast<Integration::NavigatorImpl&>(handle);
}

inline const Integration::NavigatorImpl& GetImpl(const Ui::Navigator& navigator)
{
  DALI_ASSERT_ALWAYS(navigator);
  const Dali::RefObject& handle = navigator.GetImplementation();
  return static_cast<const Integration::NavigatorImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
