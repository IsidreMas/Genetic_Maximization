#include "ran1.h"
#include <math.h>
#include <time.h>

#define USHRT_WIDTH 16
#define UINT_WIDTH 32

/*********************************
 * ran1 from "Numerical Recipes" *
 * note #undef's at end of file  *
**********************************/
#define IA 16807
#define IM 2147483647L
#define AM (1.0/IM)
#define IQ 127773L
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

float ran1(long *idum)
{   int j; long k;
    static long iy=0;
    static long iv[NTAB];
    float temp;

    if (*idum <= 0 || !iy) {
        if (-(*idum) < 1) *idum=1; else *idum = -(*idum);
        for (j=NTAB+7;j>=0;j--) {
            k=(*idum)/IQ;
            *idum=IA*(*idum-k*IQ)-IR*k;
            if (*idum < 0) *idum += IM;
            if (j < NTAB) iv[j] = *idum;
        }
        iy=iv[0];
    }
    k=(*idum)/IQ;
    *idum=IA*(*idum-k*IQ)-IR*k;
    if (*idum < 0) *idum += IM;
    j=(int) (iy/(long)NDIV);
    iy=iv[j];
    iv[j] = *idum;
    if ((temp=AM*iy) > RNMX) return RNMX;
    else return temp;
} /* ran1 */
#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX

/* Utility functions to simplify the use of ran1 and idum */
long idum;
void randomize(void) {idum = time(NULL); if(idum > 0) idum = -idum;}
float uniform(void) {return ran1(&idum);} // between 0.0 and 1.0
/* Based on von-neuman observation ; rather inefficient; */
unsigned char random_bit(void){ unsigned char f, s;
    do { f = 2*ran1(&idum); s = 2*ran1(&idum); } while (f == s);
    return f;
}

unsigned UINTran(void){ register unsigned char i;
    unsigned oneUL = 1U, base = 0U;

    for(i=0; i < UINT_WIDTH; i++) if(random_bit()) { base = oneUL; break; }
    for(i++; i < UINT_WIDTH; i++){ base = base << 1;
        if(random_bit()) base = base | oneUL;
    }
    return base;
}

unsigned short USHRTran(void){ register unsigned char i;
    unsigned short oneU = 1U, base = 0U;

    for(i=0; i < USHRT_WIDTH; i++) if(random_bit()) { base = oneU; break; }
    for(i++; i < USHRT_WIDTH; i++){ base = base << 1;
        if(random_bit()) base = base | oneU;
    }
    return base;
}
unsigned char UCHARran(void){ register unsigned char i;
    unsigned char oneU = 1U, base = 0U;

    for(i=0; i < 8; i++) if(random_bit()) { base = oneU; break; }
    for(i++; i < 8; i++){ base = base << 1;
        if(random_bit()) base = base | oneU;
    }
    return base;
}