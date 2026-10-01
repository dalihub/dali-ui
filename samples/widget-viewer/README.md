# DALI UI Widget Viewer Sample

This sample creates widgets with `Dali::Ui::WidgetView::New()`. The application
and actor hierarchy own the views. Scene connection resumes a visible widget;
scene disconnection pauses it. Removing it from its parent and releasing the last
handle destroys the view and removes the platform widget.
The provider button (or `P` key) cycles through DALi Toolkit, DALI UI, and NUI
widget providers.

For callback registration, frame delivery, and buffer lifetime details, see
[WidgetView internals (Korean)](../../docs/widget-view-lifecycle.md).

The DALI UI provider is in `../widget-app`; Toolkit and NUI providers are
supplied separately. To run the sample, install:

- `dali3-widget-viewer-engine` (DALi 3), or `libwidget_viewer_dali-engine` (DALi 2)
- `com.samsung.dali.widget` for the Toolkit provider
- `com.samsung.dali.ui.widget` for the optional DALI UI provider
- `Tizen.NUI.WidgetTest` for the optional NUI provider

If an engine or selected provider is unavailable, the sample reports the
failure on screen and remains usable for switching providers or exiting.

The engine package depends on `dali3-widget-viewer-resources`, which supplies the
shared translations and the `unknown.png` default preview selected by the engine.
The library initializes the default `widget_viewer_dali` catalog on first use;
the sample does not need to register its path. Custom installations can set
`WIDGET_VIEWER_LOCALE_DIR` when configuring DALI UI. The app default domain is
left unchanged. First use binds the widget domain to the library catalog path.
App-owned catalogs remain the application's responsibility.

Applications customize messages with `SetLoadingText()` / `SetRetryText()`,
font attributes with `SetLoadingTextFont()` / `SetRetryTextFont()`, and colors
with `SetLoadingTextColor()` / `SetRetryTextColor()`. An empty message restores
the translated default. Visibility remains controlled by the existing visibility setters.

WidgetView follows ImageView's API and Property model: use named methods for
application code, with Property access available for generic configuration.
Widget-specific properties are event-side properties, not animation properties.
For example, after creating a valid view:

```cpp
view.SetPreviewEnabled(true);
view.SetKeepWidgetSizeEnabled(true);
Dali::String widgetId = view.GetWidgetId();
Dali::String content = view.GetContentInfo();

Dali::Ui::Text::FontAttributes font;
font.SetSize(24.0f);
font.SetWeight(Dali::Ui::Text::FontWeight::BOLD);
view.SetLoadingText("Loading widget");
view.SetLoadingTextFont(font);
view.SetLoadingTextColor(Dali::Ui::UiColor(Dali::Color::WHITE));
```

Include `<dali-ui-foundation/public-api/text/style/font-attributes.h>` when
constructing FontAttributes. Only explicitly set font attributes are applied.

`SetCustomShader(vertexSource, fragmentSource)` overrides the image shader;
an empty source uses the default stage. `ClearCustomShader()` restores the
standard shader. Framework code can use the Map properties in
`integration-api/widget-view-integ.h`, including advanced shader options.
The public WidgetView properties and setters do not expose these Map settings. `PERMANENT_DELETE` remains inert.

`WRAP_CONTENT` measures the initial natural size supplied to New(), adjusted
by UI scale and parent constraints. Arranging into a smaller or larger area
resizes the provider but does not overwrite that natural size. Returning from
an explicit requested size to WRAP_CONTENT restores the initial natural size.

Use `1` or Return to resize the red widget, `2` to toggle automatic add/remove
of the blue widget, `3`/`4` to focus a widget for key forwarding, and `F` to
inject a fault into providers that support the sample MessagePort protocol.
Touch input and ContentInfo updates are reported on screen and in the log.
After a created provider exits, each instance attempts automatic recovery up to
five times. Successful recreation does not reset this count; activating the
faulted widget explicitly resets it and requests another launch.

For image lifetime regression checks, `d` detaches/reattaches the red widget
with automatic pause/resume. `r` clears a manual pause, and `c` changes its corner
radius to exercise native shader regeneration after frame replacement.
Pause does not guarantee that a provider stops rendering (the UI sample has
an empty OnPause handler). For a strict last-frame check, temporarily stop
the provider process before detaching, verify the frame after reattaching,
then continue the process. The corresponding host regression test supplies
no replacement frame.

`m` checks that live views share an engine, reject a different viewer app ID,
and can release two probe widgets before their CREATE event. Removal before
CREATE terminates the provider as in the legacy viewer, so other displayed
instances in that process may fault and recover. Results are written to the log.

The shared engine and its service connection stay alive until application
shutdown, even after all views have been destroyed. Individual widget instances
and image resources are released normally. This keeps provider-death monitoring
working when widgets are recreated: on the tested Tizen image, widget-service
loses those notifications after closing and reopening the service in the same
process. The retention policy is confined to the internal WidgetView factory so
it can be changed once the platform issue is fixed.

The sample RPM defaults to DALi 3. To build it against DALi 2, pass
`--define "dali_api_major 2"` to `gbs build`. Standalone CMake builds select
the same version with `-DDALI_API_MAJOR=2` (default: `3`).
