#ifndef _OLDBOY_IMG_DEAL_H_
#define _OLDBOY_IMG_DEAL_H_
void BottomFind();                        //图像底部寻找，确定基准值
void GenZong();                          //找到边界线
void jisuan_Middle();                    //计算中线
void ForeSight();                        //确定前瞻
uint8 Refind_L(uint8 y);                 //再找一次左边界
uint8 Refind_R(uint8 y);                 //再找一次右边界
int  judge_crossxieR(uint8 judge);      //右斜出十字判断及补线
int  judge_crossxieL(uint8 judge);      //左斜出十字判断及补线
void judge_road();                      //判断赛道类型
float jisuan_piancha();                 //计算图像偏差
void StartLineCheck();                  //起跑线检测
void StraightCheck();                   //直线判断
void GoInBendCheck();                   //入弯判断
void CrossCheck();                      //十字弯检测
uint8 OvertakeAreaFind(uint8 judge);   //加宽超车区检测
uint8 OvertakeArea_ing(uint8 judge);        //进入超车区
uint8 OvertakeArea_Return(uint8 judge);//从超车区返回
void Overtake_LaterToFront();//超车区后车变前车
void Overtake_LaterReturn();//后车从超车区返回
void OvertakeArea_Over();   //结束超车
void Overtake_FrontReturn();//前车进入正常赛道
void Overtake_Middle();
void CurveType_Roll();                   //赛道类型滚动更新
void GenZong_Again_Right();             //前车和边界混在一起，再次寻找
void GenZong_Again_Left();
void Left_Correct(uint8 y);
void Right_Correct(uint8 y);
void Middle_Triangle();                 //三角超车区中线计算
void OutsideCheck();
void LCD_Find();
#endif 