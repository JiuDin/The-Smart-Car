#include "include.h"
#include "ServoControl.h"
#include "ImageProcessing.h"
#include "MotorControl.h"
#define kp 1.8//可以调低一点   //参数自己调
#define ki 1.4//调大
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
extern  int    Axis_X ;   //获取到的是亮斑X中点 控制舵机
extern  int    Axis_Y ;   //根据大小判断距灯的远近，以此控制速度
extern  uint32 timevar;

short int PID_add; //PID的增量输出


uint8 mode=1;

void Servo_ctl(uint16 ii)
{
 
    short int P1,I1,D1;  //定义局部变量
    static  pid_s1 q;   // PID_m_add=0;
 
    q.pre_error=q.last_error;  //更新每次的差值
    q.last_error=q.current_error;//更新每次的差值
    q.current_error=ii-Axis_X;//更新每次的差值
    P1=(short int)(kp*(q.current_error-q.last_error));//比例P输出公式
    I1=(short int)(ki*q.current_error);     //积分I输出公式
    D1=(short int)(kd*(q.current_error-(2*q.last_error)+q.pre_error));//微分D输出公式
    PID_add=(P1+I1+D1); 
    
   
    if(Axis_Y==0)//看不到灯时转弯
    {
 
    ftm_pwm_duty(FTM1 , FTM_CH0,285-52);//本来是60，但新舵机用60容易撞
   
    }
      else
        {
  
          if(PID_add>65 )
            PID_add=65;                   //限制电机的最大速度
          if(PID_add<-65)
            PID_add=-65;                     //限制电机的最小速度
          ftm_pwm_duty(FTM1 , FTM_CH0,285-PID_add);         //设置电机占空比
     
        }
}

void jude(uint16 max_speed)         //调节电机速度

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


void  derection(uint16 max_speed)       //舵机角度控制

{
    
  if(mode==1)                         //转空圈可能是由于摄像头有偏角，如果车不在灯右边，就会看不到灯
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
