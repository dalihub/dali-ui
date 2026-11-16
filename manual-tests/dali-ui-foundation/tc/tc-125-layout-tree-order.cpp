/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali-ui-foundation/integration-api/view-integ.h>
#include "layout-validation-fixtures.h"

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

namespace
{
struct Ordered
{
  StackLayout stack;
  View        stage, sibling, a, b, c, d;
  float       nudge{10};
  void        Nudge()
  {
    sibling.SetRequestedWidth(nudge += 1);
  }
  std::vector<View> Fence() const
  {
    return {stage, stack, a, b, c, sibling};
  }
  /// The child named 'a', 'b', 'c' or 'd'.
  View Child(char name) const
  {
    return name == 'a' ? a : name == 'b' ? b : name == 'c' ? c : d;
  }
  /// Requested width of that child: the fixture input the expected x values are summed from.
  static float Width(char name)
  {
    return name == 'a' ? 20.0f : name == 'b' ? 30.0f : name == 'c' ? 40.0f : 15.0f;
  }
  /// Fence of a pass that lays out exactly the named children (a removed child is left out).
  std::vector<View> Fence(const std::string& children) const
  {
    std::vector<View> views{stage, stack, sibling};
    for(char name : children) views.push_back(Child(name));
    return views;
  }
};
std::shared_ptr<Ordered> MountThree(Run& r)
{
  auto s   = std::make_shared<Ordered>();
  s->stack = StackLayout::New(StackOrientation::HORIZONTAL);
  Fixed(s->stack, 200, 60);
  s->a = Leaf(20, 10, "a");
  s->b = Leaf(30, 10, "b");
  s->c = Leaf(40, 10, "c");
  s->stack.Add(s->a);
  s->stack.Add(s->b);
  s->stack.Add(s->c);
  s->sibling = Leaf(10, 10, "sibling");
  s->sibling.SetRequestedY(70);
  s->stage = r.Stage(200, 90);
  s->stage.Add(s->stack);
  s->stage.Add(s->sibling);
  r.SetState(s);
  return s;
}
/// The "mount" step of the three-step scenarios: 12 rows.
void MountChecked(Run& r)
{
  auto s = MountThree(r);
  r.AfterRender(s->Fence(), [=](Run& r) {
    r.Rendered("initial-a", s->a, LayoutRect(0, 0, 20, 10));
    r.Rendered("initial-b", s->b, LayoutRect(20, 0, 30, 10));
    r.Rendered("initial-c", s->c, LayoutRect(50, 0, 40, 10));
  });
}
/// 2 + 6n rows for n children: both child counts, the layout order and the Actor order of every
/// slot, then each child's rendered rect. x accumulates the requested widths in the expected
/// layout order `logical`; `actor` is the expected Actor order and has the same length.
void CheckOrders(Run& r, const std::shared_ptr<Ordered>& s, const std::string& actor, const std::string& logical)
{
  r.Equal("logical.count", s->stack.GetChildViewCount(), static_cast<int64_t>(logical.size()));
  r.Equal("actor.count", s->stack.GetChildCount(), static_cast<int64_t>(actor.size()));
  for(uint32_t i = 0; i < logical.size(); ++i)
  {
    Identity(r, Id("logical.order", i).c_str(), s->stack.GetChildViewAt(i), s->Child(logical[i]));
    r.Truth(Id("actor.order", i).c_str(), i < s->stack.GetChildCount() && s->stack.GetChildAt(i) == s->Child(actor[i]));
  }
  float x = 0.0f;
  for(char name : logical)
  {
    r.Rendered(std::string(1, name).c_str(), s->Child(name), LayoutRect(x, 0, Ordered::Width(name), 10));
    x += Ordered::Width(name);
  }
}
/// Creates the 15x10 leaf d that the insert/add steps add to the stack.
View NewD(Ordered& s)
{
  s.d = Leaf(Ordered::Width('d'), 10, "d");
  return s.d;
}
/// One three-step scenario: mount -> a ...PreservingLayout() call -> a plain call that either
/// rebuilds the layout order from the Actor order or keeps it.
struct OrderRule
{
  const char* id;         ///< Scenario ID.
  bool        toTop;      ///< "preserve" step: RaiseToTopPreservingLayout(a) (Actor b,c,a) instead of RaisePreservingLayout(a) (Actor b,a,c).
  const char* action;     ///< ID of the last step.
  std::size_t checks;     ///< required_checks of the last step: 2 + 6 * children.
  void (*call)(Ordered&); ///< The call the last step makes.
  const char* actor;      ///< Expected Actor order after it.
  const char* logical;    ///< Expected layout order after it.
};
} // namespace

