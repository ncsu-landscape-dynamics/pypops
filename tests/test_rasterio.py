import rasterio
import numpy as np

from pypops.simulation import PopsModel, int_type, float_type


def geo_file_to_integer_array(filename):
    with rasterio.open(filename, "r") as ds:
        array = ds.read(1)
    if array.dtype != int_type:
        array = np.array(array, dtype=int_type)
    return array

def geo_file_to_float_array(filename):
    with rasterio.open(filename, "r") as ds:
        array = ds.read(1)
    if array.dtype != float_type:
        array = np.array(array, dtype=float_type)
    return array

def geo_file_to_float_array_list(filename):
    result = []
    with rasterio.open(filename, "r") as ds:
        # Read bands one by one
        for array in ds.read():
            if array.dtype != float_type:
                array = np.array(array, dtype=float_type)
            result.append(array)
    return result


def test_with_rasterio2(shared_datadir):
    from _pypops import PyPopsConfig, PyPopsModel

    config = PyPopsConfig()

    model = PyPopsModel(config)


def test_with_rasterio(shared_datadir):

    model = PopsModel(shared_datadir / "sod" / "config.json")
    model.susceptible = geo_file_to_integer_array(shared_datadir / "sod" / "host.tif")
    model.total_plants = geo_file_to_integer_array(shared_datadir / "sod" / "all_plants.tif")
    model.infected = geo_file_to_integer_array(
        shared_datadir / "sod" / "initial_infections.tif"
    )
    model.management = geo_file_to_integer_array(shared_datadir / "sod" / "management.tif")
    model.weather_coefficient = geo_file_to_float_array_list(shared_datadir / "sod" / "weather.tif")

    model.dispersers = np.zeros_like(model.susceptible)
    # TODO: mortality not handled yet
    model.mortality_tracker = np.zeros_like(model.susceptible)
    # TODO: make temperature and other optional parameters, optional
    model.temperature = geo_file_to_float_array_list(shared_datadir / "sod" / "weather.tif")

    model.run()
    
    


def test_with_rasterio(shared_datadir):

    data = shared_datadir / "simple2x2"
    model = PopsModel(data / "config.json")
    model.susceptible = geo_file_to_integer_array(data / "total_plants.tif")
    model.total_plants = geo_file_to_integer_array(data / "total_plants.tif")
    model.infected = geo_file_to_integer_array(
        data / "infected.tif"
    )

    model.dispersers = np.zeros_like(model.susceptible)
    # TODO: mortality not handled yet
    model.mortality_tracker = np.zeros_like(model.susceptible)
    # TODO: make temperature and other optional parameters, optional

    while model.can_advance():
        model.run_step()

# def test_with_rasterio(shared_datadir):
#     host = geo_file_to_array(shared_datadir / "sod" / "host.tif")
#     all_plants = geo_file_to_array(shared_datadir / "sod" / "all_plants.tif")
#     initial_infections = geo_file_to_array(shared_datadir / "sod" / "initial_infections.tif")
#     management = geo_file_to_array(shared_datadir / "sod" / "management.tif")
#     weather = geo_file_to_array(shared_datadir / "sod" / "weather.tif")

#     config_file = shared_datadir / "sod" / "config.json"

#     model = PopsModel()
#     model.initialize(config_file)
#     model.host = host
#     model.all_plants = all_plants
#     model.initial_infections = initial_infections
#     model.management = management
#     model.weather = weather


#     model.update()

#     array_to_geo_file(model.infected)

#     for step in model.steps:
#         model.update()


#     model = PopsMultiModel()
#     model.initialize(config_file)
#     model.host = host
#     model.all_plants = all_plants
#     model.initial_infections = initial_infections
#     model.management = management
#     model.weather = weather

#     model.update()

#     array_to_geo_file(model.infected)


#     model.update()


#     models = [PopsModel() * 10]
#     for model in models:
#         model.initialize(config_file)
#         model.host = host.copy()
#         model.all_plants = all_plants
#         model.infections = initial_infections
#         model.management = management
#         model.weather = weather

#     for model in models:
#         model.update()


#     array_to_geo_file(model.infected)
