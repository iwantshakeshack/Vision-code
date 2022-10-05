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
	VideoCapture cap(0);	//0�� �⺻ī�޶�(��ķ) 1����������..	1iIlL
//	VideoCapture cap(0);	//��Ʈ�� �����.	//220902
	if (!cap.isOpened()) {	//����ó���ڵ�
		cerr << "Camera open failed!" << endl;
		return;
	}
	

	Mat frame;
	frame = imread("c_test_frame_7.png");
//	frame = imread("c_test07.png");
	frame = frame(Rect(130, 40, 384, 380));	//ROI �����ϴ� �κ�.
	resize(frame, frame, Size(500, 500));		//����������

	/*
	*/
	Point2f c_center;
	Size2f c_size;
	float c_angle;


	while (true) {
		//		cap >> frame;	//@@@@@@@@@@@@@@@@
		if (frame.empty()) {	//����ó��
			cerr << "Image load failed!" << endl;
			break;
		}


		//		frame = imread("c_test_frame.png");	//��Ʈ�� �����. //220902
		//		flip(frame,frame,1);	//�¿����(�ſ���)
		imshow("frame", frame);
		//		imshow("frame_b", b_b(frame, 20, 130, 80, 110, 255, 255));
		int kc = waitKey(1000);	// 1ms refresh-rate
		if (kc == 27) { // ESC key
			cout << "esc ����" << endl;
			break;
		}
		Mat oa = b_b(frame, 20, 130, 80, 110, 255, 255);	//	v2
//		imshow("oa", oa);
		dilate(oa, oa, Mat(), Point(-1, -1), 5);
//		imshow("oa_dilate", oa);
		vector<vector<Point>> contours;	//�ܰ��������� vector<vector<Point>> Ÿ����.
		findContours(oa, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);	//�ܰ��������� contours �� ����

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
			//�ܰ��� 0���϶� ����ó��. ������ �簢�� ���Ƿ� �����. 9999�� �����ϸ� �� ���⼭.
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
				for (int i = 0; i < contours.size(); i++)	// i���ݺ�, i=contours.size �ܰ�������
				{
					cout << contours.size() << endl;	//contours.size = 2
					cout << "contour " << i << " �� ������ : " << contourArea(contours[i]) << endl;
					if (contourArea(contours[i]) > contourArea(contours[c_max])) c_max = i;
				}
				printf("\n���� ū contour�� %d �� �Դϴ�.\n", c_max);
			}
			RotatedRect a = minAreaRect(contours[c_max]);	//280������ �����ؼ� �׸� @�����ذ�

			Point2f points[4];
			a.points(points);
			cvtColor(oa, oa, COLOR_GRAY2BGR, 0);
			for (int i = 0; i < 4; i++)
				line(oa, points[i], points[(i + 1) % 4], Scalar(0, 255, 0), 2);
			drawContours(oa, contours, -1, Scalar(0, 0, 255), 2);	//�ܰ��� �׸���
			circle(oa, a.center, 10, Scalar(255, 0, 0), 2); //�߾ӿ� �� ǥ��$$$$$

//			cout << a.center << endl << a.size << endl << a.angle << endl;
			c_center = a.center;
			c_size = a.size;
			c_angle = a.angle;
		}

		imshow("oa_f", oa);

		//���� ǥ���� �ٲٱ�
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
//	erode(output, output, Mat(), Point(-1, -1), 2);		//2��ħ��
//	dilate(output, output, Mat(), Point(-1, -1), 2);	//2����â
	output = ~output;
	return output;
}