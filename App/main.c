/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,ɽ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�ɽ����̳ http://www.vcan123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����ɽ��Ƽ����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣��ɽ��Ƽ��İ�Ȩ������
 *
 * @file       main.c
 * @brief      ɽ��K60 ƽ̨������
 * @author     ɽ��Ƽ�
 * @version    v5.0
 * @date       2013-08-28 12
 */

#include "common.h"
#include "include.h"
#include "MotorControl.h"
#include "ImageProcessing.h"
#include "ServoControl.h"
#include "devoce.h"  
uint8           D=0,Start=0;                                //D��������������Start���ڷ���  
uint8           imgbuff[CAMERA_SIZE];                       //����洢����ͼ�������
uint8           img[CAMERA_H][CAMERA_W];
int             speed;                                     //val
int             set_speed;                                //���õ���ٶ�

extern  int    Axis_X ;                                 //��ȡ����������X�е� ���ƶ��
extern  int    Axis_Y ;                                 //��Ҫ���ڵ��ڵ���ٶ�
extern  uint8  mode;                                      //�����������ǵ���ת�䷽��ı���
extern  uint8  DMA_Over_Flg;                             //��1ʱͼ��ɼ����

void PORTA_IRQHandler();
void DMA0_IRQHandler();
void sendimg(void *imgaddr, uint32 imgsize);            //���ڷ���ͼ��
void PIT0_IRQHandler(void);                             //������������
void PIT1_IRQHandler(void);                             //���ڵ���


/*!
 *  @brief      main����
 *  @since      v5.0
 *  @note      
 */
void  main(void)

{ 
   int time=0;                                          //��־λ
   int ModeSpeed=20;                                    //�趨�ٶ�
   camera_init(imgbuff);   
   Init();                                              //��ģ���ʼ����������У���devoce.c�ļ�����
   if(gpio_get(PTE0)==1)                                //���뿪�صĳ������û�Ӳ��뿪�أ�ע�͵���һ�Σ�ͬʱȥInit()ע�͵���ʼ������
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
    set_vector_handler(PIT0_VECTORn ,PIT0_IRQHandler);    //����PIT0���жϷ�����Ϊ PIT0_IRQHandler
    set_vector_handler(PORTA_VECTORn , PORTA_IRQHandler); //���� PORTA ���жϷ�����Ϊ PORTA_IRQHandler
    set_vector_handler(DMA0_VECTORn , DMA0_IRQHandler);   //���� DMA0 ���жϷ�����Ϊ PORTA_IRQHandler

    while(1)
    {
   
     camera_get_img();                                   //����ͷ��ȡͼ��
    //  vcan_sendimg(imgbuff, CAMERA_SIZE);              //���ڷ��ͺ���
      if(DMA_Over_Flg == 1)                             //�ɼ����һ֡ͼ��
         {
            DMA_Over_Flg = 0 ;
            IMAGE_PRO();                                 //ͼ������������鿴ImageProcessing.c
            Get_X();                         
             if(Start==0)                    
                { 
                 enable_irq (PIT0_IRQn);
                  Start=1;                              //��������
                  pit_time_start(PIT3);                 //����ʱ��ʱ �����ڱ��⵹������
                }
              if(Start==1)
                {
         
                derection(ModeSpeed);                     //�Ƕȿ���
                jude(ModeSpeed);                          //�ٶȿ���
                if(pit_time_get_ms(PIT3)>1000&&time==0)   //ֻ�ڿ���1S�����һ�Σ����ڱ��⵹������
                  {
                    D=1;
                    time=1;
                    pit_close(PIT3);
                   }
                            
                }
              if(Start==1&&speed==0&&D==1)               //���������ò�����
                {
                  disable_irq(PIT0_IRQn);
                  ftm_pwm_duty(FTM1 , FTM_CH0,285-30);   //�������
                  ftm_pwm_duty(FTM0 , FTM_CH3,30);         
                  ftm_pwm_duty(FTM0 , FTM_CH4,0);//���õ��ռ�ձ�
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
 *  @brief      PORTA�жϷ�����
 *  @since      v5.0
 */
void PORTA_IRQHandler()
{
    uint8  n;    //���ź�
    uint32 flag;

    while(!PORTA_ISFR);
    flag = PORTA_ISFR;
    PORTA_ISFR  = ~0;                                   //���жϱ�־λ

    n = 29;                                             //���ж�
    if(flag & (1 << n))                                 //PTA29�����ж�
    {
        camera_vsync();
    }
#if ( CAMERA_USE_HREF == 1 )                            //ʹ�����ж�
    n = 28;
    if(flag & (1 << n))                                 //PTA28�����ж�
    {
        camera_href();
    }
#endif


}

/*!
 *  @brief      DMA0�жϷ�����
 *  @since      v5.0
 */
void DMA0_IRQHandler()
{
    camera_dma();
}


//����ͼ����λ����ʾ
//��ͬ����λ������ͬ���������ʹ�� yy_����ͷ���ڵ��� ���
//���ʹ��������λ��������Ҫ�޸Ĵ���
void sendimg(void *imgaddr, uint32 imgsize)
{
    uint8 cmd[4] = {0, 255, 1, 0 };    //yy_����ͷ���ڵ��� ʹ�õ�����

    uart_putbuff(VCAN_PORT, (uint8_t *)cmd, sizeof(cmd));    //�ȷ�������

    uart_putbuff(VCAN_PORT, imgaddr, imgsize); //�ٷ���ͼ��
}

/*!
 *  @brief      ��������
 *  @since      v5.0
 */
void PIT0_IRQHandler(void)
{

    int16 val;
   
    val = ftm_quad_get(FTM2); //��ȡFTM �������� ��������(������ʾ������)
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
    Motor_ctl(set_speed);//�趨������   
    PIT_Flag_Clear(PIT0);       //���жϱ�־λ
      
}

void PIT1_IRQHandler(void)  //���ڵ���
{
     disable_irq(PIT1_IRQn);
     D=1;
     PIT_Flag_Clear(PIT1);       //���жϱ�־λ
   
}