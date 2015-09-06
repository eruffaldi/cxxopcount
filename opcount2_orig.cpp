
#ifndef BARE
#include <stdio.h>
#include <math.h>
#else
extern "C"
{
extern float sin(float f);
extern float cos(float f);
extern float sqrt(float f);
}
#endif

/*
 *Common parameters based on Bartek Naming
 *
 */

float Ldown = 0.07;
float Lup   = 0.183;
float Rdown = 0.08;
float Rup   = 0.042;

float Rint  = 0; // init as Rdown -Rup;        
float S01[3], S02[3], S03[3];
float S1[3], S2[3], S3[3];
float S12[3], S13[3], INCIRC[3], NORM[3];

float XEE[3]; // Direct cinematic target 
float XEEfkd[3];

#define  CTHETA -0.5f // cos(2*pi/3)
#define  STHETA 0.8660254037844387f //sin(2*pi/3)

#define X 0
#define Y 1
#define Z 2


float Rot1[] = {0, -1, 0};            // cross ([  1        0    0], k)
float Rot2[] = {STHETA, -CTHETA, 0};  // cross ([CTHETA, STHETA, 0], k)
float Rot3[] = {-STHETA, -CTHETA, 0}; // cross ([CTHETA,-STHETA, 0], k)

float Ldown_CTHETA;
float Ldown_STHETA;
float Rint_CTHETA;
float Rint_STHETA;

float Jinv[3][3];
float Jdir[3][3];

void USGinit()
{
    int i;
    // initialize intermediate variables and products
    Rint = Rdown - Rup;
    Ldown_CTHETA = Ldown*CTHETA;
    Rint_CTHETA  = Rint*CTHETA;
    Ldown_STHETA = Ldown*STHETA;
    Rint_STHETA  = Rint*STHETA;
    for (i=0; i<3; i++)
    {
        S01[i] = S02[i] = S03[i] = 0;
        S1[i]  = S2[i]  = S3[i]  = 0;
    }
}

extern "C"
void fkd(float fi[3])
{
float cf1 = cos(fi[0]);
float sf1 = sin(fi[0]);
float cf2 = cos(fi[1]);
float sf2 = sin(fi[1]);
float cf3 = cos(fi[2]);
float sf3 = sin(fi[2]);
float x1,x2,x3,y1,y2,y3,z1,z2,z3;
float a,b,m;
float yp0, yp1, xp0,xp1,zp0,zp1,zp2;
/* Computing the Ei points*/
x1 = (Rint+Ldown*cf1)*1;
y1 = (Rint+Ldown*cf1)*0;
z1 = Ldown*sf1;

x2 = (Rint+Ldown*cf2)*CTHETA;
y2 = (Rint+Ldown*cf2)*STHETA;
z2 = Ldown*sf2;

x3 = (Rint+Ldown*cf3)*CTHETA;
y3 = (Rint+Ldown*cf3)*(-STHETA);
z3 = Ldown*sf3;
a = (x2*x2+y2*y2+z2*z2-(x1*x1+y1*y1+z1*z1));
b = ((x3*x3) + (y3*y3) + (z3*z3) -((x1*x1) +(y1*y1)+(z1*z1)));
m = ((y3-y1)*(x2-x1)-(y2-y1)*(x3-x1));

yp0 = ((x2-x1)*b-(x3-x1)*a)/(2*m);
yp1 = ((x3-x1)*(z2-z1)-(z3-z1)*(x2-x1))/m;

xp0 = (0.5*a-yp0*(y2-y1))/(x2-x1);
xp1 = (yp1*(y2-y1)+(z2-z1))/(x2-x1);

zp0 = (xp0*xp0+yp0*yp0+z1*z1-Lup*Lup+y1*y1+x1*x1-2*(x1*xp0+yp0*y1));
zp1 = (xp1*xp0-yp1*yp0+z1-x1*xp1+yp1*y1);
zp2 = (xp1*xp1+yp1*yp1+1);

XEEfkd[X] = -(2*zp1+sqrt(4*zp1*zp1-4*zp2*zp0))/(2*zp2)*xp1+xp0;
XEEfkd[Y] = (2*zp1+sqrt(4*zp1*zp1-4*zp2*zp0))/(2*zp2)*yp1+yp0;
XEEfkd[Z] = (2*zp1+sqrt(4*zp1*zp1-4*zp2*zp0))/(2*zp2);

}

