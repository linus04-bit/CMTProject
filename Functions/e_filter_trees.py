import numpy as np
import math
from Functions.h_structures import *




def readfile(file_name):
    names_values_dic = {}

    with open(file_name, "r") as file:
        for line in file:
            data = line.split(";")

            full_name = data[0]
            first_name = full_name.split()[0]
            names_values_dic[first_name] = []
            n = len(data[1:])
            if n == 1:
                value = float(data[1])
                names_values_dic[first_name].append(value)#takes a lot of data time

            elif n == 3:  
                v = []                                                 # pour EF -> on a trois valeurs
                for i in range(1, 4):
                    if data[i] == "None" or data[i] == "None\n":       # pour traîter les cas où on a None -> peut-être à modifier plus tard ??
                        data[i] = 0.0
                    v.append(float(data[i]))
                names_values_dic[first_name].append(v)
        
        for key in names_values_dic.keys():
            nbr_values = len(names_values_dic[key])
            if type(names_values_dic[key][0]) == float:
                sum_k = sum(names_values_dic[key])
                names_values_dic[key] = sum_k / nbr_values

            
            elif type(names_values_dic[key][0]) == list:
                mean_values = [0, 0, 0]
                for i in range(3):
                    v = 0

                    for j in range(nbr_values):
                        v += names_values_dic[key][j][i]

                    mean_values[i] = v / nbr_values
                
                names_values_dic[key] = mean_values # normalement on devrait maintenant avoir une seule liste avec 3 valeurs
  
    return names_values_dic     
#Can we work with multi threading?
def filter_trees(trees, filtered_trees, conversion_factor, EF, shading_coeff, MIR):
    dic_conversion_factor = readfile(conversion_factor)
    dic_EF = readfile(EF)
    dic_shading_coefficient = readfile(shading_coeff) 
    dic_MIR = readfile(MIR)

    names1 = list(dic_conversion_factor.keys())
    names2 = list(dic_EF.keys())
    names3 = list(dic_shading_coefficient.keys())
    counts = 0
    hits = 0
    for i in range(len(trees)):
        full_species_name = str(trees[i]['species_name'])
        first_species_name = full_species_name.split()[0]
        counts+=1
        print(counts)
        if first_species_name in names1 and first_species_name in names2 and first_species_name in names3:
            filtered_trees = np.append(filtered_trees, trees[i])
            index1 = names1.index(first_species_name)
            index2 = names2.index(first_species_name)
            index3 = names3.index(first_species_name)
            filtered_trees[hits]['conversion_factor'] = dic_conversion_factor[first_species_name]
            filtered_trees[hits]['mass_emission_factor'] = dic_EF[first_species_name]
            filtered_trees[hits]['shading_factor'] = dic_shading_coefficient[first_species_name]
            filtered_trees[hits]['max_incremental_reactivity'][0] = dic_MIR['isoprene']
            filtered_trees[hits]['max_incremental_reactivity'][1] = dic_MIR['monoterpenes']
            filtered_trees[hits]['max_incremental_reactivity'][2] = dic_MIR['sesquiterpenes']
            hits+=1
        
    return filtered_trees