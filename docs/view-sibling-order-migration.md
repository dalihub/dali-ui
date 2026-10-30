# Migration guide: `Dali::Ui::View` sibling-order policy APIs → `Dali::Actor` APIs

Audience: application developers (and automated migration tools) updating **application code** that uses the
deprecated `Dali::Ui::View` / `Dali::Ui::ViewImpl` sibling-order overloads taking a
`LayoutOrderPolicy`, which a later dali-ui removes. Every claim below was checked against the
sources listed in the Evidence appendix; re-check there if something looks wrong for your
dali-core/dali-ui version.

Applies to: a dali-ui whose `dali-ui-foundation/public-api/views/view-types.h` declares
`enum class LayoutOrderPolicy` with a `@deprecated` note (the overloads still work unchanged and
will be removed in a later dali-ui), and every later dali-ui whose header no longer declares the
enum. Identify the version by that header, **not** by the commit title or by the version number:
the overloads existed at least since 2.5.26 (then declared in
`dali-ui-foundation/public-api/view-types.h`, before the headers moved under `views/`); an earlier
removal (`b62b35f0`, "Remove View sibling-order APIs duplicating Dali::Actor") shipped only in
release 2.5.35.11198 and was reverted by `908f9ec0` before 2.5.35.11201; every release from
2.5.35.11201 up to and including 2.5.45.11629 has the overloads without a deprecation note.
(Whether this guide applies unchanged to 2.5.35.11198 was not checked.) Requires dali-core >= 2.5.35
(`Actor::Property::DEPTH_INDEX` and `Actor::InsertAbove/InsertBelow` are `@SINCE_2_5.35`).
`pkg-config --modversion dali3-core` prints the library version `3.0.0`, not the release number,
and the release numbers are run-time constants (`CORE_MAJOR_VERSION` etc.), so check the dali-core
headers instead: `grep -n "void SetDepthIndex" <prefix>/include/dali/public-api/actors/actor.h`
must hit. The migrated code (plain inherited calls plus `SetDepthIndex`) compiles and behaves the
same against every dali-ui that has the overloads, deprecated or not (`View` re-exposes the
inherited methods with `using Dali::Actor::RaiseToTop;` etc.), so migrate now: the removal turns
every remaining call into a compile error.

**Binary compatibility:** the deprecated overloads are exported out-of-line symbols of
`libdali3-ui-foundation.so.3` (12 symbols, e.g. `Dali::Ui::View::RaiseToTop(Dali::Ui::LayoutOrderPolicy)`).
They stay during the deprecation, so existing binaries keep working. The removal does not change
the SONAME: a binary, plugin or prebuilt third-party library built against headers that still have
the overloads then loads against the new library and fails with an `undefined symbol` error, at
load time or at the first call. Rebuild everything that includes dali-ui headers once it is
migrated; check prebuilt artifacts with `nm -D --undefined-only <file> | grep LayoutOrderPolicy` (§7).

---

## 0. Quick answer

