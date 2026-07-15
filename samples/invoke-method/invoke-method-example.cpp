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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float WINDOW_WIDTH  = 480.0f;
constexpr float WINDOW_HEIGHT = 800.0f;

template<typename ValueType>
bool SetPropertyByName(Handle handle, const char* propertyName, const ValueType& value)
{
  Property::Index index = handle.GetPropertyIndex(Property::Key(propertyName));
  if(index == Property::INVALID_INDEX)
  {
    DALI_LOG_ERROR("[invoke-method-sample] SetProperty(%s): INVALID_INDEX\n", propertyName);
    return false;
  }

  handle.SetProperty(index, value);
  return true;
}

template<typename... ArgumentTypes>
bool Invoke(BaseHandle handle, const char* methodName, const ArgumentTypes&... values)
{
  InvokeArguments arguments;
  (arguments.PushBack(Any(values)), ...);

  InvokeResult result;
  bool         success = handle.InvokeMethod(methodName, arguments, result);
  DALI_LOG_ERROR("[invoke-method-sample] %s: %s\n", methodName, success ? "PASS" : "FAIL");
  return success;
}

template<typename ResultType>
bool InvokeWithResult(BaseHandle handle, const char* methodName, ResultType& output)
{
  InvokeArguments arguments;
  InvokeResult    result;
  bool            success = handle.InvokeMethod(methodName, arguments, result);
  DALI_LOG_ERROR("[invoke-method-sample] %s: %s\n", methodName, success ? "PASS" : "FAIL");
  if(!success)
  {
    return false;
  }

  const ResultType* value = AnyCast<ResultType>(&result);
  if(!value)
  {
    DALI_LOG_ERROR("[invoke-method-sample] %s: FAIL (unexpected result type)\n", methodName);
    return false;
  }

  output = *value;
  return true;
}

template<typename ParentType, typename ChildType>
bool AddActor(ParentType parent, ChildType child)
{
  Actor parentActor = Actor::DownCast(parent);
  Actor childActor  = Actor::DownCast(child);
  if(!parentActor || !childActor)
  {
    DALI_LOG_ERROR("[invoke-method-sample] Add: FAIL (invalid actor handle)\n");
    return false;
  }

  return Invoke(parentActor, "Add", childActor);
}

void Place(View view, const Vector3& position, const Vector3& size)
{
  Invoke(view, "SetParentOrigin", ParentOrigin::TOP_LEFT);
  Invoke(view, "SetPivot", Pivot::TOP_LEFT);
  Invoke(view, "SetRequestedX", position.x);
  Invoke(view, "SetRequestedY", position.y);
  Invoke(view, "SetRequestedWidth", size.width);
  Invoke(view, "SetRequestedHeight", size.height);
}

View CreateColorPanel(const Vector3& position, const Vector3& size, const Vector4& color)
{
  View panel = View::New();
  Place(panel, position, size);
  Invoke(panel, "SetOpacity", color.a);
  SetPropertyByName(panel, "background", color);
  return panel;
}

ImageView CreateImage(const Dali::String& resourceUrl, const Vector3& position, const Vector3& size, const UiColor& color)
{
  ImageView imageView = ImageView::New();
  Place(imageView, position, size);
  Invoke(imageView, "SetResourceUrl", resourceUrl);
  Invoke(imageView, "SetImageColor", color);
  return imageView;
}

Label CreateLabel(const char* text, const Vector3& position, const Vector3& size, float fontSize, const Vector4& color)
{
  Label label = Label::New();
  Place(label, position, size);

  Invoke(label, "SetText", Dali::String(text));
  Invoke(label, "SetFontSize", fontSize);
  Invoke(label, "SetTextColor", UiColor(color));
  Invoke(label, "SetMultiLine", true);
  Invoke(label, "SetOpacity", color.a);
  return label;
}

