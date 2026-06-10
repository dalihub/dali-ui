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

constexpr float STACK_SPACING  = 6.0f;
constexpr float STACK_PADDING  = 12.0f;
constexpr float BUTTON_HEIGHT  = 36.0f;
constexpr float BUTTON_SPACING = 4.0f;

const char* GetStateName(InputMethodContext::State state)
{
  switch(state)
  {
    case InputMethodContext::State::SHOW:
      return "SHOW";
    case InputMethodContext::State::HIDE:
      return "HIDE";
    case InputMethodContext::State::WILL_SHOW:
      return "WILL_SHOW";
  }
  return "UNKNOWN";
}

const char* GetKeyboardTypeName(InputMethodContext::KeyboardType type)
{
  switch(type)
  {
    case InputMethodContext::KeyboardType::SOFTWARE_KEYBOARD:
      return "SOFTWARE";
    case InputMethodContext::KeyboardType::HARDWARE_KEYBOARD:
      return "HARDWARE";
  }
  return "UNKNOWN";
}

const char* GetAlignName(InputMethodContext::InputPanelAlign align)
{
  switch(align)
  {
    case InputMethodContext::InputPanelAlign::TOP_LEFT:
      return "TOP_LEFT";
    case InputMethodContext::InputPanelAlign::TOP_CENTER:
      return "TOP_CENTER";
    case InputMethodContext::InputPanelAlign::TOP_RIGHT:
      return "TOP_RIGHT";
    case InputMethodContext::InputPanelAlign::MIDDLE_LEFT:
      return "MIDDLE_LEFT";
    case InputMethodContext::InputPanelAlign::MIDDLE_CENTER:
      return "MIDDLE_CENTER";
    case InputMethodContext::InputPanelAlign::MIDDLE_RIGHT:
      return "MIDDLE_RIGHT";
    case InputMethodContext::InputPanelAlign::BOTTOM_LEFT:
      return "BOTTOM_LEFT";
    case InputMethodContext::InputPanelAlign::BOTTOM_CENTER:
      return "BOTTOM_CENTER";
    case InputMethodContext::InputPanelAlign::BOTTOM_RIGHT:
      return "BOTTOM_RIGHT";
  }
  return "UNKNOWN";
}

const char* GetPanelLayoutName(Dali::InputMethod::PanelLayout layout)
{
  switch(layout)
  {
    case Dali::InputMethod::PanelLayout::NORMAL:
      return "NORMAL";
    case Dali::InputMethod::PanelLayout::NUMBER:
      return "NUMBER";
    case Dali::InputMethod::PanelLayout::NUMBER_ONLY:
      return "NUMBER_ONLY";
    case Dali::InputMethod::PanelLayout::PASSWORD:
      return "PASSWORD";
    case Dali::InputMethod::PanelLayout::EMAIL:
      return "EMAIL";
    case Dali::InputMethod::PanelLayout::URL:
      return "URL";
    case Dali::InputMethod::PanelLayout::PHONENUMBER:
      return "PHONENUMBER";
    case Dali::InputMethod::PanelLayout::IP:
      return "IP";
    case Dali::InputMethod::PanelLayout::MONTH:
      return "MONTH";
    case Dali::InputMethod::PanelLayout::DATE_TIME:
      return "DATE_TIME";
    case Dali::InputMethod::PanelLayout::EMOTICON:
      return "EMOTICON";
    case Dali::InputMethod::PanelLayout::VOICE:
      return "VOICE";
    default:
      break;
  }
  return "UNKNOWN";
}

const char* GetReturnKeyName(Dali::InputMethod::ReturnKeyType action)
{
  switch(action)
  {
    case Dali::InputMethod::ReturnKeyType::DEFAULT:
      return "DEFAULT";
    case Dali::InputMethod::ReturnKeyType::DONE:
      return "DONE";
    case Dali::InputMethod::ReturnKeyType::GO:
      return "GO";
    case Dali::InputMethod::ReturnKeyType::JOIN:
      return "JOIN";
    case Dali::InputMethod::ReturnKeyType::LOGIN:
      return "LOGIN";
    case Dali::InputMethod::ReturnKeyType::NEXT:
      return "NEXT";
    case Dali::InputMethod::ReturnKeyType::SEARCH:
      return "SEARCH";
    case Dali::InputMethod::ReturnKeyType::SEND:
      return "SEND";
    case Dali::InputMethod::ReturnKeyType::SIGNIN:
      return "SIGNIN";
  }
  return "UNKNOWN";
}

const char* GetAutoCapitalName(Dali::InputMethod::AutoCapitalType autoCapital)
{
  switch(autoCapital)
  {
    case Dali::InputMethod::AutoCapitalType::NONE:
      return "NONE";
    case Dali::InputMethod::AutoCapitalType::SENTENCE:
      return "SENTENCE";
    case Dali::InputMethod::AutoCapitalType::WORD:
      return "WORD";
    case Dali::InputMethod::AutoCapitalType::ALL_CHARACTER:
      return "ALL_CHARACTER";
  }
  return "UNKNOWN";
}

const char* GetPanelLayoutVariationName(Dali::InputMethod::PanelLayoutVariation variation)
{
  switch(variation)
  {
    case Dali::InputMethod::PanelLayoutVariation::NORMAL_NORMAL:
      return "NORMAL_NORMAL";
    case Dali::InputMethod::PanelLayoutVariation::NORMAL_WITH_FILENAME:
      return "NORMAL_WITH_FILENAME";
    case Dali::InputMethod::PanelLayoutVariation::NORMAL_WITH_PERSON_NAME:
      return "NORMAL_WITH_PERSON_NAME";
    case Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_NORMAL:
      return "NUMBER_ONLY_NORMAL";
    case Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_WITH_SIGNED:
      return "NUMBER_ONLY_WITH_SIGNED";
    case Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_WITH_DECIMAL:
      return "NUMBER_ONLY_WITH_DECIMAL";
    case Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_WITH_SIGNED_AND_DECIMAL:
      return "NUMBER_ONLY_WITH_SIGNED_AND_DECIMAL";
    case Dali::InputMethod::PanelLayoutVariation::PASSWORD_NORMAL:
      return "PASSWORD_NORMAL";
    case Dali::InputMethod::PanelLayoutVariation::PASSWORD_WITH_NUMBER_ONLY:
      return "PASSWORD_WITH_NUMBER_ONLY";
  }
  return "UNKNOWN";
}

