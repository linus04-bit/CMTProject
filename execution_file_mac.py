import numpy as np
import ctypes
import os
import matplotlib.pyplot as plt
from matplotlib.colors import SymLogNorm
import time

from Functions.h_structures import *
from Functions.e_filter_trees import *
#----------------------------------------------------------------------------------------
#This is our main python file, which calls functions in C and needs to be run in then end                   
#The first part of this file contains the functions that we call from C
#The second part runs the defined functions
#This file also predefines the input 
#---------------------------------------------------------------------------------------
path = os.getcwd()
#The following value defines, how many values are read into our file  
str_to_filepath = os.path.join(path, 'Data/SIPV_ICA_ARBRE_ISOLE.csv')

#--------------------------------------------import c library---------------------------------------
clibrary = ctypes.CDLL(os.path.join(path, 'Functions/main.so'))


#----------------------------------Fix constant values, can be changed by user-----------------------
C_PM10= 15.2 #Concentration of PM10 --> Kofel: 16.99 (2019)
C_O3 = 48.09 #Concentration of Ozone --> Kofel: 44.786 (2019)
gridsize = 100 #Size of the fields over which we calculate the output
NR_LINES_GE =120000 #Amount of data taken into account from the csv file
#----------------------------------------------------------------------------------------------------
summary = open("Results/summary.txt", "w")
summary.write(f"Concentration of PM10 in microgrammes per m^3 : {C_PM10}\n") 
summary.write(f"Concentration of Ozone in microgrammes per m^3: {C_O3}\n")
summary.write(f"Amount of trees analyzed: {NR_LINES_GE}\n")
summary.write(f"Size of grid cells: {gridsize}\n")
#test runtime
start = time.time()
#------------------------------------------------------------------------------------------------------


#-----------------------------------------Defining the functions in C in python--------------------------------
#This function will read the csv document and write its contents to a csv and an array containing the tree structures
#Main_func2 will be defined later, because the length of the array containing the filtered trees is necessary
main_func1 = clibrary.main_function1
main_func1.argtypes = [ctypes.c_char_p, ctypes.POINTER(NR_LINES_GE*Tree), ctypes.c_int]
main_func1.restype = None


#This function will allocate memory for an array of structures
get_array = clibrary.gettreearray
get_array.argtypes =[ctypes.c_int]
get_array.restype = ctypes.POINTER(NR_LINES_GE* Tree)


#This function will allocate memory for an a 2D array (=grid)
get_gridarray = clibrary.get_gridarray
get_gridarray.argtypes = [ctypes.c_int, ctypes.c_int]
get_gridarray.restype = ctypes.POINTER(ctypes.POINTER(ctypes.c_double))


#This function will print one tree, for control
printtree = clibrary.printTree
printtree.argtypes = [ctypes.POINTER(Tree)]
printtree.restype = None


#This function frees an array of tree structures
free_tree_array1 = clibrary.free_tree_array
free_tree_array1.argtypes = [ctypes.POINTER(NR_LINES_GE*Tree)]
free_tree_array1.restype = None

free_grid = clibrary.free_grid
free_grid.argtypes=[ctypes.POINTER(ctypes.POINTER(ctypes.c_double))]
free_grid.restype = None
#-------------------------------------------------------------------------------------------------


#-----------------------------Defining input for main function 1 and 2----------------------------

#Input main_func 1
trees = get_array(NR_LINES_GE)
str_to_filepath.encode(encoding="utf-8")
filepath = ctypes.c_char_p(str_to_filepath.encode(encoding="utf-8"))
#Input main_func 2
grid_OFP = ctypes.POINTER(ctypes.POINTER(ctypes.c_double))()
grid_PM10 = ctypes.POINTER(ctypes.POINTER(ctypes.c_double))()
grid_O3 = ctypes.POINTER(ctypes.POINTER(ctypes.c_double))()
grid_O3_net_uptake = ctypes.POINTER(ctypes.POINTER(ctypes.c_double))()
length_y = ctypes.c_int(0)
length_x = ctypes.c_int(0)
#---------------------------------------------------------------------------------------------------------------------


#---------------------------------------------------------------------------------------------------------------------
#                                         Running the code
#---------------------------------------------------------------------------------------------------------------------