extern "C"
void USGDC(float fi[3])
{
float cf1 = cos(fi[0]);
float sf1 = sin(fi[0]);
float cf2 = cos(fi[1]);
float sf2 = sin(fi[1]);
float cf3 = cos(fi[2]);
float sf3 = sin(fi[2]);
float d11, d13, d33, DET, P1,P2,k; // intermediate products

/* Computing base joints and Ei position and versors */
    S01[X] = Ldown*cf1;
    S02[X] = Ldown_CTHETA*cf2;
    S03[X] = Ldown_CTHETA*cf3;

    //S01[Y] =0;
    S02[Y] = Ldown_STHETA*cf2;
    S03[Y] = -Ldown_STHETA*cf3;

    S01[Z] = Ldown*sf1;
    S02[Z] = Ldown*sf2;
    S03[Z] = Ldown*sf3;

    S1[X]  = S01[X]+Rint;
    S2[X]  = Rint_CTHETA + S02[X];
    S3[X]  = Rint_CTHETA+S03[X];

    //S1[Y]  = 0;
    S2[Y]  = Rint_STHETA + S02[Y];
    S3[Y]  = -Rint_STHETA+ S03[Y];

    S1[Z]  = S01[Z];    
    S2[Z]  = S02[Z];
    S3[Z]  = S03[Z];
/*
    #if 1
        printf("S1 %g %g %g\n", S1[X], S1[Y], S1[Z]);
        printf("S2 %g %g %g\n", S2[X], S2[Y], S2[Z]);
        printf("S3 %g %g %g\n", S3[X], S3[Y], S3[Z]);
    #endif
    #if 1
        printf("S01 %g %g %g\n", S01[X], S01[Y], S01[Z]);
        printf("S02 %g %g %g\n", S02[X], S02[Y], S02[Z]);
        printf("S03 %g %g %g\n", S03[X], S03[Y], S03[Z]);
    #endif
    */
    /* Processing the internal triangle */
    S12[X] = S2[X] - S1[X];
    S12[Y] = S2[Y] - S1[Y];
    S12[Z] = S2[Z] - S1[Z];
    S13[X] = S3[X] - S1[X];
    S13[Y] = S3[Y] - S1[Y];
    S13[Z] = S3[Z] - S1[Z];
    // Cross product
    NORM[X] =  S12[Y]*S13[Z] - S12[Z]*S13[Y];
    NORM[Y] =  S13[X]*S12[Z] - S12[X]*S13[Z];
    NORM[Z] =  S12[X]*S13[Y] - S12[Y]*S13[X];

/*    #if DEBUG
        printf("S12 %g %g %g\n", S12[X], S12[Y], S12[Z]);
        printf("S13 %g %g %g\n", S13[X], S13[Y], S13[Z]);
        printf("NOR %g %g %g\n", NORM[X], NORM[Y], NORM[Z]);
    #endif
  */  
    /* dot products and determinant*/
    d11 = S12[X]*S12[X] + S12[Y]*S12[Y] + S12[Z]*S12[Z];
    d13 = S12[X]*S13[X] + S12[Y]*S13[Y] + S12[Z]*S13[Z];
    d33 = S13[X]*S13[X] + S13[Y]*S13[Y] + S13[Z]*S13[Z];
    DET = d11*d33-d13*d13;
    P1 = 0.5*d33*(d11-d13)/DET;
    P2 = 0.5*d11*(d33-d13)/DET;
    INCIRC[X] = S12[X]*P1+S13[X]*P2;
    INCIRC[Y] = S12[Y]*P1+S13[Y]*P2;
    INCIRC[Z] = S12[Z]*P1+S13[Z]*P2;
/*
    #if 1
        printf("INCIRC %g %g %g\n", INCIRC[X], INCIRC[Y], INCIRC[Z]);
    #endif 
        */
    // Gain by Pitagora
    k = sqrt((Lup*Lup -
            (INCIRC[X]*INCIRC[X] + INCIRC[Y]*INCIRC[Y] + INCIRC[Z]*INCIRC[Z])
        ) /   (NORM[X]*NORM[X] + NORM[Y]*NORM[Y] + NORM[Z]*NORM[Z]));
    
    XEE[X] = S1[X] + INCIRC[X] + k*NORM[X];
    XEE[Y] = S1[Y] + INCIRC[Y] + k*NORM[Y];
    XEE[Z] = S1[Z] + INCIRC[Z] + k*NORM[Z];

}

void USGinvJ()
{
}


int main(int argc, char *argv[])
{
    float fi[3];
    fi[1] = 0.1;
    fi[2] = 0.1;
    USGinit();
#ifdef BARE
    USGDC(fi);
    float w = XEE[X];
    fkd(fi);
    return (int)(XEE[X]+w);
#else
    for (fi[X]= 0.1; fi[X]<1; fi[X]+=.1)
    {
        USGDC(fi);
        fkd(fi);
        printf("%g/%g   %g/%g   %g/%g\n",
                XEE[X], XEEfkd[X],
                XEE[Y], XEEfkd[Y],
                XEE[Z], XEEfkd[Z]
                );
        
    }
#endif
} 

