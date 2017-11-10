//2017.11.10
// demo_4.cpp : �������̨Ӧ�ó������ڵ�.
//����:��ȡ���֤������ο��Ե����ͨ������任����б�����ε�ͼƬ����У����ͶӰ������
//==========================================================
#include "stdafx.h"
#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/highgui/highgui.hpp>  
#include <iostream>  
#include <map>
#include "opencv/cv.h"  
#include "opencv/highgui.h"  
#include <opencv2/opencv.hpp>  
#include <tchar.h>  
#include <iostream>  
#include <fstream> 
#include <string>
#include <math.h>
#include <windows.h>
#include <atlstr.h>
using namespace std;
using namespace cv;
//�Զ�ֵ��ͼ���з�ת
Mat reverse(Mat biImage)
{
	Mat result_3 = biImage < 100;
	return result_3;
}
//��UTF-8����������ַ�ת��ΪASNI�����ʽ
//ԭ����Ϊͨ��Tesseract-OCRʶ������ֻ�����������UTF8�����ʽ�洢��txt�У�����Ϊ�����ʱ���������룬��ת����ANSI��ʽ�������
void UTF8toANSI(CString &strUTF8)  
{
	//��ȡת��Ϊ���ֽں���Ҫ�Ļ�������С���������ֽڻ�����  
	UINT nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8, -1, NULL, NULL);
	WCHAR *wszBuffer = new WCHAR[nLen + 1];
	nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8, -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;

	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	CHAR *szBuffer = new CHAR[nLen + 1];
	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;

	strUTF8 = szBuffer;
	//�����ڴ�  
	delete[]szBuffer;
	delete[]wszBuffer;
}
//===================================================
int main()
{
	string src_path;   //ԭʼͼƬ·��
	int src_width, src_height;
	cout << "����ͼƬ·����";
	cin >> src_path;
	Mat src = imread(src_path);
	src_width = src.size().width;
	src_height = src.size().height;

    //ʹ��mean_shift������ͻ��ǰ���ͱ����Ĳ��죬ͬʱ��������Ϊ������ȡ������׼��
	Mat mean_shift;
	pyrMeanShiftFiltering(src, mean_shift, 20, 20, 3);  //ԭʼ:30 , 40 , 3
	imwrite("mean_shift.jpg", mean_shift);

    //ת��Ϊ�Ҷ�ͼ
	Mat gray_image;
	cvtColor(mean_shift, gray_image, CV_BGR2GRAY);
	imwrite("gray.jpg", gray_image);

	//��ֵ��
	Mat biImage;
	adaptiveThreshold(gray_image, biImage, 255, CV_ADAPTIVE_THRESH_MEAN_C,
		CV_THRESH_BINARY_INV, 15, 10);  ///�ֲ�����Ӧ��ֵ������  
	imwrite("biImage.jpg", biImage);
    
    //contourImage��ʼ��һ����ԭʼͼƬ��ͬ��С��ͼ����󣬻Ҷ�ֵΪ0
	Mat contourImage(biImage.rows, biImage.cols, CV_8UC1, Scalar(0, 0, 0));
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//�ҳ�����������ĵ�洢��contours��
	findContours(biImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++)
	{
		drawContours(contourImage, contours, i, Scalar(255, 255, 255), 1, 8, hierarchy, 1);//��������
	}
	imwrite("lunkuo.jpg", contourImage);
    
    //����HoughLinesP�������������е�ֱ��
	vector<Vec4i>lines;
	HoughLinesP(contourImage, lines, 1, CV_PI / 180, 60, 60, 10);  //ԭʼֵ��1,CV_PI/180,50,50,10
	Mat img_lines;
	cvtColor(contourImage, img_lines, CV_GRAY2BGR);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i I = lines[i];
		line(img_lines, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 3, CV_AA);
	}
	imwrite("lunkuo_line.jpg", img_lines);

    //�����ĸ������ֱ�洢���ο������ĸ����ϵ�ֱ�߶�
	vector<Vec4i>up_ping_lines;
	vector<Vec4i>down_ping_lines;
	vector<Vec4i>left_shu_lines;
	vector<Vec4i>right_shu_lines;
	//int a = 0 , b = 0 , c = 0 , d = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i I = lines[i];
		//�������ֵ����Ϊ60
		if ((abs(I[2] - I[0]) > 60) && (abs(I[3] - I[1]) < 60))  //ˮƽֱ��
		{
			if (I[1] < (src_height / 2))
				up_ping_lines.push_back(I);
			else
				down_ping_lines.push_back(I);
		}
		else if (abs(I[2] - I[0] < 60) && (abs(I[3] - I[1]) > 60))  //��ֱֱ��
		{
			if (I[0] < (src_width / 2))
				left_shu_lines.push_back(I);
			else
				right_shu_lines.push_back(I);
		}
			
	}

	// =================�ҳ�����б��ͼƬ���ο���ĸ����㣬֮���ٲ��÷���任���н���=========================
	// ѡȡ�����ǣ�����������ϽǵĶ��㣬��������ֵ���ں������ڵ������ˮƽֱ�߶κ���ֱֱ�߶εĽ��㣬���������������ƣ�
	int left_up_x = 0, left_up_y = 0, right_up_x = 0, right_up_y = 0 , left_down_x = 0 , left_down_y = 0 , right_down_x = 0 , right_down_y = 0;
	float x_1, x_2, x_3, x_4, y_1, y_2, y_3, y_4;
	float a, b, c, d;
	//======================���Ͻ�����ֵ=====================================
	if (up_ping_lines.size() == 1)
	{
		x_1 = up_ping_lines[0][0];
		x_2 = up_ping_lines[0][2];
		y_1 = up_ping_lines[0][1];
		y_2 = up_ping_lines[0][3];
	}
	else
	{
		x_1 = up_ping_lines[0][0];
		x_2 = up_ping_lines[0][2];
		y_1 = up_ping_lines[0][1];
		y_2 = up_ping_lines[0][3];
		for (size_t i = 0; i < up_ping_lines.size() - 1; i++)
		{
			if (x_1 >= up_ping_lines[i + 1][0])
			{
				x_1 = up_ping_lines[i + 1][0];
				x_2 = up_ping_lines[i + 1][2];
				y_1 = up_ping_lines[i + 1][1];
				y_2 = up_ping_lines[i + 1][3];
			}
		}
	}
	if (left_shu_lines.size() == 1)
	{
		x_3 = left_shu_lines[0][0];
		x_4 = left_shu_lines[0][2];
		y_3 = left_shu_lines[0][1];
		y_4 = left_shu_lines[0][3];
	}
	else
	{
		x_3 = left_shu_lines[0][0];
		x_4 = left_shu_lines[0][2];
		y_3 = left_shu_lines[0][1];
		y_4 = left_shu_lines[0][3];
		for (size_t i = 0; i < left_shu_lines.size() - 1; i++)
		{
			if (y_3 >= left_shu_lines[i + 1][1])
			{
				x_3 = left_shu_lines[i + 1][0];
				x_4 = left_shu_lines[i + 1][2];
				y_3 = left_shu_lines[i + 1][1];
				y_4 = left_shu_lines[i + 1][3];
			}
		}
	}
	a = (y_2 - y_1) / (x_2 - x_1);
	b = (x_2 * y_1 - x_1 * y_2) / (x_2 - x_1);
	if (x_3 == x_4)  //�����Ͻ�����ֵ
	{
		left_up_x = x_3;
		left_up_y = a * x_3 + b;   
	}
	else
	{
		c = (y_4 - y_3) / (x_4 - x_3);
		d = (x_4 * y_3 - x_3 * y_4) / (x_4 - x_3);
		left_up_x = (d - b) / (a - c);
		left_up_y = (a * d - b * c) / (a - c);     
	}
