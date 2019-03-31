/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,山外科技
 *     All rights reserved.
 *     技术讨论：山外论坛 http://www.vcan123.com
 *
 *     除注明出处外，以下所有内容版权均属山外科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留山外科技的版权声明。
 *
 * @file       main.c
 * @brief      山外K60 平台主程序
 * @author     山外科技
 * @version    v5.0
 * @date       2013-08-28 12
 */

#include "common.h"
#include "include.h"
#include "MotorControl.h"
#include "ImageProcessing.h"
#include "ServoControl.h"
#include "devoce.h"  
uint8           D=0,Start=0;                                //D用于启动倒车，Start用于发车  
uint8           imgbuff[CAMERA_SIZE];                       //定义存储接收图像的数组
uint8           img[CAMERA_H][CAMERA_W];
int             speed;                                     //val
int             set_speed;                                //设置电机速度

extern  int    Axis_X ;                                 //获取到的是亮斑X中点 控制舵机
extern  int    Axis_Y ;                                 //主要用于调节电机速度
extern  uint8  mode;                                      //本是用来考虑调节转弯方向的变量
extern  uint8  DMA_Over_Flg;                             //置1时图像采集完成

void PORTA_IRQHandler();
void DMA0_IRQHandler();
void sendimg(void *imgaddr, uint32 imgsize);            //串口发送图像
void PIT0_IRQHandler(void);                             //用于正交解码
void PIT1_IRQHandler(void);                             //用于倒车


/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note      
 */
void  main(void)

{ 
   int time=0;                                          //标志位
   int ModeSpeed=20;                                    //设定速度
   camera_init(imgbuff);   
   Init();                                              //各模块初始化，舵机归中，在devoce.c文件定义
   if(gpio_get(PTE0)==1)                                //拨码开关的程序，如果没加拨码开关，注释掉这一段，同时去Init()注释掉初始化函数
   {
    mode=0;                                             //
    led (LED0,LED_ON);
    }
      else if(gpio_get(PTE1)==1)
        {
          ModeSpeed = 25;
          led (LED1,LED_ON);
        }
        else
            {
            }                                               //
   
   
    set_vector_handler(PIT1_VECTORn ,PIT1_IRQHandler);
    set_vector_handler(PIT0_VECTORn ,PIT0_IRQHandler);    //设置PIT0的中断服务函数为 PIT0_IRQHandler
    set_vector_handler(PORTA_VECTORn , PORTA_IRQHandler); //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
    set_vector_handler(DMA0_VECTORn , DMA0_IRQHandler);   //设置 DMA0 的中断服务函数为 PORTA_IRQHandler

    while(1)
    {
   
     camera_get_img();                                   //摄像头获取图像
    //  vcan_sendimg(imgbuff, CAMERA_SIZE);              //串口发送函数
      if(DMA_Over_Flg == 1)                             //采集完成一帧图像
         {
            DMA_Over_Flg = 0 ;
            IMAGE_PRO();                                 //图像处理函数，具体查看ImageProcessing.c
            Get_X();                         
             if(Start==0)                    
                { 
                 enable_irq (PIT0_IRQn);
                  Start=1;                              //启动发车
                  pit_time_start(PIT3);                 //启动时计时 ，用于避免倒车误判
                }
              if(Start==1)
                {
         
                derection(ModeSpeed);                     //角度控制
                jude(ModeSpeed);                          //速度控制
                if(pit_time_get_ms(PIT3)>1000&&time==0)   //只在开机1S后进入一次，用于避免倒车误判
                  {
                    D=1;
                    time=1;
                    pit_close(PIT3);
                   }
                            
                }
              if(Start==1&&speed==0&&D==1)               //估计你们用不上了
                {
                  disable_irq(PIT0_IRQn);
                  ftm_pwm_duty(FTM1 , FTM_CH0,285-30);   //舵机控制
                  ftm_pwm_duty(FTM0 , FTM_CH3,30);         
                  ftm_pwm_duty(FTM0 , FTM_CH4,0);//设置电机占空比
                  DELAY_MS(400);                
                  D=0;
                  enable_irq(PIT0_IRQn);                 
                  enable_irq(PIT1_IRQn);
              
            
                }
         //  printf("x=%d,  Y is %d  \n", Axis_X,Axis_Y);
               
        }

    }
}



/*!
 *  @brief      PORTA中断服务函数
 *  @since      v5.0
 */
void PORTA_IRQHandler()
{
    uint8  n;    //引脚号
    uint32 flag;

    while(!PORTA_ISFR);
    flag = PORTA_ISFR;
    PORTA_ISFR  = ~0;                                   //清中断标志位

    n = 29;                                             //场中断
    if(flag & (1 << n))                                 //PTA29触发中断
    {
        camera_vsync();
    }
#if ( CAMERA_USE_HREF == 1 )                            //使用行中断
    n = 28;
    if(flag & (1 << n))                                 //PTA28触发中断
    {
        camera_href();
    }
#endif


}

/*!
 *  @brief      DMA0中断服务函数
 *  @since      v5.0
 */
void DMA0_IRQHandler()
{
    camera_dma();
}


//发送图像到上位机显示
//不同的上位机，不同的命令，这里使用 yy_摄像头串口调试 软件
//如果使用其他上位机，则需要修改代码
void sendimg(void *imgaddr, uint32 imgsize)
{
    uint8 cmd[4] = {0, 255, 1, 0 };    //yy_摄像头串口调试 使用的命令

    uart_putbuff(VCAN_PORT, (uint8_t *)cmd, sizeof(cmd));    //先发送命令

    uart_putbuff(VCAN_PORT, imgaddr, imgsize); //再发送图像
}

/*!
 *  @brief      正交解码
 *  @since      v5.0
 */
void PIT0_IRQHandler(void)
{

    int16 val;
   
    val = ftm_quad_get(FTM2); //获取FTM 正交解码 的脉冲数(负数表示反方向)
    if(val>0)
      {
        speed=val;
      }
    if(val==0)
      speed=0;
    if(val<-5)
      { 
  
        speed=-val;
      }
    ftm_quad_clean(FTM2);
    Motor_ctl(set_speed);//设定脉冲数   
    PIT_Flag_Clear(PIT0);       //清中断标志位
      
}

void PIT1_IRQHandler(void)  //用于倒车
{
     disable_irq(PIT1_IRQn);
     D=1;
     PIT_Flag_Clear(PIT1);       //清中断标志位
   
}