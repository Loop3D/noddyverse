#include "randomFaultsManager.h"
// #include "xoroshiro128plus.h"
#include <math.h>
#include <stddef.h>

// #ifndef M_PI
// #define M_PI 3.14159265358979323846
// #endif

extern void sample_dip_dipdir(double mean_dip, double mean_dip_dir, double dip_var, double dip_dir_var, double *new_dip, double *new_dip_dir);
extern double sample_centred_normal();

void initRandomFaultManager(FaultManager *fltsManager, xrshr128p_state_t state){
    int nbFamilies = 1+(xrshr128p_next(&state)%3); //amandine // Select a specific number of fault families. Between 1 and 3
    int nbFaults;

    fltsManager->family_number = (size_t) nbFamilies;
    fltsManager->families = malloc(fltsManager->family_number * sizeof(FaultFamily));

    if (fltsManager->families == NULL) {
        perror("Erreur d'allocation");
        return ;
    }

    int numFaults = 5;
    double D_min = 0.0;

    for (size_t i = 0; i < fltsManager->family_number; i++) {

        FaultFamily *family = &fltsManager->families[i];
        
        family->family_dip = 45 + 25 * sample_centred_normal();
        family->family_dipdir = 360.0 * xrshr128p_next_double(&state);

        D_min = 1600.0 / tan(family->family_dip * M_PI/180.0);
        numFaults = 6400 / (int)D_min;
        if (numFaults == 0){
            numFaults = 1;
        }
        else if (numFaults > 5){
            numFaults = 5;
        }

        family->faults_number = 1 + (xrshr128p_next(&state)%numFaults);
        family->faults = malloc(family->faults_number * sizeof(RandomFault));

        nbFaults += family->faults_number;

        if (family->faults == NULL) {
            perror("Erreur d'allocation");
            return ;
        }

        for (size_t j = 0; j < family->faults_number; j++){

            RandomFault *fault = &family->faults[j];

            double conjuguate = xrshr128p_next(&state)%2;

            double dip = 0.0;
            double dipdir = 0.0;
            
            // Outside the box if no position in the box can be found
            fault->posX = -5000.0;
            fault->posY = -5000.0; 
            fault->posZ = -5000.0;

            newPositionFault(&family, j, D_min + 1.0, state);

            sample_dip_dipdir(family->family_dip, family->family_dipdir, 10, 10, &dip, &dipdir);

            dipdir = (dipdir + 180.0*conjuguate);
				if (dipdir>= 360.0)
					dipdir -= 360.0;

            fault->dip = dip;
            fault->dipdir = dipdir;
            fault->pitch = 85.0 + 5.0 * xrshr128p_next_double(&state);

            fault->Zaxis = 2000.0 + 18000.0 * xrshr128p_next_double(&state); //Length
            fault->Xaxis = 0.5 * fault->Zaxis; //Heigth
            fault->Yaxis = 0.5 * fault->Zaxis; 

            double ratio = -1.88 + 1.12 * sample_centred_normal();
            fault->slip = pow(10, ratio) * fault->Zaxis;

        }
    }

    fltsManager->total_fault = nbFaults;
}

void freeRandomFaultManager(FaultManager *fltsManager){

    for (size_t i = 0; i < fltsManager->family_number; i++) {

        FaultFamily *family = &fltsManager->families[i];

        free(family->faults);
    }

    free(fltsManager->families);

}

void newPositionFault(FaultFamily *family, size_t c_fault_idx, double D_min, xrshr128p_state_t state){

    int max_attempts = 1000;

    RandomFault *fault = &family->faults[c_fault_idx];

    int attempts = 0;
    int accepted = 0;

    while (!accepted && attempts < max_attempts) {
        attempts++;

        // New Position 
        double x = 6400.0 * xrshr128p_next_double(&state);
        double y = 6400.0 * xrshr128p_next_double(&state);
        double z = 500.0 + 2700.0 * xrshr128p_next_double(&state);

        accepted = 1;
        for (size_t k = 0; k < c_fault_idx; k++) {
            RandomFault *other = &family->faults[k];

            double dx = other->posX - x;
            double dy = other->posY - y;
            double dz = other->posZ - z;
            double dist = sqrt(dx*dx + dy*dy + dz*dz);

            if (dist < D_min) {
                accepted = 0;
                break;
            }
        }

        if (accepted) {
            fault->posX = x;
            fault->posY = y;
            fault->posZ = z;

            return ;
        }
    }    
}

void randomFault(RandomFault *fault, xrshr128p_state_t state){

    fault->posX = 10000.0 * xrshr128p_next_double(&state);
    fault->posY = 10000.0 * xrshr128p_next_double(&state);
    fault->posZ = 10000.0 * xrshr128p_next_double(&state);
    fault->dip  = 90.0 * xrshr128p_next_double(&state);
    fault->dipdir = 360.0 * xrshr128p_next_double(&state);
    fault->pitch = 90.0 * xrshr128p_next_double(&state);
    fault->Zaxis = 2000.0 + 18000.0 * xrshr128p_next_double(&state);
    fault->Xaxis = 0.5 * fault->Zaxis;
    fault->Yaxis = 0.5 * fault->Zaxis;
    double ratio = -1.88 + 1.12 * sample_centred_normal();
    fault->slip = pow(10, ratio) * fault->Zaxis;

}