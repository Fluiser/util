// copy-paste from https://github.com/OpenCL/ComplexMath/blob/master/clcomplex.h
// but modify: add sqr
#define OPENCL_COMPLEX_MATH_USE_DOUBLE

#ifndef OPENCL_COMPLEX_MATH
#define OPENCL_COMPLEX_MATH

#define CONCAT(x, y) x##y
#define FNAME(name, sufix) c##name##sufix

// float2
#define clrealf(complex) complex.x;
#define climagf(complex) complex.y;

// double2
#define clreal(complex) complex.x;
#define climag(complex) complex.y;

#define OPENCL_COMPLEX_MATH_FUNCS(complex_type, real_type, func_sufix, math_consts_sufix) \
    complex_type CONCAT(complex, func_sufix)(real_type r, real_type i) \
    { \
        return (complex_type)(r, i); \
    } \
    \
    complex_type FNAME(add, func_sufix)(complex_type x, complex_type y) \
    { \
        return x + y; \
    } \
    \
    complex_type FNAME(sub, func_sufix)(complex_type x, complex_type y) \
    { \
        return x - y; \
    } \
    \
    complex_type FNAME(add_real, func_sufix)(complex_type z, real_type r) \
    { \
        return (complex_type)(z.x + r, z.y); \
    } \
    \
    complex_type FNAME(sub_real, func_sufix)(complex_type z, real_type r) \
    { \
        return (complex_type)(z.x - r, z.y); \
    } \
    \
    real_type FNAME(abs, func_sufix)(complex_type z) \
    { \
        return length(z); \
    } \
    \
    real_type FNAME(arg, func_sufix)(complex_type z) \
    { \
        return atan2(z.y, z.x); \
    } \
    \
    complex_type FNAME(mul, func_sufix)(complex_type z1, complex_type z2) \
    { \
        real_type x1 = z1.x; \
        real_type y1 = z1.y; \
        real_type x2 = z2.x; \
        real_type y2 = z2.y; \
        return (complex_type)(x1 * x2 - y1 * y2, x1 * y2 + x2 * y1); \
    } \
    \
    complex_type FNAME(div, func_sufix)(complex_type z1, complex_type z2) \
    { \
        real_type x1 = z1.x; \
        real_type y1 = z1.y; \
        real_type x2 = z2.x; \
        real_type y2 = z2.y; \
        real_type iabs_z2 = CONCAT(1.0, func_sufix) / FNAME(abs, func_sufix)(z2); \
        return (complex_type)( \
            ((x1 * x2 * iabs_z2) + (y1 * y2 * iabs_z2)) * iabs_z2, \
            ((y1 * x2 * iabs_z2) - (x1 * y2 * iabs_z2)) * iabs_z2  \
        ); \
    } \
    \
    complex_type FNAME(mul_real, func_sufix)(complex_type z, real_type r) \
    { \
        return z * r; \
    } \
    \
    complex_type FNAME(div_real, func_sufix)(complex_type z, real_type r) \
    { \
        return z / r; \
    } \
    \
    complex_type FNAME(conj, func_sufix)(complex_type z) \
    { \
        return (complex_type)(z.x, -z.y); \
    } \
    \
    complex_type FNAME(proj, func_sufix)(complex_type z) \
    { \
        if(isinf(z.x) || isinf(z.y)) \
        { \
            return (complex_type)(INFINITY, (copysign(CONCAT(0.0, func_sufix), z.y))); \
        } \
        return z; \
    } \
    \
    real_type FNAME(norm, func_sufix)(complex_type z) \
    { \
        /* Returns the squared magnitude of the complex number z. */ \
        /* The norm calculated by this function is also known as */ \
        /* field norm or absolute square. */ \
        real_type x = z.x; \
        real_type y = z.y; \
        return x * x + y * y; \
    } \
    \
    complex_type FNAME(polar, func_sufix)(real_type r, real_type theta) \
    { \
        /* Returns a complex number with magnitude r and phase angle theta. */ \
        return (complex_type)(r * cos(theta), r * sin(theta)); \
    } \
    \
    complex_type FNAME(exp, func_sufix)(complex_type z) \
    { \
        /* The complex exponential function e^z for z = x+i*y */ \
        /* equals to e^x * cis(y), */ \
        /* or, e^x * (cos(y) + i*sin(y)) */ \
        real_type expx = exp(z.x); \
        return (complex_type)(expx * cos(z.y), expx * sin(z.y)); \
    } \
    \
    complex_type FNAME(log, func_sufix)(complex_type z) \
    { \
        /* log(z) = log(abs(z)) + i * arg(z)  */ \
        return (complex_type)(log(FNAME(abs, func_sufix)(z)),FNAME(arg, func_sufix)(z)); \
    } \
    \
    complex_type FNAME(log10, func_sufix)(complex_type z) \
    { \
        return FNAME(log, func_sufix)(z) / log(CONCAT(10.0, func_sufix)); \
    } \
    \
    complex_type FNAME(pow, func_sufix)(complex_type z1, complex_type z2) \
    { \
        /* (z1)^(z2) = exp(z2 * log(z1)) = cexp(mul(z2, clog(z1))) */ \
        return \
            FNAME(exp, func_sufix)( \
                FNAME(mul, func_sufix)( \
                    z2, \
                    FNAME(log, func_sufix)(z1) \
                ) \
            ); \
    } \
    \
	complex_type FNAME(sqr, func_sufix)(complex_type z) \
    { \
        return (complex_type)(z.x*z.x - z.y*z.y, z.x*z.y + z.y*z.x); \
    } \
    complex_type FNAME(sqrt, func_sufix)(complex_type z) \
    { \
        /*  */ \
        real_type x = z.x; \
        real_type y = z.y; \
        if(x == CONCAT(0.0, func_sufix)) \
        { \
            real_type t = sqrt(fabs(y) / 2); \
            return (complex_type)(t, y < CONCAT(0.0, func_sufix) ? -t : t); \
        } \
        else \
        { \
            real_type t = sqrt(2 * FNAME(abs, func_sufix)(z) + fabs(x)); \
            real_type u = t / 2; \
            return x > CONCAT(0.0, func_sufix) \
                ? (complex_type)(u, y / t) \
                : (complex_type)(fabs(y) / t, y < CONCAT(0.0, func_sufix) ? -u : u); \
        } \
    } \
    \
    complex_type FNAME(sin, func_sufix)(complex_type z) \
    { \
        const real_type x = z.x; \
        const real_type y = z.y; \
        return (complex_type)(sin(x) * cosh(y), cos(x) * sinh(y)); \
    } \
    \
    complex_type FNAME(sinh, func_sufix)(complex_type z) \
    { \
        const real_type x = z.x; \
        const real_type y = z.y; \
        return (complex_type)(sinh(x) * cos(y), cosh(x) * sin(y)); \
    } \
    \
    complex_type FNAME(cos, func_sufix)(complex_type z) \
    { \
        const real_type x = z.x; \
        const real_type y = z.y; \
        return (complex_type)(cos(x) * cosh(y), -sin(x) * sinh(y)); \
    } \
    \
    complex_type FNAME(cosh, func_sufix)(complex_type z) \
    { \
        const real_type x = z.x; \
        const real_type y = z.y; \
        return (complex_type)(cosh(x) * cos(y), sinh(x) * sin(y)); \
    } \
    \
    complex_type FNAME(tan, func_sufix)(complex_type z) \
    { \
        return FNAME(div, func_sufix)( \
            FNAME(sin, func_sufix)(z), \
            FNAME(cos, func_sufix)(z) \
        ); \
    } \
    \
    complex_type FNAME(tanh, func_sufix)(complex_type z) \
    { \
        return FNAME(div, func_sufix)( \
            FNAME(sinh, func_sufix)(z), \
            FNAME(cosh, func_sufix)(z) \
        ); \
    } \
    \
    complex_type FNAME(asinh, func_sufix)(complex_type z) \
    { \
        complex_type t = (complex_type)( \
            (z.x - z.y) * (z.x + z.y) + CONCAT(1.0, func_sufix), \
            CONCAT(2.0, func_sufix) * z.x * z.y \
        ); \
        t = FNAME(sqrt, func_sufix)(t) + z; \
        return FNAME(log, func_sufix)(t); \
    } \
    \
    complex_type FNAME(asin, func_sufix)(complex_type z) \
    { \
        complex_type t = (complex_type)(-z.y, z.x); \
        t = FNAME(asinh, func_sufix)(t); \
        return (complex_type)(t.y, -t.x); \
    } \
    \
    complex_type FNAME(acosh, func_sufix)(complex_type z) \
    { \
        return \
            CONCAT(2.0, func_sufix) * FNAME(log, func_sufix)( \
                FNAME(sqrt, func_sufix)( \
                    CONCAT(0.5, func_sufix) * (z + CONCAT(1.0, func_sufix)) \
                ) \
                + FNAME(sqrt, func_sufix)( \
                    CONCAT(0.5, func_sufix) * (z - CONCAT(1.0, func_sufix)) \
                ) \
            ); \
    } \
    \
    complex_type FNAME(acos, func_sufix)(complex_type z) \
    { \
        complex_type t = FNAME(asin, func_sufix)(z);\
        return (complex_type)( \
            CONCAT(M_PI_2, math_consts_sufix) - t.x, -t.y \
        ); \
    } \
    \
    complex_type FNAME(atanh, func_sufix)(complex_type z) \
    { \
        const real_type zy2 = z.y * z.y; \
        real_type n = CONCAT(1.0, func_sufix) + z.x; \
        real_type d = CONCAT(1.0, func_sufix) - z.x; \
        n = zy2 + n * n; \
        d = zy2 + d * d; \
        return (complex_type)( \
            CONCAT(0.25, func_sufix) * (log(n) - log(d)), \
            CONCAT(0.5, func_sufix) * atan2( \
                CONCAT(2.0, func_sufix) * z.y, \
                CONCAT(1.0, func_sufix) - zy2 - (z.x * z.x) \
            ) \
        ); \
    } \
    \
    complex_type FNAME(atan, func_sufix)(complex_type z) \
    { \
        const real_type zx2 = z.x * z.x; \
        real_type n = z.y + CONCAT(1.0, func_sufix); \
        real_type d = z.y - CONCAT(1.0, func_sufix); \
        n = zx2 + n * n; \
        d = zx2 + d * d; \
        return (complex_type)( \
            CONCAT(0.5, func_sufix) * atan2( \
                CONCAT(2.0, func_sufix) * z.x, \
                CONCAT(1.0, func_sufix) - zx2 - (z.y * z.y) \
            ), \
            CONCAT(0.25, func_sufix) * (log(n / d)) \
        ); \
    }

