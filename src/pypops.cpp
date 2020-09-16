#include "raster.hpp"
#include "helpers.hpp"

#include <pops/simulation.hpp>
#include <pops/kernel.hpp>
#include <pops/model.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <vector>
#include <sstream>
#include <typeinfo>
#include <cstdint>

namespace py = pybind11;
using namespace pybind11::literals;

using namespace pops;

void modify_existing_raster(FloatRaster raster, Float x)
{
    raster += x;
}

void modify_existing_raster(IntegerRaster raster, Integer x)
{
    raster += x;
}

FloatRaster return_new_raster(double start_number)
{
    FloatRaster raster = {
        {start_number + 11, start_number + 12, start_number + 1.3},
        {start_number + 21, start_number + 22, start_number + 2.3},
    };
    return raster;
}

struct Result
{
    // IntegerRaster infected;
    // IntegerRaster susceptible;
    std::vector<std::tuple<int, int>> outside_dispersers;
};

// TODO: an output type seems to be reasonable
// or a higher level object for the model/simulation
// or the customizable output functions will take care of it by a custom
// context which can be these vectors or write functions
Result test_simulation(
    int random_seed,
    bool use_lethal_temperature,
    double lethal_temperature,
    IntegerRaster infected,
    IntegerRaster susceptible,
    std::vector<IntegerRaster> exposed,
    IntegerRaster total_plants,
    std::vector<IntegerRaster> mortality_tracker,
    IntegerRaster died,
    bool weather,
    std::vector<FloatRaster> temperatures,
    std::vector<FloatRaster> weather_coefficient,
    double ew_res,
    double ns_res,
    double reproductive_rate,
    std::string natural_kernel_type,
    double natural_scale,
    std::string natural_direction,
    double natural_kappa,
    bool use_anthropogenic_kernel,
    double percent_natural_dispersal,
    std::string anthro_kernel_type,
    double anthro_scale,
    std::string anthro_direction,
    double anthro_kappa)
{
    IntegerRaster dispersers(infected.rows(), infected.cols(), 0);
    IntegerRaster zeros(infected.rows(), infected.cols(), 0);
    IntegerRaster resistant(infected.rows(), infected.cols(), 0);
    IntegerRaster quarantine_areas(infected.rows(), infected.cols(), 0);
    std::vector<std::tuple<int, int>> outside_dispersers;
    std::vector<std::vector<int>> movements;

    Config config;
    config.random_seed = random_seed;
    // TODO: all time related here!
    config.model_type = "SI";
    config.use_lethal_temperature = use_lethal_temperature;
    config.lethal_temperature = lethal_temperature;
    config.weather = weather;
    config.ew_res = ew_res;
    config.ns_res = ns_res;
    config.rows = infected.rows();
    config.cols = infected.cols();
    config.reproductive_rate = reproductive_rate;
    config.natural_kernel_type = natural_kernel_type;
    config.natural_scale = natural_scale;
    config.natural_direction = natural_direction;
    config.natural_kappa = natural_kappa;
    config.use_anthropogenic_kernel = use_anthropogenic_kernel;
    config.percent_natural_dispersal = percent_natural_dispersal;
    config.anthro_kernel_type = anthro_kernel_type;
    config.anthro_scale = anthro_scale;
    config.anthro_direction = anthro_direction;
    config.anthro_kappa = anthro_kappa;

    config.create_schedules();

    Treatments<IntegerRaster, FloatRaster> treatments(config.scheduler());
    SpreadRate<IntegerRaster> spread_rate(
        infected, config.ew_res, config.ns_res, config.rate_num_steps());
    QuarantineEscape<IntegerRaster> quarantine(zeros, config.ew_res, config.ns_res, 0);

    Model<IntegerRaster, FloatRaster, FloatRaster::IndexType> model(config);
    for (unsigned step = 0; step < config.scheduler().get_num_steps(); ++step) {
        model.run_step(
            step,
            infected,
            susceptible,
            total_plants,
            dispersers,
            exposed,
            mortality_tracker,
            died,
            temperatures,
            weather_coefficient[step],
            treatments,
            resistant,
            outside_dispersers,
            spread_rate,
            quarantine,
            quarantine_areas,
            movements);
    }
    return {outside_dispersers};
}

// TODO: an output type seems to be reasonable
// or a higher level object for the model/simulation
// or the customizable output functions will take care of it by a custom
// context which can be these vectors or write functions
Result test_simulation_wrapper(
    int random_seed,
    bool use_lethal_temperature,
    double lethal_temperature,
    py::buffer infected,
    py::buffer susceptible,
    std::vector<py::buffer> exposed,
    py::buffer total_plants,
    std::vector<py::buffer> mortality_tracker,
    py::buffer died,
    bool weather,
    std::vector<py::buffer> temperature,
    std::vector<py::buffer> weather_coefficient,
    double ew_res,
    double ns_res,
    double reproductive_rate,
    std::string natural_kernel_type,
    double natural_scale,
    std::string natural_direction,
    double natural_kappa,
    bool use_anthropogenic_kernel,
    double percent_natural_dispersal,
    std::string anthro_kernel_type,
    double anthro_scale,
    std::string anthro_direction,
    double anthro_kappa)
{
    return test_simulation(
        random_seed,
        use_lethal_temperature,
        lethal_temperature,
        py_buffer_info_to_raster<IntegerRaster>(infected),
        py_buffer_info_to_raster<IntegerRaster>(susceptible),
        py_buffers_info_to_rasters<IntegerRaster>(exposed),
        py_buffer_info_to_raster<IntegerRaster>(total_plants),
        py_buffers_info_to_rasters<IntegerRaster>(mortality_tracker),
        py_buffer_info_to_raster<IntegerRaster>(died),
        weather,
        py_buffers_info_to_rasters<FloatRaster>(temperature),
        py_buffers_info_to_rasters<FloatRaster>(weather_coefficient),
        ew_res,
        ns_res,
        reproductive_rate,
        natural_kernel_type,
        natural_scale,
        natural_direction,
        natural_kappa,
        use_anthropogenic_kernel,
        percent_natural_dispersal,
        anthro_kernel_type,
        anthro_scale,
        anthro_direction,
        anthro_kappa);
}