#------------------------------------------------------main 1----------------------------------------------------------
#Running main1:

main_func1(filepath, trees, NR_LINES_GE)

# Create an empty NumPy array
#tree_dtype is defined in structures file
np_trees = np.empty(NR_LINES_GE, dtype=tree_dtype)

# Populate the NumPy array with data from the trees array
for i in range(NR_LINES_GE):
    tree = trees.contents[i]
    species_name = tree.species_name.decode('utf-8') if tree.species_name else None
    np_trees[i] = (
        species_name,  # Decode using UTF-8
        tree.crown_height,
        tree.crown_diameter,
        tree.position_y,
        tree.position_x,
        tree.position_y_grid,
        tree.position_x_grid,
        tree.shading_factor,
        tree.conversion_factor,
        tree.leaves_days,
        tree.stomatal_conductance,
        tuple(tree.mass_emission_factor),
        tuple(tree.max_incremental_reactivity),
        tree.leaf_area,
        tree.leaf_dry_wright,
        tree.OFP_hourly,
        tree.OFP_yearly,
        tree.PM10_yearly,
        tree.O3_instantaneous,
        tree.O3_yearly,
        tree.O3_removal_yearly,
        tree.O3_removed_mass_yearly,
        tree.O3_net_uptake_yearly
    )
#------------------------------------------------------------------------------------------------------



#--------------------------------------------filter trees---------------------------------------------
#getting paths of csv documents
Shading = os.path.join(path,'Data/shading_coeff.csv' )
EF = os.path.join(path,'Data/EF.csv' )
conversion_factor = os.path.join(path,'Data/conversion_factor.csv' )
MIR = os.path.join(path,'Data/MIR.csv' )
#launch filteration
np_filtered_trees = np.empty(0, dtype=tree_dtype)
np_filtered_trees = filter_trees(np_trees, np_filtered_trees, conversion_factor, EF, Shading, MIR)
#------------------------------------------------------------------------------------------------------



#--------------------------------------grid calculations/ main function2----------------------------------------------
size_filtered_trees = len(np_filtered_trees)
summary.write(f"Amount of trees used for calculations: {size_filtered_trees}\n")

#convert np.array to pointer in c
c_filtered_trees = np_filtered_trees.ctypes.data_as(ctypes.POINTER(size_filtered_trees*Tree))
free_tree_array1(trees)


#defining main_func2 (needs to be defined down here, because of size_filtered_trees)
main_func2 = clibrary.main_function2
main_func2.argtypes = [ctypes.POINTER(size_filtered_trees*Tree), 
                        ctypes.c_int,
                        ctypes.POINTER(ctypes.POINTER(ctypes.POINTER(ctypes.c_double))),
                        ctypes.POINTER(ctypes.POINTER(ctypes.POINTER(ctypes.c_double))),
                        ctypes.POINTER(ctypes.POINTER(ctypes.POINTER(ctypes.c_double))),
                        ctypes.POINTER(ctypes.POINTER(ctypes.POINTER(ctypes.c_double))),
                        ctypes.POINTER(ctypes.c_int),
                        ctypes.POINTER(ctypes.c_int),
                        ctypes.c_double,
                        ctypes.c_double,
                        ctypes.c_int
                       ] 
main_func2.restype = None

#running main_func2
main_func2(
    c_filtered_trees,
    size_filtered_trees,
    ctypes.byref(grid_OFP),
    ctypes.byref(grid_PM10),
    ctypes.byref(grid_O3),
    ctypes.byref(grid_O3_net_uptake),
    ctypes.byref(length_y),
    ctypes.byref(length_x),
    C_PM10,
    C_O3,
    gridsize
)
#----------------------------------------Free Filtered Tree-----------------------------------------------------
#Redefine free_tree_array function for different size of array
free_tree_array2 = clibrary.free_tree_array
free_tree_array2.argtypes = [ctypes.POINTER(size_filtered_trees*Tree)]
free_tree_array2.restype = None

free_tree_array2(c_filtered_trees)

#--------------------------------------affichage----------------------------------------------

