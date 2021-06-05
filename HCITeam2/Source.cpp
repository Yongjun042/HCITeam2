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

//get contour of mask
vector<Point> getContours(Mat mask)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
	int max_contour = -1;
	int max_area = -1;
	if (!contours.empty() && !hierarchy.empty()) {

		// loop through the contours/hierarchy
		for (int i = 0; i < contours.size(); i++) {

			// look for hierarchy[i][3]!=-1, ie hole boundaries
			if (hierarchy[i][3] == -1) {

				int area = contourArea(contours[i]);
				if (area > max_area)
				{
					max_area = area;
					max_contour = i;
				}
			}
		}
	}
	//drawContours(mask, contours, -1, (0, 255, 0), 3);
	//drawContours(input, contours, -1, (0, 255, 0), 3);
	//imshow("asdf", input);
	return contours[max_contour];
}

//get contours of mask
vector<vector<Point> > getContours(Mat mask, int a)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
	int max_contour = -1;
	int max_area = -1;
	int max_area2 = 2;
	//drawContours(mask, contours, -1, (0, 255, 0), 3);
	//drawContours(input, contours, -1, (0, 255, 0), 3);
	//imshow("asdf", input);
	int max_contour2 = 0;
	Mat img(mask.rows, mask.cols, CV_8U);
	if (!contours.empty() && !hierarchy.empty()) {

		// loop through the contours/hierarchy
		for (int i = 0; i < contours.size(); i++) {

			// look for hierarchy[i][3]!=-1, ie hole boundaries
			if (hierarchy[i][3] == -1) {
				drawContours(img, contours, i, Scalar(155, 230, 70));
				int area = contourArea(contours[i]);
				if (area > max_area)
				{
					int area = contourArea(contours[i]);
					max_area2 = max_area;
					max_area = area;
					max_contour2 = max_contour;
					max_contour = i;
				}
				else if (area > max_area2)
				{
					max_area2 = area;
					max_contour2 = i;
				}
			}
		}
	}
	//imshow("conts", img);
	vector<vector<Point> > retval;
	retval.push_back(contours[max_contour]);
	retval.push_back(contours[max_contour2]);
	return retval;
}

// find inner rect fron contoured image
Rect findMinRect(const Mat1b& src)
{
	Mat1f W(src.rows, src.cols, float(0));
	Mat1f H(src.rows, src.cols, float(0));

	Rect maxRect(0, 0, 0, 0);
	float maxArea = 0.f;

	for (int r = 0; r < src.rows; ++r)
	{
		for (int c = 0; c < src.cols; ++c)
		{
			if (src(r, c) == 0)
			{
				H(r, c) = 1.f + ((r > 0) ? H(r - 1, c) : 0);
				W(r, c) = 1.f + ((c > 0) ? W(r, c - 1) : 0);
			}

			float minw = W(r, c);
			for (int h = 0; h < H(r, c); ++h)
			{
				minw = min(minw, W(r - h, c));
				float area = (h + 1) * minw;
				if (area > maxArea)
				{
					maxArea = area;
					maxRect = Rect(Point(c - minw + 1, r - h), Point(c + 1, r + 1));
				}
			}
		}
	}

	return maxRect;
}

// crop inner text from pill
Mat croptext(Mat img, vector<Point> cont, int shape = 0)
{
	// Create a mask for each single blob
	Mat maskSingleContour(img.rows, img.cols,CV_8U);
	maskSingleContour = Scalar(0, 0, 0);
	vector< vector<Point> > cont2;
	cont2.push_back(cont);
	drawContours(maskSingleContour, cont2, 0, Scalar(255), FILLED);

	Rect box = findMinRect(~maskSingleContour);
	//Adjust box size
	if (box.width > box.height * 2 && shape !=5)
	{

		box.x = box.x+(box.width / 5);
		box.y = box.y + 5;
		box.width = 7*box.width/10;
		box.height = box.height - 5;
	}
	else
	{
		box.x = box.x + 5;
		box.y = box.y + 5;
		box.width = box.width - 5;
		box.height = box.height - 5;
	}
	// Draw rect
	Scalar rectColor(234, 22, 100);
	rectangle(img, box, rectColor, 2);

	return img(box);
}

