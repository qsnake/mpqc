#include <chemistry/qc/oint3/build.h>
int sc::BuildIntV3::i3300eAB(){
/* the cost is 63 */
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
t1=int_v_W0-int_v_p120;
double***restrictxx int_v_list0=int_v_list(0);
double**restrictxx int_v_list00=int_v_list0[0];
double*restrictxx int_v_list002=int_v_list00[2];
t2=t1*int_v_list002[0];
t3=int_v_ooze*2;
t4=int_v_zeta34*t3;
t3=int_v_oo2zeta12*t4;
t4=(-1)*t3;
t3=t4*t2;
double*restrictxx int_v_list001=int_v_list00[1];
t5=t1*int_v_list001[0];
t6=int_v_oo2zeta12*2;
t7=t6*t5;
t8=t7+t3;
t3=int_v_zeta34*int_v_ooze;
t7=int_v_oo2zeta12*t3;
t3=(-1)*t7;
t7=t3*int_v_list002[0];
t9=int_v_oo2zeta12*int_v_list001[0];
t10=t9+t7;
double*restrictxx int_v_list003=int_v_list00[3];
t7=t1*int_v_list003[0];
t9=t1*t7;
t11=t9+t10;
t9=t1*t11;
t1=t9+t8;
double***restrictxx int_v_list3=int_v_list(3);
double**restrictxx int_v_list30=int_v_list3[0];
double*restrictxx int_v_list300=int_v_list30[0];
int_v_list300[9]=t1;
t8=int_v_W2-int_v_p122;
t9=t8*t11;
int_v_list300[8]=t9;
t12=int_v_W1-int_v_p121;
t13=t11*t12;
int_v_list300[7]=t13;
t11=t3*t2;
t2=int_v_oo2zeta12*t5;
t5=t2+t11;
t2=t8*t7;
t11=t8*t2;
t2=t11+t5;
int_v_list300[6]=t2;
t11=t12*t7;
t7=t8*t11;
int_v_list300[5]=t7;
t14=t12*t11;
t11=t5+t14;
int_v_list300[4]=t11;
t5=t8*int_v_list002[0];
t14=t4*t5;
t5=t8*int_v_list001[0];
t15=t6*t5;
t5=t15+t14;
t14=t8*int_v_list003[0];
t15=t8*t14;
t14=t10+t15;
t15=t8*t14;
t14=t15+t5;
int_v_list300[3]=t14;
t5=t12*int_v_list002[0];
t15=t3*t5;
t3=t12*int_v_list001[0];
t16=int_v_oo2zeta12*t3;
t17=t16+t15;
t15=t12*int_v_list003[0];
t16=t8*t15;
t18=t8*t16;
t16=t18+t17;
int_v_list300[2]=t16;
t17=t12*t15;
t15=t10+t17;
t10=t8*t15;
int_v_list300[1]=t10;
t8=t4*t5;
t4=t6*t3;
t3=t4+t8;
t4=t12*t15;
t5=t4+t3;
int_v_list300[0]=t5;
return 1;}
