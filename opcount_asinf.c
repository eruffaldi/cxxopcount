//#include "fdlibm.h"
#define __ieee754_sqrtf __builtin_sqrtf
#define fabsf __builtin_fabsf
#define __uint32_t unsigned int
#define __int32_t  int
typedef union
{
float value;
 __uint32_t word;
} ieee_float_shape_type;

#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)

// same efficiency as of #define
#if 1
#define MAKECONST(x,y)  static const float x = y;
MAKECONST(one , 1.0000000000e+00) /* 0x3F800000 */
MAKECONST(huge ,  1.000e+30)
MAKECONST(pio2_hi , 1.57079637050628662109375f)
MAKECONST(pio2_lo , -4.37113900018624283e-8f)
MAKECONST(pio4_hi , 0.785398185253143310546875f)
	/* coefficient for R(x^2) */
MAKECONST(pS0 ,  1.6666667163e-01) /* 0x3e2aaaab */
MAKECONST(pS1 , -3.2556581497e-01) /* 0xbea6b090 */
MAKECONST(pS2 ,  2.0121252537e-01) /* 0x3e4e0aa8 */
MAKECONST(pS3 , -4.0055535734e-02) /* 0xbd241146 */
MAKECONST(pS4 ,  7.9153501429e-04) /* 0x3a4f7f04 */
MAKECONST(pS5 ,  3.4793309169e-05) /* 0x3811ef08 */
MAKECONST(qS1 , -2.4033949375e+00) /* 0xc019d139 */
MAKECONST(qS2 ,  2.0209457874e+00) /* 0x4001572d */
MAKECONST(qS3 , -6.8828397989e-01) /* 0xbf303361 */
MAKECONST(qS4 ,  7.7038154006e-02) /* 0x3d9dc62e */
#else
#define one  1.0000000000e+00f /* 0x3F800000 */
#define huge   1.000e+30f
#define pio2_hi  1.57079637050628662109375f
#define pio2_lo  -4.37113900018624283e-8f
#define pio4_hi  0.785398185253143310546875f
	/* coefficient for R(x^2 */
#define pS0   1.6666667163e-01f /* 0x3e2aaaab */
#define pS1  -3.2556581497e-01f /* 0xbea6b090 */
#define pS2   2.0121252537e-01f /* 0x3e4e0aa8 */
#define pS3  -4.0055535734e-02f /* 0xbd241146 */
#define pS4   7.9153501429e-04f /* 0x3a4f7f04 */
#define pS5   3.4793309169e-05f /* 0x3811ef08 */
#define qS1  -2.4033949375e+00f /* 0xc019d139 */
#define qS2   2.0209457874e+00f /* 0x4001572d */
#define qS3  -6.8828397989e-01f /* 0xbf303361 */
#define qS4   7.7038154006e-02f /* 0x3d9dc62e */
#endif

#ifdef __STDC__
	float __ieee754_asinf(float x)
#else
	float __ieee754_asinf(x)
	float x;
#endif
{
	// 7 cases: 
	// 1
	// |x|>=1
	// special when |x|<2**-27
	// |x| < 2**-27
	// |x| > 0.975
	// |x|<=0.5
	// |x|>= 0.5
	float t,w,p,q,c,r,s;
	__int32_t hx,ix;
	GET_FLOAT_WORD(hx,x);
	ix = hx&0x7fffffff;
	if(ix==0x3f800000) {
		/* asin(1)=+-pi/2 with inexact */
	    return x*pio2_hi+x*pio2_lo;	
	} else if(ix> 0x3f800000) {	/* |x|>= 1 */
	    return (x-x)/(x-x);		/* asin(|x|>1) is NaN */   
	} else if (ix<0x3f000000) {	/* |x|<0.5 */
	    if(ix<0x32000000) {		/* if |x| < 2**-27 */
		if(huge+x>one) return x;/* return x with inexact if x!=0*/
          } else {
		t = x*x;
		p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
		q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
		w = p/q;
		return x+x*w;
          }
	}
	/* 1> |x|>= 0.5 */
	w = one-fabsf(x);
	t = w*(float)0.5f;
	p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
	q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
	s = __ieee754_sqrtf(t);
	if(ix>=0x3F79999A) { 	/* if |x| > 0.975 */
	    w = p/q;
	    t = pio2_hi-((float)2.0f*(s+s*w)-pio2_lo);
	} else {
	    __int32_t iw;
	    w  = s;
	    GET_FLOAT_WORD(iw,w);
	    SET_FLOAT_WORD(w,iw&0xfffff000);
	    c  = (t-w*w)/(s+w);
	    r  = p/q;
	    p  = (float)2.0f*s*r-(pio2_lo-(float)2.0f*c); // modified to float not double (is i fine?)
	    q  = pio4_hi-(float)2.0f*w;
	    t  = pio4_hi-(p-q);
	}    
	if(hx>0) return t; else return -t;    
}