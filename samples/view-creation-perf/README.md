# DALi UI View Creation Performance

Keys:

- `0`: toggle the root's layout mode between `STANDALONE` (default) and `DEFAULT`
  (also clears the accumulated Average timings)
- `1` / `2`: create 100 / 10000 plain Views
- `3` / `4`: create 100 / 10000 Views with individual Renderers
- `5` / `6`: create 100 / 10000 Views with background colors
- `9`: hide or show the timing labels
- `Escape` / `Back`: quit

The measured interval covers View creation, geometry setup, optional visual or
renderer setup, and adding each View to the scene-connected root. Bulk-created
children use the default layout mode so they do not become independent layout
roots. The timer does not wait for a layout pass or GPU rendering to complete.

The *root* they are added to is `STANDALONE` by default; key `0` rebuilds it as
`DEFAULT` (and back), so the `DEFAULT` root with `DEFAULT` children case can be
measured directly instead of inferred from the `STANDALONE`-root numbers.

It also acts as a control. With no `LayoutTransition` alive anywhere in the
process, the zero-transition gate in `OnChildAdded` skips the window, controller
and resolver hop altogether, so both root modes should now measure the same; the
ancestor-walk cost that used to separate them returns only once a
`LayoutTransition` exists. The toggle takes effect from the next `1`-`6` run, and
every result line, on stderr and in the on-screen log, is suffixed with
`(root=STANDALONE)` or `(root=DEFAULT)` so it stays attributable.

The accumulated *Average* panel carries no mode dimension, so key `0` clears it. Every
Average therefore describes runs from a single root mode; the per-run log lines above it
keep the full history, each labelled with the mode it ran under.

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
