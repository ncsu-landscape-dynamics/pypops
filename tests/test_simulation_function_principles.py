import numpy as np
import pypops

int_type = pypops.get_integer_raster_scalar_type()
float_type = pypops.get_float_raster_scalar_type()

infected = np.matrix("5 0 0; 0 0 0", dtype=int_type)
mortality_tracker = np.matrix("0 0 0; 0 0 0", dtype=int_type)
dispersers = np.matrix("0 0 0; 0 0 0", dtype=int_type)
susceptible = np.matrix("10 6 20; 14 15 20", dtype=int_type)
total_plants = np.matrix("15 6 20; 14 15 25", dtype=int_type)
#temperature = np.matrix("5 0 5; 0 0 5", dtype=float_type)
weather_coefficient = np.matrix("0.6 0.8 0.7; 0.2 0.8 0.5", dtype=float_type)


def test_simulation2_wrapper(infected):
    b = pypops.test_simulation2(infected)
    result = np.matrix(b, copy=False)
    return result

result = test_simulation2_wrapper(infected)
print(infected)
print(result)
