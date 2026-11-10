# Migration guide: `Dali::Ui::View` sibling-order policy APIs → `Dali::Actor` and `Extension::View` APIs

This guide is for code that calls the `Dali::Ui::View` or `Dali::Ui::ViewImpl` overloads of
`Raise`, `Lower`, `RaiseToTop`, `LowerToBottom`, `RaiseAbove` and `LowerBelow` that take a
`Dali::Ui::LayoutOrderPolicy`. These overloads and the enum are deprecated, and a later dali-ui
removes them; every call that is left then stops compiling. The source locations behind the
statements in this guide are listed in the Evidence section at the end.

**Applies to** a dali-ui whose installed `dali-ui-foundation/extension-api/view.h` declares
`RaisePreservingLayout`:
`grep -n "RaisePreservingLayout" <prefix>/include/dali-ui-foundation/extension-api/view.h` must
print a line. Releases up to 3.0.0.11709 do not have this function yet.

**History.** The overloads exist since at least release 2.5.26.10706. An earlier removal shipped
only in release 2.5.35.11198 and was reverted before release 2.5.35.11201. Release 2.5.46.11678
deprecated them, and the version of this guide in that release recommended
`Actor::Property::DEPTH_INDEX` as the replacement for `PRESERVE`. This version replaces that
advice: every `PRESERVE` call now has an exact replacement among six new
`Dali::Ui::Extension::View::...PreservingLayout()` functions. Code that already moved to
`DEPTH_INDEX` keeps working (section 7).

**Binary compatibility.** The deprecated overloads are 12 exported symbols of
`libdali3-ui-foundation.so.3` (six `View` and six `ViewImpl` members, for example
`Dali::Ui::View::RaiseToTop(Dali::Ui::LayoutOrderPolicy)`). They stay during the deprecation, so
existing binaries keep working. Their removal does not change the SONAME, so a binary, plugin or
prebuilt library that still calls one of them then fails with an `undefined symbol` error, at load
time or at the first call. Rebuild everything that includes dali-ui headers once it is migrated,
and check every prebuilt file that you keep with
`nm -D --undefined-only <file> | grep LayoutOrderPolicy`, which must print nothing. The six new
functions are additional exports; no existing symbol changes.

---

## 1. Quick answer

`v` is a `Dali::Ui::View` and `t` is a `Dali::Ui::View` sibling of `v`.

| Deprecated call | Replacement |
|---|---|
| `v.Raise(LayoutOrderPolicy::UPDATE)`, and likewise `Lower`, `RaiseToTop` and `LowerToBottom` | `v.Raise()` and so on: the inherited `Dali::Actor` method |
| `v.RaiseAbove(t, LayoutOrderPolicy::UPDATE)`, and likewise `LowerBelow` | `if(t) { v.RaiseAbove(t); }` |
| `v.Raise(LayoutOrderPolicy::PRESERVE)` | `Dali::Ui::Extension::View::RaisePreservingLayout(v)` |
| `v.Lower(LayoutOrderPolicy::PRESERVE)` | `Dali::Ui::Extension::View::LowerPreservingLayout(v)` |
| `v.RaiseToTop(LayoutOrderPolicy::PRESERVE)` | `Dali::Ui::Extension::View::RaiseToTopPreservingLayout(v)` |
| `v.LowerToBottom(LayoutOrderPolicy::PRESERVE)` | `Dali::Ui::Extension::View::LowerToBottomPreservingLayout(v)` |
| `v.RaiseAbove(t, LayoutOrderPolicy::PRESERVE)` | `Dali::Ui::Extension::View::RaiseAbovePreservingLayout(v, t)` |
| `v.LowerBelow(t, LayoutOrderPolicy::PRESERVE)` | `Dali::Ui::Extension::View::LowerBelowPreservingLayout(v, t)` |
| Inside a `ViewImpl` subclass: `RaiseToTop(Ui::LayoutOrderPolicy::UPDATE)` | `Self().RaiseToTop()` |
| Inside a `ViewImpl` subclass: `RaiseToTop(Ui::LayoutOrderPolicy::PRESERVE)` | `Dali::Ui::Extension::View::RaiseToTopPreservingLayout(Dali::Ui::View::DownCast(Self()))` |

Each replacement runs the same code as the call it replaces: the deprecated overloads call the
inherited `Dali::Actor` method for `UPDATE` and the matching `...PreservingLayout()` function for
`PRESERVE`. The replacement therefore also behaves the same when the parent is not a `Ui::View`
(both make the plain `Dali::Actor` call), when `v` has no parent (dali-core logs a warning and
nothing changes) and, for `PRESERVE`, when `t` is empty (the call is ignored). The one difference
is an empty `t` under `UPDATE`: the deprecated overload ignores it, while the inherited
`Dali::Actor::RaiseAbove()` and `LowerBelow()` throw `Dali::DaliException`. Keep the `if(t)`
wherever `t` can be empty.

