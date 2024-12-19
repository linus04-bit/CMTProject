import ctypes
import numpy as np

#-------------------------------------------------------------------
# This file contains the class Tree and ndtype Tree used in Python.
#-------------------------------------------------------------------

class Tree(ctypes.Structure):
    _fields_ = [("species_name", ctypes.c_char_p),
                ("crown_height", ctypes.c_double),
                ("crown_diameter", ctypes.c_double),
                ("position_y", ctypes.c_double),
                ("position_x", ctypes.c_double),
                ("position_y_grid", ctypes.c_double),
                ("position_x_grid", ctypes.c_double),
                ("shading_factor", ctypes.c_double),
                ("conversion_factor", ctypes.c_double),
                ("leaves_days", ctypes.c_int),
                ("stomatal_conductance", ctypes.c_double),
                ("mass_emission_factor", ctypes.c_double*3),
                ("max_incremental_reactivity",ctypes.c_double*3),
                ("leaf_area", ctypes.c_double),
                ("leaf_dry_wright", ctypes.c_double),
                ("OFP_hourly", ctypes.c_double),
                ("OFP_yearly", ctypes.c_double),
                ("PM10_yearly", ctypes.c_double),
                ("O3_instantaneous", ctypes.c_double),
                ("O3_yearly", ctypes.c_double),
                ("O3_removal_yearly", ctypes.c_double),
                ("O3_removed_mass_yearly", ctypes.c_double),
                ("O3_net_uptake_yearly", ctypes.c_double)]

# Define the dtype for the NumPy array
tree_dtype = np.dtype([
    ('species_name', np.object_),  # Use np.str_ for string data
    ('crown_height', np.float64),
    ('crown_diameter', np.float64),
    ('position_y', np.float64),
    ('position_x', np.float64),
    ('position_y_grid', np.float64),
    ('position_x_grid', np.float64),
    ('shading_factor', np.float64),
    ('conversion_factor', np.float64),
    ('leaves_days', np.int32),
    ('stomatal_conductance', np.float64),
    ('mass_emission_factor', np.float64, (3,)),
    ('max_incremental_reactivity', np.float64, (3,)),
    ('leaf_area', np.float64),
    ('leaf_dry_wright', np.float64),
    ('OFP_hourly', np.float64),
    ('OFP_yearly', np.float64),
    ('PM10_yearly', np.float64),
    ('O3_instantaneous', np.float64),
    ('O3_yearly', np.float64),
    ('O3_removal_yearly', np.float64),
    ('O3_removed_mass_yearly', np.float64),
    ('O3_net_uptake_yearly', np.float64)
], align = True)
