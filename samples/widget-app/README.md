# DALI UI Widget App Sample

This adaptor-based widget provider exercises DALI UI widget compatibility.
It provides the `class1` and `class2` widget classes for
`com.samsung.dali.ui.widget`.

The sample verifies touch and key forwarding, resize handling, ContentInfo
bundle updates, MessagePort test commands, and widget fault handling. It is
packaged independently from the DALI UI libraries.

The provider RPM defaults to DALi 3. To build it against DALi 2, pass
`--define "dali_api_major 2"` to `gbs build`. Standalone CMake builds select
the same version with `-DDALI_API_MAJOR=2` (default: `3`).
