# DALi UI View Creation Performance

Keys:

- `0`: toggle the root's layout mode between `STANDALONE` (default) and `DEFAULT`
  (also clears the accumulated Average timings)
- `1` / `2`: create 100 / 10000 plain Views
- `3` / `4`: create 100 / 10000 Views with individual Renderers
- `5` / `6`: create 100 / 10000 Views with background colors
- `7` / `8`: create 100 / 10000 plain STANDALONE Views
- `t`: toggle a dormant, never-attached `LayoutTransition` on and off
  (also clears the accumulated Average timings)
- `n`: construct 10000 Views with `View::New()` only - no geometry, no scene
- `r`: remove 10000 pre-created Views one by one
- `a`: remove 10000 pre-created Views with a single `RemoveAll`
- `9`: hide or show the timing labels
- `Escape` / `Back`: quit

The removal and creation-only cases take letter keys because every digit is already
taken here.

The measured interval covers View creation, geometry setup, optional visual or
renderer setup, and adding each View to the scene-connected root. Bulk-created
children use the default layout mode so they do not become independent layout
roots. Keys `7` and `8` instead create independent STANDALONE layout roots.
The timer does not wait for a layout pass or GPU rendering to complete.

The *root* they are added to is `STANDALONE` by default; key `0` rebuilds it as
`DEFAULT` (and back), so the `DEFAULT` root with `DEFAULT` children case can be
measured directly instead of inferred from the `STANDALONE`-root numbers.

It also acts as a control. With no `LayoutTransition` alive anywhere in the
process, the zero-transition gate in `OnChildAdded` skips the window, controller
and resolver hop altogether, so both root modes should now measure the same; the
ancestor-walk cost that used to separate them returns only once a
`LayoutTransition` exists. The toggle takes effect from the next `1`-`8` run, and
every result line, on stderr and in the on-screen log, is suffixed with
`(root=..., transitions=...)` so it stays attributable.

The remove keys `r` and `a` pre-create the same 10000 plain Views *unmeasured* and
time only the removal: `r` calls `Remove(child, ANIMATE_EXIT)` once per child, `a`
calls a single `RemoveAll(ANIMATE_EXIT)`. `n` measures `View::New()` on its own, with
no geometry and no scene, so the constructor's cost can be read apart from the add
path's.

Removal here is dominated by per-child list maintenance, quadratic in the child count,
so the gate's delta is small on this flat tree and grows with hierarchy depth.

Key `t` creates one `LayoutTransition` and holds it without ever attaching it to a
View, which flips `HasAnyInstance()` and nothing else. Pressing it around the same runs
therefore measures gate-on against gate-off for *both* gates: the add-side one behind
keys `1`-`8`, and the remove-side one behind `r` and `a`. The steady-state Add measured
by `1`-`8` also exercises the known-parent invalidation entry `OnChildAdded` uses, so
that path needs no key of its own.

Unlike key `0`, key `t` does **not** rebuild the scene - there is nothing about the tree
for it to change. It takes effect immediately, for the very next run.

The accumulated *Average* panel carries neither a mode nor a gate dimension, so keys `0`
and `t` both clear it. Every Average therefore describes runs from a single root mode and
a single gate state; the per-run log lines above it keep the full history, each labelled
with the mode and gate state it ran under.

## Ubuntu

```sh
cd samples/view-creation-perf
cmake --fresh -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
make -j8
./bin/view-creation-perf.example
```

## Tizen GBS

```sh
gbs build -A armv7l --include-all --packaging-dir samples/view-creation-perf/packaging
```
