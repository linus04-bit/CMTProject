import ctypes
import os
import numpy as np
from Functions.H_structures import *
#---------------------------------------------------------------------------------------------
#This file contains the functions that we call from C, except for main_func2 and free_tree_array2
#This file also predefines the input and contains the static variable of the length of the csv
#---------------------------------------------------------------------------------------
path = os.getcwd()
#The following value defines, how many values are read into our file
NR_LINES_GE =70000 #239535   
str_to_filepath = os.path.join(path, 'Data/SIPV_ICA_ARBRE_ISOLE.csv')

#--------------------------------------------import c library---------------------------------------
path = os.getcwd()#For the relative paths--> we'll take wd of folder and do join with either code or data folder
clibrary = ctypes.CDLL(os.path.join(path, 'Functions\main.dll'))

#-----------------------------------------Defining the functions in C in python--------------------------------
#This function will read the csv document and write its contents to a csv and an array containing the tree structures
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
#-----------------------------Defining input for main function 1 and 2----------------------------

#Input main_func 1
trees = get_array(NR_LINES_GE)
str_to_filepath.encode(encoding="utf-8")
filepath = ctypes.c_char_p(str_to_filepath.encode(encoding="utf-8"))
#Input main_func 2
grid_OFP = ctypes.POINTER(ctypes.POINTER(ctypes.c_double))()
grid_PM10 = ctypes.POINTER(ctypes.POINTER(ctypes.c_double))()
grid_O3 = ctypes.POINTER(ctypes.POINTER(ctypes.c_double))()
length_y = ctypes.c_int(0)
length_x = ctypes.c_int(0)
#--------------------------------------------------------------------------------------------------------------------------------------------
