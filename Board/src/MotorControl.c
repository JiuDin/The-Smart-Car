
/***************************************************
  这个函数用于电机调速，PID算法大家都差不多，只不过你们可能

用的电机比较多或是什么。PID参数要根据不同电机调整，响应慢了就把P调大

电机速度一直上下抖动就把I调大，网上有教程，自己查

****************************************************/
#include "MotorControl.h"
#include "include.h"
#define kp  0.5   //0.5  28的时候还不错
#define ki  0.25   //0.8
#define kd  0   //0.2
struct
{
    short int current_error;                  //当前差值
    short int last_error;                     //上次差值
    short int prev_error;                     //上上次差值
}PID_M;                                 
short int PID_m_add;                         //PID的增量输出
extern int speed;

void Motor_ctl(uint16 ii)
{
  
    short int P,I,D;                          //定义局部变量
    PID_M.prev_error=PID_M.last_error;         //更新每次的差值
    PID_M.last_error=PID_M.current_error;      //更新每次的差值
    PID_M.current_error=ii-speed;               //更新每次的差值
    P=(short int)(kp*(PID_M.current_error-PID_M.last_error));//比例P输出公式
    I=(short int)(ki*PID_M.current_error);     //积分I输出公式
    D=(short int)(kd*(PID_M.current_error-(2*PID_M.last_error)+PID_M.prev_error));//微分D输出公式
    PID_m_add=PID_m_add+(P+I+D);                //电机的PID增量值输出

   if(PID_m_add>0)
    {
      if(PID_m_add>80)
        PID_m_add=80;                              //限制电机的最大速度
      ftm_pwm_duty(FTM0 , FTM_CH3,0);              //设置电机占空比
      ftm_pwm_duty(FTM0 , FTM_CH4,PID_m_add);
    }
      
   if(PID_m_add<0)                                 //输出的PID_m_add是正的，只是改了输出通道
    {
     
      PID_m_add=-PID_m_add;                        //限制电机的最小速度
      if(PID_m_add>80)
      PID_m_add=80; 
      ftm_pwm_duty(FTM0 , FTM_CH3,PID_m_add);     //设置电机占空比
      ftm_pwm_duty(FTM0 , FTM_CH4,0);
    }
   
   }
