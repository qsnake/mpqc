#include <chemistry/qc/oint3/build.h>
int sc::BuildIntV3::i2201eAB(){
/* the cost is 116 */
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
double t28;
double t29;
double t30;
double t31;
t1=int_v_W0-int_v_p120;
double***restrictxx int_v_list0=int_v_list(0);
double**restrictxx int_v_list00=int_v_list0[0];
double*restrictxx int_v_list002=int_v_list00[2];
t2=t1*int_v_list002[0];
t3=0.5*int_v_ooze;
t4=t3*t2;
t5=int_v_W0-int_v_p340;
t6=t5*int_v_list002[0];
t7=int_v_p340-int_v_r30;
double*restrictxx int_v_list001=int_v_list00[1];
t8=t7*int_v_list001[0];
t9=t8+t6;
t6=int_v_zeta34*int_v_ooze;
t8=int_v_oo2zeta12*t6;
t6=(-1)*t8;
t8=t6*t9;
t9=t8+t4;
t10=t5*int_v_list001[0];
double*restrictxx int_v_list000=int_v_list00[0];
t11=t7*int_v_list000[0];
t12=t11+t10;
t10=int_v_oo2zeta12*t12;
t11=t10+t9;
t9=t3*int_v_list002[0];
double*restrictxx int_v_list003=int_v_list00[3];
t12=t5*int_v_list003[0];
t5=t7*int_v_list002[0];
t7=t5+t12;
t5=t1*t7;
t12=t5+t9;
t5=t1*t12;
t13=t5+t11;
double***restrictxx int_v_list2=int_v_list(2);
double**restrictxx int_v_list21=int_v_list2[1];
double*restrictxx int_v_list210=int_v_list21[0];
int_v_list210[17]=t13;
t5=int_v_W2-int_v_p342;
t11=t5*int_v_list002[0];
t14=int_v_p342-int_v_r32;
t15=t14*int_v_list001[0];
t16=t15+t11;
t11=t6*t16;
t15=t5*int_v_list001[0];
t16=t14*int_v_list000[0];
t17=t16+t15;
t15=int_v_oo2zeta12*t17;
t16=t15+t11;
t17=t5*int_v_list003[0];
t5=t14*int_v_list002[0];
t14=t5+t17;
t5=t1*t14;
t17=t1*t5;
t18=t17+t16;
int_v_list210[16]=t18;
t17=int_v_W1-int_v_p341;
t19=t17*int_v_list002[0];
t20=int_v_p341-int_v_r31;
t21=t20*int_v_list001[0];
t22=t21+t19;
t19=t6*t22;
t21=t17*int_v_list001[0];
t22=t20*int_v_list000[0];
t23=t22+t21;
t21=int_v_oo2zeta12*t23;
t22=t21+t19;
t23=t17*int_v_list003[0];
t17=t20*int_v_list002[0];
t20=t17+t23;
t17=t1*t20;
t23=t1*t17;
t24=t23+t22;
int_v_list210[15]=t24;
t23=int_v_W2-int_v_p122;
t25=t23*t12;
int_v_list210[14]=t25;
t26=t23*t5;
t27=t4+t26;
int_v_list210[13]=t27;
t26=t23*t17;
int_v_list210[12]=t26;
t28=int_v_W1-int_v_p121;
t29=t12*t28;
int_v_list210[11]=t29;
t12=t28*t5;
int_v_list210[10]=t12;
t5=t28*t17;
t17=t4+t5;
int_v_list210[9]=t17;
t4=t10+t8;
t5=t23*t7;
t8=t23*t5;
t5=t8+t4;
int_v_list210[8]=t5;
t8=t23*int_v_list002[0];
t10=t3*t8;
t30=t11+t10;
t10=t15+t30;
t11=t23*t14;
t15=t9+t11;
t11=t23*t15;
t15=t11+t10;
int_v_list210[7]=t15;
t10=t23*t20;
t11=t23*t10;
t10=t22+t11;
int_v_list210[6]=t10;
t11=t28*t7;
t7=t23*t11;
int_v_list210[5]=t7;
t22=t28*t14;
t14=t23*t22;
t30=t28*int_v_list002[0];
t31=t3*t30;
t3=t31+t14;
int_v_list210[4]=t3;
t14=t28*t20;
t20=t9+t14;
t9=t23*t20;
int_v_list210[3]=t9;
t14=t28*t11;
t11=t4+t14;
int_v_list210[2]=t11;
t4=t28*t22;
t14=t16+t4;
int_v_list210[1]=t14;
t4=t19+t31;
t16=t21+t4;
t4=t28*t20;
t19=t4+t16;
int_v_list210[0]=t19;
t4=t6*int_v_list001[0];
t6=int_v_oo2zeta12*int_v_list000[0];
t16=t6+t4;
t4=t1*t2;
t1=t4+t16;
double**restrictxx int_v_list20=int_v_list2[0];
double*restrictxx int_v_list200=int_v_list20[0];
int_v_list200[5]=t1;
t4=t23*t2;
int_v_list200[4]=t4;
t6=t28*t2;
int_v_list200[3]=t6;
t2=t23*t8;
t8=t16+t2;
int_v_list200[2]=t8;
t2=t23*t30;
int_v_list200[1]=t2;
t20=t28*t30;
t21=t16+t20;
int_v_list200[0]=t21;
return 1;}