| # | Old call | Parent of the view | New call |
|---|---|---|---|
| 1 | `v.Raise(LayoutOrderPolicy::UPDATE)` and the other five `UPDATE` overloads | any | `v.Raise()` etc.: the **inherited `Dali::Actor`** method, drop the argument. Same code path. One exception: `RaiseAbove`/`LowerBelow` whose target **can be empty** need `if(t)` (the deprecated overloads ignore an empty target, the inherited calls throw), see §3.1. |
| 2 | `v.RaiseToTop(LayoutOrderPolicy::PRESERVE)` etc. | **not** a `Dali::Ui::View` (window root layer via `Window::Add`, a plain `Dali::Actor`/`Layer`) | `v.RaiseToTop()` etc.: the deprecated code only does something different when the parent is a `Ui::View`. Same empty-target exception as row 1. The dali-ui migrated its own two window-parented drag views this way (the grid sample's drag preview and the reorder sample's floated dragged card). |
| 3 | `v.RaiseToTop(LayoutOrderPolicy::PRESERVE)` etc. | an **application-owned** `Dali::Ui::View` layout container (not one of the dali-ui containers listed in §6) | `v.SetDepthIndex(n)` (`Actor::Property::DEPTH_INDEX`), reset with `v.SetDepthIndex(0)` when the lift is over. See §3.2 / §5. |
| 4 | `v.RaiseToTop(LayoutOrderPolicy::PRESERVE)` etc. | a dali-ui container that stacks its own children by sibling order (`RecyclerView`, `ScrollView`, `Navigator`, `DialogContainer`) | Do **not** use `DEPTH_INDEX` on their direct children. The old raise only moved the child within the sibling order, and the container re-asserts its own stacking (scroll bar on top, modals above pages, content above scrim) on its next restack; a `DEPTH_INDEX` lift outranks every such restack until the application resets it. Use the container's own API, or keep a plain `RaiseToTop()` (§6). **This row wins over rows 3, 6 and 7.** RecyclerView **item** views (children of the internal scroller) may be lifted with a positive index, reset on bind (§5.1, §6). |
| 5 | `RaiseToTop(PRESERVE)` immediately after a **fresh `Add`** of the same child (the child was not already in that parent) | any | Delete the call: `Actor::Add` appends, so the child was already last and the old call moved nothing. |
| 6 | `RaiseToTop(PRESERVE)` after `InsertAbove`/`InsertBelow` of the child, or after `Add` of a child that was **already** in that parent | any | Usually **not** a no-op: the inserted child sits next to its anchor (a re-`Add` of an existing child does nothing), so the old call brought it to the front while a `Ui::View` parent kept its inserted layout slot. Exception: when the child provably ended up last anyway (`InsertAbove` with the last child as anchor, an `InsertAbove`/`InsertBelow` whose anchor had vanished so dali-core appended, a re-`Add` of the last child) the old call moved nothing; treat that as row 5. Application-owned `Ui::View` parent: keep the insert and add `v.SetDepthIndex(n)` (exactly what dali-ui's reorder sample does after `InsertBelow`); §6 container parent: row 4; other parent: plain `v.RaiseToTop()`. |
| 7 | shared helper whose parent type varies at run time | — | branch on the parent type at run time, with the §6 containers excluded from the `DEPTH_INDEX` branch; see §3.2 `BringToFront`. |
| 8 | inside a `ViewImpl` subclass: `RaiseToTop(LayoutOrderPolicy::X)` | — | `Self().RaiseToTop()` / `Self().SetDepthIndex(n)`; the `ViewImpl` overloads are deprecated as well, and `CustomActorImpl` has no sibling-order members. See §3.3. |
| 9 | "Change the View's layout order but keep the actor sibling order" | — | **Not available through the public application API, and the deprecated overloads never did it either.** The layout order is derived from the actor sibling order (§1). Reorder the actor (changes both) or use `DEPTH_INDEX` (changes neither). |

The rows are not mutually exclusive. Precedence when more than one matches: rows 1 and 8 are
about the call form; among the `PRESERVE` rows the parent decides first (row 2 for a non-View
parent, row 4 for a dali-ui container, both win over rows 3, 6 and 7), then row 5 (fresh `Add`,
including row 6's "ended up last anyway" exception) wins over row 3. One more case cuts across
rows 3, 6 and 7: a call on a view that has **no parent at call time** (e.g. "configure, then
`Add`" code, or a RecyclerView bind handler, which runs before the item is added to the
scroller) was a logged no-op; `SetDepthIndex` is not (the value is stored and applies at the
later `Add`). Delete such calls or guard them with `if(v.GetParent())` (§3.2 rule 0).

So: **yes, `SetDepthIndex` is the right replacement for the `PRESERVE` overloads whenever the
application used them for what they actually delivered: a z-order ("bring to front") change
inside an application-owned dali-ui container that leaves the layout slot untouched.** Section 4
lists the behavioural differences you must handle (the value is persistent, also across
`Remove`/`Add` and RecyclerView recycling; ties fall back to sibling order, so repeated raises
among overlapping siblings need increasing values or the §5.2 helpers rather than one shared
constant; the actor sibling order no longer changes, which the accessibility at-point lookup and
RenderEffect task ordering still read). A common application pattern, a focused or a
user-selected item lifted above its neighbours in a horizontal or vertical `StackLayout`, is worked
through in §5.5 (focus-driven, selection-driven, and both at once).

---

## 1. The three orders you must keep apart

| Order | Owner | Query | Changed by |
|---|---|---|---|
| **A. Actor sibling order** | dali-core (`ActorParentImpl::mChildren`) | `Dali::Actor::GetChildCount()/GetChildAt(i)` | `Add`, `Remove`, `Raise`, `Lower`, `RaiseToTop`, `LowerToBottom`, `RaiseAbove`, `LowerBelow`, `InsertAbove`, `InsertBelow`, and the writable property `DevelActor::Property::SIBLING_ORDER` (it is implemented with the same Raise/Lower moves, so it is **not** a draw-order-only control). |
| **B. View logical (layout) order** | dali-ui (`ViewDataImpl::mChildren`) | `Dali::Ui::View::GetChildViewCount()/GetChildViewAt(i)/IndexOfChildView(v)` | **Follows A automatically.** dali-ui connects `DevelActor::ChildOrderChangedSignal` and rebuilds B from `Actor::GetChildAt` order whenever the rebuilt View-only sequence differs from the current one; it then invalidates measure and, if a `LayoutTransition` is attached, tags the children `LayoutChangeCause::REORDERED`. Emissions that move no View (a `RaiseAbove` of a child already above its target but not yet top-most, a reorder among non-View actors) return early and do nothing as long as B matches the View sequence of A, which only a deprecated `PRESERVE` call breaks (see below). B excludes non-View actors and in-flight EXIT ghosts, nothing else. |
| **C. Draw + hit-test order** | dali-core depth tree | `GetDepthIndex()` per child | Equals A unless a child overrides `Actor::Property::DEPTH_INDEX`: children are stable-sorted by depth index (ascending = drawn later = on top); equal values keep order A. Touch/hover/wheel/gesture hit-testing walks the same order top-most first. |

Other consumers of A that applications rarely think about (none of them reads `DEPTH_INDEX`):

- dali-ui's focus finder walks `Actor::GetChildAt` (candidates are then chosen by geometry, §4).
- The accessibility bridge reports children in A order **and** resolves "which accessible is at
  this point" (screen-reader touch exploration, `GetNavigableAtPoint`) by walking the accessible
  children in **reverse A order**; it uses the real hit-test algorithm only under `LAYER_3D`.
- The execution order of off-screen render tasks (dali-ui RenderEffects such as
  `BackgroundBlurEffect`, `GaussianBlurEffect`, `OffscreenRendering`) is computed by walking
  `Actor::GetChildAt`.

What the deprecated overloads really do (from their implementation, see the Evidence appendix):

- `UPDATE`: calls the inherited `Dali::Actor` method. Nothing else. → A changes, B resyncs, C follows.
- `PRESERVE` **with a `Ui::View` parent**: calls the same inherited method while the parent's
  A→B resync is suppressed for that one call. → A and C change, B keeps its previous order,
  **but only until the next `ChildOrderChangedSignal` on that parent that is emitted outside a
  `PRESERVE` call**. Because B then no longer matches the View sequence of A, that next emission
  rebuilds B from A even when it moves no View: a plain `Raise`/`Lower`/`RaiseToTop`/
  `LowerToBottom`/`RaiseAbove`/`LowerBelow` that moves something, but also a `RaiseAbove`/
  `LowerBelow` that moves nothing (dali-core emits whenever the child is not already at that end
  and the target is a sibling), an `InsertAbove`/`InsertBelow` of an already-parented sibling
  even at its current position, and a reorder among non-View actors. The "preserved" layout
  order then silently snaps to the actor order. In 2.5.35 and 2.5.36 the handler had no
  equality check at all, so there every unsuppressed emission rebuilt B. No emission happens,
  so B stays stale, for `Raise`/`Lower`/`RaiseToTop`/`LowerToBottom` of a child already at that
  end, for `RaiseAbove`/`LowerBelow` of a child already top-most/bottom-most or with a
  non-sibling target, for a fresh insert (it only places the newcomer), and for another
  `PRESERVE` call on the same parent (suppressed as well).
- `PRESERVE` **with any other parent** (window root layer, plain actor): identical to `UPDATE`,
  i.e. the plain inherited call.
- `RaiseAbove/LowerBelow` with an **empty** target: return silently, under both policies and
  whatever the parent.

`DEPTH_INDEX` changes only C, permanently and consistently: A and B stay as they are, so the
latent snap described above cannot happen.

---

## 2. What is deprecated, and how to find the call sites

Deprecated in `dali-ui-foundation/public-api/views/view.h`, `view-types.h` and
`public-api/views/view-impl.h` (each with a `@deprecated` note that names its replacement), and
removed in a later dali-ui:

```cpp
enum class Dali::Ui::LayoutOrderPolicy { UPDATE, PRESERVE };            // deprecated
void View::Raise(LayoutOrderPolicy);        void ViewImpl::Raise(LayoutOrderPolicy);        // deprecated
void View::Lower(LayoutOrderPolicy);        void ViewImpl::Lower(LayoutOrderPolicy);        // deprecated
void View::RaiseToTop(LayoutOrderPolicy);   void ViewImpl::RaiseToTop(LayoutOrderPolicy);   // deprecated
void View::LowerToBottom(LayoutOrderPolicy);void ViewImpl::LowerToBottom(LayoutOrderPolicy);// deprecated
void View::RaiseAbove(View, LayoutOrderPolicy);   void ViewImpl::RaiseAbove(View, LayoutOrderPolicy);   // deprecated
void View::LowerBelow(View, LayoutOrderPolicy);   void ViewImpl::LowerBelow(View, LayoutOrderPolicy);   // deprecated
```

Still available on every `Dali::Ui::View` handle (inherited from `Dali::Actor`, unchanged):
`Raise()`, `Lower()`, `RaiseToTop()`, `LowerToBottom()`, `RaiseAbove(Actor)`, `LowerBelow(Actor)`,
`InsertAbove(child, target)`, `InsertBelow(child, target)`, `GetChildAt()`, `GetChildCount()`,
`SetDepthIndex(int32_t)`, `GetDepthIndex()`, `SetProperty(Actor::Property::DEPTH_INDEX, …)`.
Inside a `ViewImpl` subclass reach them through `Self()`.

Kept dali-ui APIs (not duplicates): `View::Remove(View, RemovePolicy)`, `View::RemoveAll(RemovePolicy)`,
the inherited `Remove(Actor)`/`RemoveAll()`, `GetChildViewCount()`, `GetChildViewAt()`, `IndexOfChildView()`.

During the deprecation every call compiles without a diagnostic: the deprecation is documented
only, so the compiler does not find the call sites for you; the searches below do. After the
removal the compiler reports (GCC 15, verified against dali-ui headers without the overloads; the
spelling follows the call site, and in a UTF-8 locale GCC prints typographic quotes ‘…’ instead
of '…'):

```
error: 'LayoutOrderPolicy' has not been declared             // unqualified argument `LayoutOrderPolicy::X` or parameter type (e.g. under `using namespace Dali::Ui;`)
error: 'Dali::Ui::LayoutOrderPolicy' has not been declared   // qualified argument (`Dali::Ui::` or `Ui::`)
error: 'RaiseToTop' was not declared in this scope           // additionally, inside a ViewImpl subclass
error: 'LayoutOrderPolicy' was not declared in this scope    // typed local variable
error: 'LayoutOrderPolicy' does not name a type              // data member / typed constant
error: 'LayoutOrderPolicy' is not a member of 'Dali::Ui'     // qualified typed local variable
error: 'LayoutOrderPolicy' in namespace 'Dali::Ui' does not name a type        // `using Policy = Dali::Ui::LayoutOrderPolicy;`
error: 'p' was not declared in this scope                                      // the CALL line, policy passed through a typed local / member
error: no matching function for call to 'Dali::Ui::View::RaiseToTop(int&)'     // the CALL line, policy passed through a parameter / alias
```

**Trap (after the removal):** a call that passes the policy through an `auto`-deduced (or template-deduced)
constant or variable (`v.RaiseToTop(kKeep);` with `constexpr auto kKeep =
LayoutOrderPolicy::PRESERVE;`) produces **no diagnostic at the call line**; only the constant's
declaration fails. Explicitly typed locals, parameters, members and aliases do report the call
line, with the messages listed last above. Either way the `UPDATE`/`PRESERVE` meaning must be
traced by hand, and the search below, not the compiler, is the authority. **Second trap:** a
wrapper that took the policy as a defaulted parameter (`void BringUp(View v, LayoutOrderPolicy p
= LayoutOrderPolicy::PRESERVE)`) is found, but its callers (`BringUp(card);`) are not: once the
wrapper is migrated they compile silently. Grep the wrapper's name and classify every caller by
§3.2 as well.

Find every call site (run from the application repository root):

```sh
# 1. Every spelling of the type: direct uses, constants, parameters, typedefs, wrappers with a
#    defaulted policy parameter. This is the authority. Search EVERY file (no --include filter:
#    .hxx/.tpp/.ixx/.cppm/.C/.mm and generated or scripting files would otherwise be skipped);
#    exclude only build output.
grep -rn --exclude-dir=build --exclude-dir=.git "LayoutOrderPolicy" .
# 2. Direct calls (single-line; arguments may contain nested parentheses; `.` and `->` forms).
grep -rnE --exclude-dir=build --exclude-dir=.git "(\.|->)(Raise|Lower|RaiseToTop|LowerToBottom|RaiseAbove|LowerBelow)\(.*(UPDATE|PRESERVE)" .
# 3. For every constant / parameter / member / wrapper function found by grep 1, grep its NAME
#    to find the indirect calls (a wrapper's callers pass no policy and are invisible to 1-2).
```

Calls split across lines are found by grep 1 (the type name is on the declaration of whatever
is passed) and, after the removal, by the compiler. Generated code and scripting/binding layers that name the enum
as a string are covered by grep 1 only when they live in the searched tree; search them
separately otherwise.

A handle call such as `view.RaiseToTop()` (no argument) compiles before and after the removal
with identical behaviour; it needs no edit. Inside a `ViewImpl` subclass the **only** unqualified
form that compiles is the one-argument `RaiseToTop(policy)` (it resolves to the deprecated
`ViewImpl` overload); the no-argument unqualified form does not compile, neither now nor after the
removal (`CustomActorImpl` has no such member). Use `Self().RaiseToTop()`, see §3.3.

---

## 3. Mechanical replacement per call site

### 3.1 `LayoutOrderPolicy::UPDATE` → drop the argument

```cpp
// Before                                      // After
v.Raise(LayoutOrderPolicy::UPDATE);            v.Raise();
v.Lower(LayoutOrderPolicy::UPDATE);            v.Lower();
v.RaiseToTop(LayoutOrderPolicy::UPDATE);       v.RaiseToTop();
v.LowerToBottom(LayoutOrderPolicy::UPDATE);    v.LowerToBottom();
v.RaiseAbove(t, LayoutOrderPolicy::UPDATE);    if(t) { v.RaiseAbove(t); }
v.LowerBelow(t, LayoutOrderPolicy::UPDATE);    if(t) { v.LowerBelow(t); }
```

Behaviour is identical except for the two target-taking calls:

- The deprecated overloads return silently on an **empty** `target`. The inherited
  `Dali::Actor::RaiseAbove/LowerBelow` dereference the handle and hit
  `DALI_ASSERT_ALWAYS(actor && "Actor handle is empty")`, which logs and **throws
  `Dali::DaliException`** (uncaught, that terminates the application). Keep the `if(t)` guard
  whenever the target can be empty.
- A **non-sibling** target (different parent) is a silent no-op in dali-core, as before.
- Calling any of the six on a view that has **no parent** logs
  `Actor must have a parent, Sibling order not changed.` and does nothing, as before.
- `RaiseAbove(t)` only moves `v` when it is currently **below** `t` (then directly above `t`);
  if `v` is already above `t` nothing moves. `LowerBelow` is the mirror image. This was the old
  behaviour too (the overload forwarded to the same code).

### 3.2 `LayoutOrderPolicy::PRESERVE` → depends on the parent and on what preceded the call

Classify each call site:

0. **The view has no parent at call time** (the call precedes the `Add`, or sits in a
   RecyclerView `BindViewHolderSignal` handler: RecyclerView binds a created or recycled holder
   **before** `mScroller.Add(holder.view)` and only content-changed rebinds run while the item is
   parented): the old call logged `Actor must have a parent, Sibling order not changed.` and did
   nothing, under both policies. `SetDepthIndex` would instead store a value that takes effect at
   the later `Add`. Delete the call, or keep the old no-op semantics with `if(v.GetParent())`.
1. **Parent is not a `Dali::Ui::View`** (e.g. `window.Add(preview); preview.RaiseToTop(PRESERVE);`):
   the old call was the plain inherited call. Replace with `preview.RaiseToTop()` etc. (§3.1).
   Do **not** introduce `DEPTH_INDEX` here; it would leave a sticky value behind for no benefit.
2. **`RaiseToTop(PRESERVE)` directly after a fresh `Add` of that child** (the child was not already
   a child of that parent): `Actor::Add` appends, the child was already last, the old call moved
   nothing. Delete it. (Likewise `LowerToBottom(PRESERVE)` on a child that is provably at the
   bottom.)
   This rule does **not** cover `InsertAbove`/`InsertBelow` (the child lands next to its anchor)
   or `Add` of a child that was already in that parent (dali-core ignores the re-add, the child
   stays where it was). There the old call was a real bring-to-front unless the child had ended
   up last anyway (`InsertAbove` with the last child as anchor, an insert whose anchor had
   vanished so dali-core appended, a re-`Add` of the last child): with a `Ui::View` parent it
   moved A and C and kept the inserted layout slot B, the "insert into slot k, draw on top"
   pattern. Migrate it as rule 3 (keep the insert, add `SetDepthIndex(n)`), as rule 4 for a §6
   container parent, or as rule 1 for a non-View parent; where the child was provably last,
   delete the call as in this rule. dali-ui's own reorder sample does
   `mStack.InsertBelow(droppedChild, dropAnchor); droppedChild.SetDepthIndex(1);`.
3. **Parent is an application-owned `Dali::Ui::View`** and the intent is "draw on top / below
   without touching the layout": use `DEPTH_INDEX`.
   Alternative when the parent's layout does **not** depend on child order (`AbsoluteLayout`:
   every child's bounds come from its own layout params, so a reorder re-measures but moves
   nothing) **and** nothing reads the logical order (`GetChildViewAt`/`IndexOfChildView`): the
   plain inherited call (`v.RaiseToTop()` etc.) reproduces the old result for A, C, accessibility
   order and at-point lookup, focus tie-breaks and RenderEffect task order, needs no reset
   bookkeeping and no persistence/recycling/EXIT caveats, at the cost of one extra measure pass
   and a changed logical order (which the old `PRESERVE` only kept until its latent snap, §1).
   With a `LayoutTransition` attached nothing animates, because CHANGE fires only when bounds
   change. Order-dependent layouts (`StackLayout`, `FlexLayout`, `GridLayout`) need
   `DEPTH_INDEX`, because there a reorder moves the layout slot.
4. **Parent is one of the dali-ui containers of §6** (direct child of `RecyclerView`,
   `ScrollView`, `Navigator`, `DialogContainer`): no `DEPTH_INDEX`; see §6.
5. **Parent type is only known at run time** (a shared helper that sometimes raises a
   window-parented drag view and sometimes a child of an application-owned container): reproduce
   the deprecated overloads' decision at run time, **plus** the rule-4 exclusion that they do not
   need (they raise a container child in A, which the container's next restack undoes; a
   `DEPTH_INDEX` lift would survive those restacks):

```cpp
// Before: draw on top, keep layout slot (parent is a dali-ui layout container)
card.RaiseToTop(LayoutOrderPolicy::PRESERVE);

// After: draw on top, keep layout slot AND keep sibling order
constexpr int32_t LIFTED_DEPTH_INDEX  = 1; // > every sibling that stays at the default 0
constexpr int32_t DEFAULT_DEPTH_INDEX = 0;
card.SetDepthIndex(LIFTED_DEPTH_INDEX);
// ... when the card must go back into normal stacking (blur / drag end / animation end):
card.SetDepthIndex(DEFAULT_DEPTH_INDEX);

// Rule 5: the deprecated PRESERVE overload decides by the parent type on every call. Same decision
// now, with the dali-ui containers of §6 (rule 4) sent to the plain call. Navigator and
// DialogContainer live in dali-ui-components; drop those two tests (and the include) if the
// application does not link that library.
#include <dali-ui-components/dali-ui-components.h>

bool IsSiblingOrderContainer(Dali::Actor parent)
{
  return Dali::Ui::RecyclerView::DownCast(parent) || Dali::Ui::ScrollView::DownCast(parent) ||
         Dali::Ui::Navigator::DownCast(parent) || Dali::Ui::DialogContainer::DownCast(parent);
}

void BringToFront(Dali::Actor v)
{
  if(!v || !v.GetParent()) { return; }                 // old call: warning, no-op
  Dali::Actor parent = v.GetParent();
  if(Dali::Ui::View::DownCast(parent) && !IsSiblingOrderContainer(parent)) // old: A moved, B kept -> now draw order only
  {
    v.SetDepthIndex(LIFTED_DEPTH_INDEX);               // remember to reset (§5.1)
  }
  else                                                 // non-View parent (old and new: plain call) or §6 container (rule 4)
  {
    v.RaiseToTop();
  }
}
```

Per-call mapping. "Old behaviour" is what dali-core did with the actor order (hence with the
draw order). The "simple" column is only exact where the note says so; use the helpers of §5.2
when exactness matters:

| Old call | Old behaviour (draw order) | Simple replacement (all siblings at default 0) | Exact replacement |
|---|---|---|---|
| `RaiseToTop(PRESERVE)` | on top of all siblings; no-op if already on top; no-op without a parent | `v.SetDepthIndex(1)` (exact while `v` is parented, see rule 0) | `RaiseToTopInDrawOrder(v)` |
| `LowerToBottom(PRESERVE)` | below all siblings; no-op if already at the bottom; no-op without a parent | `v.SetDepthIndex(-1)` (exact while `v` is parented) | mirror of the above |
| `RaiseAbove(t, PRESERVE)` | if `v` is below `t`: directly above `t`; otherwise unchanged; empty `t` ignored | `if(t) { v.SetDepthIndex(1); }`. Exact only when `v` was below `t` **and** no sibling was above `t`. Otherwise it lifts `v` above every `0` sibling (including those above `t`), and it lifts even where the old call did nothing (`v` already above `t`). No formula on depth indices can reproduce "otherwise unchanged": with equal indices the order is the sibling order, which the indices do not encode. | `RaiseAboveInDrawOrder(v, t)` |
| `LowerBelow(t, PRESERVE)` | if `v` is above `t`: directly below `t`; otherwise unchanged; empty `t` ignored | `if(t) { v.SetDepthIndex(-1); }`, same caveats mirrored | mirror of the above |
| `Raise(PRESERVE)` / `Lower(PRESERVE)` | one step up / down; no-op at the end | — | `RaiseOneStepInDrawOrder(v)` / mirror |

The "simple" column assumes that no sibling holds a non-zero index. After the first lift that is
no longer true: a second `SetDepthIndex(1)` on another sibling ties with the first, and sibling
order decides. Repeated raises among the same overlapping siblings (the old calls built a
most-recently-raised-on-top stack in A) need the exact helpers, or the one-lift-at-a-time
discipline of §5.1 where its difference is acceptable (§4 "Persistence").

Property form (property tables, scripting, bindings): name `"depthIndex"`, type
`Property::INTEGER`, writable, **not animatable**, not a constraint input:

```cpp
view.SetProperty(Dali::Actor::Property::DEPTH_INDEX, 1);
int32_t d = view.GetProperty<int32_t>(Dali::Actor::Property::DEPTH_INDEX);
```

### 3.3 Inside a `ViewImpl` subclass

The six `Dali::Ui::ViewImpl` policy methods are deprecated as well, and `Dali::CustomActorImpl`
has no sibling-order or depth-index members, so after the removal an unqualified call no longer
resolves. Use the handle:

```cpp
// Before (member of a ViewImpl subclass)
RaiseToTop(Ui::LayoutOrderPolicy::UPDATE);
RaiseToTop(Ui::LayoutOrderPolicy::PRESERVE);

// After
Self().RaiseToTop();          // sibling reorder (parent's logical order follows)
Self().SetDepthIndex(1);      // draw order only; Self().SetDepthIndex(0) to undo
```

The classification of §3.2 applies unchanged (`Self().GetParent()`).

---

## 4. `DEPTH_INDEX` semantics and the differences from `PRESERVE`

Verified in dali-core (see Evidence):

1. **Sort key among siblings only.** The parent stable-sorts its children by depth index when it
   builds the render order. Higher value = drawn later = on top. Default `0`; negative values
   are allowed and draw beneath the default siblings. The value is never compared with cousins
   or ancestors: it cannot lift a view above its parent's siblings. Renderer- and visual-level
   depth indices (`Renderer::Property::DEPTH_INDEX`, `Visual::Base::SetDepthIndex`) order the
   renderers **within** one actor; the render sort key is the actor's depth-tree index plus the
   renderer index, so they combine with the actor order in the same way under both mechanisms.
2. **Ties fall back to sibling order (A).** Two siblings with the same depth index keep their
   actor order. Hence "all at 0" is exactly today's behaviour, and a child added later (default
   `0`) is drawn above the other `0` siblings but **below** any sibling holding a positive index.
