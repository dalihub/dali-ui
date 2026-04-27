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

#include "manual-test-case.h"

#include <dali-ui-foundation/public-api/animated-image-view.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const ANIM_WEBP  = TEST_RESOURCE_DIR "/dog-anim.webp";
const char* const ANIM_GIF   = TEST_RESOURCE_DIR "/dali-logo-anim.gif";
const char* const ANIM_GIF2  = TEST_RESOURCE_DIR "/animatedLoading.gif";

const char* const DOG_URLS[] = {
  TEST_RESOURCE_DIR "/dog-anim-001.png",
  TEST_RESOURCE_DIR "/dog-anim-002.png",
  TEST_RESOURCE_DIR "/dog-anim-003.png",
  TEST_RESOURCE_DIR "/dog-anim-004.png",
  TEST_RESOURCE_DIR "/dog-anim-005.png",
  TEST_RESOURCE_DIR "/dog-anim-006.png",
  TEST_RESOURCE_DIR "/dog-anim-007.png",
  TEST_RESOURCE_DIR "/dog-anim-008.png",
};
constexpr int DOG_URL_COUNT = 8;

const char* const LOGO_URLS[] = {
  TEST_RESOURCE_DIR "/dali-logo-anim-001.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-002.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-003.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-004.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-005.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-006.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-007.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-008.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-009.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-010.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-011.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-012.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-013.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-014.png",
  TEST_RESOURCE_DIR "/dali-logo-anim-015.png",
};
constexpr int LOGO_URL_COUNT = 15;

constexpr float    PREVIEW_SIZE  = 200.0f;
constexpr float    BTN_H         = 52.0f;
constexpr float    STATUS_H      = 36.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;
} // namespace

/**
 * @brief Verifies AnimatedImageView image source API:
 *   SetResourceUrl / GetResourceUrl
 *   SetResourceUrls / GetResourceUrls
 *
 * Steps:
 *   [SetResourceUrl verification]:
 *   1. [WebP] -> confirm dog-anim.webp plays, GetResourceUrl shown in label
 *   2. [GIF]  -> confirm switch to dali-logo-anim.gif
 *   3. [GIF2] -> confirm switch to animatedLoading.gif
 *
 *   [SetResourceUrls verification]:
 *   1. [URLs: Dog(8)]  -> confirm GetResourceUrls count=8, PNG sequence plays
 *   2. [URLs: Logo(15)] -> confirm GetResourceUrls count=15, more frames play
 *
 * Expected result:
 *   Single URL: GetResourceUrl returns the path that was set.
 *   URL array: GetResourceUrls size matches the count that was set.
 */
class TcAnimatedImageViewSource : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "AnimatedImageView: SetResourceUrl / SetResourceUrls";
  }

  Dali::String GetDescription() const override
  {
    return "Verify source switching between single URL (WebP/GIF) and URL array (PNG sequence); check Get* return values";
  }

  void OnEnter(View contentArea) override
  {
    mView = AnimatedImageView::New(ANIM_WEBP)
              .SetRequestedWidth(PREVIEW_SIZE)
              .SetRequestedHeight(PREVIEW_SIZE);

    mView.Play();

    mUrlLabel   = MakeStatusLabel("URL: dog-anim.webp");
    mCountLabel = MakeStatusLabel("ResourceUrls count: (single URL mode)");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mView));
    content.Add(mUrlLabel);
    content.Add(mCountLabel);

    content.Add(MakeButtonRow({
      MakeButton("WebP", [this] { OnSetUrl(ANIM_WEBP); }),
      MakeButton("GIF",  [this] { OnSetUrl(ANIM_GIF); }),
      MakeButton("GIF2", [this] { OnSetUrl(ANIM_GIF2); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("URLs:\nDog(8)",   [this] { OnSetUrls(DOG_URLS,  DOG_URL_COUNT); }),
      MakeButton("URLs:\nLogo(15)", [this] { OnSetUrls(LOGO_URLS, LOGO_URL_COUNT); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnSetUrl(const char* url)
  {
    mView.SetResourceUrl(url);
    mView.Play();

    Dali::String got = mView.GetResourceUrl();
    std::string  s(got.CStr());
    std::string  filename = s.substr(s.rfind('/') + 1);
    mUrlLabel.SetText(Dali::String("URL: ") + Dali::String(filename.c_str()));
    mCountLabel.SetText("ResourceUrls count: (single URL mode)");
  }

  void OnSetUrls(const char* const* urls, int count)
  {
    Dali::Vector<Dali::String> urlVec;
    for(int i = 0; i < count; ++i) urlVec.PushBack(urls[i]);
    mView.SetResourceUrls(urlVec);
    mView.Play();

    auto got = mView.GetResourceUrls();
    mUrlLabel.SetText("URL: (URL array mode)");
    mCountLabel.SetText(
      Dali::String("ResourceUrls count: ") + Dali::String(std::to_string(got.Size()).c_str()) +
      Dali::String(" (expected ") + Dali::String(std::to_string(count).c_str()) + Dali::String(")"));
  }

  View MakeCentered(View child)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(PREVIEW_SIZE + 16)
                        .SetPadding(Extents(0, 0, 8, 8));
    row.Add(View::New().SetRequestedWidth(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)));
    row.Add(child);
    row.Add(View::New().SetRequestedWidth(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)));
    return row;
  }

  Label MakeStatusLabel(const Dali::String& text)
  {
    return Label::New(text)
      .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(STATUS_H)
      .SetFontSize(12.0f).SetTextColor(UiColor(C_STATUS_TEXT))
      .SetBackgroundColor(UiColor(C_STATUS_BG))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetMultiLine(true);
  }

  View MakeButton(const Dali::String& label, std::function<void()> onClick)
  {
    StackLayout btn = StackLayout::New(StackOrientation::VERTICAL)
                        .SetRequestedHeight(BTN_H)
                        .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                        .SetBackgroundColor(UiColor(C_BTN_BG))
                        .Children({
                          Label::New(label)
                            .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT)
                            .SetFontSize(12.0f).SetTextColor(UiColor(C_BTN_TEXT))
                            .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                            .SetVerticalTextAlignment(Text::Alignment::CENTER)
                            .SetMultiLine(true),
                        });
    btn.SetFocusable(true).AsInteractive([this, onClick = std::move(onClick)](InteractiveTrait& trait) {
      trait.ClickedSignal().Connect(this, [onClick](View, const InputEvent&) -> bool {
        onClick(); return true;
      });
    });
    return btn;
  }

  StackLayout MakeButtonRow(std::initializer_list<View> buttons)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(BTN_H)
                        .SetPadding(Extents(0, 0, 2, 2));
    for(auto& b : buttons) row.Add(b);
    return row;
  }

  AnimatedImageView mView;
  Label             mUrlLabel;
  Label             mCountLabel;
};

REGISTER_MANUAL_TEST(TcAnimatedImageViewSource)