// float complex
typedef float2 cfloat;
OPENCL_COMPLEX_MATH_FUNCS(float2, float, f, _F)

// double complex
#ifdef cl_khr_fp64
#   ifdef OPENCL_COMPLEX_MATH_USE_DOUBLE
#       pragma OPENCL EXTENSION cl_khr_fp64 : enable
        typedef double2 cdouble;
        OPENCL_COMPLEX_MATH_FUNCS(double2, double, , )
#   endif
#endif

#undef FNAME
#undef CONCAT
#endif // OPENCL_COMPLEX_MATH


__kernel void SHADERMAIN(
    __global unsigned int* canvas, // 0
    double W_X, // 1
    double scale, // 2
    double offsetX, // 3
    double offsetY, // 4
    int MAX_ITERATIONS, //5
    double c0, // 6
    double c1 // 7
)
{
   size_t idx = get_global_id(0);
    double tx, ty;
    //z
    double2 z;
    {
	    double x = canvas[idx] >> 16;
	    double y = canvas[idx] ^ ((unsigned)x << 16);
	    x = (x/W_X - 0.5) / scale - offsetX;
	    y = (y/W_X - 0.5) / scale - offsetY;
		tx = x;
		ty = y;
		z = (double2)(x, y);
	}


    int depht = 0;

    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        // Мат. вычисление фрактала
        z = csqr(z);
        z.x += tx;
        z.y += ty;

        // А вот это уже не трогай.
        depht += (unsigned)(z.x*z.x + z.y*z.y >= 20) * (i+1);
    }
		
    unsigned c = 0xff*depht/(MAX_ITERATIONS*MAX_ITERATIONS); // Считаем, как быстро возрастало число и делаем из этого картинку.
    // Если убрать MAX_ITERATIONS^2, то получается depht будет показателем того, как часто число выходило за рамки.
    // С MAX_ITERATIONS^2 отображает, как быстро увеличивается число.
    // Возможно, что более правильно использовать адаптивное число MAX_IT, ибо z! возрастает быстрее, чем MAX_IT^n.
    // Но зачем, если проще когда проще, а использование "малинового цвета" оставляет место для переполнения.
		
    // Если убрать (i+1) с 340й строки, depht будет отображать частоту выхода число за интервал,
    // а не скорость возрастания числа.
    // Если использовать показатель частоты выхода, то следует убрать MAX_ITERATIONS из степени.
		
    
   
    canvas[idx] = ((((c)) | 0xff000000)) + ((depht >= 0) * 0xffffffff);
}
