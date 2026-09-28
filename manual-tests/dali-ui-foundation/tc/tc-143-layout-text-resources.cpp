/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-fixtures.h"

#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <cmath>

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;
namespace
{
// Existing repository images; their natural sizes are the PNG header dimensions.
constexpr const char* IMAGE_HEARTS = TEST_RESOURCE_DIR "/heartsframe-plain.png";  // 247x167
constexpr const char* IMAGE_LOGO   = TEST_RESOURCE_DIR "/dali-logo-anim-001.png"; // 326x171
// A path that does not exist: the resource fails to load without any asset added for the test.
constexpr const char* IMAGE_ABSENT = TEST_RESOURCE_DIR "/layout-validation-absent.png";

// No expectation depends on a particular font: text sizes are read at run time from the public
// Label API (GetNaturalSize, GetHeightForWidth) or compared with each other.
struct TextFixture
{
  Label                                   label;
  bool                                    armed{false}, windowReady{false}, queued{false};
  uint32_t                                relayouts{0};
  LayoutRect                              target{};
  Vector3                                 firstNatural, previousNatural;
  std::function<void(Run&, TextFixture&)> validate;
};
void QueueTextResult(Run& r, const std::shared_ptr<TextFixture>& s)
{
  if(!s->armed || s->queued || !s->windowReady || s->relayouts == 0) return;
  s->armed  = false;
  s->queued = true;
  // The checks read the Label after the core relayout stack has unwound.
  r.Delay(1, [s](Run& done)
  {
    done.Truth("text.relayout.after-action", s->relayouts > 0);
    s->validate(done, *s);
  });
}
std::shared_ptr<TextFixture> MakeTextFixture(Run& r, Label label)
{
  auto s   = std::make_shared<TextFixture>();
  s->label = label;
  DevelActor::OnRelayoutSignal(label).Connect(&r, [&r, s](Actor)
  {
    if(!s->armed) return;
    ++s->relayouts;
    s->target = LayoutValidation::Bounds(s->label);
    QueueTextResult(r, s);
  });
  r.OnCleanup([s]()
  {s->armed=false;s->validate={}; });
  return s;
}
void ArmText(Run& r, const std::shared_ptr<TextFixture>& s, bool first, std::function<void(Run&, TextFixture&)> validate)
{
  s->armed       = true;
  s->queued      = false;
  s->relayouts   = 0;
  s->windowReady = !first;
  s->validate    = std::move(validate);
  r.Pending();
  if(first)
  {
    r.AfterLayout({s->label}, [s](Run& done)
    {
      s->windowReady = true;
      QueueTextResult(done, s);
      if(!s->queued) done.Pending();
    });
  }
}
// The vertical alignment only places the text in the fixed frame; it must not change the natural size.
void CheckAlignmentIndependent(Run& done, TextFixture& t)
{
  const Vector3 natural = t.label.GetNaturalSize();
  done.Rect("text.target", t.target, LayoutRect(0, 0, 100, 120));
  done.Near("text.natural.alignment-independent.width", natural.x, t.firstNatural.x);
  done.Near("text.natural.alignment-independent.height", natural.y, t.firstNatural.y);
}

struct AsyncImage
{
  ImageView              image;
  bool                   armed{false}, resourceReady{false}, childSeen{false}, queued{false};
  uint32_t               readyCount{0}, windowEpisodes{0};
  int                    width{247}, height{167};
  Visual::ResourceStatus status{};
  LayoutRect             pendingSnapshot{}, frozenSnapshot{};
};
void QueueImageResult(Run& r, const std::shared_ptr<AsyncImage>& s)
{
  if(!s->armed || s->queued || !s->resourceReady || s->windowEpisodes == 0) return;
  s->queued = true;
  // No layout mutation: allow the callback stack and any naturally queued resource
  // layout to settle, retaining newer complete Window snapshots until this read.
  r.Delay(100, [s](Run& done)
  {
    s->armed = false;
    done.Equal("image.ready.count", s->readyCount, 1);
    done.Equal("image.ready.status", static_cast<int>(s->status), static_cast<int>(Visual::ResourceStatus::READY));
    done.Truth("image.request.window-observed", s->windowEpisodes > 0);
    done.Rect("image.async.target", s->frozenSnapshot, LayoutRect(0, 0, s->width, s->height));
    done.Rendered("image.current.target", s->image, LayoutRect(0, 0, s->width, s->height));
    done.Truth("image.async.on.scene", s->image.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));
  });
}
std::shared_ptr<AsyncImage> MakeAsyncImage(Run& r)
{
  auto s   = std::make_shared<AsyncImage>();
  s->image = ImageView::New();
  s->image.SetRequestedWidth(WRAP_CONTENT);
  s->image.SetRequestedHeight(WRAP_CONTENT);
  s->image.ResourceReadySignal().Connect(&r, [&r, s](View)
  {
    if(!s->armed) return;
    ++s->readyCount;
    s->resourceReady = true;
    s->status        = s->image.GetLoadingStatus();
    QueueImageResult(r, s);
  });
  s->image.LayoutFinishedSignal().Connect(&r, [s](View, const LayoutRect& bounds)
  {
    if(!s->armed) return;
    s->pendingSnapshot = bounds;
    s->childSeen       = true;
  });
  LayoutController::Get(r.GetWindow()).LayoutFinishedSignal().Connect(&r, [&r, s](Window)
  {
    if(!s->armed) return;
    if(s->childSeen)
    {
      s->frozenSnapshot = s->pendingSnapshot;
      ++s->windowEpisodes;
    }
    // Never combine one child's observation with a later, unrelated episode.
    s->childSeen = false;
    QueueImageResult(r, s);
  });
  r.OnCleanup([s]()
  { s->armed = false; });
  return s;
}
void ChangeImage(Run& r, const std::shared_ptr<AsyncImage>& s, const char* url, int width, int height, bool attach)
{
  s->width         = width;
  s->height        = height;
  s->armed         = true;
  s->resourceReady = false;
  s->childSeen     = false;
  s->queued        = false;
  s->readyCount = s->windowEpisodes = 0;
  s->pendingSnapshot                = {};
  s->frozenSnapshot                 = {};
  r.Pending();
  s->image.SetResourceUrl(Dali::String(url));
  if(attach) r.Attach(s->image);
}
struct IntrinsicText
{
  Label   label;
  View    stage;
  Vector3 single; ///< Natural size of "A" at 24 px
};
struct SyncImage
{
  ImageView image;
  View      stage;
};
class Tc143 : public Case
{
public:
  Tc143()
  : Case("TC143", "Text and image resources", "Check Label and ImageView intrinsic sizing")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> result;
    // A fixed-size Label is not re-measured by text changes (only WRAP axes are), so each step
    // waits for the core relayout of the changed text and then reads the public natural size.
    result.push_back({"text-model", "Fresh core relayout of a fixed-size Label after text, size and alignment changes; font-independent natural-size relations", {{"single-A", 6, [](Run& r)
    {
      auto label = Label::New("A");
      label.SetFontSize(24);
      label.SetMultiLine(true);
      label.SetRequestedWidth(100);
      label.SetRequestedHeight(120);
      auto s = MakeTextFixture(r, label);
      r.SetState(s);
      ArmText(r, s, true, [](Run& done, TextFixture& t)
      {
        const Vector3 natural = t.label.GetNaturalSize();
        done.Rect("text.target", t.target, LayoutRect(0, 0, 100, 120));
        done.Truth("text.natural.positive", natural.x > 0 && natural.y > 0);
        t.firstNatural = t.previousNatural = natural;
      });
      r.Attach(label);
    }},
                                                                                                                {"two-lines", 6, [](Run& r)
    {
      auto s = r.State<TextFixture>();
      ArmText(r, s, false, [](Run& done, TextFixture& t)
      {
        const Vector3 natural = t.label.GetNaturalSize();
        done.Rect("text.target", t.target, LayoutRect(0, 0, 100, 120));
        done.Truth("text.natural.two-lines.taller", natural.y > t.previousNatural.y);
        t.previousNatural = natural;
      });
      s->label.SetText("A\nA");
    }},
                                                                                                                {"font-size", 7, [](Run& r)
    {
      auto s = r.State<TextFixture>();
      ArmText(r, s, false, [](Run& done, TextFixture& t)
      {
        const Vector3 natural = t.label.GetNaturalSize();
        done.Rect("text.target", t.target, LayoutRect(0, 0, 100, 120));
        done.Truth("text.natural.font.wider", natural.x > t.previousNatural.x);
        done.Truth("text.natural.font.taller", natural.y > t.previousNatural.y);
        t.previousNatural = natural;
      });
      s->label.SetFontSize(36);
    }},
                                                                                                                {"return-A", 7, [](Run& r)
    {
      auto s = r.State<TextFixture>();
      ArmText(r, s, false, [](Run& done, TextFixture& t)
      {
        const Vector3 natural = t.label.GetNaturalSize();
        done.Rect("text.target", t.target, LayoutRect(0, 0, 100, 120));
        done.Near("text.natural.restored.width", natural.x, t.firstNatural.x);
        done.Near("text.natural.restored.height", natural.y, t.firstNatural.y);
        t.previousNatural = natural;
      });
      s->label.SetText("A");
      s->label.SetFontSize(24);
    }},
                                                                                                                {"align-center", 7, [](Run& r)
    {
      auto s = r.State<TextFixture>();
      ArmText(r, s, false, &CheckAlignmentIndependent);
      s->label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    }},
                                                                                                                {"align-end", 7, [](Run& r)
    {
      auto s = r.State<TextFixture>();
      ArmText(r, s, false, &CheckAlignmentIndependent);
      s->label.SetVerticalTextAlignment(Text::Alignment::END);
    }}}});
    // H(w) = Label::GetHeightForWidth(w) and N = Label::GetNaturalSize() are the functions the Label
    // measure itself uses (natural width for a WRAP width; the height for the assigned or measured
    // width), so every expectation is derived at run time from the same public API.
    result.push_back(Steps("text-intrinsic", "Natural metrics, constrained wrapping and intrinsic invalidation rendered on screen", {
      {"natural", 6, [](Run& r) {
         auto s   = std::make_shared<IntrinsicText>();
         s->label = Label::New("A");
         s->label.SetFontSize(24);
         s->label.SetMultiLine(true);
         s->label.SetRequestedWidth(WRAP_CONTENT);
         s->label.SetRequestedHeight(WRAP_CONTENT);
         s->stage = Mount(r, s->label, 300, 300);
         r.SetState(s);
         r.AfterRender({s->stage, s->label}, [=](Run& r) {
           s->single          = s->label.GetNaturalSize();
           const float height = s->label.GetHeightForWidth(s->single.x);
           const auto  a      = s->label.GetMeasuredSize();
           r.Near("intrinsic.A.width", a.width, s->single.x);
           r.Near("intrinsic.A.height", a.height, height);
           r.Rendered("intrinsic.A.rendered", s->label, LayoutRect(0, 0, s->single.x, height));
         });
       }},
      {"narrow", 7, [](Run& r) {
         auto s = r.State<IntrinsicText>();
         s->label.SetText("AAAAAA");
         s->label.SetLineWrapMode(Text::LineWrapMode::CHARACTER);
         s->label.SetRequestedWidth(MATCH_PARENT);
         s->stage.SetRequestedWidth(25);
         r.AfterRender({s->stage, s->label}, [=](Run& r) {
           const auto  narrow = s->label.GetMeasuredSize();
           const float height = s->label.GetHeightForWidth(25);
           r.Near("intrinsic.match.minimum", narrow.width, 0);
           r.Near("intrinsic.narrow.height", narrow.height, height);
           r.Rendered("intrinsic.narrow.rendered", s->label, LayoutRect(0, 0, 25, height));
           r.Truth("intrinsic.narrow.wraps", height > s->single.y);
         });
       }},
      {"wide", 7, [](Run& r) {
         auto s = r.State<IntrinsicText>();
         s->stage.SetRequestedWidth(40);
         r.AfterRender({s->stage, s->label}, [=](Run& r) {
           const auto  wide   = s->label.GetMeasuredSize();
           const float height = s->label.GetHeightForWidth(40);
           r.Near("intrinsic.match.wide.minimum", wide.width, 0);
           r.Near("intrinsic.wide.height", wide.height, height);
           r.Rendered("intrinsic.wide.rendered", s->label, LayoutRect(0, 0, 40, height));
           r.Truth("intrinsic.wide.not-taller", height <= s->label.GetHeightForWidth(25));
         });
       }},
      {"fixed-width", 6, [](Run& r) {
         auto s = r.State<IntrinsicText>();
         s->label.SetRequestedWidth(25);
         s->stage.SetRequestedWidth(300);
         r.AfterRender({s->stage, s->label}, [=](Run& r) {
           const auto  fixed  = s->label.GetMeasuredSize();
           const float height = s->label.GetHeightForWidth(25);
           r.Near("intrinsic.fixed.width", fixed.width, 25);
           r.Near("intrinsic.fixed.height", fixed.height, height);
           r.Rendered("intrinsic.fixed.rendered", s->label, LayoutRect(0, 0, 25, height));
         });
       }},
      {"changed-text", 6, [](Run& r) {
         auto s = r.State<IntrinsicText>();
         s->label.SetText("A");
         r.AfterRender({s->stage, s->label}, [=](Run& r) {
           const auto  changed = s->label.GetMeasuredSize();
           const float height  = s->label.GetHeightForWidth(25);
           r.Near("intrinsic.changed.width", changed.width, 25);
           r.Near("intrinsic.changed.height", changed.height, height);
           r.Rendered("intrinsic.changed.rendered", s->label, LayoutRect(0, 0, 25, height));
         });
       }},
      {"font-size", 7, [](Run& r) {
         auto s = r.State<IntrinsicText>();
         s->label.SetRequestedWidth(WRAP_CONTENT);
         s->label.SetFontSize(36);
         r.AfterRender({s->stage, s->label}, [=](Run& r) {
           const Vector3 natural = s->label.GetNaturalSize();
           const float   height  = s->label.GetHeightForWidth(natural.x);
           const auto    large   = s->label.GetMeasuredSize();
           r.Near("intrinsic.font.width", large.width, natural.x);
           r.Near("intrinsic.font.height", large.height, height);
           r.Rendered("intrinsic.font.rendered", s->label, LayoutRect(0, 0, natural.x, height));
           r.Truth("intrinsic.font.larger", natural.x > s->single.x && natural.y > s->single.y);
         });
       }},
      {"empty", 6, [](Run& r) {
         auto s = r.State<IntrinsicText>();
         s->label.SetText("");
         r.AfterRender({s->stage, s->label}, [=](Run& r) {
           r.Size("intrinsic.empty", s->label.GetMeasuredSize(), MeasuredSize(0, 0));
           r.Rendered("intrinsic.empty.rendered", s->label, LayoutRect(0, 0, 0, 0));
         });
       }},
    }));
    struct ImageCase
    {
      const char* url;
      float       w, h;
    };
    for(const ImageCase& image : {ImageCase{IMAGE_HEARTS, 247, 167}, ImageCase{IMAGE_LOGO, 326, 171}})
    {
      const std::string path = image.url;
      const float       w = image.w, h = image.h;
      result.push_back(Steps("image-" + std::to_string(static_cast<int>(w)), "Natural dimensions and aspect-preserving one-axis requests rendered on screen", {
        {"wrap", 8, [path, w, h](Run& r) {
           auto s   = std::make_shared<SyncImage>();
           s->image = ImageView::New();
           s->image.SetSynchronousLoading(true);
           s->image.SetLoadPolicy(Image::LoadPolicy::IMMEDIATE);
           s->image.SetResourceUrl(Dali::String(path.c_str()));
           s->image.SetRequestedWidth(WRAP_CONTENT);
           s->image.SetRequestedHeight(WRAP_CONTENT);
           const auto natural = s->image.GetNaturalSize();
           r.Near("image.natural.width", natural.x, w);
           r.Near("image.natural.height", natural.y, h);
           s->stage = Mount(r, s->image, 300, 300);
           r.SetState(s);
           r.AfterRender({s->stage, s->image}, [=](Run& r) {
             r.Size("image.wrap", s->image.GetMeasuredSize(), MeasuredSize(w, h));
             r.Rendered("image.wrap.rendered", s->image, LayoutRect(0, 0, w, h));
           });
         }},
        {"fixed-width", 6, [w, h](Run& r) {
           auto s = r.State<SyncImage>();
           s->image.SetRequestedWidth(w * 2);
           r.AfterRender({s->stage, s->image}, [=](Run& r) {
             r.Size("image.fixed.width", s->image.GetMeasuredSize(), MeasuredSize(w * 2, h * 2));
             r.Rendered("image.fixed.width.rendered", s->image, LayoutRect(0, 0, w * 2, h * 2));
           });
         }},
        {"fixed-height", 6, [w, h](Run& r) {
           auto s = r.State<SyncImage>();
           s->image.SetRequestedWidth(WRAP_CONTENT);
           s->image.SetRequestedHeight(h * 3);
           r.AfterRender({s->stage, s->image}, [=](Run& r) {
             r.Size("image.fixed.height", s->image.GetMeasuredSize(), MeasuredSize(w * 3, h * 3));
             r.Rendered("image.fixed.height.rendered", s->image, LayoutRect(0, 0, w * 3, h * 3));
           });
         }},
        {"both-fixed", 7, [](Run& r) {
           auto s = r.State<SyncImage>();
           s->image.SetRequestedWidth(80);
           s->image.SetRequestedHeight(30);
           r.AfterRender({s->stage, s->image}, [=](Run& r) {
             r.Size("image.both.fixed", s->image.GetMeasuredSize(), MeasuredSize(80, 30));
             r.Rendered("image.both.fixed.rendered", s->image, LayoutRect(0, 0, 80, 30));
             r.Equal("image.status", static_cast<int>(s->image.GetLoadingStatus()), static_cast<int>(Visual::ResourceStatus::READY));
           });
         }},
      }));
    }
    result.push_back({"image-async", "Observe this request in either completion order and compare fresh current geometry", {{"load-247", 12, [](Run& r)
    {
      auto state = MakeAsyncImage(r);
      r.SetState(state);
      ChangeImage(r, state, IMAGE_HEARTS, 247, 167, true);
    }},
                                                                                                                            {"replace-326", 12, [](Run& r)
    { ChangeImage(r, r.State<AsyncImage>(), IMAGE_LOGO, 326, 171, false); }},
                                                                                                                            {"return-247", 12, [](Run& r)
    { ChangeImage(r, r.State<AsyncImage>(), IMAGE_HEARTS, 247, 167, false); }}}});
    result.push_back(Steps("image-failure-recovery", "A failed resource cannot corrupt the fixed rendered slot; a valid replacement recovers the intrinsic size", {
      {"invalid", 7, [](Run& r) {
         auto s   = std::make_shared<SyncImage>();
         s->image = ImageView::New();
         s->image.SetSynchronousLoading(true);
         s->image.SetLoadPolicy(Image::LoadPolicy::IMMEDIATE);
         s->image.SetRequestedWidth(80);
         s->image.SetRequestedHeight(30);
         s->image.SetMargin(Insets(3, 0, 5, 0));
         s->image.SetResourceUrl(IMAGE_ABSENT);
         s->stage = Mount(r, s->image, 300, 300);
         r.SetState(s);
         r.AfterRender({s->stage, s->image}, [=](Run& r) {
           r.Size("invalid.fixed", s->image.GetMeasuredSize(), MeasuredSize(80, 30));
           r.Equal("invalid.status", static_cast<int>(s->image.GetLoadingStatus()), static_cast<int>(Visual::ResourceStatus::FAILED));
           r.Rendered("invalid.slot", s->image, LayoutRect(3, 5, 80, 30));
         });
       }},
      {"recovered", 6, [](Run& r) {
         auto s = r.State<SyncImage>();
         s->image.SetResourceUrl(IMAGE_LOGO);
         s->image.SetRequestedWidth(WRAP_CONTENT);
         s->image.SetRequestedHeight(WRAP_CONTENT);
         r.AfterRender({s->stage, s->image}, [=](Run& r) {
           r.Size("recovered.intrinsic", s->image.GetMeasuredSize(), MeasuredSize(326, 171));
           r.Rendered("recovered.rendered", s->image, LayoutRect(3, 5, 326, 171));
         });
       }},
    }));
    return result;
  }
};
} //namespace
REGISTER_MANUAL_TEST(Tc143)