#convert Pointer(Pointer(double)) to a 2D array in python.
rows = length_y.value
cols = length_x.value
grid_OFP_np = c_pp_to_np(rows, cols, grid_OFP)
grid_PM10_np = c_pp_to_np(rows, cols, grid_PM10)
grid_O3_np=c_pp_to_np(rows, cols, grid_O3)
grid_O3_net_uptake_np = c_pp_to_np(rows, cols, grid_O3_net_uptake)
#-----Calculate total amounts--------------------------------------------------------
OFP_tot = np.sum(grid_OFP_np)
PM10_tot = np.sum(grid_PM10_np)
O3_tot = np.sum(grid_O3_np)
O3_net_uptake_tot = np.sum(grid_O3_net_uptake_np)
summary.write(f"Total amount of ozone emitted: {OFP_tot} kg/y\n")
summary.write(f"Total amount of PM10 absorbed: {PM10_tot} kg/y\n")
summary.write(f"Total amount of ozone absorbed: {O3_tot} kg/y \n")
summary.write(f"Total net ozone absorbed(+)/emitted(-): {O3_net_uptake_tot} kg/y\n")

# OFP #
plt.imshow(grid_OFP_np, origin='lower', cmap="YlGnBu", interpolation='nearest', norm = SymLogNorm(linthresh=10, vmin=0, vmax=grid_OFP_np.max()))
plt.title("Yearly Ozone Forming Potential (OFP) (kg/y)\n - grid based on indices")
plt.xlabel("x-index")
plt.ylabel("y-index")
cbar = plt.colorbar(label = "OFP_yearly values", ticks = [100, 200, 300, 800])
cbar.ax.set_yticklabels(['100', '200', '300', '800'])
plt.savefig(f'Results/OFP_map_{NR_LINES_GE}_indices.png')



# PM10 # 
plt.clf()
plt.imshow(grid_PM10_np, origin='lower', cmap='YlGnBu', interpolation='nearest', norm = SymLogNorm(linthresh=1, vmin=0, vmax=grid_PM10_np.max()))
plt.title("Yearly PM10 Deposition (kg/y)\n - grid based on indices")
plt.xlabel("x index")
plt.ylabel("y index")
cbar = plt.colorbar(label = "PM10_yearly values", ticks = [1,2,3,4,5])
cbar.ax.set_yticklabels(['1', '2', '3', '4', '5'])
plt.savefig(f'Results/PM10_map_{NR_LINES_GE}_indices.png')


#O3_uptake
plt.clf()
plt.imshow(grid_O3_np, origin='lower', cmap='YlGnBu', interpolation='nearest', norm = SymLogNorm(linthresh=2, vmin=0, vmax=grid_O3_np.max()))
plt.title("Yearly amount of O3 absorbed (kg/y)\n - grid based on indices")
plt.xlabel("x index")
plt.ylabel("y index")
cbar = plt.colorbar(label = "O3_removed_mass values", ticks = [1,2,3,4,5,6,7,8])
cbar.ax.set_yticklabels(['1', '2', '3', '4', '5', '6', '7', '8'])
plt.savefig(f'Results/O3_map_{NR_LINES_GE}_indices.png')

# O3__net_uptake #

plt.clf()
plt.imshow(grid_O3_net_uptake_np, origin='lower', cmap='afmhot', interpolation= 'nearest',  norm = SymLogNorm(linthresh=10, vmin=grid_O3_net_uptake_np.min(), vmax=0))
plt.title("Yearly net amount of O3 absorbed (kg/y)\n - grid based on indices")
plt.xlabel("x index")
plt.ylabel("y index")
cbar = plt.colorbar(label = "O3_net_removed_mass values", ticks = [-100, -200, -300, -400, -600, -800])
cbar.ax.set_yticklabels(['-100', '-200', '-300', '-400', '-600', '-800'])
plt.savefig(f'Results/O3_net_uptake_map_{NR_LINES_GE}_indices.png')

print("This was the last grid.")
#-------------------------------------free memory-----------------------------------------------
free_grid(grid_O3)
free_grid(grid_O3_net_uptake)
free_grid(grid_OFP)
free_grid(grid_PM10)
end = time.time()
summary.write(f"Run time: {end- start}s")
summary.close()
print("Done")
#-------------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------------
#                                       end
#-------------------------------------------------------------------------------------------------

