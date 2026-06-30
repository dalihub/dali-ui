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
#include <dali/public-api/adaptor-framework/clipboard.h>
#include <dali/public-api/adaptor-framework/clipboard-data.h>
#include <dali/public-api/adaptor-framework/timer.h>

#include <cstdint>
#include <initializer_list>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING     = 6.0f;
constexpr float STACK_PADDING     = 12.0f;
constexpr float BUTTON_HEIGHT     = 36.0f;
constexpr float BUTTON_SPACING    = 4.0f;
constexpr float INPUT_HEIGHT      = 54.0f;
constexpr uint32_t STRESS_INTERVAL_MS   = 50u;
constexpr uint32_t STRESS_MAX_COUNT     = 100u;
constexpr uint32_t STRESS_TIMEOUT_TICKS = 20u;

constexpr const char* MIME_TYPE_PLAIN_TEXT = "text/plain;charset=utf-8";
constexpr const char* MIME_TYPE_HTML       = "application/xhtml+xml";
constexpr const char* MIME_TYPE_TEXT_URI   = "text/uri-list";

constexpr const char* MULTI_COPY_PLAIN_TEXT = "www.samsung.com";
constexpr const char* MULTI_COPY_HTML =
  "<html><body>"
  "<a href=\"https://www.samsung.com\">"
  "<span style=\"font-weight:bold;color:#1428A0;\">www.samsung.com</span>"
  "</a>"
  "</body></html>";

bool IsTouchUp(const TouchEvent& touch)
{
  return touch.GetState(0u) == PointState::UP;
}

Label CreateLabel(const char* text, float fontSize, uint32_t bgColor)
{
  Label label = Label::New(text);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(0x222222));
  label.SetBackgroundColor(UiColor(bgColor));
  label.SetPadding(Extents(8, 8, 6, 6));
  label.SetMultiLine(true);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  return label;
}

Label CreateButton(const char* text, uint32_t bgColor)
{
  Label button = Label::New(text);
  button.SetFontSize(11.0f);
  button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  button.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.SetTextColor(UiColor(0xFFFFFF));
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

  for(auto& button : buttons)
  {
    row.Add(button);
  }
  return row;
}
} // namespace

// Basic Clipboard usage:
// 1. Copy data to the clipboard.
//    Dali::Clipboard clipboard = Dali::Clipboard::Get();
//    clipboard.SetData(Dali::ClipboardData(Dali::String(MIME_TYPE_PLAIN_TEXT),
//                                          Dali::String("Hello Clipboard")));
//
// 2. Request data from the clipboard.
//    The callback receiver must inherit Dali::ConnectionTracker, or use
//    Dali::SlotDelegate if inheritance is not possible. The callback is
//    delivered asynchronously and is not invoked after the receiver is destroyed.
//
//    clipboard.GetData(Dali::String(MIME_TYPE_PLAIN_TEXT),
//                      this,
//                      &ClipboardExample::OnClipboardDataReceived);
//
//    void ClipboardExample::OnClipboardDataReceived(bool succeeded, const Dali::ClipboardData& data)
//    {
//      if(!succeeded)
//      {
//        return;
//      }
//
//      Dali::String mimeType = data.GetMimeType();
//      Dali::String content  = data.GetContent();
//    }

