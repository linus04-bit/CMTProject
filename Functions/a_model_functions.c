#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846 
#define MW_O3 47.997                       // molar weight of O3 in g/mol
#define MOLAR_VOlUME 0.02445               // molar volume of air at STP in m³/mol
#define P 12                               // photoperiod: approximation of 12 h/day
#define Vd 0.0064                          // dry deposition velocity of PM10 in m/s
#define DIFF_RATIO 0.613                   // diffusibility ratio between ozone and water vapor
#define PART_SUSP_RATE 0.5                 // particle resuspension rate back to the atmosphere 
#define STOMATAL_O3_FLUX 0.3               // a stomatal flux of 30% of total potential O3 was considered 


//-------------------------------------------------------------------------------------------------------------------
// This file contains the definition of the Tree structure, as well as all functions related to computing the fields.
//-------------------------------------------------------------------------------------------------------------------

// ------------------------------
// Structure for a single tree : 
// ------------------------------ 

struct Tree {
    char * species_name;
    double crown_height;          // m, total_height - trunc_height
    double crown_diameter;        // m  
    double position_y;            // coordinates
    double position_x;            // coordinates
    double position_y_grid;
    double position_x_grid;

    // From literature review :
    double shading_factor;           // no units, %
    double conversion_factor;        // g/m2
    int leaves_days;                 // number of days -> 365 if evergreen, 183 if deciduous
    double stomatal_conductance;     // mmol(water vapor)/m2/s ; species-specific stomatal conductance to water vapor 

    double mass_emission_factor[3];  // ug(VOC)/gdw/h ; VOC = Volatile Organic Compound ; gdw: grams of leaf dry weight 
                                     // mass_emission_factor[0] : isoprene
                                     // mass_emission_factor[1] : monoterpenes
                                     // mass_emission_factor[2] : sesquiterpenes
    
    double max_incremental_reactivity[3];  // g(O3)/g(VOC)     
                                           // max_incremental_reactivity[0] : isoprene
                                           // max_incremental_reactivity[1] : monoterpenes
                                           // max_incremental_reactivity[2] : sesquiterpenes
    
    // Calculated during the execution in main() :
    double leaf_area;                // m2
    double leaf_dry_weight;          // g
    double OFP_hourly;               // ug(O3)/h
    double OFP_yearly;               // ug(O3)/y
    double PM10_yearly;              // ug(PM10)/y
    double O3_instantaneous;         // (nmol(O3)/m2/s) 
    double O3_yearly;                // mol(O3)/m2/y
    double O3_removal_yearly;        // mol(O3)/m2/y
    double O3_removed_mass_yearly;   // g(O3)/y
    double O3_net_uptake_yearly;     // g(O3)/y
};

// ---------------------------------
// Functions defined in this file : 
// ---------------------------------

// leaf_area_func
// leaf_dry_weight_func       
// OFP_hourly_func            
// OFP_yearly_func           
// PM10_yearly_func            
// O3_instantaneous_func     
// O3_yearly_func               
// O3_removal_yearly_func    
// O3_removed_mass_yearly_func
// O3_net_uptake_yearly_func 

// -------------------------------- 
// Leaf area of a single tree : LA
// --------------------------------

void leaf_area_func(struct Tree * tree) {
    // Parameters 
    double H = tree->crown_height;
    double D = tree->crown_diameter;
    double S = tree->shading_factor;
    double C = PI * D * (H + D) / 2;   // C is based on the outer surface area of the tree crown.
    
    // Calculation of the leaf area 
    double ln_LA = -4.33 + 0.29 * H + 0.73 * D + 5.72 * S - 0.01 * C;   // regression equation
    double LA = exp(ln_LA);

    // Adding the calculated leaf area to the tree structure 
    tree->leaf_area = LA;
};

// ------------------------------------
// Leaf biomass / leaf dry weight : LW
// ------------------------------------

void leaf_dry_weight_func(struct Tree * tree) {
    // Parameters 
    double LA = tree->leaf_area;
    double f = tree->conversion_factor;
    
    // Calculation of the leaf dry weight 
    double LW = LA * f;

    // Adding the calculated leaf dry weight to the tree structure 
    tree->leaf_dry_weight = LW;
};

// -------------------------------------------------------------- 
// Ozone-forming potential (for one tree, per hour) : OFP_hourly
// --------------------------------------------------------------

void OFP_hourly_func(struct Tree * tree) {
    // Parameters and calculation of the hourly ozone-forming potential
    double LW = tree->leaf_dry_weight;          
    double EF[3];
    double MIR[3];
    double sum = 0.0;

    for (int i=0; i<3; i++) {
        EF[i] = tree->mass_emission_factor[i];
        MIR[i] = tree->max_incremental_reactivity[i];
        sum += EF[i] * MIR[i];
    };

    double OFP_hourly = LW * sum;
    
    // Adding the calculated hourly ozone-forming potential to the tree structure
    tree->OFP_hourly = OFP_hourly;
};

// ---------------------------------------------------------------------
// Annual ozone-forming potential (for one tree, per year) : OFP_yearly
// ---------------------------------------------------------------------