// get a outline of phill
Mat getMask(Mat input)
{
	Mat gray_input;
	cv::cvtColor(input, gray_input, COLOR_BGR2GRAY);
	//imshow("gray", gray_input);
	Mat morph;
	Mat mask = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2), cv::Point(1, 1));
	morphologyEx(gray_input, morph, MORPH_BLACKHAT, mask);
	//imshow("morph", morph);
	//imshow("minus", gray_input+(morph));
	Mat morph2;
	morphologyEx(gray_input + (morph), morph2, MORPH_BLACKHAT, mask);
	//imshow("morph2", morph2);
	Mat morph3;
	morphologyEx(gray_input + (morph)+(morph2), morph3, MORPH_BLACKHAT, mask);
	//imshow("morph3", morph3);
	Mat thresh_2;
	Mat thresh_input;
	//imshow("mf", gray_input + morph +(morph2)+morph3);
	cv::adaptiveThreshold(gray_input + morph, thresh_input, 255,
		cv::ADAPTIVE_THRESH_GAUSSIAN_C,
		cv::THRESH_BINARY, 35, 3);
	//cv::adaptiveThreshold(gray_input , thresh_2, 255,
		//cv::ADAPTIVE_THRESH_GAUSSIAN_C,
		//cv::THRESH_BINARY, 93, 3);
	imshow("adapt", ~thresh_input);
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	Mat asdf, asdf2, asdf3,asdf4;
	morphologyEx(~thresh_input, asdf, MORPH_CLOSE, kernel2);
	//imshow("asdf", asdf);
	morphologyEx(asdf, asdf2, MORPH_OPEN, kernel);
	//imshow("asdf2", asdf2);
	morphologyEx(asdf2, asdf3, MORPH_CLOSE, kernel2);
	//imshow("asdf3", asdf3);
	morphologyEx(asdf3, asdf4, MORPH_OPEN, kernel);
	imshow("asdf4", asdf4);
	//imshow("adapt2", ~thresh_2);
	return asdf4;
}

