# pypops

[![Actions Status](https://github.com/ncsu-landscape-dynamics/pypops/workflows/C%2B%2B%20CMake%20CI%20with%20Python/badge.svg)](https://github.com/ncsu-landscape-dynamics/pypops/actions)
[![Actions Status](https://github.com/ncsu-landscape-dynamics/pypops/workflows/Python%20package%20test%20build/badge.svg)](https://github.com/ncsu-landscape-dynamics/pypops/actions)

PoPS (Pest or Pathogen Spread) Model Python Interface

## Install using pip

Here, we will use a Python virtual environment to separate the pypops package install
from the rest of your system. First, create the environment:

```
python3 -m venv testvenv
```

Then, active the environment ([this will be different for different platfroms and shells](https://docs.python.org/3/library/venv.html)):

```
source bin/activate
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

## Run tests using *pytest*

In the virtual environment, install *pytest* using *pip*:

```
pip install pytest
```

Run the tests:

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
>>> import pypops
>>> pypops.get_float_raster_scalar_type()
dtype('float64')
>>> pypops.get_integer_raster_scalar_type()
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
in the `build` directory (assuming nested one under source tree):

```
cd ../build/; PYTHONPATH="." python3 ../tests/test_pypops.py
```

This adds the library on the path and also ensures that you are in the
current `build` directly in case you recreated the directory in the mean
time (see `rm` step above).

Run it with Valgrind:

```
PYTHONPATH="." valgrind python3 ../tests/test_simulation_function.py
```

Run it with GDB:

```
PYTHONPATH="." gdb -ex=run --args python3 ../tests/test_simulation_function.py
```

## Version of PoPS C++ used

Currently, this repository uses `cmake-build` branch from the PoPS.
