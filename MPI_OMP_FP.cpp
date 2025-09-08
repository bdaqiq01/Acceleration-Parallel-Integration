#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <iostream>
#include <cmath>
#include <time.h>
#include <omp.h>

using namespace std;
#include "output.h"
const double A_SCALE = 0.236589076381454;  // amplitude 
const double T_SCALE = 286.478897565412;   // period 

double acceleration(double t) {
    return A_SCALE * sin(t / T_SCALE);
}


double interpolate_acceleration(double t)
{
    int index = static_cast<int>(t);  // floor(t)
    int len = sizeof(DefaultProfile) / sizeof(DefaultProfile[0]);

    if (index < 0)
        return DefaultProfile[0];
    if (index >= len - 1)
        return DefaultProfile[len - 1];

    double fraction = t - index;  // between 0 and 1
    double acc = DefaultProfile[index] * (1 - fraction) + DefaultProfile[index + 1] * fraction;
    return acc;
}
void Get_input(int my_rank, double* a, double* b, double* step_size, int* n, struct timespec* start) {
    if (my_rank == 0) {
        cout << "Enter start time (s), end time (s) , and step_size(ms): ";
        cin >> *a >> *b >> *step_size;
        *step_size /= 1000.0;
        *n = static_cast<int>((*b - *a) / *step_size);
        clock_gettime(CLOCK_MONOTONIC, start);
    }

    MPI_Bcast(a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(step_size, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(n, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {
    int my_rank, comm_sz, n, local_n;
    double a, b, step_size, local_a, local_b;
    double local_dis = 0.0, total_dis = 0.0;
    double local_vel_delta = 0.0, final_vel = 0.0;

    struct timespec start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    Get_input(my_rank, &a, &b, &step_size, &n, &start);

    if (my_rank == 0)
        printf("my_rank=%d, a=%10.14lf, b=%10.14lf, step_size=%10.14lf, number of total steps=%d\n",
               my_rank, a, b, step_size, n);

    local_n = n / comm_sz;
    int remainder = n % comm_sz;
    if (my_rank == comm_sz - 1) {
        local_n += remainder;
    }

    local_a = a + my_rank * (n / comm_sz) * step_size;
    local_b = local_a + local_n * step_size;
    if (my_rank == comm_sz - 1) {
        local_b = b;
    }

    printf("my_rank=%d, start a=%10.14lf, end b=%10.14lf, number of quadratures = %d, step_size=%10.14lf (sec)\n",
           my_rank, local_a, local_b, local_n, step_size);

    // compute only delta velocity
    #pragma omp parallel for reduction(+:local_vel_delta)
    for (int i = 0; i < local_n; i++) {
        double t = local_a + i * step_size;
        local_vel_delta += interpolate_acceleration(t) * step_size;
    }

    // compute prefix sum to get starting velocity
    double starting_velocity = 0.0;
    MPI_Exscan(&local_vel_delta, &starting_velocity, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    if (my_rank == 0) starting_velocity = 0.0;

    //  integration with the correct starting velocity
    double v = starting_velocity;
    double v_new;

    // loop-carried dependency must be done sequentially
    for (int i = 0; i < local_n; i++) {
        double t = local_a + i * step_size;
        double a_left = interpolate_acceleration(t);
        v_new = v + a_left * step_size;
        local_dis += fabs(v) * step_size;
        v = v_new;
    }

    // reduce total distance
    MPI_Reduce(&local_dis, &total_dis, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    //final velocity from last rank
    if (my_rank == comm_sz - 1) {
        MPI_Send(&v, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    if (my_rank == 0) {
        MPI_Recv(&final_vel, 1, MPI_DOUBLE, comm_sz - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

        printf("Step size = %lf sec, our estimate\n", step_size);
        printf("of the total distance from %lf to %lf = %10.14lf m\n", a, b, total_dis);
        printf("Final velocity at t = %lf sec is %10.14lf m/s\n", b, final_vel);
        printf("Total execution time: %f seconds\n", elapsed_time);
    }

    MPI_Finalize();
    return 0;
}
