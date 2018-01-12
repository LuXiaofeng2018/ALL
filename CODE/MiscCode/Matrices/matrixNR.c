#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SWAP(a,b) {dum=(a);(a)=(b);(b)=dum;}
#define TINY 1.0e-20
#define NR_END 1


const double i24 = 1.0/24.0;
const double i12 = 1.0/12.0;
const double i16 = 1.0/16.0;
const double i3 = 1.0/3.0;
const double i6 = 1.0/6.0;
const double i8 = 1.0/8.0;
const double i48 = 1.0/48.0;
const double i5 = 1.0/5.0;
const double i42 = 1.0/42.0;
const double i280 = 1.0/280.0;
const double i1680 = 1.0/1680.0;
const double i560 = 1.0/560.0;
const double i140 = 1.0/140.0;
const double i210 = 1.0/210.0;
const double i105 = 1.0 / 105.0;
const double i84 = 1.0/84.0;
const double i70 = 1.0/ 70.0;
const double i80 = 1.0/80.0;
const double i28 = 1.0/28.0;
const double i15 = 1.0/15.0;
const double i10 = 0.1;
const double i9 = 1.0 / 9.0;
const double i120 = 1.0/ 120.0;
const double i20 = 1.0/ 20.0;
const double i35 = 1.0/35.0;
const double i14 = 1.0/ 14.0;
const double i420 = 1.0/420.0;
const double i1344 = 1.0/ 1344.0;
const double i6720 = 1.0 / 6720.0;
const double i2240 = 1.0 / 2240.0;
const double i320 = 1.0 / 320.0;
const double i336 = 1.0 / 336.0;
const double i112 = 1.0 / 122.0;
const double i240 = 1.0 / 240.0;
const double i60 = 1.0 / 60.0;
const double i30 = 1.0 / 30.0;


// ############################################################################ START OF NUMERICAL RECIPES CODE #########################################################################

void nrerror(char error_text[])
/* Numerical Recipes standard error handler */
{
	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
	exit(1);
}


long LMIN(long a, long b)
{
    return (b >= a)*a + (b < a)*b;

}

long LMAX(long a, long b)
{
    return (b >= a)*b + (b < a)*a;

}

double **dmatrix(long nrl, long nrh, long ncl, long nch)
/* allocate a double matrix with subscript range m[nrl..nrh][ncl..nch] */
{
	long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
	double **m;

	/* allocate pointers to rows */
	m=(double **) malloc((size_t)((nrow+NR_END)*sizeof(double*)));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;

	/* allocate rows and set pointers to them */
	m[nrl]=(double *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(double)));
	if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;

	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

	/* return pointer to array of pointers to rows */
	return m;
}



void banmul(double **a, unsigned long n, int m1, int m2, double x0[], double b0[])
{
// x, b are not zero offset, as is a
    double *x, *b;
    x = x0 -1;
    b = b0 - 1;
    unsigned long i,j,k,tmploop;
    for (i=1;i<=n;i++) 
    {
        k=i-m1-1;
        tmploop=LMIN(m1+m2+1,n-k);
        b[i]=0.0;

        for (j=LMAX(1,1-k);j<=tmploop;j++) b[i] += a[i][j]*x[j+k];
    }

}

double bandec(double **a, unsigned long n, int m1, int m2, double **al,unsigned long indx0[])
{

    double d = 1.0;
    unsigned long *indx;
    indx = indx0 -1;

    unsigned long i,j,k,l;
    int mm;
    double dum;

    mm=m1+m2+1;
    l=m1;


    for (i=1;i<=m1;i++) 
    {
        //Rearrange the storage a bit.
        for (j=m1+2-i;j<=mm;j++) a[i][j-l]=a[i][j];

        l--;

        for (j=mm-l;j<=mm;j++) a[i][j]=0.0;
    }

    l=m1;
    for (k=1;k<=n;k++) 
    {
        //For each row...
        dum=a[k][1];
        i=k;
        if (l < n) l++;

        for (j=k+1;j<=l;j++) 
        {
        //Find the pivot element.
            if (fabs(a[j][1]) > fabs(dum)) 
            {
                dum=a[j][1];
                i=j;
            }
        }

        indx[k]=i;

        if (dum == 0.0) a[k][1]=TINY;
        //Matrix is algorithmically singular, but proceed anyway with TINY pivot (desirable in   some applications).

        if (i != k) 
        {
        //Interchange rows.
            d = -(d);
            for (j=1;j<=mm;j++) SWAP(a[k][j],a[i][j])
        }

        for (i=k+1;i<=l;i++) 
        {
            //Do the elimination.
            dum=a[i][1]/a[k][1];
            al[k][i-k]=dum;
            for (j=2;j<=mm;j++) a[i][j-1]=a[i][j]-dum*a[k][j];
            a[i][mm]=0.0;
        }
    }

    return d;

}


void banbks(double **a, unsigned long n, int m1, int m2, double **al, unsigned long indx0[], double b0[])
{

/*Given the arrays a , al , and indx as returned from bandec , and given a right-hand side vector
b[1..n] , solves the band diagonal linear equations A · x = b. The solution vector x overwrites
b[1..n] . The other input arrays are not modified, and can be left in place for successive calls
with different right-hand sides.*/

    unsigned long i,k,l;
    int mm;
    double dum;

    unsigned long *indx;
    indx = indx0 -1;

    double *b;
    b = b0 - 1;

    mm=m1+m2+1;
    l=m1;

    for (k=1;k<=n;k++) 
    {
        //Forward substitution, unscrambling the permuted rows as we go.
        i=indx[k];

        if (i != k) SWAP(b[k],b[i])

        if (l < n) l++;

        for (i=k+1;i<=l;i++) b[i] -= al[k][i-k]*b[k];
    }

    l=1;

    for (i=n;i>=1;i--) 
    {
        //Backsubstitution.
        dum=b[i];
        for (k=2;k<=l;k++) dum -= a[i][k]*b[k+i-1];

        b[i]=dum/a[i][1];

        if (l < mm) l++;
    }
}


// ############################################################################ END OF NUMERICAL RECIPES CODE #########################################################################


// ####################################################################################################### START OF  CODE REQUIRED TO INTERFACE WITH PYTHON ##############################################################
double *mallocPy(int n)
{
    double *x = malloc(n*sizeof(double));
    return x;
}

unsigned long *mallocLongPy(int n)
{
    unsigned long *x = malloc(n*sizeof(unsigned long));
    return x;
}

double **malloc22Py(int r, int c)
{

   return dmatrix(1,r,1, c);

}


void writetomem(double *x, int i , double f)
{
    x[i] = f;

}

void writeto2mem(double **x, int i, int j , double f)
{
    x[i][j] = f;

}



double readfrommem(double*x,int i)
{
    return x[i];
}

double readfrom2Dmem(double **x,int i,int j)
{
    return x[i][j];
}

void deallocPy(double *x)
{
    free(x);
}

// ####################################################################################################### END OF  CODE REQUIRED TO INTERFACE WITH PYTHON ##############################################################


void conc(double *a , double *b, double *c,int n,int m ,int k, double *d)
{
    //replace with memcpy for performance?
    memcpy(d,a,n*sizeof(double));
    memcpy(d+n,b,m*sizeof(double));
    memcpy(d+n+m,c,k*sizeof(double));
}

double minmod(double a, double b, double c)
{
    if((a > 0) && (b>0) && (c>0))
    {
        return fmin(a,fmin(b,c));
    }
    else if((a < 0) && (b<0) && (c<0))
    {
        return fmax(a,fmax(b,c));
    }
        return 0.0;
}

void PENT(double *e, double *a, double *d, double *c,double *f, double *B, int n, double *x)
{
    //works but is destructive on inputs
    int i;
    double m;
    for(i=1; i<n-1; i++)
    {
        m = a[i-1] /d[i-1];
        
        d[i] = d[i] - m*c[i-1];
        c[i] = c[i] - m*f[i-1];
        B[i] = B[i] - m*B[i-1];

        m = e[i-1] /d[i-1];
        a[i] = a[i] - m*c[i-1];
        d[i+1] = d[i+1] - m*f[i-1];
        B[i+1] = B[i+1] - m*B[i-1];
    }

    m = a[n-2] / d[n-2];
    d[n-1] = d[n-1] - m*c[n-2];
    x[n-1] = (B[n-1] - m*B[n-2]) /d[n-1];
    x[n-2] = (B[n-2] - c[n-2]*x[n-1]) / d[n-2];

    for (i=n-3; i > -1; i--)
    {
        x[i] = (B[i] - f[i]*x[i+2] - c[i]*x[i+1]) / d[i];  
   
    }
}