// TODO: an output type seems to be reasonable
// or a higher level object for the model/simulation
// or the customizable output functions will take care of it by a custom
// context which can be these vectors or write functions
IntegerRaster test_simulation2(IntegerRaster infected)
{
    infected(1, 1) = 200;
    auto result = 5 * infected;
    return result;
}

IntegerRaster test_simulation2_buffer_wrapper(py::buffer b)
{
    py::buffer_info info = b.request();
    if (info.format == py::format_descriptor<Integer>::format()
        && info.itemsize == sizeof(Integer)) {
        raster_compatible_or_throw<Integer>(info);
        return test_simulation2(IntegerRaster(
            static_cast<Integer*>(info.ptr), info.shape[0], info.shape[1]));
    }
    raster_compatible_or_throw<Integer>(info);
    throw std::logic_error("Raster-array scalar type incompatibility not identified");
}

PYBIND11_MODULE(_pypops, m)
{
    m.doc() = "C++ PoPS wrapper for PyPoPS";

    m.def("get_float_raster_scalar_type", &get_float_raster_scalar_type);
    m.def("get_integer_raster_scalar_type", &get_integer_raster_scalar_type);

    raster_class<FloatRaster>(m, "FloatRaster");
    raster_class<IntegerRaster>(m, "IntegerRaster");

    test_compatibility<Float>(m, "test_compatibility_Float");
    test_compatibility<Integer>(m, "test_compatibility_Integer");

    test_compatibility<double>(m, "test_compatibility_double");
    test_compatibility<float>(m, "test_compatibility_float");
    test_compatibility<long double>(m, "test_compatibility_long_double");
    test_compatibility<std::int32_t>(m, "test_compatibility_int32_t");
    test_compatibility<std::int64_t>(m, "test_compatibility_int64_t");
    // pypops.test_compatibility_int64_t(np.array([[2,1,0], [4,6,7]], dtype=np.int64))
    // expected q (bytes: 8, C++ typeid: l); got l (bytes: 8)
    test_compatibility<int>(m, "test_compatibility_int");
    test_compatibility<long long>(m, "test_compatibility_long_long");
    test_compatibility<long>(m, "test_compatibility_long");
    // pypops.test_compatibility_long(np.array([[2,1,0], [4,6,7]], dtype=np.long))
    // expected q, got l (both have 8 bytes, using C++ type with id: l)

    m.def("return_new_raster", &return_new_raster, "start_number"_a);

    m.def("modify_existing_raster", [](py::buffer b, Float x) {
        // we convert Float scalar to Integer when needed
        py::buffer_info info = b.request();
        if (info.format == py::format_descriptor<Float>::format()
            && info.itemsize == sizeof(Float)) {
            raster_compatible_or_throw<Float>(info);
            modify_existing_raster(
                FloatRaster(
                    static_cast<Float*>(info.ptr), info.shape[0], info.shape[1]),
                x);
        }
        else if (
            info.format == py::format_descriptor<Integer>::format()
            && info.itemsize == sizeof(Integer)) {
            raster_compatible_or_throw<Integer>(info);
            modify_existing_raster(
                IntegerRaster(
                    static_cast<Integer*>(info.ptr), info.shape[0], info.shape[1]),
                Integer(x));
        }
        else {
            // throw
            raster_compatible_or_throw<Float>(info);
        }
    });

    //    pybind11::class_<Config>(m, "Config");
    //    pybind11::class_<Model>(m, "Model").
    //            .def_readwrite("random_seed", &Config::random_seed);

    py::class_<Result>(m, "Result")
        //.def_readonly("infected", &Result::infected)
        //.def_readonly("susceptible", &Result::susceptible)
        .def_readonly("outside_dispersers", &Result::outside_dispersers);

    // Tip: if you get "number of annotations does not match"
    // but parameters seems to be right, check the commas.
    m.def(
        "test_simulation",
        &test_simulation_wrapper,
        "random_seed"_a,
        "use_lethal_temperature"_a,
        "lethal_temperature"_a,
        "infected"_a,
        "susceptible"_a,
        "exposed"_a,
        "total_plants"_a,
        "mortality_tracker"_a,
        "died"_a,
        "weather"_a,
        "temperature"_a,
        "weather_coefficient"_a,
        "ew_res"_a,
        "ns_res"_a,
        "reproductive_rate"_a,
        "natural_kernel_type"_a,
        "natural_scale"_a,
        "natural_direction"_a,
        "natural_kappa"_a,
        "use_anthropogenic_kernel"_a,
        "percent_natural_dispersal"_a,
        "anthro_kernel_type"_a,
        "anthro_scale"_a,
        "anthro_direction"_a,
        "anthro_kappa"_a);

    m.def("test_simulation2", &test_simulation2_buffer_wrapper);
}
