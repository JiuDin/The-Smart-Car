#include "ImageProcessing.h"
#include "include.h"
uint8 Image_Data_Black_spots[CAMERA_H];   
int    Axis_X = 0;   //��ȡ����������X�е� ���ƶ��
int    Axis_Y = 0;    //��ȡ����������Y�е� ���Ƶ��
extern uint8 imgbuff[CAMERA_SIZE];      //imgbuff�ǲɼ��Ļ�������img�ǽ�ѹ��Ļ�������
                                  
extern uint8 img[CAMERA_H][CAMERA_W]; //imgbuff���ڲɼ�ͼ��img����ͼ����. 

int gete_X[5]={0};
int gete_Y[5]={0};

void IMAGE_PRO(void)
{
        unsigned int i=0;  
	unsigned char *p=NULL;
	int n;
      
        int j,num=0;
        int sum=0;
        Axis_Y=0;
        img_extract((uint8 *)img,(uint8 *) imgbuff, CAMERA_H*CAMERA_W/8);    //��ѹ
        for(j=3;j>0;j--)
        {
        gete_Y[j]=gete_Y[j-1];
   
        }
	for (i = 0;i < CAMERA_H;i++)                                         //�˵��������ĵ�
        {               
		p = &img[i][1];
		while (p < &img[i][CAMERA_W-1])
                {
		  if (*(p-1) == 0xff && *p == 0x00 && *(p+1) ==0xff)
                  {
			  *p = 0xff;
                  }
                  else if (*(p-1) == 0x00 && *p ==0xff && *(p+1) ==0x00)
                  {
			*p = 0x00;
                  }
                  p++;
		}
	}
        
        

         //----------------------------------------��ȡ�ű��Y����------------------------------------------------//
        for(i=0;i<CAMERA_H;i++) 
        {
           Image_Data_Black_spots[i] = 0;//����ֵ
        }
        for (i = 0;i < CAMERA_H;i++)
        {               
          p = &img[i][1];
          while (p < &img[i][CAMERA_W-1])
            {
		  if (*p == 0xff && *(p+1) ==0xff)
                  {
			 Image_Data_Black_spots[i]++;  //�����߲�Ϊ0��
                         //����Image_Data_Black_spots[i]�Ĵ�С�жϿ��
                  }
                  p++;
		}
                if(i == 0){
                  n=Image_Data_Black_spots[0];
                }
                else if(Image_Data_Black_spots[i] > n )
                {
                  n=Image_Data_Black_spots[i];  //   nΪ�����
                  Axis_Y = i;                   // i Ӧ�����������ĵ�������
             }
             
	}  
     
      gete_Y[0]=  Axis_Y;                     //��Ϊ�ű������˸�ģ�ȡ���ͼ��Ȼ������ͼ�����������ȡƽ��ֵ
      for(j=0;j<5;j++)
        {
          if(gete_Y[j]>0)
          num++;
          sum+=gete_Y[j];
        }
  
      if(num==0)
          Axis_Y=0;
          else
        Axis_Y=sum/num;
}

//----------------------------------------��ȡX������------------------------------------------------//
void Get_X(void)
  {
    int i,j,num=0;
 
    int sum=0;
    Axis_X=0;
    int start=0,end=0;
    for(i=0;i<60;i++)// ��ͼ���������߱���ʺϼ��� �����BUG
      {
          img[i][0] =img[i][79]=0x00;  
      }
    for(i = 0; i < CAMERA_W; i++ )
      {
        if(img[Axis_Y][i] == 0x00 && img[Axis_Y][i+1] ==0xff)
        start=i;
        if(img[Axis_Y][i] == 0xff && img[Axis_Y][i+1] ==0x00)
        end=i;
    
      }
   
    for(j=4;j>0;j--)
      {
        gete_X[j]=gete_X[j-1];
      }
      gete_X[0]=(start+end)/2;
    for(j=0;j<5;j++)
      {
        if(gete_X[j]>0)
          num++;
        sum+=gete_X[j];
      }
  
    if(num==0)
        Axis_X=0;
        else
            Axis_X=sum/num;             //ͬ��
  
// printf("sum=%d\n",sum);
}
