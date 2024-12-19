import numpy as np
import math
from Functions.h_structures import *
import ctypes

# -----------------------------------------------------------------------------------------------------------------------
# This file contains a function that filters through the initial array of trees to identify the genera for which we have 
# all the required data. Trees that do not meet this criterion are excluded. The function then creates a new nd.array for
# the filtered trees. This file also contains a function used to change C_POINTERS into 2D arrays for visualization.
# -----------------------------------------------------------------------------------------------------------------------

# -------------------------------------------------------------------------------------------------
# readfile: This function reads in CSV files and calculates average values for each genus present.
# Input: filepath of CSV file
# Output: dictionary
# Note: processing the data requires a significant amount of time
# -------------------------------------------------------------------------------------------------

def readfile(file_name):
    names_values_dic = {}

    with open(file_name, "r") as file:
        for line in file:
            data = line.split(";")
            full_name = data[0]                                     # extract the whole name: genus + species
            first_name = full_name.split()[0]                       # extract only the genus 
            names_values_dic[first_name] = []                       # create a dictionary with the genera as keys 
            
            n = len(data[1:])                                       # count how many values follow the name  
            if n == 1:                                              # this applies for the conversion factor and the shading coefficient 
                value = float(data[1])
                names_values_dic[first_name].append(value)          # append the value
            elif n == 3:                                            # this applies for EF: there are three values following the name, one for each BVOC  
                v = []                                                
                for i in range(1, 4):
                    if data[i] == "None" or data[i] == "None\n":    # if there is no data available (None), set the value to 0.0 
                        data[i] = 0.0
                    v.append(float(data[i]))
                names_values_dic[first_name].append(v)              # append a list with the three EF values 
        
        for key in names_values_dic.keys():
            nbr_values = len(names_values_dic[key])                 # count how many values were found for each genus in the dictionary
            
            if type(names_values_dic[key][0]) == float:             # this applies for the conversion factor and the shading coefficient  
                sum_k = sum(names_values_dic[key])
                names_values_dic[key] = sum_k / nbr_values          # calculate the average value associated with each genus 

            elif type(names_values_dic[key][0]) == list:            # this applies for EF: we need to calculate the average value for each BVOC 
                mean_values = [0, 0, 0]
                for i in range(3):
                    v = 0
                    for j in range(nbr_values):
                        v += names_values_dic[key][j][i]
                    mean_values[i] = v / nbr_values
                names_values_dic[key] = mean_values 
  
    return names_values_dic     

# --------------------------------------------------------------------------------------------------------------------------------------------------------------              
# filter_trees: This function creates dictionaries based on different files containing scientific parameters. An average value of the scientific parameter
#               is calculated for each tree genus present in that file. Then, the function iterates over the array of trees in Geneva and tests if data can 
#               be found for the tree (only the genus is considered) in ALL of the dictionaries. Only in that case, the tree is added to a new array containing
#               the filtered trees.
# Inputs: filled tree array, empty tree array, four file paths
# Outputs: filtered tree array (fills the empty tree array)
# --------------------------------------------------------------------------------------------------------------------------------------------------------------

def filter_trees(trees, filtered_trees, conversion_factor, EF, shading_coeff, MIR):
    dic_conversion_factor = readfile(conversion_factor)
    dic_EF = readfile(EF)
    dic_shading_coefficient = readfile(shading_coeff)        # filtering is done for conversion factor, EF and shading coefficient; it is not necessary for MIR
    dic_MIR = readfile(MIR)                                           

    names1 = list(dic_conversion_factor.keys())
    names2 = list(dic_EF.keys())
    names3 = list(dic_shading_coefficient.keys())
    counts = 0
    hits = 0
    for i in range(len(trees)):
        full_species_name = str(trees[i]['species_name'])
        first_species_name = full_species_name.split()[0]         # for each tree in the array of trees in Geneva, extract the genus
        counts += 1
        if first_species_name in names1 and first_species_name in names2 and first_species_name in names3:    # test if the genus is present in ALL the dictionaries containing the scientific parameters 
            filtered_trees = np.append(filtered_trees, trees[i])
            index1 = names1.index(first_species_name)
            index2 = names2.index(first_species_name)
            index3 = names3.index(first_species_name)
            filtered_trees[hits]['conversion_factor'] = dic_conversion_factor[first_species_name]              # add the scientific parameters to the tree structure 
            filtered_trees[hits]['mass_emission_factor'] = dic_EF[first_species_name]
            filtered_trees[hits]['shading_factor'] = dic_shading_coefficient[first_species_name]
            filtered_trees[hits]['max_incremental_reactivity'][0] = dic_MIR['isoprene']                        # furthermore, add the MIR data to the tree structure
            filtered_trees[hits]['max_incremental_reactivity'][1] = dic_MIR['monoterpenes']
            filtered_trees[hits]['max_incremental_reactivity'][2] = dic_MIR['sesquiterpenes']
            hits += 1
        
    return filtered_trees

# -------------------------------------------------------------------------
# c_pp_to_np: This function turns a pointer pointer into a 2D NumPy array.
# Input: length in y and x, POINTER(POINTER(ctypes.c_double))
# Output: 2D NumPy array
# -------------------------------------------------------------------------

def c_pp_to_np(y,  x, grid):
    grid_np = np.zeros((y, x))     # initialize a NumPy array
    for i in range(y):
        row_ptr = grid[i]          # dereference the row pointer
        grid_np[i, :] = np.ctypeslib.as_array(row_ptr, shape=(x,))
    return grid_np