class ClipboardExample : public ConnectionTracker
{
public:
  explicit ClipboardExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ClipboardExample::OnInit);
  }

  ~ClipboardExample()
  {
    StopStress();
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xF5F5F5));

    mClipboard = Clipboard::Get();

    Label titleLabel = Label::New("Clipboard Example");
    titleLabel.SetFontSize(16.0f);
    titleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    titleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    titleLabel.SetTextColor(UiColor(0xFFFFFF));
    titleLabel.SetBackgroundColor(UiColor(0x2C3E50));
    titleLabel.SetRequestedWidth(MATCH_PARENT);
    titleLabel.SetRequestedHeight(36.0f);

    Label description = CreateLabel(
      "SetData() copies text with a MIME type.\n"
      "GetData() requests data with a MIME type and receives the result via callback.\n"
      "The requested MIME type must match the stored MIME type.\n"
      "Multi Copy/Paste writes plain text and HTML clipboard data sequentially, then requests both MIME types.\n"
      "Multi MIME behavior may vary by backend; target verification is recommended.\n"
      "Stress test repeatedly calls SetData/GetData to check callback lifecycle.",
      10.0f,
      0xE8EEF4);

    mInputField = InputField::New();
    mInputField.SetPlaceholder("Enter text to copy");
    mInputField.SetFontSize(18.0f);
    mInputField.SetTextColor(UiColor(0x222222));
    mInputField.SetPlaceholderColor(UiColor(0x606060));
    mInputField.SetBackgroundColor(UiColor(0xFFFFFF));
    mInputField.SetRequestedWidth(MATCH_PARENT);
    mInputField.SetRequestedHeight(INPUT_HEIGHT);
    mInputField.SetPadding(Extents(12, 12, 8, 8));
    mInputField.SetMaximumLength(500);
    mInputField.SetFocusable(true);

    mResultLabel = CreateLabel("", 10.0f, 0xF0F0F0);
    mResultLabel.SetRequestedHeight(122.0f);

    mMultiResultLabel = CreateLabel("", 10.0f, 0xFFF8E1);

    Label plainTitle = CreateLabel("Plain text", 11.0f, 0xFFFFFF);
    Label htmlTitle  = CreateLabel("HTML", 11.0f, 0xFFFFFF);
    Label uriTitle   = CreateLabel("URI", 11.0f, 0xFFFFFF);
    Label multiTitle = CreateLabel("Multi MIME copy/paste", 11.0f, 0xFFFFFF);

    Label btnCopyPlain   = CreateButton("Copy", 0x3498DB);
    Label btnPastePlain  = CreateButton("Paste", 0x27AE60);
    Label btnHasPlain    = CreateButton("HasType", 0x7F8C8D);
    Label btnCopyHtml    = CreateButton("Copy", 0x8E44AD);
    Label btnPasteHtml   = CreateButton("Paste", 0x27AE60);
    Label btnHasHtml     = CreateButton("HasType", 0x7F8C8D);
    Label btnCopyUri     = CreateButton("Copy", 0xD35400);
    Label btnPasteUri    = CreateButton("Paste", 0x27AE60);
    Label btnHasUri      = CreateButton("HasType", 0x7F8C8D);
    Label btnMultiCopy   = CreateButton("Multi Copy", 0x16A085);
    Label btnMultiPaste  = CreateButton("Multi Paste", 0x2980B9);
    Label btnStressStart = CreateButton("Stress Start", 0xC0392B);
    Label btnStressStop  = CreateButton("Stress Stop", 0x34495E);
    Label btnClear       = CreateButton("Clear", 0x95A5A6);

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetSpacing(STACK_SPACING);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetPadding(Extents(0, 0, 0, STACK_PADDING));
    content.AddChildren({
      plainTitle,
      CreateButtonRow({btnCopyPlain, btnPastePlain, btnHasPlain}),
      htmlTitle,
      CreateButtonRow({btnCopyHtml, btnPasteHtml, btnHasHtml}),
      uriTitle,
      CreateButtonRow({btnCopyUri, btnPasteUri, btnHasUri}),
      multiTitle,
      CreateButtonRow({btnMultiCopy, btnMultiPaste}),
      mMultiResultLabel,
      CreateButtonRow({btnStressStart, btnStressStop, btnClear}),
    });

    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetRequestedHeight(0.0f);
    scrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    scrollView.SetContent(content);

    StackLayout rootLayout = StackLayout::New(StackOrientation::VERTICAL);
    rootLayout.SetSpacing(STACK_SPACING);
    rootLayout.SetRequestedWidth(MATCH_PARENT);
    rootLayout.SetRequestedHeight(MATCH_PARENT);
    rootLayout.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));
    rootLayout.AddChildren({
      titleLabel,
      description,
      mInputField,
      mResultLabel,
      scrollView,
    });

    window.Add(rootLayout);

    btnCopyPlain.TouchEventSignal().Connect(this, &ClipboardExample::OnCopyPlainTouched);
    btnPastePlain.TouchEventSignal().Connect(this, &ClipboardExample::OnPastePlainTouched);
    btnHasPlain.TouchEventSignal().Connect(this, &ClipboardExample::OnHasPlainTouched);
    btnCopyHtml.TouchEventSignal().Connect(this, &ClipboardExample::OnCopyHtmlTouched);
    btnPasteHtml.TouchEventSignal().Connect(this, &ClipboardExample::OnPasteHtmlTouched);
    btnHasHtml.TouchEventSignal().Connect(this, &ClipboardExample::OnHasHtmlTouched);
    btnCopyUri.TouchEventSignal().Connect(this, &ClipboardExample::OnCopyUriTouched);
    btnPasteUri.TouchEventSignal().Connect(this, &ClipboardExample::OnPasteUriTouched);
    btnHasUri.TouchEventSignal().Connect(this, &ClipboardExample::OnHasUriTouched);
    btnMultiCopy.TouchEventSignal().Connect(this, &ClipboardExample::OnMultiCopyTouched);
    btnMultiPaste.TouchEventSignal().Connect(this, &ClipboardExample::OnMultiPasteTouched);
    btnStressStart.TouchEventSignal().Connect(this, &ClipboardExample::OnStressStartTouched);
    btnStressStop.TouchEventSignal().Connect(this, &ClipboardExample::OnStressStopTouched);
    btnClear.TouchEventSignal().Connect(this, &ClipboardExample::OnClearTouched);
    window.KeyEventSignal().Connect(this, &ClipboardExample::OnKeyEvent);

    ClearMultiState();
    SetLastResult("Ready");
  }

  void SetLastResult(const char* result)
  {
    mLastResult = result;
    UpdateStatus();
  }

  void SetClipboardResult(const char* result, const Dali::String& mimeType, const Dali::String& content)
  {
    mLastResult = result;
    mLastMimeType = mimeType;
    mLastData = content;
    UpdateStatus();
  }

  void UpdateStatus()
  {
    Dali::String status;
    status += "Last result: ";
    status += mLastResult.CStr();
    status += "\nMIME type: ";
    status += mLastMimeType.CStr();
    status += "\nData: ";
    status += mLastData.CStr();
    status += "\nCallback count: ";
    status += std::to_string(mCallbackCount).c_str();
    status += "\nStress progress: ";
    status += std::to_string(mStressRequestedCount).c_str();
    status += "/";
    status += std::to_string(STRESS_MAX_COUNT).c_str();
    status += " callbacks=";
    status += std::to_string(mStressCallbackCount).c_str();
    status += " ok=";
    status += std::to_string(mStressSuccessCount).c_str();
    status += " fail=";
    status += std::to_string(mStressFailCount).c_str();
    status += (mStressRunning ? " running" : " stopped");
    mResultLabel.SetText(status);
  }

  void ClearMultiState()
  {
    mMultiPlainCopyResult.Clear();
    mMultiHtmlCopyResult.Clear();
    mMultiPlainPasteResult.Clear();
    mMultiHtmlPasteResult.Clear();
    mMultiPlainData.Clear();
    mMultiHtmlData.Clear();
    mMultiPlainCallbackCount = 0u;
    mMultiHtmlCallbackCount = 0u;
    UpdateMultiStatus();
  }

  void UpdateMultiStatus()
  {
    if(!mMultiResultLabel)
    {
      return;
    }

    Dali::String status;
    status += "Multi MIME Copy/Paste\n";
    status += "Plain SetData: ";
    status += mMultiPlainCopyResult.Empty() ? "-" : mMultiPlainCopyResult.CStr();
    status += "\nHTML SetData: ";
    status += mMultiHtmlCopyResult.Empty() ? "-" : mMultiHtmlCopyResult.CStr();
    status += "\nPlain GetData: ";
    status += mMultiPlainPasteResult.Empty() ? "-" : mMultiPlainPasteResult.CStr();
    status += " callbacks=";
    status += std::to_string(mMultiPlainCallbackCount).c_str();
    status += "\nPlain Data: ";
    status += mMultiPlainData.Empty() ? "-" : mMultiPlainData.CStr();
    status += "\nHTML GetData: ";
    status += mMultiHtmlPasteResult.Empty() ? "-" : mMultiHtmlPasteResult.CStr();
    status += " callbacks=";
    status += std::to_string(mMultiHtmlCallbackCount).c_str();
    status += "\nHTML Data: ";
    status += mMultiHtmlData.Empty() ? "-" : mMultiHtmlData.CStr();
    status += "\nNote: desktop may expose different multi MIME behavior than target.";

    mMultiResultLabel.SetText(status);
  }

  Dali::String GetInputText() const
  {
    Dali::String input = mInputField.GetText();
    if(input.Empty())
    {
      return Dali::String("Clipboard sample text");
    }
    return input;
  }

  void CopyWithMimeType(const char* mimeType)
  {
    if(!mClipboard)
    {
      SetLastResult("SetData failed: clipboard unavailable");
      return;
    }

    Dali::String content = GetInputText();
    Dali::ClipboardData data(Dali::String(mimeType), content);
    bool result = mClipboard.SetData(data);
    SetClipboardResult(result ? "SetData success" : "SetData failed", Dali::String(mimeType), content);
  }

  void PasteWithMimeType(const char* mimeType)
  {
    if(!mClipboard)
    {
      SetLastResult("GetData failed: clipboard unavailable");
      return;
    }

    mLastRequestedMimeType = Dali::String(mimeType);
    mClipboard.GetData(mLastRequestedMimeType, this, &ClipboardExample::OnClipboardDataReceived);
  }

  void CheckHasType(const char* mimeType)
  {
    if(!mClipboard)
    {
      SetLastResult("HasType failed: clipboard unavailable");
      return;
    }

    bool hasType = mClipboard.HasType(Dali::String(mimeType));
    mLastMimeType = Dali::String(mimeType);
    mLastData.Clear();
    SetLastResult(hasType ? "HasType true" : "HasType false");
  }

  void MultiCopyPlainAndHtml()
  {
    if(!mClipboard)
    {
      SetLastResult("Multi Copy failed: clipboard unavailable");
      return;
    }

    Dali::ClipboardData plainData(MIME_TYPE_PLAIN_TEXT, MULTI_COPY_PLAIN_TEXT);
    bool plainResult = mClipboard.SetData(plainData);

    Dali::ClipboardData htmlData(MIME_TYPE_HTML, MULTI_COPY_HTML);
    bool htmlResult = mClipboard.SetData(htmlData);

    mMultiPlainCopyResult = plainResult ? "success" : "failed";
    mMultiHtmlCopyResult = htmlResult ? "success" : "failed";
    mMultiPlainData = Dali::String(MULTI_COPY_PLAIN_TEXT);
    mMultiHtmlData = Dali::String(MULTI_COPY_HTML);

    mLastMimeType = "plain+html";
    mLastData = "plain/html multi clipboard data";
    SetLastResult((plainResult && htmlResult) ? "Multi Copy SetData complete" : "Multi Copy SetData failed");
    UpdateMultiStatus();
  }

  void MultiPastePlainAndHtml()
  {
    if(!mClipboard)
    {
      SetLastResult("Multi Paste failed: clipboard unavailable");
      return;
    }

    mMultiPlainPasteResult = "requested";
    mMultiHtmlPasteResult = "requested";
    mMultiPlainData.Clear();
    mMultiHtmlData.Clear();
    mMultiPlainCallbackCount = 0u;
    mMultiHtmlCallbackCount = 0u;

    mClipboard.GetData(Dali::String(MIME_TYPE_PLAIN_TEXT), this, &ClipboardExample::OnMultiPlainDataReceived);
    mClipboard.GetData(Dali::String(MIME_TYPE_HTML), this, &ClipboardExample::OnMultiHtmlDataReceived);

    SetLastResult("Multi Paste GetData requested");
    UpdateMultiStatus();
  }

  void OnClipboardDataReceived(bool succeeded, const Dali::ClipboardData& data)
  {
    ++mCallbackCount;
    if(!succeeded)
    {
      mLastMimeType = mLastRequestedMimeType;
      mLastData.Clear();
      SetLastResult("GetData failed");
      return;
    }

    SetClipboardResult("GetData success", data.GetMimeType(), data.GetContent());
  }

  void OnMultiPlainDataReceived(bool succeeded, const Dali::ClipboardData& data)
  {
    ++mMultiPlainCallbackCount;
    if(!succeeded)
    {
      mMultiPlainPasteResult = "failed";
      mMultiPlainData.Clear();
      UpdateMultiStatus();
      return;
    }

    bool mimeMatches = data.GetMimeType() == Dali::String(MIME_TYPE_PLAIN_TEXT);
    mMultiPlainPasteResult = mimeMatches ? "success" : "mime mismatch";
    mMultiPlainData = data.GetContent();
    UpdateMultiStatus();
  }

  void OnMultiHtmlDataReceived(bool succeeded, const Dali::ClipboardData& data)
  {
    ++mMultiHtmlCallbackCount;
    if(!succeeded)
    {
      mMultiHtmlPasteResult = "failed";
      mMultiHtmlData.Clear();
      UpdateMultiStatus();
      return;
    }

    bool mimeMatches = data.GetMimeType() == Dali::String(MIME_TYPE_HTML);
    mMultiHtmlPasteResult = mimeMatches ? "success" : "mime mismatch";
    mMultiHtmlData = data.GetContent();
    UpdateMultiStatus();
  }

  void StartStress()
  {
    if(!mClipboard)
    {
      SetLastResult("Stress failed: clipboard unavailable");
      return;
    }

    StopStress();
    mStressRequestedCount = 0u;
    mStressCallbackCount = 0u;
    mStressSuccessCount = 0u;
    mStressFailCount = 0u;
    mStressOutstanding = false;
    mStressOutstandingTicks = 0u;
    mStressExpectedContent.Clear();
    mStressRunning = true;

    mStressTimer = Timer::New(STRESS_INTERVAL_MS);
    mStressTimer.TickSignal().Connect(this, &ClipboardExample::OnStressTimerTick);
    mStressTimer.Start();
    SetLastResult("Stress started");
  }

  void StopStress()
  {
    if(mStressTimer)
    {
      mStressTimer.Stop();
      mStressTimer.Reset();
    }
    mStressRunning = false;
    mStressOutstanding = false;
  }

  bool OnStressTimerTick()
  {
    if(!mStressRunning)
    {
      return false;
    }

    if(mStressOutstanding)
    {
      ++mStressOutstandingTicks;
      if(mStressOutstandingTicks <= STRESS_TIMEOUT_TICKS)
      {
        UpdateStatus();
        return true;
      }

      ++mStressFailCount;
      mStressOutstanding = false;
      mStressOutstandingTicks = 0u;
      SetLastResult("Stress timeout");
    }

    if(mStressRequestedCount >= STRESS_MAX_COUNT)
    {
      mStressRunning = false;
      mStressOutstanding = false;
      SetLastResult("Stress complete");
      return false;
    }

    ++mStressRequestedCount;
    std::string content = "stress-" + std::to_string(mStressRequestedCount);
    mStressExpectedContent = Dali::String(content.c_str());
    Dali::ClipboardData data(MIME_TYPE_PLAIN_TEXT, mStressExpectedContent);
    bool setResult = mClipboard.SetData(data);
    if(!setResult)
    {
      ++mStressFailCount;
      SetLastResult("Stress SetData failed");
      return true;
    }

    mStressOutstanding = true;
    mStressOutstandingTicks = 0u;
    mClipboard.GetData(Dali::String(MIME_TYPE_PLAIN_TEXT), this, &ClipboardExample::OnStressClipboardDataReceived);
    UpdateStatus();
    return true;
  }

  void OnStressClipboardDataReceived(bool succeeded, const Dali::ClipboardData& data)
  {
    ++mStressCallbackCount;
    mStressOutstanding = false;
    mStressOutstandingTicks = 0u;

    bool matches = succeeded &&
                   data.GetMimeType() == Dali::String(MIME_TYPE_PLAIN_TEXT) &&
                   data.GetContent() == mStressExpectedContent;
    if(matches)
    {
      ++mStressSuccessCount;
      SetClipboardResult("Stress callback success", data.GetMimeType(), data.GetContent());
    }
    else
    {
      ++mStressFailCount;
      SetClipboardResult(succeeded ? "Stress data mismatch" : "Stress callback failed", data.GetMimeType(), data.GetContent());
    }
  }

  bool OnCopyPlainTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      CopyWithMimeType(MIME_TYPE_PLAIN_TEXT);
    }
    return true;
  }

  bool OnPastePlainTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      PasteWithMimeType(MIME_TYPE_PLAIN_TEXT);
    }
    return true;
  }

  bool OnHasPlainTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      CheckHasType(MIME_TYPE_PLAIN_TEXT);
    }
    return true;
  }

  bool OnCopyHtmlTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      CopyWithMimeType(MIME_TYPE_HTML);
    }
    return true;
  }

  bool OnPasteHtmlTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      PasteWithMimeType(MIME_TYPE_HTML);
    }
    return true;
  }

  bool OnHasHtmlTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      CheckHasType(MIME_TYPE_HTML);
    }
    return true;
  }

  bool OnCopyUriTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      CopyWithMimeType(MIME_TYPE_TEXT_URI);
    }
    return true;
  }

  bool OnPasteUriTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      PasteWithMimeType(MIME_TYPE_TEXT_URI);
    }
    return true;
  }

  bool OnHasUriTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      CheckHasType(MIME_TYPE_TEXT_URI);
    }
    return true;
  }

  bool OnMultiCopyTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      MultiCopyPlainAndHtml();
    }
    return true;
  }

  bool OnMultiPasteTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      MultiPastePlainAndHtml();
    }
    return true;
  }

  bool OnStressStartTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      StartStress();
    }
    return true;
  }

  bool OnStressStopTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      StopStress();
      SetLastResult("Stress stopped");
    }
    return true;
  }

  bool OnClearTouched(Actor, TouchEvent touch)
  {
    if(IsTouchUp(touch))
    {
      StopStress();
      mLastResult = "Cleared";
      mLastMimeType.Clear();
      mLastData.Clear();
      mLastRequestedMimeType.Clear();
      mCallbackCount = 0u;
      mStressRequestedCount = 0u;
      mStressCallbackCount = 0u;
      mStressSuccessCount = 0u;
      mStressFailCount = 0u;
      mStressExpectedContent.Clear();
      ClearMultiState();
      UpdateStatus();
    }
    return true;
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
  }

private:
  Application& mApplication;
  Clipboard    mClipboard;
  InputField   mInputField;
  Label        mResultLabel;
  Label        mMultiResultLabel;
  Timer        mStressTimer;
  Dali::String mLastResult;
  Dali::String mLastMimeType;
  Dali::String mLastData;
  Dali::String mLastRequestedMimeType;
  Dali::String mStressExpectedContent;
  Dali::String mMultiPlainCopyResult;
  Dali::String mMultiHtmlCopyResult;
  Dali::String mMultiPlainPasteResult;
  Dali::String mMultiHtmlPasteResult;
  Dali::String mMultiPlainData;
  Dali::String mMultiHtmlData;
  uint32_t     mCallbackCount{0u};
  uint32_t     mMultiPlainCallbackCount{0u};
  uint32_t     mMultiHtmlCallbackCount{0u};
  uint32_t     mStressRequestedCount{0u};
  uint32_t     mStressCallbackCount{0u};
  uint32_t     mStressSuccessCount{0u};
  uint32_t     mStressFailCount{0u};
  uint32_t     mStressOutstandingTicks{0u};
  bool         mStressRunning{false};
  bool         mStressOutstanding{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  ClipboardExample controller(application);
  application.MainLoop();

  return 0;
}
