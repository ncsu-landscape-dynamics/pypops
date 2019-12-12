#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <pops/raster.hpp>

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

// We use template parameter instead of py::module to avoid this
// implementation detail by duck typing.
template<typename RasterType, typename PythonModule>
void raster_class(PythonModule& m, const std::string& name) {
    typedef typename RasterType::NumberType NumberType;
    py::class_<RasterType>(m, name.c_str(), py::buffer_protocol())
            .def(py::init([](py::buffer b) {
                     py::buffer_info info = b.request();
                     raster_compatible_or_throw<NumberType>(info);
                     return new RasterType(static_cast<NumberType*>(info.ptr), info.shape[0], info.shape[1]);
                     // return by raw pointer
                     // or: return std::make_unique<Foo>(...); // return by holder
                     // or: return Foo(...); // return by value (move constructor)
                 }))

            .def_buffer(
                [](RasterType& raster) -> py::buffer_info {
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
            });
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
    // return None or throw;
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
    // return None or throw;
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
}
