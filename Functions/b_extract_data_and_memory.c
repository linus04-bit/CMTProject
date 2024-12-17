#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "a_model_functions.c" 


#define PI 3.14159265358979323846 
#define MAX_Line_Length 1024
#define LEAVE_DAYS_EVERGREENS 365 // Value taken from Kofel, Donato, et al.
#define LEAVE_DAYS_BROADLEAVES 183 // Value taken from Kofel, Donato, et al.
#define STOMATAL_COND_EVERGREENS 16.896 //Value taken from Zeppel et al.
#define STOMATAL_COND_BROADLEAVES 72.637 // Value taken from Zeppel et al.


//-----------------------------------------------------------------------------------------------------------------------------
// This file contains the functions to read the csv document with the trees in Geneva, as well as the functions to allocate and free memory.
//--------------------------------------------------------------------------------------------------------------------------------




//------------for memory allocation of str into structure------------------------
// This functions is used within the readandwrite Document function to allocate memory
// for strings

char* my_strndup(const char* str, size_t n) {
    // Allocate memory for the substring + 1 for null terminator
    char *dup = malloc(n + 1);
    if (dup == NULL) {
        return NULL;  // Return NULL if malloc fails
    }
    // Copy the substring
    memcpy(dup, str, n);
    dup[n] = '\0';  // Null-terminate the copied string
    return dup;
}

//-------------------------------------------------------------------------------
//This function opens the file and saves it to a predefined list of structures. 
// size_org is length of tree array and hence the amount of values we read in from our file
int readwriteDocument(char* filename, struct Tree *trees, int size_org){
    // open csv file
   FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        exit(-1);
    }
    char* data;
    int index = 0;
    printf("File opening worked\n");
    static char line[MAX_Line_Length];
    // read in first line
    fgets(line, sizeof(line), file);
    //read in the rest and parse it
    while(index<=size_org){
        fgets(line, sizeof(line), file);
        line[strcspn(line, "\n")] = '\0';
        char * substring = strstr(line, ";");// will send me a last_substringer to the first ;
        int position =0;
        int last_position = 0;
        int matches = 1;
        // as long as a substring is found, this while will turn
        while(substring){
            last_position = position;
            position = substring - line;
            char* substring_saved = substring;
            substring = strstr(substring+1, ";");
            //species name
            if(matches == 2){
                trees[index].species_name = my_strndup(line+last_position + 1, position - last_position-1);
            }
            // crown height
            double trunk_height = 0.0;
            if(matches == 9){
                trunk_height = atof(my_strndup(line+last_position + 1, position - last_position-1) );
            }
            if(matches == 10){
                double total_height = atof(my_strndup(line+last_position + 1, position - last_position-1));
                trees[index].crown_height = total_height - trunk_height;
            }
            //crown diameter
            if(matches == 11){
                double value = atof(my_strndup(line+last_position + 1, position - last_position-1));
            }
            //Adding attributes depending on if its a evergreen or broadleaf. Values found in table and Kofel
            if(matches == 23){
                char* type = my_strndup(line+last_position + 1, position - last_position-1);

                if(strcmp(type,"Feuillus") == 0){
                    trees[index].leaves_days = LEAVE_DAYS_BROADLEAVES;
                    trees[index].stomatal_conductance = STOMATAL_COND_BROADLEAVES; 
                }
                if(strcmp(type,"Conifères") == 0){
                    trees[index].leaves_days = LEAVE_DAYS_EVERGREENS;
                    trees[index].stomatal_conductance = STOMATAL_COND_EVERGREENS;
                }
            }
            // postition, coordinate system used is LV95
            if(matches == 29){
                trees[index].position_x = atof(my_strndup(line+last_position + 1, position - last_position-1));
                trees[index].position_y = atof(my_strndup(line+position+ 1, strlen(line) - position));
            }
            matches +=1;
        }
        index += 1;
    }
    fclose(file);
    //calcualte average for trees that don't have a measured value:
    int count_h = 0;
    int count_d = 0;
    double sum_h = 0.0;
    double sum_d = 0.0;
    for(int i=0; i<size_org; i++){
        if(trees[i].crown_height != 0.0){
            count_h += 1;
            sum_h += trees[i].crown_height;
        }
        if(trees[i].crown_diameter != 0.0){
            count_d += 1;
            sum_d += trees[i].crown_diameter;
        }
    }
    double average_height = sum_h/count_h;
    double average_crown_diameter = sum_d/count_d;
    for(int i=0; i<size_org; i++){
        if(trees[i].crown_height==0.0){
            trees[i].crown_height = average_height;
        }
        if(trees[i].crown_diameter==0.0){
            trees[i].crown_diameter = average_crown_diameter;
        }
    }
    //writes content to csv file --> not necessary but helps to check for errors
    FILE *file_out = fopen("Results/trees_GE.csv", "w"); // Open the file in write mode

    // Check if the file was opened successfully
    if (file_out == NULL) {
        printf("Error opening file!\n");
        return 1; // Return error code
    }

    // Write the header row
    fprintf(file_out, "Tree name; Crown Height; Crown Diameter, Position X, Position Y, Leave Days\n");

     for(int i = 0; i < size_org; i++){
        fprintf(file_out, "%s ; %f ; %f ; %f ; %f ; %d \n ", trees[i].species_name, trees[i].crown_height, trees[i].crown_diameter, trees[i].position_x, trees[i].position_y, trees[i].leaves_days);
    }
    // Close the file
    fclose(file_out);
    printf("Data written to 'trees_GE.txt' successfully!\n");
}
//------------------------------------------------------


