#include <pops/raster.hpp>
#include <pops/simulation.hpp>
#include <pops/radial_kernel.hpp>

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

// py::format_descriptor has some numpy incopatible behavior:
// long long and long require np.longlong otherwise format won't match
// although the sizes are the same.
// TODO: This may need to be fixed as this may behave differently on
// different platforms based on what long is.
// The problem may be in the choice of letter as info.format contains
// the letter "l" which is not in py::format_descriptor
// https://docs.scipy.org/doc/numpy/user/basics.types.html
// https://docs.python.org/3/library/struct.html
// C++: long long
// Python: dtype=np.longlong
// C++: int
// dtype=np.int32
// C++: std::int32_t
// dtype=np.int32
// C++ long and Python np.long or std::int64_t and np.int64
// reults in "expected q (bytes: 8, C++ typeid: l); got l (bytes: 8)"
typedef int32_t Integer;
// Floating point types are expected to be IEEE
typedef double Float;
typedef Raster<Integer> IntegerRaster;
typedef Raster<Float> FloatRaster;

template <typename T>
std::string to_string(const T& value) {
    std::stringstream stream;
    stream << value;
    return stream.str();
}

std::string to_string(const py::buffer_info& info) {
    std::stringstream stream;
    stream << "itemsize: " << info.itemsize << "\n"
           << "format: " << info.format << "\n"
           << "ndim: " << info.ndim << "\n"
           // TODO: generalize
           << "shape[0], shape[1]: " << info.shape[0] << ", " << info.shape[1] << "\n"
           << "strides[0], strides[1]: " << info.strides[0] << ", " << info.strides[1] << "\n"
           << "strides / itemsize: " << info.strides[0] / info.itemsize << ", " << info.strides[1] / info.itemsize << "\n";
    // TODO: tell if it is C or F order (silent otherwise)
    return stream.str();
}

template<typename Number>
void compatible_scalar_type_or_throw(const py::buffer_info& info)
{
    bool diff_format = false;
    bool diff_size = false;

    if (info.format != py::format_descriptor<Number>::format())
        diff_format = true;
    if (info.itemsize != sizeof(Number))
        diff_size = true;
    if (!diff_format && !diff_size)
        return;

    std::string expected_format = py::format_descriptor<Number>::format();
    std::string provided_format = info.format;
    std::string expected_size = to_string(sizeof(Number));
    std::string provided_size = to_string(info.itemsize);
    std::string expected_cpp_typeid = typeid(Number).name();

    if (diff_format && !diff_size)
        throw std::runtime_error(
                "Incompatible scalar format in array: expected " + expected_format + ", got " + provided_format + " (both have " + expected_size + " bytes, using C++ type with id: "+ expected_cpp_typeid + ")");
    else if (!diff_format && diff_size)
        throw std::runtime_error(
                "Incompatible scalar size in array: expected " + expected_size + ", got " + provided_size + " (both have " + expected_format + " format, using C++ type with id: "+ expected_cpp_typeid + ")");
    else
        throw std::runtime_error(
                "Incompatible scalar in array: expected " + expected_format + " format with " + expected_size + " bytes, got " + provided_format + " format with " + provided_size + " bytes (using C++ type with id: "+ expected_cpp_typeid + ")");
}

template<typename Number>
void raster_compatible_or_throw(const py::buffer_info& info)
{
    compatible_scalar_type_or_throw<Number>(info);
    if (info.ndim != 2)
        throw std::runtime_error("Incompatible buffer dimension: expected a 2D array!");
    if (info.strides[0] != info.itemsize * info.shape[1]
            && info.strides[1] != info.itemsize)
        throw std::runtime_error("Incompatible order: expected row-major (C-style) order. Got " + to_string(info)); // TODO: tell if it is the F order
}

/*
// This would be a way how to do handle py::buffer using constructors,
// but since we are extensively relying on operators, making everything
// work smoothly would require additional work.
class IntegerRaster: public IntegerRasterX
{
public:
    using IntegerRasterX::IntegerRasterX;
    IntegerRaster() : IntegerRasterX() {}
    IntegerRaster(py::buffer buffer)
        :
          IntegerRaster(buffer.request())
    {}
    IntegerRaster(py::buffer_info info)
        :
          IntegerRasterX(static_cast<Integer*>(info.ptr), info.shape[0], info.shape[1])
    {
        raster_compatible_or_throw<Integer>(info);
    }
};
*/

template<typename RasterType>
RasterType py_buffer_info_to_raster(py::buffer b)
{
    typedef typename RasterType::NumberType NumberType;
    py::buffer_info info = b.request();
    raster_compatible_or_throw<NumberType>(info);
    return RasterType(static_cast<NumberType*>(info.ptr), info.shape[0], info.shape[1]);
}

