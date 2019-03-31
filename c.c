#include "ImageProcessing.h"
#include "include.h"
int Center_point0[100][2] = { 0 };//��¼������ĵ㣬���ܴ��ڼ�¼0�����ݣ�1�����ݣ��������
int Center_point1[100][2] = { 0 };
int Center_point2[100][2] = { 0 };
int Center_point3[100][2] = { 0 };
extern uint8 imgbuff[CAMERA_SIZE];      //imgbuff�ǲɼ��Ļ�������img�ǽ�ѹ��Ļ�������
extern uint8 img[CAMERA_H][CAMERA_W]; //imgbuff���ڲɼ�ͼ��img����ͼ����
//uint8 img1[CAMERA_H][CAMERA_W];    //�Աȴ���
//uint8 img2[CAMERA_H][CAMERA_W];     //����ͼ�����
//uint8 img3[CAMERA_H][CAMERA_W];
static int k1 = 0, k2 = 0, k3 = 0, k = 0; //��ֻ��һ����ʱ���ȡֵ��������˸�����ж����ʱ~~~

void IMAGE_PRO(void)
{
	unsigned int i = 0, j = 0;
	unsigned char *p = NULL;
	int n;
	int center_get = 0;
	int  left = 0, right = 0, middle = 0, width = 0, center_X = 0, center_Y = 0;
	k = 0;
	//��ѹ
	img_extract((uint8 *)img, (uint8 *)imgbuff, CAMERA_H*CAMERA_W / 8);
	//�˵��ϵ�
	for (i = 0; i < CAMERA_H; i++)                     //�˵��ϵ�
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
	// ��ͼ���������߱���ʺϼ���
	for (i = 0; i < 60; i++)
	{
		img[i][0] = img[i][79] = 0x00;
	}
	//��ȡ���ĵ�
	while (1)
	{
		for (i = 0; i < CAMERA_H; i++)
		{
			//��ȡ�ߵ�����
			if (center_get == 2)
			{//���м�������
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
				//����һ���ߵ�
				else
				{
					Center_point0[k][0] = center_X;//��0��ͼ�ĵ�k�����X����
					Center_point0[k][1] = center_Y;
					center_get = 0;
					k++;
					break;//�ѻ�ȡһ�����ĵ㣬��ʼ��һ��ɨ��
				}
			}
			//�Ұߵ�
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
						width++;  //�����߲�Ϊ0��
								  //����Image_Data_Black_spots[i]�Ĵ�С�жϿ��
						img[i][j] = 0x00;//ɨ����ĸ�0
					}
					else if (img[i][j] == 0xff && img[i][j + 1] == 0x00 && center_get == 1)
					{
						img[i][j] = 0x00;//ɨ����ĸ�0
						right = j;
						center_get = 2;
						middle = (left + right) / 2;

						break;   //һ��ֻɨһ���е�һ���ߵ�,�˳���һ��ɨ��
					}
					j++;
				}
				Center_point0[k][0] = middle;//��0��ͼ�ĵ�k�����X����
				Center_point0[k][1] = i;

			}
		}
		//����ͼ���ްߵ�,�˳�ѭ��
		if (center_get == 3)
		{
			center_get == 0;
			break;
		}
	}
}

void Flicker_recognition(void)   //��һֱ���ڵĵ��˵�
{

	for (i = 0; i < 100; i++)//���Ĵε�ֵ
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

	/*****************�Ա�ȷ��**********************/

	//���ֻ��һ�������0���㣬����������������
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

	//������ڶ�������һ����һֱ���ڣ��˵�һֱ���ڵĵ