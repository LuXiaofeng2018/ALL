#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>



const double i24 = 1.0/24.0;
const double i12 = 1.0/12.0;
const double i3 = 1.0/3.0;

double phikm(double r)
{
    return fmax(0.,fmin(fmin(2.*r,i3*(1.+2.*r)),2.0));
}
double phikp(double r)
{
    return fmax(0.,fmin(fmin(2.*r,i3*(2.+r)),2.));
}




int main()
{
    printf("h\n");
    double r1 = 1.0 - 10.0;
    double r2 = 10.0 - 10.0;
    double r3 = r1/r2;
    double phr = phikm(r3);
    double phrp = phikp(r3);
    printf("result of update : %f \n", phr*(10.0 - 10.0));
    printf("r3 : %f | phr : %f \n | php : %f \n ", r3,phr,phrp);
    return 1;
}
