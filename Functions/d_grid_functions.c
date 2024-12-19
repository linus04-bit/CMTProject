#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "b_extract_data_and_memory.c"

//-----------------------------------------------------------------------
// This file includes the functions related to the creation of the grid.
//-----------------------------------------------------------------------

// -----------------------------------------------------------------
// max: This function finds the maximum within an array of doubles.
// Input: Array, size of array
// Output: Max value
// -----------------------------------------------------------------

double max(double *list, int size_trees_array) {
    double max = 0.0;
    for(int i = 0; i < size_trees_array; i++) {
        if(list[i] > max) max = list[i];
    }
    return max;
}

// -----------------------------------------------------------------
// min: This function finds the minimum within an array of doubles.
// Input: Array, size of array
// Output: Min value
// -----------------------------------------------------------------

double min(double *list, int size_trees_array) {
    double min = *list;
    for(int i = 0; i < size_trees_array; i++) {
        if (list[i] < min) min = list[i];
    }
    return min;
}

// ------------------------------------------------------------------------------------------------------
// extract_coordinates: This function writes the coordinates into the structure as an array of doubles.
// Input: Tree array, array of coordinates in x and y and size of the tree array
// Output: None
// ------------------------------------------------------------------------------------------------------

void extract_coordinates(struct Tree *trees, double *x, double *y, int size_trees_array){
    for(int i = 0; i < size_trees_array; i++) {
        x[i] = trees[i].position_x;
        y[i] = trees[i].position_y;
    }
}

// --------------------------------------------------------------------------------------------------------------------------
// coordinates_adaption: This function adds an additional field to the Tree structure and adjusts the tree positions so that 
//                       the origin of the coordinate system is set at the tree located at the most southwestern point.
// Input: Tree array, array of coordinates in x and y and size of the tree array
// Output: None
// -------------------------------------------------------------------------------------------------------------------------- 

void coordinates_adaption(struct Tree *trees, double *x, double *y, int size_trees_array) {
    // Find the minimum x coordinate and the minimum y coordinate -> coordinates of the tree at the most southwestern point  
    double min_x = min(x, size_trees_array);
    double min_y = min(y, size_trees_array);
    
    // Allocate memory for the converted x and y coordinates
    double *trees_x_converted = malloc(size_trees_array * sizeof(double));
    double *trees_y_converted = malloc(size_trees_array * sizeof(double));
    
    for (int i = 0; i < size_trees_array; i++) {
        // Shift the coordinates
        trees_x_converted[i] = (x[i] - min_x); 
        trees_y_converted[i] = (y[i] - min_y);
        
        // Add the adjusted coordinates to the Tree structure
        trees[i].position_x_grid = trees_x_converted[i];
        trees[i].position_y_grid = trees_y_converted[i];
    }

    // Free the allocated memory
    free(trees_x_converted);
    free(trees_y_converted);
}

// ---------------------------------------------------------------------------------------------------------------------------------
// distance: This function finds the point with the minimum x and y coordinates and the point with the maximum x and y coordinates.
//           It then calculates the distance between these two points and divides it by the size of the grid. We use gridsize = 100
//           to distribute the trees over square fields of 100 x 100 m. The distance is given in meters since we are using the LV95 
//           coordinate system. 
// ---------------------------------------------------------------------------------------------------------------------------------

int distance(double *points, int size_trees_array, int gridsize) {
    // Find the point with the minimum coordinates and the point with the maximum coordinates
    double min_p = min(points, size_trees_array);
    double max_p = max(points, size_trees_array);
    
    // Calculate the distance between these points and divide it by gridsize
    return (int)ceil((max_p - min_p)/gridsize);
}


//-------------calculations-----------------------------

// calculations: This function calculates all the properties necessary to find the PM10 deposition, the OFP and the O3 removal. 
//               It applies the formulas of the model to all the trees present in a 100 x 100 m square field and adds the sum of 
//               the results to the corresponding grid cell.
// Inputs: Size of tree array, distances of grid in x and y direction, previously allocated grids, tree array, pollutant concentrations, size of gridcells
// Ouputs: None


void calculations(int size_trees_array_filtered_trees, int length_y,int length_x, double** grid_OFP, double** grid_PM10, double** grid_O3, double** grid_O3_net_uptake, struct Tree *trees, double conc_PM10_city, double conc_O3_city, int gridsize){
    for(int i = 0; i < length_y; i++){
        for(int j = 0; j<length_x; j++){
            double OFP_tot = 0.0;
            double PM10_tot = 0.0;
            double O3_uptake_tot = 0.0;
            double O3_net_uptake_tot = 0.0;

            for(int k = 0; k<size_trees_array_filtered_trees; k++){
                if((trees[k].position_x_grid <=j*gridsize+gridsize && trees[k].position_x_grid >=j*gridsize) &&(trees[k].position_y_grid >=i*gridsize && trees[k].position_y_grid <= i*gridsize+ gridsize)){ //tree needs to be within the grid cell that we are currently analyzing
                    // Apply the functions defined in a_model_functions.c to each tree present in a square field
                    leaf_area_func(&trees[k]);
                    leaf_dry_weight_func(&trees[k]);
                    OFP_hourly_func(&trees[k]);
                    OFP_yearly_func(&trees[k]);
                    OFP_tot += trees[k].OFP_yearly;
                    PM10_yearly_func(&trees[k], conc_PM10_city);
                    PM10_tot += trees[k].PM10_yearly;
                    O3_instantaneous_func(&trees[k], conc_O3_city);
                    O3_yearly_func(&trees[k]);
                    O3_removal_yearly_func(&trees[k]);
                    O3_removed_mass_yearly_func(&trees[k]);
                    O3_uptake_tot += trees[k].O3_removed_mass_yearly;
                    O3_net_uptake_yearly_func(&trees[k]);
                    O3_net_uptake_tot += trees[k].O3_net_uptake_yearly;
                }
            }
            
            // Add the values to the fields in the grid and convert them to kg/y
            grid_OFP[i][j] = OFP_tot * pow(10, -9);
            grid_PM10[i][j] = PM10_tot;
            grid_O3[i][j] = O3_uptake_tot * pow(10,-3);
            grid_O3_net_uptake[i][j] = O3_net_uptake_tot * pow(10,-3);
        }
    }
}

