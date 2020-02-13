import numpy as np
from numpy.testing import assert_array_equal, assert_array_less

from pypops.simulation import pops, int_type, float_type


def test_types_are_different():
    assert int_type != float_type


def test_simulation_function():
    infected = np.array([[5, 0, 0], [0, 0, 0]], dtype=int_type)
    mortality_tracker = np.array([[0, 0, 0], [0, 0, 0]], dtype=int_type)
    # dispersers = np.matrix("0 0 0; 0 0 0", dtype=int_type)
    susceptible = np.array([[10, 6, 20], [14, 15, 20]], dtype=int_type)
    total_plants = np.array([[15, 6, 20], [14, 15, 25]], dtype=int_type)
    total_plants_original = total_plants.copy()
    temperature = np.array([[5, 0, 5], [0, 0, 5]], dtype=float_type)
    weather_coefficient = np.array([[0.6, 0.8, 0.7], [0.2, 0.8, 0.5]], dtype=float_type)

    result = pops(
        random_seed=42,
        steps=2,
        use_lethal_temperature=False,
        lethal_temperature=-1.5,
        infected=infected,
        susceptible=susceptible,
        total_plants=total_plants,
        mortality_tracker=mortality_tracker,
        # dispersers=dispersers,
        weather=True,
        temperature=[temperature, temperature],
        weather_coefficient=[weather_coefficient, weather_coefficient],
        # weather_coefficient=weather_coefficient,
        ew_res=100.0,
        ns_res=100.0,
        reproductive_rate=400.4,
        natural_kernel_type="cauchy",
        natural_scale=20,
        natural_direction="none",
        natural_kappa=0,
        use_anthropogenic_kernel=False,
        percent_natural_dispersal=0,
        anthro_kernel_type="cauchy",
        anthro_scale=0,
        anthro_direction="none",
        anthro_kappa=0,
    )

    assert result, "Result should not be empty"

    # we use 0 - 1 and not 0 because we have only <, not <= test
    minus_one = np.full(total_plants_original.shape, -1, dtype=int_type)
    assert_array_less(minus_one, susceptible)
    assert_array_less(minus_one, infected)
    assert_array_less(minus_one, mortality_tracker)
    assert_array_less(minus_one, total_plants)

    assert len(result.outside_dispersers) == 1434
    assert_array_less(mortality_tracker, total_plants_original + 1)
    assert_array_equal(mortality_tracker, [[10, 6, 20], [14, 15, 20]])


if __name__ == "__main__":
    test_simulation_function()
