# Parallel Numerical Integration using MPI and OpenMP

This project implements a parallel numerical integration algorithm to compute final velocity and displacement from a given acceleration profile. The goal is to achieve high performance using both MPI (Message Passing Interface) and OpenMP, and to solve  loop-carried dependencies using hybrid parallelization in time-sensitive simulations. Applications include autonomous vehicle navigation, rocket landing systems, and other real-time control systems where fast and accurate motion estimation is critical.


🚦 Running the Program
🛠 Build
🛠 Build & Run Instructions
./run_integrate a b n <acceleration_profile.csv> col#
a: Starting point of integration (beginning of the acceleration profile)
b: End point of the profile
n: Step size for numerical integration
<acceleration_profile.csv>: CSV file containing the acceleration profile data
col#: Column number in the CSV file to extract data from

This shell script first runs a Python pre-processing script that converts the CSV acceleration data into a C-style array. Then, it calls a C++ integration script to compute the total distance by integrating the acceleration profile.