The six functions are declared in `<dali-ui-foundation/extension-api/view.h>`, which
`<dali-ui-foundation/dali-ui-foundation-extension.h>` includes. Write the namespace in full,
`Dali::Ui::Extension::View` (or `Ui::Extension::View` inside `namespace Dali`). Under
`using namespace Dali;` a bare `Extension::` can find dali-adaptor's `Dali::Extension` namespace
instead or be ambiguous with it, and inside a `ViewImpl` subclass it names
`CustomActorImpl::Extension`.

---

## 2. Finding and replacing the calls

### 2.1 Finding them

The deprecation is documented only, so the compiler reports nothing until the overloads are
removed. Search instead, from the root of your repository:

```sh
# 1. Every spelling of the type: calls, constants, parameters, aliases and wrapper functions
#    with a defaulted policy parameter. This search is the authority. Search every file and
#    exclude only build output.
grep -rn --exclude-dir=build --exclude-dir=.git "LayoutOrderPolicy" .
# 2. Direct calls written on one line, with `.` or `->`.
grep -rnE --exclude-dir=build --exclude-dir=.git "(\.|->)(Raise|Lower|RaiseToTop|LowerToBottom|RaiseAbove|LowerBelow)\(.*(UPDATE|PRESERVE)" .
# 3. For every constant, parameter, member or wrapper function that search 1 finds, search its
#    name: the callers of a wrapper pass no policy, so searches 1 and 2 miss them.
```

Search 1 also finds calls that are split across lines, through the line that names the type.
Three patterns need care:

- A policy passed through an `auto` constant (`constexpr auto kKeep = LayoutOrderPolicy::PRESERVE;`
  and `v.RaiseToTop(kKeep);`): after the removal the compiler reports the constant's line but not
  the call. Search 1 finds the constant and search 3 the call.
- A wrapper with a defaulted policy parameter
  (`void BringUp(View v, LayoutOrderPolicy p = LayoutOrderPolicy::PRESERVE)`): once the wrapper is
  migrated, its callers (`BringUp(card);`) compile silently. Decide for each caller whether it
  needs the plain call or the `...PreservingLayout()` call.
- A policy chosen at run time becomes an explicit branch:

```cpp
if(keepLayout)
{
  Dali::Ui::Extension::View::RaiseToTopPreservingLayout(v);
}
else
{
  v.RaiseToTop();
}
```

After the removal GCC reports, for example (recorded with GCC 15.2 for the 2.5.46 version of this
guide; the quote characters depend on the locale):

```
error: 'LayoutOrderPolicy' has not been declared             // LayoutOrderPolicy::X argument or parameter type
error: 'Dali::Ui::LayoutOrderPolicy' has not been declared   // the same, written with Dali::Ui:: or Ui::
error: 'LayoutOrderPolicy' does not name a type              // data member or typed constant
error: 'RaiseToTop' was not declared in this scope           // additionally, inside a ViewImpl subclass
error: no matching function for call to 'Dali::Ui::View::RaiseToTop(int&)'   // policy passed through a parameter
```

### 2.2 `UPDATE` calls

```cpp
// Before                                       // After
v.Raise(LayoutOrderPolicy::UPDATE);             v.Raise();
v.Lower(LayoutOrderPolicy::UPDATE);             v.Lower();
v.RaiseToTop(LayoutOrderPolicy::UPDATE);        v.RaiseToTop();
v.LowerToBottom(LayoutOrderPolicy::UPDATE);     v.LowerToBottom();
v.RaiseAbove(t, LayoutOrderPolicy::UPDATE);     if(t) { v.RaiseAbove(t); }
v.LowerBelow(t, LayoutOrderPolicy::UPDATE);     if(t) { v.LowerBelow(t); }
```

The plain calls compile against every dali-ui release: wherever the overloads exist, `View`
re-exposes the inherited methods next to them (`using Dali::Actor::RaiseToTop;` and so on). As
before, a call on a view without a parent logs
`Actor must have a parent, Sibling order not changed.` and does nothing, `RaiseAbove(t)` and
`LowerBelow(t)` do nothing when `t` is not a sibling, and `RaiseAbove(t)` moves `v` only when it is
below `t` (`LowerBelow(t)` only when it is above `t`).

### 2.3 `PRESERVE` calls

```cpp
#include <dali-ui-foundation/extension-api/view.h>

// Before                                       // After
v.Raise(LayoutOrderPolicy::PRESERVE);           Dali::Ui::Extension::View::RaisePreservingLayout(v);
v.Lower(LayoutOrderPolicy::PRESERVE);           Dali::Ui::Extension::View::LowerPreservingLayout(v);
v.RaiseToTop(LayoutOrderPolicy::PRESERVE);      Dali::Ui::Extension::View::RaiseToTopPreservingLayout(v);
v.LowerToBottom(LayoutOrderPolicy::PRESERVE);   Dali::Ui::Extension::View::LowerToBottomPreservingLayout(v);
v.RaiseAbove(t, LayoutOrderPolicy::PRESERVE);   Dali::Ui::Extension::View::RaiseAbovePreservingLayout(v, t);
v.LowerBelow(t, LayoutOrderPolicy::PRESERVE);   Dali::Ui::Extension::View::LowerBelowPreservingLayout(v, t);
```

