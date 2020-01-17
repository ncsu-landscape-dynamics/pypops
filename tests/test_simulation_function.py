import numpy as np
import pypops

int_type = pypops.get_integer_raster_scalar_type()
float_type = pypops.get_float_raster_scalar_type()

infected = np.matrix("5 0 0; 0 0 0", dtype=np.int32)
mortality_tracker = np.matrix("0 0 0; 0 0 0", dtype=int_type)
dispersers = np.matrix("0 0 0; 0 0 0", dtype=int_type)
susceptible = np.matrix("10 6 20; 14 15 20", dtype=int_type)
total_plants = np.matrix("15 6 20; 14 15 25", dtype=int_type)
temperature = np.matrix("5 0 5; 0 0 5", dtype=float_type)
weather_coefficient = np.matrix("0.6 0.8 0.7; 0.2 0.8 0.5", dtype=float_type)

a = pypops.test_simulation(
    random_seed=42,
    steps=2,
    use_lethal_temperature=False,
    lethal_temperature=-1.5,
    infected=infected,
    susceptible=susceptible,
    total_plants=total_plants,
    mortality_tracker=mortality_tracker,
    #dispersers=dispersers,
    weather=True,
    temperature=[temperature, temperature],
    weather_coefficient=[weather_coefficient, weather_coefficient],
    #weather_coefficient=weather_coefficient,
    ew_res=100.,
    ns_res=100.,
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
    anthro_kappa=0
    )

print(a)
#print(a.infected)
print(len(a.outside_dispersers))
print(mortality_tracker)