void getufromG(double *h, double *G, double *bed, double *hMbeg, double *hMend, double *wMbeg, double *wMend, double *GMbeg, double *GMend,double *uMbeg, double *uMend, double *bMbeg, double *bMend, double theta, double dx , int n, int m, int nGhBC,int unBC,int nbBC, int nGhbc, int nubc, int nbhc, double *u, double *hhbc, double *whbc,double *Ghbc, double *bedhbc)
{
    //trying to join different B.C systems...., also try incorporating into  C code that already has beds, like o2bedfix.

    // n is length of h, G
    //nu is length of u (should be n + 1, to count edges of n cells)

    //enforcing B.Cs at cell edges now

    double idx = 1.0/ dx;
    double *uais = malloc((m-2)*sizeof(double));
    double *ubis = malloc((m-1)*sizeof(double));
    double *ucis = malloc((m)*sizeof(double));
    double *udis = malloc((m-1)*sizeof(double));
    double *ueis = malloc((m-2)*sizeof(double));

    double *nGis = malloc((m)*sizeof(double));
    double *utemp = malloc((m)*sizeof(double));

    int i,j;

    double dGib,dGim,dGif,dhib,dhim,dhif,dGi,dhi,Gjphm,Gjmhp,hjphm,hjmhp,bedai,bedbi,bedci,beddi,bjmh,bjms,bjps,bjph;

    double Gintia11,Gintia21,Gintia31,uhintia11,uhintia12,uhintia13,uhintia21,uhintia22,uhintia23,uhintia31,uhintia32,uhintia33;
    double h3uxintia11,h3uxintia12,h3uxintia13,h3uxintia21,h3uxintia22,h3uxintia23,h3uxintia31,h3uxintia32,h3uxintia33;
    double h2bxuxva11,h2bxuxva12,h2bxuxva13,h2bxuxva21,h2bxuxva22,h2bxuxva23,h2bxuxva31,h2bxuxva32,h2bxuxva33;
    double h2bxuvxa11,h2bxuvxa12,h2bxuvxa13,h2bxuvxa21,h2bxuvxa22,h2bxuvxa23,h2bxuvxa31,h2bxuvxa32,h2bxuvxa33;
    double hbx2uva11,hbx2uva12,hbx2uva13,hbx2uva21,hbx2uva22,hbx2uva23,hbx2uva31,hbx2uva32,hbx2uva33;
    double LHSa11,LHSa12,LHSa13,LHSa21,LHSa22,LHSa23,LHSa31,LHSa32,LHSa33;
    double wim1, wi,wip1,dwim,dwif,dwi,dwib,wjmhp,wjphm;
    double bedbegmiddle,bedendmiddle;

    // Zero out the diagonal arrays
    for(i = 0; i < m-2 ; i++)
    {
        uais[i] = 0;
        ubis[i] = 0;
        ucis[i] = 0;
        udis[i] = 0;
        ueis[i] = 0;
        nGis[i] = 0;
    
    }

    ubis[m-2] = 0;
    ucis[m-2] = 0;
    udis[m-2] = 0;
    nGis[m-2] = 0;


    ucis[m-1] = 0;
    nGis[m-1] = 0;
    j = 3;
    for (i =1;i < n-1 ; i++)
    {
        // Reconstruct G and h
        dGib = (G[i] - G[i-1]);
        dGim = 0.5*(G[i+1] - G[i-1]);
        dGif = (G[i+1] - G[i]);

        dhib = (h[i] - h[i-1]);
        dhim = 0.5*(h[i+1] - h[i-1]);
        dhif = (h[i+1] - h[i]);

        wi = h[i] + bed[i];
        wip1 = h[i+1] + bed[i+1];
        wim1 = h[i-1] + bed[i-1];
        dwib = (wi - wim1);
        dwim = 0.5*(wip1 - wim1);
        dwif = (wip1 - wi);

        dGi = minmod(theta*dGib, dGim, theta*dGif);
        dhi = minmod(theta*dhib, dhim, theta*dhif);
        dwi = minmod(theta*dwib, dwim, theta*dwif);

        Gjphm= G[i] + 0.5*dGi;
        Gjmhp= G[i] - 0.5*dGi;

        hjphm= fmin(h[i] + 0.5*dhi, 0);
        hjmhp= fmin(h[i] - 0.5*dhi, 0);   

        wjphm= wi + 0.5*dwi;
        wjmhp= wi - 0.5*dwi;   

        // reconstruct bed
        if(i == 1) 
        {
            bedbegmiddle = -bMbeg[0]*i16 + 9*bMbeg[1]*i16 + 9*bMbeg[2]*i16 - bMbeg[3]*i16;

            bedai = i12*idx*idx*idx*(-bedbegmiddle + 2*bed[i-1] -2*bed[i+1] + bed[i+2]);
            bedbi = i6*idx*idx*(bedbegmiddle - bed[i-1] - bed[i+1] + bed[i+2]);
            bedci = i12*idx*(bedbegmiddle - 8*bed[i-1] + 8*bed[i+1] - bed[i+2]);
            beddi = -bedbegmiddle*i6 + 2*i3*bed[i-1] + 2*i3*bed[i+1] - bed[i+2]*i6;
        }
        else if(i == n-2)
        {
            bedendmiddle = -bMend[0]*i16 + 9*bMend[1]*i16 + 9*bMend[2]*i16 - bMend[3]*i16;

            bedai = i12*idx*idx*idx*(-bed[i-2] + 2*bed[i-1] -2*bed[i+1] + bedendmiddle);
            bedbi = i6*idx*idx*(bed[i-2] - bed[i-1] - bed[i+1] + bedendmiddle);
            bedci = i12*idx*(bed[i-2] - 8*bed[i-1] + 8*bed[i+1] - bedendmiddle);
            beddi = -bed[i-2]*i6 + 2*i3*bed[i-1] + 2*i3*bed[i+1] - bedendmiddle*i6;

        }
        else
        {
            bedai = i12*idx*idx*idx*(-bed[i-2] + 2*bed[i-1] -2*bed[i+1] + bed[i+2]);
            bedbi = i6*idx*idx*(bed[i-2] - bed[i-1] - bed[i+1] + bed[i+2]);
            bedci = i12*idx*(bed[i-2] - 8*bed[i-1] + 8*bed[i+1] - bed[i+2]);
            beddi = -bed[i-2]*i6 + 2*i3*bed[i-1] + 2*i3*bed[i+1] - bed[i+2]*i6;

        }

        bjmh = -bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) - bedci*(0.5*dx) + beddi;
        bjms = -bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) - bedci*(dx*i6) + beddi;
        bjps = bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) + bedci*(dx*i6) + beddi;
        bjph = bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) + bedci*(0.5*dx) + beddi;

        // G integral (RHS)
        Gintia11 = i6*dx*(Gjmhp);
        Gintia21 = i6*dx*(2*Gjmhp + 2*Gjphm);
        Gintia31 = i6*dx*(Gjphm);
        
        
        //uh integral
        uhintia11 = 0.1*i6*dx*(7*hjmhp + hjphm);
        uhintia12 = 0.1*i6*dx*(4*hjmhp );
        uhintia13 = 0.1*i6*dx*(-hjmhp - hjphm);
        
        uhintia21 = 0.1*i6*dx*(4*hjmhp);
        uhintia22 = 0.1*i6*dx*(16*hjmhp + 16*hjphm);
        uhintia23 = 0.1*i6*dx*(4*hjphm);
        
        uhintia31 = 0.1*i6*dx*(-hjmhp - hjphm);
        uhintia32 = 0.1*i6*dx*(4*hjphm);
        uhintia33 = 0.1*i6*dx*(hjmhp + 7*hjphm);
        
        //h3ux
        
        h3uxintia11 = 2*i3*idx*((79.0*0.1*i12)*hjmhp*hjmhp*hjmhp +  (39.0*0.1*i12)*hjmhp*hjmhp*hjphm + (3.0*0.5*i12)*hjmhp*hjphm*hjphm + (7.0*0.1*i12)*hjphm*hjphm*hjphm);        
        h3uxintia12 = 2*i3*idx*((-23.0*i3*0.1)*hjmhp*hjmhp*hjmhp +  (-3.0*0.1)*hjmhp*hjmhp*hjphm + (-3.0*i3*0.1)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
        h3uxintia13 = 2*i3*idx*((13.0*0.1*i12)*hjmhp*hjmhp*hjmhp +  (-3.0*0.1*i12)*hjmhp*hjmhp*hjphm + (-3.0*0.1*i12)*hjmhp*hjphm*hjphm + (13.0*0.1*i12)*hjphm*hjphm*hjphm);
        
        
        h3uxintia21 = 2*i3*idx*((-23.0*i3*0.1)*hjmhp*hjmhp*hjmhp +  (-3.0*0.1)*hjmhp*hjmhp*hjphm + (-3.0*0.1*i3)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
        h3uxintia22 = 2*i3*idx*((14.0*i3*i5)*hjmhp*hjmhp*hjmhp +  (6.0*i3*i5)*hjmhp*hjmhp*hjphm + (6.0*i3*i5)*hjmhp*hjphm*hjphm + (14.0*i3*i5)*hjphm*hjphm*hjphm);        
        h3uxintia23 = 2*i3*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i3*0.1)*hjmhp*hjmhp*hjphm + (-3.0*0.1)*hjmhp*hjphm*hjphm + (-23.0*i3*0.1)*hjphm*hjphm*hjphm);
        
        h3uxintia31 = 2*i3*idx*((13.0*0.1*i12)*hjmhp*hjmhp*hjmhp +  (-3.0*0.1*i12)*hjmhp*hjmhp*hjphm + (-3.0*0.1*i12)*hjmhp*hjphm*hjphm + (13.0*0.1*i12)*hjphm*hjphm*hjphm);       
        h3uxintia32 = 2*i3*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i3*0.1)*hjmhp*hjmhp*hjphm + (-3.0*0.1)*hjmhp*hjphm*hjphm + (-23.0*i3*0.1)*hjphm*hjphm*hjphm);        
        h3uxintia33 = 2*i3*idx*((7.0*0.1*i12)*hjmhp*hjmhp*hjmhp +  (3.0*0.5*i12)*hjmhp*hjmhp*hjphm + (39.0*0.1*i12)*hjmhp*hjphm*hjphm + (79.0*0.1*i12)*hjphm*hjphm*hjphm);
        
        //h2 bx ux v
        h2bxuxva11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
        h2bxuxva12 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
        h2bxuxva13 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);

        h2bxuxva21 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
        h2bxuxva22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
        h2bxuxva23 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);

        h2bxuxva31 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);
        h2bxuxva32 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);
        h2bxuxva33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);

        //h2 bx u vx
        h2bxuvxa11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
        h2bxuvxa12 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
        h2bxuvxa13 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);

        h2bxuvxa21 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
        h2bxuvxa22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
        h2bxuvxa23 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);

        h2bxuvxa31 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);
        h2bxuvxa32 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);
        h2bxuvxa33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);
                                        

        //h bx bx u v
        hbx2uva11 = 2*idx*((1333.0*i1344)*hjmhp*bjmh*bjmh+(443.0*i6720)*hjphm*bjmh*bjmh+(-3141.0*i2240)*hjmhp*bjms*bjmh+(-171.0*i2240)*hjphm*bjms*bjmh+(1161.0*i2240)*hjmhp*bjps*bjmh+(27.0*i2240)*hjphm*bjps*bjmh+(-145.0*i1344)*hjmhp*bjph*bjmh+(-11.0*i6720)*hjphm*bjph*bjmh+(-3141.0*i2240)*hjmhp*bjmh*bjms+(-171.0*i2240)*hjphm*bjmh*bjms+(4617.0*i2240)*hjmhp*bjms*bjms+(243.0*i2240)*hjphm*bjms*bjms+(-1863.0*i2240)*hjmhp*bjps*bjms+(-81.0*i2240)*hjphm*bjps*bjms+(387.0*i2240)*hjmhp*bjph*bjms+(9.0*i2240)*hjphm*bjph*bjms+(1161.0*i2240)*hjmhp*bjmh*bjps+(27.0*i2240)*hjphm*bjmh*bjps+(-1863.0*i2240)*hjmhp*bjms*bjps+(-81.0*i2240)*hjphm*bjms*bjps+(891.0*i2240)*hjmhp*bjps*bjps+(81.0*i2240)*hjphm*bjps*bjps+(-27.0*i320)*hjmhp*bjph*bjps+(-27.0*i2240)*hjphm*bjph*bjps+(-145.0*i1344)*hjmhp*bjmh*bjph+(-11.0*i6720)*hjphm*bjmh*bjph+(387.0*i2240)*hjmhp*bjms*bjph+(9.0*i2240)*hjphm*bjms*bjph+(-27.0*i320)*hjmhp*bjps*bjph+(-27.0*i2240)*hjphm*bjps*bjph+(131.0*i6720)*hjmhp*bjph*bjph+(13.0*i1344)*hjphm*bjph*bjph);
        hbx2uva12 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
        hbx2uva13 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);


        hbx2uva21 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
        hbx2uva22 = 2*idx*((101.0*i420)*hjmhp*bjmh*bjmh+(29.0*i420)*hjphm*bjmh*bjmh+(-6.0*i35)*hjmhp*bjms*bjmh+(-3.0*i35)*hjphm*bjms*bjmh+(-3.0*i28)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(4.0*i105)*hjmhp*bjph*bjmh+(4.0*i105)*hjphm*bjph*bjmh+(-6.0*i35)*hjmhp*bjmh*bjms+(-3.0*i35)*hjphm*bjmh*bjms+(27.0*i28)*hjmhp*bjms*bjms+(27.0*i28)*hjphm*bjms*bjms+(-27.0*i35)*hjmhp*bjps*bjms+(-27.0*i35)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(-3.0*i28)*hjphm*bjph*bjms+(-3.0*i28)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-27.0*i35)*hjmhp*bjms*bjps+(-27.0*i35)*hjphm*bjms*bjps+(27.0*i28)*hjmhp*bjps*bjps+(27.0*i28)*hjphm*bjps*bjps+(-3.0*i35)*hjmhp*bjph*bjps+(-6.0*i35)*hjphm*bjph*bjps+(4.0*i105)*hjmhp*bjmh*bjph+(4.0*i105)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(-3.0*i28)*hjphm*bjms*bjph+(-3.0*i35)*hjmhp*bjps*bjph+(-6.0*i35)*hjphm*bjps*bjph+(29.0*i420)*hjmhp*bjph*bjph+(101.0*i420)*hjphm*bjph*bjph);
        hbx2uva23 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);

        hbx2uva31 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);                
        hbx2uva32 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);
        hbx2uva33 = 2*idx*((13.0*i1344)*hjmhp*bjmh*bjmh+(131.0*i6720)*hjphm*bjmh*bjmh+(-27.0*i2240)*hjmhp*bjms*bjmh+(-27.0*i320)*hjphm*bjms*bjmh+(9.0*i2240)*hjmhp*bjps*bjmh+(387.0*i2240)*hjphm*bjps*bjmh+(-11.0*i6720)*hjmhp*bjph*bjmh+(-145.0*i1344)*hjphm*bjph*bjmh+(-27.0*i2240)*hjmhp*bjmh*bjms+(-27.0*i320)*hjphm*bjmh*bjms+(81.0*i2240)*hjmhp*bjms*bjms+(891.0*i2240)*hjphm*bjms*bjms+(-81.0*i2240)*hjmhp*bjps*bjms+(-1863.0*i2240)*hjphm*bjps*bjms+(27.0*i2240)*hjmhp*bjph*bjms+(1161.0*i2240)*hjphm*bjph*bjms+(9.0*i2240)*hjmhp*bjmh*bjps+(387.0*i2240)*hjphm*bjmh*bjps+(-81.0*i2240)*hjmhp*bjms*bjps+(-1863.0*i2240)*hjphm*bjms*bjps+(243.0*i2240)*hjmhp*bjps*bjps+(4617.0*i2240)*hjphm*bjps*bjps+(-171.0*i2240)*hjmhp*bjph*bjps+(-3141.0*i2240)*hjphm*bjph*bjps+(-11.0*i6720)*hjmhp*bjmh*bjph+(-145.0*i1344)*hjphm*bjmh*bjph+(27.0*i2240)*hjmhp*bjms*bjph+(1161.0*i2240)*hjphm*bjms*bjph+(-171.0*i2240)*hjmhp*bjps*bjph+(-3141.0*i2240)*hjphm*bjps*bjph+(443.0*i6720)*hjmhp*bjph*bjph+(1333.0*i1344)*hjphm*bjph*bjph);       

        // LHS 
        
        LHSa11 = uhintia11 + h3uxintia11 + h2bxuxva11 + h2bxuvxa11 + hbx2uva11;
        LHSa12 = uhintia12 + h3uxintia12 + h2bxuxva12 + h2bxuvxa12 + hbx2uva12; 
        LHSa13 = uhintia13 + h3uxintia13 + h2bxuxva13 + h2bxuvxa13 + hbx2uva13;
        LHSa21 = uhintia21 + h3uxintia21 + h2bxuxva21 + h2bxuvxa21 + hbx2uva21;
        LHSa22 = uhintia22 + h3uxintia22 + h2bxuxva22 + h2bxuvxa22 + hbx2uva22; 
        LHSa23 = uhintia23 + h3uxintia23 + h2bxuxva23 + h2bxuvxa23 + hbx2uva23;
        LHSa31 = uhintia31 + h3uxintia31 + h2bxuxva31 + h2bxuvxa31 + hbx2uva31;
        LHSa32 = uhintia32 + h3uxintia32 + h2bxuxva32 + h2bxuvxa32 + hbx2uva32;
        LHSa33 = uhintia33 + h3uxintia33 + h2bxuxva33 + h2bxuvxa33 + hbx2uva33;
        
        uais[j-1] = uais[j-1] + LHSa31;
        
        ubis[j-1] = ubis[j-1] + LHSa21;
        ubis[j] = ubis[j] + LHSa32;
        
        ucis[j-1] = ucis[j-1] + LHSa11;
        ucis[j] = ucis[j] + LHSa22;
        ucis[j+1] = ucis[j+1] + LHSa33;
        
        udis[j-1] = udis[j-1] + LHSa12;
        udis[j] = udis[j] + LHSa23;
        
        ueis[j-1] = ueis[j-1] + LHSa13;

        nGis[j-1] = nGis[j-1] + Gintia11;
        nGis[j] = nGis[j] + Gintia21; 
        nGis[j+1] = nGis[j+1] + Gintia31; 


        hhbc[3*i + (nGhBC)] = hjmhp;
        hhbc[3*i+1 + (nGhBC)] = fmin(h[i], 0);
        hhbc[3*i+2 + (nGhBC)] = hjphm; 

        whbc[3*i + (nGhBC)] = wjmhp;
        whbc[3*i+1 + (nGhBC)] = wi;
        whbc[3*i+2 + (nGhBC)] = wjphm;

        Ghbc[3*i + (nGhBC)] = Gjmhp;
        Ghbc[3*i+1 + (nGhBC)] = G[i];
        Ghbc[3*i+2 + (nGhBC)] = Gjphm;

        bedhbc[4*i + (nbBC)] = bjmh;
        bedhbc[4*i+1 + (nbBC)] = bjms;
        bedhbc[4*i+2 + (nbBC)] = bjps;
        bedhbc[4*i+3 + (nbBC)] = bjph;
        
        
        j = j + 2 ;       


    }


    // first 
    i = 0;
    j = 1;

    // Get it from interior
    // Reconstruct w
    wi = h[i] + bed[i];
 
    wjphm= whbc[i + 2*nGhBC];
    wjmhp= wMbeg[nGhBC-1];  

    // Reconstruct G and h
    Gjphm= Ghbc[i + 2*nGhBC];
    Gjmhp= GMbeg[nGhBC-1];

    hjphm= hhbc[i + 2*nGhBC] ;
    hjmhp= fmin(hMbeg[nGhBC-1],0);

    // reconstruct bed
    bedai = i6*idx*idx*idx*(bed[i+3] -3*bed[i+2] +3*bed[i+1] - bed[i]);
    bedbi = 0.5*idx*idx*(-bed[i+3] + 4*bed[i+2] - 5*bed[i+1] + 2*bed[i]);
    bedci = i6*idx*(2*bed[i+3] - 9*bed[i+2] + 18*bed[i+1] - 11*bed[i]);
    beddi = bed[i];

    bjmh = -bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) - bedci*(0.5*dx) + beddi;
    bjms = -bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) - bedci*(dx*i6) + beddi;
    bjps = bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) + bedci*(dx*i6) + beddi;
    bjph = bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) + bedci*(0.5*dx) + beddi;

    // G integral (RHS)
    Gintia11 = dx*i6*(Gjmhp);
    Gintia21 = dx*i6*(2*Gjmhp + 2*Gjphm);
    Gintia31 = dx*i6*(Gjphm);
    
    
    //uh integral
    uhintia11 = (1.0 *i60)*dx*(7*hjmhp + hjphm);
    uhintia12 = (1.0 *i60)*dx*(4*hjmhp );
    uhintia13 = (1.0 *i60)*dx*(-hjmhp - hjphm);
    
    uhintia21 = (1.0 *i60)*dx*(4*hjmhp);
    uhintia22 = (1.0 *i60)*dx*(16*hjmhp + 16*hjphm);
    uhintia23 = (1.0 *i60)*dx*(4*hjphm);
    
    uhintia31 = (1.0 *i60)*dx*(-hjmhp - hjphm);
    uhintia32 = (1.0 *i60)*dx*(4*hjphm);
    uhintia33 = (1.0 *i60)*dx*(hjmhp + 7*hjphm);
    
    //h3ux
    
    h3uxintia11 = (2.0*i3)*idx*((79.0*i120)*hjmhp*hjmhp*hjmhp +  (39.0*i120)*hjmhp*hjmhp*hjphm + (3.0*i24)*hjmhp*hjphm*hjphm + (7.0*i120)*hjphm*hjphm*hjphm);        
    h3uxintia12 = (2.0*i3)*idx*((-23.0*i30)*hjmhp*hjmhp*hjmhp +  (-3.0*i10)*hjmhp*hjmhp*hjphm + (-3.0*i30)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
    h3uxintia13 = (2.0*i3)*idx*((13.0*i120)*hjmhp*hjmhp*hjmhp +  (-3.0*i120)*hjmhp*hjmhp*hjphm + (-3.0*i120)*hjmhp*hjphm*hjphm + (13.0*i120)*hjphm*hjphm*hjphm);
    
    
    h3uxintia21 = (2.0*i3)*idx*((-23.0*i30)*hjmhp*hjmhp*hjmhp +  (-3.0*i10)*hjmhp*hjmhp*hjphm + (-3.0*i30)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
    h3uxintia22 = (2.0*i3)*idx*((14.0*i15)*hjmhp*hjmhp*hjmhp +  (6.0*i15)*hjmhp*hjmhp*hjphm + (6.0*i15)*hjmhp*hjphm*hjphm + (14.0*i15)*hjphm*hjphm*hjphm);        
    h3uxintia23 = (2.0*i3)*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i30)*hjmhp*hjmhp*hjphm + (-3.0*i10)*hjmhp*hjphm*hjphm + (-23.0*i30)*hjphm*hjphm*hjphm);
    
    h3uxintia31 = (2.0*i3)*idx*((13.0*i120)*hjmhp*hjmhp*hjmhp +  (-3.0*i120)*hjmhp*hjmhp*hjphm + (-3.0*i120)*hjmhp*hjphm*hjphm + (13.0*i120)*hjphm*hjphm*hjphm);       
    h3uxintia32 = (2.0*i3)*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i30)*hjmhp*hjmhp*hjphm + (-3.0*i10)*hjmhp*hjphm*hjphm + (-23.0*i30)*hjphm*hjphm*hjphm);        
    h3uxintia33 = (2.0*i3)*idx*((7.0*i120)*hjmhp*hjmhp*hjmhp +  (3.0*i24)*hjmhp*hjmhp*hjphm + (39.0*i120)*hjmhp*hjphm*hjphm + (79.0*i120)*hjphm*hjphm*hjphm);
    
    //h2 bx ux v
    h2bxuxva11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
    h2bxuxva12 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva13 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);

    h2bxuxva21 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
    h2bxuxva22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva23 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);

    h2bxuxva31 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva32 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);
    h2bxuxva33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);

    //h2 bx u vx
    h2bxuvxa11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
    h2bxuvxa12 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
    h2bxuvxa13 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);

    h2bxuvxa21 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
    h2bxuvxa22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
    h2bxuvxa23 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);

    h2bxuvxa31 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);
    h2bxuvxa32 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);
    h2bxuvxa33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);
                                    

    //h bx bx u v
    hbx2uva11 = 2*idx*((1333.0*i1344)*hjmhp*bjmh*bjmh+(443.0*i6720)*hjphm*bjmh*bjmh+(-3141.0*i2240)*hjmhp*bjms*bjmh+(-171.0*i2240)*hjphm*bjms*bjmh+(1161.0*i2240)*hjmhp*bjps*bjmh+(27.0*i2240)*hjphm*bjps*bjmh+(-145.0*i1344)*hjmhp*bjph*bjmh+(-11.0*i6720)*hjphm*bjph*bjmh+(-3141.0*i2240)*hjmhp*bjmh*bjms+(-171.0*i2240)*hjphm*bjmh*bjms+(4617.0*i2240)*hjmhp*bjms*bjms+(243.0*i2240)*hjphm*bjms*bjms+(-1863.0*i2240)*hjmhp*bjps*bjms+(-81.0*i2240)*hjphm*bjps*bjms+(387.0*i2240)*hjmhp*bjph*bjms+(9.0*i2240)*hjphm*bjph*bjms+(1161.0*i2240)*hjmhp*bjmh*bjps+(27.0*i2240)*hjphm*bjmh*bjps+(-1863.0*i2240)*hjmhp*bjms*bjps+(-81.0*i2240)*hjphm*bjms*bjps+(891.0*i2240)*hjmhp*bjps*bjps+(81.0*i2240)*hjphm*bjps*bjps+(-27.0*i320)*hjmhp*bjph*bjps+(-27.0*i2240)*hjphm*bjph*bjps+(-145.0*i1344)*hjmhp*bjmh*bjph+(-11.0*i6720)*hjphm*bjmh*bjph+(387.0*i2240)*hjmhp*bjms*bjph+(9.0*i2240)*hjphm*bjms*bjph+(-27.0*i320)*hjmhp*bjps*bjph+(-27.0*i2240)*hjphm*bjps*bjph+(131.0*i6720)*hjmhp*bjph*bjph+(13.0*i1344)*hjphm*bjph*bjph);
    hbx2uva12 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
    hbx2uva13 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);


    hbx2uva21 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
    hbx2uva22 = 2*idx*((101.0*i420)*hjmhp*bjmh*bjmh+(29.0*i420)*hjphm*bjmh*bjmh+(-6.0*i35)*hjmhp*bjms*bjmh+(-3.0*i35)*hjphm*bjms*bjmh+(-3.0*i28)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(4.0*i105)*hjmhp*bjph*bjmh+(4.0*i105)*hjphm*bjph*bjmh+(-6.0*i35)*hjmhp*bjmh*bjms+(-3.0*i35)*hjphm*bjmh*bjms+(27.0*i28)*hjmhp*bjms*bjms+(27.0*i28)*hjphm*bjms*bjms+(-27.0*i35)*hjmhp*bjps*bjms+(-27.0*i35)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(-3.0*i28)*hjphm*bjph*bjms+(-3.0*i28)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-27.0*i35)*hjmhp*bjms*bjps+(-27.0*i35)*hjphm*bjms*bjps+(27.0*i28)*hjmhp*bjps*bjps+(27.0*i28)*hjphm*bjps*bjps+(-3.0*i35)*hjmhp*bjph*bjps+(-6.0*i35)*hjphm*bjph*bjps+(4.0*i105)*hjmhp*bjmh*bjph+(4.0*i105)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(-3.0*i28)*hjphm*bjms*bjph+(-3.0*i35)*hjmhp*bjps*bjph+(-6.0*i35)*hjphm*bjps*bjph+(29.0*i420)*hjmhp*bjph*bjph+(101.0*i420)*hjphm*bjph*bjph);
    hbx2uva23 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);

    hbx2uva31 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);                
    hbx2uva32 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);
    hbx2uva33 = 2*idx*((13.0*i1344)*hjmhp*bjmh*bjmh+(131.0*i6720)*hjphm*bjmh*bjmh+(-27.0*i2240)*hjmhp*bjms*bjmh+(-27.0*i320)*hjphm*bjms*bjmh+(9.0*i2240)*hjmhp*bjps*bjmh+(387.0*i2240)*hjphm*bjps*bjmh+(-11.0*i6720)*hjmhp*bjph*bjmh+(-145.0*i1344)*hjphm*bjph*bjmh+(-27.0*i2240)*hjmhp*bjmh*bjms+(-27.0*i320)*hjphm*bjmh*bjms+(81.0*i2240)*hjmhp*bjms*bjms+(891.0*i2240)*hjphm*bjms*bjms+(-81.0*i2240)*hjmhp*bjps*bjms+(-1863.0*i2240)*hjphm*bjps*bjms+(27.0*i2240)*hjmhp*bjph*bjms+(1161.0*i2240)*hjphm*bjph*bjms+(9.0*i2240)*hjmhp*bjmh*bjps+(387.0*i2240)*hjphm*bjmh*bjps+(-81.0*i2240)*hjmhp*bjms*bjps+(-1863.0*i2240)*hjphm*bjms*bjps+(243.0*i2240)*hjmhp*bjps*bjps+(4617.0*i2240)*hjphm*bjps*bjps+(-171.0*i2240)*hjmhp*bjph*bjps+(-3141.0*i2240)*hjphm*bjph*bjps+(-11.0*i6720)*hjmhp*bjmh*bjph+(-145.0*i1344)*hjphm*bjmh*bjph+(27.0*i2240)*hjmhp*bjms*bjph+(1161.0*i2240)*hjphm*bjms*bjph+(-171.0*i2240)*hjmhp*bjps*bjph+(-3141.0*i2240)*hjphm*bjps*bjph+(443.0*i6720)*hjmhp*bjph*bjph+(1333.0*i1344)*hjphm*bjph*bjph);       

    // LHS 
    
    LHSa11 = uhintia11 + h3uxintia11 + h2bxuxva11 + h2bxuvxa11 + hbx2uva11;
    LHSa12 = uhintia12 + h3uxintia12 + h2bxuxva12 + h2bxuvxa12 + hbx2uva12; 
    LHSa13 = uhintia13 + h3uxintia13 + h2bxuxva13 + h2bxuvxa13 + hbx2uva13;
    LHSa21 = uhintia21 + h3uxintia21 + h2bxuxva21 + h2bxuvxa21 + hbx2uva21;
    LHSa22 = uhintia22 + h3uxintia22 + h2bxuxva22 + h2bxuvxa22 + hbx2uva22; 
    LHSa23 = uhintia23 + h3uxintia23 + h2bxuxva23 + h2bxuvxa23 + hbx2uva23;
    LHSa31 = uhintia31 + h3uxintia31 + h2bxuxva31 + h2bxuvxa31 + hbx2uva31;
    LHSa32 = uhintia32 + h3uxintia32 + h2bxuxva32 + h2bxuvxa32 + hbx2uva32;
    LHSa33 = uhintia33 + h3uxintia33 + h2bxuxva33 + h2bxuvxa33 + hbx2uva33;

    uais[j-1] = uais[j-1] + LHSa31;
    
    ubis[j-1] = ubis[j-1] + LHSa21;
    ubis[j] = ubis[j] + LHSa32;
    
    ucis[j-1] = 1;
    ucis[j] = ucis[j] + LHSa22;
    ucis[j+1] = ucis[j+1] + LHSa33;
    
    udis[j-1] = 0;
    udis[j] = udis[j] + LHSa23;
    
    ueis[j-1] = 0;
    
    
    nGis[j-1] = uMbeg[unBC-1];
    nGis[j] = nGis[j] + Gintia21; 
    nGis[j+1] = nGis[j+1] + Gintia31;     

    hhbc[3*i + (nGhBC)] = hjmhp;
    hhbc[3*i+1 + (nGhBC)] = fmin(0.5*(hjmhp +  hjphm),0);
    hhbc[3*i+2 + (nGhBC)] = hjphm; 

    whbc[3*i + (nGhBC)] = wjmhp;
    whbc[3*i+1 + (nGhBC)] = 0.5*(wjmhp +  wjphm);
    whbc[3*i+2 + (nGhBC)] = wjphm;

    Ghbc[3*i + (nGhBC)] = Gjmhp;
    Ghbc[3*i+1 + (nGhBC)] = 0.5*(Gjmhp +  Gjphm);
    Ghbc[3*i+2 + (nGhBC)] = Gjphm;

    bedhbc[4*i + (nbBC)] = bjmh;
    bedhbc[4*i+1 + (nbBC)] = bjms;
    bedhbc[4*i+2 + (nbBC)] = bjps;
    bedhbc[4*i+3 + (nbBC)] = bjph;

    bedhbc[0] = bMbeg[nbBC-4];
    bedhbc[1] = bMbeg[nbBC-3];
    bedhbc[2] = bMbeg[nbBC-2];
    bedhbc[3] = bMbeg[nbBC-1];
  

