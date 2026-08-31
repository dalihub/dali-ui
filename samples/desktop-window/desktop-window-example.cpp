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
 */

// This file is the application. It is not part of what a product framework
// copies - desktop-window.h and desktop-window.cpp are. Everything below is
// what an application written against that wrapper looks like: content, its own
// actions, and no window code.

#include "desktop-window.h"

#include <memory>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const UiColor CONTENT_LIGHT = UiColor(0xF3F5F7);
const UiColor CONTENT_DARK  = UiColor(0x1B242B);
const UiColor TEXT_LIGHT    = UiColor(0x17212B);
const UiColor TEXT_DARK     = UiColor(0xF3F5F7);

Label MakeLabel(const std::string& text, float fontSize, const UiColor& color)
{
  Label label = Label::New(Dali::String(text.c_str()));
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}
} // unnamed namespace

class GalleryApplication : public ConnectionTracker
{
public:
  explicit GalleryApplication(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &GalleryApplication::Create);
  }

private:
  void Create(Application application)
  {
    Window window = application.GetWindow();

    // The shadow is drawn in the surface outside the frame, so the window has
    // to be transparent for it to show.
    window.SetBackgroundColor(Color::TRANSPARENT);
    window.SetTransparency(true);
    window.KeyEventSignal().Connect(this, &GalleryApplication::OnKeyEvent);

    // Four calls. No border thickness, no window state, no handle to keep alive.
    mWindow = std::make_unique<DesktopWindow>(window, DesktopWindow::CloseCallback::New(this, &GalleryApplication::OnClose));
    mWindow->SetTitle("Gallery");
    mWindow->SetMinimumFrameSize(Vector2(300.0f, 200.0f));
    BuildContent();
    mWindow->AddBarAction("theme.png", DesktopWindow::ActionCallback::New(this, &GalleryApplication::ToggleTheme));
  }

  void BuildContent()
  {
    mContent = AbsoluteLayout::New();
    mContent.SetBackgroundColor(CONTENT_LIGHT);
    mContent.SetLayoutParams(AbsoluteLayoutParams::New()
                               .SetBounds(LayoutRect(0.0f, 0.0f, 1.0f, 1.0f))
                               .SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL));

    mHeading = MakeLabel("Gallery", 26.0f, TEXT_LIGHT);
    mHeading.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(24.0f, 20.0f, 320.0f, 38.0f)));
    mContent.Add(mHeading);

    mBody = MakeLabel("Application content. The wrapper owns the border.", 14.0f, TEXT_LIGHT);
    mBody.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(24.0f, 60.0f, 400.0f, 28.0f)));
    mContent.Add(mBody);

    mWindow->GetContentRoot().Add(mContent);
  }

  /// An application action, reached from the border. Not a window operation, so
  /// it is an ordinary callback rather than a command.
  void ToggleTheme()
  {
    mDark = !mDark;
    mContent.SetBackgroundColor(mDark ? CONTENT_DARK : CONTENT_LIGHT);
    mHeading.SetTextColor(mDark ? TEXT_DARK : TEXT_LIGHT);
    mBody.SetTextColor(mDark ? TEXT_DARK : TEXT_LIGHT);
  }

  void OnClose()
  {
    mApplication.Quit();
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN &&
       (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK)))
    {
      mApplication.Quit();
    }
  }

private:
  Application&                   mApplication;
  std::unique_ptr<DesktopWindow> mWindow;
  AbsoluteLayout                 mContent;
  Label                          mHeading;
  Label                          mBody;
  bool                           mDark{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig    config      = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  GalleryApplication controller(application);
  application.MainLoop();
  return 0;
}
