> [!CAUTION]
> This page is no longer valid (2026.06.08~)

<br/>

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Fluent-API-(kr))

# Fluent API

The View-family fluent API has been removed.

`View`, `Label`, `InputField`, `InputEditor`, `ScrollView`, layout classes, and other public
APIs that app or framework developers may subclass no longer support method chaining such as:

```cpp
View::New()
  .SetRequestedWidth(100.0f)
  .SetRequestedHeight(100.0f);
```

Use ordinary statements instead:

```cpp
View view = View::New();
view.SetRequestedWidth(100.0f);
view.SetRequestedHeight(100.0f);
```

`As` and `With` were also removed from the View API. Capture handles in local/member variables
and run custom setup in separate statements.

Some builder-like APIs, such as Visuals, AnimationSpec/AnimationBridge, LayoutParams, and Text
style value objects, may still keep fluent chaining where inheritance does not create the same
public API burden.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
