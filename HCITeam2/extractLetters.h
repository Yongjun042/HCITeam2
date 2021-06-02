#pragma once
#include<opencv2/opencv.hpp>
#include<vector>
#include<iostream>
#include<string>

using namespace std;
using namespace cv;

cv::Mat GS_histeq(cv::Mat src_image);
cv::Mat GS_imhist(int& bgcolor, cv::Mat img, int max_length);
cv::Mat GS_threshold(cv::Mat src_image, double thresh, int threshold_type);
cv::Mat GS_threshold(cv::Mat src_image, double thresh, double max_value, int threshold_type);
cv::Mat GS_adaptive_threshold(cv::Mat src_image, double max_value, int adaptive_method, int threshold_type, int block_size, double C);
cv::Mat GS_createImage(cv::Size size, int nChannels);
cv::Mat GS_createImage(cv::Size size, int depth, int nChannels);
int GS_clamping(double var);
cv::Mat GS_laplacian_edge_Laplacian(cv::Mat src_image);
cv::Mat GS_laplacian_edge(cv::Mat src_image, int method);

cv::Mat toGrayScale(Mat input); // BGR-> gray
cv::Mat histEq(Mat input); // input -> gray -> 히스토그램 평활화 
cv::Mat getBinary(Mat input, int threshold); // input -> gray -> 이진화 
cv::Mat getOTSU(Mat input, int threshold); // input -> gray-> 오츠 방법
cv::Mat binaryToCanny(Mat input_binary, int threshold, int downThresh, int upThresh); // input -> gray-> 이진화 -> canny 
cv::Mat grayToCanny(Mat input, int threshold, int downThresh, int upThresh); // input -> gray -> canny 