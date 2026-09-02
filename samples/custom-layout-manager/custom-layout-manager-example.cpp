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

class CustomLayoutManagerController : public ConnectionTracker
{
public:
  CustomLayoutManagerController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &CustomLayoutManagerController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Attach the custom LayoutManager directly to a plain View. The View now
    // delegates Measure/Arrange to DiagonalLayoutManager for every layout pass.
    View root = View::New();
    // Keep a raw pointer to the manager so the key handler below can retune it at
    // runtime. The View owns the manager from here on, so this pointer stays valid for
    // as long as `root` does.
    auto managed = Dali::MakeUnique<DiagonalLayoutManager>();
    mManager     = managed.Get();
    root.AttachLayoutManager(std::move(managed));

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

    root.Add(child1);
    root.Add(child2);
    root.Add(child3);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &CustomLayoutManagerController::OnKeyEvent);
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
      else if(mManager)
      {
        // Retune the manager's own state. Nothing about this write is visible to the
        // layout caches, so DiagonalLayoutManager::SetStep has to invalidate its owner
        // itself -- see the comment there. Without that call this key would change
        // nothing on screen, however many frames later you looked.
        if(IsKey(event, Dali::DALI_KEY_CURSOR_UP))
        {
          mManager->SetStep(1.5f);
        }
        else if(IsKey(event, Dali::DALI_KEY_CURSOR_DOWN))
        {
          mManager->SetStep(1.0f);
        }
      }
    }
  }

private:
  Application&           mApplication;
  DiagonalLayoutManager* mManager{nullptr};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                   application = Application::New(&argc, &argv);
  CustomLayoutManagerController controller(application);
  application.MainLoop();
  return 0;
}
