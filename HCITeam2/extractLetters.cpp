#include "extractLetters.h"

// 히스토그램 평활화
cv::Mat GS_histeq(cv::Mat src_image)
{
	cv::Mat dst_image = cv::Mat(src_image.size(), src_image.type());

	cv::equalizeHist(src_image, dst_image);

	return dst_image;
}
// Calculate Histogram and create Histogram Image
// Input : 8-bit Grayscale Image
// 히스토그램 사진 출력
cv::Mat GS_imhist(int& bgcolor, cv::Mat img, int max_length)
{
	int hist_height = 128; int hist_width = 256;


	int hist_bar_height = 15;
	int tmp = 0;

	int bin_size = 256;
	int histSize[] = { bin_size };
	float range[] = { 0, 255 };
	int channels[] = { 0 };
	const float* ranges[] = { range };

	// Calculate Histogram
	cv::Mat hist;
	cv::calcHist(&img, 1, channels, cv::Mat(), hist, 1, histSize, ranges);


	cv::Mat hist_img = cv::Mat(hist_height, hist_width, CV_8UC1, cv::Scalar(0));
	cv::Mat hist_img_bar = cv::Mat(hist_bar_height, hist_width, CV_8UC1, cv::Scalar(0));

	// get min and max value from histogram
	double maxVal, minVal;
	cv::minMaxIdx(hist, &minVal, &maxVal);

	// set Histogram Height
	int tmpHistHeight = round(maxVal);
	// 알약 배경 색 
	bgcolor = tmpHistHeight;
	if (max_length > 0) tmpHistHeight = max_length;


	cv::Mat tmp_hist_imgA = cv::Mat(cv::Size(hist_width, tmpHistHeight), CV_8UC1, cv::Scalar(0));
	cv::Mat tmp_hist_imgB = cv::Mat(cv::Size(hist_width, hist_height), CV_8UC1);

	// normalize histogram
	cv::normalize(hist, hist, 0, tmp_hist_imgA.rows, cv::NORM_MINMAX, -1, cv::Mat());

	// draw histogram image
	for (int i = 0; i < tmp_hist_imgA.cols; i++)
	{
		tmp = tmpHistHeight - round(hist.at<float>(i));

		if (tmp == 0 || tmp > tmpHistHeight) continue;

		for (int j = tmp - 1; j >= 0; j--) {
			tmp_hist_imgA.at<uchar>(j, i) = 192;
		}
	}

	// resize temp histogram image to [hist_height * hist_width]
	cv::resize(tmp_hist_imgA, tmp_hist_imgB, tmp_hist_imgB.size(), 0.0, 0.0, cv::INTER_CUBIC);
	tmp_hist_imgB.copyTo(hist_img);

	// draw color bar
	float hist_bar_width = (float)hist_img.cols / hist.rows;
	for (int i = 0; i < hist_img_bar.cols; i++)
	{
		for (int j = 0; j < hist_img_bar.rows; j++)
		{
			hist_img_bar.at<uchar>(j, i) = i;
		}
	}

	// attach color bar to histogram image
	hist_img.push_back(hist_img_bar);

	hist.release();
	hist_img_bar.release();

	return hist_img;
}
cv::Mat GS_threshold(cv::Mat src_image, double thresh, int threshold_type)
{
	double max_value = 255.0;

	if (threshold_type == cv::THRESH_BINARY || threshold_type == cv::THRESH_BINARY_INV)
		max_value = thresh;

	return GS_threshold(src_image, thresh, max_value, threshold_type);
}
cv::Mat GS_threshold(cv::Mat src_image, double thresh, double max_value, int threshold_type)
{
	cv::Mat dst_image = cv::Mat(src_image.size(), src_image.type());

	cv::threshold(src_image, dst_image, thresh, max_value, threshold_type);

	return dst_image;
}
cv::Mat GS_adaptive_threshold(cv::Mat src_image, double max_value, int adaptive_method, int threshold_type, int block_size, double C)
{
	cv::Mat dst_image = cv::Mat(src_image.size(), src_image.type());

	cv::adaptiveThreshold(src_image, dst_image, max_value,
		adaptive_method, threshold_type,
		block_size, C);

	return dst_image;
}
cv::Mat GS_createImage(cv::Size size, int nChannels)
{
	uchar type = ((nChannels - 1) << 3) + 0;
	cv::Mat dst_image = cv::Mat(size, type);
	return dst_image;
}
cv::Mat GS_createImage(cv::Size size, int depth, int nChannels)
{
	uchar type = ((nChannels - 1) << 3) + depth;
	cv::Mat dst_image = cv::Mat(size, type, cv::Scalar(0));
	return dst_image;
}
int GS_clamping(double var)
{
	return GS_clamping((int)var);
}
// 라플라시안 에지 추출 : Laplacian() (Extraction Laplacian edge: Laplacian())
cv::Mat GS_laplacian_edge_Laplacian(cv::Mat src_image)
{
	//CUtil cUtil;

	// 초기화 
	cv::Mat dst_image = GS_createImage(src_image.size(), src_image.channels());
	if (dst_image.channels() != 1) return dst_image;

	cv::Mat laplacian_edge = GS_createImage(src_image.size(), CV_32F, src_image.channels());

	// 4번째 인자가 1이면 아래의 라플라시안 마스크를 갖고 처리한다. (if the third argument is 1, use Laplacian mask below)
	// |0  1  0|
	// |1 -4  1|
	// |0  1  0|
	// https://docs.opencv.org/3.4.1/d4/d86/group__imgproc__filter.html#gad78703e4c8fe703d479c1860d76429e6
	// 4 번째 인자가 1이 default로 선언되어있어 밑에서는 생략됨.
	cv::Laplacian(src_image, laplacian_edge, laplacian_edge.depth());

	laplacian_edge.convertTo(dst_image, CV_8UC1, 1, 0);

	return dst_image;
}
// 라플라시안 에지 추출 : 마스크 (Extraction Laplacian edge: mask)
// method - 0 : 4 방향 마스크 1 (mask 1 for 4 directions)
//        - 1 : 4 방향 마스크 2 (mask 2 for 4 directions)
//        - 2 : 8 방향 마스크 1 (mask 1 for 8 directions)
//        - 3 : 8 방향 마스크 2 (mask 2 for 8 directions)
cv::Mat GS_laplacian_edge(cv::Mat src_image, int method)
{
	int i, j, m, n;
	int mask_height, mask_width;
	double var, ret_var;
	//CUtil cUtil;

	// 초기화  (Initialization)
	int height = src_image.rows;
	int width = src_image.cols;
	cv::Mat dst_image = GS_createImage(src_image.size(), src_image.channels());
	if (dst_image.channels() != 1) return dst_image;

	// 라플라시안 마스크 정의  (define Laplacian mask)
	mask_height = mask_width = 3;
	int mask_four1[3][3] = { { 0, -1, 0 },{ -1, 4, -1 },{ 0, -1, 0 } };
	int mask_four2[3][3] = { { 0, 1, 0 },{ 1, -4, 1 },{ 0, 1, 0 } };
	int mask_eight1[3][3] = { { -1, -1, -1 },{ -1, 8, -1 },{ -1, -1, -1 } };
	int mask_eight2[3][3] = { { 1, -2, 1 },{ -2, 4, -2 },{ 1, -2, 1 } };

	for (i = 0; i < height - mask_height + ((mask_height - 1) / 2); i++)
	{
		for (j = 0; j < width - mask_width + ((mask_width - 1) / 2); j++)
		{
			// 초기화 (Initialization)
			ret_var = 0.0;

			if ((i + mask_height > height) || (j + mask_width > width))
				continue;

			for (m = 0; m < mask_height; m++)
			{
				for (n = 0; n < mask_width; n++)
				{
					var = src_image.at<uchar>(i + m, j + n);

					if (method == 0) ret_var += var * mask_four1[m][n];
					else if (method == 1) ret_var += var * mask_four2[m][n];
					else if (method == 2) ret_var += var * mask_eight1[m][n];
					else if (method == 3) ret_var += var * mask_eight2[m][n];
				}
			}

			ret_var = GS_clamping(ret_var);
			dst_image.at<uchar>(i + (mask_height - 1) / 2, j + (mask_width - 1) / 2) = ret_var;
		}
	}

	return dst_image;
}
// BGR(input) -> gray
cv::Mat toGrayScale(Mat input) {
	Mat input_gray;
	cvtColor(input, input_gray, CV_BGR2GRAY);
	return input_gray;
}
// input -> gray -> 히스토그램 평활화
cv::Mat histEq(Mat input) {
	Mat input_gray;
	cvtColor(input, input_gray, CV_BGR2GRAY);
	return GS_histeq(input_gray);
}
// input -> gray -> 이진화 
cv::Mat getBinary(Mat input, int threshold) {
	Mat input_gray;
	cvtColor(input, input_gray, CV_BGR2GRAY);
	return GS_threshold(input_gray, threshold, THRESH_BINARY);
}
// input -> gray-> 오츠 방법
cv::Mat getOTSU(Mat input, int threshold) {
	Mat input_gray;
	cvtColor(input, input_gray, CV_BGR2GRAY);
	return GS_threshold(input_gray, threshold, THRESH_BINARY | THRESH_OTSU);
}
// input -> gray -> 이진화 -> canny 
cv::Mat binaryToCanny(Mat input, int threshold, int downThresh, int upThresh) {
	Mat binary= getBinary(input, threshold);
	Mat canny;
	Canny(binary, canny, downThresh, upThresh);
	return canny;
}
// input -> gray -> canny 
cv::Mat grayToCanny(Mat input, int threshold, int downThresh, int upThresh) {
	Mat input_gray;
	cvtColor(input, input_gray, CV_BGR2GRAY);
	Mat canny;
	Canny(input_gray, canny, downThresh, upThresh);
	return canny;
}