Dali::InputMethod::PanelLayout GetPanelLayoutForVariation(Dali::InputMethod::PanelLayoutVariation variation)
{
  switch(variation)
  {
    case Dali::InputMethod::PanelLayoutVariation::NORMAL_NORMAL:
    case Dali::InputMethod::PanelLayoutVariation::NORMAL_WITH_FILENAME:
    case Dali::InputMethod::PanelLayoutVariation::NORMAL_WITH_PERSON_NAME:
      return Dali::InputMethod::PanelLayout::NORMAL;

    case Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_NORMAL:
    case Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_WITH_SIGNED:
    case Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_WITH_DECIMAL:
    case Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_WITH_SIGNED_AND_DECIMAL:
      return Dali::InputMethod::PanelLayout::NUMBER_ONLY;

    case Dali::InputMethod::PanelLayoutVariation::PASSWORD_NORMAL:
    case Dali::InputMethod::PanelLayoutVariation::PASSWORD_WITH_NUMBER_ONLY:
      return Dali::InputMethod::PanelLayout::PASSWORD;
  }

  return Dali::InputMethod::PanelLayout::NORMAL;
}

Label CreateButton(const char* text, uint32_t bgColor)
{
  Label button = Label::New(text);
  button.SetFontSize(11.0f);
  button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  button.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.SetBackgroundColor(UiColor(bgColor));
  button.SetRequestedWidth(0.0f);
  button.SetRequestedHeight(BUTTON_HEIGHT);
  button.SetPadding(Extents(4, 4, 4, 4));
  button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  return button;
}

View CreateButtonRow(std::initializer_list<Label> buttons)
{
  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(WRAP_CONTENT);
  row.SetSpacing(BUTTON_SPACING);

  for(auto& btn : buttons)
  {
    row.Add(btn);
  }
  return row;
}

Label CreateSectionTitle(const char* text)
{
  Label title = Label::New(text);
  title.SetFontSize(12.0f);
  title.SetTextColor(UiColor(0xFFFFFF));
  title.SetBackgroundColor(UiColor(0x34495E));
  title.SetRequestedWidth(MATCH_PARENT);
  title.SetRequestedHeight(28);
  title.SetPadding(Extents(8, 8, 4, 4));
  return title;
}

} // namespace

class InputMethodContextExample : public ConnectionTracker
{
public:
  explicit InputMethodContextExample(Application& application)
  : mApplication(application),
    mFieldAutoShowEnabled(true),
    mEditorAutoShowEnabled(true),
    mRestoreAfterFocusLost(true),
    mFieldReturnKeyEnabled(true),
    mEditorReturnKeyEnabled(true),
    mPredictionEnabled(true),
    mFullScreenEnabled(false),
    mLastFocusedInputIsEditor(false)
  {
    mApplication.InitSignal().Connect(this, &InputMethodContextExample::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xF5F5F5));

    // Title label
    Label titleLabel = Label::New("InputMethodContext Test");
    titleLabel.SetFontSize(14.0f);
    titleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    titleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    titleLabel.SetTextColor(UiColor(0xFFFFFF));
    titleLabel.SetBackgroundColor(UiColor(0x2C3E50));
    titleLabel.SetRequestedWidth(MATCH_PARENT);
    titleLabel.SetRequestedHeight(32);

    // Status label (multi-line, focusable for IME focus testing)
    mStatusLabel = Label::New();
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(120);
    mStatusLabel.SetFontSize(10.0f);
    mStatusLabel.SetMultiLine(true);
    mStatusLabel.SetFocusable(true);
    mStatusLabel.SetBackgroundColor(UiColor(0xE8E8E8));
    mStatusLabel.SetPadding(Extents(8, 8, 8, 8));

    // InputField - single line context test
    mInputField = InputField::New();
    mInputField.SetPlaceholder("InputField - type here");
    mInputField.SetFontSize(18.0f);
    mInputField.SetRequestedWidth(0.0f);
    mInputField.SetRequestedHeight(48);
    mInputField.SetBackgroundColor(UiColor(0xFFFFFF));
    mInputField.SetTextColor(UiColor(0x222222));
    mInputField.SetPadding(Extents(12, 12, 8, 8));
    mInputField.SetFocusable(true);
    mInputField.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    // InputEditor - context test
    mInputEditor = InputEditor::New();
    mInputEditor.SetPlaceholder("InputEditor - type here");
    mInputEditor.SetFontSize(18.0f);
    mInputEditor.SetRequestedWidth(0.0f);
    mInputEditor.SetRequestedHeight(48);
    mInputEditor.SetBackgroundColor(UiColor(0xFFFFFF));
    mInputEditor.SetTextColor(UiColor(0x222222));
    mInputEditor.SetPadding(Extents(12, 12, 8, 8));
    mInputEditor.SetFocusable(true);
    mInputEditor.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    // Get InputMethodContext from each control
    mFieldContext = mInputField.GetInputMethodContext();
    mEditorContext = mInputEditor.GetInputMethodContext();

