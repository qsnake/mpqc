#include <chemistry/qc/oint3/build.h>
int sc::BuildIntV3::i0101(){
/* the cost is 71 */
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
t1=0.5*int_v_ooze;
double***restrictxx int_v_list0=int_v_list(0);
double**restrictxx int_v_list00=int_v_list0[0];
double*restrictxx int_v_list001=int_v_list00[1];
t2=t1*int_v_list001[0];
t1=int_v_W0-int_v_p340;
double*restrictxx int_v_list002=int_v_list00[2];
t3=t1*int_v_list002[0];
t4=int_v_p340-int_v_r30;
t5=t4*int_v_list001[0];
t6=t5+t3;
t3=int_v_W0-int_v_p120;
t5=t3*t6;
t7=t5+t2;
t5=t1*int_v_list001[0];
double*restrictxx int_v_list000=int_v_list00[0];
t1=t4*int_v_list000[0];
t4=t1+t5;
double**restrictxx int_v_list01=int_v_list0[1];
double*restrictxx int_v_list010=int_v_list01[0];
int_v_list010[2]=t4;
t1=int_v_p120-int_v_r10;
t5=t1*t4;
t8=t5+t7;
double***restrictxx int_v_list1=int_v_list(1);
double**restrictxx int_v_list11=int_v_list1[1];
double*restrictxx int_v_list110=int_v_list11[0];
int_v_list110[8]=t8;
t5=int_v_W2-int_v_p342;
t7=t5*int_v_list002[0];
t9=int_v_p342-int_v_r32;
t10=t9*int_v_list001[0];
t11=t10+t7;
t7=t3*t11;
t10=t5*int_v_list001[0];
t5=t9*int_v_list000[0];
t9=t5+t10;
int_v_list010[1]=t9;
t5=t1*t9;
t10=t5+t7;
int_v_list110[7]=t10;
t5=int_v_W1-int_v_p341;
t7=t5*int_v_list002[0];
t12=int_v_p341-int_v_r31;
t13=t12*int_v_list001[0];
t14=t13+t7;
t7=t3*t14;
t13=t5*int_v_list001[0];
t5=t12*int_v_list000[0];
t12=t5+t13;
int_v_list010[0]=t12;
t5=t1*t12;
t13=t5+t7;
int_v_list110[6]=t13;
t5=int_v_W2-int_v_p122;
t7=t5*t6;
t15=int_v_p122-int_v_r12;
t16=t15*t4;
t17=t16+t7;
int_v_list110[5]=t17;
t7=t5*t11;
t16=t2+t7;
t7=t15*t9;
t18=t7+t16;
int_v_list110[4]=t18;
t7=t5*t14;
t16=t15*t12;
t19=t16+t7;
int_v_list110[3]=t19;
t7=int_v_W1-int_v_p121;
t16=t6*t7;
t6=int_v_p121-int_v_r11;
t20=t6*t4;
t4=t20+t16;
int_v_list110[2]=t4;
t16=t7*t11;
t11=t6*t9;
t9=t11+t16;
int_v_list110[1]=t9;
t11=t7*t14;
t14=t2+t11;
t2=t6*t12;
t11=t2+t14;
int_v_list110[0]=t11;
t2=t3*int_v_list001[0];
t3=t1*int_v_list000[0];
t1=t3+t2;
double**restrictxx int_v_list10=int_v_list1[0];
double*restrictxx int_v_list100=int_v_list10[0];
int_v_list100[2]=t1;
t2=t5*int_v_list001[0];
t3=t15*int_v_list000[0];
t5=t3+t2;
int_v_list100[1]=t5;
t2=t7*int_v_list001[0];
t3=t6*int_v_list000[0];
t6=t3+t2;
int_v_list100[0]=t6;
return 1;}
