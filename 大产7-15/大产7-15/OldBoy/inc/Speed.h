#ifndef _OLDBOY_SPEED_H_
#define _OLDBOY_SPEED_H_
void SpeedSet();                 //速度设置
void MotorOut(int PWMOut);      //电机输出
void SpeedPID();                //增量式PID
void DistanceRecord();          //路程记录
void BangBang();                //bangbang控制
void KaiHuan();                //电机开环运行
void PID_Test();                //速度PID矩形波测试
void PID_Test2();               //速度PID三角波测试
#endif 