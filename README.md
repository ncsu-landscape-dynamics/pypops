# pypops - Python Interface to PoPS

[![Python package test build](https://github.com/ncsu-landscape-dynamics/pypops/workflows/Python%20package%20test%20build/badge.svg)](https://github.com/ncsu-landscape-dynamics/pypops/actions?query=workflow%3A%22Python+package+test+build%22)
[![C++ CMake CI with Python](https://github.com/ncsu-landscape-dynamics/pypops/workflows/C%2B%2B%20CMake%20CI%20with%20Python/badge.svg)](https://github.com/ncsu-landscape-dynamics/pypops/actions?query=workflow%3A%22C%2B%2B+CMake+CI+with+Python%22)
[![Code style: black](https://img.shields.io/badge/code%20style-black-000000.svg)](https://github.com/psf/black)

This is a Python interface to the PoPS (Pest or Pathogen Spread) model.

## Install using pip

Here, we will use a Python virtual environment to separate the pypops package install
from the rest of your system. First, create the environment:

```
python3 -m venv testvenv
```

Then, active the environment ([this will be different for different platfroms and shells](https://docs.python.org/3/library/venv.html)):

```
source testvenv/bin/activate
```

Before using *pip*, upgrate it:

```
python -m pip install --upgrade pip
```

Install pypops from the GitHub repository:

```
pip install git+https://github.com/ncsu-landscape-dynamics/pypops
```

To install from previosly downloaded source code, just use path the source code instead of the URL.
If you want to edit the source code and run without installing it, use *pip*
with the `--editable` flag.

## Run tests using *pytest*

In the virtual environment, install *pytest* using *pip*:

```
pip install pytest
```

Assuming you are in the pypops project directory (e.g. your Git clone),
run the tests:

```
pytest tests/
```

### Test basic functionality in Python

In the virtual environment, start Python:

```
python
```

In Python, import the package and try some basic functions:

```
>>> import _pypops
>>> _pypops.get_float_raster_scalar_type()
dtype('float64')
>>> _pypops.get_integer_raster_scalar_type()
dtype('int32')
```

## Compile using CMake

For development of pypops, you can use CMake build directly (skipping the installation with *pip*).

Configure the project and use build directory for configure and build
outputs:

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

The `-DCMAKE_BUILD_TYPE=Debug` is for debugging build which will, e.g.,
show line numbers when executed with *gdb*. It and can be removed
in other cases.

Build the project:

```
cmake --build build
```

Run tests:

```
CTEST_OUTPUT_ON_FAILURE=1 cmake --build build --target test
```

Optionally, to remove the build directory when you are done, use:

```
rm -rf build
```

## Run Python code for testing manually

Assuming that you have a build from CMake, 
to run Python code from `tests` or any other script, use
in the `build` directory as `PYTHONPATH`
(here, assuming nested `build` directory under the source tree):

```
PYTHONPATH="build" python3 tests/test_pypops.py
```

Run it with Valgrind:

```
PYTHONPATH="build" valgrind python3 tests/test_simulation_function.py
```

Run it with GDB:

```
PYTHONPATH="build" gdb -ex=run --args python3 tests/test_simulation_function.py
```

## Version of PoPS C++ used

Currently, this repository uses `cmake-build` branch from the PoPS.
