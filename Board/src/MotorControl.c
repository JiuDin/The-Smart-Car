
/***************************************************
  ����������ڵ�����٣�PID�㷨��Ҷ���ֻ࣬�������ǿ���

�õĵ���Ƚ϶����ʲô��PID����Ҫ���ݲ�ͬ�����������Ӧ���˾Ͱ�P����

����ٶ�һֱ���¶����Ͱ�I���������н̳̣��Լ���

****************************************************/
#include "MotorControl.h"
#include "include.h"
#define kp  0.5   //0.5  28��ʱ�򻹲���
#define ki  0.25   //0.8
#define kd  0   //0.2
struct
{
    short int current_error;                  //��ǰ��ֵ
    short int last_error;                     //�ϴβ�ֵ
    short int prev_error;                     //���ϴβ�ֵ
}PID_M;                                 
short int PID_m_add;                         //PID���������
extern int speed;

void Motor_ctl(uint16 ii)
{
  
    short int P,I,D;                          //����ֲ�����
    PID_M.prev_error=PID_M.last_error;         //����ÿ�εĲ�ֵ
    PID_M.last_error=PID_M.current_error;      //����ÿ�εĲ�ֵ
    PID_M.current_error=ii-speed;               //����ÿ�εĲ�ֵ
    P=(short int)(kp*(PID_M.current_error-PID_M.last_error));//����P�����ʽ
    I=(short int)(ki*PID_M.current_error);     //����I�����ʽ
    D=(short int)(kd*(PID_M.current_error-(2*PID_M.last_error)+PID_M.prev_error));//΢��D�����ʽ
    PID_m_add=PID_m_add+(P+I+D);                //�����PID����ֵ���

   if(PID_m_add>0)
    {
      if(PID_m_add>80)
        PID_m_add=80;                              //���Ƶ��������ٶ�
      ftm_pwm_duty(FTM0 , FTM_CH3,0);              //���õ��ռ�ձ�
      ftm_pwm_duty(FTM0 , FTM_CH4,PID_m_add);
    }
      
   if(PID_m_add<0)                                 //�����PID_m_add�����ģ�ֻ�Ǹ������ͨ��
    {
     
      PID_m_add=-PID_m_add;                        //���Ƶ������С�ٶ�
      if(PID_m_add>80)
      PID_m_add=80; 
      ftm_pwm_duty(FTM0 , FTM_CH3,PID_m_add);     //���õ��ռ�ձ�
      ftm_pwm_duty(FTM0 , FTM_CH4,0);
    }
   
   }
