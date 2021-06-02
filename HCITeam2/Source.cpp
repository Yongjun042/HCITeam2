#include<opencv2/opencv.hpp>
#include<vector>
#include<iostream>
// #include "extractLetters.h"
/*
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
int convert_mat(Mat image);
TessBaseAPI api;
*/
using namespace std;
using namespace cv;

Mat cropimage(Mat input, Mat mask)
{
	vector<vector<Point> > contours;
	findContours(mask, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
	int max_contour = -1;
	int max_area = -1;
	//drawContours(mask, contours, -1, (0, 255, 0), 3);
	imshow("asdf", mask);
	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		if (area > max_area && area<300000)
		{
			max_area = area;
			max_contour = i;
		}
	}
	return input(boundingRect(contours[max_contour]));

}

Mat getMask(Mat input)
{
	Mat hsv, h, s, v;
	Mat hls, h2, l, v2;

	//imshow("input", input);

	cvtColor(input, hsv, COLOR_BGR2HSV);
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
	Mat mask = getStructuringElement(MORPH_ELLIPSE, Size(2, 2));
	Mat mask2 = getStructuringElement(MORPH_ELLIPSE, Size(8, 8));
	morphologyEx(ssmask, ssmasked1, MORPH_CLOSE, mask);
	morphologyEx(ssmasked1, ssmasked1, MORPH_OPEN, mask);
	//imshow("ssmasked1", ssmasked1);
	morphologyEx(ssmasked1, ssmasked2, MORPH_CLOSE, mask2);
	//imshow("ssmasked2", ssmasked2);
	return ssmasked2;
}
/*
//mat image ocr 가능하게 바꿔줌
int convert_mat(Mat image) {
    // 이미지 로딩
    // imshow("Display window", image);
    // 개략적인 전처리
    Mat gray;
    cvtColor(image, gray, CV_BGR2GRAY);
    // Tesseract API로 입력시킴. 
  
    api.Init(NULL, "eng+kor", OEM_DEFAULT);
    api.SetPageSegMode(PSM_SINGLE_BLOCK);
    api.SetImage((uchar*)gray.data, gray.cols, gray.rows,1, gray.step);
    api.Recognize(0);
    waitKey(0);
    return 0;
}
*/
int main()
{
	 //parlinepilldata 4000장이라 testpill 10장정도 실험해봄
	/*
	// 한글 인코딩 문제 해결
    locale::global(std::locale("ko_KR.UTF-8")); 
    string outText;
    Mat p_img;


    String path("./testpill/*.jpg"); // jpg 확장자 파일만 읽음
    vector<String> str;
    // 이미지 저장을 위한 변수
    int index = 0;
    char buf[256];

    glob(path, str, false); // 파일 목록을 가져오는 glob 함수
                      // glob(찾을 파일 경로, 찾은 파일 경로, recusive(true or false)
                      // true : 폴더 내 하위 폴더 속 까지 파일을 찾음
                      // false : 폴더 내 파일을 찾음
    cout << "로드 개수 : " << str.size() << endl;
    if (str.size() == 0)
        cout << "이미지가 존재하지 않습니다.\n" << endl;
 // 하나씩 불러와서 처리해줌
    for (int cnt = 0; cnt < str.size(); cnt++)
    {
        p_img = imread(str[cnt]);
        //하나씩 ocr 함수에 넣어줌
        convert_mat(p_img);
        outText = string(api.GetUTF8Text());
        cout << "OUT Text: " << outText << endl;
        api.End();
        //string temp = to_string(cnt + 1);

    }
	*/
	Mat input =imread("./sample2.jpg");

	resize(input, input, cv::Size(500, 300), 0, 0, CV_INTER_NN);
	Mat mask = getMask(input);
	imshow("asdf", mask);
	//원본이미지에서 알약 자르기
	Mat crop = cropimage(input, mask);
	imshow("asdf2", crop);
	//불안정해서 임시 제거
	//Mat ssmasked2_bgr;
	//cvtColor(ssmasked2, ssmasked2_bgr, COLOR_GRAY2BGR);
	//Mat maskedImage;
	//maskedImage = input + ~ssmasked2_bgr;
	//imshow("maskedImage", maskedImage);

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
	Canny(input, p1_canny, 15, 60);
	imshow("p1_canny", p1_canny);

	// Hough Transform
	// 4번쨰 파라미터를 조정하여 선 검출 정도를 조절
	vector<Vec2f> lines;
	HoughLines(p1_canny, lines, 1, CV_PI / 180, 100);

	Mat img_hough;
	input.copyTo(img_hough);

	Mat img_lane;
	threshold(p1_canny, img_lane, 150, 255, THRESH_MASK);

	

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		

		std::cout <<"rho:"<< rho << " theta:" << theta << endl;
		if (theta==0 && rho<250 && rho>200)
		{
			std::cout << "평범 분할선이 있습니다." << endl;
		}


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

	std::cout <<"검출한 직선 개수:"<< lines.size();


	imshow("img_hough", img_hough);
	imshow("img_lane", img_lane);


	waitKey(0);

	return 0;
}
