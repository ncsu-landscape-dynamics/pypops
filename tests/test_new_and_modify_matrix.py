import numpy as np
import pypops

a = pypops.return_new_raster(start_number=300)
a = np.matrix(a, copy = False)
print(a)

b = np.matrix("0.6 0.8 0.7; 0.2 0.8 0.5")
pypops.modify_existing_raster(b, 100)
print(b)

c = np.matrix([[1.1, 2, 3],
               [4, 6, 7]])
pypops.modify_existing_raster(c, 100)
print(c)

d = np.matrix([[2, 1, 0],
               [4, 6, 7]], dtype=np.float64)
pypops.modify_existing_raster(d, 100)
print(d)