View CreateMetricCard(const Vector3& position, const Vector4& background, const char* value, const char* caption)
{
  View card = CreateColorPanel(position, Vector3(188.0f, 122.0f, 0.0f), background);
  AddActor(card, CreateLabel(value, Vector3(18.0f, 18.0f, 0.0f), Vector3(152.0f, 40.0f, 0.0f), 30.0f, Color::WHITE));
  AddActor(card, CreateLabel(caption, Vector3(18.0f, 66.0f, 0.0f), Vector3(152.0f, 38.0f, 0.0f), 15.0f, Vector4(1.0f, 1.0f, 1.0f, 0.74f)));
  return card;
}

} // namespace

class InvokeMethodExample : public ConnectionTracker
{
public:
  explicit InvokeMethodExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &InvokeMethodExample::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    Invoke(window, "SetBackgroundColor", Vector4(0.94f, 0.96f, 0.99f, 1.0f));

    View root = View::New();
    Place(root, Vector3::ZERO, Vector3(WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f));
    Invoke(root, "SetName", Dali::String("InvokeMethodRoot"));
    Invoke(window, "Add", Actor::DownCast(root));

    mBackdrop = CreateColorPanel(Vector3::ZERO, Vector3(WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f), Vector4(0.94f, 0.96f, 0.99f, 1.0f));
    AddActor(root, mBackdrop);

    ImageView heroImage = CreateImage(Dali::String(RESOURCES_DIR "landscape-sample.jpg"), Vector3(32.0f, 42.0f, 0.0f), Vector3(416.0f, 214.0f, 0.0f), UiColor(Color::WHITE));
    Invoke(heroImage, "SetPivot", Pivot::CENTER);
    AddActor(root, heroImage);

    View overlay = CreateColorPanel(Vector3(32.0f, 42.0f, 0.0f), Vector3(416.0f, 214.0f, 0.0f), Vector4(0.05f, 0.08f, 0.12f, 0.36f));
    AddActor(root, overlay);

    ImageView logo = CreateImage(Dali::String(RESOURCES_DIR "dali-logo-anim-001.png"), Vector3(342.0f, 60.0f, 0.0f), Vector3(72.0f, 72.0f, 0.0f), UiColor(Color::WHITE));
    Invoke(logo, "SetPivot", Pivot::CENTER);
    AddActor(root, logo);

    Label title = CreateLabel("InvokeMethod Gallery", Vector3(54.0f, 76.0f, 0.0f), Vector3(300.0f, 54.0f, 0.0f), 30.0f, Color::WHITE);
    AddActor(root, title);

    Label subtitle = CreateLabel("Label text, color, size and marquee are called through generated InvokeMethod wrappers.", Vector3(54.0f, 136.0f, 0.0f), Vector3(330.0f, 76.0f, 0.0f), 17.0f, Vector4(1.0f, 1.0f, 1.0f, 0.82f));
    AddActor(root, subtitle);

    View textCard = CreateMetricCard(Vector3(32.0f, 286.0f, 0.0f), Vector4(0.10f, 0.42f, 0.82f, 1.0f), "SetText", "Dali::String argument");
    textCard.ConnectSignal(this, "touched", [this, textCard]()
    { OnTextCardTouched(textCard); });
    AddActor(root, textCard);

    View colorCard = CreateMetricCard(Vector3(260.0f, 286.0f, 0.0f), Vector4(0.90f, 0.36f, 0.12f, 1.0f), "UiColor", "typed Any argument");
    colorCard.ConnectSignal(this, "touched", [this, colorCard]()
    { OnColorCardTouched(colorCard); });
    AddActor(root, colorCard);

    View fontCard = CreateMetricCard(Vector3(32.0f, 438.0f, 0.0f), Vector4(0.10f, 0.58f, 0.46f, 1.0f), "Font", "SetFontSize(float)");
    fontCard.ConnectSignal(this, "touched", [this, fontCard]()
    { OnFontCardTouched(fontCard); });
    AddActor(root, fontCard);

