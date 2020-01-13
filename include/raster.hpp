#ifndef RASTER_HPP
#define RASTER_HPP

#include "helpers.hpp"

#include <pops/raster.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <cstdint>

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
typedef pops::Raster<Integer> IntegerRaster;
typedef pops::Raster<Float> FloatRaster;

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
RasterType py_buffer_info_to_raster(pybind11::buffer b)
{
    typedef typename RasterType::NumberType NumberType;
    pybind11::buffer_info info = b.request();
    raster_compatible_or_throw<NumberType>(info);
    return RasterType(static_cast<NumberType*>(info.ptr), info.shape[0], info.shape[1]);
}

// TODO: The return from function does not work, so unused for now.
// Perhaps a 3D array will be needed for interface anyway.
template<typename RasterType>
std::vector<RasterType> py_buffers_info_to_rasters(std::vector<pybind11::buffer>& buffers)
{
    std::vector<RasterType> rasters;
    rasters.reserve(buffers.size());
    for (auto buffer : buffers) {
        rasters.push_back(py_buffer_info_to_raster<RasterType>(buffer));
    }
    return rasters;
}

template<typename RasterType>
pybind11::buffer_info raster_to_py_buffer_info(RasterType& raster)
{
    typedef typename RasterType::NumberType NumberType;
    return pybind11::buffer_info(
        // Pointer to buffer
        raster.data(),
        // Size of one scalar
        sizeof(NumberType),
        // Python struct-style format descriptor
        pybind11::format_descriptor<NumberType>::format(),
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
    pybind11::class_<RasterType>(m, name.c_str(), pybind11::buffer_protocol())
            .def(pybind11::init([](pybind11::buffer b) {
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
          [](pybind11::buffer b) {
        pybind11::buffer_info info = b.request();
        raster_compatible_or_throw<NumberType>(info);
    });
}

pybind11::object get_float_raster_scalar_type()
{
    pybind11::object np = pybind11::module::import("numpy");
    // https://docs.scipy.org/doc/numpy/user/basics.types.html
    pybind11::object dtype = np.attr("dtype");
    std::vector<pybind11::object> candidates = {
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
        if (sizeof(Float) == pybind11::int_(candidate.attr("itemsize")))
            return candidate;
    }
    throw std::logic_error("Cannot find a compatible scalar type");
}

pybind11::object get_integer_raster_scalar_type()
{
    pybind11::object np = pybind11::module::import("numpy");
    // https://docs.scipy.org/doc/numpy/user/basics.types.html
    pybind11::object dtype = np.attr("dtype");
    std::vector<pybind11::object> candidates = {
        dtype("int32"),  // works for int32_t
        dtype("int64")  // fails for int64_t
        // expected q, got l (both have 8 bytes, using C++ type with id: l)
    };
    for (const auto candidate : candidates) {
        if (sizeof(Integer) == pybind11::int_(candidate.attr("itemsize")))
            return candidate;
    }
    throw std::logic_error("Cannot find a compatible scalar type");
}

#endif // RASTER_HPP