// last
    j = m-2;
    i = n-1;

    // Get it from interior
    // Reconstruct w
    wi = h[i] + bed[i];
 
    wjphm= wMend[0];
    wjmhp= whbc[nGhbc - nGhBC -4]; 

    Gjphm= GMend[0];
    Gjmhp= Ghbc[nGhbc - nGhBC -4];

    hjphm= fmin(hMend[0],0);
    hjmhp= hhbc[nGhbc - nGhBC -4];

    // reconstruct bed

    bedai = i6*idx*idx*idx*(-bed[i-3] +3*bed[i-2] -3*bed[i-1] + bed[i]); 
    bedbi = 0.5*idx*idx*(-bed[i-3] + 4*bed[i-2] - 5*bed[i-1] + 2*bed[i]);
    bedci = i6*idx*(-2*bed[i-3] + 9*bed[i-2] - 18*bed[i-1] + 11*bed[i]);
    beddi = bed[i];

    bjmh = -bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) - bedci*(0.5*dx) + beddi;
    bjms = -bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) - bedci*(dx*i6) + beddi;
    bjps = bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) + bedci*(dx*i6) + beddi;
    bjph = bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) + bedci*(0.5*dx) + beddi;

    // G integral (RHS)
    Gintia11 = dx*i6*(Gjmhp);
    Gintia21 = dx*i6*(2*Gjmhp + 2*Gjphm);
    Gintia31 = dx*i6*(Gjphm);
    
    
    //uh integral
    uhintia11 = (1.0 *i60)*dx*(7*hjmhp + hjphm);
    uhintia12 = (1.0 *i60)*dx*(4*hjmhp );
    uhintia13 = (1.0 *i60)*dx*(-hjmhp - hjphm);
    
    uhintia21 = (1.0 *i60)*dx*(4*hjmhp);
    uhintia22 = (1.0 *i60)*dx*(16*hjmhp + 16*hjphm);
    uhintia23 = (1.0 *i60)*dx*(4*hjphm);
    
    uhintia31 = (1.0 *i60)*dx*(-hjmhp - hjphm);
    uhintia32 = (1.0 *i60)*dx*(4*hjphm);
    uhintia33 = (1.0 *i60)*dx*(hjmhp + 7*hjphm);
    
    //h3ux
    
    h3uxintia11 = (2.0*i3)*idx*((79.0*i120)*hjmhp*hjmhp*hjmhp +  (39.0*i120)*hjmhp*hjmhp*hjphm + (3.0*i24)*hjmhp*hjphm*hjphm + (7.0*i120)*hjphm*hjphm*hjphm);        
    h3uxintia12 = (2.0*i3)*idx*((-23.0*i30)*hjmhp*hjmhp*hjmhp +  (-3.0*i10)*hjmhp*hjmhp*hjphm + (-3.0*i30)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
    h3uxintia13 = (2.0*i3)*idx*((13.0*i120)*hjmhp*hjmhp*hjmhp +  (-3.0*i120)*hjmhp*hjmhp*hjphm + (-3.0*i120)*hjmhp*hjphm*hjphm + (13.0*i120)*hjphm*hjphm*hjphm);
    
    
    h3uxintia21 = (2.0*i3)*idx*((-23.0*i30)*hjmhp*hjmhp*hjmhp +  (-3.0*i10)*hjmhp*hjmhp*hjphm + (-3.0*i30)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
    h3uxintia22 = (2.0*i3)*idx*((14.0*i15)*hjmhp*hjmhp*hjmhp +  (6.0*i15)*hjmhp*hjmhp*hjphm + (6.0*i15)*hjmhp*hjphm*hjphm + (14.0*i15)*hjphm*hjphm*hjphm);        
    h3uxintia23 = (2.0*i3)*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i30)*hjmhp*hjmhp*hjphm + (-3.0*i10)*hjmhp*hjphm*hjphm + (-23.0*i30)*hjphm*hjphm*hjphm);
    
    h3uxintia31 = (2.0*i3)*idx*((13.0*i120)*hjmhp*hjmhp*hjmhp +  (-3.0*i120)*hjmhp*hjmhp*hjphm + (-3.0*i120)*hjmhp*hjphm*hjphm + (13.0*i120)*hjphm*hjphm*hjphm);       
    h3uxintia32 = (2.0*i3)*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i30)*hjmhp*hjmhp*hjphm + (-3.0*i10)*hjmhp*hjphm*hjphm + (-23.0*i30)*hjphm*hjphm*hjphm);        
    h3uxintia33 = (2.0*i3)*idx*((7.0*i120)*hjmhp*hjmhp*hjmhp +  (3.0*i24)*hjmhp*hjmhp*hjphm + (39.0*i120)*hjmhp*hjphm*hjphm + (79.0*i120)*hjphm*hjphm*hjphm);
    
    //h2 bx ux v
    h2bxuxva11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
    h2bxuxva12 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva13 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);

    h2bxuxva21 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
    h2bxuxva22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva23 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);

    h2bxuxva31 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva32 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);
    h2bxuxva33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);

    //h2 bx u vx
    h2bxuvxa11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
    h2bxuvxa12 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
    h2bxuvxa13 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);

    h2bxuvxa21 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
    h2bxuvxa22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
    h2bxuvxa23 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);

    h2bxuvxa31 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);
    h2bxuvxa32 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);
    h2bxuvxa33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);
                                    

    //h bx bx u v
    hbx2uva11 = 2*idx*((1333.0*i1344)*hjmhp*bjmh*bjmh+(443.0*i6720)*hjphm*bjmh*bjmh+(-3141.0*i2240)*hjmhp*bjms*bjmh+(-171.0*i2240)*hjphm*bjms*bjmh+(1161.0*i2240)*hjmhp*bjps*bjmh+(27.0*i2240)*hjphm*bjps*bjmh+(-145.0*i1344)*hjmhp*bjph*bjmh+(-11.0*i6720)*hjphm*bjph*bjmh+(-3141.0*i2240)*hjmhp*bjmh*bjms+(-171.0*i2240)*hjphm*bjmh*bjms+(4617.0*i2240)*hjmhp*bjms*bjms+(243.0*i2240)*hjphm*bjms*bjms+(-1863.0*i2240)*hjmhp*bjps*bjms+(-81.0*i2240)*hjphm*bjps*bjms+(387.0*i2240)*hjmhp*bjph*bjms+(9.0*i2240)*hjphm*bjph*bjms+(1161.0*i2240)*hjmhp*bjmh*bjps+(27.0*i2240)*hjphm*bjmh*bjps+(-1863.0*i2240)*hjmhp*bjms*bjps+(-81.0*i2240)*hjphm*bjms*bjps+(891.0*i2240)*hjmhp*bjps*bjps+(81.0*i2240)*hjphm*bjps*bjps+(-27.0*i320)*hjmhp*bjph*bjps+(-27.0*i2240)*hjphm*bjph*bjps+(-145.0*i1344)*hjmhp*bjmh*bjph+(-11.0*i6720)*hjphm*bjmh*bjph+(387.0*i2240)*hjmhp*bjms*bjph+(9.0*i2240)*hjphm*bjms*bjph+(-27.0*i320)*hjmhp*bjps*bjph+(-27.0*i2240)*hjphm*bjps*bjph+(131.0*i6720)*hjmhp*bjph*bjph+(13.0*i1344)*hjphm*bjph*bjph);
    hbx2uva12 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
    hbx2uva13 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);


    hbx2uva21 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
    hbx2uva22 = 2*idx*((101.0*i420)*hjmhp*bjmh*bjmh+(29.0*i420)*hjphm*bjmh*bjmh+(-6.0*i35)*hjmhp*bjms*bjmh+(-3.0*i35)*hjphm*bjms*bjmh+(-3.0*i28)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(4.0*i105)*hjmhp*bjph*bjmh+(4.0*i105)*hjphm*bjph*bjmh+(-6.0*i35)*hjmhp*bjmh*bjms+(-3.0*i35)*hjphm*bjmh*bjms+(27.0*i28)*hjmhp*bjms*bjms+(27.0*i28)*hjphm*bjms*bjms+(-27.0*i35)*hjmhp*bjps*bjms+(-27.0*i35)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(-3.0*i28)*hjphm*bjph*bjms+(-3.0*i28)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-27.0*i35)*hjmhp*bjms*bjps+(-27.0*i35)*hjphm*bjms*bjps+(27.0*i28)*hjmhp*bjps*bjps+(27.0*i28)*hjphm*bjps*bjps+(-3.0*i35)*hjmhp*bjph*bjps+(-6.0*i35)*hjphm*bjph*bjps+(4.0*i105)*hjmhp*bjmh*bjph+(4.0*i105)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(-3.0*i28)*hjphm*bjms*bjph+(-3.0*i35)*hjmhp*bjps*bjph+(-6.0*i35)*hjphm*bjps*bjph+(29.0*i420)*hjmhp*bjph*bjph+(101.0*i420)*hjphm*bjph*bjph);
    hbx2uva23 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);

    hbx2uva31 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);                
    hbx2uva32 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);
    hbx2uva33 = 2*idx*((13.0*i1344)*hjmhp*bjmh*bjmh+(131.0*i6720)*hjphm*bjmh*bjmh+(-27.0*i2240)*hjmhp*bjms*bjmh+(-27.0*i320)*hjphm*bjms*bjmh+(9.0*i2240)*hjmhp*bjps*bjmh+(387.0*i2240)*hjphm*bjps*bjmh+(-11.0*i6720)*hjmhp*bjph*bjmh+(-145.0*i1344)*hjphm*bjph*bjmh+(-27.0*i2240)*hjmhp*bjmh*bjms+(-27.0*i320)*hjphm*bjmh*bjms+(81.0*i2240)*hjmhp*bjms*bjms+(891.0*i2240)*hjphm*bjms*bjms+(-81.0*i2240)*hjmhp*bjps*bjms+(-1863.0*i2240)*hjphm*bjps*bjms+(27.0*i2240)*hjmhp*bjph*bjms+(1161.0*i2240)*hjphm*bjph*bjms+(9.0*i2240)*hjmhp*bjmh*bjps+(387.0*i2240)*hjphm*bjmh*bjps+(-81.0*i2240)*hjmhp*bjms*bjps+(-1863.0*i2240)*hjphm*bjms*bjps+(243.0*i2240)*hjmhp*bjps*bjps+(4617.0*i2240)*hjphm*bjps*bjps+(-171.0*i2240)*hjmhp*bjph*bjps+(-3141.0*i2240)*hjphm*bjph*bjps+(-11.0*i6720)*hjmhp*bjmh*bjph+(-145.0*i1344)*hjphm*bjmh*bjph+(27.0*i2240)*hjmhp*bjms*bjph+(1161.0*i2240)*hjphm*bjms*bjph+(-171.0*i2240)*hjmhp*bjps*bjph+(-3141.0*i2240)*hjphm*bjps*bjph+(443.0*i6720)*hjmhp*bjph*bjph+(1333.0*i1344)*hjphm*bjph*bjph);       

    // LHS 
    
    LHSa11 = uhintia11 + h3uxintia11 + h2bxuxva11 + h2bxuvxa11 + hbx2uva11;
    LHSa12 = uhintia12 + h3uxintia12 + h2bxuxva12 + h2bxuvxa12 + hbx2uva12; 
    LHSa13 = uhintia13 + h3uxintia13 + h2bxuxva13 + h2bxuvxa13 + hbx2uva13;
    LHSa21 = uhintia21 + h3uxintia21 + h2bxuxva21 + h2bxuvxa21 + hbx2uva21;
    LHSa22 = uhintia22 + h3uxintia22 + h2bxuxva22 + h2bxuvxa22 + hbx2uva22; 
    LHSa23 = uhintia23 + h3uxintia23 + h2bxuxva23 + h2bxuvxa23 + hbx2uva23;
    LHSa31 = uhintia31 + h3uxintia31 + h2bxuxva31 + h2bxuvxa31 + hbx2uva31;
    LHSa32 = uhintia32 + h3uxintia32 + h2bxuxva32 + h2bxuvxa32 + hbx2uva32;
    LHSa33 = uhintia33 + h3uxintia33 + h2bxuxva33 + h2bxuvxa33 + hbx2uva33;
    
    uais[j-1] = 0;
    
    ubis[j-1] = ubis[j-1] + LHSa21;
    ubis[j] = 0;
    
    ucis[j-1] = ucis[j-1] + LHSa11;
    ucis[j] = ucis[j] + LHSa22;
    ucis[j+1] = 1;
    
    udis[j-1] = udis[j-1] + LHSa12;
    udis[j] = udis[j] + LHSa23;
    
    ueis[j-1] = ueis[j-1] + LHSa13;
    
    
    nGis[j-1] = nGis[j-1] + Gintia11; 
    nGis[j] = nGis[j] + Gintia21; 
    nGis[j+1] = uMend[0];


    hhbc[3*i + (nGhBC)] = hjmhp;
    hhbc[3*i+1 + (nGhBC)] = fmin(0.5*(hjmhp + hjphm),0);
    hhbc[3*i+2 + (nGhBC)] = hjphm; 

    whbc[3*i + (nGhBC)] = wjmhp;
    whbc[3*i+1 + (nGhBC)] = 0.5*(wjmhp + wjphm);
    whbc[3*i+2 + (nGhBC)] = wjphm;

    Ghbc[3*i + (nGhBC)] = Gjmhp;
    Ghbc[3*i+1 + (nGhBC)] = 0.5*(Gjmhp + Gjphm);
    Ghbc[3*i+2 + (nGhBC)] = Gjphm;

    bedhbc[4*i + (nbBC)] = bjmh;
    bedhbc[4*i+1 + (nbBC)] = bjms;
    bedhbc[4*i+2 + (nbBC)] = bjps;
    bedhbc[4*i+3 + (nbBC)] = bjph;

    bedhbc[4*(i+1) + (nbBC)] = bMend[0];
    bedhbc[4*(i+1) + 1 + (nbBC)] = bMend[1];
    bedhbc[4*(i+1) + 2 + (nbBC)] = bMend[2];
    bedhbc[4*(i+1) + 3 + (nbBC)] = bMend[3];

    PENT(uais,ubis,ucis,udis,ueis,nGis,m,utemp); 
    memcpy(u,uMbeg, (unBC-1)*sizeof(double));
    memcpy(u+(unBC-1),utemp,m*sizeof(double));
    memcpy(u+nubc - (unBC-1),uMend + 1,(unBC-1)*sizeof(double));

    // B.C stuff
    for(i=0;i < nGhBC;i++)
    {
        whbc[i] = wMbeg[i];
        whbc[nGhbc-nGhBC + i] = wMend[i];
        hhbc[i] = fmin(hMbeg[i],0);
        hhbc[nGhbc-nGhBC + i] = fmin(hMend[i],0);
        Ghbc[i] = GMbeg[i];
        Ghbc[nGhbc-nGhBC +i] = GMend[i];

        //printf("Beg: %d | %f | %f | %f \n",i,wMbeg[i],hMbeg[i],GMbeg[i]);
        //printf("end: %d | %f | %f | %f \n",i,wMend[i],hMend[i],GMend[i]);
    }

    free(uais);
    free(ubis);
    free(ucis);
    free(udis);
    free(ueis);
    free(nGis);
    free(utemp);

}

