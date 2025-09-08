#include <iostream>
#include <cmath>
#include <string>

#define RANGE (M_PI)
#define STEPS (1000000)

using namespace std;


const double A_SCALE = 0.236589076381454;  //amplitude 
const double T_SCALE = 286.478897565412; //period 

int thread_count=1;


#include "output.h"

double acceleration(double x)
{
    //return (sin(x));
    return A_SCALE * sin(x / T_SCALE); 
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

void Int_vel_pos(double a, double b, int n, 
    double* vel,
     double* dis) 
{
    double v = 0.0; //inital velocity gained  
    double x = 0.0; // inital  distance travled
    double step_size = (b - a) / n;

    for (int i = 0; i < n; i++) {
        double t = a + i * step_size;   //a + (i+0.5) * step_size;  //sampling from the midpoint
        //double a_t = acceleration(t);  // acceleration at time t 
        double a_t = interpolate_acceleration(t);
        v += a_t * step_size; // velocity time t
        //x += fabs(v) * step_size;   // distance traveled at time t
        x += v * step_size;
    }

    *vel = v;  // 
    *dis = x;       // Final position
}

int main(int argc, char* argv[]) 
{
    double a = 0.0;
    double b = RANGE;
    int n = STEPS;

    if(argc == 4)
    {
        a = atof(argv[1]);  // starting bound 
        b = atof(argv[2]);  // ending bound 
        n = atoi(argv[3]);  // number of steps
        printf("Running with: a = %f, b = %f, steps = %d\n", a, b, n);
    }
    else
    {
        cout << "Usage: " << argv[0] << " <a> <b> <steps> <input.csv> <column_index>" << endl;
        //printf("Using default values: a = %f, b = %f, steps = %d\n", a, b, n);
    }

    double step_size = (b - a) / n;
    double final_velocity = 0.0;
    double final_position = 0.0;
    Int_vel_pos(a, b, n, &final_velocity, &final_position);

    cout.precision(15);
    cout << "Final velocity at t = " << b << " is " << final_velocity << endl;
    cout << "Total distance traveled from t = " << a << " to t = " << b << " is " << final_position << endl;
    return 0;
}



