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
#include <dali-ui-foundation/public-api/render-effects/mask-effect.h>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float WIDTH = 540.0f;
constexpr float HEIGHT = 640.0f;
constexpr float CONTENT_HEIGHT = 2400.0f;
constexpr float FADE_WIDTH = 100.0f;

void place(View view, float x, float y, float width, float height)
{
  view.SetRequestedX(x);
  view.SetRequestedY(y);
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
}

class GradientMaskExample : public ConnectionTracker
{
public:
  explicit GradientMaskExample(Application& application)
  {
    application.InitSignal().Connect(this, &GradientMaskExample::create);
  }

private:
  void create(Application application)
  {
    Window window = application.GetWindow();
    auto bounds = window.GetPositionSize();
    window.SetPositionSize(PositionSize(bounds.x, bounds.y, 600, 800));

    window.Add(createBackground());

    ScrollView scrollView = createScrollView();
    window.Add(scrollView);

    View maskView = createGradientMask();
    window.Add(maskView);

    // MaskEffect uses the source View's alpha to mask the entire ScrollView.
    MaskEffect maskEffect = MaskEffect::New(maskView);
    // By default, both views are captured every frame, reflecting content scrolling.
    scrollView.SetRenderEffect(maskEffect);
  }

  View createBackground()
  {
    View background = View::New();
    background.SetBackgroundColor(UiColor(0x454A52));
    place(background, 0, 0, 600, 800);
    return background;
  }

  ScrollView createScrollView()
  {
    ScrollView scrollView = ScrollView::New();
    place(scrollView, 30, 90, WIDTH, HEIGHT);
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetKeyScrollEnabled(true);
    scrollView.SetFocusable(true);
    scrollView.SetContent(createScrollContent());
    return scrollView;
  }

  View createScrollContent()
  {
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(WIDTH);
    content.SetRequestedHeight(CONTENT_HEIGHT);
    content.SetPadding(Insets(16.0f, 16.0f, 12.0f, 18.0f));
    content.SetSpacing(30.0f);
    for(int index = 0; index < 10; ++index)
    {
      View card = View::New();
      card.SetRequestedWidth(WIDTH - 32.0f);
      card.SetRequestedHeight(210.0f);
      card.SetBackgroundColor(Color::WHITE);
      card.SetCornerRadius(32.0f);
      Label title = Label::New((std::string("Card ") + std::to_string(index + 1)).c_str());
      place(title, 28, 28, 400, 48);
      card.Add(title);
      for(int line = 0; line < 2; ++line)
      {
        View bar = View::New();
        place(bar, 28, 104 + line * 38, 310 - line * 90, 20);
        bar.SetBackgroundColor(Vector4(0.08f, 0.22f, 0.30f, 0.35f));
        bar.SetCornerRadius(10.0f);
        card.Add(bar);
      }
      content.Add(card);
    }
    return content;
  }

  View createGradientMask()
  {
    View maskView = View::New();
    place(maskView, 30, 90, WIDTH, HEIGHT);

    GradientVisual gradient = GradientVisual::New();
    gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
    gradient.SetLinearGradient(Vector2(0, -0.5f), Vector2(0, 0.5f));
    const float edge = FADE_WIDTH / HEIGHT;
    // Sample smoothstep so the fade joins transparent and opaque regions gently.
    constexpr int STEPS = 16;
    Dali::Vector<Gradient::StopNode> stops;
    for(int index = 0; index <= STEPS; ++index)
    {
      const float t = static_cast<float>(index) / STEPS;
      const float alpha = t * t * (3.0f - 2.0f * t);
      stops.PushBack(Gradient::StopNode(edge * t, Vector4(1, 1, 1, alpha)));
    }
    for(int index = 0; index <= STEPS; ++index)
    {
      const float t = static_cast<float>(index) / STEPS;
      const float alpha = 1.0f - t * t * (3.0f - 2.0f * t);
      stops.PushBack(Gradient::StopNode(1.0f - edge + edge * t, Vector4(1, 1, 1, alpha)));
    }
    gradient.SetStopNodes(stops);
    maskView.AddVisual(gradient, Visual::DepthLayer::BACKGROUND);
    return maskView;
  }
};
} // namespace

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  GradientMaskExample example(application);
  application.MainLoop();
  return 0;
}
