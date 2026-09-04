# Toast sample

This sample verifies the `Toast` post, timeout, action expansion, explicit
dismissal, signals, action-less mode, and custom `ToastStyle` paths.

## Local build

```sh
cmake -S samples/toast -B samples/toast/build
cmake --build samples/toast/build
```

## GBS build

The Toast sample is packaged as an independent Git repository. Run GBS from
the sample directory, not from the parent `dali-ui` repository.

```sh
cd samples/toast
git init # Required only when this directory is not already a Git repository.
gbs build -A armv7l --include-all --packaging-dir packaging
```

The package id is `com.samsung.dali.toast` and the installed executable is
`toast.example`.