//------memory allocation-------------------------------
//for simple array
double* getarray(int size){
    double* arr = malloc(size * sizeof(double));
    return arr;
}
// 2D array used for grid
double ** get_gridarray(int rows, int columns){
    //memory allocation for rows
    double **grid = malloc(rows * sizeof(double *));
    if (grid == NULL) {
        printf("Memory allocation failed for grid rows\n");
        return NULL;
    }
    //memory allocation for columns
    for (int i = 0; i < rows; i++) {
        grid[i] = (double *)malloc(columns * sizeof(double));
        if (grid[i] == NULL) {
            printf("Memory allocation failed for grid columns\n");
            return NULL;
        }
    }
    // initialize the grid to zero
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<columns; j++){
            grid[i][j] = 0.0;
        }
    }
    return grid;
}
//for an array containing tree structures, initializing the value to zero
struct Tree* gettreearray(int size){
    struct Tree * trees = malloc(size *sizeof(struct Tree));
    for(int i = 0; i<size; i++){
        struct Tree *tree = &trees[i];
        tree->crown_height = 0.0;
        tree->crown_diameter = 0.0;
        tree->position_y = 0.0;
        tree->position_x = 0.0;
        tree->position_y_grid = 0;
        tree->position_x_grid = 0;
        tree->shading_factor = 0.0;
        tree->conversion_factor = 0.0;
        tree->leaves_days = 0;
        tree->stomatal_conductance = 0.0;
        for (int i = 0; i < 3; i++) {
            tree->mass_emission_factor[i] = 0.0;
            tree->max_incremental_reactivity[i] = 0.0;
        }
        tree->leaf_area = 0.0;
        tree->leaf_dry_weight = 0.0;
        tree->OFP_hourly = 0.0;
        tree -> OFP_yearly = 0.0;
        tree->PM10_yearly = 0.0;
        tree->O3_instantaneous = 0.0;
        tree->O3_yearly = 0.0;
        tree->O3_removal_yearly = 0.0;
        tree->O3_removed_mass_yearly = 0.0;
        tree->O3_net_uptake_yearly = 0.0;
    }
    return trees;
}
//for a the name of the species
void memory_allocation(struct Tree *tree){
    tree->species_name = NULL;
}
//-------------------------------------------------------


//-------------free memory-------------------------------
// free an array
void freearray(double* arr){
    free(arr);
}
//free memory of the name of the species
void free_memory(struct Tree *tree){
    free(tree->species_name);
}
// free the grid
void free_grid(double** grid){
    free(grid);
}
//free an array of tree structures
void free_tree_array(struct Tree *tree){
    free(tree);
}
//-------------------------------------------------------