    View loopCard = CreateMetricCard(Vector3(260.0f, 438.0f, 0.0f), Vector4(0.45f, 0.26f, 0.78f, 1.0f), "Loop", "StartMarquee()");
    loopCard.ConnectSignal(this, "touched", [this, loopCard]()
    { OnLoopCardTouched(loopCard); });
    AddActor(root, loopCard);

    View tickerBar = CreateColorPanel(Vector3(32.0f, 612.0f, 0.0f), Vector3(416.0f, 72.0f, 0.0f), Vector4(0.10f, 0.12f, 0.18f, 1.0f));
    AddActor(root, tickerBar);

    mTicker = CreateLabel("Touch a color card to update the sample background through InvokeMethod-friendly state changes.", Vector3(18.0f, 18.0f, 0.0f), Vector3(380.0f, 34.0f, 0.0f), 18.0f, Color::WHITE);
    AddActor(tickerBar, mTicker);
    Invoke(mTicker, "StartMarquee");

    uint32_t rootChildCount = 0u;
    InvokeWithResult(root, "GetChildCount", rootChildCount);
    DALI_LOG_ERROR("[invoke-method-sample] root child count after InvokeMethod Add: %u\n", rootChildCount);

    mAmbientAnimation = Animation::New(3.0f);

    AlphaFunction ambientEase(AlphaFunction::EASE_IN_OUT);

    ViewAnimationSpec heroSpec = View::NewAnimationSpec();
    Invoke(heroSpec, "ScaleX", 1.035f, Duration(3.0f), ambientEase);
    Invoke(heroSpec, "ScaleY", 1.035f, Duration(3.0f), ambientEase);
    Invoke(heroSpec, "ApplyTo", mAmbientAnimation, View::DownCast(heroImage));

    ViewAnimationSpec logoSpec = View::NewAnimationSpec();
    Invoke(logoSpec, "PositionYBy", 10.0f, Duration(3.0f), ambientEase);
    Invoke(logoSpec, "Opacity", 0.72f, Duration(3.0f), ambientEase);
    Invoke(logoSpec, "ApplyTo", mAmbientAnimation, View::DownCast(logo));

    Invoke(mAmbientAnimation, "SetLooping", true);
    Invoke(mAmbientAnimation, "Play");
  }

  void OnTextCardTouched(View card)
  {
    ApplyTouchedPalette(card, Vector4(0.84f, 0.91f, 1.0f, 1.0f), "Touched SetText card: background shifted to blue.");
  }

  void OnColorCardTouched(View card)
  {
    ApplyTouchedPalette(card, Vector4(1.0f, 0.90f, 0.83f, 1.0f), "Touched UiColor card: background shifted to orange.");
  }

  void OnFontCardTouched(View card)
  {
    ApplyTouchedPalette(card, Vector4(0.84f, 0.96f, 0.91f, 1.0f), "Touched Font card: background shifted to green.");
  }

  void OnLoopCardTouched(View card)
  {
    ApplyTouchedPalette(card, Vector4(0.91f, 0.86f, 1.0f, 1.0f), "Touched Loop card: background shifted to purple.");
  }

  void ApplyTouchedPalette(View touchedCard, const Vector4& backgroundColor, const char* message)
  {
    if(touchedCard)
    {
      Invoke(touchedCard, "SetScale", 1.03f, 1.03f);
      Invoke(touchedCard, "SetOpacity", 0.94f);
    }

    SetPropertyByName(mBackdrop, "background", backgroundColor);
    Invoke(mTicker, "SetText", Dali::String(message));
    Invoke(mTicker, "StartMarquee");
    DALI_LOG_ERROR("[invoke-method-sample] touch palette applied: %s\n", message);
  }

private:
  Application& mApplication;
  Animation    mAmbientAnimation;
  Label        mTicker;
  View         mBackdrop;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  InvokeMethodExample example(application);
  application.MainLoop();
  return 0;
}
