#ifndef LEOPARD_MATH_SHIM_H
#define LEOPARD_MATH_SHIM_H
#ifdef __cplusplus
extern "C" {
#endif
extern long long llrintf(float);
extern long long llrintl(long double);
extern long long llrint(double);
extern long long llroundf(float);
extern long long llroundl(long double);
extern long long llround(double);
#ifdef __cplusplus
}
#endif
#endif
