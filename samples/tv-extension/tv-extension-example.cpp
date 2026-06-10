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

#include "tv-config.h"
#include "tv-ext.h"

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <cstdio>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

const char* ToString(FluxPolicy policy)
{
  switch(policy)
  {
    case FluxPolicy::CALM:
      return "CALM";
    case FluxPolicy::BRIGHT:
      return "BRIGHT";
    case FluxPolicy::BOLD:
      return "BOLD";
    case FluxPolicy::SHARP:
      return "SHARP";
    case FluxPolicy::NONE:
    default:
      return "NONE";
  }
}

Label MakeStatusLabel(const char* text, float y)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(620.0f);
  label.SetRequestedHeight(48.0f);
  label.SetRequestedPositionX(40.0f);
  label.SetRequestedPositionY(y);
  label.SetFontSize(18.0f);
  label.SetTextColor(UiColor(0x202124));
  label.SetBackgroundColor(UiColor(0xF1F3F4));
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

} // unnamed namespace

class TVExtensionExampleController : public ConnectionTracker
{
public:
  explicit TVExtensionExampleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TVExtensionExampleController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    View view = View::New();
    view.SetRequestedWidth(180.0f);
    view.SetRequestedHeight(120.0f);
    view.SetRequestedPositionX(40.0f);
    view.SetRequestedPositionY(40.0f);
    view.SetBackgroundColor(UiColor(0xDDE7F6));

    Label label = Label::New("Label + FluxLabelData");
    label.SetRequestedWidth(260.0f);
    label.SetRequestedHeight(120.0f);
    label.SetRequestedPositionX(250.0f);
    label.SetRequestedPositionY(40.0f);
    label.SetFontSize(20.0f);
    label.SetTextColor(UiColor(0xFFFFFF));
    label.SetBackgroundColor(UiColor(0x1565C0));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);

    ImageView imageView = ImageView::New();
    imageView.SetRequestedWidth(120.0f);
    imageView.SetRequestedHeight(120.0f);
    imageView.SetRequestedPositionX(540.0f);
    imageView.SetRequestedPositionY(40.0f);
    imageView.SetBackgroundColor(UiColor(0x2E7D32));

    mViewStatus  = MakeStatusLabel("View policy: pending", 200.0f);
    mLabelStatus = MakeStatusLabel("Label policy / overflow: pending", 260.0f);
    mImageStatus = MakeStatusLabel("Image async resize: pending", 320.0f);

    view.With(TVExt::FluxPolicyAppliedSignal).Connect(this, &TVExtensionExampleController::OnViewFluxPolicyApplied);
    label.With(TVExt::FluxPolicyAppliedSignal).Connect(this, &TVExtensionExampleController::OnLabelFluxPolicyApplied);
    imageView.With(TVExt::FluxPolicyAppliedSignal).Connect(this, &TVExtensionExampleController::OnImageFluxPolicyApplied);
    imageView.With(TVExt::AsyncImageResizeFinishedSignal).Connect(this, &TVExtensionExampleController::OnAsyncImageResizeFinished);

    view.With(TVExt::SetFluxPolicy, FluxPolicy::CALM);
    label.With(TVExt::SetFluxPolicy, FluxPolicy::BOLD);
    label.With(TVExt::SetOverflowOption, 7);
    imageView.With(TVExt::SetFluxPolicy, FluxPolicy::SHARP);
    imageView.With(TVExt::SetAsyncImageResize, true);

    UpdateStatusTexts(view, label, imageView);

    window.Add(view);
    window.Add(label);
    window.Add(imageView);
    window.Add(mViewStatus);
    window.Add(mLabelStatus);
    window.Add(mImageStatus);
  }

private:
  void UpdateStatusTexts(View view, Label label, ImageView imageView)
  {
    char buffer[160];

    std::snprintf(buffer, sizeof(buffer), "View policy: %s", ToString(view.With(TVExt::GetFluxPolicy)));
    mViewStatus.SetText(buffer);

    std::snprintf(buffer, sizeof(buffer), "Label policy: %s, overflow option: %d", ToString(label.With(TVExt::GetFluxPolicy)), label.With(TVExt::GetOverflowOption));
    mLabelStatus.SetText(buffer);

    std::snprintf(buffer, sizeof(buffer), "Image policy: %s, async resize: %s", ToString(imageView.With(TVExt::GetFluxPolicy)), imageView.With(TVExt::GetAsyncImageResize) ? "true" : "false");
    mImageStatus.SetText(buffer);
  }

  void OnViewFluxPolicyApplied(View view)
  {
    char buffer[120];
    std::snprintf(buffer, sizeof(buffer), "View policy applied after timer: %s", ToString(view.With(TVExt::GetFluxPolicy)));
    mViewStatus.SetText(buffer);
  }

  void OnLabelFluxPolicyApplied(View view)
  {
    Label label = Label::DownCast(view);
    char  buffer[140];
    std::snprintf(buffer, sizeof(buffer), "Label policy applied: %s, overflow option: %d", ToString(view.With(TVExt::GetFluxPolicy)), label.With(TVExt::GetOverflowOption));
    mLabelStatus.SetText(buffer);
  }

  void OnImageFluxPolicyApplied(View view)
  {
    ImageView imageView = ImageView::DownCast(view);
    char      buffer[140];
    std::snprintf(buffer, sizeof(buffer), "Image policy applied: %s, async resize: %s", ToString(view.With(TVExt::GetFluxPolicy)), imageView.With(TVExt::GetAsyncImageResize) ? "true" : "false");
    mImageStatus.SetText(buffer);
  }

  bool OnAsyncImageResizeFinished(ImageView imageView)
  {
    char buffer[140];
    std::snprintf(buffer, sizeof(buffer), "Async image resize finished, async resize: %s", imageView.With(TVExt::GetAsyncImageResize) ? "true" : "false");
    mImageStatus.SetText(buffer);
    return false;
  }

private:
  Application& mApplication;
  Label        mViewStatus;
  Label        mLabelStatus;
  Label        mImageStatus;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  TVConfig::New().Apply();
  TVExtensionExampleController controller(application);
  application.MainLoop();
  return 0;
}
