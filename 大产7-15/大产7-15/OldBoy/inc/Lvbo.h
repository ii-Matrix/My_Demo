#ifndef _OLDBOY_LVBO_H_
#define _OLDBOY_LVBO_H_
void Lvbo_Middle();     //将出弯时断成两截的中线缝合起来
void Lvbo_Right(uint8 y);
void Lvbo_Left(uint8 y);
int Lvbo_Middle2(uint8 y,int middle);
void Img_GoOutBend_Optimize();          //出弯入直图像优化，减弱转过的情况
void Middle_Correct();                   //前车内切，和边界混在一起
void Middle_Correct2();
void Overtake_Return_GenZong();         //超车区后车返回时的跟踪方法
void RodRemove();                       //去除前车摄像头支架的影响
#endif

