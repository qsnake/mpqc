#include <chemistry/qc/oint3/build.h>
int sc::BuildIntV3::i0200eAB(){
/* the cost is 24 */
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
t1=int_v_zeta34*int_v_ooze;
t2=int_v_oo2zeta12*t1;
t1=(-1)*t2;
double***restrictxx int_v_list0=int_v_list(0);
double**restrictxx int_v_list00=int_v_list0[0];
double*restrictxx int_v_list001=int_v_list00[1];
t2=int_v_list001[0]*t1;
double*restrictxx int_v_list000=int_v_list00[0];
t1=int_v_list000[0]*int_v_oo2zeta12;
t3=t1+t2;
t1=int_v_W0-int_v_p120;
double*restrictxx int_v_list002=int_v_list00[2];
t2=int_v_list002[0]*t1;
t4=t1*t2;
t5=t4+t3;
double***restrictxx int_v_list2=int_v_list(2);
double**restrictxx int_v_list20=int_v_list2[0];
double*restrictxx int_v_list200=int_v_list20[0];
int_v_list200[5]=t5;
t4=int_v_W2-int_v_p122;
t6=t4*t2;
int_v_list200[4]=t6;
t7=int_v_W1-int_v_p121;
t8=t2*t7;
int_v_list200[3]=t8;
t2=int_v_list002[0]*t4;
t9=t4*t2;
t2=t3+t9;
int_v_list200[2]=t2;
t9=int_v_list002[0]*t7;
t10=t4*t9;
int_v_list200[1]=t10;
t11=t7*t9;
t9=t3+t11;
int_v_list200[0]=t9;
t3=int_v_list001[0]*t1;
double***restrictxx int_v_list1=int_v_list(1);
double**restrictxx int_v_list10=int_v_list1[0];
double*restrictxx int_v_list100=int_v_list10[0];
int_v_list100[2]=t3;
t1=int_v_list001[0]*t4;
int_v_list100[1]=t1;
t4=int_v_list001[0]*t7;
int_v_list100[0]=t4;
return 1;}
