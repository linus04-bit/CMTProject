import numpy as np
import ctypes
#----------------------------------------------------------------
#This file contains all functions written in python
#----------------------------------------------------------------

#----------------Function to turn pointer pointer into a 2D numpy array--------------
def c_pp_to_np( y,  x, grid):
    grid_np = np.zeros((y, x))  # Initialize a NumPy array
    for i in range(y):
        row_ptr = grid[i]  # Dereference the row pointer
        grid_np[i, :] = np.ctypeslib.as_array(row_ptr, shape=(x,))
    return grid_np

#-----------------------Function to transfrom grid based on indices into coordinates-----#
def coordinate_grid(grid_indices, gridsize):
   grid_coordinates = np.repeat(np.repeat(grid_indices, gridsize, axis=0), gridsize, axis=1).astype(np.float32)
   return grid_coordinates