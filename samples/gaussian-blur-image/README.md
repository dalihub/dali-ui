# Gaussian Blur / Opacity Lab

This sample reproduces the `ImageView` request sequences used by the loading
backdrop and compares them in one interactive window. Every scenario creates a
fresh source view, so "early attach" really installs the effect before the view
is added to the scene.

## Build and run

```sh
. ~/setenv
cmake -S samples -B build/samples \
  -DCMAKE_INSTALL_PREFIX="$DESKTOP_PREFIX" \
  -DDALI_UI_SAMPLE_LIST=gaussian-blur-image
cmake --build build/samples --target gaussian-blur-image.example -j8
./samples/gaussian-blur-image/bin/gaussian-blur-image.example
```

The sample can also be configured at startup with
`--resource-ready-attach`, `--resource-ready-opaque`,
`--resource-ready-epsilon`, `--early-epsilon`, `--early-zero-static`, or
`--continuous`.

## Scenarios and actions

- **Original request**: attach before scene, assign the URL, set opacity to
  exactly zero, then fade to `0.55`.
- **ResourceReady + zero**: make opacity `0.0` current, attach and activate the
  effect from `ResourceReadySignal`, then fade to `0.55`.
- **Opaque baseline**: attach from `ResourceReadySignal` at opacity `1.0`.
- **Ready/Early + epsilon**: use opacity `0.02` as the pre-core-fix workaround.
- **Zero static FBO**: remain at opacity `0.0`; the window stays black in that
  area, while the blur FBOs show whether the source was rendered.
- **BlurOnce OFF**: compare the continuous render-effect path.
- Fade-in, fade-out, and cycle actions animate over three seconds so the
  compositor opacity change is easy to see.
- Resource actions change the URL and refresh the existing effect, or refresh
  immediately and request another FBO dump.

Keyboard shortcuts mirror the buttons: `1`-`7`, `I`, `O`, `C`, Space, and `R`.

## Direct FBO read-back

After applying or refreshing the effect, the sample walks the window's
`RenderTaskList`. For every task with a `FrameBuffer`, it calls
`KeepRenderResult()` and retains that task handle. The blur effect's
`FinishedSignal` then calls `GetRenderResult()`, obtains the pixel buffer with
`Dali::Integration::GetPixelDataBuffer()`, and writes it with `EncodeToFile()`.

Files are named:

```text
/tmp/gaussian-blur-run-<run>-fbo-<index>.png
```

These images are the Gaussian blur pipeline's actual FBO stages. They are not
produced by `Capture`, which would perform a separate render of the view tree.
