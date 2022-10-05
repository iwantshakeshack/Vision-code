//

#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void camera_in();
Mat b_b(Mat input, int hh, int ss, int vv, int hhh, int sss, int vvv);

int main()
{
	cout << "Hello OpenCV " << CV_VERSION << endl;
	
	camera_in();

	destroyAllWindows();
	waitKey();
	return 0;
}

void camera_in()
{
	VideoCapture cap(0);	//0번 기본카메라(웹캠) 1번은다음거..	1iIlL
//	VideoCapture cap(0);	//노트북 시험용.	//220902
	if (!cap.isOpened()) {	//예외처리코드
		cerr << "Camera open failed!" << endl;
		return;
	}
	

	Mat frame;
	frame = imread("c_test_frame_7.png");
//	frame = imread("c_test07.png");
	frame = frame(Rect(130, 40, 384, 380));	//ROI 설정하는 부분.
	resize(frame, frame, Size(500, 500));		//사이즈조절

	/*
	*/
	Point2f c_center;
	Size2f c_size;
	float c_angle;


	while (true) {
		//		cap >> frame;	//@@@@@@@@@@@@@@@@
		if (frame.empty()) {	//예외처리
			cerr << "Image load failed!" << endl;
			break;
		}


		//		frame = imread("c_test_frame.png");	//노트북 시험용. //220902
		//		flip(frame,frame,1);	//좌우반전(거울모드)
		imshow("frame", frame);
		//		imshow("frame_b", b_b(frame, 20, 130, 80, 110, 255, 255));
		int kc = waitKey(1000);	// 1ms refresh-rate
		if (kc == 27) { // ESC key
			cout << "esc 감지" << endl;
			break;
		}
		Mat oa = b_b(frame, 20, 130, 80, 110, 255, 255);	//	v2
//		imshow("oa", oa);
		dilate(oa, oa, Mat(), Point(-1, -1), 5);
//		imshow("oa_dilate", oa);
		vector<vector<Point>> contours;	//외곽선정보는 vector<vector<Point>> 타입임.
		findContours(oa, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);	//외곽선정보를 contours 에 저장

		float c_a_t = 0;
		for (int ii = 0; ii < contours.size(); ii++)
		{
			c_a_t += contourArea(contours[ii]);
		}
		printf("\nc_a_t : %f\n", c_a_t);
		int filter = 1;
		if (c_a_t < 5000)
		{
			printf("\n nothing here\n");
			filter = 0;
		}

		int c_max = 0;	//contours_max
		if (contours.size() == 0 || filter == 0) {
			printf("\ncontours size : zero!!\n");
			//외곽선 0개일때 예외처리. 정해진 사각형 임의로 만들기. 9999로 변경하면 됨 여기서.
			RotatedRect b = RotatedRect(Point2f(999.99, 999.99), Size2f(999.999, 999.999), -99.999);
//			cout << b.center << endl << b.size << endl << b.angle << endl;
			c_center = b.center;
			c_size = b.size;
			c_angle = b.angle;

		}
		else if (contours.size() >= 1)
		{
			if (contours.size() > 1)
			{
				for (int i = 0; i < contours.size(); i++)	// i번반복, i=contours.size 외곽선개수
				{
					cout << contours.size() << endl;	//contours.size = 2
					cout << "contour " << i << " 의 면적은 : " << contourArea(contours[i]) << endl;
					if (contourArea(contours[i]) > contourArea(contours[c_max])) c_max = i;
				}
				printf("\n제일 큰 contour는 %d 번 입니다.\n", c_max);
			}
			RotatedRect a = minAreaRect(contours[c_max]);	//280페이지 참조해서 그림 @문제해결

			Point2f points[4];
			a.points(points);
			cvtColor(oa, oa, COLOR_GRAY2BGR, 0);
			for (int i = 0; i < 4; i++)
				line(oa, points[i], points[(i + 1) % 4], Scalar(0, 255, 0), 2);
			drawContours(oa, contours, -1, Scalar(0, 0, 255), 2);	//외곽선 그리기
			circle(oa, a.center, 10, Scalar(255, 0, 0), 2); //중앙에 원 표시$$$$$

//			cout << a.center << endl << a.size << endl << a.angle << endl;
			c_center = a.center;
			c_size = a.size;
			c_angle = a.angle;
		}

		imshow("oa_f", oa);

		//각도 표현법 바꾸기
		if (c_size.width >= c_size.height)
			c_angle = c_angle*(-1);
		else c_angle = c_angle*(-1) + 90;

		cout << c_center << endl << c_size << endl << c_angle << endl;

	}
	return;
}

Mat b_b(Mat input, int hh, int ss, int vv, int hhh, int sss, int vvv)
{
	Mat output, mid;
	cvtColor(input, mid, COLOR_BGR2HSV);
	inRange(mid, Scalar(hh, ss, vv), Scalar(hhh, sss, vvv), output);
//	erode(output, output, Mat(), Point(-1, -1), 2);		//2번침식
//	dilate(output, output, Mat(), Point(-1, -1), 2);	//2번팽창
	output = ~output;
	return output;
}