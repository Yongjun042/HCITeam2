#include<opencv2/opencv.hpp>
#include<vector>
#include<iostream>


using namespace std;
using namespace cv;

int main()
{
	Mat input =imread("./색다른_크롭_분할선.jpg");
	Mat hsv, h, s, v;
	Mat hls, h2, l, v2;
	
	resize(input, input, cv::Size(500, 300), 0, 0, CV_INTER_NN);
	
	imshow("input", input);

	cvtColor(input,hsv,COLOR_BGR2HSV);
	cvtColor(input, hls, COLOR_BGR2HLS);
	
	vector<Mat> hsvc, hlsc;
	
	split(hsv, hsvc);
	split(hls, hlsc);
	
	//imshow("hls", hlsc[2]);
	//imshow("hsv", hsvc[2]);
	
	//그림자 부분 추출
	Mat shadowMask;
	
	//그림자 범위
	Scalar lows = Scalar(0, 0, 76.5);
	Scalar highs = Scalar(180, 255, 145.5);
	inRange(hsv, lows, highs, shadowMask);
	
	//imshow("shadowMask", shadowMask);
	//알약 추출
	Mat shapemask;
	
	//알약 범위
	Scalar low = Scalar(0, 0, 0);
	Scalar high = Scalar(180, 255, 30);
	inRange(hls, low, high, shapemask);
	//imshow("shapemask", shapemask);

	//알약 범위 - 그림자 범위
	Mat ssmask;
	subtract(shapemask, shadowMask, ssmask);
	//imshow("ssmask", ssmask);
	//오프닝 클로징으로 다듬기
	Mat ssmasked1, ssmasked2;
	Mat mask = getStructuringElement(MORPH_ELLIPSE,Size(2,2));
	Mat mask2 = getStructuringElement(MORPH_ELLIPSE,Size(8,8));
	morphologyEx(ssmask, ssmasked1, MORPH_CLOSE, mask);
	morphologyEx(ssmasked1, ssmasked1, MORPH_OPEN,mask);
	//imshow("ssmasked1", ssmasked1);
	morphologyEx(ssmasked1, ssmasked2, MORPH_CLOSE, mask2);
	//imshow("ssmasked2", ssmasked2);
	
	//원본이미지에서 알약 자르기
	cvtColor(ssmasked2, ssmasked2, COLOR_GRAY2BGR);
	Mat p1;
	p1 = input + ~ssmasked2;
	//imshow("p1", p1);

	// 알약 영역에서 음각 추출
	// Sobel Edge
	Mat p1_sobel_x;
	Sobel(input, p1_sobel_x, -1, 1, 0);
	Mat p1_sobel_y;
	Sobel(input, p1_sobel_y, -1, 0, 1);

	imshow("sobel_x", p1_sobel_x);
	imshow("sobel_y", p1_sobel_y);
	imshow("sobel_p1", p1_sobel_x + p1_sobel_y);



	// Laplacian Edge
	Mat p1_laplacian;
	Laplacian(input, p1_laplacian, CV_8U);
	imshow("Laplacian_p1", p1_laplacian);

	// Canny Edge detection
	Mat p1_canny;
	Canny(input, p1_canny, 25, 50);
	imshow("p1_canny", p1_canny);

	// Hough Transform
	// 4번쨰 파라미터를 조정하여 선 검출 정도를 조절
	vector<Vec2f> lines;
	HoughLines(p1_canny, lines, 1, CV_PI / 180, 150);

	Mat img_hough;
	input.copyTo(img_hough);

	Mat img_lane;
	threshold(p1_canny, img_lane, 150, 255, THRESH_MASK);

	std::cout<<lines.size();

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(img_hough, pt1, pt2, Scalar(0, 0, 255), 2, 8);
		line(img_lane, pt1, pt2, Scalar::all(255), 1, 8);
	}

	imshow("img_hough", img_hough);
	imshow("img_lane", img_lane);


	waitKey(0);

	return 0;
}
