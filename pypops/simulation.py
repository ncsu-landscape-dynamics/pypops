import _pypops


int_type = _pypops.get_integer_raster_scalar_type()
float_type = _pypops.get_float_raster_scalar_type()


def pops(
    random_seed,
    steps,
    use_lethal_temperature=False,
    lethal_temperature=None,
    infected=None,
    susceptible=None,
    total_plants=None,
    mortality_tracker=None,
    # dispersers=dispersers,
    weather=False,
    temperature=None,
    weather_coefficient=None,
    ew_res=None,
    ns_res=None,
    reproductive_rate=None,
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
):
    result = _pypops.test_simulation(
        random_seed=random_seed,
        steps=steps,
        use_lethal_temperature=use_lethal_temperature,
        lethal_temperature=lethal_temperature,
        infected=infected,
        susceptible=susceptible,
        total_plants=total_plants,
        mortality_tracker=mortality_tracker,
        weather=weather,
        temperature=temperature,
        weather_coefficient=weather_coefficient,
        ew_res=ew_res,
        ns_res=ns_res,
        reproductive_rate=reproductive_rate,
        natural_kernel_type=natural_kernel_type,
        natural_scale=natural_scale,
        natural_direction=natural_direction,
        natural_kappa=natural_kappa,
        use_anthropogenic_kernel=use_anthropogenic_kernel,
        percent_natural_dispersal=percent_natural_dispersal,
        anthro_kernel_type=anthro_kernel_type,
        anthro_scale=anthro_scale,
        anthro_direction=anthro_direction,
        anthro_kappa=anthro_kappa,
    )
    return result
