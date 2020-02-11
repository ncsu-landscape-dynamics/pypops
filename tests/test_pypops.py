import numpy as np

# we only test the C++ interface here
import _pypops as pypops


def test_compatibility_of_used_scalar_type():
    a = np.array([[5, 0, 0],
                  [0, 0, 0]])
    b = np.array([[0.6, 0.8, 0.7],
                  [0.2, 0.8, 0.5]])

    tf = pypops.get_float_raster_scalar_type()
    print("Type:", tf)
    pypops.test_compatibility_Float(np.array([[2,1,0], [4,6,7]], dtype=tf))

    ti = pypops.get_integer_raster_scalar_type()
    print("Type:", ti)
    pypops.test_compatibility_Integer(np.array([[2,1,0], [4,6,7]], dtype=ti))

    #m = pypops.Matrix(b)
    m = pypops.FloatRaster(b)
    print(m)
    print(b)

    a = np.array(a, dtype=ti)

    m = pypops.IntegerRaster(a)
    print(m)
    print(a)


def test_compatibility_of_scalar_types():
    pypops.test_compatibility_double(np.array([[2.2,1,0], [4,6,7]]))
    pypops.test_compatibility_double(np.array([[2,1,0], [4,6,7]], dtype=np.float64))
    pypops.test_compatibility_float(np.array([[2,1,0], [4,6,7]], dtype=np.float32))
    pypops.test_compatibility_long_double(np.array([[2,1,0], [4,6,7]], dtype=np.float128))
    pypops.test_compatibility_int32_t(np.array([[2,1,0], [4,6,7]], dtype=np.int32))
    pypops.test_compatibility_long_long(np.array([[2,1,0], [4,6,7]], dtype=np.longlong))

    # pypops.test_compatibility_int(np.array([[2,1,0], [4,6,7]]))
    # pypops.test_compatibility_float(np.array([[2,1,0], [4,6,7]], dtype=np.int32))
    # pypops.test_compatibility_int64_t(np.array([[2,1,0], [4,6,7]], dtype=np.int64))
    # pypops.test_compatibility_long(np.array([[2,1,0], [4,6,7]], dtype=np.long))


if __name__ == "__main__":
    test_compatibility_of_used_scalar_type()
    test_compatibility_of_scalar_types()
