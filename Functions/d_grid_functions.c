#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "b_extract_data_and_memory.c"


//--------------------------------------------------------------------------------------
// This file includes the functions related to the creation of the grid.
//--------------------------------------------------------------------------------------

//-----------------find max----------------------------
// This function finds the max within a list of doubles
double max(double * list, int size_trees_array){
    double max = 0.0;
    for(int i =0; i<size_trees_array; i++){
        if(list[i]>max) max = list[i];
    }
    return max;
}


//-------------------find min---------------------------
// This function finds the min within a list of doubles
double min(double * list, int size_trees_array){
    double min = *list;
    for(int i =0; i<size_trees_array; i++){
        if (list[i]<min) min = list[i];
    }
    return min;
}


//------------extract coordinates---------------------
// This function writes the coordinates within the structures to a list of doubles
void extract_coordinates(struct Tree * trees, double * x, double * y, int size_trees_array){
    for(int i = 0; i<size_trees_array; i ++){
        x[i] = trees[i].position_x;
        y[i] = trees[i].position_y;
    }
}


//------------adjusted coordinates to structures---------
// In this function we add an additional field to the structures
// We shift the trees such that the origin of our coordinate system is at
// the tree at the most west and south position.

void coordinates_adaption(struct Tree *trees, double * x, double* y, int size_trees_array){
    double min_x = min(x, size_trees_array);
    double min_y = min(y, size_trees_array);
    //allocate memory
    double* trees_x_converted = malloc(size_trees_array*sizeof(double));
    double* trees_y_converted = malloc(size_trees_array*sizeof(double));
    for (int i =0; i<size_trees_array; i++){
        //shift coordinates
        trees_x_converted[i] = (x[i]-min_x); 
        trees_y_converted[i] = (y[i] - min_y);
        //add adjusted coordinates to tree structure
        trees[i].position_x_grid = trees_x_converted[i];
        trees[i].position_y_grid = trees_y_converted[i];
    }
    free(trees_x_converted);
    free(trees_y_converted);
}
//-------------calculate length array--------------
//Calculates the distance between the min and max of a list
// divided by our gridsize. We use gridsize = 100, to normalize our trees
// over fields of squares of 100x100m
int distance(double* points, int size_trees_array, int gridsize){
    double min_p = min(points, size_trees_array);
    double max_p = max(points, size_trees_array);
    return (int)ceil((max_p - min_p)/gridsize);
}
// This distance is given in meters, since we're using the LV95 coordinate system

//-------------calculations-----------------------------
// This function calculates the necessary properties within 
// a 100x100 square and writes that value to the corresponding 
// element of our grid



void calculations(int size_trees_array_filtered_trees, int length_y,int length_x, double** grid_OFP, double** grid_PM10, double** grid_O3, double** grid_O3_net_uptake, struct Tree *trees, double Ci_City, double conc_O3_city, int gridsize){
    for(int i = 0; i < length_y; i++){
        for(int j = 0; j<length_x; j++){
            double OFP_tot = 0.0;
            double PM_tot = 0.0;
            double O3_uptake_tot = 0.0;
            double O3_net_uptake_tot = 0.0;
            for(int k = 0; k<size_trees_array_filtered_trees; k++){
                if((trees[k].position_x_grid <=j*gridsize+gridsize && trees[k].position_x_grid >=j*gridsize) &&(trees[k].position_y_grid >=i*gridsize && trees[k].position_y_grid <= i*gridsize+ gridsize)){
                    // functions are defined in a_model_functions.c
                    leaf_area_func(&trees[k]);
                    leaf_dry_weight_func(&trees[k]);
                    OFP_hourly_func(&trees[k]);
                    OFP_yearly_func(&trees[k]);
                    OFP_tot += trees[k].OFP_yearly;
                    PM10_yearly_func(&trees[k], Ci_City);
                    PM_tot += trees[k].PM10_yearly;
                    O3_instantaneous_func(&trees[k], conc_O3_city);
                    O3_yearly_func(&trees[k]);
                    O3_removal_yearly_func(&trees[k]);
                    O3_removed_mass_yearly_func(&trees[k]);
                    O3_uptake_tot += trees[k].O3_removed_mass_yearly;
                    O3_net_uptake_yearly_func(&trees[k]);
                    O3_net_uptake_tot += trees[k].O3_net_uptake_yearly;
                }
            }
            // add values to field in grid and conversion to kg/y
            grid_OFP[i][j] = OFP_tot*pow(10, -9);
            grid_PM10[i][j] = PM_tot;
            grid_O3[i][j] = O3_uptake_tot *pow(10,-3);
            grid_O3_net_uptake[i][j] = O3_net_uptake_tot*pow(10,-3);
    }
    }
}

