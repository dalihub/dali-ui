# Text Visualizer sample

This sample demonstrates the use of **TextVisualizer**.

## Features

- Demonstrates TextVisualizer rendering behavior.
- Demonstrates exclusion-based dynamic text layout behavior.
- Provides TextVisualizer performance test scenarios.
- Includes text breaker examples used for text layout verification.

## Build

### Ubuntu

Requires DALi environment to be set up first.

```bash
# From dali-ui root
cd samples/text-visualizer
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make -j4
```

Run:

```bash
./bin/text-visualizer-performance.example
```

### GBS build (Tizen)

```bash
# From dali-ui root
gbs build -A armv7l --include-all --packaging-dir samples/text-visualizer/packaging
```

Output: `com.samsung.dali.text-2.0.0-1.armv7l.rpm`

## Controls

- **Escape** or **Back**: Quit the application.