No case analysis is needed: the same call is right for every kind of parent, for a view without a
parent and for an empty `t`. `v` must not be empty; the functions assert on an empty handle, as the
deprecated overloads do.

### 2.4 Inside a `ViewImpl` subclass

The `ViewImpl` overloads are deprecated as well. Inside a `ViewImpl` subclass an unqualified
`RaiseToTop(policy)` calls the deprecated `ViewImpl` overload, and an unqualified `RaiseToTop()`
does not compile, now or after the removal, because neither `ViewImpl` nor `Dali::CustomActorImpl`
has a sibling-order method without a policy. Go through the handle that `Self()` returns:

```cpp
// Before (member function of a class derived from Dali::Ui::ViewImpl)
RaiseToTop(Ui::LayoutOrderPolicy::UPDATE);
RaiseToTop(Ui::LayoutOrderPolicy::PRESERVE);
RaiseAbove(target, Ui::LayoutOrderPolicy::PRESERVE);

// After
Self().RaiseToTop();
Dali::Ui::Extension::View::RaiseToTopPreservingLayout(Dali::Ui::View::DownCast(Self()));
Dali::Ui::Extension::View::RaiseAbovePreservingLayout(Dali::Ui::View::DownCast(Self()), target);
```

---

## 3. The two child orders

A `Ui::View` keeps two orders of its children:

- The **actor order** is the order of `Dali::Actor::GetChildAt()`; reading
  `Dali::DevelActor::Property::SIBLING_ORDER` returns a child's index in it. dali-core draws the
  children in this order, later children on top, and hit-tests them top-most first, unless
  `Actor::Property::DEPTH_INDEX` changes that (section 7). The focus finder, the accessibility
  child list and the order of off-screen render tasks (used by render effects) also read the
  actor order.
- The **layout order** is the order of `Ui::View::GetChildViewAt()`, and the index that
  `Ui::View::IndexOfChildView()` returns; the API documentation also calls it the logical order.
  The parent's layout measures and arranges its children in this order; a `StackLayout`, for
  example, places them one after another in it. It contains only `Ui::View` children: non-View
  actors attached through the integration API, and views that are still running their EXIT
  animation after `Remove(child, RemovePolicy::ANIMATE_EXIT)`, are in the actor order only.

Normally the layout order is the actor order without those two kinds of children. A
`...PreservingLayout()` call, like the deprecated `PRESERVE` call before it, changes the actor order
and leaves the layout order as it is, so afterwards the two orders can differ. No call arranges the
layout order on its own: each call either keeps the layout order of the children that stay (a new
child is appended, removed children are dropped) or makes the whole layout order follow the actor
order (section 4). To move a view into another layout slot, reorder it with the plain
`Dali::Actor` calls; the layout order then follows the whole actor order.

---

## 4. When the layout order follows the actor order