void getufromGBAND(double *h, double *G, double *bed, double *hMbeg, double *hMend, double *wMbeg, double *wMend, double *GMbeg, double *GMend,double *uMbeg, double *uMend, double *bMbeg, double *bMend, double theta, double dx , int n, int m, int nGhBC,int unBC,int nbBC, int nGhbc, int nubc, int nbhc, double *u, double *hhbc, double *whbc,double *Ghbc, double *bedhbc)
{
    //trying to join different B.C systems...., also try incorporating into  C code that already has beds, like o2bedfix.

    // n is length of h, G
    //nu is length of u (should be n + 1, to count edges of n cells)

    //enforcing B.Cs at cell edges now

    double idx = 1.0/ dx;
    //double *uais = malloc((m-2)*sizeof(double));
    //double *ubis = malloc((m-1)*sizeof(double));
    //double *ucis = malloc((m)*sizeof(double));
    //double *udis = malloc((m-1)*sizeof(double));
    //double *ueis = malloc((m-2)*sizeof(double));

    double **AL, **Ared;
    unsigned long *indx;

    Ared = dmatrix(1, m, 1, 5);
    AL = dmatrix(1, m, 1, 5);
    indx = mallocLongPy(m);

    double *nGis = malloc((m)*sizeof(double));
    //double *utemp = malloc((m)*sizeof(double));

    int i,j;

    double dGib,dGim,dGif,dhib,dhim,dhif,dGi,dhi,Gjphm,Gjmhp,hjphm,hjmhp,bedai,bedbi,bedci,beddi,bjmh,bjms,bjps,bjph;

    double Gintia11,Gintia21,Gintia31,uhintia11,uhintia12,uhintia13,uhintia21,uhintia22,uhintia23,uhintia31,uhintia32,uhintia33;
    double h3uxintia11,h3uxintia12,h3uxintia13,h3uxintia21,h3uxintia22,h3uxintia23,h3uxintia31,h3uxintia32,h3uxintia33;
    double h2bxuxva11,h2bxuxva12,h2bxuxva13,h2bxuxva21,h2bxuxva22,h2bxuxva23,h2bxuxva31,h2bxuxva32,h2bxuxva33;
    double h2bxuvxa11,h2bxuvxa12,h2bxuvxa13,h2bxuvxa21,h2bxuvxa22,h2bxuvxa23,h2bxuvxa31,h2bxuvxa32,h2bxuvxa33;
    double hbx2uva11,hbx2uva12,hbx2uva13,hbx2uva21,hbx2uva22,hbx2uva23,hbx2uva31,hbx2uva32,hbx2uva33;
    double LHSa11,LHSa12,LHSa13,LHSa21,LHSa22,LHSa23,LHSa31,LHSa32,LHSa33;
    double wim1, wi,wip1,dwim,dwif,dwi,dwib,wjmhp,wjphm;
    double bedbegmiddle,bedendmiddle;

    // Zero out the diagonal arrays
    for(i = 0; i < m ; i++)
    {
        Ared[i + 1][1] = 0;
        Ared[i + 1][2] = 0;
        Ared[i + 1][3] = 0;
        Ared[i + 1][4] = 0;
        Ared[i + 1][5] = 0;

        nGis[i] = 0;
    
    }



    j = 3;
    for (i =1;i < n-1 ; i++)
    {
        // Reconstruct G and h
        dGib = (G[i] - G[i-1]);
        dGim = 0.5*(G[i+1] - G[i-1]);
        dGif = (G[i+1] - G[i]);

        dhib = (h[i] - h[i-1]);
        dhim = 0.5*(h[i+1] - h[i-1]);
        dhif = (h[i+1] - h[i]);

        wi = fmax(h[i] + bed[i] ,bed[i]);
        wip1 = fmax(h[i+1] + bed[i+1],bed[i +1]);
        wim1 = fmax(h[i-1] + bed[i-1],bed[i + 1]);
        dwib = (wi - wim1);
        dwim = 0.5*(wip1 - wim1);
        dwif = (wip1 - wi);

        dGi = minmod(theta*dGib, dGim, theta*dGif);
        dhi = minmod(theta*dhib, dhim, theta*dhif);
        dwi = minmod(theta*dwib, dwim, theta*dwif);

        wjphm= wi + 0.5*dwi;
        wjmhp= wi - 0.5*dwi; 

        hjphm=  fmax(h[i] + 0.5*dhi,0);
        hjmhp= fmax(h[i] - 0.5*dhi,0); 

        Gjphm= (hjphm > 0)*(G[i] + 0.5*dGi);
        Gjmhp= (hjmhp > 0)*(G[i] - 0.5*dGi);    

        // reconstruct bed
        if(i == 1) 
        {
            bedbegmiddle = -bMbeg[0]*i16 + 9*bMbeg[1]*i16 + 9*bMbeg[2]*i16 - bMbeg[3]*i16;

            bedai = i12*idx*idx*idx*(-bedbegmiddle + 2*bed[i-1] -2*bed[i+1] + bed[i+2]);
            bedbi = i6*idx*idx*(bedbegmiddle - bed[i-1] - bed[i+1] + bed[i+2]);
            bedci = i12*idx*(bedbegmiddle - 8*bed[i-1] + 8*bed[i+1] - bed[i+2]);
            beddi = -bedbegmiddle*i6 + 2*i3*bed[i-1] + 2*i3*bed[i+1] - bed[i+2]*i6;
        }
        else if(i == n-2)
        {
            bedendmiddle = -bMend[0]*i16 + 9*bMend[1]*i16 + 9*bMend[2]*i16 - bMend[3]*i16;

            bedai = i12*idx*idx*idx*(-bed[i-2] + 2*bed[i-1] -2*bed[i+1] + bedendmiddle);
            bedbi = i6*idx*idx*(bed[i-2] - bed[i-1] - bed[i+1] + bedendmiddle);
            bedci = i12*idx*(bed[i-2] - 8*bed[i-1] + 8*bed[i+1] - bedendmiddle);
            beddi = -bed[i-2]*i6 + 2*i3*bed[i-1] + 2*i3*bed[i+1] - bedendmiddle*i6;

        }
        else
        {
            bedai = i12*idx*idx*idx*(-bed[i-2] + 2*bed[i-1] -2*bed[i+1] + bed[i+2]);
            bedbi = i6*idx*idx*(bed[i-2] - bed[i-1] - bed[i+1] + bed[i+2]);
            bedci = i12*idx*(bed[i-2] - 8*bed[i-1] + 8*bed[i+1] - bed[i+2]);
            beddi = -bed[i-2]*i6 + 2*i3*bed[i-1] + 2*i3*bed[i+1] - bed[i+2]*i6;

        }

        bjmh = -bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) - bedci*(0.5*dx) + beddi;
        bjms = -bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) - bedci*(dx*i6) + beddi;
        bjps = bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) + bedci*(dx*i6) + beddi;
        bjph = bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) + bedci*(0.5*dx) + beddi;

        // G integral (RHS)
        Gintia11 = i6*dx*(Gjmhp);
        Gintia21 = i6*dx*(2*Gjmhp + 2*Gjphm);
        Gintia31 = i6*dx*(Gjphm);
        
        
        //uh integral
        uhintia11 = 0.1*i6*dx*(7*hjmhp + hjphm);
        uhintia12 = 0.1*i6*dx*(4*hjmhp );
        uhintia13 = 0.1*i6*dx*(-hjmhp - hjphm);
        
        uhintia21 = 0.1*i6*dx*(4*hjmhp);
        uhintia22 = 0.1*i6*dx*(16*hjmhp + 16*hjphm);
        uhintia23 = 0.1*i6*dx*(4*hjphm);
        
        uhintia31 = 0.1*i6*dx*(-hjmhp - hjphm);
        uhintia32 = 0.1*i6*dx*(4*hjphm);
        uhintia33 = 0.1*i6*dx*(hjmhp + 7*hjphm);
        
        //h3ux
        
        h3uxintia11 = 2*i3*idx*((79.0*0.1*i12)*hjmhp*hjmhp*hjmhp +  (39.0*0.1*i12)*hjmhp*hjmhp*hjphm + (3.0*0.5*i12)*hjmhp*hjphm*hjphm + (7.0*0.1*i12)*hjphm*hjphm*hjphm);        
        h3uxintia12 = 2*i3*idx*((-23.0*i3*0.1)*hjmhp*hjmhp*hjmhp +  (-3.0*0.1)*hjmhp*hjmhp*hjphm + (-3.0*i3*0.1)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
        h3uxintia13 = 2*i3*idx*((13.0*0.1*i12)*hjmhp*hjmhp*hjmhp +  (-3.0*0.1*i12)*hjmhp*hjmhp*hjphm + (-3.0*0.1*i12)*hjmhp*hjphm*hjphm + (13.0*0.1*i12)*hjphm*hjphm*hjphm);
        
        
        h3uxintia21 = 2*i3*idx*((-23.0*i3*0.1)*hjmhp*hjmhp*hjmhp +  (-3.0*0.1)*hjmhp*hjmhp*hjphm + (-3.0*0.1*i3)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
        h3uxintia22 = 2*i3*idx*((14.0*i3*i5)*hjmhp*hjmhp*hjmhp +  (6.0*i3*i5)*hjmhp*hjmhp*hjphm + (6.0*i3*i5)*hjmhp*hjphm*hjphm + (14.0*i3*i5)*hjphm*hjphm*hjphm);        
        h3uxintia23 = 2*i3*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i3*0.1)*hjmhp*hjmhp*hjphm + (-3.0*0.1)*hjmhp*hjphm*hjphm + (-23.0*i3*0.1)*hjphm*hjphm*hjphm);
        
        h3uxintia31 = 2*i3*idx*((13.0*0.1*i12)*hjmhp*hjmhp*hjmhp +  (-3.0*0.1*i12)*hjmhp*hjmhp*hjphm + (-3.0*0.1*i12)*hjmhp*hjphm*hjphm + (13.0*0.1*i12)*hjphm*hjphm*hjphm);       
        h3uxintia32 = 2*i3*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i3*0.1)*hjmhp*hjmhp*hjphm + (-3.0*0.1)*hjmhp*hjphm*hjphm + (-23.0*i3*0.1)*hjphm*hjphm*hjphm);        
        h3uxintia33 = 2*i3*idx*((7.0*0.1*i12)*hjmhp*hjmhp*hjmhp +  (3.0*0.5*i12)*hjmhp*hjmhp*hjphm + (39.0*0.1*i12)*hjmhp*hjphm*hjphm + (79.0*0.1*i12)*hjphm*hjphm*hjphm);
        
        //h2 bx ux v
        h2bxuxva11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
        h2bxuxva12 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
        h2bxuxva13 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);

        h2bxuxva21 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
        h2bxuxva22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
        h2bxuxva23 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);

        h2bxuxva31 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);
        h2bxuxva32 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);
        h2bxuxva33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);

        //h2 bx u vx
        h2bxuvxa11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
        h2bxuvxa12 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
        h2bxuvxa13 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);

        h2bxuvxa21 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
        h2bxuvxa22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
        h2bxuvxa23 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);

        h2bxuvxa31 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);
        h2bxuvxa32 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);
        h2bxuvxa33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);
                                        

        //h bx bx u v
        hbx2uva11 = 2*idx*((1333.0*i1344)*hjmhp*bjmh*bjmh+(443.0*i6720)*hjphm*bjmh*bjmh+(-3141.0*i2240)*hjmhp*bjms*bjmh+(-171.0*i2240)*hjphm*bjms*bjmh+(1161.0*i2240)*hjmhp*bjps*bjmh+(27.0*i2240)*hjphm*bjps*bjmh+(-145.0*i1344)*hjmhp*bjph*bjmh+(-11.0*i6720)*hjphm*bjph*bjmh+(-3141.0*i2240)*hjmhp*bjmh*bjms+(-171.0*i2240)*hjphm*bjmh*bjms+(4617.0*i2240)*hjmhp*bjms*bjms+(243.0*i2240)*hjphm*bjms*bjms+(-1863.0*i2240)*hjmhp*bjps*bjms+(-81.0*i2240)*hjphm*bjps*bjms+(387.0*i2240)*hjmhp*bjph*bjms+(9.0*i2240)*hjphm*bjph*bjms+(1161.0*i2240)*hjmhp*bjmh*bjps+(27.0*i2240)*hjphm*bjmh*bjps+(-1863.0*i2240)*hjmhp*bjms*bjps+(-81.0*i2240)*hjphm*bjms*bjps+(891.0*i2240)*hjmhp*bjps*bjps+(81.0*i2240)*hjphm*bjps*bjps+(-27.0*i320)*hjmhp*bjph*bjps+(-27.0*i2240)*hjphm*bjph*bjps+(-145.0*i1344)*hjmhp*bjmh*bjph+(-11.0*i6720)*hjphm*bjmh*bjph+(387.0*i2240)*hjmhp*bjms*bjph+(9.0*i2240)*hjphm*bjms*bjph+(-27.0*i320)*hjmhp*bjps*bjph+(-27.0*i2240)*hjphm*bjps*bjph+(131.0*i6720)*hjmhp*bjph*bjph+(13.0*i1344)*hjphm*bjph*bjph);
        hbx2uva12 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
        hbx2uva13 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);


        hbx2uva21 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
        hbx2uva22 = 2*idx*((101.0*i420)*hjmhp*bjmh*bjmh+(29.0*i420)*hjphm*bjmh*bjmh+(-6.0*i35)*hjmhp*bjms*bjmh+(-3.0*i35)*hjphm*bjms*bjmh+(-3.0*i28)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(4.0*i105)*hjmhp*bjph*bjmh+(4.0*i105)*hjphm*bjph*bjmh+(-6.0*i35)*hjmhp*bjmh*bjms+(-3.0*i35)*hjphm*bjmh*bjms+(27.0*i28)*hjmhp*bjms*bjms+(27.0*i28)*hjphm*bjms*bjms+(-27.0*i35)*hjmhp*bjps*bjms+(-27.0*i35)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(-3.0*i28)*hjphm*bjph*bjms+(-3.0*i28)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-27.0*i35)*hjmhp*bjms*bjps+(-27.0*i35)*hjphm*bjms*bjps+(27.0*i28)*hjmhp*bjps*bjps+(27.0*i28)*hjphm*bjps*bjps+(-3.0*i35)*hjmhp*bjph*bjps+(-6.0*i35)*hjphm*bjph*bjps+(4.0*i105)*hjmhp*bjmh*bjph+(4.0*i105)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(-3.0*i28)*hjphm*bjms*bjph+(-3.0*i35)*hjmhp*bjps*bjph+(-6.0*i35)*hjphm*bjps*bjph+(29.0*i420)*hjmhp*bjph*bjph+(101.0*i420)*hjphm*bjph*bjph);
        hbx2uva23 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);

        hbx2uva31 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);                
        hbx2uva32 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);
        hbx2uva33 = 2*idx*((13.0*i1344)*hjmhp*bjmh*bjmh+(131.0*i6720)*hjphm*bjmh*bjmh+(-27.0*i2240)*hjmhp*bjms*bjmh+(-27.0*i320)*hjphm*bjms*bjmh+(9.0*i2240)*hjmhp*bjps*bjmh+(387.0*i2240)*hjphm*bjps*bjmh+(-11.0*i6720)*hjmhp*bjph*bjmh+(-145.0*i1344)*hjphm*bjph*bjmh+(-27.0*i2240)*hjmhp*bjmh*bjms+(-27.0*i320)*hjphm*bjmh*bjms+(81.0*i2240)*hjmhp*bjms*bjms+(891.0*i2240)*hjphm*bjms*bjms+(-81.0*i2240)*hjmhp*bjps*bjms+(-1863.0*i2240)*hjphm*bjps*bjms+(27.0*i2240)*hjmhp*bjph*bjms+(1161.0*i2240)*hjphm*bjph*bjms+(9.0*i2240)*hjmhp*bjmh*bjps+(387.0*i2240)*hjphm*bjmh*bjps+(-81.0*i2240)*hjmhp*bjms*bjps+(-1863.0*i2240)*hjphm*bjms*bjps+(243.0*i2240)*hjmhp*bjps*bjps+(4617.0*i2240)*hjphm*bjps*bjps+(-171.0*i2240)*hjmhp*bjph*bjps+(-3141.0*i2240)*hjphm*bjph*bjps+(-11.0*i6720)*hjmhp*bjmh*bjph+(-145.0*i1344)*hjphm*bjmh*bjph+(27.0*i2240)*hjmhp*bjms*bjph+(1161.0*i2240)*hjphm*bjms*bjph+(-171.0*i2240)*hjmhp*bjps*bjph+(-3141.0*i2240)*hjphm*bjps*bjph+(443.0*i6720)*hjmhp*bjph*bjph+(1333.0*i1344)*hjphm*bjph*bjph);       

        // LHS 
        
        LHSa11 = uhintia11 + h3uxintia11 + h2bxuxva11 + h2bxuvxa11 + hbx2uva11;
        LHSa12 = uhintia12 + h3uxintia12 + h2bxuxva12 + h2bxuvxa12 + hbx2uva12; 
        LHSa13 = uhintia13 + h3uxintia13 + h2bxuxva13 + h2bxuvxa13 + hbx2uva13;
        LHSa21 = uhintia21 + h3uxintia21 + h2bxuxva21 + h2bxuvxa21 + hbx2uva21;
        LHSa22 = uhintia22 + h3uxintia22 + h2bxuxva22 + h2bxuvxa22 + hbx2uva22; 
        LHSa23 = uhintia23 + h3uxintia23 + h2bxuxva23 + h2bxuvxa23 + hbx2uva23;
        LHSa31 = uhintia31 + h3uxintia31 + h2bxuxva31 + h2bxuvxa31 + hbx2uva31;
        LHSa32 = uhintia32 + h3uxintia32 + h2bxuxva32 + h2bxuvxa32 + hbx2uva32;
        LHSa33 = uhintia33 + h3uxintia33 + h2bxuxva33 + h2bxuvxa33 + hbx2uva33;

        Ared[j-1 + 3][1] = Ared[j-1 + 3][1] + LHSa31;

        Ared[j-1 +2][2] = Ared[j-1 + 2][2] + LHSa21;
        Ared[j + 2][2] = Ared[j + 2][2] + LHSa32;

        Ared[j-1 + 1][3] = Ared[j-1 + 1][3] + LHSa11;
        Ared[j + 1][3] = Ared[j + 1][3] + LHSa22;
        Ared[j+1 + 1][3] = Ared[j+1 + 1][3] + LHSa33;

        Ared[j-1 + 1][4] = Ared[j-1 + 1][4] + LHSa12;
        Ared[j + 1][4] = Ared[j + 1][4] + LHSa23;
        
        Ared[j-1 + 1 ][5] = Ared[j-1 + 1][5] + LHSa13;


        nGis[j-1] = nGis[j-1] + Gintia11;
        nGis[j] = nGis[j] + Gintia21; 
        nGis[j+1] = nGis[j+1] + Gintia31; 


        hhbc[3*i + (nGhBC)] = hjmhp;
        hhbc[3*i+1 + (nGhBC)] = fmax(h[i],0);
        hhbc[3*i+2 + (nGhBC)] = hjphm; 

        whbc[3*i + (nGhBC)] = wjmhp;
        whbc[3*i+1 + (nGhBC)] = wi;
        whbc[3*i+2 + (nGhBC)] = wjphm;

        Ghbc[3*i + (nGhBC)] = Gjmhp;
        Ghbc[3*i+1 + (nGhBC)] = (h[i] > 0)*G[i];
        Ghbc[3*i+2 + (nGhBC)] = Gjphm;

        bedhbc[4*i + (nbBC)] = bjmh;
        bedhbc[4*i+1 + (nbBC)] = bjms;
        bedhbc[4*i+2 + (nbBC)] = bjps;
        bedhbc[4*i+3 + (nbBC)] = bjph;
        
        
        j = j + 2 ;       


    }


    // first 
    i = 0;
    j = 1;

    // Get it from interior
    // Reconstruct w
    wi = fmax(h[i] + bed[i],bed[i]); 

    // Reconstruct G and h
    hjphm= hhbc[i + 2*nGhBC] ;
    hjmhp= fmax(hMbeg[nGhBC-1],0);

    Gjphm= (hjphm > 0)*Ghbc[i + 2*nGhBC];
    Gjmhp= (hjmhp > 0)*GMbeg[nGhBC-1];

    // reconstruct bed
    bedai = i6*idx*idx*idx*(bed[i+3] -3*bed[i+2] +3*bed[i+1] - bed[i]);
    bedbi = 0.5*idx*idx*(-bed[i+3] + 4*bed[i+2] - 5*bed[i+1] + 2*bed[i]);
    bedci = i6*idx*(2*bed[i+3] - 9*bed[i+2] + 18*bed[i+1] - 11*bed[i]);
    beddi = bed[i];

    bjmh = -bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) - bedci*(0.5*dx) + beddi;
    bjms = -bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) - bedci*(dx*i6) + beddi;
    bjps = bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) + bedci*(dx*i6) + beddi;
    bjph = bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) + bedci*(0.5*dx) + beddi;

    wjphm= fmax(whbc[i + 2*nGhBC], bjph);
    wjmhp= fmax(wMbeg[nGhBC-1], bjmh);

    // G integral (RHS)
    Gintia11 = dx*i6*(Gjmhp);
    Gintia21 = dx*i6*(2*Gjmhp + 2*Gjphm);
    Gintia31 = dx*i6*(Gjphm);
    
    
    //uh integral
    uhintia11 = (1.0 *i60)*dx*(7*hjmhp + hjphm);
    uhintia12 = (1.0 *i60)*dx*(4*hjmhp );
    uhintia13 = (1.0 *i60)*dx*(-hjmhp - hjphm);
    
    uhintia21 = (1.0 *i60)*dx*(4*hjmhp);
    uhintia22 = (1.0 *i60)*dx*(16*hjmhp + 16*hjphm);
    uhintia23 = (1.0 *i60)*dx*(4*hjphm);
    
    uhintia31 = (1.0 *i60)*dx*(-hjmhp - hjphm);
    uhintia32 = (1.0 *i60)*dx*(4*hjphm);
    uhintia33 = (1.0 *i60)*dx*(hjmhp + 7*hjphm);
    
    //h3ux
    
    h3uxintia11 = (2.0*i3)*idx*((79.0*i120)*hjmhp*hjmhp*hjmhp +  (39.0*i120)*hjmhp*hjmhp*hjphm + (3.0*i24)*hjmhp*hjphm*hjphm + (7.0*i120)*hjphm*hjphm*hjphm);        
    h3uxintia12 = (2.0*i3)*idx*((-23.0*i30)*hjmhp*hjmhp*hjmhp +  (-3.0*i10)*hjmhp*hjmhp*hjphm + (-3.0*i30)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
    h3uxintia13 = (2.0*i3)*idx*((13.0*i120)*hjmhp*hjmhp*hjmhp +  (-3.0*i120)*hjmhp*hjmhp*hjphm + (-3.0*i120)*hjmhp*hjphm*hjphm + (13.0*i120)*hjphm*hjphm*hjphm);
    
    
    h3uxintia21 = (2.0*i3)*idx*((-23.0*i30)*hjmhp*hjmhp*hjmhp +  (-3.0*i10)*hjmhp*hjmhp*hjphm + (-3.0*i30)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
    h3uxintia22 = (2.0*i3)*idx*((14.0*i15)*hjmhp*hjmhp*hjmhp +  (6.0*i15)*hjmhp*hjmhp*hjphm + (6.0*i15)*hjmhp*hjphm*hjphm + (14.0*i15)*hjphm*hjphm*hjphm);        
    h3uxintia23 = (2.0*i3)*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i30)*hjmhp*hjmhp*hjphm + (-3.0*i10)*hjmhp*hjphm*hjphm + (-23.0*i30)*hjphm*hjphm*hjphm);
    
    h3uxintia31 = (2.0*i3)*idx*((13.0*i120)*hjmhp*hjmhp*hjmhp +  (-3.0*i120)*hjmhp*hjmhp*hjphm + (-3.0*i120)*hjmhp*hjphm*hjphm + (13.0*i120)*hjphm*hjphm*hjphm);       
    h3uxintia32 = (2.0*i3)*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i30)*hjmhp*hjmhp*hjphm + (-3.0*i10)*hjmhp*hjphm*hjphm + (-23.0*i30)*hjphm*hjphm*hjphm);        
    h3uxintia33 = (2.0*i3)*idx*((7.0*i120)*hjmhp*hjmhp*hjmhp +  (3.0*i24)*hjmhp*hjmhp*hjphm + (39.0*i120)*hjmhp*hjphm*hjphm + (79.0*i120)*hjphm*hjphm*hjphm);
    
    //h2 bx ux v
    h2bxuxva11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
    h2bxuxva12 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva13 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);

    h2bxuxva21 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
    h2bxuxva22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva23 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);

    h2bxuxva31 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva32 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);
    h2bxuxva33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);

    //h2 bx u vx
    h2bxuvxa11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
    h2bxuvxa12 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
    h2bxuvxa13 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);

    h2bxuvxa21 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
    h2bxuvxa22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
    h2bxuvxa23 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);

    h2bxuvxa31 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);
    h2bxuvxa32 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);
    h2bxuvxa33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);
                                    

    //h bx bx u v
    hbx2uva11 = 2*idx*((1333.0*i1344)*hjmhp*bjmh*bjmh+(443.0*i6720)*hjphm*bjmh*bjmh+(-3141.0*i2240)*hjmhp*bjms*bjmh+(-171.0*i2240)*hjphm*bjms*bjmh+(1161.0*i2240)*hjmhp*bjps*bjmh+(27.0*i2240)*hjphm*bjps*bjmh+(-145.0*i1344)*hjmhp*bjph*bjmh+(-11.0*i6720)*hjphm*bjph*bjmh+(-3141.0*i2240)*hjmhp*bjmh*bjms+(-171.0*i2240)*hjphm*bjmh*bjms+(4617.0*i2240)*hjmhp*bjms*bjms+(243.0*i2240)*hjphm*bjms*bjms+(-1863.0*i2240)*hjmhp*bjps*bjms+(-81.0*i2240)*hjphm*bjps*bjms+(387.0*i2240)*hjmhp*bjph*bjms+(9.0*i2240)*hjphm*bjph*bjms+(1161.0*i2240)*hjmhp*bjmh*bjps+(27.0*i2240)*hjphm*bjmh*bjps+(-1863.0*i2240)*hjmhp*bjms*bjps+(-81.0*i2240)*hjphm*bjms*bjps+(891.0*i2240)*hjmhp*bjps*bjps+(81.0*i2240)*hjphm*bjps*bjps+(-27.0*i320)*hjmhp*bjph*bjps+(-27.0*i2240)*hjphm*bjph*bjps+(-145.0*i1344)*hjmhp*bjmh*bjph+(-11.0*i6720)*hjphm*bjmh*bjph+(387.0*i2240)*hjmhp*bjms*bjph+(9.0*i2240)*hjphm*bjms*bjph+(-27.0*i320)*hjmhp*bjps*bjph+(-27.0*i2240)*hjphm*bjps*bjph+(131.0*i6720)*hjmhp*bjph*bjph+(13.0*i1344)*hjphm*bjph*bjph);
    hbx2uva12 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
    hbx2uva13 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);


    hbx2uva21 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
    hbx2uva22 = 2*idx*((101.0*i420)*hjmhp*bjmh*bjmh+(29.0*i420)*hjphm*bjmh*bjmh+(-6.0*i35)*hjmhp*bjms*bjmh+(-3.0*i35)*hjphm*bjms*bjmh+(-3.0*i28)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(4.0*i105)*hjmhp*bjph*bjmh+(4.0*i105)*hjphm*bjph*bjmh+(-6.0*i35)*hjmhp*bjmh*bjms+(-3.0*i35)*hjphm*bjmh*bjms+(27.0*i28)*hjmhp*bjms*bjms+(27.0*i28)*hjphm*bjms*bjms+(-27.0*i35)*hjmhp*bjps*bjms+(-27.0*i35)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(-3.0*i28)*hjphm*bjph*bjms+(-3.0*i28)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-27.0*i35)*hjmhp*bjms*bjps+(-27.0*i35)*hjphm*bjms*bjps+(27.0*i28)*hjmhp*bjps*bjps+(27.0*i28)*hjphm*bjps*bjps+(-3.0*i35)*hjmhp*bjph*bjps+(-6.0*i35)*hjphm*bjph*bjps+(4.0*i105)*hjmhp*bjmh*bjph+(4.0*i105)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(-3.0*i28)*hjphm*bjms*bjph+(-3.0*i35)*hjmhp*bjps*bjph+(-6.0*i35)*hjphm*bjps*bjph+(29.0*i420)*hjmhp*bjph*bjph+(101.0*i420)*hjphm*bjph*bjph);
    hbx2uva23 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);

    hbx2uva31 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);                
    hbx2uva32 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);
    hbx2uva33 = 2*idx*((13.0*i1344)*hjmhp*bjmh*bjmh+(131.0*i6720)*hjphm*bjmh*bjmh+(-27.0*i2240)*hjmhp*bjms*bjmh+(-27.0*i320)*hjphm*bjms*bjmh+(9.0*i2240)*hjmhp*bjps*bjmh+(387.0*i2240)*hjphm*bjps*bjmh+(-11.0*i6720)*hjmhp*bjph*bjmh+(-145.0*i1344)*hjphm*bjph*bjmh+(-27.0*i2240)*hjmhp*bjmh*bjms+(-27.0*i320)*hjphm*bjmh*bjms+(81.0*i2240)*hjmhp*bjms*bjms+(891.0*i2240)*hjphm*bjms*bjms+(-81.0*i2240)*hjmhp*bjps*bjms+(-1863.0*i2240)*hjphm*bjps*bjms+(27.0*i2240)*hjmhp*bjph*bjms+(1161.0*i2240)*hjphm*bjph*bjms+(9.0*i2240)*hjmhp*bjmh*bjps+(387.0*i2240)*hjphm*bjmh*bjps+(-81.0*i2240)*hjmhp*bjms*bjps+(-1863.0*i2240)*hjphm*bjms*bjps+(243.0*i2240)*hjmhp*bjps*bjps+(4617.0*i2240)*hjphm*bjps*bjps+(-171.0*i2240)*hjmhp*bjph*bjps+(-3141.0*i2240)*hjphm*bjph*bjps+(-11.0*i6720)*hjmhp*bjmh*bjph+(-145.0*i1344)*hjphm*bjmh*bjph+(27.0*i2240)*hjmhp*bjms*bjph+(1161.0*i2240)*hjphm*bjms*bjph+(-171.0*i2240)*hjmhp*bjps*bjph+(-3141.0*i2240)*hjphm*bjps*bjph+(443.0*i6720)*hjmhp*bjph*bjph+(1333.0*i1344)*hjphm*bjph*bjph);       

    // LHS 
    
    LHSa11 = uhintia11 + h3uxintia11 + h2bxuxva11 + h2bxuvxa11 + hbx2uva11;
    LHSa12 = uhintia12 + h3uxintia12 + h2bxuxva12 + h2bxuvxa12 + hbx2uva12; 
    LHSa13 = uhintia13 + h3uxintia13 + h2bxuxva13 + h2bxuvxa13 + hbx2uva13;
    LHSa21 = uhintia21 + h3uxintia21 + h2bxuxva21 + h2bxuvxa21 + hbx2uva21;
    LHSa22 = uhintia22 + h3uxintia22 + h2bxuxva22 + h2bxuvxa22 + hbx2uva22; 
    LHSa23 = uhintia23 + h3uxintia23 + h2bxuxva23 + h2bxuvxa23 + hbx2uva23;
    LHSa31 = uhintia31 + h3uxintia31 + h2bxuxva31 + h2bxuvxa31 + hbx2uva31;
    LHSa32 = uhintia32 + h3uxintia32 + h2bxuxva32 + h2bxuvxa32 + hbx2uva32;
    LHSa33 = uhintia33 + h3uxintia33 + h2bxuxva33 + h2bxuvxa33 + hbx2uva33;


    Ared[j-1 + 3][1] = Ared[j-1 + 3][1] + LHSa31;

    Ared[j-1 +2][2] = Ared[j-1 + 2][2] + LHSa21;
    Ared[j + 2][2] = Ared[j + 2][2] + LHSa32;

    Ared[j-1 + 1][3] = 1;
    Ared[j + 1][3] = Ared[j + 1][3] + LHSa22;
    Ared[j+1 + 1][3] = Ared[j+1 + 1][3] + LHSa33;

    Ared[j-1 + 1][4] = 0;
    Ared[j + 1][4] = Ared[j + 1][4] + LHSa23;
    
    Ared[j-1 + 1 ][5] = 0;

    nGis[j-1] = uMbeg[unBC-1];
    nGis[j] = nGis[j] + Gintia21; 
    nGis[j+1] = nGis[j+1] + Gintia31;     

    hhbc[3*i + (nGhBC)] = hjmhp;
    hhbc[3*i+1 + (nGhBC)] = 0.5*(hjmhp +  hjphm);
    hhbc[3*i+2 + (nGhBC)] = hjphm; 

    whbc[3*i + (nGhBC)] = wjmhp;
    whbc[3*i+1 + (nGhBC)] = 0.5*(wjmhp +  wjphm);
    whbc[3*i+2 + (nGhBC)] = wjphm;

    Ghbc[3*i + (nGhBC)] = Gjmhp;
    Ghbc[3*i+1 + (nGhBC)] = (hhbc[3*i+1 + (nGhBC)] > 0)*0.5*(Gjmhp +  Gjphm);
    Ghbc[3*i+2 + (nGhBC)] = Gjphm;

    bedhbc[4*i + (nbBC)] = bjmh;
    bedhbc[4*i+1 + (nbBC)] = bjms;
    bedhbc[4*i+2 + (nbBC)] = bjps;
    bedhbc[4*i+3 + (nbBC)] = bjph;

    bedhbc[0] = bMbeg[nbBC-4];
    bedhbc[1] = bMbeg[nbBC-3];
    bedhbc[2] = bMbeg[nbBC-2];
    bedhbc[3] = bMbeg[nbBC-1];
  