    // Connect IME signals for Field context
    if(mFieldContext)
    {
      mFieldContext.ActivatedSignal().Connect(this, &InputMethodContextExample::OnFieldActivated);
      mFieldContext.StatusChangedSignal().Connect(this, &InputMethodContextExample::OnFieldStatusChanged);
      mFieldContext.ResizedSignal().Connect(this, &InputMethodContextExample::OnFieldResized);
      mFieldContext.LanguageChangedSignal().Connect(this, &InputMethodContextExample::OnFieldLanguageChanged);
      mFieldContext.KeyboardTypeChangedSignal().Connect(this, &InputMethodContextExample::OnFieldKeyboardTypeChanged);
      mFieldContext.PrivateCommandReceivedSignal().Connect(this, &InputMethodContextExample::OnFieldPrivateCommandReceived);
    }

    // Connect IME signals for Editor context
    if(mEditorContext)
    {
      mEditorContext.ActivatedSignal().Connect(this, &InputMethodContextExample::OnEditorActivated);
      mEditorContext.StatusChangedSignal().Connect(this, &InputMethodContextExample::OnEditorStatusChanged);
      mEditorContext.ResizedSignal().Connect(this, &InputMethodContextExample::OnEditorResized);
      mEditorContext.LanguageChangedSignal().Connect(this, &InputMethodContextExample::OnEditorLanguageChanged);
      mEditorContext.KeyboardTypeChangedSignal().Connect(this, &InputMethodContextExample::OnEditorKeyboardTypeChanged);
      mEditorContext.PrivateCommandReceivedSignal().Connect(this, &InputMethodContextExample::OnEditorPrivateCommandReceived);
    }
    FocusManager::Get().FocusChangedSignal().Connect(this, &InputMethodContextExample::OnFocusChanged);

    // --- Create buttons ---

    // Target Context
    Label btnUseField = CreateButton("Use Field", 0x3498DB);
    Label btnUseEditor = CreateButton("Use Editor", 0x2ECC71);
    View targetRow = CreateButtonRow({btnUseField, btnUseEditor});

    // Panel Visibility
    Label btnShowPanel = CreateButton("Show", 0x3498DB);
    Label btnHidePanel = CreateButton("Hide", 0xE74C3C);
    View panelRow1 = CreateButtonRow({btnShowPanel, btnHidePanel});

    Label btnGetState = CreateButton("Get State", 0x9B59B6);
    Label btnGetArea = CreateButton("Get Area", 0x1ABC9C);
    View panelRow2 = CreateButtonRow({btnGetState, btnGetArea});

    // Auto / Restore / Return
    Label btnAutoShow = CreateButton("AutoShow", 0x2980B9);
    Label btnRestore = CreateButton("Restore", 0x27AE60);
    Label btnReturn = CreateButton("ReturnKey", 0xE67E22);
    View autoRow = CreateButtonRow({btnAutoShow, btnRestore, btnReturn});

    // Prediction / Fullscreen
    Label btnPrediction = CreateButton("Prediction", 0x8E44AD);
    Label btnFullscreen = CreateButton("Fullscreen", 0x16A085);
    View modeRow1 = CreateButtonRow({btnPrediction, btnFullscreen});

    // Panel Data
    Label btnSetData = CreateButton("Set Data", 0xD35400);
    Label btnGetData = CreateButton("Get Data", 0x7F8C8D);
    View dataRow = CreateButtonRow({btnSetData, btnGetData});

    // Panel Position
    Label btnPos00 = CreateButton("Pos 0,0", 0x95A5A6);
    Label btnPos100 = CreateButton("Pos 100,100", 0x7F8C8D);
    View posRow = CreateButtonRow({btnPos00, btnPos100});

    Label btnAlignTL = CreateButton("Align TL", 0x3498DB);
    Label btnAlignBC = CreateButton("Align BC", 0x2ECC71);
    Label btnAlignMC = CreateButton("Align MC", 0xE67E22);
    View alignRow = CreateButtonRow({btnAlignTL, btnAlignBC, btnAlignMC});

    // Input Panel Options
    Label btnLayoutNormal   = CreateButton("Normal", 0x3498DB);
    Label btnLayoutNumber   = CreateButton("Number", 0x2ECC71);
    Label btnLayoutEmail    = CreateButton("Email", 0xE67E22);
    Label btnLayoutUrl      = CreateButton("URL", 0x9B59B6);
    View  layoutRow1        = CreateButtonRow({btnLayoutNormal, btnLayoutNumber, btnLayoutEmail, btnLayoutUrl});

    Label btnLayoutPhone    = CreateButton("Phone", 0x3498DB);
    Label btnLayoutIp       = CreateButton("IP", 0x2ECC71);
    Label btnLayoutMonth    = CreateButton("Month", 0xE67E22);
    Label btnLayoutNumOnly  = CreateButton("NumOnly", 0x9B59B6);
    View  layoutRow2        = CreateButtonRow({btnLayoutPhone, btnLayoutIp, btnLayoutMonth, btnLayoutNumOnly});

    Label btnLayoutPassword = CreateButton("Password", 0xE67E22);
    Label btnLayoutDateTime = CreateButton("DateTime", 0x9B59B6);
    View  layoutRow3        = CreateButtonRow({btnLayoutPassword, btnLayoutDateTime});

    Label btnLayoutEmoticon = CreateButton("Emoticon", 0x3498DB);
    Label btnLayoutVoice    = CreateButton("Voice", 0x2ECC71);
    View  layoutRow4        = CreateButtonRow({btnLayoutEmoticon, btnLayoutVoice});

    Label btnReturnDefault = CreateButton("Ret Default", 0x3498DB);
    Label btnReturnDone    = CreateButton("Ret Done", 0x2ECC71);
    Label btnReturnGo      = CreateButton("Ret Go", 0xE67E22);
    Label btnReturnJoin    = CreateButton("Ret Join", 0x9B59B6);
    View  returnKeyRow1    = CreateButtonRow({btnReturnDefault, btnReturnDone, btnReturnGo, btnReturnJoin});