void OFP_yearly_func(struct Tree * tree) {
    // Parameters and calculation of the yearly ozone-forming potential
    double OFP_hourly = tree->OFP_hourly;
    int leaves_days = tree->leaves_days;
    double OFP_yearly = OFP_hourly * leaves_days * 24; 

    // Adding the calculated yearly ozone-forming potential to the tree structure
    tree->OFP_yearly = OFP_yearly;
};

// --------------------------------------------------------------------------------
// Estimation of the yearly PM10 deposition (for one tree, per year) : PM10_yearly
// --------------------------------------------------------------------------------

void PM10_yearly_func(struct Tree * tree, double conc_PM10_city) {
    // Parameters
    double conc_PM10 = conc_PM10_city;     // ug(PM10)/m3   mean yearly PM10 concentration
    double LA = tree->leaf_area;           // m2            total leaf area of a tree   
    int leaves_days = tree->leaves_days;   
    
    // Calculation of the estimation of the yearly PM10 deposition
    double PM10_yearly = Vd * conc_PM10 * LA * leaves_days * 24 * 3600 * PART_SUSP_RATE * pow(10, -9);
    
    // Adding the calculated estimation of the yearly PM10 deposition to the tree structure
    tree->PM10_yearly = PM10_yearly;
};

// -----------------------------------------------------------------------------
// Instantaneous stomatal O3 flux (for one tree, per second) : O3_instantaneous
// -----------------------------------------------------------------------------

void O3_instantaneous_func(struct Tree * tree, double conc_O3_city) {
    // Parameters 
    double stomatal_conductance = tree->stomatal_conductance;       
    double conc_O3 = conc_O3_city * pow(10, -6);

    // conc_O3_city is given in ug(O3)/m3. conc_O3 converts this value to g(O3)/m3.
    // The following calculation expects an O3 concentration in ppb or nmol(O3)/mol(air).
    // A conversion is necessary:

    double ppb_O3 = pow(10, 9) * (conc_O3 * MOLAR_VOlUME) / MW_O3;     // ppb, nmol(O3)/mol(air)

    // Calculation of the instantaneous stomatal O3 flux
    double O3_instantaneous = stomatal_conductance* pow(10, -3) * ppb_O3 * DIFF_RATIO;

    // Adding the calculated instantaneous stomatal O3 flux to the tree structure
    tree->O3_instantaneous = O3_instantaneous;
};

// --------------------------------------------------------------------
// Total annual cumulated O3 flux (for one tree, per year) : O3_yearly
// --------------------------------------------------------------------

void O3_yearly_func(struct Tree * tree) {
    // Parameters 
    double O3_instantaneous = tree->O3_instantaneous;
    int leaves_days = tree->leaves_days;
    
    // Calculation of the total annual cumulated O3 flux
    double O3_yearly = O3_instantaneous* P * leaves_days * 3600 * pow(10, -9);

    // Adding the calculated total annual cumulated O3 flux to the tree structure
    tree->O3_yearly = O3_yearly;
};

// ------------------------------------------------------------------------
// Total potential O3 removal (for one tree, per year) : O3_removal_yearly
// ------------------------------------------------------------------------

// We assumed that the stomatal O3 flux corresponds to 30% of the total potential O3 removal.

void O3_removal_yearly_func(struct Tree * tree) {
    // Parameters
    double O3_yearly = tree->O3_yearly;

    // Calculation of the total potential O3 removal
    double O3_removal_yearly = O3_yearly / STOMATAL_O3_FLUX;

    // Adding the calculated total potential O3 removal to the tree structure 
    tree->O3_removal_yearly = O3_removal_yearly;
};

// ----------------------------------------------------------------------------
// Annual mass of removed O3 (for one tree, per year) : O3_removed_mass_yearly
// ----------------------------------------------------------------------------

void O3_removed_mass_yearly_func(struct Tree * tree) {
    // Parameters 
    double O3_removal_yearly = tree->O3_removal_yearly;
    double leaf_area = tree->leaf_area;

    // Calculation of the annual mass of removed O3
    double O3_removed_mass_yearly = O3_removal_yearly * leaf_area * MW_O3;

    // Adding the calculated annual mass of removed O3 to the tree structure
    tree->O3_removed_mass_yearly = O3_removed_mass_yearly;
};

// -----------------------------------------------------------------
// Net ozone uptake (for one tree, per year) : O3_net_uptake_yearly
// -----------------------------------------------------------------

void O3_net_uptake_yearly_func(struct Tree * tree) {
    // Paramaters
    double O3_removed_mass_yearly = tree->O3_removed_mass_yearly;    // g(O3)/y
    double OFP_yearly = tree->OFP_yearly;                            // ug(O3)/y      

    // Calculation of the yearly net ozone uptake
    double O3_net_uptake_yearly = O3_removed_mass_yearly - OFP_yearly * pow(10, -6);   // conversion factor due to the different units

    // Adding the calculated yearly net ozone uptake to the tree structure
    tree->O3_net_uptake_yearly = O3_net_uptake_yearly; 
};