3. **Whole subtree moves.** The depth tree is built depth-first, so a lifted view's descendants
   are drawn above the lower siblings' descendants, exactly as a sibling reorder would do.
4. **Touch hit-testing follows the same order.** Touch, hover, wheel and gesture hit-testing
   (`HitTestAlgorithm`) walk the depth order top-most first, so the lifted view receives touches
   first, as it did after a `PRESERVE` raise. **Exception: the accessibility bridge's at-point
   lookup** (`GetNavigableAtPoint`, used by screen-reader touch exploration) walks the accessible
   children in reverse **sibling** order under `LAYER_UI` and ignores `DEPTH_INDEX`; where a
   lifted view overlaps a sibling that comes later in A, the screen reader resolves the covered
   sibling. After the old `PRESERVE` raise A had changed, so it resolved the raised view.
5. **Off-screen render tasks keep the sibling order.** Every RenderEffect (`BackgroundBlurEffect`,
   `GaussianBlurEffect`, `MaskEffect`, `OffscreenRendering`, and any other `RenderEffect`
   subclass) creates off-screen render tasks whose execution order is computed by walking
   `Actor::GetChildAt`. A sibling reorder changed that order (recursive reorder request);
   `SetDepthIndex` requests a reorder as well (effective only when the lifted view itself
   carries an effect) but A is unchanged, so two sibling views that both carry such an effect
   keep executing their tasks in A order while their draw order changed. What the tasks capture does follow the draw order
   (`RenderUntil`). The visible consequence (if any, e.g. a stale blur of an overlapped blurred
   sibling) was **not verified**; re-test such screens.
6. **Nothing else changes.** `SetDepthIndex` does not touch the sibling list, does not emit
   `ChildOrderChangedSignal`, does not change `GetChildAt()` / `GetChildViewAt()`. dali-ui has
   no handling for the property at all, so no measure/arrange pass and no `LayoutTransition`
   dispatch follows. (dali-ui's test `UtcDaliViewDepthIndexDoesNotChangeChildOrderInternalP`
   asserts the value, zero `ChildOrderChangedSignal` emissions and unchanged actor/logical
   indices.)
7. **The value belongs to the actor, not to the parent slot.** `Remove`, `Add`, reparenting and
   `InsertAbove/InsertBelow` leave it untouched (the parent only maintains a counter of non-zero
   children). A lifted view that is moved to another parent, or a recycled RecyclerView item
   view, carries its lift with it. Reset to `0` before letting go of a view and before re-adding
   a view whose history you do not know (§5.1, §6).
8. **Only under `Layer::Behavior::LAYER_UI` with the parent's
   `DevelActor::Property::CHILDREN_DEPTH_INDEX_POLICY == INCREASE`.** Both are the dali-core
   defaults and dali-ui never changes either. Under the `EQUAL` policy the render order no longer
   follows the index while hit-testing still does, so the two disagree: never combine them.
9. **Not animatable** and not usable as a constraint input (`DALI_PROPERTY("depthIndex", INTEGER,
   true, false, false, …)`). Set it from event-side code at the moment the stacking must change.