    Label btnReturnLogin  = CreateButton("Ret Login", 0x3498DB);
    Label btnReturnNext   = CreateButton("Ret Next", 0x2ECC71);
    Label btnReturnSearch = CreateButton("Ret Search", 0xE67E22);
    Label btnReturnSend   = CreateButton("Ret Send", 0x9B59B6);
    View  returnKeyRow2   = CreateButtonRow({btnReturnLogin, btnReturnNext, btnReturnSearch, btnReturnSend});

    Label btnReturnSignIn = CreateButton("Ret SignIn", 0x3498DB);
    View  returnKeyRow3   = CreateButtonRow({btnReturnSignIn});

    Label btnAutoCapNone = CreateButton("AC None", 0x3498DB);
    Label btnAutoCapSentence = CreateButton("AC Sentence", 0x2ECC71);
    Label btnAutoCapWord = CreateButton("AC Word", 0xE67E22);
    Label btnAutoCapAll = CreateButton("AC All", 0x9B59B6);
    View autoCapitalRow = CreateButtonRow({btnAutoCapNone, btnAutoCapSentence, btnAutoCapWord, btnAutoCapAll});

    Label btnVarNormal     = CreateButton("Normal", 0x3498DB);
    Label btnVarFilename   = CreateButton("Filename", 0x2ECC71);
    Label btnVarPersonName = CreateButton("Person", 0xE67E22);
    Label btnVarNumOnly    = CreateButton("NumOnly", 0x9B59B6);
    View  variationRow1    = CreateButtonRow({btnVarNormal, btnVarFilename, btnVarPersonName, btnVarNumOnly});

    Label btnVarSigned     = CreateButton("Signed", 0x3498DB);
    Label btnVarDecimal    = CreateButton("Decimal", 0x2ECC71);
    Label btnVarSignDec    = CreateButton("Sign+Dec", 0xE67E22);
    Label btnVarPassword   = CreateButton("Password", 0x9B59B6);
    View  variationRow2    = CreateButtonRow({btnVarSigned, btnVarDecimal, btnVarSignDec, btnVarPassword});

    Label btnVarPwdNum     = CreateButton("PwdNum", 0x3498DB);
    View  variationRow3    = CreateButtonRow({btnVarPwdNum});

    // Query
    Label btnKeyboardType = CreateButton("KB Type", 0x34495E);
    Label btnLocale = CreateButton("Locale", 0x2C3E50);
    Label btnRefresh = CreateButton("Refresh", 0x95A5A6);
    View queryRow = CreateButtonRow({btnKeyboardType, btnLocale, btnRefresh});

    // --- Build layout ---

    StackLayout inputRow = StackLayout::New(StackOrientation::HORIZONTAL);
    inputRow.SetRequestedWidth(MATCH_PARENT);
    inputRow.SetRequestedHeight(WRAP_CONTENT);
    inputRow.SetSpacing(STACK_SPACING);
    inputRow.AddChildren({
      mInputField,
      mInputEditor,
    });

    // Fixed header area
    StackLayout fixedHeader = StackLayout::New(StackOrientation::VERTICAL);
    fixedHeader.SetSpacing(STACK_SPACING);
    fixedHeader.SetRequestedWidth(MATCH_PARENT);
    fixedHeader.SetRequestedHeight(WRAP_CONTENT);
    fixedHeader.AddChildren({
      titleLabel,
      mStatusLabel,
      inputRow,
    });

    // Scrollable content area
    StackLayout scrollContent = StackLayout::New(StackOrientation::VERTICAL);
    scrollContent.SetSpacing(STACK_SPACING);
    scrollContent.SetRequestedWidth(MATCH_PARENT);
    scrollContent.SetRequestedHeight(WRAP_CONTENT);
    scrollContent.SetPadding(Extents(0, 0, 0, STACK_PADDING));
    scrollContent.AddChildren({
      CreateSectionTitle("Target Context"),
      targetRow,

      CreateSectionTitle("Panel Visibility"),
      panelRow1,
      panelRow2,

      CreateSectionTitle("Auto / Restore / Return"),
      autoRow,

      CreateSectionTitle("Prediction / Fullscreen"),
      modeRow1,

      CreateSectionTitle("Panel Data"),
      dataRow,

      CreateSectionTitle("Panel Position"),
      posRow,
      alignRow,

      CreateSectionTitle("Panel Layout"),
      layoutRow1,
      layoutRow2,
      layoutRow3,
      layoutRow4,

      CreateSectionTitle("Return Key"),
      returnKeyRow1,
      returnKeyRow2,
      returnKeyRow3,

      CreateSectionTitle("Auto Capital"),
      autoCapitalRow,

      CreateSectionTitle("Layout Variation"),
      variationRow1,
      variationRow2,
      variationRow3,

      CreateSectionTitle("Query"),
      queryRow,
    });

    // ScrollView for options
    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetRequestedHeight(0.0f);
    scrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    scrollView.SetContent(scrollContent);