//find similar shape
int matchShape(Mat img, vector<Point> cont)
{
	Mat shapes[] = { imread("./shape/circle.png",IMREAD_GRAYSCALE), imread("./shape/hexagon.png",IMREAD_GRAYSCALE), imread("./shape/oval.png",IMREAD_GRAYSCALE), imread("./shape/rounded.png",IMREAD_GRAYSCALE) , imread("./shape/soft.png",IMREAD_GRAYSCALE), imread("./shape/capsule.png",IMREAD_GRAYSCALE) };
	String shapeName[] = { "circle","hexagon","oval","round","soft","capsule"};
	//for (int i = 0; i < 4; i++)
	//{
	//	resize(shapes[i], shapes[i], Size(300, 300));
	//}

	// Create a mask for each single blob
	Mat maskSingleContour(img.rows, img.cols, CV_8U);
	maskSingleContour = Scalar(0, 0, 0);
	vector< vector<Point> > cont2;
	cont2.push_back(cont);
	drawContours(maskSingleContour, cont2, 0, Scalar(255), FILLED);

	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));
	Mat oc;
	imshow("maskSingleContour", maskSingleContour);
	morphologyEx(maskSingleContour, oc, MORPH_OPEN, kernel);
	morphologyEx(oc, oc, MORPH_CLOSE, kernel);
	imshow("oc", oc);

	vector<vector<Point> > contours1;
	findContours(oc, contours1, RETR_TREE, CHAIN_APPROX_NONE);
	Rect bound = boundingRect(contours1[0]);
	Mat crop = oc(bound);
	//imshow("crop", crop);
	Mat resized;
	//resize(crop, resized, Size(300, 300));
	//imshow("resized", resized);
	double result[6];
	for (int i = 0; i < 6; i++)
	{
		vector<vector<Point> > contours2;
		findContours(shapes[i], contours2, RETR_TREE, CHAIN_APPROX_NONE);
		result[i]= matchShapes(contours1[0], contours2[0],1,0.0);
		cout << result[i]<<endl;
	}

	int matched = distance(result, min_element(result, result + (sizeof(result) / sizeof(*result))));
	cout << shapeName[matched] <<endl;
	return matched;

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
	//./partingLinePillData/198900519.jpg
	//200200234
	//200201201 안됨
	//200600830
	//./testpill/199901998.jpg
	//200101361
	//200301828

	Mat input =imread("./testpill/200101361.jpg");

	//resize(input, input, cv::Size(500, 300), 0, 0, CV_INTER_NN);
	Mat mask = getMask(input);
	//imshow("mask", mask);
	vector<Point> cont = getContours(mask,1)[1];
	int shape =matchShape(input, cont);
	Mat crop = croptext(input, cont,shape);
	imshow("crop", crop);

	//imshow("a", a);
	//imshow("asdf", mask);
	// 
	// 
	////원본이미지에서 알약 자르기
	//Mat crop = cropimage(input, mask);
	//imshow("asdf2", crop);
	////불안정해서 임시 제거
	////Mat ssmasked2_bgr;
	////cvtColor(ssmasked2, ssmasked2_bgr, COLOR_GRAY2BGR);
	////Mat maskedImage;
	////maskedImage = input + ~ssmasked2_bgr;
	////imshow("maskedImage", maskedImage);

	//// 알약 영역에서 음각 추출
	//// Sobel Edge
	//Mat p1_sobel_x;
	//Sobel(input, p1_sobel_x, -1, 1, 0);
	//Mat p1_sobel_y;
	//Sobel(input, p1_sobel_y, -1, 0, 1);

	//imshow("sobel_x", p1_sobel_x);
	//imshow("sobel_y", p1_sobel_y);
	//imshow("sobel_p1", p1_sobel_x + p1_sobel_y);



	//// Laplacian Edge
	//Mat p1_laplacian;
	//Laplacian(input, p1_laplacian, CV_8U);
	//imshow("Laplacian_p1", p1_laplacian);

	//// Canny Edge detection
	//Mat p1_canny;
	//Canny(input, p1_canny, 15, 60);
	//imshow("p1_canny", p1_canny);

	//// Hough Transform
	//// 4번쨰 파라미터를 조정하여 선 검출 정도를 조절
	//vector<Vec2f> lines;
	//HoughLines(p1_canny, lines, 1, CV_PI / 180, 100);

	//Mat img_hough;
	//input.copyTo(img_hough);

	//Mat img_lane;
	//threshold(p1_canny, img_lane, 150, 255, THRESH_MASK);

	//

	//for (size_t i = 0; i < lines.size(); i++)
	//{
	//	float rho = lines[i][0], theta = lines[i][1];
	//	

	//	std::cout <<"rho:"<< rho << " theta:" << theta << endl;
	//	if (theta==0 && rho<250 && rho>200)
	//	{
	//		std::cout << "평범 분할선이 있습니다." << endl;
	//	}


	//	Point pt1, pt2;
	//	double a = cos(theta), b = sin(theta);
	//	double x0 = a * rho, y0 = b * rho;
	//	pt1.x = cvRound(x0 + 1000 * (-b));
	//	pt1.y = cvRound(y0 + 1000 * (a));
	//	pt2.x = cvRound(x0 - 1000 * (-b));
	//	pt2.y = cvRound(y0 - 1000 * (a));
	//	line(img_hough, pt1, pt2, Scalar(0, 0, 255), 2, 8);
	//	line(img_lane, pt1, pt2, Scalar::all(255), 1, 8);
	//}

	//std::cout <<"검출한 직선 개수:"<< lines.size();


	//imshow("img_hough", img_hough);
	//imshow("img_lane", img_lane);


	waitKey(0);

	return 0;
}