class Tc125 : public Case
{
public:
  Tc125()
  : Case("TC125", "Tree and order", "Check Raise/Lower/Insert effects on layout order")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(bool preserve : {false, true})
      out.push_back(Steps(preserve ? "ORDER.preserve" : "ORDER.update", "Plain RaiseAbove versus RaiseAbovePreservingLayout, rendered on screen", {
        {"mount", 8, [](Run& r) {
           auto s = MountThree(r);
           r.AfterRender(s->Fence(), [=](Run& r) {
             r.Rendered("initial-a", s->a, LayoutRect(0, 0, 20, 10));
             r.Rendered("initial-b", s->b, LayoutRect(20, 0, 30, 10));
           });
         }},
        {"raise-a-above-c", 10, [preserve](Run& r) {
           auto s = r.State<Ordered>();
           if(preserve)
             Dali::Ui::Extension::View::RaiseAbovePreservingLayout(s->a, s->c);
           else
             s->a.RaiseAbove(s->c);
           s->Nudge();
           r.AfterRender(s->Fence(), [=](Run& r) {
             Identity(r, "first", s->stack.GetChildViewAt(0), preserve ? s->a : s->b);
             Identity(r, "last", s->stack.GetChildViewAt(2), preserve ? s->c : s->a);
             r.Rendered("a", s->a, LayoutRect(preserve ? 0 : 70, 0, 20, 10));
             r.Rendered("b", s->b, LayoutRect(preserve ? 20 : 0, 0, 30, 10));
           });
         }},
      }));
    out.push_back(Single("ORDER.non-view", "A non-View actor is excluded from the logical layout order", 7, [](Run& r) {
      auto s   = std::make_shared<Ordered>();
      s->stack = StackLayout::New(StackOrientation::HORIZONTAL);
      s->a     = Leaf(20, 10, "a");
      s->b     = Leaf(30, 10, "b");
      s->stack.Add(s->a);
      Actor decoration = Actor::New();
      Dali::Ui::Integration::View::AddActorChild(s->stack, decoration);
      s->stack.Add(s->b);
      r.Equal("logical", s->stack.GetChildViewCount(), 2);
      r.Equal("actor", s->stack.GetChildCount(), 3);
      r.Truth("out-of-range", !s->stack.GetChildViewAt(2));
      s->stage = Mount(r, s->stack, 100, 50);
      r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) { r.Rendered("b", s->b, LayoutRect(20, 0, 30, 10)); });
    }));
    const char* const preserveIds[] = {"ORDER.preserve-raise", "ORDER.preserve-lower", "ORDER.preserve-lower-below", "ORDER.preserve-raise-to-top", "ORDER.preserve-lower-to-bottom", "ORDER.preserve-raise-above"};
    for(unsigned operation = 0; operation < 6; ++operation)
      out.push_back(Steps(preserveIds[operation], "A ...PreservingLayout() call changes the actual Actor order while retaining all logical slots", {
        {"mount", 12, [](Run& r) {
           auto s = MountThree(r);
           r.AfterRender(s->Fence(), [=](Run& r) {
             r.Rendered("initial-a", s->a, LayoutRect(0, 0, 20, 10));
             r.Rendered("initial-b", s->b, LayoutRect(20, 0, 30, 10));
             r.Rendered("initial-c", s->c, LayoutRect(50, 0, 40, 10));
           });
         }},
        {"reorder", 20, [operation](Run& r) {
           auto s = r.State<Ordered>();
           switch(operation)
           {
             case 0:
               Dali::Ui::Extension::View::RaisePreservingLayout(s->a);
               break;
             case 1:
               Dali::Ui::Extension::View::LowerPreservingLayout(s->c);
               break;
             case 2:
               Dali::Ui::Extension::View::LowerBelowPreservingLayout(s->c, s->a);
               break;
             case 3:
               Dali::Ui::Extension::View::RaiseToTopPreservingLayout(s->a);
               break;
             case 4:
               Dali::Ui::Extension::View::LowerToBottomPreservingLayout(s->c);
               break;
             default:
               Dali::Ui::Extension::View::RaiseAbovePreservingLayout(s->a, s->c);
               break;
           }
           s->Nudge();
           r.AfterRender(s->Fence(), [=](Run& r) {
             const View logical[]   = {s->a, s->b, s->c};
             const View visual[][3] = {{s->b, s->a, s->c}, {s->a, s->c, s->b}, {s->c, s->a, s->b}, {s->b, s->c, s->a}, {s->c, s->a, s->b}, {s->b, s->c, s->a}};
             r.Equal("logical.count", s->stack.GetChildViewCount(), 3);
             r.Equal("actor.count", s->stack.GetChildCount(), 3);
             for(unsigned i = 0; i < 3; ++i)
             {
               Identity(r, Id("logical.order", i).c_str(), s->stack.GetChildViewAt(i), logical[i]);
               r.Truth(Id("actor.order", i).c_str(), s->stack.GetChildAt(i) == visual[operation][i]);
             }
             r.Rendered("preserved.a", s->a, LayoutRect(0, 0, 20, 10));
             r.Rendered("preserved.b", s->b, LayoutRect(20, 0, 30, 10));
             r.Rendered("preserved.c", s->c, LayoutRect(50, 0, 40, 10));
           });
         }},
      }));
    const OrderRule rules[] = {
      {"ORDER.rebuild-reorder", true, "lower-c", 20, [](Ordered& s) { s.c.Lower(); }, "cba", "cba"},
      {"ORDER.rebuild-insert-existing", false, "insert-c-above-b", 20, [](Ordered& s) { s.stack.InsertAbove(s.c, s.b); }, "bca", "bca"},
      {"ORDER.rebuild-insert-new", false, "insert-d-above-b", 26, [](Ordered& s) { s.stack.InsertAbove(NewD(s), s.b); }, "bdac", "bdac"},
      {"ORDER.keep-add", false, "add-d", 26, [](Ordered& s) { s.stack.Add(NewD(s)); }, "bacd", "abcd"},
      {"ORDER.keep-insert-last", true, "insert-d-above-a", 26, [](Ordered& s) { s.stack.InsertAbove(NewD(s), s.a); }, "bcad", "abcd"},
      {"ORDER.keep-remove", true, "remove-c", 14, [](Ordered& s) { s.stack.Remove(s.c); }, "ba", "ab"},
    };
    for(const OrderRule& rule : rules)
      out.push_back(Steps(rule.id, "After a ...PreservingLayout() call, a later call rebuilds the layout order from the Actor order or keeps it", {
        {"mount", 12, MountChecked},
        {"preserve", 20, [rule](Run& r) {
           auto s = r.State<Ordered>();
           if(rule.toTop)
             Dali::Ui::Extension::View::RaiseToTopPreservingLayout(s->a);
           else
             Dali::Ui::Extension::View::RaisePreservingLayout(s->a);
           s->Nudge();
           r.AfterRender(s->Fence(), [=](Run& r) { CheckOrders(r, s, rule.toTop ? "bca" : "bac", "abc"); });
         }},
        {rule.action, rule.checks, [rule](Run& r) {
           auto s = r.State<Ordered>();
           rule.call(*s);
           s->Nudge();
           r.AfterRender(s->Fence(rule.logical), [=](Run& r) { CheckOrders(r, s, rule.actor, rule.logical); });
         }},
      }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc125)
