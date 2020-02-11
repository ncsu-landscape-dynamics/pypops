import numpy as np

# we only test the C++ interface here
import _pypops as pypops


def simulation2_wrapper(infected):
    b = pypops.test_simulation2(infected)
    result = np.array(b, copy=False)
    return result


def test_simplified_simulation_function():
    int_type = pypops.get_integer_raster_scalar_type()

    infected = np.array([[5, 0, 0],
                          [0, 0, 0]], dtype=int_type)

    result = simulation2_wrapper(infected)

    print(infected)
    print(result)


if __name__ == "__main__":
    test_simplified_simulation_function()
