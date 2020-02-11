import numpy as np

# we only test the C++ interface here
import _pypops as pypops


def test_basic_raster_functions_with_array():
    a = pypops.return_new_raster(start_number=300)
    a = np.array(a, copy = False)
    print(a)

    c = np.array([[1.1, 2, 3],
                  [4, 6, 7]], order='C')
    pypops.modify_existing_raster(c, 100)
    print(c)

    b = np.array([[0.6, 0.8, 0.7],
                  [0.2, 0.8, 0.5]])
    pypops.modify_existing_raster(b, 100)
    print(b)

    d = np.array([[2,1,0],
                  [4,6,7]], dtype=np.float64)
    pypops.modify_existing_raster(d, 100)
    print(d)

    # The test code ends here.
    # All code below is expected to fail (and thus it is disabled).

    # f = np.array([[1.1, 2, 3],
    #               [4, 6, 7]], order='F')
    # pypops.modify_existing_raster(f, 100)
    # print(f)

    # ff = np.array(np.array([1.1, 2, 3, 4, 5, 6]).reshape(1, 1, 6, order='F'), order='F')
    # print(ff)
    # pypops.modify_existing_raster(ff, 100)
    # print(ff)


    # i = np.array([[1, 2, 3],
    #               [4, 6, 7]])
    # pypops.modify_existing_raster(i, 100)
    # print(i)


if __name__ == "__main__":
    test_basic_raster_functions_with_array()
