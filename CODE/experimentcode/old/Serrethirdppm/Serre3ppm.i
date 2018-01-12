 /* Serre3ppm.i */
 %module Serre3ppm
 %{
 /* Put header files here or function declarations like below */
 extern void conc(double *a , double *b, double *c,int n,int m ,int k, double *d);
 extern double *mallocPy(int n);
 extern void writetomem(double *x, int i , double f);
 extern double readfrommem(double *x,int i);
 extern void deallocPy(double *x);
 extern void TDMA(double *a, double *b, double *c, double *d, int n, double *x);
 extern void PENT(double *e, double *a, double *d, double *c,double *f, double *B, int n, double *x);
 extern void midpt2ca(double *qm , double dx , int n, double *qa);
 extern void ca2midpt(double *qa, double dx, int n,double *qm);
 extern void ufromGh(double *G, double *h,double *hbeg,double *hend,double *ubeg,double *uend, double dx , int n, int nBC, double *u);
 extern void Gfromuh(double *u, double *h,double *hbeg,double *hend,double *ubeg,double *uend, double dx , int n, int nBC, double *G);
 extern void reconstructppm(double *u, int i, double dx, double* uil, double* uir);
 extern void weightsum(double a,double *x, double b, double *y, int n, double *z);
 extern void evolve(double *G, double *h, double *u, double g, double dx, double dt,int n, int nBC, double *nh, double *nG);
 extern void evolvewrap(double *Ga, double *ha, double *Gabeg, double *Gaend, double *habeg, double *haend, double *hmbeg, double *hmend, double *uabeg, double *uaend, double *umbeg, double *umend, int nfcBC, int nGsBC, double g, double dx, double dt, int n, int nBCa, int nBCm);
 %} 
 extern void conc(double *a , double *b, double *c,int n,int m ,int k, double *d);
 extern double *mallocPy(int n);
 extern void writetomem(double *x, int i , double f);
 extern double readfrommem(double *x,int i);
 extern void deallocPy(double *x);
 extern void TDMA(double *a, double *b, double *c, double *d, int n, double *x);
 extern void PENT(double *e, double *a, double *d, double *c,double *f, double *B, int n, double *x);
 extern void midpt2ca(double *qm , double dx , int n, double *qa);
 extern void ca2midpt(double *qa, double dx, int n,double *qm);
 extern void ufromGh(double *G, double *h,double *hbeg,double *hend,double *ubeg,double *uend, double dx , int n, int nBC, double *u);
 extern void Gfromuh(double *u, double *h,double *hbeg,double *hend,double *ubeg,double *uend, double dx , int n, int nBC, double *G);
 extern void reconstructppm(double *u, int i, double dx, double* uil, double* uir);
 extern void weightsum(double a,double *x, double b, double *y, int n, double *z);
 extern void evolve(double *G, double *h, double *u, double g, double dx, double dt,int n, int nBC, double *nh, double *nG);
 extern void evolvewrap(double *Ga, double *ha, double *Gabeg, double *Gaend, double *habeg, double *haend, double *hmbeg, double *hmend, double *uabeg, double *uaend, double *umbeg, double *umend, int nfcBC, int nGsBC, double g, double dx, double dt, int n, int nBCa, int nBCm);
