#include <chemistry/qc/oint3/build.h>
int sc::BuildIntV3::i2211eAB(){
/* the cost is 104 */
double t1;
double t2;
double t3;
double t4;
double t5;
double t6;
double t7;
double t8;
double t9;
double t10;
double t11;
double t12;
double t13;
double t14;
double t15;
double t16;
double t17;
double t18;
double t19;
double t20;
double t21;
double t22;
double t23;
double t24;
double t25;
double t26;
double t27;
t1=int_v_W0-int_v_p120;
double***restrictxx int_v_list0=int_v_list(0);
double**restrictxx int_v_list00=int_v_list0[0];
double*restrictxx int_v_list002=int_v_list00[2];
t2=t1*int_v_list002[0];
t3=0.5*int_v_ooze;
t4=t3*t2;
t2=int_v_W0-int_v_p340;
t5=t2*int_v_list002[0];
t6=int_v_p340-int_v_r30;
double*restrictxx int_v_list001=int_v_list00[1];
t7=t6*int_v_list001[0];
t8=t7+t5;
t5=int_v_zeta34*int_v_ooze;
t7=int_v_oo2zeta12*t5;
t5=(-1)*t7;
t7=t5*t8;
t8=t7+t4;
t9=t2*int_v_list001[0];
double*restrictxx int_v_list000=int_v_list00[0];
t10=t6*int_v_list000[0];
t11=t10+t9;
t9=int_v_oo2zeta12*t11;
t10=t9+t8;
t8=t3*int_v_list002[0];
double*restrictxx int_v_list003=int_v_list00[3];
t11=t2*int_v_list003[0];
t2=t6*int_v_list002[0];
t6=t2+t11;
t2=t1*t6;
t11=t2+t8;
t2=t1*t11;
t12=t2+t10;
double***restrictxx int_v_list2=int_v_list(2);
double**restrictxx int_v_list21=int_v_list2[1];
double*restrictxx int_v_list210=int_v_list21[0];
int_v_list210[17]=t12;
t2=int_v_W2-int_v_p342;
t10=t2*int_v_list002[0];
t13=int_v_p342-int_v_r32;
t14=t13*int_v_list001[0];
t15=t14+t10;
t10=t5*t15;
t14=t2*int_v_list001[0];
t15=t13*int_v_list000[0];
t16=t15+t14;
t14=int_v_oo2zeta12*t16;
t15=t14+t10;
t16=t2*int_v_list003[0];
t2=t13*int_v_list002[0];
t13=t2+t16;
t2=t1*t13;
t16=t1*t2;
t17=t16+t15;
int_v_list210[16]=t17;
t16=int_v_W1-int_v_p341;
t18=t16*int_v_list002[0];
t19=int_v_p341-int_v_r31;
t20=t19*int_v_list001[0];
t21=t20+t18;
t18=t5*t21;
t5=t16*int_v_list001[0];
t20=t19*int_v_list000[0];
t21=t20+t5;
t5=int_v_oo2zeta12*t21;
t20=t5+t18;
t21=t16*int_v_list003[0];
t16=t19*int_v_list002[0];
t19=t16+t21;
t16=t1*t19;
t21=t1*t16;
t1=t21+t20;
int_v_list210[15]=t1;
t21=int_v_W2-int_v_p122;
t22=t21*t11;
int_v_list210[14]=t22;
t23=t21*t2;
t24=t4+t23;
int_v_list210[13]=t24;
t23=t21*t16;
int_v_list210[12]=t23;
t25=int_v_W1-int_v_p121;
t26=t11*t25;
int_v_list210[11]=t26;
t11=t25*t2;
int_v_list210[10]=t11;
t2=t25*t16;
t16=t4+t2;
int_v_list210[9]=t16;
t2=t9+t7;
t4=t21*t6;
t7=t21*t4;
t4=t7+t2;
int_v_list210[8]=t4;
t7=t21*int_v_list002[0];
t9=t3*t7;
t7=t10+t9;
t9=t14+t7;
t7=t21*t13;
t10=t8+t7;
t7=t21*t10;
t10=t7+t9;
int_v_list210[7]=t10;
t7=t21*t19;
t9=t21*t7;
t7=t20+t9;
int_v_list210[6]=t7;
t9=t25*t6;
t6=t21*t9;
int_v_list210[5]=t6;
t14=t25*t13;
t13=t21*t14;
t20=t25*int_v_list002[0];
t27=t3*t20;
t3=t27+t13;
int_v_list210[4]=t3;
t13=t25*t19;
t19=t8+t13;
t8=t21*t19;
int_v_list210[3]=t8;
t13=t25*t9;
t9=t2+t13;
int_v_list210[2]=t9;
t2=t25*t14;
t13=t15+t2;
int_v_list210[1]=t13;
t2=t18+t27;
t14=t5+t2;
t2=t25*t19;
t5=t2+t14;
int_v_list210[0]=t5;
return 1;}