// TODO: The return from function does not work, so unused for now.
// Perhaps a 3D array will be needed for interface anyway.
template<typename RasterType>
std::vector<RasterType> py_buffers_info_to_rasters(std::vector<py::buffer>& buffers)
{
    std::vector<RasterType> rasters;
    rasters.reserve(buffers.size());
    for (auto buffer : buffers) {
        rasters.push_back(py_buffer_info_to_raster<RasterType>(buffer));
    }
    return rasters;
}

template<typename RasterType>
py::buffer_info raster_to_py_buffer_info(RasterType& raster)
{
    typedef typename RasterType::NumberType NumberType;
    return py::buffer_info(
        // Pointer to buffer
        raster.data(),
        // Size of one scalar
        sizeof(NumberType),
        // Python struct-style format descriptor
        py::format_descriptor<NumberType>::format(),
        // Number of dimensions
        2,
        // Buffer dimensions
        {raster.rows(), raster.cols()},
        // Strides (in bytes) for each index, row-major order
        {sizeof(NumberType) * raster.cols(),
         sizeof(NumberType) * 1}
    );
}

// We use template parameter instead of py::module to avoid this
// implementation detail by duck typing.
template<typename RasterType, typename PythonModule>
void raster_class(PythonModule& m, const std::string& name) {
    py::class_<RasterType>(m, name.c_str(), py::buffer_protocol())
            .def(py::init([](py::buffer b) {
                     return py_buffer_info_to_raster<RasterType>(b);
                     // return by raw pointer
                     // or: return std::make_unique<Foo>(...); // return by holder
                     // or: return Foo(...); // return by value (move constructor)
                 }))

            .def_buffer(
                raster_to_py_buffer_info<RasterType>
                );
}

template<typename NumberType, typename PythonModule>
void test_compatibility(PythonModule m, std::string name)
{
    m.def(name.c_str(),
          [](py::buffer b) {
        py::buffer_info info = b.request();
        raster_compatible_or_throw<NumberType>(info);
    });
}

py::object get_float_raster_scalar_type()
{
    py::object np = py::module::import("numpy");
    // https://docs.scipy.org/doc/numpy/user/basics.types.html
    py::object dtype = np.attr("dtype");
    std::vector<py::object> candidates = {
//        numpy.attr("double").attr("itemsize"),
//        numpy.attr("single"),
//        numpy.attr("longdouble"),
//        numpy.attr("float16"),
//        numpy.attr("half")
        dtype("float64"),
        dtype("float32"),
        dtype("double"),
        dtype("single"),
        dtype("longdouble")
    };
    for (const auto candidate : candidates) {
        if (sizeof(Float) == py::int_(candidate.attr("itemsize")))
            return candidate;
    }
    throw std::logic_error("Cannot find a compatible scalar type");
}

py::object get_integer_raster_scalar_type()
{
    py::object np = py::module::import("numpy");
    // https://docs.scipy.org/doc/numpy/user/basics.types.html
    py::object dtype = np.attr("dtype");
    std::vector<py::object> candidates = {
        dtype("int32"),  // works for int32_t
        dtype("int64")  // fails for int64_t
        // expected q, got l (both have 8 bytes, using C++ type with id: l)
    };
    for (const auto candidate : candidates) {
        if (sizeof(Integer) == py::int_(candidate.attr("itemsize")))
            return candidate;
    }
    throw std::logic_error("Cannot find a compatible scalar type");
}

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
    //IntegerRaster infected;
    //IntegerRaster susceptible;
    std::vector<std::tuple<int, int>> outside_dispersers;
};

// TODO: an output type seems to be reasonable
// or a higher level object for the model/simulation
// or the customizable output functions will take care of it by a custom
// context which can be these vectors or write functions
Result test_simulation(
        int random_seed,
        //bool use_lethal_temperature,
        //double lethal_temperature,
        IntegerRaster infected,
        IntegerRaster susceptible,
        IntegerRaster total_plants,
        IntegerRaster mortality_tracker,
        bool weather,
        //std::vector<FloatRaster> temperature,
        std::vector<FloatRaster> weather_coefficient,
        //FloatRaster weather_coefficient,
        double ew_res,
        double ns_res,
        double reproductive_rate
        //std::string natural_kernel_type
        // double natural_distance_scale
        )
{
    double natural_distance_scale = 42;
    // TODO: implement in PoPS or or put back to simulation here
    IntegerRaster dispersers(infected.rows(), infected.cols(), 0);
    std::vector<std::tuple<int, int>> outside_dispersers;
    //DispersalKernelType dispersal_kernel = kernel_type_from_string(natural_kernel_type);
    DispersalKernelType dispersal_kernel = kernel_type_from_string("cauchy");
    Simulation<IntegerRaster, FloatRaster> simulation(random_seed, infected, dispersers);
//    /*if (use_lethal_temperature)
//        simulation.remove(infected, susceptible, temperature[0], lethal_temperature);*/
    simulation.generate(infected, weather, weather_coefficient[0], reproductive_rate);
    RadialDispersalKernel kernel(ew_res, ns_res, dispersal_kernel,
                                 natural_distance_scale);
    simulation.disperse(susceptible, infected,
                        mortality_tracker, total_plants,
                        outside_dispersers, weather, weather_coefficient[0],
                        kernel);
    return {outside_dispersers};
}