`DrawMode::OVERLAY_2D` actors follow the same rule: the overlay list is sorted by the same
depth-tree key, and overlay hit roots are collected in the same depth order, so a lifted overlay
behaves like a reordered one. Not verified (test before relying on it): `DEPTH_INDEX` set on a
`Layer` child (layers are ordered by the scene's layer list, not by their sibling position).

Differences an application will notice when it replaces a `PRESERVE` call (View parent) with
`DEPTH_INDEX`:

| Topic | Old `PRESERVE` overload | `DEPTH_INDEX` |
|---|---|---|
| Persistence | A permanent move in A: raising X and then Y gave `[…, X, Y]`, a most-recently-raised-on-top stack in which X **stayed above every sibling that was never raised**; nothing to undo. | A **sticky property**. If X and Y both hold `1`, they tie and sibling order decides. Exact reproduction of the old stack: hand out strictly increasing values (`RaiseToTopInDrawOrder`, §5.2). Simplification: reset the previously lifted view to `0` when lifting the next one (§5.1 `LiftTracker`), which puts X back into its sibling slot, i.e. **below** later siblings it used to cover; acceptable only when the non-lifted siblings do not overlap. Survives `Remove`/`Add`/reparent/recycling (§4.7). |
| Reset timing | The raised view stayed in front after blur/release, and above the never-raised siblings even after the next raise (see Persistence). | The reset is an explicit call. If you reset at blur while a shrink animation is still running (as dali-ui's text-render-scale sample does), the still-enlarged view is drawn under later siblings for the rest of the animation. "Keep the lift until the next lift replaces it" (`LiftTracker`) does **not** help when the blur and the next lift are one event: dali-ui delivers focus-lost and focus-gained synchronously inside one focus change, so the shrinking view is reset at once. Resetting in the shrink animation's `FinishedSignal` keeps it lifted, but with one shared value (`1`) the shrinking view and the newly focused view tie and sibling order decides, which draws the shrinking view **over** the new one whenever it comes later in A. Exact: give the new lift a value above the still-animating one (`RaiseToTopInDrawOrder`, §5.2) and, in the old view's `FinishedSignal`, reset it only if it has not been lifted again since its opposite event (§5.5 `StackFocusLift`). |
| Actor sibling order (A) | Changed (`Actor::GetChildAt` reflected the new z-order; accessibility child order and at-point lookup, focus walk and RenderEffect task order changed with it). | Unchanged. Code that inferred the "topmost" child from `Actor::GetChildAt(count-1)` must read `GetDepthIndex()` or compute `DrawOrder()` (§5.2) instead. |
| Layout order (B) | Kept, until the next unsuppressed `ChildOrderChangedSignal` on the parent (even one that moved no View) rebuilt it from A (latent snap, §1). | Kept, permanently. |
| Focus navigation | Directional moves walk A in reverse and pick the best candidate by geometry; FORWARD/BACKWARD stable-sort candidates by screen y then x. A changed, so tie-breaks between equally scored candidates changed. | Same rules over the unchanged A. In practice only equally scored / equally placed siblings can resolve differently. |
| Accessibility | Child order and the at-point lookup followed the new A (the bridge listens to `ChildOrderChangedSignal`). | Both keep the old A: a lifted view overlapping a later sibling is read as covered (§4.4). |
| `LayoutTransition` | The reorder emitted `ChildOrderChangedSignal`, which `PRESERVE` made dali-ui ignore. | No signal, no `REORDERED` tagging, no layout pass. |
| Later plain reorders in the same parent | Every raise was a reorder of A, so the **most recent** raise won: `b.RaiseToTop(PRESERVE)` (hover) then `c.RaiseToTop()` (tap) put `c` on top. | A held lift beats every later plain reorder: after `b.SetDepthIndex(1)`, `c.RaiseToTop()` moves `c` to the end of A but it is still drawn (and hit-tested) **under** `b`. Plain reorders include the former `UPDATE` calls kept as inherited calls, `RaiseAbove`/`InsertAbove`/`InsertBelow` of an existing child and any other `RaiseToTop()`. Drop or reset the held lift (`LiftTracker::Drop`) before a plain bring-to-front in the same parent, or do that bring-to-front with the same depth scheme (§5.2). |
| Children added later | Appended at the top of A, so above the raised view. | Appended at their own (persisted, usually `0`) index, so **below** a view still holding a positive index. Reset lifted views to `0` as soon as the lift is over. |

---

## 5. Recipes

### 5.1 Temporary lift (focus, press, drag, settle animation): preferred when one lift at a time is enough

This is the pattern dali-ui's own samples use (`samples/text/text-render-scale-example.cpp`,
`samples/layout-transition/layout-transition-reorder-example.cpp`). It keeps a single lifted
view per parent. It is **not** an exact reproduction of repeated `PRESERVE` raises among
overlapping siblings (those built a most-recently-raised-on-top stack, §4 "Persistence"); use
`RaiseToTopInDrawOrder` (§5.2) when that stack must survive. A complete focus/selection example for
the items of a `StackLayout` row is in §5.5.

```cpp
#include <dali/public-api/object/weak-handle.h>

namespace
{
constexpr int32_t LIFTED_DEPTH_INDEX  = 1; ///< Drawn above every sibling at the default 0
constexpr int32_t DEFAULT_DEPTH_INDEX = 0; ///< Back to sibling-order rendering
}

void OnFocusGained(View card) { card.SetDepthIndex(LIFTED_DEPTH_INDEX); }
void OnFocusLost(View card)   { card.SetDepthIndex(DEFAULT_DEPTH_INDEX); }

// One lifted view per parent: lifting the next one resets the previous one, so a missed
// "opposite event" (cancelled transition, view removed while lifted) cannot leave two lifts.
class LiftTracker
{
public:
  void Lift(View next)
  {
    View previous = mLifted.GetHandle();
    if(previous && previous != next) { previous.SetDepthIndex(DEFAULT_DEPTH_INDEX); }
    mLifted = Dali::WeakHandle<View>(next);
    if(next) { next.SetDepthIndex(LIFTED_DEPTH_INDEX); }
  }
  void Drop() { Lift(View()); }
  // A reset deferred to an animation's FinishedSignal cannot be expressed with this tracker: Lift
  // resets the previous view at once. For that case see §5.5 StackFocusLift.
private:
  Dali::WeakHandle<View> mLifted; // weak: must not keep a removed view alive
};
```

Rules:
- Exactly one sibling holds `LIFTED_DEPTH_INDEX` at a time (`LiftTracker`), or you accept that
  ties fall back to sibling order.
- Always reset on the opposite event (blur, release, drop): to `0`, or to the tier of a state
  the view still holds (§5.5 `ItemStacking`). A `LayoutTransition`
  `OnFinished` callback is **not** a dependable reset point: it fires only on normal completion
  and is silently dropped on every cancellation path (same-slot or cross-slot supersession,
  `Remove` starting EXIT, reparent, view destruction, scene disconnection of the view or of an
  ancestor), and nothing is dispatched at all when the reorder produces no CHANGE for that view.
  dali-ui's reorder sample documents that its lift then stays until the card's next completed
  transition. If you reset in `OnFinished`, add a fallback (`LiftTracker`, or a reset on the
  next drag start / on removal).
- If a shrink or slide-back animation follows the opposite event and the view must stay on top
  while it animates, do not reset at the event. Reset in that animation's `FinishedSignal`, and
  guard the reset: `FinishedSignal` also fires after `Animation::Stop()` (only `Clear()`
  suppresses it) and possibly after the view was lifted again, so forget the view's lift at the
  opposite event and, in `FinishedSignal`, reset it only if it has not been lifted again since
  (§5.5 `StackFocusLift`, which also `Clear()`s a superseded animation), never unconditionally.
  If the next lift can start while the previous view still animates (focus moving between
  overlapping cards), one shared value is not enough: the new lift must get a value above the
  still-animating one (`RaiseToTopInDrawOrder`, §5.2). See §4 "Reset timing".
- Reset before the view leaves the parent by an immediate removal or a reparent, before
  re-adding a view whose history you do not know, and, for RecyclerView item views, on every
  bind so a recycled view never shows a stale lift. **Exception, animated removal:** with
  `Remove(child, RemovePolicy::ANIMATE_EXIT)` and an EXIT effect the child stays in the actor
  tree as a ghost until its EXIT animation ends; a reset before the call drops the ghost back
  into its sibling slot, under the siblings that slide into the freed space, whereas the old
  raised view faded out on top. Keep the lift during the EXIT and reset in the EXIT
  `OnFinished` (it fires after the unparent, so this is the "before re-add" reset) or before the
  next `Add` of that view:

```cpp
// ItemAdapter: BindViewHolderSignal / RecycleViewHolderSignal carry ItemViewHolder& (holder.view).
// With a GroupAdapter, header views are bound through GroupAdapter::BindHeaderViewHolderSignal /
// RecycleHeaderViewHolderSignal instead: connect the same reset there.
void MyAdapterOwner::OnBindViewHolder(Dali::Ui::ItemViewHolder& holder)
{
  holder.view.SetDepthIndex(DEFAULT_DEPTH_INDEX); // then apply the bound state (lift again only if needed)
}
```

- Do not combine with a sibling reorder for the same purpose; one mechanism per intent. Within
  one parent a held lift also outranks every later **plain** reorder (§4 "Later plain reorders"):
  `Drop()` the lift before a plain `RaiseToTop()`/`RaiseAbove()` of another sibling, or lift that
  sibling instead.
- Do not mix this `1`/`0` scheme with the explicit `0..N-1` encoding of §5.2 on the same parent.
  After `ApplyDrawOrder`, `SetDepthIndex(1)` can **lower** a view (a view encoded at `2` drops to
  `1` and ties with another `1`), and the `SetDepthIndex(0)` reset makes it bottom-most and
  destroys the explicit stacking. On a parent where `ApplyDrawOrder` has run, lift with
  `RaiseToTopInDrawOrder` and restore by re-applying a saved `DrawOrder()` snapshot (§5.4).

### 5.2 Persistent, arbitrary stacking among many siblings (exact replacement for `Raise/Lower/RaiseAbove/LowerBelow(PRESERVE)`)

Scope: **only for a parent the application owns completely** (every child is yours, no dali-ui
container internals, no EXIT transition in flight). The helpers stamp an index on **every actor
child** of the parent, including non-View helper actors and in-flight EXIT ghosts: a ghost leaves
the parent carrying its stamped value and brings it back if the same view is re-added later
(reset before re-adding, §4.7), and inside a dali-ui container (e.g. the RecyclerView scroller,
which also holds the group background views) the stamping breaks the container's own stacking
(§6). Prefer §5.1 wherever a temporary lift is enough.

Keep the desired draw order explicit and re-encode it into depth indices. The helpers work on
`Dali::Actor` so that non-View siblings are ordered too, and they reproduce dali-core's
conditional moves (no-op when already on top / already above the target).

```cpp
#include <algorithm>
#include <iterator>
#include <vector>
using Dali::Actor;

// Current draw order of parent's children: stable sort by depth index, ties keep sibling order.
// front() = bottom-most, back() = top-most. Empty parent → empty vector.
std::vector<Actor> DrawOrder(Actor parent)
{
  std::vector<Actor> order;
  if(!parent) { return order; }
  const uint32_t count = parent.GetChildCount();
  order.reserve(count);
  for(uint32_t i = 0; i < count; ++i) { order.push_back(parent.GetChildAt(i)); }
  std::stable_sort(order.begin(), order.end(), [](const Actor& a, const Actor& b)
  { return a.GetDepthIndex() < b.GetDepthIndex(); });
  return order;
}

// Encode an explicit draw order: every child gets a distinct index, 0 = bottom-most.
void ApplyDrawOrder(const std::vector<Actor>& order)
{
  int32_t depth = 0;
  for(Actor a : order) { a.SetDepthIndex(depth++); } // handle copy: SetDepthIndex is non-const
}

// Old v.RaiseToTop(PRESERVE): above every sibling; no-op when already top-most.
void RaiseToTopInDrawOrder(Actor v)
{
  if(!v || !v.GetParent()) { return; }
  const auto order = DrawOrder(v.GetParent());
  if(order.empty() || order.back() == v) { return; }
  v.SetDepthIndex(order.back().GetDepthIndex() + 1);
}

// Old v.Raise(PRESERVE): one step up in draw order; no-op when already top-most.
void RaiseOneStepInDrawOrder(Actor v)
{
  if(!v || !v.GetParent()) { return; }
  auto order = DrawOrder(v.GetParent());
  auto it    = std::find(order.begin(), order.end(), v);
  if(it == order.end() || std::next(it) == order.end()) { return; }
  std::iter_swap(it, std::next(it));
  ApplyDrawOrder(order);
}

// Old v.RaiseAbove(target, PRESERVE): if v is below target, move it directly above target;
// otherwise nothing (dali-core only moves a child that is below the target).
void RaiseAboveInDrawOrder(Actor v, Actor target)
{
  if(!v || !target || v == target || !v.GetParent() || v.GetParent() != target.GetParent()) { return; }
  auto order = DrawOrder(v.GetParent());
  auto vIt   = std::find(order.begin(), order.end(), v);
  auto tIt   = std::find(order.begin(), order.end(), target);
  if(vIt == order.end() || tIt == order.end() || vIt > tIt) { return; } // already above target
  const Actor moved = *vIt;
  order.erase(vIt);
  tIt = std::find(order.begin(), order.end(), target); // re-find: erase invalidated it
  order.insert(std::next(tIt), moved);
  ApplyDrawOrder(order);
}
// LowerToBottom / Lower are the mirror images of RaiseToTop / Raise. LowerBelow:

// Old v.LowerBelow(target, PRESERVE): if v is above target, move it directly below target;
// otherwise nothing (dali-core only moves a child that is above the target).
void LowerBelowInDrawOrder(Actor v, Actor target)
{
  if(!v || !target || v == target || !v.GetParent() || v.GetParent() != target.GetParent()) { return; }
  auto order = DrawOrder(v.GetParent());
  auto vIt   = std::find(order.begin(), order.end(), v);
  auto tIt   = std::find(order.begin(), order.end(), target);
  if(vIt == order.end() || tIt == order.end() || vIt < tIt) { return; } // already below target
  const Actor moved = *vIt;
  order.erase(vIt);
  tIt = std::find(order.begin(), order.end(), target); // re-find: erase invalidated it
  order.insert(tIt, moved);
  ApplyDrawOrder(order);
}
```

Caveats of the explicit scheme:
- After `ApplyDrawOrder` every child of that parent carries an index `0..N-1`. A child added
  **later** keeps its own (persisted, usually `0`) index: `Add` appends it to A, so at `0` it ties
  with `order[0]` and is drawn directly above `order[0]`, near the bottom. Re-running
  `ApplyDrawOrder(DrawOrder(parent))` only freezes that position (the newcomer gets `1`). To put
  the newcomer on top, lift it (`RaiseToTopInDrawOrder(newcomer)`) or append it to the order
  vector before `ApplyDrawOrder`.
- `RaiseToTopInDrawOrder` only touches `v`; the others rewrite every sibling's index (one
  property write per child). Repeated `RaiseToTopInDrawOrder` calls reproduce the old
  most-recently-raised-on-top stack exactly (each raise lands above the current top); the values
  only grow, which is harmless for an `int32_t`.
- All helpers are no-ops without a parent (the old calls only logged a warning there) and when
  `v` and `target` have different parents (the old calls were no-ops there too).
- Never apply §5.1's fixed `SetDepthIndex(1)` lift to a child of a parent that uses these helpers
  (a helper value above `1` still covers it). Never restore with `SetDepthIndex(0)` on a parent on
  which `ApplyDrawOrder` has run, directly or through `RaiseOneStepInDrawOrder`/
  `RaiseAboveInDrawOrder`/`LowerBelowInDrawOrder` (§5.1, last rule): there, lift with
  `RaiseToTopInDrawOrder` and restore with a saved snapshot. Where only `RaiseToTopInDrawOrder`
  (or its `LowerToBottom` mirror) is used, each call writes only the lifted view, so
  `SetDepthIndex(0)` puts a view back into its sibling slot under every sibling that is still
  lifted (§4 "Reset timing", §5.5).

### 5.3 "Move the view to another layout slot" (B must change)

This is a real reorder; use the inherited actor API. The logical order B is resynced
automatically and the layout is re-measured:

```cpp
// Move v so that its logical index becomes i (erase-then-insert semantics).
void MoveToLogicalIndex(View parent, View v, uint32_t i)
{
  const int32_t from = parent.IndexOfChildView(v);
  if(from < 0) { return; }                                   // not a logical child of parent
  if(i >= parent.GetChildViewCount()) { v.RaiseToTop(); return; }
  View anchor = parent.GetChildViewAt(i);                    // the view currently in slot i
  if(static_cast<uint32_t>(from) < i)      { parent.InsertAbove(v, anchor); } // moving later: directly above anchor
  else if(static_cast<uint32_t>(from) > i) { parent.InsertBelow(v, anchor); } // moving earlier: directly below anchor
}
// Relative moves with a View sibling as the anchor:
v.RaiseAbove(sibling);  v.LowerBelow(sibling);
```

`InsertAbove/InsertBelow(child, target)` on a child that already belongs to the same parent is a
pure sibling reorder (emits `ChildOrderChangedSignal`); on a child that is new or belongs to
another parent it adds it at that position in one step without an order signal. The draw order
follows automatically unless some sibling holds a non-zero `DEPTH_INDEX` (§4.2). Traced example
for `[a, b, c, d]`: moving `a` to 2 gives `[b, c, a, d]`, moving `d` to 1 gives `[a, d, b, c]`,
moving `b` to 1 is a no-op.

`v.Raise()` / `v.Lower()` swap `v` with the adjacent **actor** sibling in A. If that neighbour is a
non-View actor or an in-flight EXIT ghost, A changes but B does not (the View sequence is
unchanged, so dali-ui returns early). Use `RaiseAbove/LowerBelow(viewSibling)` or
`MoveToLogicalIndex` when the layout slot must change.

### 5.4 "Reorder the layout but keep the old visual stacking"

Take `DrawOrder(parent)` before the reorder, do the reorder (§5.3), then `ApplyDrawOrder` with
the snapshot (§5.2, same scope restrictions).

### 5.5 Worked example: the focused or the user-selected item of a horizontal/vertical `StackLayout`

A common application use of the `PRESERVE` overloads (dali-ui's own `text-render-scale` sample was
one). The items are the children of one `StackLayout` (`StackLayout::New(StackOrientation::HORIZONTAL)`
or `VERTICAL`); the layout places them in a row by logical index, so every item has a slot and an
obvious "next" neighbour. The item that has keyboard focus, or the item the user tapped (selected),
must be drawn above its neighbours (it scales up, grows a shadow or a border) while every item
keeps its slot. Three old spellings, on two kinds of event:

