#include "opcount.hpp"

#define if(x)  (x);
#define else
using real32_T = counted_t<float>;
using int32_T = counted_t<int>;

int32_T Encoders[3];
real32_T POSE[3];

struct Motore15_8_DWork_t {
	int32_T Eold_m[3];
	real32_T rap_l;
} Motore15_8_DWork;

struct Motore15_8_P_t {
	real32_T DirectKinematics_CPR[3];
	real32_T DirectKinematics_L;
	real32_T rap_l;
} Motore15_8_P;


int32_T MIN_int32_T;
int32_T MAX_int32_T;

static void Motore15_8_step_e(void)
{
  real32_T cp;
  real32_T cp_0;
  real32_T cp_1;
  real32_T dth_idx;
  real32_T dth_idx_0;
  real32_T dth_idx_1;
  int32_T qY;

  /* MATLAB Function 'step': '<S28>:15' */
  /* '<S28>:15:3' */
  qY = Encoders[0] - Motore15_8_DWork.Eold_m[0];
  if ((Encoders[0] < 0L) && ((Motore15_8_DWork.Eold_m[0] >= 0L) && (qY >= 0L)))
  {
    qY = MIN_int32_T;
  } else {
    if ((Encoders[0] >= 0L) && ((Motore15_8_DWork.Eold_m[0] < 0L) && (qY < 0L)))
    {
      qY = MAX_int32_T;
    }
  }

  dth_idx = 6.28318548F / Motore15_8_P.DirectKinematics_CPR[0] * (real32_T)qY;
  qY = Encoders[1] - Motore15_8_DWork.Eold_m[1];
  if ((Encoders[1] < 0L) && ((Motore15_8_DWork.Eold_m[1] >= 0L) && (qY >= 0L)))
  {
    qY = MIN_int32_T;
  } else {
    if ((Encoders[1] >= 0L) && ((Motore15_8_DWork.Eold_m[1] < 0L) && (qY < 0L)))
    {
      qY = MAX_int32_T;
    }
  }

  dth_idx_0 = 6.28318548F / Motore15_8_P.DirectKinematics_CPR[1] * (real32_T)qY;
  qY = Encoders[2] - Motore15_8_DWork.Eold_m[2];
  if ((Encoders[2] < 0L) && ((Motore15_8_DWork.Eold_m[2] >= 0L) && (qY >= 0L)))
  {
    qY = MIN_int32_T;
  } else {
    if ((Encoders[2] >= 0L) && ((Motore15_8_DWork.Eold_m[2] < 0L) && (qY < 0L)))
    {
      qY = MAX_int32_T;
    }
  }

  dth_idx_1 = 6.28318548F / Motore15_8_P.DirectKinematics_CPR[2] * (real32_T)qY;

  /*  delta teta. Variazione di angolo  */
  /*  tra un passo e l'altro. */
  /* '<S28>:15:5' */
  /*  sfasamento meccanico delle ruote */
  /* '<S28>:15:7' */
  cp = (real32_T)cos(POSE[2]);

  /* '<S28>:15:8' */
  /* '<S28>:15:9' */
  cp_0 = (real32_T)cos(POSE[2] + 2.09439516F);

  /* '<S28>:15:10' */
  /* '<S28>:15:11' */
  cp_1 = (real32_T)cos(POSE[2] - 2.09439516F);

  /* '<S28>:15:12' */
  /*  il pi/2 serve perche' il sistema di riferimento del robot e' ruotato di 90  */
  /*  gradi rispetto a quello fisso. Xf= Yb e  Yf= -Xb */
  /* '<S28>:15:16' */
  POSE[0] += (((real32_T)sin(POSE[2] + 2.09439516F) * dth_idx_0 + (real32_T)sin
               (POSE[2]) * dth_idx) + (real32_T)sin(POSE[2] - 2.09439516F) *
              dth_idx_1) * Motore15_8_DWork.rap_l;

  /* '<S28>:15:17' */
  POSE[1] += ((-cp * dth_idx - cp_0 * dth_idx_0) - cp_1 * dth_idx_1) *
    Motore15_8_DWork.rap_l;

  /* '<S28>:15:18' */
  POSE[2] += Motore15_8_DWork.rap_l / (2.0F * Motore15_8_P.DirectKinematics_L) *
    ((dth_idx + dth_idx_0) + dth_idx_1);

  /*  POSE(3) = mod((POSE(3) + rap * 1/(2*L)*(dth(1)+dth(2)+dth(3))+pi),2*pi)-pi; */
}


int main(int argc, char const *argv[])
{
	Motore15_8_step_e();

	counted_t<int>::dump();
	counted_t<float>::dump();
	return 0;
}