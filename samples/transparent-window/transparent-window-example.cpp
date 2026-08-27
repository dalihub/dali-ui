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

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr char BLACK_TEXT[]       = "BLACK TEXT";
constexpr char TEST_IMAGE_NAME[]  = "transparent-window-black-controls.png";

View CreatePanel(const Vector3& position, const Vector2& size, const UiColor& color)
{
  View panel = View::New();
  panel.SetParentOrigin(ParentOrigin::TOP_LEFT);
  panel.SetPivot(Vector3(0.0f, 0.0f, 0.5f));
  panel.SetRequestedX(position.x);
  panel.SetRequestedY(position.y);
  panel.SetRequestedWidth(size.width);
  panel.SetRequestedHeight(size.height);
  panel.SetBackgroundColor(color);
  return panel;
}

Label CreateBlackTextCard(const Vector3& position, const Vector2& size)
{
  Label label = Label::New(BLACK_TEXT);
  label.SetParentOrigin(ParentOrigin::TOP_LEFT);
  label.SetPivot(Vector3(0.0f, 0.0f, 0.5f));
  label.SetRequestedX(position.x);
  label.SetRequestedY(position.y);
  label.SetRequestedWidth(size.width);
  label.SetRequestedHeight(size.height);
  label.SetBackgroundColor(UiColor(0xFFFFFF));
  label.SetTextColor(UiColor(0x000000));
  label.SetFontSize(42.0f);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

ImageView CreateBlackPngImage()
{
  const std::string imagePath = std::string(RESOURCES_DIR) + TEST_IMAGE_NAME;
  return ImageView::New(imagePath.c_str());
}

View CreateBlackPngCard(const Vector3& position, const Vector2& size)
{
  View      card  = CreatePanel(position, size, UiColor(0xFFFFFF));
  ImageView image = CreateBlackPngImage();
  image.SetParentOrigin(ParentOrigin::TOP_LEFT);
  image.SetPivot(Vector3(0.0f, 0.0f, 0.5f));
  image.SetRequestedX(35.0f);
  image.SetRequestedY(15.0f);
  image.SetRequestedWidth(size.width - 70.0f);
  image.SetRequestedHeight(size.height - 30.0f);
  image.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
  card.Add(image);
  return card;
}
} // unnamed namespace

class TransparentWindowController : public ConnectionTracker
{
public:
  explicit TransparentWindowController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TransparentWindowController::Create);
  }

private:
  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::TRANSPARENT);

    // Opaque RGB colors, including black, must remain visible.
    window.Add(CreatePanel(Vector3(45.0f, 45.0f, 0.0f), Vector2(180.0f, 150.0f), UiColor(0xE53935)));
    window.Add(CreatePanel(Vector3(255.0f, 45.0f, 0.0f), Vector2(180.0f, 150.0f), UiColor(0x43A047)));
    window.Add(CreatePanel(Vector3(465.0f, 45.0f, 0.0f), Vector2(180.0f, 150.0f), UiColor(0x000000)));
    // This panel must blend with the desktop at 50% alpha.
    window.Add(CreatePanel(Vector3(675.0f, 45.0f, 0.0f), Vector2(180.0f, 150.0f), UiColor(0x1565C0).WithAlpha(0.5f)));

    // Both black resources are rendered over opaque white Dali backgrounds.
    // Their black pixels and anti-aliased grayscale edges must remain visible.
    window.Add(CreateBlackTextCard(Vector3(45.0f, 235.0f, 0.0f), Vector2(390.0f, 150.0f)));
    window.Add(CreateBlackPngCard(Vector3(465.0f, 235.0f, 0.0f), Vector2(390.0f, 150.0f)));
    // Keep the renderer active long enough to evaluate window dragging and
    // input responsiveness while transparent content is being presented.
    window.KeepRendering(60.0f);
  }

  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  WindowData   windowData;
  PositionSize positionSize(100, 100, 1400, 650);
  // Leave enough physical pixels for high-DPI desktop scale factors.
  windowData.SetPositionSize(positionSize);
  windowData.SetTransparency(true);

  Application application = Application::New(&argc, &argv, "", false, windowData);
  TransparentWindowController controller(application);
  application.MainLoop();
  return 0;
}
