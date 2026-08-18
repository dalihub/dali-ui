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

// Portable Actor annotation sample for Ubuntu and Windows.
//
// The sample uses real Dali::Ui::View annotation and focus APIs. The four
// Tizen.Action.View requests are represented by a local simulator so the
// interaction remains available without TIDL or rpc-port.

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

#include <algorithm>
#include <functional>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t COLOR_CANVAS          = 0xF3F6FBu;
constexpr uint32_t COLOR_INK             = 0x172033u;
constexpr uint32_t COLOR_MUTED           = 0x68738Au;
constexpr uint32_t COLOR_PANEL           = 0xFFFFFFu;
constexpr uint32_t COLOR_BORDER          = 0xDCE3EFu;
constexpr uint32_t COLOR_PRIMARY         = 0x4667F2u;
constexpr uint32_t COLOR_PRIMARY_DARK    = 0x263A8Bu;
constexpr uint32_t COLOR_SUCCESS         = 0x19A974u;
constexpr uint32_t COLOR_WARNING         = 0xE69A24u;
constexpr uint32_t COLOR_DANGER          = 0xD94A64u;
constexpr uint32_t COLOR_CONSOLE         = 0x111827u;
constexpr uint32_t COLOR_CONSOLE_TEXT    = 0xD7E4FFu;
constexpr uint32_t COLOR_SELECTED_CARD   = 0xEEF2FFu;
constexpr uint32_t COLOR_UNSELECTED_CARD = 0xFFFFFFu;

struct EntityCard
{
  InteractiveView card;
  Label           state;
  Label           metadata;
  std::string     title;
  std::string     entityId;
  std::string     entityType;
  std::string     entityInfo;
  uint32_t        accentColor{COLOR_PRIMARY};
};

Label MakeLabel(const std::string& text, float fontSize, uint32_t color, float height = WRAP_CONTENT)
{
  Label label = Label::New(text.c_str());
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(height);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

std::string JsonEscape(const std::string& value)
{
  std::string escaped;
  escaped.reserve(value.size());
  for(const char character : value)
  {
    switch(character)
    {
      case '"':
        escaped += "\\\"";
        break;
      case '\\':
        escaped += "\\\\";
        break;
      case '\n':
        escaped += "\\n";
        break;
      case '\r':
        escaped += "\\r";
        break;
      case '\t':
        escaped += "\\t";
        break;
      default:
        escaped += character;
        break;
    }
  }
  return escaped;
}
} // unnamed namespace

class AppEntityAnnotationExample : public ConnectionTracker
{
public:
  explicit AppEntityAnnotationExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AppEntityAnnotationExample::Create);
  }

  ~AppEntityAnnotationExample() = default;

