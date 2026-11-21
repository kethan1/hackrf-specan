# HackRF Specan

HackRF Specan is a native spectrum analyzer for the [HackRF One](https://greatscottgadgets.com/hackrf/one).

## Rationale

While other spectrum analyzers exist for the HackRF (e.g. [pavsa/hackrf-spectrum-analyzer](https://github.com/pavsa/hackrf-spectrum-analyzer), this project is a C++ rewrite that aims to be more robust and maintainable, mainly by utilizing the `hackrf_sweeper` library rather than applying patch files to the `hackrf_sweep` tool.

## Building

Only Ubuntu 26.04 has been tested. Bug reports and PRs for other Linux distributions or OSes and absolutely welcome!

CMake 3.21 or newer, a C++20 compiler, Qt 6, libhackrf, libusb 1.0, and FFTW 3 are required dependencies for building. On Debian or Ubuntu:

```sh
sudo apt install build-essential cmake pkg-config qt6-base-dev qt6-svg-dev \
    libhackrf-dev libusb-1.0-0-dev libfftw3-dev
```

Qwt and `hackrf_sweeper` are git submodules, so ensure they are cloned with:

```sh
git clone --recurse-submodules https://github.com/kethan1/hackrf-specan
```

Or with in an existing clone:

```sh
git submodule update --init --recursive
```

Then build with:

```sh
cd hackrf-specan
cmake -B build
cmake --build build -j"$(nproc)"
```

## Usage

```sh
./build/hackrf-specan
```

If the HackRF is not found, check that `hackrf_info` (from the `hackrf` package) works as your user.

## Acknowledgements

The project builds on [hackrf_sweeper](https://github.com/subreception/hackrf_sweeper), [Qt] (https://www.qt.io), [Qwt](https://qwt.sourceforge.io), and [FFTW](https://www.fftw.org), and takes inspiration from [hackrf-spectrum-analyzer](https://github.com/pavsa/hackrf-spectrum-analyzer).
