# Computational Methods and Tools - Project: Ozone and PM10 absorbtion by Trees in Canton of Geneva


## Project Description

Given a certain ozone and PM10 concentration, this program displays graphically the yearly uptake of PM10, ozone-forming potential (OFP), removal of ozone, and net O3 uptake by the trees located within the canton of Geneva. The net uptake of ozone describes the difference between the ozone removal and the OFP.

This program will:
1. Read in data concerning the location, as well as certain properties of the trees located within the canton of Geneva. This data is provided by *Le Système d’Information du Territoire à Genève (SITG)*.
2. Filter through that data and add scientific parameters from other files necessary for the following calculations. Tree genera for which no data was found were discarded.
3. Compute the yearly PM10 uptake, ozone-forming potential, ozone removal, and net ozone uptake, aggregated over 100 x 100 m grid cells.
4. Calculate the total PM10 uptake, OFP, ozone removal and net ozone uptake for the whole grid corresponding to the canton.
5. Display the found results graphically and save them as .png files.


## Project Structure

- "*Data*" contains different input files
- "*Functions*" contains the necessary code in C and Python, as well as the shared library between C and Python, once compiled. The files in this folder should not be modified.
- "*Results*" contains the created graphs, a CSV file containing the data taken from the first input file as a control tool, and a summary file with the most important information, as well as the total values of PM10, OFP, ozone removal and net ozone uptake.
- "*execution_file_windows.py*" or "*execution_file_mac.py*" is the Python file that can be modified and needs to be run in order to read in the data, compute and display the results. Depending on the operating system, a different file needs to be used (*execution_file_windows.py* for windows,  *execution_file_mac.py* for MacOS and Linux).


## Inputs and Outputs

Inputs:
- "*Data/SIPV_ICA_ARBRE_ISOLE.csv*" is a semicolon-delimited file.
- "*Data/conversion_factor.csv*" is a semicolon-delimited file.
- "*Data/EF.csv*" is a semicolon-delimited file.
- "*Data/MIR.csv*" is a semicolon-delimited file.
- "*Data/shading_coeff.csv.*" is a semicolon-delimited file.

Outputs: 
- "*Results/OFP_{map_amount_of_trees}_indices.png*" is a figure depicting the yearly ozone-forming potential distribution within the canton depending on the grid indices. 
- "*Results/PM10_{map_amount_of_trees}_indices.png*" is a figure depicting the yearly PM10 uptake distribution within the canton depending on the grid indices.
- "*Results/O3_map_{amount_of_trees}_indices.png*" is a figure depicting the yearly ozone removal distribution within the canton depending on the grid indices.
- "*Results/O3_net_uptake_map_{amount_of_trees}_indices.png*" is a figure depicting the yearly net ozone uptake distribution within the canton depending on the grid indices.
- "*Results/summary.txt*" contains a summary of the parameters used, along with the total and maximal values computed. It also includes the computation time of the program.
- "*Results/trees_GE.csv*" is a semicolon-delimited file. It contains the initial information regarding the trees within the scope of our analysis. It serves as a control file and can be deleted once used.


## Implementation Details
- Python calls a C function reading the tree data provided by the canton of Geneva and saving the data as an array in Python.
- Python filters the array, creating a new one and adding certain parameters.
- Python calls a C function that computes the necessary values.
- Python takes care of the visualization.

**Structure**

In the directory "*Functions/*" are located:
- "*a_model_functions.c*"
    - Contains a Tree structure used throughout the computation.
    - Contains the necessary functions used for the later computation of modeled values.
- "*b_extract_data_and_memory.c*":
    - Imports "*a_model_functions*" as a module.
    - Contains a function that reads the data and saves it to an array, as well as all necessary functions regarding memory allocation and liberation.
- "*d_grid_functions.c*":
    - Imports "*b_extract_data_and_memory.c*" as a module.
    - Contains functions computing the data for the final grid cells.
- "*e_filter_trees.py*":
    - Reads in multiple CSV files such as "*Data/conversion_factor.csv*", "*EF.csv*", "*MIR.csv*" and "*shading_coeff.csv*". 
    - Contains functions that filter the initial array of trees and create a second array containing only those trees for which the necessary scientific data is available, while adding that data to the array."
    - Contains functions to transform a c_POINTER(c_POINTER(double)) to a two-dimensional np.array.
- "*h_structures.py*":
    - Contains structures used in Python.
- "*main.c*":
    - Imports "*d_grid_functions.c*" as a module.
    - File that is used to create the shared library between Python and C.
    - Contains two functions that are called from Python. Performs all necessary computations by calling functions defined in mentioned files.
    - Writes data from "*SIPV_ICA_ARBRE_ISOLE.csv*" to "*Results/trees_GE.txt*".

In the main directory are located:
- "*execution_file_windows.py*": 
    - Main execution file for Windows.
    - Calls "*Functions/main.c*" as a module using a shared library. 
    - Implements functions written in C and calls them from Python.
    - Contains parameters that can be changed as wished.
    - Writes a summary to "*Results/summary.txt*".
    - Plots results and saves graphs to "*Results/*".