private:
  void Create(Application application)
  {
    UiConfig config = UiConfig::New();
    config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
    config.Apply();

    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_CANVAS));
    window.KeyEventSignal().Connect(this, &AppEntityAnnotationExample::OnKeyEvent);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(28, 28, 22, 18));
    root.SetSpacing(16.0f);

    root.Add(CreateHeader());

    StackLayout workspace = StackLayout::New(StackOrientation::HORIZONTAL);
    workspace.SetRequestedWidth(MATCH_PARENT);
    workspace.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    workspace.SetSpacing(18.0f);
    workspace.Add(CreateEntityPanel());
    workspace.Add(CreateActionPanel());
    root.Add(workspace);
    root.Add(CreateFooter());

    window.Add(root);
    FocusManager::Get().FocusChangedSignal().Connect(this, &AppEntityAnnotationExample::OnFocusChanged);

    SelectEntity(0u);
    RunGetAnnotatedViews();
  }

  View CreateHeader()
  {
    StackLayout header = StackLayout::New(StackOrientation::HORIZONTAL);
    header.SetRequestedWidth(MATCH_PARENT);
    header.SetRequestedHeight(82.0f);
    header.SetPadding(Extents(22, 22, 12, 12));
    header.SetSpacing(18.0f);
    header.SetBackgroundColor(UiColor(COLOR_PRIMARY_DARK));
    header.SetCornerRadius(18.0f);

    StackLayout titleBlock = StackLayout::New(StackOrientation::VERTICAL);
    titleBlock.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    titleBlock.SetSpacing(1.0f);
    titleBlock.Add(MakeLabel("App Entity Studio", 25.0f, 0xFFFFFFu, 34.0f));
    titleBlock.Add(MakeLabel("Actor annotations on real Dali::Ui::View objects", 13.0f, 0xC8D2FFu, 24.0f));
    header.Add(titleBlock);

    Label mode = MakeLabel("PORTABLE  |  LOCAL ACTION SIMULATOR", 12.0f, 0xFFFFFFu, 38.0f);
    mode.SetRequestedWidth(300.0f);
    mode.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mode.SetBackgroundColor(UiColor(0x3C52A8u));
    mode.SetCornerRadius(19.0f);
    header.Add(mode);
    return header;
  }

  View CreateEntityPanel()
  {
    StackLayout panel = StackLayout::New(StackOrientation::VERTICAL);
    panel.SetLayoutParams(StackLayoutParams::New().SetWeight(0.46f).SetAlignment(LayoutAlignment::FILL));
    panel.SetPadding(Extents(18, 18, 16, 16));
    panel.SetSpacing(12.0f);
    panel.SetBackgroundColor(UiColor(COLOR_PANEL));
    panel.SetCornerRadius(18.0f);

    panel.Add(MakeLabel("Live Views", 20.0f, COLOR_INK, 30.0f));
    panel.Add(MakeLabel("Select a card by click, touch, or Up/Down.", 12.0f, COLOR_MUTED, 22.0f));

    AddEntityCard(panel,
                  "Living Room Light",
                  "living-room.light",
                  "Tizen.Entity.Light",
                  "{\"power\":true,\"level\":72}",
                  0xF3B63Fu);
    AddEntityCard(panel,
                  "Hall Thermostat",
                  "hall.thermostat",
                  "Tizen.Entity.Thermostat",
                  "{\"temperature\":23.5,\"unit\":\"C\"}",
                  0xEF6A66u);
    AddEntityCard(panel,
                  "Media Player",
                  "media.player",
                  "Tizen.Entity.Media",
                  "{\"state\":\"paused\",\"source\":\"HDMI 1\"}",
                  0x6D71E5u);

    StackLayout mutations = StackLayout::New(StackOrientation::HORIZONTAL);
    mutations.SetRequestedWidth(MATCH_PARENT);
    mutations.SetRequestedHeight(48.0f);
    mutations.SetSpacing(10.0f);
    mutations.Add(MakeButton("Toggle annotation", COLOR_PRIMARY, [this]() { ToggleAnnotation(); }));
    mutations.Add(MakeButton("Toggle enabled", COLOR_WARNING, [this]() { ToggleEnabled(); }));
    mutations.Add(MakeButton("Reset", COLOR_MUTED, [this]() { ResetEntities(); }));
    panel.Add(mutations);

    return panel;
  }

  View CreateActionPanel()
  {
    StackLayout panel = StackLayout::New(StackOrientation::VERTICAL);
    panel.SetLayoutParams(StackLayoutParams::New().SetWeight(0.54f).SetAlignment(LayoutAlignment::FILL));
    panel.SetPadding(Extents(18, 18, 16, 16));
    panel.SetSpacing(12.0f);
    panel.SetBackgroundColor(UiColor(COLOR_PANEL));
    panel.SetCornerRadius(18.0f);

    panel.Add(MakeLabel("Action Console", 20.0f, COLOR_INK, 30.0f));
    panel.Add(MakeLabel("Portable simulation of the Tizen.Action.View request flow", 12.0f, COLOR_MUTED, 22.0f));

    StackLayout firstRow = StackLayout::New(StackOrientation::HORIZONTAL);
    firstRow.SetRequestedWidth(MATCH_PARENT);
    firstRow.SetRequestedHeight(50.0f);
    firstRow.SetSpacing(10.0f);
    firstRow.Add(MakeButton("1  FindById", COLOR_PRIMARY, [this]() { RunFindById(); }));
    firstRow.Add(MakeButton("2  GetAnnotatedViews", COLOR_SUCCESS, [this]() { RunGetAnnotatedViews(); }));
    panel.Add(firstRow);

    StackLayout secondRow = StackLayout::New(StackOrientation::HORIZONTAL);
    secondRow.SetRequestedWidth(MATCH_PARENT);
    secondRow.SetRequestedHeight(50.0f);
    secondRow.SetSpacing(10.0f);
    secondRow.Add(MakeButton("3  GetFocusedView", 0x7A5DE8u, [this]() { RunGetFocusedView(); }));
    secondRow.Add(MakeButton("4  ToPresentation", 0x1885A8u, [this]() { RunToPresentation(); }));
    panel.Add(secondRow);

    StackLayout response = StackLayout::New(StackOrientation::VERTICAL);
    response.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    response.SetPadding(Extents(16, 16, 13, 13));
    response.SetSpacing(8.0f);
    response.SetBackgroundColor(UiColor(COLOR_CONSOLE));
    response.SetCornerRadius(14.0f);

    mResponseStatus = MakeLabel("READY  /  choose an action", 12.0f, 0x7EE2B8u, 24.0f);
    response.Add(mResponseStatus);

    mResponse = MakeLabel("The response payload will appear here.", 13.0f, COLOR_CONSOLE_TEXT);
    mResponse.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mResponse.SetMultiLine(true);
    mResponse.SetVerticalTextAlignment(Text::Alignment::START);
    response.Add(mResponse);
    panel.Add(response);

    return panel;
  }

  View CreateFooter()
  {
    StackLayout footer = StackLayout::New(StackOrientation::HORIZONTAL);
    footer.SetRequestedWidth(MATCH_PARENT);
    footer.SetRequestedHeight(34.0f);
    footer.SetSpacing(12.0f);

    mActivity = MakeLabel("Ready", 12.0f, COLOR_MUTED, 34.0f);
    mActivity.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    footer.Add(mActivity);

    Label shortcuts = MakeLabel("A annotation   E enabled   1-4 actions   R reset   Esc quit", 12.0f, COLOR_MUTED, 34.0f);
    shortcuts.SetRequestedWidth(470.0f);
    shortcuts.SetHorizontalTextAlignment(Text::Alignment::END);
    footer.Add(shortcuts);
    return footer;
  }

  InteractiveView MakeButton(const std::string& text, uint32_t color, std::function<void()> callback)
  {
    InteractiveView button = InteractiveView::New();
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    button.SetBackgroundColor(UiColor(color));
    button.SetCornerRadius(10.0f);
    button.SetFocusable(true);

    Label label = MakeLabel(text, 12.0f, 0xFFFFFFu, MATCH_PARENT);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    button.Add(label);
    button.ConnectClickedSignal(this, [callback](View, InputEvent) { callback(); });
    return button;
  }

  void AddEntityCard(StackLayout&       panel,
                     const std::string& title,
                     const std::string& entityId,
                     const std::string& entityType,
                     const std::string& entityInfo,
                     uint32_t           accentColor)
  {
    const size_t index = mEntities.size();

    EntityCard entity;
    entity.title       = title;
    entity.entityId    = entityId;
    entity.entityType  = entityType;
    entity.entityInfo  = entityInfo;
    entity.accentColor = accentColor;
    entity.card        = InteractiveView::New();
    entity.card.SetRequestedWidth(MATCH_PARENT);
    entity.card.SetRequestedHeight(112.0f);
    entity.card.SetPadding(Extents(16, 16, 11, 11));
    entity.card.SetBackgroundColor(UiColor(COLOR_UNSELECTED_CARD));
    entity.card.SetCornerRadius(13.0f);
    entity.card.SetFocusable(true);
    entity.card.SetAnnotation(entityId.c_str(), entityType.c_str(), entityInfo.c_str());

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(MATCH_PARENT);
    content.SetSpacing(2.0f);

    StackLayout titleRow = StackLayout::New(StackOrientation::HORIZONTAL);
    titleRow.SetRequestedWidth(MATCH_PARENT);
    titleRow.SetRequestedHeight(30.0f);
    titleRow.SetSpacing(10.0f);

    View accent = View::New();
    accent.SetRequestedWidth(7.0f);
    accent.SetRequestedHeight(26.0f);
    accent.SetBackgroundColor(UiColor(accentColor));
    accent.SetCornerRadius(3.5f);
    titleRow.Add(accent);

    Label titleLabel = MakeLabel(title, 17.0f, COLOR_INK, 30.0f);
    titleLabel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    titleRow.Add(titleLabel);

    entity.state = MakeLabel("ANNOTATED", 11.0f, COLOR_SUCCESS, 30.0f);
    entity.state.SetRequestedWidth(150.0f);
    entity.state.SetHorizontalTextAlignment(Text::Alignment::END);
    titleRow.Add(entity.state);
    content.Add(titleRow);

    entity.metadata = MakeLabel("", 12.0f, COLOR_MUTED, 25.0f);
    content.Add(entity.metadata);
    content.Add(MakeLabel(entityType, 11.0f, 0x8791A6u, 22.0f));

    entity.card.Add(content);
    entity.card.ConnectClickedSignal(this, [this, index](View, InputEvent) { SelectEntity(index); });
    panel.Add(entity.card);
    mEntities.push_back(entity);
  }

  void SelectEntity(size_t index)
  {
    if(index >= mEntities.size())
    {
      return;
    }
    mSelectedIndex = index;
    FocusManager::Get().SetCurrentFocusView(mEntities[index].card);
    SetActivity("Selected " + mEntities[index].title + " (Actor " + std::to_string(mEntities[index].card.GetId()) + ")");
    RefreshCards();
  }

  void RefreshCards()
  {
    const View focusedView = FocusManager::Get().GetCurrentFocusView();
    for(size_t index = 0u; index < mEntities.size(); ++index)
    {
      EntityCard& entity = mEntities[index];
      Dali::String annotationId;
      Dali::String annotationType;
      Dali::String annotationInfo;
      const bool annotated = entity.card.GetAnnotation(annotationId, annotationType, annotationInfo);
      const bool enabled   = entity.card.GetProperty<bool>(Actor::Property::ENABLED);
      const bool focused   = focusedView == entity.card;

      entity.card.SetBackgroundColor(UiColor(index == mSelectedIndex ? COLOR_SELECTED_CARD : COLOR_UNSELECTED_CARD));
      std::string state = annotated ? "ANNOTATED" : "NO ANNOTATION";
      if(focused)
      {
        state += "  •  FOCUSED";
      }
      if(!enabled)
      {
        state += "  •  DISABLED";
      }
      entity.state.SetText(state.c_str());
      entity.state.SetTextColor(UiColor(annotated ? (enabled ? COLOR_SUCCESS : COLOR_WARNING) : COLOR_DANGER));

      const std::string metadata = "Actor " + std::to_string(entity.card.GetId()) + "   |   " +
                                   (annotated ? std::string(annotationId.CStr()) : std::string("—"));
      entity.metadata.SetText(metadata.c_str());
    }
  }

  void ToggleAnnotation()
  {
    EntityCard& entity = mEntities[mSelectedIndex];
    Dali::String annotationId;
    Dali::String annotationType;
    Dali::String annotationInfo;
    if(entity.card.GetAnnotation(annotationId, annotationType, annotationInfo))
    {
      entity.card.ClearAnnotation();
      SetActivity("Cleared annotation from " + entity.title);
    }
    else
    {
      entity.card.SetAnnotation(entity.entityId.c_str(), entity.entityType.c_str(), entity.entityInfo.c_str());
      SetActivity("Restored annotation on " + entity.title);
    }
    RefreshCards();
    RunGetAnnotatedViews();
  }

  void ToggleEnabled()
  {
    EntityCard& entity  = mEntities[mSelectedIndex];
    const bool  enabled = entity.card.GetProperty<bool>(Actor::Property::ENABLED);
    entity.card.SetProperty(Actor::Property::ENABLED, !enabled);
    SetActivity(entity.title + (enabled ? " disabled" : " enabled"));
    RefreshCards();
  }

  void ResetEntities()
  {
    for(auto& entity : mEntities)
    {
      entity.card.SetAnnotation(entity.entityId.c_str(), entity.entityType.c_str(), entity.entityInfo.c_str());
      entity.card.SetProperty(Actor::Property::ENABLED, true);
      entity.card.SetProperty(Actor::Property::VISIBLE, true);
    }
    SelectEntity(0u);
    SetActivity("Restored the sample data");
    RunGetAnnotatedViews();
  }

  void RunFindById()
  {
    const uint32_t requestedId = mEntities[mSelectedIndex].card.GetId();
    auto found = std::find_if(mEntities.begin(), mEntities.end(), [requestedId](const EntityCard& entity) {
      return entity.card.GetId() == requestedId;
    });

    if(found == mEntities.end())
    {
      SetResponse("FindById", false, "{\n  \"status\": \"View not found\"\n}");
      return;
    }

    SetResponse("FindById", true, BuildPresentation(*found));
  }

  void RunGetAnnotatedViews()
  {
    std::ostringstream result;
    result << "{\n  \"views\": [";
    bool first = true;
    for(const auto& entity : mEntities)
    {
      Dali::String annotationId;
      Dali::String annotationType;
      Dali::String annotationInfo;
      if(!entity.card.GetAnnotation(annotationId, annotationType, annotationInfo) ||
         !entity.card.GetProperty<bool>(Actor::Property::VISIBLE))
      {
        continue;
      }
      result << (first ? "\n" : ",\n")
             << "    { \"id\": \"" << entity.card.GetId()
             << "\", \"entityId\": \"" << JsonEscape(annotationId.CStr()) << "\" }";
      first = false;
    }
    if(!first)
    {
      result << "\n  ";
    }
    result << "]\n}";
    SetResponse("GetAnnotatedViews", true, result.str());
  }

  void RunGetFocusedView()
  {
    const View focusedView = FocusManager::Get().GetCurrentFocusView();
    auto found = std::find_if(mEntities.begin(), mEntities.end(), [&focusedView](const EntityCard& entity) {
      return entity.card == focusedView;
    });
    if(found == mEntities.end())
    {
      SetResponse("GetFocusedView", false, "{\n  \"status\": \"No focused view\"\n}");
      return;
    }
    SetResponse("GetFocusedView", true, BuildPresentation(*found));
  }

  void RunToPresentation()
  {
    SetResponse("ToPresentation", true, BuildPresentation(mEntities[mSelectedIndex]));
  }

  std::string BuildPresentation(const EntityCard& entity) const
  {
    Dali::String annotationId;
    Dali::String annotationType;
    Dali::String annotationInfo;
    const bool   annotated = entity.card.GetAnnotation(annotationId, annotationType, annotationInfo);
    const Vector2 screenPosition = entity.card.GetProperty<Vector2>(Actor::Property::SCREEN_POSITION);
    const Vector3 size           = entity.card.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
    const bool focused = FocusManager::Get().GetCurrentFocusView() == entity.card;

    std::ostringstream json;
    json.imbue(std::locale::classic());
    json << std::fixed << std::setprecision(0)
         << "{\n"
         << "  \"id\": \"" << entity.card.GetId() << "\",\n"
         << "  \"type\": \"" << JsonEscape(entity.card.GetTypeName().CStr()) << "\",\n"
         << "  \"screenBounds\": { \"x\": " << screenPosition.x << ", \"y\": " << screenPosition.y
         << ", \"width\": " << size.width << ", \"height\": " << size.height << " },\n"
         << "  \"isFocused\": " << (focused ? "true" : "false") << ",\n"
         << "  \"isEnabled\": " << (entity.card.GetProperty<bool>(Actor::Property::ENABLED) ? "true" : "false") << ",\n"
         << "  \"annotation\": {\n"
         << "    \"entityId\": \"" << (annotated ? JsonEscape(annotationId.CStr()) : "") << "\",\n"
         << "    \"entityType\": \"" << (annotated ? JsonEscape(annotationType.CStr()) : "") << "\",\n"
         << "    \"entityInfo\": \"" << (annotated ? JsonEscape(annotationInfo.CStr()) : "") << "\"\n"
         << "  }\n"
         << "}";
    return json.str();
  }

  void SetResponse(const std::string& action, bool success, const std::string& payload)
  {
    const std::string status = std::string(success ? "200 OK  /  " : "404  /  ") + action + "  /  LOCAL";
    mResponseStatus.SetText(status.c_str());
    mResponseStatus.SetTextColor(UiColor(success ? 0x7EE2B8u : 0xFF9BAAu));
    mResponse.SetText(payload.c_str());
    SetActivity("Invoked " + action + " for the local simulator");
  }

  void SetActivity(const std::string& text)
  {
    mActivity.SetText(("●  " + text).c_str());
  }

  void OnFocusChanged(View, View current)
  {
    auto found = std::find_if(mEntities.begin(), mEntities.end(), [&current](const EntityCard& entity) {
      return entity.card == current;
    });
    if(found != mEntities.end())
    {
      mSelectedIndex = static_cast<size_t>(std::distance(mEntities.begin(), found));
    }
    RefreshCards();
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    const Dali::String& keyName = event.GetKeyName();
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(IsKey(event, Dali::DALI_KEY_CURSOR_UP))
    {
      SelectEntity((mSelectedIndex + mEntities.size() - 1u) % mEntities.size());
    }
    else if(IsKey(event, Dali::DALI_KEY_CURSOR_DOWN))
    {
      SelectEntity((mSelectedIndex + 1u) % mEntities.size());
    }
    else if(keyName == "1")
    {
      RunFindById();
    }
    else if(keyName == "2")
    {
      RunGetAnnotatedViews();
    }
    else if(keyName == "3")
    {
      RunGetFocusedView();
    }
    else if(keyName == "4")
    {
      RunToPresentation();
    }
    else if(keyName == "a" || keyName == "A")
    {
      ToggleAnnotation();
    }
    else if(keyName == "e" || keyName == "E")
    {
      ToggleEnabled();
    }
    else if(keyName == "r" || keyName == "R")
    {
      ResetEntities();
    }
  }

private:
  Application&            mApplication;
  std::vector<EntityCard> mEntities;
  size_t                  mSelectedIndex{0u};
  Label                   mResponseStatus;
  Label                   mResponse;
  Label                   mActivity;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  AppEntityAnnotationExample example(application);
  application.MainLoop();
  return 0;
}
