# PoPS model Python API - Main simulation interface
#
# Authors: Vaclav Petras (wenzeslaus gmail com)
#
# Copyright (C) 2020 by the authors.
#
# The code contained herein is licensed under the GNU General Public
# License version 2 or later. You may obtain a copy of the GNU General Public
# License at the following location:
# http://www.gnu.org/copyleft/gpl.html

"""PyPoPS - Main simulation interface
"""

import json

import _pypops

# import rasterio


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
    # pylint: disable=too-many-arguments,too-many-locals
    """Run one PoPS simulation"""
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


def json_to_config(config,):
    """Run one PoPS simulation"""
    with open(config) as config_file:
        config_values = json.load(config_file)
    result = _pypops.PyPopsConfig()
    for key, value in config_values.items():
        setattr(result, key, value)
    return result


class PopsModel:
    def __init__(self, config_file):
        self._config = json_to_config(config_file)
        self._model = _pypops.PyPopsModel(self._config)
        # Arrays (grids/rasters) as properties
        self.infected = None
        self.susceptible = None
        self.total_plants = None
        self.mortality_tracker = None
        self.dispersers = None
        self.temperature = None
        self.weather_coefficient = None
        
        # TODO: We may want this private or a read only property.
        self.step = 0

    def _all_arrays_set(self):
        arrays = [
            self.infected,
            self.total_plants,
            self.susceptible,
            self.mortality_tracker,
            self.dispersers,
            self.temperature,
            self.weather_coefficient,
        ]
        for array in arrays:
            if array is None:
                return False
        return True

    def _check_arrays(self):
        arrays = [
            (self.infected, "infected"),
            (self.susceptible, "susceptible"),
            (self.total_plants, "total_plants"),
            (self.mortality_tracker, "mortality_tracker"),
            (self.dispersers, "dispersers"),
            (self.temperature, "temperature"),
            (self.weather_coefficient, "weather_coefficient"),
        ]
        for array, name in arrays:
            if array is None:
                raise ValueError(f"Array {name} needs to be set before calling run()")

    def run(self):
        if not self._all_arrays_set():
            # raise ValueError("All arrays need to be set before calling run()")
            self._check_arrays()
        self._model.run(
            infected=self.infected,
            susceptible=self.susceptible,
            total_plants=self.total_plants,
            mortality_tracker=self.mortality_tracker,
            temperature=self.temperature,
            weather_coefficient=self.weather_coefficient,
        )

    def can_advance(self):
        if self.step < self._config.steps:
            return True
        return False

    def run_step(self):
        if self.step >= self._config.steps:
            raise ValueError(f"Already reached maximum number of steps ({self._config.steps})")
        self._model.run_step(self.step, infected=self.infected,
            susceptible=self.susceptible,
            total_plants=self.total_plants,
            mortality_tracker=self.mortality_tracker,
            temperature=self.temperature[self.step],
            weather_coefficient=self.weather_coefficient[self.step])
        self.step += 1

    # def update():
    #     if self._config.use_lethal_temperature:
    #         self._simulation.remove(self.infected, self.susceptible, self.temperature[step], lethal_temperature);
    #     self._simulation.generate(self.dispersers, self.infected, self.weather, self.weather_coefficient[step], self.reproductive_rate);
    #     self._simulation.disperse(self.dispersers, self.susceptible, self.infected,
    #                         self.mortality_tracker, self.total_plants,
    #                         self.outside_dispersers, self.weather, self.weather_coefficient[step],
    #                         self.dispersal_kernel);
