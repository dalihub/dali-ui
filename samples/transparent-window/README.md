# Transparent window sample

This sample exercises native per-pixel transparent-window backend support.
The window requests transparency before `Application` creation and uses a
transparent-black background.

Expected behavior:

- the desktop should be visible through the window background;
- the opaque red, green, and black panels should remain visible;
- the blue panel should blend with the desktop at 50% alpha;
- the black glyphs should remain visible on the white label;
- the PNG's black glyph and anti-aliased grayscale edges should remain visible,
  while its white pixels remain white.

On Windows this requires an EGL backend that advertises
`EGL_ANGLE_direct_composition`. If unavailable, the backend uses an opaque
window surface rather than corrupting valid black content with a color key.

The PNG case uses the project-owned
`res/transparent-window-black-controls.png` asset. It intentionally contains
large pure-black regions, pure-white regions, and anti-aliased grayscale edges
so the color-key regression is reproducible without external product assets.

Build `dali-adaptor`, then configure this directory as a standalone CMake
project using the same DALi Windows SDK toolchain. Run the installed executable
after sourcing `dali-env/setenv.ps1`.
