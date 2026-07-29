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

#include <initializer_list>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

constexpr float STACK_SPACING  = 6.0f;
constexpr float STACK_PADDING  = 12.0f;
constexpr float BUTTON_HEIGHT  = 36.0f;
constexpr float BUTTON_SPACING = 4.0f;
constexpr size_t MAX_LOG_LINES = 10u;

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

StackLayoutParams CreateFillWeightParams()
{
  StackLayoutParams params = StackLayoutParams::New();
  params.SetWeight(1.0f);
  params.SetAlignment(LayoutAlignment::FILL);
  return params;
}

Label CreateButton(const char* text, uint32_t bgColor)
{
  Label button = Label::New(text);
  button.SetFontSize(11.0f);
  button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  button.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.SetBackgroundColor(UiColor(bgColor));
  button.SetTextColor(UiColor(0xFFFFFF));
  button.SetRequestedWidth(0.0f);
  button.SetRequestedHeight(BUTTON_HEIGHT);
  button.SetPadding(Extents(4, 4, 4, 4));
  button.SetLayoutParams(CreateFillWeightParams());
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
    mLastFocusedInputIsEditor(false)
  {
    mApplication.InitSignal().Connect(this, &InputMethodContextExample::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xF5F5F5));

    // Status labels
    mStatusLeftLabel = Label::New();
    mStatusLeftLabel.SetRequestedWidth(0.0f);
    mStatusLeftLabel.SetRequestedHeight(72);
    mStatusLeftLabel.SetFontSize(9.0f);
    mStatusLeftLabel.SetMultiLine(true);
    mStatusLeftLabel.SetFocusable(false);
    mStatusLeftLabel.SetBackgroundColor(UiColor(0xE8E8E8));
    mStatusLeftLabel.SetPadding(Extents(8, 8, 6, 6));
    mStatusLeftLabel.SetLayoutParams(CreateFillWeightParams());

    mStatusRightLabel = Label::New();
    mStatusRightLabel.SetRequestedWidth(0.0f);
    mStatusRightLabel.SetRequestedHeight(72);
    mStatusRightLabel.SetFontSize(9.0f);
    mStatusRightLabel.SetMultiLine(true);
    mStatusRightLabel.SetFocusable(false);
    mStatusRightLabel.SetBackgroundColor(UiColor(0xE8E8E8));
    mStatusRightLabel.SetPadding(Extents(8, 8, 6, 6));
    mStatusRightLabel.SetLayoutParams(CreateFillWeightParams());

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
    mInputField.SetLayoutParams(CreateFillWeightParams());

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
    mInputEditor.SetLayoutParams(CreateFillWeightParams());

    // Get InputMethodContext from each control
    mFieldContext = mInputField.GetInputMethodContext();
    mEditorContext = mInputEditor.GetInputMethodContext();

    // Connect IME signals for Field context
    ConnectContextSignals(mFieldContext);
    // Connect IME signals for Editor context
    ConnectContextSignals(mEditorContext);

    FocusManager::Get().FocusChangedSignal().Connect(this, &InputMethodContextExample::OnFocusChanged);

    // --- Create buttons ---

    // Panel Visibility
    Label btnShowPanel = CreateButton("Show", 0x3498DB);
    Label btnHidePanel = CreateButton("Hide", 0xE74C3C);
    Label btnGetState = CreateButton("Get State", 0x9B59B6);
    Label btnGetArea = CreateButton("Get Area", 0x1ABC9C);
    View panelRow = CreateButtonRow({btnShowPanel, btnHidePanel, btnGetState, btnGetArea});

    // Auto / Restore / Return
    Label btnAutoShow = CreateButton("AutoShow", 0x2980B9);
    Label btnRestore = CreateButton("Restore", 0x27AE60);
    Label btnReturn = CreateButton("ReturnKey", 0xE67E22);
    View autoRow = CreateButtonRow({btnAutoShow, btnRestore, btnReturn});

    // Prediction / Fullscreen / Panel Data
    Label btnPrediction = CreateButton("Prediction", 0x8E44AD);
    Label btnFullscreen = CreateButton("Fullscreen", 0x16A085);
    Label btnSetData = CreateButton("Set Data", 0xD35400);
    Label btnGetData = CreateButton("Get Data", 0x7F8C8D);
    View modeDataRow = CreateButtonRow({btnPrediction, btnFullscreen, btnSetData, btnGetData});

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
    Label btnLayoutEmoticon = CreateButton("Emoticon", 0x3498DB);
    Label btnLayoutVoice    = CreateButton("Voice", 0x2ECC71);
    View  layoutRow3        = CreateButtonRow({btnLayoutPassword, btnLayoutDateTime, btnLayoutEmoticon, btnLayoutVoice});

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

    mLogLabel = Label::New();
    mLogLabel.SetRequestedWidth(MATCH_PARENT);
    mLogLabel.SetRequestedHeight(128);
    mLogLabel.SetFontSize(11.0f);
    mLogLabel.SetMultiLine(true);
    mLogLabel.SetBackgroundColor(UiColor(0x202020));
    mLogLabel.SetTextColor(UiColor(0xFFFFFF));
    mLogLabel.SetPadding(Extents(8, 8, 6, 6));

    // --- Build layout ---

    StackLayout inputRow = StackLayout::New(StackOrientation::HORIZONTAL);
    inputRow.SetRequestedWidth(MATCH_PARENT);
    inputRow.SetRequestedHeight(WRAP_CONTENT);
    inputRow.SetSpacing(STACK_SPACING);
    inputRow.Add(mInputField);
    inputRow.Add(mInputEditor);

    StackLayout statusRow = StackLayout::New(StackOrientation::HORIZONTAL);
    statusRow.SetRequestedWidth(MATCH_PARENT);
    statusRow.SetRequestedHeight(WRAP_CONTENT);
    statusRow.SetSpacing(STACK_SPACING);
    statusRow.Add(mStatusLeftLabel);
    statusRow.Add(mStatusRightLabel);

    // Fixed header area
    StackLayout fixedHeader = StackLayout::New(StackOrientation::VERTICAL);
    fixedHeader.SetSpacing(STACK_SPACING);
    fixedHeader.SetRequestedWidth(MATCH_PARENT);
    fixedHeader.SetRequestedHeight(WRAP_CONTENT);
    fixedHeader.Add(statusRow);
    fixedHeader.Add(inputRow);

    // Scrollable content area
    StackLayout scrollContent = StackLayout::New(StackOrientation::VERTICAL);
    scrollContent.SetSpacing(STACK_SPACING);
    scrollContent.SetRequestedWidth(MATCH_PARENT);
    scrollContent.SetRequestedHeight(WRAP_CONTENT);
    scrollContent.SetPadding(Extents(0, 0, 0, static_cast<int16_t>(STACK_PADDING)));
    scrollContent.Add(CreateSectionTitle("Panel Visibility"));
    scrollContent.Add(panelRow);
    scrollContent.Add(CreateSectionTitle("Auto / Restore / Return"));
    scrollContent.Add(autoRow);
    scrollContent.Add(CreateSectionTitle("Prediction / Fullscreen / Panel Data"));
    scrollContent.Add(modeDataRow);
    scrollContent.Add(CreateSectionTitle("Panel Position"));
    scrollContent.Add(posRow);
    scrollContent.Add(alignRow);
    scrollContent.Add(CreateSectionTitle("Panel Layout"));
    scrollContent.Add(layoutRow1);
    scrollContent.Add(layoutRow2);
    scrollContent.Add(layoutRow3);
    scrollContent.Add(CreateSectionTitle("Return Key"));
    scrollContent.Add(returnKeyRow1);
    scrollContent.Add(returnKeyRow2);
    scrollContent.Add(returnKeyRow3);
    scrollContent.Add(CreateSectionTitle("Auto Capital"));
    scrollContent.Add(autoCapitalRow);
    scrollContent.Add(CreateSectionTitle("Layout Variation"));
    scrollContent.Add(variationRow1);
    scrollContent.Add(variationRow2);
    scrollContent.Add(variationRow3);
    scrollContent.Add(CreateSectionTitle("Query"));
    scrollContent.Add(queryRow);
    scrollContent.Add(CreateSectionTitle("Log"));
    scrollContent.Add(mLogLabel);

    // ScrollView for options
    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetRequestedHeight(0.0f);
    scrollView.SetLayoutParams(CreateFillWeightParams());
    scrollView.SetContent(scrollContent);

    // Root layout
    StackLayout rootLayout = StackLayout::New(StackOrientation::VERTICAL);
    rootLayout.SetSpacing(STACK_SPACING);
    rootLayout.SetRequestedWidth(MATCH_PARENT);
    rootLayout.SetRequestedHeight(MATCH_PARENT);
    rootLayout.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));
    rootLayout.Add(fixedHeader);
    rootLayout.Add(scrollView);

    window.Add(rootLayout);

    // --- Connect button touch signals ---

    // Panel Visibility
    btnShowPanel.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonShowPanelTouched);
    btnHidePanel.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonHidePanelTouched);
    btnGetState.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonGetStateTouched);
    btnGetArea.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonGetAreaTouched);

    // Auto / Restore / Return
    btnAutoShow.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonAutoShowTouched);
    btnRestore.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonRestoreTouched);
    btnReturn.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonReturnKeyTouched);

    // Prediction / Fullscreen
    btnPrediction.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonPredictionTouched);
    btnFullscreen.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonFullscreenTouched);

    // Panel Data
    btnSetData.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonSetDataTouched);
    btnGetData.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonGetDataTouched);

    // Panel Position
    btnPos00.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonPos00Touched);
    btnPos100.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonPos100Touched);
    btnAlignTL.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonAlignTLTouched);
    btnAlignBC.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonAlignBCTouched);
    btnAlignMC.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonAlignMCTouched);

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
    btnKeyboardType.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonKeyboardTypeTouched);
    btnLocale.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonLocaleTouched);
    btnRefresh.TouchEventSignal().Connect(this, &InputMethodContextExample::OnButtonRefreshTouched);

    // Key event
    window.KeyEventSignal().Connect(this, &InputMethodContextExample::OnKeyEvent);

    FocusInputField();
    UpdateStatus();
  }

  // --- Helper ---

  bool IsTouchUp(const TouchEvent& touch)
  {
    return touch.GetPointCount() > 0u && touch.GetState(0) == PointState::UP;
  }

  InputMethodContext GetCurrentContext()
  {
    if(IsEditorTarget())
    {
      return mEditorContext;
    }

    return mFieldContext ? mFieldContext : mEditorContext;
  }

  const char* GetContextName(InputMethodContext context) const
  {
    if(context == mFieldContext)
    {
      return "Field";
    }
    if(context == mEditorContext)
    {
      return "Editor";
    }
    return "Unknown";
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

  void FocusInputField()
  {
    mLastFocusedInputIsEditor = false;
    FocusManager::Get().SetCurrentFocusView(mInputField);
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
      AppendLog(info.CStr());
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
      AppendLog(info.CStr());
    }
  }

  void ConnectLayoutButton(Label button, Dali::InputMethod::PanelLayout layout)
  {
    button.TouchEventSignal().Connect(this, [this, layout](Actor, const TouchEvent touch) -> bool
    {
      if(IsTouchUp(touch))
      {
        SetPanelLayout(layout);
      }
      return true;
    });
  }

  void ConnectVariationButton(Label button, Dali::InputMethod::PanelLayoutVariation variation)
  {
    button.TouchEventSignal().Connect(this, [this, variation](Actor, const TouchEvent touch) -> bool
    {
      if(IsTouchUp(touch))
      {
        SetPanelLayoutVariation(variation);
      }
      return true;
    });
  }

  void ConnectReturnKeyButton(Label button, Dali::InputMethod::ReturnKeyType action)
  {
    button.TouchEventSignal().Connect(this, [this, action](Actor, const TouchEvent touch) -> bool
    {
      if(IsTouchUp(touch))
      {
        InputMethodContext context = GetCurrentContext();
        if(context)
        {
          context.SetInputPanelReturnKeyType(action);
          Dali::String info = "ReturnKey=";
          info += GetReturnKeyName(context.GetInputPanelReturnKeyType());
          UpdateStatus(info.CStr());
          AppendLog(info.CStr());
        }
      }
      return true;
    });
  }

  void ConnectAutoCapitalButton(Label button, Dali::InputMethod::AutoCapitalType autoCapital)
  {
    button.TouchEventSignal().Connect(this, [this, autoCapital](Actor, const TouchEvent touch) -> bool
    {
      if(IsTouchUp(touch))
      {
        InputMethodContext context = GetCurrentContext();
        if(context)
        {
          context.SetInputPanelAutoCapitalType(autoCapital);
          Dali::String info = "AutoCapital=";
          info += GetAutoCapitalName(context.GetInputPanelAutoCapitalType());
          UpdateStatus(info.CStr());
          AppendLog(info.CStr());
        }
      }
      return true;
    });
  }

  void ConnectContextSignals(InputMethodContext context)
  {
    if(!context)
    {
      return;
    }

    context.ActivatedSignal().Connect(this, &InputMethodContextExample::OnActivated);
    context.StatusChangedSignal().Connect(this, &InputMethodContextExample::OnStatusChanged);
    context.ResizedSignal().Connect(this, &InputMethodContextExample::OnResized);
    context.LanguageChangedSignal().Connect(this, &InputMethodContextExample::OnLanguageChanged);
    context.KeyboardTypeChangedSignal().Connect(this, &InputMethodContextExample::OnKeyboardTypeChanged);
    context.PrivateCommandReceivedSignal().Connect(this, &InputMethodContextExample::OnPrivateCommandReceived);
  }

  void UpdateStatus(const char* extra = nullptr)
  {
    InputMethodContext context = GetCurrentContext();

    Dali::String leftStatus;
    Dali::String rightStatus;
    leftStatus += "Target:";
    leftStatus += GetCurrentTargetName();

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

      leftStatus += "\nState:";
      leftStatus += GetStateName(state);
      leftStatus += " KB:";
      leftStatus += GetKeyboardTypeName(keyboardType);
      leftStatus += "\nLocale:";
      leftStatus += locale.CStr();
      leftStatus += "\nArea:";
      leftStatus += FormatArea(area);

      rightStatus += "Restore:";
      rightStatus += restoreEnabled ? "ON" : "OFF";
      rightStatus += " Prediction:";
      rightStatus += predictionEnabled ? "ON" : "OFF";
      rightStatus += "\nFull:";
      rightStatus += fullScreenEnabled ? "ON" : "OFF";
      rightStatus += " AutoShow(set):";
      rightStatus += GetCurrentAutoShowEnabled() ? "ON" : "OFF";
      rightStatus += " Return:";
      rightStatus += returnKeyEnabled ? "ON" : "OFF";

      rightStatus += "\nLayout:";
      rightStatus += GetPanelLayoutName(layout);
      rightStatus += " Ret:";
      rightStatus += GetReturnKeyName(returnKey);
      rightStatus += "\nAutoCap:";
      rightStatus += GetAutoCapitalName(autoCapital);
      rightStatus += " Var:";
      rightStatus += GetPanelLayoutVariationName(variation);
    }
    else
    {
      leftStatus += "\nContext:null";
    }

    if(mLastSignal.Size() > 0)
    {
      rightStatus += "\nLast Signal:";
      rightStatus += mLastSignal.CStr();
    }

    if(extra)
    {
      rightStatus += "\n";
      rightStatus += extra;
    }

    mStatusLeftLabel.SetText(leftStatus);
    mStatusRightLabel.SetText(rightStatus);
  }

  Dali::String FormatArea(const Dali::BoundsInteger& area)
  {
    std::string str = "(" + std::to_string(area.x) + "," + std::to_string(area.y) + "," + std::to_string(area.width) + "," + std::to_string(area.height) + ")";
    return Dali::String(str.c_str());
  }

  void AppendLog(const char* text)
  {
    if(!text)
    {
      return;
    }

    mLogLines.insert(mLogLines.begin(), text);
    while(mLogLines.size() > MAX_LOG_LINES)
    {
      mLogLines.pop_back();
    }

    Dali::String logText;
    for(size_t index = 0u; index < mLogLines.size(); ++index)
    {
      if(index > 0u)
      {
        logText += "\n";
      }
      logText += mLogLines[index].c_str();
    }

    if(mLogLabel)
    {
      mLogLabel.SetText(logText);
    }
  }

  // --- IME Signal callbacks ---

  void OnFocusChanged(View, View current)
  {
    if(current == mInputEditor)
    {
      mLastFocusedInputIsEditor = true;
      AppendLog("FocusChanged Target=Editor");
      UpdateStatus();
    }
    else if(current == mInputField)
    {
      mLastFocusedInputIsEditor = false;
      AppendLog("FocusChanged Target=Field");
      UpdateStatus();
    }
  }

  void OnActivated(InputMethodContext context)
  {
    mLastSignal = GetContextName(context);
    mLastSignal += " Activated";
    AppendLog(mLastSignal.CStr());
    UpdateStatus();
  }

  void OnStatusChanged(InputMethodContext context, InputMethodContext::State state)
  {
    mLastSignal = GetContextName(context);
    mLastSignal += " StatusChanged ";
    mLastSignal += GetStateName(state);
    AppendLog(mLastSignal.CStr());
    UpdateStatus();
  }

  void OnResized(InputMethodContext context)
  {
    mLastSignal = GetContextName(context);
    mLastSignal += " GeometryChanged";

    if(context)
    {
      Dali::BoundsInteger area = context.GetInputPanelArea();
      mLastSignal += " area=";
      mLastSignal += FormatArea(area);
    }

    AppendLog(mLastSignal.CStr());
    UpdateStatus();
  }

  void OnLanguageChanged(InputMethodContext context)
  {
    mLastSignal = GetContextName(context);
    mLastSignal += " LanguageChanged";

    if(context)
    {
      Dali::String locale = context.GetInputPanelLanguageLocale();
      if(locale.Size() > 0)
      {
        mLastSignal += " locale=";
        mLastSignal += locale.CStr();
      }
    }

    AppendLog(mLastSignal.CStr());
    UpdateStatus();
  }

  void OnKeyboardTypeChanged(InputMethodContext context, InputMethodContext::KeyboardType type)
  {
    mLastSignal = GetContextName(context);
    mLastSignal += " KBTypeChanged ";
    mLastSignal += GetKeyboardTypeName(type);
    AppendLog(mLastSignal.CStr());
    UpdateStatus();
  }

  void OnPrivateCommandReceived(InputMethodContext context, const Dali::String& cmd)
  {
    mLastSignal = GetContextName(context);
    mLastSignal += " PrivateCmd ";
    mLastSignal += cmd;
    AppendLog(mLastSignal.CStr());
    UpdateStatus();
  }

  // --- Button handlers: Panel Visibility ---

  bool OnButtonShowPanelTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.ShowInputPanel();
      }
      UpdateStatus();
      AppendLog("ShowInputPanel");
    }
    return true;
  }

  bool OnButtonHidePanelTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.HideInputPanel();
      }
      UpdateStatus();
      AppendLog("HideInputPanel");
    }
    return true;
  }

  bool OnButtonGetStateTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        Dali::String info = "PanelState=";
        info += GetStateName(context.GetInputPanelState());
        UpdateStatus(info.CStr());
        AppendLog(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonGetAreaTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        auto area = context.GetInputPanelArea();
        Dali::String info = "Area=";
        info += FormatArea(area);
        UpdateStatus(info.CStr());
        AppendLog(info.CStr());
      }
    }
    return true;
  }

  // --- Button handlers: Auto / Restore / Return ---

  bool OnButtonAutoShowTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      bool& enabled = GetCurrentAutoShowEnabled();
      enabled = !enabled;
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.SetInputPanelAutoShowEnabled(enabled);
      }
      UpdateStatus();
      AppendLog(enabled ? "AutoShow=ON" : "AutoShow=OFF");
    }
    return true;
  }

  bool OnButtonRestoreTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        bool enabled = !context.IsRestoreAfterFocusLostEnabled();
        context.SetRestoreAfterFocusLostEnabled(enabled);
        UpdateStatus();
        AppendLog(enabled ? "Restore=ON" : "Restore=OFF");
      }
    }
    return true;
  }

  bool OnButtonReturnKeyTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        bool enabled = !context.IsReturnKeyEnabled();
        context.SetReturnKeyEnabled(enabled);
        UpdateStatus();
        AppendLog(enabled ? "ReturnKey=ON" : "ReturnKey=OFF");
      }
    }
    return true;
  }

  // --- Button handlers: Prediction / Fullscreen ---

  bool OnButtonPredictionTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        bool enabled = context.IsTextPredictionEnabled();
        enabled = !enabled;
        context.SetTextPredictionEnabled(enabled);
        UpdateStatus();
        AppendLog(enabled ? "Prediction=ON" : "Prediction=OFF");
      }
    }
    return true;
  }

  bool OnButtonFullscreenTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        bool enabled = context.IsFullScreenModeEnabled();
        enabled = !enabled;
        context.SetFullScreenModeEnabled(enabled);
        UpdateStatus();
        AppendLog(enabled ? "Fullscreen=ON" : "Fullscreen=OFF");
      }
    }
    return true;
  }

  // --- Button handlers: Panel Data ---

  bool OnButtonSetDataTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        const char* data = (context == mEditorContext) ? "editor-panel-data" : "field-panel-data";
        context.SetInputPanelUserData(data);
        Dali::String info = "SetPanelData=";
        info += data;
        UpdateStatus(info.CStr());
        AppendLog(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonGetDataTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        Dali::String data = context.GetInputPanelUserData();
        Dali::String info = "PanelData=";
        info += data.CStr();
        UpdateStatus(info.CStr());
        AppendLog(info.CStr());
      }
    }
    return true;
  }

  // --- Button handlers: Panel Position ---

  bool OnButtonPos00Touched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.SetInputPanelPosition(0, 0);
        UpdateStatus("Pos=0,0");
        AppendLog("Pos=0,0");
      }
    }
    return true;
  }

  bool OnButtonPos100Touched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.SetInputPanelPosition(100, 100);
        UpdateStatus("Pos=100,100");
        AppendLog("Pos=100,100");
      }
    }
    return true;
  }

  bool OnButtonAlignTLTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
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
        AppendLog(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonAlignBCTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
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
        AppendLog(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonAlignMCTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
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
        AppendLog(info.CStr());
      }
    }
    return true;
  }

  // --- Button handlers: Query ---

  bool OnButtonKeyboardTypeTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        Dali::String info = "KBType=";
        info += GetKeyboardTypeName(context.GetKeyboardType());
        UpdateStatus(info.CStr());
        AppendLog(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonLocaleTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        Dali::String locale = context.GetInputPanelLanguageLocale();
        Dali::String info = "Locale=";
        info += locale.CStr();
        UpdateStatus(info.CStr());
        AppendLog(info.CStr());
      }
    }
    return true;
  }

  bool OnButtonRefreshTouched(Actor, const TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      mLastSignal.Clear();
      UpdateStatus();
      AppendLog("Refresh");
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

    if(event.GetKeyName() == "s")
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.ShowInputPanel();
      }
      UpdateStatus("Key: Show");
      AppendLog("Key: Show");
    }
    else if(event.GetKeyName() == "h")
    {
      InputMethodContext context = GetCurrentContext();
      if(context)
      {
        context.HideInputPanel();
      }
      UpdateStatus("Key: Hide");
      AppendLog("Key: Hide");
    }
    else if(event.GetKeyName() == "r")
    {
      mLastSignal.Clear();
      UpdateStatus("Key: Refresh");
      AppendLog("Key: Refresh");
    }
  }

private:
  Application&       mApplication;
  InputField         mInputField;
  InputEditor        mInputEditor;
  InputMethodContext mFieldContext;
  InputMethodContext mEditorContext;
  Label              mStatusLeftLabel;
  Label              mStatusRightLabel;
  Label              mLogLabel;
  Dali::String       mLastSignal;
  std::vector<std::string> mLogLines;

  bool mFieldAutoShowEnabled;
  bool mEditorAutoShowEnabled;
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
