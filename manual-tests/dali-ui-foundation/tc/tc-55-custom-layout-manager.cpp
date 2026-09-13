/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include "manual-test-case.h"

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

/**
 * Custom layout manager sample: diagonal arrangement implemented by
 * subclassing LayoutManager directly and attaching it to a plain View
 * via View::AttachLayoutManager.
 *
 * Three children (50x50, 100x100, 200x200) are placed diagonally so that
 * each child's top-left corner touches the bottom-right corner of the
 * previous child.
 *
 *  +--+
 *  |50|
 *  +--+----+
 *     |100 |
 *     |    |
 *     +----+--------+
 *          |  200   |
 *          |        |
 *          |        |
 *          +--------+
 */
class DiagonalLayoutManager : public LayoutManager
{
public:
  DiagonalLayoutManager() = default;

  // Both overrides below are pure functions of the constraints/bounds they are handed
  // and of the children's layout-tracked state -- no actor geometry is read, and no
  // state is kept on the manager. That is what a layout manager should aim for.
  //
  // The MEASURE cache applies unconditionally, so Measure() is skipped for an
  // unchanged constraint and must always satisfy that contract.
  // ArrangePolicy::IF_CHANGED is also the default for a custom LayoutManager, so
  // Arrange() may be skipped when its tracked inputs are unchanged. Select
  // ArrangePolicy::ALWAYS in the constructor if Arrange() reads untracked state or
  // must perform externally visible work on every pass.
  //
  // If this manager ever grows state of its own -- a spacing, a step angle -- its
  // setter must call InvalidateOwnerMeasure(), because no cache key can see it. See
  // SetStep() below.
  MeasuredSize Measure(ViewImpl* view, float widthConstraint, float heightConstraint) override
  {
    float          totalWidth  = 0.0f;
    float          totalHeight = 0.0f;
    const uint32_t count       = GetChildViewCount(view);

    for(uint32_t i = 0; i < count; ++i)
    {
      View      child     = GetChildViewAt(view, i);
      ViewImpl& childImpl = GetImpl(child);

      if(IsStandalone(&childImpl))
      {
        continue;
      }

      MeasuredSize sz = childImpl.Measure(widthConstraint - totalWidth,
                                          heightConstraint - totalHeight);
      totalWidth += sz.width;
      totalHeight += sz.height;
    }

    return {totalWidth, totalHeight};
  }

  void Arrange(ViewImpl* view, const LayoutRect& bounds) override
  {
    float          x     = bounds.x;
    float          y     = bounds.y;
    const uint32_t count = GetChildViewCount(view);

    for(uint32_t i = 0; i < count; ++i)
    {
      View      child     = GetChildViewAt(view, i);
      ViewImpl& childImpl = GetImpl(child);

      if(IsStandalone(&childImpl))
      {
        continue;
      }

      MeasuredSize sz = childImpl.GetMeasuredSize();
      childImpl.Arrange({x, y, sz.width, sz.height});
      x += sz.width * mStep;
      y += sz.height * mStep;
    }
  }

  /**
   * @brief Scales the diagonal step between successive children.
   *
   * The point of this setter, for the sample, is the invalidation. mStep is state held
   * on the MANAGER: it is read by Arrange() but it is not part of the owner's layout
   * state, so neither the measure cache key nor the arrange cache key can see it
   * change. Without InvalidateOwnerMeasure() the owner would keep serving the result
   * it computed against the old value, and nothing would even schedule a pass.
   *
   * The equality guard is the other half: writing the value it already holds must not
   * schedule work.
   */
  void SetStep(float step)
  {
    if(mStep == step)
    {
      return;
    }
    mStep = step;
    InvalidateOwnerMeasure();
  }

private:
  float mStep{1.0f};
};

class TcCustomLayoutManager : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "55. Custom LayoutManager";
  }

  Dali::String GetDescription() const override
  {
    return "Verify attached LayoutManager arrangement and owner invalidation";
  }

  void OnEnter(View contentArea) override
  {

    // Attach the custom LayoutManager directly to a plain View. The View now
    // delegates Measure/Arrange to DiagonalLayoutManager for every layout pass.
    mRoot = View::New();
    // Keep a raw pointer to the manager so the buttons below can retune it at
    // runtime. The View owns the manager from here on, so this pointer stays valid for
    // as long as `root` does.
    auto managed = Dali::MakeUnique<DiagonalLayoutManager>();
    mManager     = managed.Get();
    mRoot.AttachLayoutManager(std::move(managed));

    View child1 = View::New();
    child1.SetRequestedWidth(50.0f);
    child1.SetRequestedHeight(50.0f);
    child1.SetBackgroundColor(Vector4(0.9f, 0.2f, 0.2f, 1.0f));

    View child2 = View::New();
    child2.SetRequestedWidth(100.0f);
    child2.SetRequestedHeight(100.0f);
    child2.SetBackgroundColor(Vector4(0.2f, 0.7f, 0.2f, 1.0f));

    View child3 = View::New();
    child3.SetRequestedWidth(200.0f);
    child3.SetRequestedHeight(200.0f);
    child3.SetBackgroundColor(Vector4(0.2f, 0.3f, 0.9f, 1.0f));

    mRoot.Add(child1);
    mRoot.Add(child2);
    mRoot.Add(child3);

    StackLayout controls = StackLayout::New(StackOrientation::HORIZONTAL);
    controls.SetRequestedWidth(MATCH_PARENT);
    controls.SetRequestedHeight(60.0f);
    controls.SetSpacing(12.0f);

    Label normalStep = MakeButton("Step 1.0");
    normalStep.TouchEventSignal().Connect(this, &TcCustomLayoutManager::OnNormalStepTouched);
    controls.Add(normalStep);

    Label wideStep = MakeButton("Step 1.5");
    wideStep.TouchEventSignal().Connect(this, &TcCustomLayoutManager::OnWideStepTouched);
    controls.Add(wideStep);

    StackLayout page = StackLayout::New(StackOrientation::VERTICAL);
    page.SetRequestedWidth(MATCH_PARENT);
    page.SetRequestedHeight(MATCH_PARENT);
    page.SetSpacing(16.0f);
    page.SetPadding(Insets(24.0f, 24.0f, 24.0f, 24.0f));
    page.Add(controls);
    page.Add(mRoot);
    contentArea.Add(page);
  }

  void OnExit() override
  {
    DisconnectAll();
    mRoot.Reset();
    mManager = nullptr;
  }

private:
  Label MakeButton(const Dali::String& text)
  {
    Label button = Label::New(text);
    button.SetRequestedWidth(160.0f);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetBackgroundColor(UiColor(0x1565C0));
    button.SetTextColor(Color::WHITE);
    button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    button.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return button;
  }

  bool OnNormalStepTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::STARTED && mManager)
    {
      mManager->SetStep(1.0f);
      return true;
    }
    return false;
  }

  bool OnWideStepTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::STARTED && mManager)
    {
      mManager->SetStep(1.5f);
      return true;
    }
    return false;
  }

  View                   mRoot;
  DiagonalLayoutManager* mManager{nullptr};
};

REGISTER_MANUAL_TEST(TcCustomLayoutManager)