// TODO: an output type seems to be reasonable
// or a higher level object for the model/simulation
// or the customizable output functions will take care of it by a custom
// context which can be these vectors or write functions
Result test_simulation_wrapper(
        int random_seed,
        //bool use_lethal_temperature,
        //double lethal_temperature,
        py::buffer infected,
        py::buffer susceptible,
        py::buffer total_plants,
        py::buffer mortality_tracker,
        bool weather,
        //std::vector<FloatRaster> temperature,
        std::vector<py::buffer> weather_coefficient,
        //py::buffer weather_coefficient,
        double ew_res,
        double ns_res,
        double reproductive_rate
        //std::string natural_kernel_type
        // double natural_distance_scale
        )
{
    std::vector<FloatRaster> rasters;
    rasters.reserve(weather_coefficient.size());
    for (auto buffer : weather_coefficient) {
        rasters.emplace_back(py_buffer_info_to_raster<FloatRaster>(buffer));
    }

    return test_simulation(
                random_seed,
                py_buffer_info_to_raster<IntegerRaster>(infected),
                py_buffer_info_to_raster<IntegerRaster>(susceptible),
                py_buffer_info_to_raster<IntegerRaster>(total_plants),
                py_buffer_info_to_raster<IntegerRaster>(mortality_tracker),
                weather,
//                py_buffers_info_to_rasters<FloatRaster>(weather_coefficient),
    //{py_buffer_info_to_raster<FloatRaster>(weather_coefficient[0])},
                rasters,
                ew_res,
                ns_res,
                reproductive_rate
            );
}

// TODO: an output type seems to be reasonable
// or a higher level object for the model/simulation
// or the customizable output functions will take care of it by a custom
// context which can be these vectors or write functions
IntegerRaster test_simulation2(
    IntegerRaster infected
)
{
    infected(1, 1) = 200;
    auto result = 5 * infected;
    return result;
}

IntegerRaster test_simulation2_buffer_wrapper(py::buffer b) {
    py::buffer_info info = b.request();
    if (info.format == py::format_descriptor<Integer>::format() && info.itemsize == sizeof(Integer)) {
        raster_compatible_or_throw<Integer>(info);
        return test_simulation2(IntegerRaster(static_cast<Integer*>(info.ptr), info.shape[0], info.shape[1]));
    }
    raster_compatible_or_throw<Integer>(info);
    throw std::logic_error("Raster-array scalar type incompatibility not identified");
}

PYBIND11_MODULE(pypops, m) {
    m.doc() = "Test of pybind11 with PoPS";

    m.def("get_float_raster_scalar_type",
          &get_float_raster_scalar_type);
    m.def("get_integer_raster_scalar_type",
          &get_integer_raster_scalar_type);

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

    m.def("return_new_raster",
          &return_new_raster,
          "start_number"_a);

    m.def("modify_existing_raster",
          [](py::buffer b, Float x) {
        // we convert Float scalar to Integer when needed
        py::buffer_info info = b.request();
        if (info.format == py::format_descriptor<Float>::format() && info.itemsize == sizeof(Float)) {
            raster_compatible_or_throw<Float>(info);
            modify_existing_raster(FloatRaster(static_cast<Float*>(info.ptr), info.shape[0], info.shape[1]), x);
        } else if (info.format == py::format_descriptor<Integer>::format() && info.itemsize == sizeof(Integer)) {
            raster_compatible_or_throw<Integer>(info);
            modify_existing_raster(IntegerRaster(static_cast<Integer*>(info.ptr), info.shape[0], info.shape[1]), Integer(x));
        } else {
            // throw
            raster_compatible_or_throw<Float>(info);
        }
    });

    py::class_<Result>(m, "Result")
             //.def_readonly("infected", &Result::infected)
             //.def_readonly("susceptible", &Result::susceptible)
             .def_readonly("outside_dispersers", &Result::outside_dispersers)
            ;

     m.def("test_simulation",
           &test_simulation_wrapper,
           "random_seed"_a,
           // "use_lethal_temperature"_a,
           // "lethal_temperature"_a,
           "infected"_a,
           "susceptible"_a,
           "total_plants"_a,
           "mortality_tracker"_a,
           //"dispersers"_a,
           "weather"_a,
           // "temperature"_a,
           "weather_coefficient"_a,
           "ew_res"_a,
           "ns_res"_a,
           "reproductive_rate"_a
           //"natural_kernel_type"_a = "cauchy",
           //"natural_distance_scale"_a = 21
           );

     m.def("test_simulation2",
           &test_simulation2_buffer_wrapper);
}
