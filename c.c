#include "ImageProcessing.h"
#include "include.h"
int Center_point0[100][2] = { 0 };//记录光斑中心点，可能存在记录0个数据，1个数据，多个数据
int Center_point1[100][2] = { 0 };
int Center_point2[100][2] = { 0 };
int Center_point3[100][2] = { 0 };
extern uint8 imgbuff[CAMERA_SIZE];      //imgbuff是采集的缓冲区，img是解压后的缓冲区。
extern uint8 img[CAMERA_H][CAMERA_W]; //imgbuff用于采集图像，img用于图像处理
//uint8 img1[CAMERA_H][CAMERA_W];    //对比处理
//uint8 img2[CAMERA_H][CAMERA_W];     //不存图，存点
//uint8 img3[CAMERA_H][CAMERA_W];
static int k1 = 0, k2 = 0, k3 = 0, k = 0; //在只有一个点时多次取值，忽略闪烁，在有多个点时~~~

void IMAGE_PRO(void)
{
	unsigned int i = 0, j = 0;
	unsigned char *p = NULL;
	int n;
	int center_get = 0;
	int  left = 0, right = 0, middle = 0, width = 0, center_X = 0, center_Y = 0;
	k = 0;
	//解压
	img_extract((uint8 *)img, (uint8 *)imgbuff, CAMERA_H*CAMERA_W / 8);
	//滤掉断点
	for (i = 0; i < CAMERA_H; i++)                     //滤掉断点
	{
		p = &img[i][1];
		while (p < &img[i][CAMERA_W - 1])
		{
			if (*(p - 1) == 0xff && *p == 0x00 && *(p + 1) == 0xff)
			{
				*p = 0xff;
			}
			else if (*(p - 1) == 0x00 && *p == 0xff && *(p + 1) == 0x00)
			{
				*p = 0x00;
			}
			p++;
		}
	}
	// 将图像左右两边变黑适合计算
	for (i = 0; i < 60; i++)
	{
		img[i][0] = img[i][79] = 0x00;
	}
	//提取中心点
	while (1)
	{
		for (i = 0; i < CAMERA_H; i++)
		{
			//提取斑点中心
			if (center_get == 2)
			{//从中间向左右
				if (img[i][middle] == 0xff)
				{
					img[i][middle] = 0x00;
					for (j = middle - 1; j > 0; j--)
					{

						if (img[i][j] == 0x00)
						{
							left = j - 1;
							break;
						}
						img[i][j] = 0x00;
					}
					for (j = middle + 1; j < CAMERA_W; j++)
					{
						if (img[i][j] == 0x00)
						{
							right = j - 1;
							break;
						}
						img[i][j] = 0x00;
					}
					if ((right - left) > width)
					{
						width = right - left;
						center_X = (right + left) / 2;
						center_Y = i;
					}
					middle = (right + left) / 2;
					left = 0;
					right = 0;

				}
				//结束一个斑点
				else
				{
					Center_point0[k][0] = center_X;//第0幅图的第k个点的X坐标
					Center_point0[k][1] = center_Y;
					center_get = 0;
					k++;
					break;//已获取一个中心点，开始下一次扫描
				}
			}
			//找斑点
			if (center_get == 0)
			{
				if (i == CAMERA_H - 1)
				{
					center_get = 3;

				}
				while (img[i][j] < &img[i][CAMERA_W - 1])
				{

					if (img[i][j] == 0x00 && img[i][j + 1] == 0xff && left == 0)
					{
						left = j + 1;
						center_get = 1;

					}
					if (img[i][j] == 0xff && img[i][j + 1] == 0xff && center_get = 1)
					{
						width++;  //有亮斑不为0，
								  //根据Image_Data_Black_spots[i]的大小判断宽度
						img[i][j] = 0x00;//扫描过的赋0
					}
					else if (img[i][j] == 0xff && img[i][j + 1] == 0x00 && center_get == 1)
					{
						img[i][j] = 0x00;//扫描过的赋0
						right = j;
						center_get = 2;
						middle = (left + right) / 2;

						break;   //一次只扫一行中的一个斑点,退出第一次扫描
					}
					j++;
				}
				Center_point0[k][0] = middle;//第0幅图的第k个点的X坐标
				Center_point0[k][1] = i;

			}
		}
		//本幅图已无斑点,退出循环
		if (center_get == 3)
		{
			center_get == 0;
			break;
		}
	}
}

void Flicker_recognition(void)   //把一直存在的点滤掉
{

	for (i = 0; i < 100; i++)//存四次的值
	{
		Center_point3[i][0] = Center_point2[i][0];
		Center_point3[i][1] = Center_point2[i][1];
		Center_point2[i][0] = Center_point1[i][0];
		Center_point2[i][1] = Center_point1[i][1];
		Center_point1[i][0] = Center_point0[i][0];
		Center_point1[i][1] = Center_point0[i][1];
		Center_point0[i][0] = 0;
		Center_point0[i][1] = 0;
	}

}



//}
	//else
	//  {
	//	if (k < 1 && k1 < 1 && k2 < 1 && k3 < 1)
	//	{
	//		if
	//	}
	//		
	//  }

	/*****************对比确认**********************/

	//如果只有一个点或是0个点，正常情况，按这个算
	/*
	for (j = 0; j<5; j++)
	{
	if (gete_Y[j]>0)
	num++;
	sum += gete_Y[j];
	}

	if (num == 0)
	Axis_Y = 0;
	else
	Axis_Y = sum / num;
	}
	*/

	//如果存在多个点或是一个点一直存在，滤掉一直存在的点