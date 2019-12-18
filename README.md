# pypops

PoPS (Pest or Pathogen Spread) Model Python Interface

## Compile

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

## Run Python code for testing separately

To run Python code from `tests` or any other script, use
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

Currently, this repository needs uses `cmake-build` branch from the PoPS
repository and the following changes.

```
diff --git a/include/pops/simulation.hpp b/include/pops/simulation.hpp
index ceadc95..6a21fd8 100644
--- a/include/pops/simulation.hpp
+++ b/include/pops/simulation.hpp
@@ -50,15 +50,15 @@ class Simulation
 private:
     int width;
     int height;
-    IntegerRaster dispersers;
+    IntegerRaster& dispersers_;
     std::default_random_engine generator;
 public:

-    Simulation(unsigned random_seed, const IntegerRaster &size)
+    Simulation(unsigned random_seed, const IntegerRaster &size, IntegerRaster& dispersers)
         :
           width(size.cols()),
           height(size.rows()),
-          dispersers(height, width)
+          dispersers_(dispersers)
     {
         generator.seed(random_seed);
     }
@@ -122,10 +122,10 @@ public:
                     for (int k = 0; k < infected(i, j); k++) {
                         dispersers_from_cell += distribution(generator);
                     }
-                    dispersers(i, j) = dispersers_from_cell;
+                    dispersers_(i, j) = dispersers_from_cell;
                 }
                 else {
-                    dispersers(i, j) = 0;
+                    dispersers_(i, j) = 0;
                 }
             }
         }
@@ -157,8 +157,8 @@ public:

         for (int i = 0; i < height; i++) {
             for (int j = 0; j < width; j++) {
-                if (dispersers(i, j) > 0) {
-                    for (int k = 0; k < dispersers(i, j); k++) {
+                if (dispersers_(i, j) > 0) {
+                    for (int k = 0; k < dispersers_(i, j); k++) {

                         std::tie(row, col) = dispersal_kernel(generator, i, j);
```