    // Root layout
    StackLayout rootLayout = StackLayout::New(StackOrientation::VERTICAL);
    rootLayout.SetSpacing(STACK_SPACING);
    rootLayout.SetRequestedWidth(MATCH_PARENT);
    rootLayout.SetRequestedHeight(MATCH_PARENT);
    rootLayout.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));
    rootLayout.AddChildren({
      fixedHeader,
      scrollView,
    });

    window.Add(rootLayout);

    // --- Connect button touch signals ---

    // Target Context
    btnUseField.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonUseFieldTouched);
    btnUseEditor.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonUseEditorTouched);

    // Panel Visibility
    btnShowPanel.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonShowPanelTouched);
    btnHidePanel.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonHidePanelTouched);
    btnGetState.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonGetStateTouched);
    btnGetArea.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonGetAreaTouched);

    // Auto / Restore / Return
    btnAutoShow.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonAutoShowTouched);
    btnRestore.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonRestoreTouched);
    btnReturn.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonReturnKeyTouched);

    // Prediction / Fullscreen
    btnPrediction.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonPredictionTouched);
    btnFullscreen.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonFullscreenTouched);

    // Panel Data
    btnSetData.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonSetDataTouched);
    btnGetData.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonGetDataTouched);

    // Panel Position
    btnPos00.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonPos00Touched);
    btnPos100.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonPos100Touched);
    btnAlignTL.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonAlignTLTouched);
    btnAlignBC.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonAlignBCTouched);
    btnAlignMC.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonAlignMCTouched);

    // Input Panel Options
    ConnectLayoutButton(btnLayoutNormal, Dali::InputMethod::PanelLayout::NORMAL);
    ConnectLayoutButton(btnLayoutNumber, Dali::InputMethod::PanelLayout::NUMBER);
    ConnectLayoutButton(btnLayoutEmail, Dali::InputMethod::PanelLayout::EMAIL);
    ConnectLayoutButton(btnLayoutUrl, Dali::InputMethod::PanelLayout::URL);
    ConnectLayoutButton(btnLayoutPhone, Dali::InputMethod::PanelLayout::PHONENUMBER);
    ConnectLayoutButton(btnLayoutIp, Dali::InputMethod::PanelLayout::IP);
    ConnectLayoutButton(btnLayoutMonth, Dali::InputMethod::PanelLayout::MONTH);
    ConnectLayoutButton(btnLayoutNumOnly, Dali::InputMethod::PanelLayout::NUMBER_ONLY);
    ConnectLayoutButton(btnLayoutPassword, Dali::InputMethod::PanelLayout::PASSWORD);
    ConnectLayoutButton(btnLayoutDateTime, Dali::InputMethod::PanelLayout::DATE_TIME);
    ConnectLayoutButton(btnLayoutEmoticon, Dali::InputMethod::PanelLayout::EMOTICON);
    ConnectLayoutButton(btnLayoutVoice, Dali::InputMethod::PanelLayout::VOICE);
    ConnectReturnKeyButton(btnReturnDefault, Dali::InputMethod::ReturnKeyType::DEFAULT);
    ConnectReturnKeyButton(btnReturnDone, Dali::InputMethod::ReturnKeyType::DONE);
    ConnectReturnKeyButton(btnReturnGo, Dali::InputMethod::ReturnKeyType::GO);
    ConnectReturnKeyButton(btnReturnJoin, Dali::InputMethod::ReturnKeyType::JOIN);
    ConnectReturnKeyButton(btnReturnLogin, Dali::InputMethod::ReturnKeyType::LOGIN);
    ConnectReturnKeyButton(btnReturnNext, Dali::InputMethod::ReturnKeyType::NEXT);
    ConnectReturnKeyButton(btnReturnSearch, Dali::InputMethod::ReturnKeyType::SEARCH);
    ConnectReturnKeyButton(btnReturnSend, Dali::InputMethod::ReturnKeyType::SEND);
    ConnectReturnKeyButton(btnReturnSignIn, Dali::InputMethod::ReturnKeyType::SIGNIN);
    ConnectAutoCapitalButton(btnAutoCapNone, Dali::InputMethod::AutoCapitalType::NONE);
    ConnectAutoCapitalButton(btnAutoCapSentence, Dali::InputMethod::AutoCapitalType::SENTENCE);
    ConnectAutoCapitalButton(btnAutoCapWord, Dali::InputMethod::AutoCapitalType::WORD);
    ConnectAutoCapitalButton(btnAutoCapAll, Dali::InputMethod::AutoCapitalType::ALL_CHARACTER);
    ConnectVariationButton(btnVarNormal, Dali::InputMethod::PanelLayoutVariation::NORMAL_NORMAL);
    ConnectVariationButton(btnVarFilename, Dali::InputMethod::PanelLayoutVariation::NORMAL_WITH_FILENAME);
    ConnectVariationButton(btnVarPersonName, Dali::InputMethod::PanelLayoutVariation::NORMAL_WITH_PERSON_NAME);
    ConnectVariationButton(btnVarNumOnly, Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_NORMAL);
    ConnectVariationButton(btnVarSigned, Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_WITH_SIGNED);
    ConnectVariationButton(btnVarDecimal, Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_WITH_DECIMAL);
    ConnectVariationButton(btnVarSignDec, Dali::InputMethod::PanelLayoutVariation::NUMBER_ONLY_WITH_SIGNED_AND_DECIMAL);
    ConnectVariationButton(btnVarPassword, Dali::InputMethod::PanelLayoutVariation::PASSWORD_NORMAL);
    ConnectVariationButton(btnVarPwdNum, Dali::InputMethod::PanelLayoutVariation::PASSWORD_WITH_NUMBER_ONLY);

    // Query
    btnKeyboardType.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonKeyboardTypeTouched);
    btnLocale.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonLocaleTouched);
    btnRefresh.TouchedSignal().Connect(this, &InputMethodContextExample::OnButtonRefreshTouched);

    // Key event
    window.KeyEventSignal().Connect(this, &InputMethodContextExample::OnKeyEvent);

    FocusInputField();
    UpdateStatus();
  }

  // --- Helper ---

  InputMethodContext GetCurrentContext()
  {
    if(IsEditorTarget())
    {
      return mEditorContext;
    }

    return mFieldContext ? mFieldContext : mEditorContext;
  }

  const char* GetCurrentTargetName()
  {
    return IsEditorTarget() ? "Editor" : "Field";
  }

  bool IsEditorTarget()
  {
    View focusedView = FocusManager::Get().GetCurrentFocusView();
    if(focusedView == mInputEditor)
    {
      return true;
    }

    if(focusedView == mInputField)
    {
      return false;
    }

    return mLastFocusedInputIsEditor;
  }

  bool& GetCurrentAutoShowEnabled()
  {
    if(IsEditorTarget())
    {
      return mEditorAutoShowEnabled;
    }

    return mFieldAutoShowEnabled;
  }

  bool& GetCurrentReturnKeyEnabled()
  {
    if(IsEditorTarget())
    {
      return mEditorReturnKeyEnabled;
    }

    return mFieldReturnKeyEnabled;
  }

  void FocusInputField()
  {
    mLastFocusedInputIsEditor = false;
    FocusManager::Get().SetCurrentFocusView(mInputField);
  }

  void FocusInputEditor()
  {
    mLastFocusedInputIsEditor = true;
    FocusManager::Get().SetCurrentFocusView(mInputEditor);
  }

  void SetPanelLayout(Dali::InputMethod::PanelLayout layout)
  {
    InputMethodContext context = GetCurrentContext();
    if(context)
    {
      context.SetInputPanelLayout(layout);

      Dali::String info = "Layout=";
      info += GetPanelLayoutName(context.GetInputPanelLayout());
      UpdateStatus(info.CStr());
    }
  }

  void SetPanelLayoutVariation(Dali::InputMethod::PanelLayoutVariation variation)
  {
    InputMethodContext context = GetCurrentContext();
    if(context)
    {
      context.SetInputPanelLayout(GetPanelLayoutForVariation(variation));
      context.SetInputPanelLayoutVariation(variation);

      Dali::String info = "Layout=";
      info += GetPanelLayoutName(context.GetInputPanelLayout());
      info += " Variation=";
      info += GetPanelLayoutVariationName(context.GetInputPanelLayoutVariation());
      UpdateStatus(info.CStr());
    }
  }

  void ConnectLayoutButton(Label button, Dali::InputMethod::PanelLayout layout)
  {
    button.TouchedSignal().Connect(this, [this, layout](Actor, const TouchEvent& touch) -> bool
    {
      if(touch.GetState(0) == PointState::UP)
      {
        SetPanelLayout(layout);
      }
      return true;
    });
  }

  void ConnectVariationButton(Label button, Dali::InputMethod::PanelLayoutVariation variation)
  {
    button.TouchedSignal().Connect(this, [this, variation](Actor, const TouchEvent& touch) -> bool
    {
      if(touch.GetState(0) == PointState::UP)
      {
        SetPanelLayoutVariation(variation);
      }
      return true;
    });
  }

  void ConnectReturnKeyButton(Label button, Dali::InputMethod::ReturnKeyType action)
  {
    button.TouchedSignal().Connect(this, [this, action](Actor, const TouchEvent& touch) -> bool
    {
      if(touch.GetState(0) == PointState::UP)
      {
        InputMethodContext context = GetCurrentContext();
        if(context)
        {
          context.SetInputPanelReturnKeyType(action);
          Dali::String info = "ReturnKey=";
          info += GetReturnKeyName(context.GetInputPanelReturnKeyType());
          UpdateStatus(info.CStr());
        }
      }
      return true;
    });
  }

  void ConnectAutoCapitalButton(Label button, Dali::InputMethod::AutoCapitalType autoCapital)
  {
    button.TouchedSignal().Connect(this, [this, autoCapital](Actor, const TouchEvent& touch) -> bool
    {
      if(touch.GetState(0) == PointState::UP)
      {
        InputMethodContext context = GetCurrentContext();
        if(context)
        {
          context.SetInputPanelAutoCapitalType(autoCapital);
          Dali::String info = "AutoCapital=";
          info += GetAutoCapitalName(context.GetInputPanelAutoCapitalType());
          UpdateStatus(info.CStr());
        }
      }
      return true;
    });
  }

  void SyncLocalStateFromContext()
  {
    InputMethodContext context = GetCurrentContext();
    if(!context)
    {
      return;
    }
    mRestoreAfterFocusLost = context.IsRestoreAfterFocusLostEnabled();
    mPredictionEnabled     = context.IsTextPredictionEnabled();
    mFullScreenEnabled     = context.IsFullScreenModeEnabled();
    GetCurrentReturnKeyEnabled() = context.IsReturnKeyEnabled();
  }

  void UpdateStatus(const char* extra = nullptr)
  {
    InputMethodContext context = GetCurrentContext();

    Dali::String status;
    status += "Target:";
    status += GetCurrentTargetName();

    if(context)
    {
      const auto state        = context.GetInputPanelState();
      const auto keyboardType = context.GetKeyboardType();
      const auto locale       = context.GetInputPanelLanguageLocale();
      const auto layout       = context.GetInputPanelLayout();
      const auto returnKey    = context.GetInputPanelReturnKeyType();
      const auto autoCapital  = context.GetInputPanelAutoCapitalType();
      const auto variation    = context.GetInputPanelLayoutVariation();

      const bool restoreEnabled    = context.IsRestoreAfterFocusLostEnabled();
      const bool predictionEnabled = context.IsTextPredictionEnabled();
      const bool fullScreenEnabled = context.IsFullScreenModeEnabled();
      const bool returnKeyEnabled  = context.IsReturnKeyEnabled();

      Dali::BoundsInteger area = context.GetInputPanelArea();

      status += " State:";
      status += GetStateName(state);
      status += " KB:";
      status += GetKeyboardTypeName(keyboardType);
      status += " Locale:";
      status += locale.CStr();

      status += "\nRestore:";
      status += restoreEnabled ? "ON" : "OFF";
      status += " Prediction:";
      status += predictionEnabled ? "ON" : "OFF";
      status += " Full:";
      status += fullScreenEnabled ? "ON" : "OFF";
      status += " AutoShow(set):";
      status += GetCurrentAutoShowEnabled() ? "ON" : "OFF";
      status += " Return:";
      status += returnKeyEnabled ? "ON" : "OFF";

      status += "\nLayout:";
      status += GetPanelLayoutName(layout);
      status += " ReturnKey:";
      status += GetReturnKeyName(returnKey);
      status += " AutoCap:";
      status += GetAutoCapitalName(autoCapital);

      status += "\nVariation:";
      status += GetPanelLayoutVariationName(variation);

      status += "\nArea:";
      std::string areaStr = std::to_string(area.x) + "," + std::to_string(area.y) + "," + std::to_string(area.width) + "," + std::to_string(area.height);
      status += areaStr.c_str();
    }
    else
    {
      status += " Context:null";
    }

    if(mLastSignal.Size() > 0)
    {
      status += "\nLast Signal: ";
      status += mLastSignal.CStr();
    }

    if(extra)
    {
      status += "\n";
      status += extra;
    }

    mStatusLabel.SetText(status);
  }

  // --- IME Signal callbacks: Field ---

  void OnFocusChanged(View, View current)
  {
    if(current == mInputEditor)
    {
      mLastFocusedInputIsEditor = true;
      SyncLocalStateFromContext();
      UpdateStatus();
    }
    else if(current == mInputField)
    {
      mLastFocusedInputIsEditor = false;
      SyncLocalStateFromContext();
      UpdateStatus();
    }
  }

  void OnFieldActivated(InputMethodContext ctx)
  {
    (void)ctx;
    mLastSignal = "Field Activated";
    UpdateStatus();
  }

  void OnFieldStatusChanged(InputMethodContext ctx, InputMethodContext::State state)
  {
    (void)ctx;
    mLastSignal = "Field StatusChanged ";
    mLastSignal += GetStateName(state);
    UpdateStatus();
  }

  void OnFieldResized(InputMethodContext context)
  {
    mLastSignal = "Field GeometryChanged";

    if(context)
    {
      Dali::BoundsInteger area = context.GetInputPanelArea();
      mLastSignal += " area=";
      mLastSignal += std::to_string(area.x).c_str();
      mLastSignal += ",";
      mLastSignal += std::to_string(area.y).c_str();
      mLastSignal += ",";
      mLastSignal += std::to_string(area.width).c_str();
      mLastSignal += ",";
      mLastSignal += std::to_string(area.height).c_str();
    }

    UpdateStatus();
  }

  void OnFieldLanguageChanged(InputMethodContext context)
  {
    mLastSignal = "Field LanguageChanged";

    if(context)
    {
      Dali::String locale = context.GetInputPanelLanguageLocale();
      if(locale.Size() > 0)
      {
        mLastSignal += " locale=";
        mLastSignal += locale.CStr();
      }
    }

    UpdateStatus();
  }

  void OnFieldKeyboardTypeChanged(InputMethodContext context, InputMethodContext::KeyboardType type)
  {
    (void)context;
    mLastSignal = "Field KBTypeChanged ";
    mLastSignal += GetKeyboardTypeName(type);
    UpdateStatus();
  }

  void OnFieldPrivateCommandReceived(InputMethodContext ctx, const Dali::String& cmd)
  {
    (void)ctx;
    mLastSignal = "Field PrivateCmd ";
    mLastSignal += cmd.CStr();
    UpdateStatus();
  }

  // --- IME Signal callbacks: Editor ---

  void OnEditorActivated(InputMethodContext ctx)
  {
    (void)ctx;
    mLastSignal = "Editor Activated";
    UpdateStatus();
  }

  void OnEditorStatusChanged(InputMethodContext ctx, InputMethodContext::State state)
  {
    (void)ctx;
    mLastSignal = "Editor StatusChanged ";
    mLastSignal += GetStateName(state);
    UpdateStatus();
  }

  void OnEditorResized(InputMethodContext context)
  {
    mLastSignal = "Editor GeometryChanged";

    if(context)
    {
      Dali::BoundsInteger area = context.GetInputPanelArea();
      mLastSignal += " area=";
      mLastSignal += std::to_string(area.x).c_str();
      mLastSignal += ",";
      mLastSignal += std::to_string(area.y).c_str();
      mLastSignal += ",";
      mLastSignal += std::to_string(area.width).c_str();
      mLastSignal += ",";
      mLastSignal += std::to_string(area.height).c_str();
    }

    UpdateStatus();
  }

  void OnEditorLanguageChanged(InputMethodContext context)
  {
    mLastSignal = "Editor LanguageChanged";

    if(context)
    {
      Dali::String locale = context.GetInputPanelLanguageLocale();
      if(locale.Size() > 0)
      {
        mLastSignal += " locale=";
        mLastSignal += locale.CStr();
      }
    }

    UpdateStatus();
  }

  void OnEditorKeyboardTypeChanged(InputMethodContext context, InputMethodContext::KeyboardType type)
  {
    (void)context;
    mLastSignal = "Editor KBTypeChanged ";
    mLastSignal += GetKeyboardTypeName(type);
    UpdateStatus();
  }

  void OnEditorPrivateCommandReceived(InputMethodContext ctx, const Dali::String& cmd)
  {
    (void)ctx;
    mLastSignal = "Editor PrivateCmd ";
    mLastSignal += cmd.CStr();
    UpdateStatus();
  }

  // --- Button handlers: Target Context ---

  bool OnButtonUseFieldTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      FocusInputField();
      SyncLocalStateFromContext();
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonUseEditorTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      FocusInputEditor();
      SyncLocalStateFromContext();
      UpdateStatus();
    }
    return true;
  }

  // --- Button handlers: Panel Visibility ---

  bool OnButtonShowPanelTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.ShowInputPanel();
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonHidePanelTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.HideInputPanel();
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonGetStateTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        Dali::String info = "PanelState=";
        info += GetStateName(context.GetInputPanelState());
        UpdateStatus(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonGetAreaTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        auto area = context.GetInputPanelArea();
        Dali::String info = "Area=(";
        info += std::to_string(area.x).c_str();
        info += ",";
        info += std::to_string(area.y).c_str();
        info += ",";
        info += std::to_string(area.width).c_str();
        info += ",";
        info += std::to_string(area.height).c_str();
        info += ")";
        UpdateStatus(info.CStr());
      }
    }
    return true;
  }

  // --- Button handlers: Auto / Restore / Return ---

  bool OnButtonAutoShowTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool& enabled = GetCurrentAutoShowEnabled();
      enabled = !enabled;
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.SetInputPanelAutoShowEnabled(enabled);
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonRestoreTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        bool enabled = context.IsRestoreAfterFocusLostEnabled();
        enabled = !enabled;
        context.SetRestoreAfterFocusLostEnabled(enabled);
        mRestoreAfterFocusLost = context.IsRestoreAfterFocusLostEnabled();
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonReturnKeyTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        bool enabled = context.IsReturnKeyEnabled();
        enabled = !enabled;
        context.SetReturnKeyEnabled(enabled);
        GetCurrentReturnKeyEnabled() = context.IsReturnKeyEnabled();
      }
      UpdateStatus();
    }
    return true;
  }

  // --- Button handlers: Prediction / Fullscreen ---

  bool OnButtonPredictionTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        bool enabled = context.IsTextPredictionEnabled();
        enabled = !enabled;
        context.SetTextPredictionEnabled(enabled);
        mPredictionEnabled = context.IsTextPredictionEnabled();
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonFullscreenTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        bool enabled = context.IsFullScreenModeEnabled();
        enabled = !enabled;
        context.SetFullScreenModeEnabled(enabled);
        mFullScreenEnabled = context.IsFullScreenModeEnabled();
      }
      UpdateStatus();
    }
    return true;
  }

  // --- Button handlers: Panel Data ---

  bool OnButtonSetDataTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        const char* data = (context == mEditorContext) ? "editor-panel-data" : "field-panel-data";
        context.SetInputPanelUserData(data);
        Dali::String info = "SetPanelData=";
        info += data;
        UpdateStatus(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonGetDataTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        Dali::String data = context.GetInputPanelUserData();
        Dali::String info = "PanelData=";
        info += data.CStr();
        UpdateStatus(info.CStr());
      }
    }
    return true;
  }

  // --- Button handlers: Panel Position ---

  bool OnButtonPos00Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.SetInputPanelPosition(0, 0);
        UpdateStatus("Pos=0,0");
      }
    }
    return true;
  }

  bool OnButtonPos100Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.SetInputPanelPosition(100, 100);
        UpdateStatus("Pos=100,100");
      }
    }
    return true;
  }

  bool OnButtonAlignTLTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        const auto align = InputMethodContext::InputPanelAlign::TOP_LEFT;
        bool result = context.SetInputPanelPositionAlign(0, 0, align);
        Dali::String info = "Align ";
        info += GetAlignName(align);
        info += result ? "=true" : "=false";
        UpdateStatus(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonAlignBCTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        const auto align = InputMethodContext::InputPanelAlign::BOTTOM_CENTER;
        bool result = context.SetInputPanelPositionAlign(0, 0, align);
        Dali::String info = "Align ";
        info += GetAlignName(align);
        info += result ? "=true" : "=false";
        UpdateStatus(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonAlignMCTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        const auto align = InputMethodContext::InputPanelAlign::MIDDLE_CENTER;
        bool result = context.SetInputPanelPositionAlign(0, 0, align);
        Dali::String info = "Align ";
        info += GetAlignName(align);
        info += result ? "=true" : "=false";
        UpdateStatus(info.CStr());
      }
    }
    return true;
  }

  // --- Button handlers: Query ---

  bool OnButtonKeyboardTypeTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        Dali::String info = "KBType=";
        info += GetKeyboardTypeName(context.GetKeyboardType());
        UpdateStatus(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonLocaleTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        Dali::String locale = context.GetInputPanelLanguageLocale();
        Dali::String info = "Locale=";
        info += locale.CStr();
        UpdateStatus(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonRefreshTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mLastSignal.Clear();
      SyncLocalStateFromContext();
      UpdateStatus();
    }
    return true;
  }

  // --- Key events ---

  void OnKeyEvent(Window window, KeyEvent event)
  {
    (void)window;
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    if(event.GetKeyName() == "1")
    {
      FocusInputField();
      SyncLocalStateFromContext();
      UpdateStatus("Key: Use Field");
    }
    else if(event.GetKeyName() == "2")
    {
      FocusInputEditor();
      SyncLocalStateFromContext();
      UpdateStatus("Key: Use Editor");
    }
    else if(event.GetKeyName() == "s")
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.ShowInputPanel();
      }
      UpdateStatus("Key: Show");
    }
    else if(event.GetKeyName() == "h")
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.HideInputPanel();
      }
      UpdateStatus("Key: Hide");
    }
    else if(event.GetKeyName() == "r")
    {
      mLastSignal.Clear();
      SyncLocalStateFromContext();
      UpdateStatus("Key: Refresh");
    }
  }

private:
  Application&       mApplication;
  InputField         mInputField;
  InputEditor        mInputEditor;
  InputMethodContext mFieldContext;
  InputMethodContext mEditorContext;
  Label              mStatusLabel;
  Dali::String       mLastSignal;

  bool mFieldAutoShowEnabled;
  bool mEditorAutoShowEnabled;
  bool mRestoreAfterFocusLost;
  bool mFieldReturnKeyEnabled;
  bool mEditorReturnKeyEnabled;
  bool mPredictionEnabled;
  bool mFullScreenEnabled;
  bool mLastFocusedInputIsEditor;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  InputMethodContextExample controller(application);
  application.MainLoop();

  return 0;
}
