#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "d_grid_functions.c"


//-------------------------------------------------------------------------
//This is our main file, containing the two main functions that are called in Python
// This is the file, that constructs our shared library
//----------------------------------------------------------------------------


//----------------Main function 1 reads file and creates array of structures---------
//Inputs
// - binary string for the filename
// - preallocated list of structures
// - length of csv document
void main_function1(char* filename, struct Tree *trees, int size_org){
    //-------------memory allocations-------------------------
    for (int i = 0; i < size_org; i++) {
        memory_allocation((trees+i));
    }
    //---------creating list of structures---------------------
    readwriteDocument(filename, trees, size_org);
}
//----------------------------------------------------------------------------------------


//---------------------------------filter in Python--------------------------------------
//----------------------------------------------------------------------------------------

//----------------------------main function2 calculates the values on the grid-----------
//Inputs:
// - Array of filitered tree structures
// - length of that array
// - 3x predefined Pointer(Pointer(Pointer(double))) for our grids
// -Pointer to possible distances in y and x that will be calculated within the function
// - Concentration of pM10 in the city
// - Concentration of O3 in the city
// - gridsize, chosen to be = 100, can be seen in execution_file

void main_function2(struct Tree * filtered_trees, int size_filtered_trees, double *** grid_OFP, double *** grid_PM10, double *** grid_O3, double ***grid_O3_net_uptake, int* length_y, int* length_x, double C_PM10, double C_O3, int gridsize ){
    //---------------------memory allocation part II----------
    double * x = calloc(size_filtered_trees, sizeof(double));
    double * y = calloc(size_filtered_trees, sizeof(double)); 
    if(x == NULL | y == NULL)printf("Memory allocation for x and y failed");
    //------------------------calculations-------------------------

    //adjust and add coordinates
    extract_coordinates(filtered_trees,x, y, size_filtered_trees);
    coordinates_adaption(filtered_trees, x, y, size_filtered_trees);
    //for(int i = 0; i<size_filtered_trees; i++){
        //printf("%f, %f\n", filtered_trees[i].position_x_grid, filtered_trees[i].position_y_grid);
    //}
    // calculate lengths
    *length_y = distance(y, size_filtered_trees, gridsize);
    *length_x = distance (x, size_filtered_trees, gridsize);
    printf("These are the lengths of our grid: %d, %d\n", *length_y, *length_x);
    //adjust grid memory to the length we actually need
    *grid_OFP = get_gridarray(*length_y, *length_x);
    *grid_PM10 = get_gridarray(*length_y, *length_x);
    *grid_O3 = get_gridarray(*length_y, *length_x);
    *grid_O3_net_uptake = get_gridarray(*length_y, *length_x);
    printf("Grids got created\n");
    //Do calculations within the grid
    calculations(size_filtered_trees, *length_y, *length_x, *grid_OFP, *grid_PM10,*grid_O3, *grid_O3_net_uptake, filtered_trees, C_PM10, C_O3, gridsize);
    printf("Calculations are done\n");
    //----------------------------------------------------------------------------------------

    //------------------free memory-------------------------------- 
    //memory of trees and grid will be freed separately 
    free(x);
    free(y);
}
//---------------------------------------------------------------------------------------------------------------------------------------------