```cpp
// Before. The parent is a StackLayout (an application-owned Ui::View), so PRESERVE was active.
// nextItem = the item's logically next neighbour (right-hand in a left-to-right row, left-hand
// under RIGHT_TO_LEFT, the lower one in a column), looked up by the application
// (e.g. stack.GetChildViewAt(stack.IndexOfChildView(item) + 1)); empty for the last item.
void OnItemFocusGained(View item, View nextItem)
{
  item.RaiseToTop(LayoutOrderPolicy::PRESERVE);              // spelling 1: above every sibling
  item.RaiseAbove(nextItem, LayoutOrderPolicy::PRESERVE);    // spelling 2: directly above the next item
}
void OnItemFocusLost(View item, View nextItem)
{
  item.LowerBelow(nextItem, LayoutOrderPolicy::PRESERVE);    // back under the next item
}
void OnItemSelected(View item)                  { item.RaiseToTop(LayoutOrderPolicy::PRESERVE); }
void OnItemDeselected(View item, View nextItem) { item.LowerBelow(nextItem, LayoutOrderPolicy::PRESERVE); }
```

What these calls did (§1; Evidence, "policy implementation bodies"): each of them reordered the actor list
A and only suppressed the rebuild of the logical order B, until the next unsuppressed order signal
on the stack. `RaiseToTop` moved the item to the end of A (no-op when it was already last),
`RaiseAbove(next)` placed it directly after `next` (only when it was before `next`),
`LowerBelow(next)` placed it directly before `next` (only when it was after `next`), which is its
original slot only while `next` itself was still in its original place (see the restore holes
below). An empty `nextItem` (the last item) returned silently; that was harmless only while no
other item was raised: the last item is drawn above its only neighbour anyway, but with spelling 2
it stayed under a raised (selected) earlier neighbour. Draw order and touch order followed A; the
row did not move as long as nothing else emitted an order signal. A fresh `Add` or a `Remove` did
not, but any later plain sibling reorder in the stack did (a former `UPDATE` call kept as an
inherited call, an `InsertAbove`/`InsertBelow` of an existing item, a `SIBLING_ORDER` write, even a
`RaiseAbove`/`LowerBelow` that moved nothing), and the raised item then jumped to its position in
A: the end of the row after `RaiseToTop`, one slot later after `RaiseAbove(next)`. That is the
latent snap of §1, which the migration removes.