// last
    j = m-2;
    i = n-1;

    // Get it from interior
    // Reconstruct w
    wi = fmax(h[i] + bed[i],bed[i]); 
 
    hjphm= fmax(hMend[0],0);
    hjmhp= hhbc[nGhbc - nGhBC -4];

    Gjphm= (hjphm > 0)*GMend[0];
    Gjmhp= (hjmhp > 0)*Ghbc[nGhbc - nGhBC -4];

    // reconstruct bed

    bedai = i6*idx*idx*idx*(-bed[i-3] +3*bed[i-2] -3*bed[i-1] + bed[i]); 
    bedbi = 0.5*idx*idx*(-bed[i-3] + 4*bed[i-2] - 5*bed[i-1] + 2*bed[i]);
    bedci = i6*idx*(-2*bed[i-3] + 9*bed[i-2] - 18*bed[i-1] + 11*bed[i]);
    beddi = bed[i];

    bjmh = -bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) - bedci*(0.5*dx) + beddi;
    bjms = -bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) - bedci*(dx*i6) + beddi;
    bjps = bedai*(dx*i6)*(dx*i6)*(dx*i6) + bedbi*(dx*i6)*(dx*i6) + bedci*(dx*i6) + beddi;
    bjph = bedai*(0.5*dx)*(0.5*dx)*(0.5*dx) + bedbi*(0.5*dx)*(0.5*dx) + bedci*(0.5*dx) + beddi;

    wjphm= fmax(wMend[0],bjph );
    wjmhp= fmax(whbc[nGhbc - nGhBC -4],bjmh); 

    // G integral (RHS)
    Gintia11 = dx*i6*(Gjmhp);
    Gintia21 = dx*i6*(2*Gjmhp + 2*Gjphm);
    Gintia31 = dx*i6*(Gjphm);
    
    
    //uh integral
    uhintia11 = (1.0 *i60)*dx*(7*hjmhp + hjphm);
    uhintia12 = (1.0 *i60)*dx*(4*hjmhp );
    uhintia13 = (1.0 *i60)*dx*(-hjmhp - hjphm);
    
    uhintia21 = (1.0 *i60)*dx*(4*hjmhp);
    uhintia22 = (1.0 *i60)*dx*(16*hjmhp + 16*hjphm);
    uhintia23 = (1.0 *i60)*dx*(4*hjphm);
    
    uhintia31 = (1.0 *i60)*dx*(-hjmhp - hjphm);
    uhintia32 = (1.0 *i60)*dx*(4*hjphm);
    uhintia33 = (1.0 *i60)*dx*(hjmhp + 7*hjphm);
    
    //h3ux
    
    h3uxintia11 = (2.0*i3)*idx*((79.0*i120)*hjmhp*hjmhp*hjmhp +  (39.0*i120)*hjmhp*hjmhp*hjphm + (3.0*i24)*hjmhp*hjphm*hjphm + (7.0*i120)*hjphm*hjphm*hjphm);        
    h3uxintia12 = (2.0*i3)*idx*((-23.0*i30)*hjmhp*hjmhp*hjmhp +  (-3.0*i10)*hjmhp*hjmhp*hjphm + (-3.0*i30)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
    h3uxintia13 = (2.0*i3)*idx*((13.0*i120)*hjmhp*hjmhp*hjmhp +  (-3.0*i120)*hjmhp*hjmhp*hjphm + (-3.0*i120)*hjmhp*hjphm*hjphm + (13.0*i120)*hjphm*hjphm*hjphm);
    
    
    h3uxintia21 = (2.0*i3)*idx*((-23.0*i30)*hjmhp*hjmhp*hjmhp +  (-3.0*i10)*hjmhp*hjmhp*hjphm + (-3.0*i30)*hjmhp*hjphm*hjphm + (-1.0*i6)*hjphm*hjphm*hjphm);        
    h3uxintia22 = (2.0*i3)*idx*((14.0*i15)*hjmhp*hjmhp*hjmhp +  (6.0*i15)*hjmhp*hjmhp*hjphm + (6.0*i15)*hjmhp*hjphm*hjphm + (14.0*i15)*hjphm*hjphm*hjphm);        
    h3uxintia23 = (2.0*i3)*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i30)*hjmhp*hjmhp*hjphm + (-3.0*i10)*hjmhp*hjphm*hjphm + (-23.0*i30)*hjphm*hjphm*hjphm);
    
    h3uxintia31 = (2.0*i3)*idx*((13.0*i120)*hjmhp*hjmhp*hjmhp +  (-3.0*i120)*hjmhp*hjmhp*hjphm + (-3.0*i120)*hjmhp*hjphm*hjphm + (13.0*i120)*hjphm*hjphm*hjphm);       
    h3uxintia32 = (2.0*i3)*idx*((-1.0*i6)*hjmhp*hjmhp*hjmhp +  (-3.0*i30)*hjmhp*hjmhp*hjphm + (-3.0*i10)*hjmhp*hjphm*hjphm + (-23.0*i30)*hjphm*hjphm*hjphm);        
    h3uxintia33 = (2.0*i3)*idx*((7.0*i120)*hjmhp*hjmhp*hjmhp +  (3.0*i24)*hjmhp*hjmhp*hjphm + (39.0*i120)*hjmhp*hjphm*hjphm + (79.0*i120)*hjphm*hjphm*hjphm);
    
    //h2 bx ux v
    h2bxuxva11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
    h2bxuxva12 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva13 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);

    h2bxuxva21 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
    h2bxuxva22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva23 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);

    h2bxuxva31 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);
    h2bxuxva32 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);
    h2bxuxva33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);

    //h2 bx u vx
    h2bxuvxa11 = -idx*((31.0*i42)*hjmhp*hjmhp*bjmh+(19.0*i280)*hjphm*hjmhp*bjmh+(19.0*i280)*hjmhp*hjphm*bjmh+(23.0*i1680)*hjphm*hjphm*bjmh+(-537.0*i560)*hjmhp*hjmhp*bjms+(-33.0*i560)*hjphm*hjmhp*bjms+(-33.0*i560)*hjmhp*hjphm*bjms+(-3.0*i280)*hjphm*hjphm*bjms+(39.0*i140)*hjmhp*hjmhp*bjps+(-3.0*i280)*hjphm*hjmhp*bjps+(-3.0*i280)*hjmhp*hjphm*bjps+(3.0*i560)*hjphm*hjphm*bjps+(-97.0*i1680)*hjmhp*hjmhp*bjph+(1.0*i560)*hjphm*hjmhp*bjph+(1.0*i560)*hjmhp*hjphm*bjph+(-1.0*i120)*hjphm*hjphm*bjph);
    h2bxuvxa12 = -idx*((71.0*i210)*hjmhp*hjmhp*bjmh+(1.0*i15)*hjphm*hjmhp*bjmh+(1.0*i15)*hjmhp*hjphm*bjmh+(1.0*i84)*hjphm*hjphm*bjmh+(-3.0*i20)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(9.0*i70)*hjphm*hjphm*bjms+(-3.0*i14)*hjmhp*hjmhp*bjps+(-6.0*i35)*hjphm*hjmhp*bjps+(-6.0*i35)*hjmhp*hjphm*bjps+(-27.0*i140)*hjphm*hjphm*bjps+(11.0*i420)*hjmhp*hjmhp*bjph+(2.0*i105)*hjphm*hjmhp*bjph+(2.0*i105)*hjmhp*hjphm*bjph+(11.0*i210)*hjphm*hjphm*bjph);
    h2bxuvxa13 = -idx*((-19.0*i280)*hjmhp*hjmhp*bjmh+(-23.0*i1680)*hjphm*hjmhp*bjmh+(-23.0*i1680)*hjmhp*hjphm*bjmh+(-43.0*i1680)*hjphm*hjphm*bjmh+(33.0*i560)*hjmhp*hjmhp*bjms+(3.0*i280)*hjphm*hjmhp*bjms+(3.0*i280)*hjmhp*hjphm*bjms+(3.0*i28)*hjphm*hjphm*bjms+(3.0*i280)*hjmhp*hjmhp*bjps+(-3.0*i560)*hjphm*hjmhp*bjps+(-3.0*i560)*hjmhp*hjphm*bjps+(-21.0*i80)*hjphm*hjphm*bjps+(-1.0*i560)*hjmhp*hjmhp*bjph+(1.0*i120)*hjphm*hjmhp*bjph+(1.0*i120)*hjmhp*hjphm*bjph+(19.0*i105)*hjphm*hjphm*bjph);

    h2bxuvxa21 = -idx*((-193.0*i210)*hjmhp*hjmhp*bjmh+(-8.0*i105)*hjphm*hjmhp*bjmh+(-8.0*i105)*hjmhp*hjphm*bjmh+(-1.0*i84)*hjphm*hjphm*bjmh+(171.0*i140)*hjmhp*hjmhp*bjms+(9.0*i140)*hjphm*hjmhp*bjms+(9.0*i140)*hjmhp*hjphm*bjms+(0)*hjphm*hjphm*bjms+(-27.0*i70)*hjmhp*hjmhp*bjps+(0)*hjphm*hjmhp*bjps+(0)*hjmhp*hjphm*bjps+(-9.0*i140)*hjphm*hjphm*bjps+(1.0*i12)*hjmhp*hjmhp*bjph+(1.0*i84)*hjphm*hjmhp*bjph+(1.0*i84)*hjmhp*hjphm*bjph+(8.0*i105)*hjphm*hjphm*bjph);
    h2bxuvxa22 = -idx*((-41.0*i105)*hjmhp*hjmhp*bjmh+(-3.0*i35)*hjphm*hjmhp*bjmh+(-3.0*i35)*hjmhp*hjphm*bjmh+(-4.0*i105)*hjphm*hjphm*bjmh+(12.0*i35)*hjmhp*hjmhp*bjms+(3.0*i35)*hjphm*hjmhp*bjms+(3.0*i35)*hjmhp*hjphm*bjms+(3.0*i35)*hjphm*hjphm*bjms+(3.0*i35)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(12.0*i35)*hjphm*hjphm*bjps+(-4.0*i105)*hjmhp*hjmhp*bjph+(-3.0*i35)*hjphm*hjmhp*bjph+(-3.0*i35)*hjmhp*hjphm*bjph+(-41.0*i105)*hjphm*hjphm*bjph);
    h2bxuvxa23 = -idx*((8.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i84)*hjphm*hjmhp*bjmh+(1.0*i84)*hjmhp*hjphm*bjmh+(1.0*i12)*hjphm*hjphm*bjmh+(-9.0*i140)*hjmhp*hjmhp*bjms+(0)*hjphm*hjmhp*bjms+(0)*hjmhp*hjphm*bjms+(-27.0*i70)*hjphm*hjphm*bjms+(0)*hjmhp*hjmhp*bjps+(9.0*i140)*hjphm*hjmhp*bjps+(9.0*i140)*hjmhp*hjphm*bjps+(171.0*i140)*hjphm*hjphm*bjps+(-1.0*i84)*hjmhp*hjmhp*bjph+(-8.0*i105)*hjphm*hjmhp*bjph+(-8.0*i105)*hjmhp*hjphm*bjph+(-193.0*i210)*hjphm*hjphm*bjph);

    h2bxuvxa31 = -idx*((19.0*i105)*hjmhp*hjmhp*bjmh+(1.0*i120)*hjphm*hjmhp*bjmh+(1.0*i120)*hjmhp*hjphm*bjmh+(-1.0*i560)*hjphm*hjphm*bjmh+(-21.0*i80)*hjmhp*hjmhp*bjms+(-3.0*i560)*hjphm*hjmhp*bjms+(-3.0*i560)*hjmhp*hjphm*bjms+(3.0*i280)*hjphm*hjphm*bjms+(3.0*i28)*hjmhp*hjmhp*bjps+(3.0*i280)*hjphm*hjmhp*bjps+(3.0*i280)*hjmhp*hjphm*bjps+(33.0*i560)*hjphm*hjphm*bjps+(-43.0*i1680)*hjmhp*hjmhp*bjph+(-23.0*i1680)*hjphm*hjmhp*bjph+(-23.0*i1680)*hjmhp*hjphm*bjph+(-19.0*i280)*hjphm*hjphm*bjph);
    h2bxuvxa32 = -idx*((11.0*i210)*hjmhp*hjmhp*bjmh+(2.0*i105)*hjphm*hjmhp*bjmh+(2.0*i105)*hjmhp*hjphm*bjmh+(11.0*i420)*hjphm*hjphm*bjmh+(-27.0*i140)*hjmhp*hjmhp*bjms+(-6.0*i35)*hjphm*hjmhp*bjms+(-6.0*i35)*hjmhp*hjphm*bjms+(-3.0*i14)*hjphm*hjphm*bjms+(9.0*i70)*hjmhp*hjmhp*bjps+(3.0*i35)*hjphm*hjmhp*bjps+(3.0*i35)*hjmhp*hjphm*bjps+(-3.0*i20)*hjphm*hjphm*bjps+(1.0*i84)*hjmhp*hjmhp*bjph+(1.0*i15)*hjphm*hjmhp*bjph+(1.0*i15)*hjmhp*hjphm*bjph+(71.0*i210)*hjphm*hjphm*bjph);
    h2bxuvxa33 = -idx*((-1.0*i120)*hjmhp*hjmhp*bjmh+(1.0*i560)*hjphm*hjmhp*bjmh+(1.0*i560)*hjmhp*hjphm*bjmh+(-97.0*i1680)*hjphm*hjphm*bjmh+(3.0*i560)*hjmhp*hjmhp*bjms+(-3.0*i280)*hjphm*hjmhp*bjms+(-3.0*i280)*hjmhp*hjphm*bjms+(39.0*i140)*hjphm*hjphm*bjms+(-3.0*i280)*hjmhp*hjmhp*bjps+(-33.0*i560)*hjphm*hjmhp*bjps+(-33.0*i560)*hjmhp*hjphm*bjps+(-537.0*i560)*hjphm*hjphm*bjps+(23.0*i1680)*hjmhp*hjmhp*bjph+(19.0*i280)*hjphm*hjmhp*bjph+(19.0*i280)*hjmhp*hjphm*bjph+(31.0*i42)*hjphm*hjphm*bjph);
                                    

    //h bx bx u v
    hbx2uva11 = 2*idx*((1333.0*i1344)*hjmhp*bjmh*bjmh+(443.0*i6720)*hjphm*bjmh*bjmh+(-3141.0*i2240)*hjmhp*bjms*bjmh+(-171.0*i2240)*hjphm*bjms*bjmh+(1161.0*i2240)*hjmhp*bjps*bjmh+(27.0*i2240)*hjphm*bjps*bjmh+(-145.0*i1344)*hjmhp*bjph*bjmh+(-11.0*i6720)*hjphm*bjph*bjmh+(-3141.0*i2240)*hjmhp*bjmh*bjms+(-171.0*i2240)*hjphm*bjmh*bjms+(4617.0*i2240)*hjmhp*bjms*bjms+(243.0*i2240)*hjphm*bjms*bjms+(-1863.0*i2240)*hjmhp*bjps*bjms+(-81.0*i2240)*hjphm*bjps*bjms+(387.0*i2240)*hjmhp*bjph*bjms+(9.0*i2240)*hjphm*bjph*bjms+(1161.0*i2240)*hjmhp*bjmh*bjps+(27.0*i2240)*hjphm*bjmh*bjps+(-1863.0*i2240)*hjmhp*bjms*bjps+(-81.0*i2240)*hjphm*bjms*bjps+(891.0*i2240)*hjmhp*bjps*bjps+(81.0*i2240)*hjphm*bjps*bjps+(-27.0*i320)*hjmhp*bjph*bjps+(-27.0*i2240)*hjphm*bjph*bjps+(-145.0*i1344)*hjmhp*bjmh*bjph+(-11.0*i6720)*hjphm*bjmh*bjph+(387.0*i2240)*hjmhp*bjms*bjph+(9.0*i2240)*hjphm*bjms*bjph+(-27.0*i320)*hjmhp*bjps*bjph+(-27.0*i2240)*hjphm*bjps*bjph+(131.0*i6720)*hjmhp*bjph*bjph+(13.0*i1344)*hjphm*bjph*bjph);
    hbx2uva12 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
    hbx2uva13 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);


    hbx2uva21 = 2*idx*((103.0*i336)*hjmhp*bjmh*bjmh+(3.0*i70)*hjphm*bjmh*bjmh+(-183.0*i560)*hjmhp*bjms*bjmh+(-3.0*i140)*hjphm*bjms*bjmh+(3.0*i112)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(-11.0*i1680)*hjmhp*bjph*bjmh+(0)*hjphm*bjph*bjmh+(-183.0*i560)*hjmhp*bjmh*bjms+(-3.0*i140)*hjphm*bjmh*bjms+(243.0*i560)*hjmhp*bjms*bjms+(0)*hjphm*bjms*bjms+(-81.0*i560)*hjmhp*bjps*bjms+(0)*hjphm*bjps*bjms+(3.0*i80)*hjmhp*bjph*bjms+(3.0*i140)*hjphm*bjph*bjms+(3.0*i112)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-81.0*i560)*hjmhp*bjms*bjps+(0)*hjphm*bjms*bjps+(81.0*i560)*hjmhp*bjps*bjps+(0)*hjphm*bjps*bjps+(-3.0*i112)*hjmhp*bjph*bjps+(3.0*i140)*hjphm*bjph*bjps+(-11.0*i1680)*hjmhp*bjmh*bjph+(0)*hjphm*bjmh*bjph+(3.0*i80)*hjmhp*bjms*bjph+(3.0*i140)*hjphm*bjms*bjph+(-3.0*i112)*hjmhp*bjps*bjph+(3.0*i140)*hjphm*bjps*bjph+(-1.0*i240)*hjmhp*bjph*bjph+(-3.0*i70)*hjphm*bjph*bjph);
    hbx2uva22 = 2*idx*((101.0*i420)*hjmhp*bjmh*bjmh+(29.0*i420)*hjphm*bjmh*bjmh+(-6.0*i35)*hjmhp*bjms*bjmh+(-3.0*i35)*hjphm*bjms*bjmh+(-3.0*i28)*hjmhp*bjps*bjmh+(-3.0*i140)*hjphm*bjps*bjmh+(4.0*i105)*hjmhp*bjph*bjmh+(4.0*i105)*hjphm*bjph*bjmh+(-6.0*i35)*hjmhp*bjmh*bjms+(-3.0*i35)*hjphm*bjmh*bjms+(27.0*i28)*hjmhp*bjms*bjms+(27.0*i28)*hjphm*bjms*bjms+(-27.0*i35)*hjmhp*bjps*bjms+(-27.0*i35)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(-3.0*i28)*hjphm*bjph*bjms+(-3.0*i28)*hjmhp*bjmh*bjps+(-3.0*i140)*hjphm*bjmh*bjps+(-27.0*i35)*hjmhp*bjms*bjps+(-27.0*i35)*hjphm*bjms*bjps+(27.0*i28)*hjmhp*bjps*bjps+(27.0*i28)*hjphm*bjps*bjps+(-3.0*i35)*hjmhp*bjph*bjps+(-6.0*i35)*hjphm*bjph*bjps+(4.0*i105)*hjmhp*bjmh*bjph+(4.0*i105)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(-3.0*i28)*hjphm*bjms*bjph+(-3.0*i35)*hjmhp*bjps*bjph+(-6.0*i35)*hjphm*bjps*bjph+(29.0*i420)*hjmhp*bjph*bjph+(101.0*i420)*hjphm*bjph*bjph);
    hbx2uva23 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);

    hbx2uva31 = 2*idx*((-443.0*i6720)*hjmhp*bjmh*bjmh+(-13.0*i1344)*hjphm*bjmh*bjmh+(171.0*i2240)*hjmhp*bjms*bjmh+(27.0*i2240)*hjphm*bjms*bjmh+(-27.0*i2240)*hjmhp*bjps*bjmh+(-9.0*i2240)*hjphm*bjps*bjmh+(11.0*i6720)*hjmhp*bjph*bjmh+(11.0*i6720)*hjphm*bjph*bjmh+(171.0*i2240)*hjmhp*bjmh*bjms+(27.0*i2240)*hjphm*bjmh*bjms+(-243.0*i2240)*hjmhp*bjms*bjms+(-81.0*i2240)*hjphm*bjms*bjms+(81.0*i2240)*hjmhp*bjps*bjms+(81.0*i2240)*hjphm*bjps*bjms+(-9.0*i2240)*hjmhp*bjph*bjms+(-27.0*i2240)*hjphm*bjph*bjms+(-27.0*i2240)*hjmhp*bjmh*bjps+(-9.0*i2240)*hjphm*bjmh*bjps+(81.0*i2240)*hjmhp*bjms*bjps+(81.0*i2240)*hjphm*bjms*bjps+(-81.0*i2240)*hjmhp*bjps*bjps+(-243.0*i2240)*hjphm*bjps*bjps+(27.0*i2240)*hjmhp*bjph*bjps+(171.0*i2240)*hjphm*bjph*bjps+(11.0*i6720)*hjmhp*bjmh*bjph+(11.0*i6720)*hjphm*bjmh*bjph+(-9.0*i2240)*hjmhp*bjms*bjph+(-27.0*i2240)*hjphm*bjms*bjph+(27.0*i2240)*hjmhp*bjps*bjph+(171.0*i2240)*hjphm*bjps*bjph+(-13.0*i1344)*hjmhp*bjph*bjph+(-443.0*i6720)*hjphm*bjph*bjph);                
    hbx2uva32 = 2*idx*((-3.0*i70)*hjmhp*bjmh*bjmh+(-1.0*i240)*hjphm*bjmh*bjmh+(3.0*i140)*hjmhp*bjms*bjmh+(-3.0*i112)*hjphm*bjms*bjmh+(3.0*i140)*hjmhp*bjps*bjmh+(3.0*i80)*hjphm*bjps*bjmh+(0)*hjmhp*bjph*bjmh+(-11.0*i1680)*hjphm*bjph*bjmh+(3.0*i140)*hjmhp*bjmh*bjms+(-3.0*i112)*hjphm*bjmh*bjms+(0)*hjmhp*bjms*bjms+(81.0*i560)*hjphm*bjms*bjms+(0)*hjmhp*bjps*bjms+(-81.0*i560)*hjphm*bjps*bjms+(-3.0*i140)*hjmhp*bjph*bjms+(3.0*i112)*hjphm*bjph*bjms+(3.0*i140)*hjmhp*bjmh*bjps+(3.0*i80)*hjphm*bjmh*bjps+(0)*hjmhp*bjms*bjps+(-81.0*i560)*hjphm*bjms*bjps+(0)*hjmhp*bjps*bjps+(243.0*i560)*hjphm*bjps*bjps+(-3.0*i140)*hjmhp*bjph*bjps+(-183.0*i560)*hjphm*bjph*bjps+(0)*hjmhp*bjmh*bjph+(-11.0*i1680)*hjphm*bjmh*bjph+(-3.0*i140)*hjmhp*bjms*bjph+(3.0*i112)*hjphm*bjms*bjph+(-3.0*i140)*hjmhp*bjps*bjph+(-183.0*i560)*hjphm*bjps*bjph+(3.0*i70)*hjmhp*bjph*bjph+(103.0*i336)*hjphm*bjph*bjph);
    hbx2uva33 = 2*idx*((13.0*i1344)*hjmhp*bjmh*bjmh+(131.0*i6720)*hjphm*bjmh*bjmh+(-27.0*i2240)*hjmhp*bjms*bjmh+(-27.0*i320)*hjphm*bjms*bjmh+(9.0*i2240)*hjmhp*bjps*bjmh+(387.0*i2240)*hjphm*bjps*bjmh+(-11.0*i6720)*hjmhp*bjph*bjmh+(-145.0*i1344)*hjphm*bjph*bjmh+(-27.0*i2240)*hjmhp*bjmh*bjms+(-27.0*i320)*hjphm*bjmh*bjms+(81.0*i2240)*hjmhp*bjms*bjms+(891.0*i2240)*hjphm*bjms*bjms+(-81.0*i2240)*hjmhp*bjps*bjms+(-1863.0*i2240)*hjphm*bjps*bjms+(27.0*i2240)*hjmhp*bjph*bjms+(1161.0*i2240)*hjphm*bjph*bjms+(9.0*i2240)*hjmhp*bjmh*bjps+(387.0*i2240)*hjphm*bjmh*bjps+(-81.0*i2240)*hjmhp*bjms*bjps+(-1863.0*i2240)*hjphm*bjms*bjps+(243.0*i2240)*hjmhp*bjps*bjps+(4617.0*i2240)*hjphm*bjps*bjps+(-171.0*i2240)*hjmhp*bjph*bjps+(-3141.0*i2240)*hjphm*bjph*bjps+(-11.0*i6720)*hjmhp*bjmh*bjph+(-145.0*i1344)*hjphm*bjmh*bjph+(27.0*i2240)*hjmhp*bjms*bjph+(1161.0*i2240)*hjphm*bjms*bjph+(-171.0*i2240)*hjmhp*bjps*bjph+(-3141.0*i2240)*hjphm*bjps*bjph+(443.0*i6720)*hjmhp*bjph*bjph+(1333.0*i1344)*hjphm*bjph*bjph);       

    // LHS 
    
    LHSa11 = uhintia11 + h3uxintia11 + h2bxuxva11 + h2bxuvxa11 + hbx2uva11;
    LHSa12 = uhintia12 + h3uxintia12 + h2bxuxva12 + h2bxuvxa12 + hbx2uva12; 
    LHSa13 = uhintia13 + h3uxintia13 + h2bxuxva13 + h2bxuvxa13 + hbx2uva13;
    LHSa21 = uhintia21 + h3uxintia21 + h2bxuxva21 + h2bxuvxa21 + hbx2uva21;
    LHSa22 = uhintia22 + h3uxintia22 + h2bxuxva22 + h2bxuvxa22 + hbx2uva22; 
    LHSa23 = uhintia23 + h3uxintia23 + h2bxuxva23 + h2bxuvxa23 + hbx2uva23;
    LHSa31 = uhintia31 + h3uxintia31 + h2bxuxva31 + h2bxuvxa31 + hbx2uva31;
    LHSa32 = uhintia32 + h3uxintia32 + h2bxuxva32 + h2bxuvxa32 + hbx2uva32;
    LHSa33 = uhintia33 + h3uxintia33 + h2bxuxva33 + h2bxuvxa33 + hbx2uva33;
       

    Ared[j-1 + 3][1] = 0;

    Ared[j-1 +2][2] = Ared[j-1 + 2][2] + LHSa21;
    Ared[j + 2][2] = 0;

    Ared[j-1 + 1][3] = Ared[j-1 + 1][3] + LHSa11;
    Ared[j + 1][3] = Ared[j + 1][3] + LHSa22;
    Ared[j+1 + 1][3] = 1;

    Ared[j-1 + 1][4] = Ared[j-1 + 1][4] + LHSa12;
    Ared[j + 1][4] = Ared[j + 1][4] + LHSa23;
    
    Ared[j-1 + 1 ][5] = Ared[j-1 + 1][5] + LHSa13;

    
    nGis[j-1] = nGis[j-1] + Gintia11; 
    nGis[j] = nGis[j] + Gintia21; 
    nGis[j+1] = uMend[0];




    hhbc[3*i + (nGhBC)] = hjmhp;
    hhbc[3*i+1 + (nGhBC)] = 0.5*(hjmhp + hjphm);
    hhbc[3*i+2 + (nGhBC)] = hjphm; 

    whbc[3*i + (nGhBC)] = wjmhp;
    whbc[3*i+1 + (nGhBC)] = 0.5*(wjmhp + wjphm);
    whbc[3*i+2 + (nGhBC)] = wjphm;

    Ghbc[3*i + (nGhBC)] = Gjmhp;
    Ghbc[3*i+1 + (nGhBC)] = (hhbc[3*i+1 + (nGhBC)] > 0)*0.5*(Gjmhp + Gjphm);
    Ghbc[3*i+2 + (nGhBC)] = Gjphm;

    bedhbc[4*i + (nbBC)] = bjmh;
    bedhbc[4*i+1 + (nbBC)] = bjms;
    bedhbc[4*i+2 + (nbBC)] = bjps;
    bedhbc[4*i+3 + (nbBC)] = bjph;

    bedhbc[4*(i+1) + (nbBC)] = bMend[0];
    bedhbc[4*(i+1) + 1 + (nbBC)] = bMend[1];
    bedhbc[4*(i+1) + 2 + (nbBC)] = bMend[2];
    bedhbc[4*(i+1) + 3 + (nbBC)] = bMend[3];


    double d;
    d = bandec(Ared, m, 2,2, AL ,indx);

    banbks(Ared, m,2,2, AL, indx, nGis);

    //PENT(uais,ubis,ucis,udis,ueis,nGis,m,utemp); 


    memcpy(u,uMbeg, (unBC-1)*sizeof(double));
    memcpy(u+(unBC-1),nGis,m*sizeof(double));
    memcpy(u+nubc - (unBC-1),uMend + 1,(unBC-1)*sizeof(double));

    // B.C stuff
    for(i=0;i < nGhBC;i++)
    {
        // Assuming bed constant in ghost cells
        whbc[i] = fmax(wMbeg[i] , bMbeg[0] );
        whbc[nGhbc-nGhBC + i] = fmax(wMend[i] , bMend[0] );
        hhbc[i] = fmax(hMbeg[i],0);
        hhbc[nGhbc-nGhBC + i] = fmax(hMend[i],0);
        Ghbc[i] = (hhbc[i] > 0)*GMbeg[i];
        Ghbc[nGhbc-nGhBC +i] = (hhbc[nGhbc-nGhBC +i] > 0)*GMend[i];

        //printf("Beg: %d | %f | %f | %f \n",i,wMbeg[i],hMbeg[i],GMbeg[i]);
        //printf("end: %d | %f | %f | %f \n",i,wMend[i],hMend[i],GMend[i]);
    }

    free(Ared);
    free(AL);
    free(indx);
    free(nGis);

}


int main()
{
    printf("h");
    return 1;
}
