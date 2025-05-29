#include <stddef.h>

#ifndef RANDOM_FAULT
#define RANDOM_FAULT

// Fault definition
typedef struct {
    double posX;
    double posY;
    double posZ;
    double dip;
    double dipdir;
    double slip;
    double pitch;
    double Xaxis;
    double Yaxis;
    double Zaxis;

} RandomFault;

typedef struct {
    double family_dip;
    double family_dipdir;
    size_t faults_number;
    RandomFault *faults;
} FaultFamily;

typedef struct {
    int total_fault;
    size_t family_number;
    FaultFamily *families;
} FaultManager;

#endif