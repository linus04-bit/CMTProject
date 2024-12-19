#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "d_grid_functions.c"

// ------------------------------------------------------------------------------------
// This is the main file, containing the two main functions that are called in Python.
// This is the file that constructs the shared library.
// ------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------
// main_function1: This function reads the file with the trees in Geneva and creates an array of Tree structures.
// Inputs:
// - binary string for the filename
// - preallocated list of structures
// - length of CSV document
// ---------------------------------------------------------------------------------------------------------------

void main_function1(char *filename, struct Tree *trees, int size_org) {
    // Memory allocations
    for (int i = 0; i < size_org; i++) {
        memory_allocation((trees+i));
    }
    // Create a list of structures
    readwriteDocument(filename, trees, size_org);
}

// ---------------------------------------------------------------------------------------
// main_function2: This function calculates the values across the grid. 
// Inputs:
// - array of filtered tree structures
// - length of that array
// - 3x predefined Pointer(Pointer(Pointer(double))) for the grids
// - pointer to possible distances in y and x that will be calculated within the function
// - concentration of PM10 in the city
// - concentration of O3 in the city
// - gridsize, chosen to be = 100, can be seen in execution_file
// ---------------------------------------------------------------------------------------

void main_function2(struct Tree *filtered_trees, int size_filtered_trees, double ***grid_OFP, double ***grid_PM10, double ***grid_O3, double ***grid_O3_net_uptake, int *length_y, int *length_x, double C_PM10, double C_O3, int gridsize) {
    // Memory allocations
    double *x = calloc(size_filtered_trees, sizeof(double));
    double *y = calloc(size_filtered_trees, sizeof(double)); 
    if(x == NULL | y == NULL) printf("Memory allocation for x and y failed");
    
    // ----- Calculations -----
    
    // Adjust and add coordinates
    extract_coordinates(filtered_trees, x, y, size_filtered_trees);
    coordinates_adaption(filtered_trees, x, y, size_filtered_trees);
    
    // Calculate lengths
    *length_y = distance(y, size_filtered_trees, gridsize);
    *length_x = distance(x, size_filtered_trees, gridsize);
    printf("These are the lengths of our grid: %d, %d\n", *length_y, *length_x);
    
    // Adjust grid memory to the length we actually need
    *grid_OFP = get_gridarray(*length_y, *length_x);
    *grid_PM10 = get_gridarray(*length_y, *length_x);
    *grid_O3 = get_gridarray(*length_y, *length_x);
    *grid_O3_net_uptake = get_gridarray(*length_y, *length_x);
    printf("Grids got created\n");
    
    // Do calculations within the grid
    calculations(size_filtered_trees, *length_y, *length_x, *grid_OFP, *grid_PM10,*grid_O3, *grid_O3_net_uptake, filtered_trees, C_PM10, C_O3, gridsize);
    printf("Calculations are done\n");
    
    // Free allocated memory
    // Memory for the trees and the grid will be freed separately 
    free(x);
    free(y);
}
