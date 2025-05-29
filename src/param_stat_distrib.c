#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// ------------------ Geometric Utility ------------------

// #ifndef M_PI
// #define M_PI 3.14159265358979323846
// #endif

// 3D vector representation
typedef struct {
    double x, y, z;
} Vector3D;

Vector3D cross_product(Vector3D a, Vector3D b) {
    Vector3D result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

Vector3D normalize(Vector3D v) {
    double length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    Vector3D result = {v.x / length, v.y / length, v.z / length};
    return result;
}

Vector3D random_unit_vector() {

    Vector3D rng_vector;

    double theta = 2 * M_PI * ((double) rand() / RAND_MAX);
    double u = 2.0 * ((double)rand() / RAND_MAX) - 1.0;
    double s = sqrt(1 - u * u);
    rng_vector.x = s * cos(theta);
    rng_vector.y = s * sin(theta);
    rng_vector.z = u;

    return rng_vector;
}

double deg2rad(double deg) {
    return deg * M_PI / 180.0;
}

// ------------------ Oriented Kent ------------------

// Dip and dip direction to vector
Vector3D dipdir_to_vector(double dip, double dip_dir) {
    Vector3D result;
    // Angles in radians
    double dip_rad = dip * M_PI / 180.0; // M_PI is Pi constant
    double dipdir_rad = dip_dir * M_PI / 180.0;
    
    // Unit normal vector
    result.x = sin(dip_rad) * sin(dipdir_rad); // East
    result.y = sin(dip_rad) * cos(dipdir_rad); // North
    result.z = cos(dip_rad); // Vertical
    
    return result;
}

void unit_vector_to_dip_dipdir(Vector3D v, double *dip, double *dipdir) {

    *dip = acos(fabs(v.z)) * 180.0 / M_PI;

    *dipdir = atan2(v.x, v.y) * 180.0 / M_PI;
    if (*dipdir < 0)
        *dipdir += 360.0;
}

// Compute kappa and beta from difference of dip and dip direction
void compute_kappa_beta(double dip_var_deg, double dipdir_var_deg, double *kappa, double *beta) {
    double sigma_dip = deg2rad(dip_var_deg);
    double sigma_dir = deg2rad(dipdir_var_deg);
    *kappa = 1.0 / (sigma_dip * sigma_dip);
    *beta  = 0.5 * (*kappa) * (sigma_dir*sigma_dir - sigma_dip*sigma_dip);
}

// Orthonormal basis
void build_orthonormal_basis(Vector3D g1, Vector3D *g2, Vector3D *g3) {

    Vector3D v; v.x = 1.0; v.y = 0.0; v.z = 0.0;

    // If too close of x axis, use y axis
    if (fabs((g1).x) > 0.9) {
        v.x = 0.0; v.y = 1.0; v.z = 0.0;
    }

    *g2 = cross_product(v, g1);
    *g2 = normalize(*g2);

    *g3 = cross_product(g1, *g2);
    *g3 = normalize(*g3);
}

// Rotation to global reference frame
Vector3D rotate_to_kent_frame(Vector3D v, Vector3D g1, Vector3D g2, Vector3D g3) {

    Vector3D result;
    
    // Calcul des coordonnées dans le repère de la distribution de Kent
    result.x = v.x * g1.x + v.y * g2.x + v.z * g3.x;
    result.y = v.x * g1.y + v.y * g2.y + v.z * g3.y;
    result.z = v.x * g1.z + v.y * g2.z + v.z * g3.z;
    
    return result;
}

// Kent density aligned on z axis
double kent_density(Vector3D v, double kappa, double beta) {
    double dot1 = v.z;
    double dot2 = v.x;
    double dot3 = v.y;
    return exp(kappa * dot1 + beta * (dot2 * dot2 - dot3 * dot3));
}

// Acceptance-rejection sampler for Kent centred on z axis
Vector3D sample_kent(double kappa, double beta) {

    double f, u;
    double f_max = exp(kappa + fabs(beta)); // Used to normalise

    Vector3D rng_vector;

    do {
        rng_vector = random_unit_vector();
        f = kent_density(rng_vector, kappa, beta);
        u = ((double)rand() / RAND_MAX) * f_max;
    } while (u > f);

    return rng_vector;
}


// Sampling normal distribution
double sample_centred_normal() {
    double u1 = ((double) rand() + 1.0) / ((double) RAND_MAX + 2.0);
    double u2 = ((double) rand() + 1.0) / ((double) RAND_MAX + 2.0);

    return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI *u2);
}


// Sampling a dip and a dip direction from a kent distribution
void sample_dip_dipdir(double mean_dip, double mean_dip_dir, double dip_var, double dip_dir_var, double *new_dip, double *new_dip_dir){

    Vector3D gamma1, new_orientation;
    Vector3D gamma2, gamma3;

    gamma1 = dipdir_to_vector(mean_dip, mean_dip_dir);


    build_orthonormal_basis(gamma1, &gamma2, &gamma3);

    double kappa, beta;
    compute_kappa_beta(dip_var, dip_dir_var, &kappa, &beta);

    new_orientation = sample_kent(kappa, beta);
    new_orientation = rotate_to_kent_frame(new_orientation, gamma1, gamma2, gamma3);

    unit_vector_to_dip_dipdir(new_orientation, new_dip, new_dip_dir);
}
