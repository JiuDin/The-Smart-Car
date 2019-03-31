#include "include.h"
#include  "devoce.h"


#include "common.h"
#include "include.h"


extern uint16 s_speed;
void Init()
{
    led_init(LED0);
     led_init(LED1);
   ftm_pwm_init(FTM0, FTM_CH3,20000,0);         
    ftm_pwm_init(FTM0, FTM_CH4,20000,0);  //���  
    ftm_pwm_init(FTM1, FTM_CH0,100,2000);  //���
    ftm_quad_init(FTM2);//��������
     ftm_pwm_duty(FTM1 , FTM_CH0,285);//����ȹ���
     gpio_init (PTA17, GPO,1);//��ʼ��GPIO
     gpio_init (PTE0,  GPI,0);
     gpio_init (PTE1,  GPI,0);
     gpio_init (PTE2,  GPI,0);
     gpio_init (PTE3,  GPI,0);
     gpio_init (PTD7,  GPI,1);
     gpio_init (PTD9,  GPI,1);
    NVIC_SetPriorityGrouping(2);                      //�����ж����ȼ�����
    NVIC_SetPriority(PIT0_IRQn, 2);
    NVIC_SetPriority(PIT1_IRQn, 3);
    NVIC_SetPriority(PORTA_IRQn , 0);
    NVIC_SetPriority(DMA0_IRQn, 1);
    pit_init_ms(PIT0, 1);              //��ʼ��PIT0����ʱʱ��Ϊ�� 1000ms
    pit_init_ms(PIT1, 1500);    
       
  
     
}