Classification (§3.2): the parent is an application-owned `Ui::View` layout container and not a §6
container (this also holds when the stack is the content of a `ScrollView` or a page of a
`Navigator`: the items' parent is still the stack), the item is parented when a focus or selection
event reaches it, and no fresh `Add` precedes the call → rule 3, `DEPTH_INDEX`. Delete the
`nextItem` lookup (unless you take the `RaiseAboveInDrawOrder` alternative at the end of this
section). Rows hosted by a `RecyclerView` are not this case: the items' parent is the
`RecyclerView`'s internal scroller (§0 row 4); keep the selection as a data position there and set
the tier in the bind handler (§5.1). One set of constants serves the whole example:

```cpp
constexpr int32_t DEFAULT_DEPTH_INDEX  = 0; // sibling order = layout order
constexpr int32_t SELECTED_DEPTH_INDEX = 1; // above every item at 0
constexpr int32_t FOCUSED_DEPTH_INDEX  = 2; // above the selected item as well

// After, focus-driven. dali-ui delivers focus-lost and then focus-gained synchronously inside one
// focus change, so at most one item is lifted at any time and any positive value does.
void OnItemFocusGained(View item) { item.SetDepthIndex(FOCUSED_DEPTH_INDEX); } // was RaiseToTop / RaiseAbove(next)
void OnItemFocusLost(View item)   { item.SetDepthIndex(DEFAULT_DEPTH_INDEX); } // was LowerBelow(next): no next needed
```

This is what dali-ui's `text-render-scale` sample does (`samples/text/text-render-scale-example.cpp:360,
407`). The reset runs before the 150 ms scale-back animation starts, so the shrinking item is drawn
under its logically next neighbour (and every later sibling) for the rest of that animation, as the
old synchronous `LowerBelow(next)` also did. If the item must stay on top until the animation ends,
use the variant below instead; do not simply move the reset into the animation's `FinishedSignal`
with the same constant. If your own code lifts the new item before it resets the previous one, both
hold the same value for a moment; that is harmless only when the reset follows in the same event.

```cpp
// Variant: the item must stay on top while its scale-back animation runs after focus loss. Two
// items are then lifted at once (the shrinking one and the newly focused one), so the new lift must
// get a value above the still-animating one: with one shared value they tie and sibling order draws
// the shrinking item OVER a newly focused earlier sibling (§4 "Reset timing"). RaiseToTopInDrawOrder
// (§5.2) hands out "largest sibling value + 1" and writes only the lifted item, so every other item
// stays at 0 and the reset to DEFAULT_DEPTH_INDEX is exact. Never call ApplyDrawOrder on this stack,
// directly or through RaiseOneStepInDrawOrder/RaiseAboveInDrawOrder/LowerBelowInDrawOrder (§5.2,
// last caveat).
#include <utility>
#include <vector>

constexpr float SHRINK_DURATION_SECONDS = 0.15f;

class StackFocusLift : public Dali::ConnectionTracker
{
public:
  void OnItemFocusGained(View item)
  {
    mFocused = Dali::WeakHandle<View>(item);
    RaiseToTopInDrawOrder(item);
    CancelShrink(item); // Clear(), not Stop(): a stopped animation still emits FinishedSignal
    // ... start the scale-up animation
  }
  void OnItemFocusLost(View item)
  {
    // Focus may leave the stack (a view outside it, FocusManager::ClearFocus(), window focus loss,
    // removal of the item): no OnItemFocusGained follows, so forget the item here.
    if(mFocused.GetHandle() == item) { mFocused.Reset(); }
    CancelShrink(item);
    Dali::Animation shrink = Dali::Animation::New(SHRINK_DURATION_SECONDS);
    shrink.AnimateTo(Dali::Property(item, Dali::Actor::Property::SCALE), Dali::Vector3::ONE);
    shrink.FinishedSignal().Connect(this, [this, item](Dali::Animation) { OnShrinkFinished(item); });
    mShrinks.emplace_back(item, shrink);
    shrink.Play();
  }
private:
  Dali::Animation TakeShrink(View item)
  {
    for(auto it = mShrinks.begin(); it != mShrinks.end(); ++it)
    {
      if(it->first == item) { Dali::Animation a = it->second; mShrinks.erase(it); return a; }
    }
    return Dali::Animation();
  }
  void CancelShrink(View item) { if(Dali::Animation a = TakeShrink(item)) { a.Clear(); } } // Clear() never emits FinishedSignal
  void OnShrinkFinished(View item)
  {
    TakeShrink(item); // only an item's latest shrink can still emit: earlier ones were cleared
    // Defensive: a re-focused item had its shrink cleared, but never reset the currently focused item.
    if(mFocused.GetHandle() != item) { item.SetDepthIndex(DEFAULT_DEPTH_INDEX); }
  }
  Dali::WeakHandle<View> mFocused;                        // weak: a removed item must not be kept alive by the tracker
  std::vector<std::pair<View, Dali::Animation>> mShrinks; // strong only while a shrink runs
};
```

Selection-driven: the user taps an item and it stays lifted until another item is selected or the
selection is cleared, possibly for minutes, and usually while keyboard focus keeps moving over the
row. Two items can then be lifted at once (the selected one and the focused one), so give the two
states distinct tiers instead of one shared value, and recompute an item's index from **both** of
its flags whenever either changes: a selected item that loses focus goes back to
`SELECTED_DEPTH_INDEX`, not to `0`, which the focus-only `OnItemFocusLost` above would get wrong.
The same class covers selection without focus (never call the focus members) and replaces the old
`OnItemSelected`/`OnItemDeselected` pair when at most one item is selected at a time
(`AsGroupSelectable()` items in the stack's `SelectionGroup`, or an application model with one
current item). Map the old pair per item: `OnItemSelected(item)` → `Select(item)`,
`OnItemDeselected(item, nextItem)` → `Deselect(item)`, never `ClearSelection()`: the old call acted
on that item only, and your code may report the new selection before the old deselection. Use
`ClearSelection()` only where the selection is cleared without naming an item. With dali-ui's
`SelectionGroup`, connect `SelectionGroup::Find(stack).SelectedMemberChangedSignal()` and call
`Select(current)` from it; `current` is empty when the group was cleared.

```cpp
// After, selection-driven, optionally together with focus. One instance per stack.
class ItemStacking
{
public:
  void OnItemFocusGained(View item) { mFocused = Dali::WeakHandle<View>(item); Apply(item); }
  void OnItemFocusLost(View item)   { if(IsFocused(item)) { mFocused.Reset(); } Apply(item); }
  void Select(View item)                       // empty item = clear the selection
  {
    View previous = mSelected.GetHandle();
    if(previous == item) { return; }
    mSelected = Dali::WeakHandle<View>(item);
    if(previous) { Apply(previous); }          // was previous.LowerBelow(next, PRESERVE): back to 0, or to the focus tier
    if(item) { Apply(item); }                  // was item.RaiseToTop(PRESERVE)
  }
  void Deselect(View item) { if(IsSelected(item)) { Select(View()); } } // was item.LowerBelow(next, PRESERVE); no-op once another item is selected
  void ClearSelection()    { Select(View()); }
  // Before an immediate stack.Remove(item) or a reparent (§6), or in the EXIT OnFinished of
  // Remove(item, RemovePolicy::ANIMATE_EXIT) so that the item keeps its lift while it fades out (§5.1).
  void Forget(View item)
  {
    if(IsFocused(item))  { mFocused.Reset(); }
    if(IsSelected(item)) { mSelected.Reset(); }
    if(item) { item.SetDepthIndex(DEFAULT_DEPTH_INDEX); }
  }
  bool IsFocused(View item) const  { return item && mFocused.GetHandle() == item; }
  bool IsSelected(View item) const { return item && mSelected.GetHandle() == item; }
  // Recompute an item's tier from both flags. Also the end point of any other lift of the same item
  // (a drag lift, see below), instead of SetDepthIndex(0).
  void Apply(View item)
  {
    item.SetDepthIndex(IsFocused(item) ? FOCUSED_DEPTH_INDEX : IsSelected(item) ? SELECTED_DEPTH_INDEX : DEFAULT_DEPTH_INDEX);
  }
private:
  Dali::WeakHandle<View> mFocused;  // weak: a removed item must not be kept alive by the tracker
  Dali::WeakHandle<View> mSelected;
};
```

Multi-selection (items that toggle independently, e.g. `AsSelectable()` without a group:
toggle-by-click is on by default and only `SelectionGroup` members are exclusive) cannot use
`mSelected`. On every focus or `SelectionChangedSignal` event of an item, set that item's tier
from its own flags, `item.SetDepthIndex(focused ? FOCUSED_DEPTH_INDEX : (item.IsSelectable() &&
item.AsSelectable().IsSelected()) ? SELECTED_DEPTH_INDEX : DEFAULT_DEPTH_INDEX);` (the selected
flag is already committed when `SelectionChangedSignal` is emitted). Selected items then tie among
themselves and are drawn in sibling order (old: the most recently selected on top).

With the tiers the focused item is always drawn above the selected one. The old code had no such
rule. With `RaiseToTop` the most recent raise won, so "focus X, then select Y" drew Y above X and
"select Y, then focus X" drew X above Y; with spelling 2 the focus lift rose only above `next`, so
it covered a selected Y only when Y was X's `next` (a selected earlier neighbour stayed above the
focused item). If that order-of-events behaviour must be kept, lift on both events with
`RaiseToTopInDrawOrder` (§5.2), track both states in one object as `ItemStacking` does, and on
focus loss or deselection reset to `DEFAULT_DEPTH_INDEX` only an item that is now neither focused
nor selected; an item that keeps the other state keeps its value (the old `LowerBelow(next)`
dropped it, the result called wrong above). If that reset must wait for the shrink, add
`Dali::WeakHandle<View> mSelected;` to `StackFocusLift`, maintain it from the selection events, and
guard its reset with `if(mFocused.GetHandle() != item && mSelected.GetHandle() != item)`. Never
call `ApplyDrawOrder` on that stack.

Differences to accept, and what to test:
- Spelling 2 (`RaiseAbove(next)`) left the item under the items after `next`; the new lift draws it
  above every sibling. In a stack whose enlarged item overlaps only its two neighbours the picture is
  identical. If the design relied on later items covering the lifted one, keep the `nextItem` lookup
  and use `RaiseAboveInDrawOrder(item, next)` on the lift and `LowerBelowInDrawOrder(item, next)` on
  the restore (§5.2; skip both for the last item). That is a whole-stack choice for a focus-only
  stack: each call that moves something stamps every sibling `0..N-1` (`ApplyDrawOrder`), so never
  combine it on one stack with the tier constants, a `SetDepthIndex(DEFAULT_DEPTH_INDEX)` restore,
  `ItemStacking` or the deferred variant (§5.1, last rule). With selection on the same stack the
  deselect restore would have to be `LowerBelowInDrawOrder` too, which reproduces the old restore
  holes of the next bullet, or a saved snapshot (§5.4), which undoes the focus moves made since.
- The old restore had holes that the new one does not. `LowerBelow(next)` moved nothing when `next`
  had been raised after the item (dali-core moves only a child that is after the target), and when
  `next` had been raised before the item (`next` is the selected item, say) it put the item directly
  below `next`, still above every item in between. Both happened with the old synchronous restores
  as soon as two items were raised (select X, focus X's next item N, deselect X: X stays above the
  items after N, and N's own `LowerBelow` on focus loss then leaves X top-most although it is neither
  selected nor focused; select 2, focus 1, move focus on: 1 lands under 2 but above 3 and 4), and
  with a restore deferred to an animation. `SetDepthIndex(DEFAULT_DEPTH_INDEX)` always restores. Do
  not reproduce the holes.
- No special case for the first or the last item, and no `nextItem` lookup (except for the
  `RaiseAboveInDrawOrder` alternative above).
- An item added to the stack while another one is lifted is drawn under it (old: above it,
  §4 "Children added later"); reset the lift first if the newcomer must be on top.
- A drag reorder of the row (`InsertAbove`/`InsertBelow`, §5.3) while an item is selected moves the
  layout as requested and keeps the selected item on top, provided the drag code does not lift the
  dropped item itself. If it does (§0 row 6, §3.2 rule 2; dali-ui's reorder sample does
  `InsertBelow`, then `SetDepthIndex(1)`, then `SetDepthIndex(0)` when the settle transition
  finishes), give that lift a value above `FOCUSED_DEPTH_INDEX` (`DRAGGED_DEPTH_INDEX = 3`) and end
  it with `stacking.Apply(item)` instead of `SetDepthIndex(0)`: at `1` the dropped item ties with
  the selected one, and the `0` reset strips the tier from a dragged item that is selected or
  focused. `OnFinished` is not dependable (§5.1): keep a fallback that also calls
  `stacking.Apply(item)`, e.g. on the next drag start or on removal. Old: dragging another item
  emitted an unsuppressed order signal, so B was rebuilt from A and, besides the requested move, the
  selected item's slot jumped to its raised actor position, normally the end of the row (the latent
  snap described above); it stayed on top unless the dragged item was inserted directly above it.
  Dragging the selected item itself put it into its new slot but back in sibling draw order, no
  longer on top.
- Removing the selected (or focused) item: call `stacking.Forget(item)` before an immediate
  `Remove` or reparent (after the EXIT for an animated removal, §5.1). Resetting the index by hand
  is not enough: a focused item loses focus inside `Remove` (dali-core emits its
  `SceneDisconnectedSignal` synchronously there, and dali-ui's focus manager turns that into
  focus-lost), so `OnItemFocusLost` would put `SELECTED_DEPTH_INDEX` back on the removed view, and
  an unfocused item stays recorded as selected and gets its tier back at its next `Apply` after a
  re-add.
- Accessibility reads the row in sibling order, so the lifted item overlapping a later sibling is
  reported as covered (§4.4); the touch order follows the lift as before.
- Test (headless, §7.5): after every transition, forward and backward, onto and off the last item,
  focus onto the selected item and away from it, focus leaving the stack and
  `FocusManager::ClearFocus()`, deselect, selection reported in both orders (the new item selected
  before and after the old one is deselected), and removal of the selected item, also while it is
  focused (the focus loss that dali-ui delivers inside `Remove` must not put the tier back):
  `GetChildViewAt` and `Actor::GetChildAt` order unchanged, each item's `GetDepthIndex()` equal to
  its expected tier (`0` for a removed item), no `ChildOrderChangedSignal`. Deferred variant: the
  focused item's value above every other item's, and every item that is neither focused nor still
  shrinking at `0` once its animation ends. On screen with the deferred variant: focus moving in
  both directions while the previous item still shrinks, focus moving back and forth across one
  item faster than the shrink, and focus leaving the stack (the item is back at `0` when its shrink
  ends).

## 6. Do not

- Do not set `DEPTH_INDEX` on **direct children of dali-ui containers that stack their own
  children by sibling order**. A former `PRESERVE` raise there only moved the child in A, and
  the container re-asserted its own stacking (scroll bar / modals / content / scrim) on its next
  restack; a `DEPTH_INDEX` lift would outrank those restacks until the application resets it.
  Use the container's own API, or keep a plain `RaiseToTop()` (same effect as before):
  - `RecyclerView`: adds its internal scroller as its first child at initialisation (lowered
    once, never restacked afterwards) and raises its scroll bar to the top at initialisation and
    on every arrange. Item views are children of that scroller, not of the RecyclerView.
    Lifting an item with a **positive** index (reset to `0` on bind, §5.1) is fine. Never run
    `ApplyDrawOrder` (§5.2) on the scroller and never use a **negative** index on an item: the
    group body backgrounds (`GroupBodyDecoration`) live in the same scroller, added and lowered to
    the bottom at index `0`; an item below them is hidden, and backgrounds stamped to `1` or more
    hide every item that is reset to `0`.
  - `ScrollView`: raises its scroll bar to the top at initialisation and in `SetContent`.
  - `Navigator`: pages and modals are direct children; modals are kept above pages with
    `RaiseToTop()` (`RestackModals`). A positive index on a page draws it above every modal, for
    good. Use the Navigator's push/modal API instead.
  - `DialogContainer`: scrim lowered to the bottom, modal content raised to the top. A negative
    index on the content draws it below the scrim.
- Do not leave a non-zero `DEPTH_INDEX` on a view you remove immediately, reparent or hand back
  to a RecyclerView, and do not re-add a view with an unknown history without resetting it
  (§4.7). For an animated removal keep the lift until the EXIT has finished (§5.1).
- Do not read `Dali::Actor::GetChildAt()` order to learn the draw order once any sibling uses
  `DEPTH_INDEX`; read `GetDepthIndex()` or compute `DrawOrder()` (§5.2).
- Do not expect a plain `RaiseToTop()`/`RaiseAbove()` of another sibling to beat a lift that is
  still held in the same parent; drop the lift first (§4 "Later plain reorders").
- Do not call `SetDepthIndex` on a view that is not yet parented as a substitute for an old call
  that ran before the `Add` (it was a no-op then, it is a lift now; §3.2 rule 0).
- Do not expect `SetDepthIndex` to reorder `GetChildViewAt()` or to trigger a layout pass or a
  `LayoutTransition`; if the layout must change, reorder the actor (§5.3).
- Do not use `DevelActor::Property::SIBLING_ORDER` as a "draw order only" control: despite its
  doc comment it reorders the siblings (A), so the layout order follows.
- Do not animate `DEPTH_INDEX` (not animatable); change it at the start and at the end of the
  interaction.
- Do not call `RaiseAbove/LowerBelow` with a possibly-empty handle without an `if(target)` guard.
- Do not touch `DevelActor::Property::CHILDREN_DEPTH_INDEX_POLICY` or create `LAYER_3D` layers
  for UI content that relies on `DEPTH_INDEX`.

---

## 7. Verification checklist after migrating an application

1. The greps of §2 return no hit that is still a use of the deprecated API (the per-name grep 3
   may legitimately hit unrelated identifiers); after the removal the application compiles against
   the new dali-ui;
   every binary, plugin and prebuilt library that includes dali-ui headers was rebuilt, and
   `nm -D --undefined-only <file> | grep LayoutOrderPolicy` is empty for each prebuilt artifact
   that is kept (header: Binary compatibility).
2. Every former `PRESERVE` site was classified (§3.2), parent first: non-View parent → plain
   call; dali-ui container parent (§6) → container API / plain call, never `DEPTH_INDEX`; fresh
   `Add` + `RaiseToTop` (or an insert that provably left the child last) → deleted; `Insert*` +
   `RaiseToTop` or re-`Add` + `RaiseToTop` → `DEPTH_INDEX` (application-owned View parent) or
   plain call; no parent at call time → deleted or guarded (rule 0); application-owned View
   parent → `DEPTH_INDEX` (or the plain call for an order-independent `AbsoluteLayout` parent,
   rule 3); run-time parent → `BringToFront` with the container exclusion. Callers of wrappers
   that had a defaulted policy parameter were classified too.
3. Every `DEPTH_INDEX` lift has a matching reset (`SetDepthIndex(0)`, or the remaining state's
   tier, §5.5) on the opposite event, a fallback for cancelled transitions, a guarded reset
   (skipped when the view was lifted again since its opposite event, §5.5 `StackFocusLift`) where
   it is deferred to an animation's `FinishedSignal`, and a reset before an immediate removal/reparenting/
   recycling/re-add (after the EXIT for an animated removal), or uses the explicit stacking of
   §5.2 on an application-owned parent.
4. Every former `RaiseAbove/LowerBelow` site guarantees a non-empty sibling target or is guarded
   with `if(target)`.
5. Run the screens that lift views (focused cards, dragged items, popups inside a layout) and
   confirm: the lifted view is drawn on top, receives touch first, stays on top for the whole
   shrink/slide-back animation if that is expected (also when the lift moves to an earlier
   overlapping sibling, §4 "Reset timing"), and the layout positions of all siblings are
   unchanged while it is lifted and after it is reset. Without a display, check the same in a
   test: `GetDepthIndex() > 0` during the lift and `0` (or the remaining state's tier, §5.5)
   after it, `GetChildViewAt`/`IndexOfChildView` and `Actor::GetChildAt` unchanged, no
   `ChildOrderChangedSignal` emission
   (the pattern of dali-ui's `UtcDaliViewDepthIndexDoesNotChangeChildOrderInternalP`), and
   touch-first through the devel `Dali::HitTestAlgorithm::HitTest`. Update the application's own
   tests that asserted an `Actor::GetChildAt` order or a `ChildOrderChangedSignal` emission after
   a former `PRESERVE` raise: both change after the migration.
6. If the application adds siblings while a view is lifted, confirm the newcomer's expected
   stacking (§4, "Children added later").
7. If the application relied on `Actor::GetChildAt` order, on focus order, or on accessibility
   (child order or screen-reader touch exploration) after a `PRESERVE` raise, re-check those
   paths: they now see the unchanged sibling order (§4.4).
8. If a lifted view or an overlapped sibling carries a RenderEffect (blur, off-screen
   rendering), re-test that screen (§4.5).

---

## Evidence (verified 2026-10-28; §5.5 additions verified 2026-10-29; dali-ui line numbers re-derived 2026-10-30)

dali-core: commit 7a05f143c (Version 2.5.45); the installed headers carry the same
`@SINCE_2_5.35` annotations.

- `dali/public-api/actors/actor.h:619-630` — `DEPTH_INDEX` doc: default 0, higher on top, ties → sibling order, draw and hit-test only, `GetChildAt` unaffected, only `LAYER_UI` + `INCREASE`.
- `dali/public-api/actors/actor.h:2160-2182` — `SetDepthIndex(int32_t)` / `GetDepthIndex() const`, `@SINCE_2_5.35`.
- `dali/public-api/actors/actor.h:2605-2663` — `Raise/Lower/RaiseToTop/LowerToBottom` (`@pre` parented), `RaiseAbove/LowerBelow` (`@pre` target is a sibling); `:2608` "Raise actor above the next sibling actor" (any actor, not only Views).
- `dali/public-api/actors/actor.h:997-1029` — `InsertAbove/InsertBelow(child, target)`, `@SINCE_2_5.35`, target must be a child of the parent.
- `dali/public-api/actors/custom-actor-impl.h` — no `Raise*/Lower*/DepthIndex` member (grep empty); `:89` `CustomActor Self() const;` is the way to the handle.
- `dali/public-api/common/dali-common.h:188-193` — `DALI_ASSERT_ALWAYS` logs and throws `Dali::DaliException`.
- `dali/public-api/object/weak-handle.h:144, 214` — `WeakHandle<T>` and `GetHandle()` (used by `LiftTracker`).
- `dali/devel-api/actors/actor-devel.h:102-108` — `SIBLING_ORDER` property doc ("draw order", "Calling Raise, Lower, … will modify this value"); its writability comes from the property table `actor-impl.cpp:149`. `actor-devel.h:125-130, 205-213` — `CHILDREN_DEPTH_INDEX_POLICY`, default `INCREASE`, only under `LAYER_UI`.
- `dali/internal/event/actors/actor-impl.cpp:125, 149` — `DALI_PROPERTY("depthIndex", INTEGER, true, false, false, …)` and `("siblingOrder", INTEGER, true, false, false, …)`: writable, not animatable, not constraint input.
- `dali/internal/event/actors/actor-impl.cpp:1018-1039` — `SetDepthIndex`: early-return on same value, updates only the parent's non-zero counter, does not touch `mChildren`, no order signal, `RequestRenderTaskReorder()` (:1035, non-recursive; `:1474-1480` it requests a task reorder only when the actor itself is an off-screen renderable) and depth-tree rebuild.
- `dali/internal/event/actors/actor-parent-impl.cpp:82` — `AddChild`: `if(&mOwner != oldParent)` (re-`Add` of an existing child does nothing); `:102-111` an anchor places the child directly next to it (`++iter` for "above"; `:109-111` a vanished anchor falls back to appending); `:115` plain `Add` appends (`push_back`); `:118-122, 165-170` — add/remove only adjust the non-zero counter; the child's own value is kept (so a value set on an unparented view takes effect at the later `Add`, §3.2 rule 0).
- `dali/internal/event/actors/actor-parent-impl.cpp:132, 195, 225` — `Add`/`Remove`/`RemoveAll` emit only `ChildAdded`/`ChildRemoved`; `EmitOrderChangedAndRebuild` is called only from the reorder paths (:372, :397, :419, :441, :468, :494, :539). dali-ui's `OnChildAdded` (`dali-ui-foundation/internal/views/view/view-data-impl.cpp:3737-3759`, append fast path :3598-3601) inserts only the newcomer and `OnChildRemoved` (:3988-4062, `mChildren.Erase` :4054) only erases it. Basis for the §5.5 statement that a fresh `Add` or a `Remove` did not trigger the latent snap.
- `dali/public-api/actors/actor.cpp:89-92` → `dali/internal/event/actors/actor-impl.cpp:1687-1690` (`Remove(child, notify)`, `notify = true` by default, `actor-impl.h:197`), `actor-parent-impl.cpp:146-178` (`removed->SetParent(nullptr, notify)` :176), `actor-impl.cpp:2084-2126` (`SetParent(nullptr)`: `mParent = nullptr`, then `DisconnectFromScene(notify)`), `:1803-1821` and `:1840-1849` (`SceneDisconnectedSignal` emitted synchronously, inside `Remove`). Basis for §5.5 `ItemStacking::Forget`: a focused item loses focus inside `Remove`.
- `dali/internal/update/manager/render-instruction-processor.cpp:270, 284` — render item sort key = node depth-tree index (`isLayer3d ? 0 : node->GetDepthIndex()`) plus the renderer's own depth index; `:674-694` overlay renderables sorted with the same comparator. `dali/internal/event/events/hit-test-algorithm-impl.cpp:561-566` — overlay hit roots collected in depth order. Basis for §4.1 (visual depth composition) and the `OVERLAY_2D` statement.
- `dali/public-api/dali-core-version.h:29-31` — `CORE_MAJOR/MINOR/MICRO_VERSION` are run-time constants; `pkg-config --modversion dali3-core dali3-ui-foundation` prints `3.0.0` on a 2.5.46 install (dali-ui takes it from `DALI_UI_VERSION`, `build/tizen/CMakeLists.txt:23-26`); installed `dali/public-api/actors/actor.h:2191` `void SetDepthIndex(int32_t depthIndex);` is the compile-time probe. `dali/devel-api/events/hit-test-algorithm.h:144` — devel `HitTest(...)` for headless touch-order checks.
- Binary compatibility: `nm -DC <prefix>/lib/libdali3-ui-foundation.so.3.0.0 | grep -c LayoutOrderPolicy` = 12 for a release (hidden-visibility) build that has the overloads (six `View` and six `ViewImpl` members, e.g. `Dali::Ui::View::RaiseToTop(Dali::Ui::LayoutOrderPolicy)`) and 0 for one without them; both carry `SONAME libdali3-ui-foundation.so.3` (`readelf -d`; `build/tizen/dali-ui-foundation/CMakeLists.txt:2, 177-178`).
- `dali/internal/event/actors/actor-parent-impl.cpp:302-332` — `SetSiblingOrderOfChild` implements the `SIBLING_ORDER` property with `RaiseChildAbove/LowerChildBelow/RaiseChildToTop/LowerChildToBottom`.
- `dali/internal/event/actors/actor-parent-impl.cpp:351-498` — `RaiseChild/LowerChild` swap with `(*mChildren)[i ± 1]` whatever actor it is; `RaiseChildToTop` (:401-421, guard :404) and `LowerChildToBottom` (:423-443, guard :426) no-op at the end of the list and emit only `if(changed)`; `RaiseChildAbove` moves only when `childIter < targetIter` (:445-470, :452) and `LowerChildBelow` only when `childIter > targetIter` (:472-496, :482), but both set their `raised`/`lowered` flag outside that inner guard while inside the outer one (:448 / :477: child not already at that end **and** target is a sibling), so within that outer condition the order signal is emitted even without a move; a non-sibling target or a child already top-most/bottom-most is a silent no-op; every real move calls `RequestRenderTaskReorderRecursively()` (:371, :396, :418, :440, :467, :493, :538; defined :753-766).
- `dali/internal/event/actors/actor-parent-impl.cpp:498-540` — `InsertChild`: a child of **this** parent = pure reorder with order signal; a new child or one parented elsewhere = `AddChild(child, true, &target, above)` (:509-516), i.e. (re)parent at that position, `ChildAdded` emitted, no order signal.
- `dali/internal/event/actors/actor-parent-impl.cpp:542-608` — depth-first traversal; children visited in render order = `std::stable_sort` by `GetDepthIndex()` (:597-598), ties keep sibling order (order list computed before the `INCREASE`/`EQUAL` branch, :555-557).
- `dali/internal/event/actors/actor-parent-impl.cpp:781-792` — `EmitOrderChangedAndRebuild`.
- `dali/internal/event/events/hit-test-algorithm-impl.cpp:465-476, 561-566` — touch hit test iterates the same render order, top-most first, independent of the depth-index policy.
- `dali/internal/event/render-tasks/render-task-list-impl.cpp:253-258` — off-screen renderables are collected by walking `actor.GetChildAt(i)`; `:344, :354` order indices assigned in that traversal order; `:180-188` tasks stable-sorted by order index.
- `dali/internal/event/actors/actor-impl.cpp:373-396` — sibling-order call without parent → warning, no-op.
- `dali/internal/event/actors/actor-impl.h:2646-2648` — `GetImplementation(Dali::Actor&)` asserts on an empty handle (reached by `Actor::RaiseAbove/LowerBelow(target)`, `dali/public-api/actors/actor.cpp:860-868`).
- `dali/internal/event/actors/layer-impl.cpp:132` — layers default to `LAYER_UI`.
- `dali/internal/update/manager/update-manager.cpp:1037-1041` — an animation notification is queued on finished **or stopped**; `dali/internal/event/animation/animation-playlist.cpp:180-186, 216` — "the animation Finish signal is emitted after Stop() has been called", `EmitSignalFinish()`; `:100-104` a played animation is kept alive by the playlist (fire-and-forget, `dali/public-api/animation/animation.h:72-96`); `animation-impl.cpp:641` `Clear()` → `OnClear(*this, true)` → `mIgnoredAnimations` (`animation-playlist.cpp:119-122`, skipped at :171/:208). Basis for the `FinishedSignal` guard in §5.1.
- `dali/public-api/animation/animation.h:126, 541` — `AnimationSignalType = Signal<void(Animation)>`, `FinishedSignal()`; `dali/public-api/signals/dali-signal.h:520-522` — `Signal<void(Arg0)>::Connect(ConnectionTrackerInterface*, const X& func)` wraps the functor in `CallbackFunctor1<X, Arg0>` (`callback.h:503` calls it with the argument), so a connected function object must accept the signal argument (`Animation` by value; the §5.5 lambda declares it); `dali/public-api/signals/connection-tracker.h:43` — `ConnectionTracker`.
- `dali/public-api/animation/animation.h:500, 508` — `Stop()`, `Clear()`; `dali/internal/event/animation/animation-impl.cpp:603-641` — `Animation::Clear()` sets the `CLEARED` state (:638) and calls `mPlaylist.OnClear(*this, true)` (:641); `animation-playlist.cpp:106-122` — `OnClear` drops the animation from the playlist and records its id in `mIgnoredAnimations`, which `:171` and `:208` skip (the set is cleared at the end of the event loop, :125-132); `animation-impl.cpp:1022-1060` — `HasFinished()` is true only from the `PLAYING`/`STOPPING`/`*_DURING_STOPPING` states, so a `CLEARED` animation reports no finish later either; `dali/internal/update/animation/scene-graph-animation.cpp:327` (`Stop` counts a play, which is what queues the notification) vs `:339-350` (`ClearAnimator`: "Do not make notify"). Basis for §5.5 `StackFocusLift::CancelShrink`: `Clear()` a superseded shrink, never `Stop()` it.
- `dali/public-api/object/weak-handle.h:129` — `WeakHandleBase::Reset()` (`StackFocusLift`, `ItemStacking`).

dali-adaptor: commit 73b9e04ea (Version 2.5.45).

- `dali/devel-api/adaptor-framework/actor-accessible.cpp:102` — the accessible connects `ChildOrderChangedSignal`; `:267-277` — accessibility children are collected in `Actor::GetChildAt` order.
- `dali/internal/accessibility/bridge/bridge-accessible.cpp:559` — `CalculateNavigableAccessibleAtPoint` uses `HitTestAlgorithm` only when the layer is `LAYER_3D`; `:599-602` — otherwise walks `root->GetChildren()` from `rbegin()` (reverse sibling order); `:878-892` — `GetNavigableAtPoint` D-Bus method calls it.

dali-ui: paths are relative to the repository root; line numbers are those of the commit that
deprecated the overloads and added this guide (`git log --diff-filter=A --format=%h --
docs/view-sibling-order-migration.md`) and drift in later trees. `git log --grep` on the title
"Remove View sibling-order APIs duplicating Dali::Actor" shows an earlier removal `b62b35f0` and
its revert `908f9ec0`.

- `dali-ui-foundation/internal/views/view/view-data-impl.cpp:3633-3735` (`ViewDataImpl::Raise` … `LowerBelow`; `RaiseToTop` :3665-3679) — `UPDATE` calls the inherited method; `PRESERVE` wraps the same call in `ScopedSkipChildrenUpdate` **only when `Ui::View::DownCast(self.GetParent())` succeeds**, otherwise falls through to the plain call; `RaiseAbove/LowerBelow` return on an empty target before any policy/parent check (:3699-3702, :3719-3722). `dali-ui-foundation/public-api/views/view.cpp:700-728` and `view-impl.cpp:656-684` forward the six `View` and `ViewImpl` overloads to them. Call sites migrated in the commit that added this guide: the two window-parented drag views of the layout-transition samples (`mWindow.Add(...)` then `RaiseToTop(PRESERVE)` → `RaiseToTop()`, non-View parent: the grid sample's preview `mDragPreview`, `layout-transition-grid-reorder-example.cpp:470-477, 688`, and the reorder sample's floated real card `mDraggedChild`, `layout-transition-reorder-example.cpp:512-513, 783`); `tc-80`/`tc-82` (`mTestArea.Add(...)` with `mTestArea = AbsoluteLayout::New()`, a fresh `Add` to a View parent, then `RaiseToTop(PRESERVE)` → `RaiseToTop()`, a no-op either way); the self-override sample (`UPDATE` → plain call); `text-render-scale` (`RaiseToTop(PRESERVE)` on focus → `SetDepthIndex(FOCUSED_CARD_DEPTH_INDEX)`, plus `SetDepthIndex(DEFAULT_CARD_DEPTH_INDEX)` on focus lost).
- `dali-ui-foundation/internal/views/view/view-data-impl.cpp:3777` — `ChildOrderChangedSignal` connected to `ViewDataImpl::OnChildOrderChanged` (:6938-7009), which rebuilds the logical list from `Actor::GetChildAt` order, returns early when the View-only sequence is unchanged (:6980-6984, comment :6966-6978 on no-move emissions and non-View reorders), tags children `REORDERED` only `if(HasLayoutTransition())` (:6995-7000) and then calls `InvalidateMeasure()` (:7008). `:3751` — a fresh insert takes its logical index from its actor position (`ComputeLogicalChildIndex`). The file never reads or writes `Actor::Property::DEPTH_INDEX` (grep for `DEPTH_INDEX` and `SetDepthIndex`: 0 hits).
- Policy-era handler, same file and function (:6938-7009) — `if(mSkipChildrenUpdate) return;` (:6940-6943), rebuild from `GetChildAt` (:6952), equality early return against the **current** (possibly stale) `mChildren` (:6980-6984), `mChildren = std::move(newChildren)` (:6986), `REORDERED` tagging `if(HasLayoutTransition())` (:6995), `InvalidateMeasure()` (:7008); `ScopedSkipChildrenUpdate` (:129-145) sets the flag for one call and restores the previous value; the six `PRESERVE` paths are its only users (:3641, :3657, :3673, :3689, :3709, :3729). The 2.5.36 handler (`git show 4132428e:dali-ui-foundation/internal/views/view/view-data-impl.cpp:3698-3744`) has no equality check and assigns unconditionally (:3723); the check came with `71d94429` (in 2.5.37.11314, not in 2.5.35.11201 or 2.5.36.11230). Basis for the §1 "latent snap" description.
- Policy implementation bodies, same file: `RaiseToTop(policy)` (:3665-3679), `RaiseAbove(target, policy)` (:3697-3715), `LowerBelow(target, policy)` (:3717-3735): an empty target returns before any check; `PRESERVE` with a `Ui::View` parent takes `ScopedSkipChildrenUpdate` and calls the inherited `Actor` method, every other case calls it directly. With dali-core's guards (`actor-parent-impl.cpp:404, 452, 482`) this gives the §5.5 description of the old focus/selection code: the item moves to the end of A or directly after `next`, `LowerBelow(next)` puts it directly before `next` only when it is after `next` (its original slot only while `next` has not been moved).
- Release history (`git log`, `packaging/dali-ui.spec`): `4a237fb4` = Release 2.5.26.10706 declares the enum at `dali-ui-foundation/public-api/view-types.h:44` (old path); `b62b35f0` (2026-08-20) removed it; `dabae505` = Release 2.5.35.11198 contains `b62b35f0` and not the revert (its `views/view-types.h` has no `LayoutOrderPolicy`); `908f9ec0` reverted; `ed0f889e` = Release 2.5.35.11201 and every later release up to `fb7895b0` = 2.5.45.11629 declare the enum again; the commit that added this guide deprecates it. Basis for the "Applies to" paragraph.
- `dali-ui-foundation/internal/focus-manager/focus-manager-impl.cpp:421, 455` — one focus change calls `keyManager.RemoveFocus(previousGlobal)` and then `keyManager.SetFocus(view)`; `keyinput-focus-manager-impl.cpp:156, 207, 215` — focus-lost is notified before focus-gained, synchronously; `focus-manager-impl.cpp:720` → `view-data-impl.cpp:1809` (`EmitFocusChangedSignal`). Basis for the §4 "Reset timing" statement that blur and the next lift are one event.
- `dali-ui-foundation/public-api/views/view.h:1625-1668` — `GetChildViewCount/GetChildViewAt/IndexOfChildView` docs (logical list excludes EXIT ghosts and non-View actors); `:1686-1699` — an ANIMATE_EXIT child stays in the actor tree as a ghost until its animation finishes; re-adding the same child to the same parent is ignored by the inherited `Actor::Add`.
- `dali-ui-foundation/public-api/layouts/absolute-layout-manager.cpp:400-410`, `grid-layout-manager.cpp:1147-1152`, `flex-layout-manager.cpp:876-881` — layouts enumerate the logical list; `absolute-layout-manager.cpp:443-451` — each child's bounds come from its own `AbsoluteLayoutParams` (`GetChildBounds(params)`), so the result does not depend on child order (§3.2 rule 3 alternative); `layout-transition.h:141` — CHANGE fires only when an existing child's bounds change.
- `dali-ui-foundation/public-api/layouts/stack-layout-manager.cpp:435-440, 650-655` — `StackLayout` measures and arranges its children in logical order (`GetChildViewCount`/`GetChildViewAt`); `stack-layout.h:70` — `StackLayout::New(StackOrientation)`. Basis for §5.5 (a lift never moves an item's slot).
- `dali-ui-foundation/integration-api/recycler-view-impl.cpp:291-295` — only content-changed rebinds (`BindViewHolder` on `mActiveItems`) run while the item is parented; `:801-805, :826-830` — recycling calls `RecycleViewHolder` and then `mScroller.Remove`; `:864-865, :881-882` — created/recycled holders are bound before `mScroller.Add`. Basis for §3.2 rule 0.
- `dali-ui-foundation/public-api/views/view.h:1715-1720` — `View` re-exposes `using Dali::Actor::Raise/Lower/RaiseToTop/LowerToBottom/RaiseAbove/LowerBelow;` next to the deprecated overloads (as every release with the overloads does, 2.5.26.10706 included), so the no-argument calls compile against every dali-ui that has the overloads (migrate-first note in the header).
- `dali-ui-foundation/public-api/layouts/layout-transition-types.h:73` — `LayoutChangeCause::REORDERED`.
- `dali-ui-foundation/public-api/layouts/layout-transition.h:80-101, 527-540` — `OnFinished` fires only on normal completion; not fired on same-slot/cross-slot supersession, `Remove` (EXIT), reparent, destruction / scene disconnection (:101, :540); `:104-109` for `slot==EXIT` the view is already unparented when `OnFinished` fires; `:190-192` reparenting to a different parent cancels an in-flight EXIT. `dali-ui-foundation/internal/layouts/layout-transition-dispatcher.cpp:2681-2689` — EXIT end: `parentHandle.Remove(childHandle)` then `EmitLifecycle(... EXIT)`; `:2510-2517` — `OnViewDestroyed`, also reached from scene disconnection, cancels the active animation / pending EXIT / animator without a lifecycle; `:2341-2350` — no EXIT spec → immediate unparent. `view-data-impl.cpp:3525-3530` — deferred EXIT: the child leaves the logical list but its actor stays under the parent (ghost); `:4139-4148` — scene disconnection unregisters the view. Basis for the §5.1 EXIT rule.
- `dali-ui-foundation/internal/focus-manager/focus-finder.cpp:351-359` (directional: reverse walk of `GetChildAt` at :352-354, `IsBetterCandidate` at :359), `:450-486` (FORWARD/BACKWARD: `stable_sort` by y then x).
- `dali-ui-foundation/internal/render-effects/background-blur-effect-impl.cpp:131` (`OffScreenRenderable::Type::BACKWARD`), `:922` (`RenderUntil(stopperActor)`); `gaussian-blur-effect-impl.cpp:129`, `offscreen-rendering-impl.cpp:69`, `mask-effect-impl.cpp:117-120` (`FORWARD`; `public-api/render-effects/mask-effect.h:47` `MaskEffect : public RenderEffect`).
- `dali-ui-foundation/integration-api/recycler-view-impl.cpp:242, 717` (scroll bar `RaiseToTop` at init and every arrange), `:723-732` (`EnsureScroller`: creates, adds and lowers the scroller once, early return on every later call from :237 init, :706 arrange, :1042), `:805, 830` (recycle → `mScroller.Remove`), `:864-865, 881-882` (`BindViewHolder` then `mScroller.Add`); `public-api/views/recycler/item-adapter.h:92-96, 114-115` — `BindViewHolderSignal`/`RecycleViewHolderSignal`; `public-api/views/recycler/group-adapter.h:105-106` — `BindHeaderViewHolderSignal`/`RecycleHeaderViewHolderSignal`; `public-api/views/recycler/group-body-decoration.cpp:132-133` — group background added to the scroller and lowered to the bottom.
- `dali-ui-foundation/integration-api/scroll-view-impl.cpp:145-147, 196-200` — scroll bar raised at init and in `SetContent`.
- `dali-ui-components/integration-api/navigator/navigator-impl.cpp:602-606` (`AddChildFill` → `Self().Add`, used for pages :146/:222 and modals :362), `:626-633` (`RestackModals` → `modal.RaiseToTop()`).
- `dali-ui-components/integration-api/dialog/dialog-container-impl.cpp:218, 384` — modal content `RaiseToTop`, scrim `LowerToBottom`.
- `samples/text/text-render-scale-example.cpp:64-67, 360, 407, 436-437` — lift on focus, reset on focus lost **before** the scale-back animation is started (`:79` 150 ms, `:369-371` and `:435-437` fire-and-forget `Animation` handles that are never stopped or cleared; `:222, :232` the cards are siblings in a horizontal `StackLayout`; `:465, :475` the user can raise the scale to 2.5, where neighbouring cards overlap); `samples/layout-transition/layout-transition-reorder-example.cpp:344, 669-673` — `InsertBelow` + `SetDepthIndex(TRANSITIONING_DEPTH_INDEX)`, comment: a settle with no CHANGE keeps the lift until the next completed transition.
- `manual-tests/dali-ui-foundation/tc/tc-80-layout-transition-reorder.cpp:162, 526-527`, `tc-82-layout-transition-grid-reorder.cpp:195, 503-504` — `mTestArea` is an `AbsoluteLayout`; fresh `Add` then `RaiseToTop()`.
- `automated-tests/src/dali-ui-foundation-internal/utc-Dali-View-order-internal.cpp:342-365` — `UtcDaliViewDepthIndexDoesNotChangeChildOrderInternalP`.
- No live `Actor::SetDepthIndex` / `Actor::Property::DEPTH_INDEX` / `CHILDREN_DEPTH_INDEX_POLICY` / `LAYER_3D` use in `dali-ui-foundation` or `dali-ui-components` library code (grep, 2026-10-30). The remaining grep hits are a different mechanism or dead: `Visual::Base::SetDepthIndex`, `Renderer::Property::DEPTH_INDEX` (renderer-level depth index, 12 uses) and the visual depth-index ranges of `integration-api/view-depth-index-ranges.h`, and three commented-out `actor.SetDepthIndex(...)` TODO lines in `internal/text/decorator/text-decorator.cpp:862, 938, 981`. dali-ui therefore never overrides an application's actor depth indices.
- Compiler messages in §2 reproduced with `LC_ALL=C g++ 15.2 -std=c++17 -fsyntax-only` against dali-ui headers from which the overloads were removed for the unqualified, qualified, `constexpr auto`, typed-local, parameter, data-member, alias and qualified-typed-local spellings (only the `auto` constant produced no call-line diagnostic), and a caller of a wrapper whose defaulted policy parameter was removed compiled without any diagnostic. Against the headers of this deprecation stage every one of these spellings compiles without a diagnostic, also under `-Wall -Werror`.
- `dali-ui-foundation/internal/focus-manager/focus-manager-impl.cpp:418-429` — `RemoveFocus(previousGlobal/previousLocal)` for any different new target, inside or outside the stack; `:212` (`mClearFocusOnWindowFocusLost(true)`) and `:1896-1901` — window focus loss calls `ClearFocus(currentView, false)`; `:1463-1473` — public `ClearFocus()` (`public-api/focus-manager/focus-manager.h:194`; `:306` `SetClearFocusOnWindowFocusLost`) → `ClearFocus(View, bool)` (:1292-1327), which ends in `KeyInputFocusManager::RemoveFocus(view)` with no `SetFocus`; `:411` and `:2044-2066` — the focused view's `SceneDisconnectedSignal` → `InvalidateFocusView` → `ClearFocus`/`RemoveFocus`; `keyinput-focus-manager-impl.cpp:222-248` (`RemoveFocus` → `NotifyFocusLost`), `:148-157` (→ `NotifyKeyInputFocus(view, false)`), `:195, :321-324` (scene disconnection → `RemoveFocus`). Basis for §5.5: a focus-lost without a following focus-gained in the stack (`StackFocusLift::OnItemFocusLost` forgets the item) and the focus-lost delivered inside `Remove` (`ItemStacking::Forget`).
- `dali-ui-foundation/internal/views/view/selectable-trait-impl.cpp:48` (`mToggleByClickEnabled(true)`), `:99-113` (`mSelected` committed, internal commit observer, then `SelectionChangedSignal`); `public-api/traits/selectable-trait.h:111, 119, 140-155` (`SelectionChangedSignal()`, `IsSelected() const`, toggle-by-click); `internal/views/view/selection-group-impl.cpp:187-195` (the new winner is recorded, then the previous one gets `SetSelected(false)`), `:212, :233` (`SelectedMemberChangedSignal(previous, current, event)`, `(member, View(), event)` on clear); `public-api/views/selection-group.h:47-63` (single selection; membership by parent auto-grouping or by name), `:124` (`Find(View parentView)`), `:185`, `:211` (`ClearSelection()`); `public-api/views/view.h:2032, 2039, 2056` (`AsSelectable()` attaches a trait when none exists, `IsSelectable() const`, `AsGroupSelectable()`). Basis for the §5.5 `Deselect(item)` mapping, the `SelectionGroup` alternative and the multi-selection note.
- `dali-ui-foundation/internal/views/view/view-data-impl.cpp:5752-5753, 5904-5909` — direct children are mirrored when the effective layout direction is `RIGHT_TO_LEFT`; `public-api/layouts/stack-layout-manager.cpp` has no direction handling (grep for `RIGHT_TO_LEFT`/`LayoutDirection`/`Mirror` is empty). Basis for "logically next neighbour" in §5.5.
- `samples/layout-transition/layout-transition-reorder-example.cpp:92` (`mStack = StackLayout::New()`), `:344` (`TRANSITIONING_DEPTH_INDEX = 1`), `:669-673` (`InsertBelow` then `SetDepthIndex(TRANSITIONING_DEPTH_INDEX)`), `:349-354` (`OnTransitionFinished` → `SetDepthIndex(DEFAULT_DEPTH_INDEX)`). Basis for the §5.5 drag-reorder bullet (a second owner lifting the same item).
