//  dividing.cpp
//
//  Created by 박범수 on 2021/06/05.
//
#include "dividing.h"

Mat* split_line(Mat src, int *split_line_type)
{
    Mat* output;
    Mat input;
    src.copyTo(input);
    
    
    //사진을 구분하기 위해 포인터를 string으로 바꿈
    std::stringstream img_name;
    img_name << split_line_type;
    std::string name = img_name.str();
    
    //imshow("img",input);
    resize(input, input, cv::Size(300, 300), 0, 0, CV_INTER_NN);
    cvtColor(input, input,CV_BGR2GRAY);
    
    Mat element5(5, 5, CV_8U, cv::Scalar(1));
    Mat element3(3, 3, CV_8U, cv::Scalar(1));
    
    erode(input, input, element3);
    imshow("erode",input);
    
    equalizeHist(input, input);
    imshow("eq",input);
    
    // Canny Edge detection
    Mat p1_canny;
    Canny(input, p1_canny, 15, 25);
    imshow("p1_canny " + name, p1_canny);

    // Hough Transform
    vector<Vec2f> lines;
    HoughLines(p1_canny, lines, 1, CV_PI / 180, 150); // 4번째 파라미터가 높을수록 "분할선"의 기준이 높아짐

    Mat img_hough; // hough 윈도우
    input.copyTo(img_hough);

    Mat img_lane; // 선을 그리는 윈도우
    threshold(p1_canny, img_lane, 150, 255, THRESH_MASK);

    
     // 분할선으로 알약을 나누기 위한 기준 변수들
     int min_divx =1000, max_divx = -1000;
     int min_divy =1000, max_divy = -1000;
     
     // 검출한 선들에 대해 for문 실행
    for (size_t i = 0; i < lines.size(); i++)
    {
       float rho = lines[i][0], theta = lines[i][1];
       
       std::cout <<"rho:"<< rho << " theta:" << theta << endl;

    
        // 분할선 후보가 아니면 continue
      if(!(theta<0.1&&rho<=185&&rho>=115) && !(theta>1.55&&rho<=185&&rho>=115))
         continue;
        
        // 선을 그리는 포인트 2개
       Point pt1, pt2;
       
       double a = cos(theta), b = sin(theta);
       double x0 = a * rho, y0 = b * rho;
       pt1.x = cvRound(x0 + 1000 * (-b));
       pt1.y = cvRound(y0 + 1000 * (a));
       pt2.x = cvRound(x0 - 1000 * (-b));
       pt2.y = cvRound(y0 - 1000 * (a));
        
        
        // 세로 분할선에 대해 추출
        if(pt1.x>max_divx && pt1.x!=1000 && pt1.x!=-1000){
            max_divx=pt1.x;
        }
        if(pt2.x>max_divx && pt2.x!=1000 && pt2.x!=-1000){
            max_divx=pt2.x;
        }
        if(pt1.x<min_divx && pt1.x!=1000 && pt1.x!=-1000){
            min_divx=pt1.x;
        }
        if(pt2.x<min_divx && pt2.x!=1000 && pt2.x!=-1000){
            min_divx=pt2.x;
        }
        
        // 가로 분할선에 대해 추출
        if(pt1.y>max_divy && pt1.y!=1000 && pt1.y!=-1000){
            max_divy=pt1.y;
        }
        if(pt2.y>max_divy && pt2.y!=1000 && pt2.y!=-1000){
            max_divy=pt2.y;
        }
        if(pt1.y<min_divy && pt1.y!=1000 && pt1.y!=-1000){
            min_divy=pt1.y;
        }
        if(pt2.y<min_divy && pt2.y!=1000 && pt2.y!=-1000){
            min_divy=pt2.y;
        }

       line(img_hough, pt1, pt2, Scalar(0, 0, 255), 2, 8);
       line(img_lane, pt1, pt2, Scalar::all(255), 1, 8);
    }
     
     cout<<"min, max div x: "<<min_divx<<", "<<max_divx<<endl;
     cout<<"min, max div x: "<<min_divy<<", "<<max_divy<<endl;

     Mat pill_left, pill_right, pill_up, pill_down;
     Mat pill_0, pill_1, pill_2, pill_3;
     
     // 가로분할선과 세로분할선이 둘다 있으면 십자 분할선
     if((min_divx<=max_divx && min_divx!= -1000 && max_divx!=1000) && (min_divy<=max_divy && min_divy!= -1000 && max_divy!=1000))
     {
         int ImageHeight, ImageWidth;
         Mat matRotation;
         
         pill_0 = src(Rect( Point( 0, 0 ), Point( min_divx, min_divy) ));
         pill_1 = src(Rect( Point( max_divx, 0), Point( 300, min_divy) ));
         pill_2 = src(Rect( Point( max_divx, max_divy ), Point( 300, 300) ));
         pill_3 = src(Rect( Point( 0, max_divy ), Point( min_divx, 300) ));
         
         resize(pill_0, pill_0, cv::Size(0, 0), 0.6, 1.0, CV_INTER_NN);
         ImageHeight = pill_0.rows / 2;
         ImageWidth  = pill_0.cols / 2;
         matRotation = getRotationMatrix2D( Point(ImageHeight, ImageWidth), (315), 1 );
         warpAffine( pill_0, pill_0, matRotation, pill_0.size());
         
         resize(pill_1, pill_1, cv::Size(0, 0), 0.6, 1.0, CV_INTER_NN);
         ImageHeight = pill_1.rows / 2;
         ImageWidth  = pill_1.cols / 2;
         matRotation = getRotationMatrix2D( Point(ImageHeight, ImageWidth), (45), 1 );
         warpAffine( pill_1, pill_1, matRotation, pill_0.size());
         
         resize(pill_2, pill_2, cv::Size(0, 0), 0.6, 1.0, CV_INTER_NN);
         ImageHeight = pill_2.rows / 2;
         ImageWidth  = pill_2.cols / 2;
         matRotation = getRotationMatrix2D( Point(ImageHeight, ImageWidth), (135), 1 );
         warpAffine( pill_2, pill_2, matRotation, pill_0.size());
         
         resize(pill_3, pill_3, cv::Size(0, 0), 0.6, 1.0, CV_INTER_NN);
         ImageHeight = pill_3.rows / 2;
         ImageWidth  = pill_3.cols / 2;
         matRotation = getRotationMatrix2D( Point(ImageHeight, ImageWidth), (225), 1 );
         warpAffine( pill_3, pill_3, matRotation, pill_0.size());
         
         imshow("pill_0",pill_0);
         imshow("pill_1",pill_1);
         imshow("pill_2",pill_2);
         imshow("pill_3",pill_3);
         *split_line_type = 3;
         
         output = new Mat[4] {pill_0, pill_1, pill_2, pill_3};
         
         cout<<"약사진을 4등분으로 갈랐습니다."<<endl;
         cout<<"십자 분할선이 있습니다."<<endl;
     }
     // 세로 분할선만 있을 때
     else if (min_divx<=max_divx && min_divx!= -1000 && max_divx!=1000){
         pill_left = src(Rect( Point( 0, 0 ), Point( min_divx, 300) ));
         pill_right = src(Rect( Point( max_divx, 0), Point( 300, 300) ));
         
         imshow("pill_left " + name,pill_left);
         imshow("pill_right " + name,pill_right);
         *split_line_type=1;
         
         output = new Mat[2] {pill_left, pill_right};
         
         cout<<"약사진을 좌우로 갈랐습니다."<<endl;
         cout<<"세로 분할선이 있습니다."<<endl;
         
     }
     // 가로 분할선만 있을 때
     else if(min_divy<=max_divy && min_divy!= -1000 && max_divy!=1000){
         pill_up = src(Rect( Point( 0, 0 ), Point( 300, min_divy) ));
         pill_down = src(Rect( Point( 0, max_divy), Point( 300, 300) ));
     
         imshow("pill_up " + name,pill_up);
         imshow("pill_down " + name,pill_down);
         *split_line_type=2;
         
         output = new Mat[2] {pill_up, pill_down};
         
         cout<<"약사진을 위아래로 갈랐습니다."<<endl;
         cout<<"가로 분할선이 있습니다."<<endl;
     }
     else
     {
         *split_line_type=0;
         
         output = new Mat {src};
         
         cout<<"약에 분할선이 없습니다."<<endl;
     }
     
    std::cout <<"검출한 직선 개수:"<< lines.size()<<endl;

    imshow("img_hough " + name, img_hough);
    imshow("img_lane " + name, img_lane);
    
    return output;
}
