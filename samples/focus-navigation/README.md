# Focus navigation fallback sample

This sample demonstrates an application-owned algorithm object registered with
`FocusManager::SetFocusNavigationFallback()`.

The sample deliberately does not request initial focus. The first arrow or Tab
navigation asks the fallback for an entry View. Later navigation follows the
custom order `1 → 4 → 2 → 3`, which differs from the Views' geometric order.
At either end the algorithm returns `Stay()`; unsupported directions return
`NotHandled()` so the framework may continue to its default `FocusFinder`.

The controller clears the callback before its target object is destroyed. A
registered member-function callback does not extend the target object's
lifetime.
