#define _USE_MATH_DEFINES
#include <math.h>
void square_complex(double* mx)
{
    double re = mx[0] * mx[0] - mx[1] * mx[1];
    double im = mx[0] * mx[1] +  mx[1] * mx[0];
    mx[0] = re;
    mx[1] = im;
}
void cos_complex(double* mx)
{
    double re = mx[0];
    double im = mx[1];
    mx[0] = cos(re) * cosh(im);
    mx[1] = sin(re) * sinh(im) * -1;
}
void sin_complex(double* mx)
{
    double re = mx[0];
    double im = mx[1];
    mx[0] = sin(re) * cosh(im);
    mx[1] = cos(re) * sinh(im);
}
void plus_copmex(double* src, const double* l)
{
    src[0] = src[0] + l[0];
    src[1] = src[1] + l[1];
}
void divide_complex(double* src, const double* left, const double* right)
{
    double div = pow(left[0],2) + pow(left[1], 2);
    double re = (left[0] * right[0] + left[1] * right[1] / div);
    double mi = (left[1] * right[0] - left[0] * right[1] / div);
    src[0] = re;
    src[1] = mi;
}
void exp_complex(double* mx)
{
    mx[0] = exp(mx[0]);
}
void multi_complex(double* src, const double* left, const double* right)
{
    double re = right[0] * left[0] - right[1] * left[1];
    double im = left[1] * right[0] + right[1] * left[0];
    src[0] = re;
    src[1] = im;
}
void log_complex(double* src, int k)
{
    double re = log(src[1]);
    double im = ( atan2(src[1], src[0]) + k * 2 * M_PI);
    src[0] = re;
    src[1] = im;
}
void pow_complex(double* mx, const double* z)
{
    log_complex(mx, 0);
    multi_complex(mx, z, mx);
    exp_complex(mx);
}
