#include "extractLetters.h"

#define nonMarkFilePath "C:\\Users\\송승민\\source\\repos\\HCI_Project\\hci-git\\HCITeam2\\HCITeam2\\testpill\\" 
//"D:\\4학년 1학기\\인컴\\KoreaPill\\nonMarkPill2\\nonMarkPillData2\\"
#define PillName (nonMarkFilePath"198901291")

int main()
{
	// 이미지 불러오기 
	String imageName = PillName;
	imageName = imageName + "L.PNG";
	Mat input = imread(imageName);
	imshow("input", input);

	// BGR -> gray scale로 변경
	//cvtColor(input, input_gray, CV_BGR2GRAY);
	Mat input_gray = toGrayScale(input);
	imshow("input_gray", input_gray);

	// 이미지 히스토그램 보기
	int bgcolor;
	Mat input_gray_hist = GS_imhist(bgcolor, input_gray, 300); // input_gray.size().height*input_gray.size().width
	cout << input_gray.size().height << endl;
	cout << bgcolor << endl;
	imshow("input_gray_hist", input_gray_hist);

	Mat input_gray_hist_eq = histEq(input); // GS_histeq(input_gray);
	imshow("input_gray_hist_eq", ~input_gray_hist_eq);


	// 이미지 이진화
	// 어두운 알약, 어두운 글씨 70
	// 밝은 알약, 밝은 글씨 180
	// 밝은 알약, 어두운 글씨 180(글자 두꺼움) 170(최적) 160(글자 얇아짐)
	// 어두운 알약, 밝은 글씨 70
	int threshold = 170;
	Mat input_gray_threshold = getBinary(input, threshold); //GS_threshold(input_gray, threshold, THRESH_BINARY);
	imshow("input_gray_threshold", input_gray_threshold);

	Mat input_gray_hist_eq_threshold = getOTSU(input, threshold); // GS_threshold(input_gray_hist_eq, threshold, THRESH_BINARY);
	imshow("input_gray_hist_eq_threshold", input_gray_hist_eq_threshold);


	// 오츠 방법
	Mat input_gray_OTSU = GS_threshold(input_gray, threshold, THRESH_BINARY | THRESH_OTSU);
	imshow("input_gray_OTSU", input_gray_OTSU);

	Mat input_gray_hist_eq_OTSU = GS_threshold(input_gray_hist_eq, threshold, THRESH_BINARY | THRESH_OTSU);
	imshow("input_gray_hist_eq_OTSU", input_gray_hist_eq_OTSU);


	// linear 필터로 blur 이미지 구하고, gray에서 뺀 다음, gray에 더해서 엣지 강조
	// 가우시안 필터로 blur 이미지 구하고, gray에서 뺀 다음, gray에 더해서 엣지 강조
	// linear & 가우시안 필터 이용 시, 가중치 곱해서 엣지 더 뽑기

	// gradient sobel 엣지 추출
	//Mat input_gray_sobel_x;
	//// Sobel(미분 적용할 원본 이미지, 결과 이미지 저장될 곳, 결과 이미지 데이터 타입, x방향 미분 차수, y방향 미분 차수)
	//Sobel(input_gray, input_gray_sobel_x, -1, 1, 0);
	//Mat input_gray_sobel_y;
	//Sobel(input_gray, input_gray_sobel_y, -1, 0, 1);
	//imshow("input_gray_sobel", input_gray_sobel_y + input_gray_sobel_y);

	// 라플라스 필터로 엣지 추출
	//Mat input_gray_Laplacian = GS_laplacian_edge_Laplacian(input_gray); //GS_laplacian_edge_Laplacian
	//imshow("input_gray_Laplacian", input_gray_Laplacian);

	int threshold1 = 150;
	int threshold2 = 190;

	// Canny Edge detection
	Mat input_gray_canny= grayToCanny(input, threshold, threshold1, threshold2);
	// Canny(~input_gray, input_gray_canny, threshold1, threshold2);
	imshow("~input_gray", ~input_gray);
	imshow("input_gray_canny", input_gray_canny);

	Mat input_gray_threshold_canny = grayToCanny(input, threshold, threshold1, threshold2);
	// Canny(~input_gray_threshold, input_gray_threshold_canny, threshold1, threshold2);
	imshow("~input_gray_threshold", ~input_gray_threshold);
	imshow("input_gray_threshold_canny", input_gray_threshold_canny);


	// 노이즈 제거 
	// erode  
	// 이진화 > canny > erode
	Mat input_gray_threshold_canny_erode;
	erode(input_gray_threshold_canny, input_gray_threshold_canny_erode, Mat());
	imshow("input_gray_threshold_canny_erode", input_gray_threshold_canny_erode);

	// 오츠 > erode 
	Mat input_gray_OTSU_erode;
	erode(input_gray_OTSU, input_gray_OTSU_erode, Mat());
	imshow("input_gray_OTSU_erode", input_gray_OTSU_erode);





	waitKey(0);
	return 0;
}