//========================================================���Ͻ�����ֵ============================
	if (up_ping_lines.size() == 1)
	{
		x_1 = up_ping_lines[0][0];
		x_2 = up_ping_lines[0][2];
		y_1 = up_ping_lines[0][1];
		y_2 = up_ping_lines[0][3];
	}
	else
	{
		x_1 = up_ping_lines[0][0];
		x_2 = up_ping_lines[0][2];
		y_1 = up_ping_lines[0][1];
		y_2 = up_ping_lines[0][3];
		for (size_t i = 0; i < up_ping_lines.size() - 1; i++)
		{
			if (x_1 <= up_ping_lines[i + 1][0])
			{
				x_1 = up_ping_lines[i + 1][0];
				x_2 = up_ping_lines[i + 1][2];
				y_1 = up_ping_lines[i + 1][1];
				y_2 = up_ping_lines[i + 1][3];
			}
		}
	}
	if (right_shu_lines.size() == 1)
	{
		x_3 = right_shu_lines[0][0];
		x_4 = right_shu_lines[0][2];
		y_3 = right_shu_lines[0][1];
		y_4 = right_shu_lines[0][3];
	}
	else
	{
		x_3 = right_shu_lines[0][0];
		x_4 = right_shu_lines[0][2];
		y_3 = right_shu_lines[0][1];
		y_4 = right_shu_lines[0][3];
		for (size_t i = 0; i < right_shu_lines.size() - 1; i++)
		{
			if (y_3 >= right_shu_lines[i + 1][1])
			{
				x_3 = right_shu_lines[i + 1][0];
				x_4 = right_shu_lines[i + 1][2];
				y_3 = right_shu_lines[i + 1][1];
				y_4 = right_shu_lines[i + 1][3];
			}
		}
	}
	a = (y_2 - y_1) / (x_2 - x_1);
	b = (x_2 * y_1 - x_1 * y_2) / (x_2 - x_1);
	if ((x_4 - x_3) == 0)  //�����Ͻ�����ֵ
	{
		right_up_x = x_3;
		right_up_y = a * x_3 + b;
	}
	else
	{
		c = (y_4 - y_3) / (x_4 - x_3);
		d = (x_4 * y_3 - x_3 * y_4) / (x_4 - x_3);
		right_up_x = (d - b) / (a - c);
		right_up_y = (a * d - b * c) / (a - c);
	}
	//======================================================���½�����ֵ=============================================
	if (down_ping_lines.size() == 1)
	{
		x_1 = down_ping_lines[0][0];
		x_2 = down_ping_lines[0][2];
		y_1 = down_ping_lines[0][1];
		y_2 = down_ping_lines[0][3];
	}
	else
	{
		x_1 = down_ping_lines[0][0];
		x_2 = down_ping_lines[0][2];
		y_1 = down_ping_lines[0][1];
		y_2 = down_ping_lines[0][3];
		for (size_t i = 0; i < down_ping_lines.size() - 1; i++)
		{
			if (x_1 >= down_ping_lines[i + 1][0])
			{
				x_1 = down_ping_lines[i + 1][0];
				x_2 = down_ping_lines[i + 1][2];
				y_1 = down_ping_lines[i + 1][1];
				y_2 = down_ping_lines[i + 1][3];
			}
		}
	}
	if (left_shu_lines.size() == 1)
	{
		x_3 = left_shu_lines[0][0];
		x_4 = left_shu_lines[0][2];
		y_3 = left_shu_lines[0][1];
		y_4 = left_shu_lines[0][3];
	}
	else
	{
		x_3 = left_shu_lines[0][0];
		x_4 = left_shu_lines[0][2];
		y_3 = left_shu_lines[0][1];
		y_4 = left_shu_lines[0][3];
		for (size_t i = 0; i < left_shu_lines.size() - 1; i++)
		{
			if (y_3 <= left_shu_lines[i + 1][1])
			{
				x_3 = left_shu_lines[i + 1][0];
				x_4 = left_shu_lines[i + 1][2];
				y_3 = left_shu_lines[i + 1][1];
				y_4 = left_shu_lines[i + 1][3];
			}
		}
	}
	a = (y_2 - y_1) / (x_2 - x_1);
	b = (x_2 * y_1 - x_1 * y_2) / (x_2 - x_1);
	if ((x_4 - x_3) == 0)  //�����½�����ֵ
	{
		left_down_x = x_3;
		left_down_y = a * x_3 + b;
	}
	else
	{
		c = (y_4 - y_3) / (x_4 - x_3);
		d = (x_4 * y_3 - x_3 * y_4) / (x_4 - x_3);
		left_down_x = (d - b) / (a - c);
		left_down_y = (a * d - b * c) / (a - c);
	}
	//=====================================���½�����ֵ=====================================
	if (down_ping_lines.size() == 1)
	{
		x_1 = down_ping_lines[0][0];
		x_2 = down_ping_lines[0][2];
		y_1 = down_ping_lines[0][1];
		y_2 = down_ping_lines[0][3];
	}
	else
	{
		x_1 = down_ping_lines[0][0];
		x_2 = down_ping_lines[0][2];
		y_1 = down_ping_lines[0][1];
		y_2 = down_ping_lines[0][3];
		for (size_t i = 0; i < down_ping_lines.size() - 1; i++)
		{
			if (x_1 <= down_ping_lines[i + 1][0])
			{
				x_1 = down_ping_lines[i + 1][0];
				x_2 = down_ping_lines[i + 1][2];
				y_1 = down_ping_lines[i + 1][1];
				y_2 = down_ping_lines[i + 1][3];
			}
		}
	}
	if (right_shu_lines.size() == 1)
	{
		x_3 = right_shu_lines[0][0];
		x_4 = right_shu_lines[0][2];
		y_3 = right_shu_lines[0][1];
		y_4 = right_shu_lines[0][3];
	}
	else
	{
		x_3 = right_shu_lines[0][0];
		x_4 = right_shu_lines[0][2];
		y_3 = right_shu_lines[0][1];
		y_4 = right_shu_lines[0][3];
		for (size_t i = 0; i < right_shu_lines.size() - 1; i++)
		{
			if (y_3 <= right_shu_lines[i + 1][1])
			{
				x_3 = right_shu_lines[i + 1][0];
				x_4 = right_shu_lines[i + 1][2];
				y_3 = right_shu_lines[i + 1][1];
				y_4 = right_shu_lines[i + 1][3];
			}
		}
	}
	a = (y_2 - y_1) / (x_2 - x_1);
	b = (x_2 * y_1 - x_1 * y_2) / (x_2 - x_1);
	if ((x_4 - x_3) == 0)  //�����Ͻ�����ֵ
	{
		right_down_x = x_3;
		right_down_y = a * x_3 + b;
	}
	else
	{
		c = (y_4 - y_3) / (x_4 - x_3);
		d = (x_4 * y_3 - x_3 * y_4) / (x_4 - x_3);
		right_down_x = (d - b) / (a - c);
		right_down_y = (a * d - b * c) / (a - c);
	}
	//==============================================================================
	//��ǰ����ж�ֵ����ʱ�����ڼӿ����������Ŀ�������һ���������ֲ�������
	left_up_x = left_up_x + 7;
	left_up_y = left_up_y + 7;
	left_down_x = left_down_x + 7;
	left_down_y = left_down_y - 7;
	right_up_x = right_up_x - 7;
	right_up_y = right_up_y + 7;
	right_down_x = right_down_x - 7;
	right_down_y = right_down_y - 7;

    //points�洢���������ĸ����㣬rec�洢����任����Ŀ�궥�����꣬�������860*600�Ĵ�С������Ⱥ����֤��ʵ�����һ��
	vector<Point2f> points;
	vector<Point2f> rec;
	points.push_back(Point2f(left_up_x, left_up_y));
	points.push_back(Point2f(right_up_x, right_up_y));
	points.push_back(Point2f(left_down_x, left_down_y));
	points.push_back(Point2f(right_down_x, right_down_y));
	rec.push_back(Point2f(0, 0));
	rec.push_back(Point2f(859, 0));
	rec.push_back(Point2f(0, 599));
	rec.push_back(Point2f(859, 599));

	line(img_lines, points[0], points[1], Scalar(255, 0, 0), 1, CV_AA);
	line(img_lines, points[1], points[3], Scalar(255, 0, 0), 1, CV_AA);
	line(img_lines, points[3], points[2], Scalar(255, 0, 0), 1, CV_AA);
	line(img_lines, points[2], points[0], Scalar(255, 0, 0), 1, CV_AA);
	namedWindow("result_lines", 1);
	imshow("result_lines", img_lines);

	line(gray_image, points[0], points[1], Scalar(255, 0, 0), 1, CV_AA);
	line(gray_image, points[1], points[3], Scalar(255, 0, 0), 1, CV_AA);
	line(gray_image, points[3], points[2], Scalar(255, 0, 0), 1, CV_AA);
	line(gray_image, points[2], points[0], Scalar(255, 0, 0), 1, CV_AA);
	namedWindow("result_gray", 1);
	imshow("result_gray", gray_image);

	Mat result_warp;
	Size size(860, 600);
	Mat h = findHomography(points, rec);  //h�Ƿ���任����
	warpPerspective(gray_image, result_warp, h, size);  //����任
	namedWindow("result_warp", 1);
	imshow("result_warp", result_warp);
	waitKey();

    //������任���ͼƬͳһ����Ϊ1000*700��С��Ȼ����������ͨ��ѡȡ�̶�λ���и�������Ҫ������
	Mat final;
	resize(result_warp, final, Size(1000, 700));
	imwrite("final.jpg", final);
	Rect rect_1(400, 120, 400, 50);
	Rect rect_2(130, 175, 285, 50);
	Rect rect_3(625, 170, 285, 50);

	Mat part_ID, part_name, part_num;
	final(rect_1).copyTo(part_ID);
	final(rect_2).copyTo(part_name);
	final(rect_3).copyTo(part_num);
	imwrite("part_ID.jpg", part_ID); //�и����֤�Ų�λ
	imwrite("part_name.jpg", part_name); //������λ
	imwrite("part_num.jpg", part_num); //���ಿλ
	
	system("tesseract part_ID.jpg output_ID -psm 7");
	system("tesseract part_name.jpg output_name -l chi_sim -psm 7");
	system("tesseract part_num.jpg output_num -psm 7");

	string temp;
	ifstream txt_ID("output_ID.txt");
	getline(txt_ID, temp);
	cout << "���֤��:" << " " << temp << endl;


	CString str_1;
	string str_2;
	CString str_3;
	ifstream txt_name("output_name.txt", ios::in);
	getline(txt_name, str_2);
	str_1 = str_2.c_str();
	UTF8toANSI(str_1);
	//str_3 = str_1.Mid(1);
	cout << "����:" << str_1 << endl;


	ifstream txt_num("output_num.txt");
	getline(txt_num, temp);
	cout << "�����:" << " " << temp << endl;
	while (1);
	return 0;
}