dali-ui listens to the parent's `Dali::DevelActor::ChildOrderChangedSignal()`. Whenever dali-core
reports a reorder there, dali-ui rebuilds the parent's whole layout order from its actor order,
keeping only the views that are already in the layout order. It also rebuilds when a `Ui::View`
that the parent did not have is inserted with `InsertAbove()` or `InsertBelow()` and does not end
up as the last child. When a rebuild changes the layout order, the parent re-measures and, if a
`LayoutTransition` is attached to the parent itself, the CHANGE transitions of the children that
move in the next layout pass report `LayoutChangeCause::REORDERED` (children reached only through
an ancestor's `SUBTREE` transition do not get this cause); a newly inserted view keeps its ENTER
handling. When a rebuild changes nothing, nothing else happens.

A `...PreservingLayout()` call changes the actor order exactly like the plain call of the same
name and keeps dali-ui from rebuilding for that one call, so it requests no measure or layout pass.
Its effect lasts until the next rebuild on the same parent, and that rebuild undoes the effect of
every earlier `...PreservingLayout()` call on the parent at once.

| Operation on a child of the parent | Effect on the layout order |
|---|---|
| `Raise()`, `Lower()`, `RaiseToTop()`, `LowerToBottom()`, `RaiseAbove()` or `LowerBelow()`, or a write of `SIBLING_ORDER`, when dali-core reports it (section 5 lists when it does not) | Rebuilt |
| `InsertAbove()` or `InsertBelow()` of a child the parent already has, relative to another of its children | Rebuilt, even when the child stays where it is |
| `InsertAbove()` or `InsertBelow()` of a `Ui::View` the parent does not have yet (a new view, or one taken from another parent) that does not end up last | Rebuilt |
| `Add()`, or `InsertAbove()` of a new view whose target is the last child | The view is appended; the other children keep their layout order |
| Removing a child (`Remove()`, `RemoveAll()` and their `RemovePolicy` forms) | The child leaves the layout order; the others keep theirs |
| A `...PreservingLayout()` call | Unchanged; `ChildOrderChangedSignal` is emitted as for the plain call |

Example: a parent `p` has the `Ui::View` children `a`, `b` and `c`, added in that order, so both
orders are `a, b, c`. Each row starts from that state or from the result of the row it names. The
`...PreservingLayout()` calls are written without their `Dali::Ui::Extension::View::` prefix, and
`d` is a new view that `p` does not have yet.

| Row | Starts from | Call | Actor order | Layout order |
|---|---|---|---|---|
| 1 | `a, b, c` | `RaisePreservingLayout(a)` | b, a, c | a, b, c |
| 2 | `a, b, c` | `LowerPreservingLayout(c)` | a, c, b | a, b, c |
| 3 | `a, b, c` | `RaiseToTopPreservingLayout(a)` | b, c, a | a, b, c |
| 4 | `a, b, c` | `LowerToBottomPreservingLayout(c)` | c, a, b | a, b, c |
| 5 | `a, b, c` | `RaiseAbovePreservingLayout(a, c)` | b, c, a | a, b, c |
| 6 | `a, b, c` | `RaiseAbovePreservingLayout(c, a)`: `c` is already above `a`, nothing moves | a, b, c | a, b, c |
| 7 | `a, b, c` | `LowerBelowPreservingLayout(c, a)` | c, a, b | a, b, c |
| 8 | row 3 | `c.Lower()` | c, b, a | **c, b, a** (rebuilt) |
| 9 | row 1 | `p.InsertAbove(c, b)` | b, c, a | **b, c, a** (rebuilt) |
| 10 | row 3 | `p.InsertAbove(a, c)`: `a` is already directly above `c` | b, c, a | **b, c, a** (rebuilt) |
| 11 | row 1 | `p.InsertAbove(d, b)` | b, d, a, c | **b, d, a, c** (rebuilt) |
| 12 | row 1 | `p.Add(d)` | b, a, c, d | a, b, c, d |
| 13 | row 3 | `p.InsertAbove(d, a)`: `d` ends up last | b, c, a, d | a, b, c, d |
| 14 | row 3 | `a.RaiseToTop()`, `a.Raise()` or `a.RaiseAbove(b)`: `a` is already top-most, nothing is reported | b, c, a | a, b, c |
| 15 | row 3 | `p.Remove(c)` | b, a | a, b |

Row 11 changed with the new functions; before, its layout order was `a, d, b, c` (section 8).

---

## 5. Changes that dali-ui does not rebuild the layout order for

In these cases dali-ui does not rebuild the layout order for the change itself, even while the
layout order differs from the actor order; item 3 explains when the layout order can still pick
the change up:

1. **Calls that dali-core does not report.** dali-core changes and reports nothing for `Raise()`,
   `RaiseToTop()` and `RaiseAbove()` on the top-most child (row 14), for `Lower()`,
   `LowerToBottom()` and `LowerBelow()` on the bottom-most child, for `RaiseAbove()` and
   `LowerBelow()` with a target that is not a sibling, and for these six calls on a view without a
   parent. A write of `SIBLING_ORDER` does nothing when the value is the child's current index, or
   when the child is already top-most and the value is beyond the last index or negative
   (dali-core reads it as an unsigned index). `InsertAbove()` and `InsertBelow()` with a target
   that is not a child of the parent do nothing either: dali-core logs
   `Target Actor must be a child of this actor, insertion not done.` and does not add a new child.
   Outside these cases `RaiseAbove()` and `LowerBelow()` report even when the child does not move,
   and `InsertAbove()` or `InsertBelow()` of a child the parent already has always reports. To make
   the layout order follow the actor order again on purpose, re-insert a child where it already
   is:

   ```cpp
   // Rebuilds p's layout order from its actor order; the actor order does not change.
   if(p.GetChildCount() >= 2u)
   {
     p.InsertBelow(p.GetChildAt(0u), p.GetChildAt(1u));
   }
   ```

2. **A new child that ends up last.** `Add()`, and an insert that cannot be told apart from it (an
   `InsertAbove()` whose target is the last child), only append the child (rows 12 and 13).

3. **A reorder made from a `ChildOrderChangedSignal()` handler of the same parent.** DALi does not
   emit a signal again while it is emitting it (it logs `Cannot call Emit() from inside Emit()`),
   so a reorder of `p`'s children made by a handler connected to
   `DevelActor::ChildOrderChangedSignal(p)` is not reported. dali-ui still sees it only if its own
   handler for the report that is being emitted runs after yours and that report did not come
   from a `...PreservingLayout()` call: handlers run in the order they were connected, and dali-ui
   connects its handler when `p` gets its first `Ui::View` child. Do not rely on either outcome;
   make such reorders outside the handler.

One related case is not an exception: if such a handler inserts a new view into the middle of `p`
while a `...PreservingLayout()` call on one of `p`'s children is running, the layout order is
rebuilt at once, which undoes that call. Adding a view with `Add()` or removing a child from the
handler does not rebuild.

**Containers.** `RecyclerView`, `ScrollView`, `Navigator` and `DialogContainer` reorder some of
their own direct children with the plain calls: `RecyclerView` raises its scroll bar to the top
when it is created and on every arrange, `ScrollView` raises its scroll bar when it is created and
when its content is set, `Navigator` raises its modals above its pages, and `DialogContainer` keeps
the modal content above the scrim. When such a reorder moves something, dali-core reports it and
the container's layout order is rebuilt, which undoes a `...PreservingLayout()` call on a direct
child of the container, as it undid a deprecated `PRESERVE` call. `RecyclerView` items are children
of its internal scroller, not of the `RecyclerView`, so these reorders do not touch them.

---

## 6. While the two orders differ

Take row 3: `RaiseToTopPreservingLayout(a)` left the actor order `b, c, a` and the layout order
`a, b, c`. Until the next rebuild on `p`:

- `p.GetChildAt(2u)` returns `a` and `a`'s `SIBLING_ORDER` is 2, while `p.GetChildViewAt(0u)`
  returns `a` and `p.IndexOfChildView(a)` is 0.
- `a` is drawn above `b` and `c` and receives touches first where it overlaps them, but the layout
  keeps it in the first slot, so no view moves on screen.
- The focus finder and the accessibility child list read the actor order, so a screen reader lists
  `a` after `c`.
- `ChildOrderChangedSignal` was emitted for the call, as for a plain `RaiseToTop()`, and your own
  handlers ran. dali-ui did not rebuild, did not re-measure and reported no `REORDERED` cause.
- The next rebuild on `p` (section 4) moves every child into the layout slot that matches its
  actor position; after `c.Lower()` (row 8) the layout order is `c, b, a`.

---

## 7. `Actor::Property::DEPTH_INDEX` is a separate draw-order control

`DEPTH_INDEX` (`Dali::Actor::SetDepthIndex()` and `GetDepthIndex()`) replaces neither policy. It is
independent of both orders:

- It is a sort key among siblings only. The parent draws its children sorted by it, higher values
  on top (default 0), and children with equal values keep their actor order.
- The child's whole subtree moves with it, and touch hit-testing follows the same order. The
  accessibility lookup of the view at a point (screen-reader touch exploration) walks the actor
  order instead.
- The value belongs to the actor: `Remove()`, `Add()` and reparenting keep it.
- It is not animatable.
- It works only under `Layer::Behavior::LAYER_UI` with the parent's
  `DevelActor::Property::CHILDREN_DEPTH_INDEX_POLICY` at `INCREASE`, which are the defaults.
- Setting it emits no `ChildOrderChangedSignal`, requests no layout pass, and changes neither the
  actor order nor the layout order.

Code that moved to `DEPTH_INDEX` under the 2.5.46 version of this guide keeps working unchanged.

---

## 8. Behaviour changes that come with the new functions

These changes apply whether or not you migrate:

1. A new `Ui::View` inserted with `InsertAbove()` or `InsertBelow()` that does not end up last now
   makes the whole layout order follow the actor order (row 11). Before, only the new view was
   placed at the layout position matching its actor position, and the other children kept their
   layout order; in row 11 that gave `a, d, b, c`. On a parent whose two orders agree the result is
   the same as before. Where it differs, the parent re-measures and, with a `LayoutTransition`
   attached to the parent, the children that move report `LayoutChangeCause::REORDERED`; the new
   view's ENTER handling is unchanged.
2. A child that a `ChildOrderChangedSignal()` handler adds or removes while a `PRESERVE` or
   `...PreservingLayout()` call is running now enters or leaves the layout order. Before, such an
   added child was missing from it (`IndexOfChildView()` returned -1), and such a removed child
   stayed in it.
3. A view that a callback removes again while dali-core is still attaching it to its new parent
   (for example the view's own `SceneConnectedSignal()` handler) is no longer kept in that
   parent's layout order.

---

## 9. Checklist

1. Searches 1 to 3 of section 2.1 find no remaining use of the deprecated API (search 3 may also
   find unrelated names).
2. After the removal the code compiles against the new dali-ui, every binary, plugin and prebuilt
   library that includes dali-ui headers was rebuilt, and
   `nm -D --undefined-only <file> | grep LayoutOrderPolicy` prints nothing for each prebuilt file
   that you keep.
3. Every `UPDATE` call is now the inherited `Dali::Actor` call, with `if(t)` in front of
   `RaiseAbove(t)` and `LowerBelow(t)` wherever `t` can be empty.
4. Every `PRESERVE` call is now the matching `Dali::Ui::Extension::View::...PreservingLayout()`
   call with the same view and target, written with `Dali::Ui::View::DownCast(Self())` inside a
   `ViewImpl` subclass.
5. Policies chosen at run time are explicit branches, and the callers of wrappers that had a
   defaulted policy parameter were checked.
6. Tests that check `GetChildViewAt()` or `IndexOfChildView()` after inserting a new view into the
   middle of a parent with an earlier `PRESERVE` or `...PreservingLayout()` call expect the
   rebuilt order (section 8, item 1).

---

## Evidence

Verified on 2026-11-10 against the dali-ui working tree with this change (on top of `ff742fa3`),
dali-core 2.5.47 at `538b3ecdb` and dali-adaptor 2.5.47 at `5bceea653`. dali-ui paths are relative
to the repository root, and the line numbers are those of the change that added the
`...PreservingLayout()` functions; they drift in later trees.

dali-ui:

- `dali-ui-foundation/extension-api/view.h:477-539` — `RaisePreservingLayout()` and its documentation of both orders, of the calls that rebuild the layout order and of those that do not, and of a reorder made from a `ChildOrderChangedSignal()` handler (`:519-523`); `:541-652` — the other five declarations.
- `dali-ui-foundation/extension-api/view.cpp:201-229` — the six functions forward to `ViewDataImpl` through `Ui::GetImpl(view)`, which asserts on an empty handle (`dali-ui-foundation/public-api/views/view-impl.h:1394-1401`, assertion at `:1396`).
- `dali-ui-foundation/internal/views/view/view-data-impl.cpp:130-171` — `ScopedSkipChildrenUpdate`: looks up the view's parent and, only when it is a `Ui::View`, sets the parent's `mSkipChildrenUpdate` for the call (`:146-153`) and restores it afterwards (`:156-162`).
- `view-data-impl.cpp:3920-3969` — the six `ViewDataImpl::...PreservingLayout()` members: the guard plus the inherited `Actor` call; the target forms return on an empty target first (`:3951-3954`, `:3963-3966`).
- `view-data-impl.cpp:3852-3918` — the deprecated `Raise(policy)` … `LowerBelow(target, policy)`: `PRESERVE` calls the member above and `UPDATE` the inherited `Actor` method; `RaiseAbove` and `LowerBelow` return on an empty target first (`:3894-3897`, `:3908-3911`). They are reached from `dali-ui-foundation/public-api/views/view.cpp:700-728` and `dali-ui-foundation/public-api/views/view-impl.cpp:690-718`.
- `view-data-impl.cpp:7302-7312` — `OnChildOrderChanged`: returns while a `...PreservingLayout()` call runs (`:7306-7309`) and otherwise rebuilds (`:7311`).
- `view-data-impl.cpp:7314-7384` — `SyncChildrenWithActorOrder`: rebuilds from `GetChildAt()`, keeping only tracked views (`:7321-7335`), returns when nothing changed (`:7355-7359`), assigns (`:7361`), tags the children `REORDERED` when a `LayoutTransition` is attached (`:7370-7376`) and invalidates the measure (`:7383`).
- `view-data-impl.cpp:3971-4249` — `OnChildAdded`: ignores a view that is no longer its child (`:3985-3988`); computes the layout position (`:3996-3997`; `ComputeLogicalChildIndex` at `:3807-3850`, last-child fast path at `:3818-3822`) and inserts the view there (`:3998-4005`); connects the order handler with the first `Ui::View` child (`:4021-4025`, the only connection in the library); rebuilds when the view is not the last actor child (`:4234-4237`). Called from `dali-ui-foundation/public-api/views/view-impl.cpp:1077-1080`.
- `view-data-impl.cpp:4251-4320` — `OnChildRemoved` erases the child (`:4312`) without a rebuild; `:3680-3805` — `Remove(child, policy)`: with an EXIT effect the child leaves the layout order at once (`:3748`) and stays an actor child.
- `dali-ui-foundation/internal/views/view/view-data-impl.h:569-574` (the six members), `:1410` (`ComputeLogicalChildIndex`), `:1444` (`SyncChildrenWithActorOrder`), `:1983-1986` (`HasLayoutTransition`: a transition attached to this view), `:2130` (`mSkipChildrenUpdate`).
- `dali-ui-foundation/public-api/views/view.h:1813-1834` and `:1836-1852` — `GetChildViewAt` and `IndexOfChildView`, whose logical order can differ from the actor order (`:1823-1828`, `:1848-1850`); `:1900-1905` — `using Dali::Actor::Lower` … `RaiseToTop`; `:1948-2064` — the deprecated `View` overloads.
- `dali-ui-foundation/public-api/views/view-impl.h:91` — `ViewImpl` derives from `CustomActorImpl`; `:757-850` — the deprecated `ViewImpl` overloads.
- `dali-ui-foundation/public-api/views/view-types.h:41-61` — `LayoutOrderPolicy` and its deprecation note.
- `dali-ui-foundation/dali-ui-foundation-extension.h:36` — includes `extension-api/view.h`.
- `dali-ui-foundation/public-api/layouts/layout-transition-types.h:75` — `LayoutChangeCause::REORDERED`; `dali-ui-foundation/internal/layouts/layout-transition-dispatcher.cpp:1027` — ENTER is handled before the change cause is resolved (`:1108-1109`, `REORDERED` first at `:313-316`); a child whose bounds do not change gets no CHANGE transition (`:1103-1106`); a child reached only through an ancestor's `SUBTREE` transition gets `OTHER` or `WINDOW_RESIZED` (`:1004-1009`).
- `dali-ui-foundation/public-api/layouts/stack-layout-manager.cpp:436-441, 651-656` — `StackLayout` measures and arranges its children in `GetChildViewAt()` order.
- `dali-ui-foundation/internal/focus-manager/focus-finder.cpp:351-354, 450-459` — the focus finder walks `GetChildCount()` and `GetChildAt()`.
- `dali-ui-foundation/internal/render-effects/mask-effect-impl.cpp:117-120`, `gaussian-blur-effect-impl.cpp:127-130`, `offscreen-rendering-impl.cpp:67-70` — render effects are off-screen renderables, whose render tasks dali-core orders by the actor order (see dali-core below).
- `dali-ui-foundation/integration-api/recycler/recycler-view-impl.cpp:241-242, 717` (scroll bar raised at creation and on arrange), `:731-732` (scroller lowered once), `:865, 882` (items are added to the scroller); `dali-ui-foundation/integration-api/scroll-view-impl.cpp:147, 197, 200`; `dali-ui-components/integration-api/navigator/navigator-impl.cpp:626-634` (`RestackModals`); `dali-ui-components/integration-api/dialog/dialog-container-impl.cpp:218, 384`.
- `git grep LayoutOrderPolicy`: outside the declarations and their forwarding functions only `automated-tests/src/dali-ui-foundation-internal/utc-Dali-View-order-internal.cpp` uses it; no library, component, sample or manual-test code does. In the library and component code no statement uses `Actor::Property::DEPTH_INDEX` or `Dali::Actor::SetDepthIndex()` (`git grep` finds only comments: the documentation in `extension-api/view.h` and three commented-out lines in `dali-ui-foundation/internal/text/decorator/text-decorator.cpp:870, 948, 992`); the library sets only renderer-level depth indices, through `Visual::Base::SetDepthIndex()` and direct `Renderer::Property::DEPTH_INDEX` writes (for example `dali-ui-foundation/internal/render-effects/background-blur-effect-impl.cpp:484` and `gaussian-blur-effect-impl.cpp:481`), and it has no handling of `SIBLING_ORDER` (`git grep` finds only documentation and `DevelLayer::SIBLING_ORDER_MULTIPLIER`).
- Tests in `automated-tests/src/dali-ui-foundation-internal/utc-Dali-View-order-internal.cpp` (found by name; their lines move as the file grows): `UtcDaliView*UpdateInternalP` and `UtcDaliView*PreserveInternalP` (the deprecated overloads), `UtcDaliViewDepthIndexDoesNotChangeChildOrderInternalP` (a depth index changes neither order and emits no order signal), `UtcDaliViewInsert*` (inserts into parents whose two orders agree), `UtcDaliView*PreservingLayout*InternalP` (rows 1-15, items 1 and 2 of section 5 and the related case after them, a mid insert of a view taken from another parent, nested `...PreservingLayout()` calls, section 8, item 2), `UtcDaliViewReorderInChildOrderChangedCallbackInternalP` (item 3 of section 5, both handler orders), `UtcDaliViewDeprecatedPreserveMatchesPreservingLayoutInternalP` (the deprecated `PRESERVE` overloads and the new functions give the same orders), `UtcDaliViewFreshMidInsertSyncTagsReorderedInternalP` and `UtcDaliViewFreshMidInsertInSyncDoesNotTagReorderedInternalP` (section 8, item 1) and `UtcDaliViewChildRemovedDuringSetParentNotTrackedInternalP` (section 8, item 3); in `automated-tests/src/dali-ui-foundation/utc-Dali-View.cpp`: `UtcDaliViewExtensionPreservingLayoutP` and `UtcDaliViewExtensionPreservingLayoutEmptyHandleN`.
- Release history (`git log`, `git merge-base --is-ancestor`): `4a237fb4` (Release 2.5.26.10706) declares the enum at `dali-ui-foundation/public-api/view-types.h:44`; the removal `b62b35f0` is in `dabae505` (Release 2.5.35.11198) and its revert `908f9ec0` in `ed0f889e` (Release 2.5.35.11201); the deprecation `e07c3368` is in `c5ba66f2` (Release 2.5.46.11678) and not in `fb7895b0` (Release 2.5.45.11629); `c5ba66f2:dali-ui-foundation/extension-api/view.h` has no `PreservingLayout`, and neither has that file in `1f5a631d` (Release 3.0.0.11709), which still declares the overloads. Every release from 2.5.26.10706 to 3.0.0.11709 that has the overloads also has `using Dali::Actor::RaiseToTop;`, and 2.5.35.11198 has neither.
- Binary compatibility: `nm -DC` on an existing build of `libdali3-ui-foundation.so.3.0.0` that has the overloads lists 12 `LayoutOrderPolicy` symbols, and `readelf -d` shows `SONAME libdali3-ui-foundation.so.3`; `build/tizen/dali-ui-foundation/CMakeLists.txt:2, 179-184` (`dali${DALI_API_MAJOR}-ui-foundation`, `SOVERSION ${DALI_UI_VERSION_MAJOR}`) and `build/tizen/CMakeLists.txt:10-12, 23` (`DALI_API_MAJOR` 3).

dali-core:

- `dali/internal/event/actors/actor-parent-impl.cpp:69-144` — `AddChild` first removes a child that has another parent from that parent (`:85-94`), then places the child at its final position (next to the anchor at `:102-111`, appended at `:115`), then calls `SetParent` (`:126`) and `OnChildAdd` (`:131`) and emits `ChildAdded` (`:132`) without checking again that the child is still its child; it emits no order signal.
- `actor-parent-impl.cpp:146-197` — `Remove` erases the child (`:173`) and calls `OnChildRemove` (`:194`); no order signal.
- `actor-parent-impl.cpp:302-332` — `SetSiblingOrderOfChild` implements a `SIBLING_ORDER` write with the moves below; `:334-348` — `GetSiblingOrderOfChild` is the value read back.
- `actor-parent-impl.cpp:351-374, 376-399, 401-421, 423-443` — `RaiseChild`, `LowerChild`, `RaiseChildToTop` and `LowerChildToBottom` report only after a move, and do nothing for the top-most or bottom-most child (`:354, :379, :404, :426`).
- `actor-parent-impl.cpp:445-470, 472-496` — `RaiseChildAbove` and `LowerChildBelow` do nothing for the top-most or bottom-most child or a target that is not a sibling (`:448, :477`), and otherwise report even without a move (`:463, :489`).
- `actor-parent-impl.cpp:498-540` — `InsertChild` asserts that child and target differ (`:500`); a target that is not a child logs a warning and returns (`:503-507`); a child the parent does not have goes through `AddChild` with the target as anchor (`:509-516`); a child the parent has is moved and always reported (`:539`).
- `actor-parent-impl.cpp:781-792` — `EmitOrderChangedAndRebuild`; `:39-46` — `EmitSignal` emits only when a handler is connected.
- `actor-parent-impl.cpp:542-578` — the depth tree is built depth-first over the children in depth-index order (`:555`, `INCREASE` policy at `:557`); `:580-608` — `GetChildrenInDepthOrder` stable-sorts the children by depth index (`:597`).
- `dali/internal/event/actors/actor-impl.cpp:373-396` — without a parent the sibling-order calls log `Actor must have a parent, Sibling order not changed.` (`:381, :394`); `:1987-2015` — `Actor::Raise()` … `LowerBelow()`; `:1018-1040` — `SetDepthIndex` does not touch the child list and emits no order signal; `:125` — `depthIndex` is writable and not animatable; `:2085-2147` — `SetParent` connects the child to the scene (`:2101`) through `ConnectToScene` (`:1735-1756`, `:1752`) and `NotifySceneConnection` (`:1778-1802`; `OnSceneConnectionExternal` at `:1787`, `SceneConnectedSignal` at `:1792`).
- `dali/internal/event/actors/actor-impl.h:2663-2670` — `GetImplementation` asserts `Actor handle is empty` (`:2665`), reached through `dali/public-api/actors/actor.cpp:870-878` (`RaiseAbove`, `LowerBelow`); `dali/public-api/common/dali-common.h:188-193` — `DALI_ASSERT_ALWAYS` throws `Dali::DaliException`.
- `dali/internal/event/actors/actor-property-handler.cpp:515-527` — a `SIBLING_ORDER` write calls `SetSiblingOrderOfChild`; `:1475-1485` — reading it returns `GetSiblingOrderOfChild`.
- `dali/public-api/signals/base-signal.h:266-308` — `Emit`: a nested `Emit` on the same signal returns at once (`:269-274`), and handlers run in connection order (`:283-303`); `dali/public-api/signals/base-signal.cpp:410-423` — the nested call logs `Cannot call Emit() from inside Emit()` (`:421`); `:119-146`, `:163-198` — `OnConnect` appends every new handler to the emit list (`:130`, `:179`) through `AppendToEmitList` (`:337-354`), which links it at the end.
- `dali/devel-api/actors/actor-devel.h:108` (`SIBLING_ORDER`), `:253` (`ChildOrderChangedSignalType`: parent and moved child), `:271` (`ChildOrderChangedSignal`), `:130` and `:205-211` (`CHILDREN_DEPTH_INDEX_POLICY`, default `INCREASE`).
- `dali/public-api/actors/actor.h:619-630` (`DEPTH_INDEX`), `:1011, :1029` (`InsertAbove`, `InsertBelow`), `:2191, :2200` (`SetDepthIndex`, `GetDepthIndex`), `:2632-2681` (`Raise` … `LowerBelow`), `:2957` (`SceneConnectedSignal`); `dali/public-api/actors/custom-actor-impl.h:74` (`CustomActorImpl::Extension`), `:90` (`Self()`).
- `dali/internal/event/events/hit-test-algorithm-impl.cpp:466-476` — hit-testing walks the children in depth-index order, top-most first; `dali/internal/event/render-tasks/render-task-list-impl.cpp:253-258` — off-screen renderables are found by walking `GetChildAt()`; `dali/internal/event/actors/layer-impl.cpp:132` — layers default to `LAYER_UI`.

dali-adaptor:

- `dali/devel-api/adaptor-framework/actor-accessible.cpp:102` — the accessible object listens to `ChildOrderChangedSignal`; `:267-278` — it lists its children in `GetChildAt()` order.
- `dali/internal/accessibility/bridge/bridge-accessible.cpp:559` — the at-point lookup uses hit-testing only under `LAYER_3D`; `:597-602` — otherwise it walks the accessible children in reverse.
- `dali/extension-api/adaptor-framework/adaptor-extension.h:31` — `namespace Extension` inside `Dali`.
