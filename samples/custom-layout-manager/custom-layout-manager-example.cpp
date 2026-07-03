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

  MeasuredSize Arrange(ViewImpl* view, const LayoutRect& bounds) override
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
      x += sz.width;
      y += sz.height;
    }

    return {bounds.width, bounds.height};
  }
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
    root.AttachLayoutManager(Dali::MakeUnique<DiagonalLayoutManager>());

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
    }
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                   application = Application::New(&argc, &argv);
  CustomLayoutManagerController controller(application);
  application.MainLoop();
  return 0;
}
