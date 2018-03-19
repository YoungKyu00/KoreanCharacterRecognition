#include <opencv2/opencv.hpp>  //  Include 'OpenCV' library.

#include <iostream>
#include <cstdlib>
#include<math.h> //qfqfqfhihu
#include<stdio.h>
using namespace cv;
using namespace std;
int point_num = 0;
int point_x1, point_y1, point_x2, point_y2, point_x3, point_y3, point_x4, point_y4;
Point2f centerpoint(0, 0);

Mat warping(Mat img);
void sortCorners(vector<Point2f> & corner_points, Point2f centerpoint);
void mouseHandler(int event, int x, int y, int flags, void *param);
void testMouseEvent(Mat img);
void matching(Mat srcImage, int width, int height);
void DrawLabelingImage(Mat img);

int main() {

	Mat img, convertedImage;

	char* imgfile = "..\\image\\original.jpg";
	img = imread(imgfile);

	imshow("image", img);
	waitKey(0);

	convertedImage = warping(img);

	DrawLabelingImage(convertedImage);
	exit(0);
}


void DrawLabelingImage(Mat img) {

	Mat img_gray, img_color, img_binary, blur;
	int i, j, x;
	int resize = 0;
	int bottomX, bottomY, topX, topY;


	cvtColor(img, img_gray, CV_BGR2GRAY);
	imshow("gray", img_gray);
	waitKey(0);

	cvtColor(img_gray, img_color, COLOR_GRAY2BGR);


	GaussianBlur(img_gray, blur, Size(3, 3), 0);
	imshow("blur", blur);
	waitKey(0);

	adaptiveThreshold(blur, img_binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, 2);
	imshow("threshold", img_binary);
	waitKey(0);



	Mat img_labels, stats, centroids;
	int numOfLables = connectedComponentsWithStats(img_binary, img_labels,
		stats, centroids, 8, CV_32S);

	int count = 0;
	int distance;
	vector<Rect> boundRect(numOfLables);
	vector<Rect> boundRect2(numOfLables);
	vector<Rect> boundRect3(numOfLables);

	//라벨링 된 이미지를 배열에 넣기
	for (j = 1; j < numOfLables; j++) {
		int area = stats.at<int>(j, CC_STAT_AREA);
		int left = stats.at<int>(j, CC_STAT_LEFT);
		int top = stats.at<int>(j, CC_STAT_TOP);
		int width = stats.at<int>(j, CC_STAT_WIDTH);
		int height = stats.at<int>(j, CC_STAT_HEIGHT);

		boundRect[j] = Rect(left, top, width, height);
	}

	//sort
	for (int i = 1; i < numOfLables; i++) {

		for (int j = 2; j < numOfLables; j++) {
			distance = boundRect[j].y - boundRect[j - 1].y;
			if (abs(distance) > boundRect[j - 1].height * 2) continue;

			if (boundRect[j].x < boundRect[j - 1].x) {
				swap(boundRect[j - 1], boundRect[j]);
			}
		}
	}

	for (i = 1; i < numOfLables;)
	{
		x = boundRect[i].x + abs(boundRect[i].width * 0.6);

		topX = boundRect[i].tl().x;
		topY = boundRect[i].tl().y;
		bottomX = boundRect[i].br().x;
		bottomY = boundRect[i].br().y;


		for (j = i + 1; j < numOfLables; j++) {
			distance = boundRect[j].y - boundRect[i].y; //abs(distance) > boundRect[j - 1].height * 2
			if (boundRect[i].y + boundRect[i].height * 2 < boundRect[j].y) break;
			//if (boundRect[j].x > x || x > boundRect[j].x + boundRect[j].width) continue;

			if (topX > boundRect[j].tl().x)
				topX = boundRect[j].tl().x;
			if (topY > boundRect[j].tl().y)
				topY = boundRect[j].tl().y;
			if (bottomX < boundRect[j].br().x)
				bottomX = boundRect[j].br().x;
			if (bottomY < boundRect[j].br().y)
				bottomY = boundRect[j].br().y;

		}

		boundRect2[resize] = Rect(topX, topY, bottomX - topX, bottomY - topY);
		resize++;
		i = j;
	}

	for (i = 0; i <numOfLables; i++) {
		rectangle(img_color, boundRect2[i].tl(), boundRect2[i].br(), Scalar(255, 0, 0), 1, 8, 0);
		putText(img_color, to_string(i), Point(boundRect2[i].x, boundRect2[i].y), FONT_HERSHEY_SIMPLEX,
			0.8, Scalar(0, 0, 255), 2);
	}

	imshow("result", img_color);
	waitKey(0);
}

void sortCorners(vector<Point2f> & corner_points, Point2f centerpoint)
{
	vector<Point2f> top, bot;

	for (int i = 0; i < corner_points.size(); i++) {
		if (corner_points[i].y < centerpoint.y) {
			top.push_back(corner_points[i]);
		}
		else {
			bot.push_back(corner_points[i]);
		}
	}
	Point2f topLeft = top[0].x > top[1].x ? top[1] : top[0];
	Point2f topRight = top[0].x > top[1].x ? top[0] : top[1];
	Point2f bottomLeft = bot[0].x > bot[1].x ? bot[1] : bot[0];
	Point2f bottomRight = bot[0].x > bot[1].x ? bot[0] : bot[1];

	corner_points.clear();
	corner_points.push_back(topLeft);
	corner_points.push_back(topRight);
	corner_points.push_back(bottomRight);
	corner_points.push_back(bottomLeft);
}

