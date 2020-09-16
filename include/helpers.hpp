#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <string>
#include <sstream>

template<typename T>
std::string to_string(const T& value)
{
    std::stringstream stream;
    stream << value;
    return stream.str();
}

std::string to_string(const pybind11::buffer_info& info)
{
    std::stringstream stream;
    stream << "itemsize: " << info.itemsize << "\n"
           << "format: " << info.format << "\n"
           << "ndim: " << info.ndim
           << "\n"
           // TODO: generalize
           << "shape[0], shape[1]: " << info.shape[0] << ", " << info.shape[1] << "\n"
           << "strides[0], strides[1]: " << info.strides[0] << ", " << info.strides[1]
           << "\n"
           << "strides / itemsize: " << info.strides[0] / info.itemsize << ", "
           << info.strides[1] / info.itemsize << "\n";
    // TODO: tell if it is C or F order (silent otherwise)
    return stream.str();
}

template<typename Number>
void compatible_scalar_type_or_throw(const pybind11::buffer_info& info)
{
    bool diff_format = false;
    bool diff_size = false;

    if (info.format != pybind11::format_descriptor<Number>::format())
        diff_format = true;
    if (info.itemsize != sizeof(Number))
        diff_size = true;
    if (!diff_format && !diff_size)
        return;

    std::string expected_format = pybind11::format_descriptor<Number>::format();
    std::string provided_format = info.format;
    std::string expected_size = to_string(sizeof(Number));
    std::string provided_size = to_string(info.itemsize);
    std::string expected_cpp_typeid = typeid(Number).name();

    if (diff_format && !diff_size)
        throw std::runtime_error(
            "Incompatible scalar format in array: expected " + expected_format
            + ", got " + provided_format + " (both have " + expected_size
            + " bytes, using C++ type with id: " + expected_cpp_typeid + ")");
    else if (!diff_format && diff_size)
        throw std::runtime_error(
            "Incompatible scalar size in array: expected " + expected_size + ", got "
            + provided_size + " (both have " + expected_format
            + " format, using C++ type with id: " + expected_cpp_typeid + ")");
    else
        throw std::runtime_error(
            "Incompatible scalar in array: expected " + expected_format
            + " format with " + expected_size + " bytes, got " + provided_format
            + " format with " + provided_size
            + " bytes (using C++ type with id: " + expected_cpp_typeid + ")");
}

template<typename Number>
void raster_compatible_or_throw(const pybind11::buffer_info& info)
{
    compatible_scalar_type_or_throw<Number>(info);
    if (info.ndim != 2)
        throw std::runtime_error("Incompatible buffer dimension: expected a 2D array!");
    if (info.strides[0] != info.itemsize * info.shape[1]
        && info.strides[1] != info.itemsize)
        throw std::runtime_error(
            "Incompatible order: expected row-major (C-style) order. Got "
            + to_string(info));  // TODO: tell if it is the F order
}

#endif  // HELPERS_HPP
