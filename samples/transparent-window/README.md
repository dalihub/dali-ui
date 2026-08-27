# Transparent window sample

This sample exercises the initial Windows transparent-window backend support.
The window requests transparency before `Application` creation and uses a
transparent-black background.

The initial backend is intentionally color-key based:

- the desktop should be visible through the window background;
- the red and green panels should remain visible;
- the black panel should disappear, documenting the known color-key limit;
- per-pixel and partial-alpha desktop composition are deferred to a later
  backend update.

Build `dali-adaptor`, then configure this directory as a standalone CMake
project using the same DALi Windows SDK toolchain. Run the installed executable
after sourcing `dali-env/setenv.ps1`.