void mouseHandler(int event, int x, int y, int flags, void *param)
{
	if (event == CV_EVENT_LBUTTONDOWN) {
		if (point_num == 0) {
			point_x1 = x;
			point_y1 = y;
		}
		else if (point_num == 1) {
			point_x2 = x;
			point_y2 = y;
		}
		else if (point_num == 2) {
			point_x3 = x;
			point_y3 = y;
		}
		else if (point_num == 3) {
			point_x4 = x;
			point_y4 = y;
		}
		/*
		point_x[point_num] = x;
		point_y[point_num] = y;
		*/
		point_num++;
	}
}

void testMouseEvent(Mat img)
{
	char fname[] = "Click Mouse";
	// read image
	Mat src_image = img;

	char *src_title = fname;

	cvNamedWindow(src_title, CV_WINDOW_AUTOSIZE);
	imshow(src_title, src_image);

	// mouse handler
	int mouseParam = 5;
	cvSetMouseCallback(src_title, mouseHandler, &mouseParam);

	// wait a keyboard input
	cvWaitKey(0);

	// release
	imshow("convertImage", src_image);
	cvDestroyAllWindows();
}

//matching TEST용. 쓸모 없음
void matching(Mat srcImage, int width, int height) {
	Mat dst;
	Mat tImage4 = imread("C:\\Users\\Young\\Pictures\\opencv\\1.png", 0);
	resize(tImage4, dst, Size(width, height), 0, 0, INTER_LINEAR);
	Mat dstImage;


	cvtColor(srcImage, dstImage, COLOR_GRAY2BGR);
	dstImage = srcImage;
	double minVal, maxVal;
	Point minLoc, maxLoc;
	Mat result;

	// 정규화된 상관계수 방법(TM_CCOEFF_NORMED)
	matchTemplate(srcImage, dst, result, TM_CCOEFF_NORMED);
	minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc);
	rectangle(dstImage, maxLoc, Point(maxLoc.x + dst.cols, maxLoc.y + dst.rows), Scalar(255, 0, 255), 2);
	if (maxLoc.x != 0) {
		printf("1");
	}
	imshow("dstImage", dstImage);
	waitKey();

}

Mat warping(Mat img) {

	Mat srcImg = img;

	// mouse event
	testMouseEvent(img);

	// print user click points
	printf("p1 = (%d %d)\n", point_x1, point_y1);
	printf("p2 = (%d %d)\n", point_x2, point_y2);
	printf("p3 = (%d %d)\n", point_x3, point_y3);
	printf("p4 = (%d %d)\n", point_x4, point_y4);

	// copy src(origianl image) to dst_img
	Mat dstImg = srcImg.clone();

	// corner point -> Point2f -> array
	Point2f pt1 = Point2f(point_x1, point_y1);
	Point2f pt2 = Point2f(point_x2, point_y2);
	Point2f pt3 = Point2f(point_x3, point_y3);
	Point2f pt4 = Point2f(point_x4, point_y4);
	Point2f pts[] = { pt1, pt2, pt3, pt4 };


	// array -> vector<Point2f>
	vector<Point2f> corner_points(pts, pts + sizeof(pts) / sizeof(pts[0]));

	// center point
	for (int i = 0; i < corner_points.size(); i++) {
		centerpoint += corner_points[i];
	}
	centerpoint *= (1. / corner_points.size());

	// where is top left, top right, bottom left, bottom right
	sortCorners(corner_points, centerpoint);


	// drawing circles (corner point)
	int radius = 3;
	circle(dstImg, corner_points[0], radius, CV_RGB(0, 255, 0), 2);
	circle(dstImg, corner_points[1], radius, CV_RGB(0, 255, 0), 2);
	circle(dstImg, corner_points[2], radius, CV_RGB(0, 255, 0), 2);
	circle(dstImg, corner_points[3], radius, CV_RGB(0, 255, 0), 2);

	// draing weighted center
	circle(dstImg, centerpoint, 3, CV_RGB(0, 0, 255), 2);

	printf("corner_points[0] : (%lf, %lf)\n", corner_points[0].x, corner_points[0].y);
	printf("corner_points[1] : (%lf, %lf)\n", corner_points[1].x, corner_points[1].y);
	printf("corner_points[2] : (%lf, %lf)\n", corner_points[2].x, corner_points[2].y);
	printf("corner_points[3] : (%lf, %lf)\n", corner_points[3].x, corner_points[3].y);

	// calculate result image's corner points
	double width_x = pow(abs(corner_points[1].x - corner_points[0].x), 2);
	double width_y = pow(abs(corner_points[1].y - corner_points[0].y), 2);
	double quad_width = sqrt(abs(width_x + width_y)) + 0.5;
	double height_x = pow(abs(corner_points[3].x - corner_points[0].x), 2);
	double height_y = pow(abs(corner_points[3].y - corner_points[0].y), 2);
	double quad_height = sqrt(abs(height_x + height_y)) + 0.5;

	Mat quad = Mat::zeros((int)quad_height, (int)quad_width, CV_8UC3);

	vector<Point2f> quad_pnts;
	quad_pnts.push_back(Point2f(0, 0));
	quad_pnts.push_back(Point2f(quad.cols, 0));
	quad_pnts.push_back(Point2f(quad.cols, quad.rows));
	quad_pnts.push_back(Point2f(0, quad.rows));

	// Perspecitive Transformaion (tooshi transform)
	Mat transmtx = getPerspectiveTransform(corner_points, quad_pnts);

	warpPerspective(srcImg, quad, transmtx, quad.size());

	// show result
	imshow("original image", dstImg);
	imshow("corrected perspective", quad);

	// wait any key
	waitKey();


	return quad;
}