- "*execution_file_mac.py*": 
    - Main execution file for MacOS.
    - Performs same tasks as "*execution_file_windows.py*".

## Instructions

To reproduce the results depicted in the report, the following steps should be implemented:
1. Open the terminal from the project root directory and compile the shared C library using one of the following commands:

    For Windows: 
    ```
    gcc -shared -o Functions/main.dll Functions/main.c

    ```
    For MacOS: 
    ```
    gcc -shared -o Functions/main.so Functions/main.c

    ```

2. Open the file "*execution_file_windows.py*" (Windows) or "*execution_file_mac.py*" (MacOS). The file contains commented instructions for the user to modify certain input parameters. Modify as desired and save the file. The values by default correspond to the values used in the report.

3. The file mentioned above can now be run. It will automatically perform all calculations and save the plotted graphs to the directory "*Results*". For 120'000 trees, the program will take approximately 32 minutes:

   For Windows:

    ```
    python3 execution_file_windows.py

    ```
    For MacOS:

    ```
    python3 execution_file_mac.py

    ```

## Requirements

Versions of Python and C used are as follows:
```
python --version
Python 3.12.8

gcc --version
gcc.exe (Rev3, Built by MSYS2 project) 14.1.0
```

### Utilized python libraries:
The libraries used in this project can be found in the "*requirements.txt*" file.
This file was created using the following command:

```
pip freeze > requirements.txt
```

and deleting all but the necessary packages.


## Credits

## Data
The data file "*SIPV_ICA_ARBRE_ISOLE.csv*" comes from [Le Territoire Genevois à la Carte](https://ge.ch/sitg/sitg_catalog/sitg_donnees?keyword=&geodataid=4571&topic=tous&service=tous&datatype=tous&distribution=tous&sort=auto#). 

The data file "*conversion_factor.csv*" was created using data from [US Department of Agriculture, Appendix 4](https://www.fs.usda.gov/nrs/pubs/gtr/gtr-nrs200-2021_appendixes/gtr_nrs200-2021_appendix4.pdf).

The data files "*EF.csv*" and "*MIR.csv*" were created using data from [Kofel, Donato, et al."*Quantifying the impact of urban trees on air quality in Geneva, Switzerland*", EPFL 2023](https://infoscience.epfl.ch/entities/publication/40973cec-92bd-4171-b671-817c28a88f64).

The data file "*shading_coeff.csv*" was created using data from [US Departement of Agriculture, Appendix 3](https://www.fs.usda.gov/nrs/pubs/gtr/gtr-nrs200-2021_appendixes/gtr_nrs200-2021_appendix3.pdf).

The concentrations used as default values in "*execution_file_windows.py*" are from [Canton of Geneva](https://www.ge.ch/connaitre-qualite-air-geneve/donnees-qualite-air-recherches-personnalisees).

The values used for stomatal conductance come from [Zeppel, Melanie, et al."*Representing nighttime and minimum conductance in CLM4.5: global hydrology and carbon sensitivity analysis using observational constraints*", 2017](https://www.researchgate.net/publication/312639755/figure/tbl1/AS:668529984544776@1536401377779/Old-and-new-minimum-stomatal-conductance-values-used-in-CLM45SP-Units-are-mmol-m-2-s.png) and can be found in "*Functions/b_extract_data_and_memory"* 

## Formulae
Certain variables, such as "*gridsize*" or "*leaves_days*", as well as  computations applied in this project come from [Kofel, Donato, et al."*Quantifying the impact of urban trees on air quality in Geneva, Switzerland*"EPFL 2023](https://infoscience.epfl.ch/entities/publication/40973cec-92bd-4171-b671-817c28a88f64). 

The formula used for the calculation of the net ozone uptake comes from [Manzini, Jacobo, et al. "*FlorTree: A unifying modelling framework for estimating the species-specific pollution removal by individual trees and shrubs, 2023.*"](https://www.sciencedirect.com/science/article/pii/S1618866723001383)

## Usage of AI
Artificial Intelligence such as ChatGPT(Version: GPT-4V (Vision) architecture) and Microsoft Copilot (Version: Microsoft Copilot, powered by GPT-4. (2024)) was used in this project. The main purpose was to explain certain code, such as the implementation of ctypes, and to debug the code. The function "*my_strndup*", located in "*Functions/b_extract_data_and_memory*", was entirely created by ChatGPT.

## Reflection and Outlook
Unfortunately, this program is strongly restricted by memory and time. This problem could be solved by using pandas instead of simple string comparison in the file "*/Functions/e_filter_trees.py*", allowing the program to run faster and take all of the data into account. The decision to use ctypes in our Python execution file allowed us to run other computations very fast and dynamically allocate memory. This choice is therefore justified and recommended. Moreover, one could decide to implement a computation for a two dimensional normal distribution or a latin hypercurbe in "*/Functions/b_extract_data_and_memory*" to calculate missing values for measured height and crown diameter of the trees.
