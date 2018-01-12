 /* Serre2dc.i */
 %module Serre2dc
 %{
 /* Put header files here or function declarations like below */
 extern double *mallocPy(int n);
 extern void conc(double *a , double *b, double *c,int n,int m ,int k, double *d);
 extern void writetomem(double *x, int i , double f);
 extern double readfrommem(double *x,int i);
 extern void deallocPy(double *x);
 extern void getufromG(double *h, double *G, double *hebeg, double *heend, double *Gebeg, double *Geend,double *uebeg, double *ueend, double theta, double dx , int n, int nu, int nhbc, int nBC , double *u, double *hhbc, double *Ghbc);
 extern double minmod(double a, double b, double c);
 extern double HankEnergyall(double *x,double *h,double *u,double g,int n, int nBC,double dx); 
 extern void getufromG(double *h, double *G, double *hebeg, double *heend, double *Gebeg, double *Geend,double *uebeg, double *ueend, double theta, double dx , int n, int nu, int nhbc, int nBC , double *u, double *hhbc, double *Ghbc);
 extern double hall(double *x,double *h,int n, int nBC,double dx);
 extern double uhall(double *x,double *h,double *u,int n, int nBC,double dx);
 extern void evolvewrap(double *G, double *h,double *hebeg , double *heend ,double *Gebeg , double *Geend,double *uebeg , double *ueend, double g, double dx, double dt, int n, int nBCs, double theta, double *ubc, double *hhbc, double *Ghbc);
 %}
 extern double *mallocPy(int n);
 extern void conc(double *a , double *b, double *c,int n,int m ,int k, double *d);
 extern void writetomem(double *x, int i , double f);
 extern double readfrommem(double *x,int i);
 extern void deallocPy(double *x);
 extern void getufromG(double *h, double *G, double *hebeg, double *heend, double *Gebeg, double *Geend,double *uebeg, double *ueend, double theta, double dx , int n, int nu, int nhbc, int nBC , double *u, double *hhbc, double *Ghbc);
 extern double minmod(double a, double b, double c);
 extern double HankEnergyall(double *x,double *h,double *u,double g,int n, int nBC,double dx); 
 extern void getufromG(double *h, double *G, double *hebeg, double *heend, double *Gebeg, double *Geend,double *uebeg, double *ueend, double theta, double dx , int n, int nu, int nhbc, int nBC , double *u, double *hhbc, double *Ghbc);
 extern double hall(double *x,double *h,int n, int nBC,double dx);
 extern double uhall(double *x,double *h,double *u,int n, int nBC,double dx);
 extern void evolvewrap(double *G, double *h,double *hebeg , double *heend ,double *Gebeg , double *Geend,double *uebeg , double *ueend, double g, double dx, double dt, int n, int nBCs, double theta, double *ubc, double *hhbc, double *Ghbc);
