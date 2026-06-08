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
 * Custom layout sample with a clickable LayoutDirection toggle.
 *
 * Same content as customlayout-diagonal-example. A 100x50 yellow
 * standalone child at the top-left toggles the root view's
 * LayoutDirection between LEFT_TO_RIGHT and RIGHT_TO_LEFT on click.
 *
 * The ArrangeCallback arranges children diagonally in a LEFT_TO_RIGHT
 * frame; the framework mirrors direct children horizontally after the
 * callback returns when the resolved direction is RIGHT_TO_LEFT.
 * The standalone toggle button is excluded from mirroring and stays
 * at the top-left.
 *
 * Press Escape or Back to quit.
 */
class DiagonalLayout
{
public:
  static MeasuredSize OnMeasure(View self, float widthConstraint, float heightConstraint)
  {
    float totalHeight = 0.0f;

    for(uint32_t i = 0; i < self.GetChildCount(); ++i)
    {
      View child = self.GetChildAt(i);
      // Standalone children carry their own size/position; framework arranges
      // them via ArrangeStandaloneChildren after this callback returns.
      if(child.GetLayoutMode() == LayoutMode::STANDALONE)
      {
        continue;
      }
      MeasuredSize sz = child.Measure(widthConstraint, heightConstraint - totalHeight);
      totalHeight += sz.height;
    }

    // Span the full available width so the framework's RTL mirror at
    // bounds.width matches the window width.
    return {widthConstraint, totalHeight};
  }

  static MeasuredSize OnArrange(View self, const LayoutRect& bounds)
  {
    float x = bounds.x;
    float y = bounds.y;

    for(uint32_t i = 0; i < self.GetChildCount(); ++i)
    {
      View child = self.GetChildAt(i);
      if(child.GetLayoutMode() == LayoutMode::STANDALONE)
      {
        continue;
      }
      MeasuredSize sz = child.GetMeasuredSize();
      child.Arrange({x, y, sz.width, sz.height});
      x += sz.width;
      y += sz.height;
    }

    return {bounds.width, bounds.height};
  }
};

class CustomLayoutDirectionController : public ConnectionTracker
{
public:
  CustomLayoutDirectionController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &CustomLayoutDirectionController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    mRoot = Layout::New();
    mRoot.SetMeasureCallback(MeasureCallback::New(&DiagonalLayout::OnMeasure));
    mRoot.SetArrangeCallback(ArrangeCallback::New(&DiagonalLayout::OnArrange));

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

    // Toggle button: 200x50 translucent-black standalone child at top-left
    // with a centered white "Change LayoutDirection" label.
    InteractiveView toggleBtn = InteractiveView::New();
    toggleBtn.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.5f));
    toggleBtn.SetRequestedWidth(200.0f);
    toggleBtn.SetRequestedHeight(50.0f);
    toggleBtn.SetRequestedPositionX(0.0f);
    toggleBtn.SetRequestedPositionY(0.0f);
    toggleBtn.SetLayoutMode(LayoutMode::STANDALONE);
    toggleBtn.SetLayoutDirection(Dali::LayoutDirection::LEFT_TO_RIGHT);
    Label toggleLabel = Label::New("Change LayoutDirection");
    toggleLabel.SetTextColor(UiColor(1.0f, 1.0f, 1.0f, 1.0f));
    toggleLabel.SetRequestedWidth(MATCH_PARENT);
    toggleLabel.SetRequestedHeight(MATCH_PARENT);
    toggleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    toggleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    toggleBtn.Add(toggleLabel);
    toggleBtn.ConnectClickedSignal(this, [this](View view, InputEvent event) -> bool {
      mIsRtl = !mIsRtl;
      mRoot.SetLayoutDirection(mIsRtl ? Dali::LayoutDirection::RIGHT_TO_LEFT : Dali::LayoutDirection::LEFT_TO_RIGHT);
      return true;
    });
    mRoot.Add(toggleBtn);

    window.Add(mRoot);
    window.KeyEventSignal().Connect(this, &CustomLayoutDirectionController::OnKeyEvent);
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
  Layout       mRoot;
  bool         mIsRtl{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  CustomLayoutDirectionController controller(application);
  application.MainLoop();
  return 0;
}
