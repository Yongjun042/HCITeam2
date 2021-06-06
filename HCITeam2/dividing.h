//
//  dividing.h
//  pill
//
//  Created by 박범수 on 2021/06/05.
//
#include <opencv2/opencv.hpp>
#include <vector>
#include<map>
#include<string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

Mat* split_line(Mat src, int *split_line_type);
