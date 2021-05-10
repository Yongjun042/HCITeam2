#include<opencv2/opencv.hpp>
#include<vector>
#include<iostream>

using namespace std;
using namespace cv;

int main()
{
	Mat input =imread("./sample.jpg");
	Mat hsv, h, s, v;
	Mat hls, h2, l, v2;
	cvtColor(input,hsv,COLOR_BGR2HSV);
	cvtColor(input, hls, COLOR_BGR2HLS);
	vector<Mat> hsvc, hlsc;
	split(hsv, hsvc);
	split(hls, hlsc);
	imshow("hls", hlsc[2]);
	imshow("hsv", hsvc[2]);
	//�׸��� �κ� ����
	Mat shadowMask;
	//�׸��� ����
	Scalar lows = Scalar(0, 0, 76.5);
	Scalar highs = Scalar(180, 255, 145.5);
	inRange(hsv, lows, highs, shadowMask);
	//imshow("shadowMask", shadowMask);
	//�˾� ����
	Mat shapemask;
	//�˾� ����
	Scalar low = Scalar(0, 0, 0);
	Scalar high = Scalar(180, 255, 25);
	inRange(hls, low, high, shapemask);
	//imshow("shapemask", shapemask);
	//�˾� ���� - �׸��� ����
	Mat ssmask;
	subtract(shapemask, shadowMask, ssmask);
	//imshow("ssmask", ssmask);
	//������ Ŭ��¡���� �ٵ��
	Mat ssmasked1, ssmasked2;
	Mat mask = getStructuringElement(MORPH_ELLIPSE,Size(2,2));
	Mat mask2 = getStructuringElement(MORPH_ELLIPSE,Size(8,8));
	morphologyEx(ssmask, ssmasked1, MORPH_CLOSE, mask);
	morphologyEx(ssmasked1, ssmasked1, MORPH_OPEN,mask);
	//imshow("ssmasked1", ssmasked1);
	morphologyEx(ssmasked1, ssmasked2, MORPH_CLOSE, mask2);
	//imshow("ssmasked2", ssmasked2);
	//�����̹������� �˾� �ڸ���
	cvtColor(ssmasked2, ssmasked2, COLOR_GRAY2BGR);
	Mat p1;
	p1 = input + ~ssmasked2;
	imshow("p1", p1);
	
	waitKey(0);

	return 0;
}