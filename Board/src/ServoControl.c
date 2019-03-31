#include "include.h"
#include "ServoControl.h"
#include "ImageProcessing.h"
#include "MotorControl.h"
#define kp 1.8//���Ե���һ��   //�����Լ���
#define ki 1.4//����
#define kd 0.8
#include  "devoce.h"
extern uint32 timevar;
extern int set_speed;
typedef struct
{
    int pre_error;                           
    int last_error;             
    int current_error;     
    }pid_s1;
extern  int    Axis_X ;   //��ȡ����������X�е� ���ƶ��
extern  int    Axis_Y ;   //���ݴ�С�жϾ�Ƶ�Զ�����Դ˿����ٶ�
extern  uint32 timevar;

short int PID_add; //PID���������


uint8 mode=1;

void Servo_ctl(uint16 ii)
{
 
    short int P1,I1,D1;  //����ֲ�����
    static  pid_s1 q;   // PID_m_add=0;
 
    q.pre_error=q.last_error;  //����ÿ�εĲ�ֵ
    q.last_error=q.current_error;//����ÿ�εĲ�ֵ
    q.current_error=ii-Axis_X;//����ÿ�εĲ�ֵ
    P1=(short int)(kp*(q.current_error-q.last_error));//����P�����ʽ
    I1=(short int)(ki*q.current_error);     //����I�����ʽ
    D1=(short int)(kd*(q.current_error-(2*q.last_error)+q.pre_error));//΢��D�����ʽ
    PID_add=(P1+I1+D1); 
    
   
    if(Axis_Y==0)//��������ʱת��
    {
 
    ftm_pwm_duty(FTM1 , FTM_CH0,285-52);//������60�����¶����60����ײ
   
    }
      else
        {
  
          if(PID_add>65 )
            PID_add=65;                   //���Ƶ��������ٶ�
          if(PID_add<-65)
            PID_add=-65;                     //���Ƶ������С�ٶ�
          ftm_pwm_duty(FTM1 , FTM_CH0,285-PID_add);         //���õ��ռ�ձ�
     
        }
}

void jude(uint16 max_speed)         //���ڵ���ٶ�

{
  if(max_speed==20)
    {
      if(Axis_Y==0)
        set_speed=13;
        else if(Axis_Y>=35)
            set_speed=13; 
            
                else if (Axis_Y<35&&Axis_Y>33)
                      set_speed=15;
                       else if (Axis_Y<33&&Axis_Y>27) 
                            set_speed=18;
                                   else 
                                          set_speed=max_speed;
    }
  if(max_speed==25)
    {
      if(Axis_Y==0)
        set_speed=13;
        else if(Axis_Y>=35)
            set_speed=13;       
                else if (Axis_Y<35&&Axis_Y>33)
                      set_speed=16;
                       else if (Axis_Y<33&&Axis_Y>30) 
                            set_speed=17;
                            else if (Axis_Y<30&&Axis_Y>27) 
                                set_speed=22;
                                   else 
                                     set_speed=max_speed;
    }
    if(max_speed==30)
        {
          if(Axis_Y==0)
            set_speed=13;
              else if(Axis_Y>=35)
                  set_speed=14; 
             else if (Axis_Y<35&&Axis_Y>33)
                      set_speed=16;
                     else if (Axis_Y<33&&Axis_Y>30) 
                            set_speed=19;
                            else if (Axis_Y<30&&Axis_Y>27) 
                                set_speed=22;
                                   else if (Axis_Y<27&&Axis_Y>20) 
                                       set_speed=25;
                                                else
                                                    set_speed=max_speed;
        }
}


void  derection(uint16 max_speed)       //����Ƕȿ���

{
    
  if(mode==1)                         //ת��Ȧ��������������ͷ��ƫ�ǣ���������ڵ��ұߣ��ͻῴ������
  {
    
    if(Axis_Y>0&&Axis_Y<10)
     Servo_ctl(38);
        else if(Axis_Y>=10&&Axis_Y<20)
          Servo_ctl(32);
            else
              Servo_ctl(29);
    
  }
  if(mode==0)
  {
       if(Axis_Y>0&&Axis_Y<26)     
            Servo_ctl(38);
                    else if(Axis_Y>=26&&Axis_Y<40)
                          Servo_ctl(37);
                         else
                              Servo_ctl(32);
    

  }
}
