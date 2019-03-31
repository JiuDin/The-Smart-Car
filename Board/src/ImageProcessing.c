#include "ImageProcessing.h"
#include "include.h"
uint8 Image_Data_Black_spots[CAMERA_H];   
int    Axis_X = 0;   //获取到的是亮斑X中点 控制舵机
int    Axis_Y = 0;    //获取到的是亮斑Y中点 控制电机
extern uint8 imgbuff[CAMERA_SIZE];      //imgbuff是采集的缓冲区，img是解压后的缓冲区。
                                  
extern uint8 img[CAMERA_H][CAMERA_W]; //imgbuff用于采集图像，img用于图像处理. 

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
        img_extract((uint8 *)img,(uint8 *) imgbuff, CAMERA_H*CAMERA_W/8);    //解压
        for(j=3;j>0;j--)
        {
        gete_Y[j]=gete_Y[j-1];
   
        }
	for (i = 0;i < CAMERA_H;i++)                                         //滤掉不连续的点
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
        
        

         //----------------------------------------获取信标灯Y坐标------------------------------------------------//
        for(i=0;i<CAMERA_H;i++) 
        {
           Image_Data_Black_spots[i] = 0;//赋初值
        }
        for (i = 0;i < CAMERA_H;i++)
        {               
          p = &img[i][1];
          while (p < &img[i][CAMERA_W-1])
            {
		  if (*p == 0xff && *(p+1) ==0xff)
                  {
			 Image_Data_Black_spots[i]++;  //有亮斑不为0，
                         //根据Image_Data_Black_spots[i]的大小判断宽度
                  }
                  p++;
		}
                if(i == 0){
                  n=Image_Data_Black_spots[0];
                }
                else if(Image_Data_Black_spots[i] > n )
                {
                  n=Image_Data_Black_spots[i];  //   n为最大宽度
                  Axis_Y = i;                   // i 应该是亮斑中心的行坐标
             }
             
	}  
     
      gete_Y[0]=  Axis_Y;                     //因为信标灯是闪烁的，取多幅图像然后在有图像的坐标里面取平均值
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

//----------------------------------------获取X轴坐标------------------------------------------------//
void Get_X(void)
  {
    int i,j,num=0;
 
    int sum=0;
    Axis_X=0;
    int start=0,end=0;
    for(i=0;i<60;i++)// 将图像左右两边变黑适合计算 不会出BUG
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
            Axis_X=sum/num;             //同上
  
// printf("sum=%d\n",sum);
}
