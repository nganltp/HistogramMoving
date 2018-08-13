#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;

/*Init histogram*/
int hist_w = 700; int hist_h = 350, histSize = 700;
int bin_w = cvRound((double)hist_w / histSize);
Mat histSrc(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
Mat histDst(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

/*Draw histogram for gray Image*/
void drawHistogram(Mat src, Mat &histImage)
{
	vector<Mat> planes;
	split(src,   planes);
	float range[] = { 0, 256 };
	const float* histRange = { range };
	Mat hist;
	calcHist(&planes[0], 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}

}
bool compute(Mat src, int &a, int &b)
{
	int cols = src.cols;
	int rows = src.rows;
	int hist[256] = { 0 };

	/*tinh hist*/
	for (int x = 0; x < cols; x++)
		for (int y = 0; y < rows; y++)
		{
			hist[(int)(src.at<uchar>(y, x))] ++;
		}
	for (int i = 0; i < 256; i++)
		cout << i << ": " << hist[i] << endl;

	/*Init*/
	int i, j;
	int T = 10; // thresh
	int A = 0, B = 255;
	int sum = 0;

	// detect value for a,b
	for (i = 0; i<256; i++)
		if (hist[i] > T)
		{
			if (a == 0) a = i;
			for (j = 255; j>i; j--)
				if (hist[j]>T)
				{
					b = j;
					j = 0; i = 300; // out
				}
		}
	if (b<B)			
	{
		cout << "a: " << a << endl;
		cout << "b: " << b << endl;
		return true;
	}
	return false;
}
int main()
{
	cout <<endl<< "\t************************************" << endl;
	cout <<endl<< "\t**Program move histogram to right!**" << endl;
	cout <<endl<< "\t************************************" << endl<<endl;

	Mat src;
	int a = 0, b =0;
	int A = 0, B = 255;

	src = imread("lenna1.png");
	cvtColor(src, src, COLOR_BGR2GRAY);
	Mat dst = src.clone();
	drawHistogram(src, histSrc);

	/*MOVING*/
	// compute for thresh a,b
	if (!compute(src, a, b))
		cout << "can't moving histogram to right";
	//moving and update new value
	for (int i = 0; i < src.cols; i++) {
		for (int j = 0; j < src.rows; j++) {
			dst.at<uchar>(j, i)= src.at<uchar>(j, i) + (B - b);
			}
		}
	drawHistogram(dst, histDst);

	/*save Image*/
	imwrite("Output.png", dst);

	namedWindow("src", WINDOW_AUTOSIZE);
	imshow("Src", src);
	imshow("Dst", dst);
	imshow("HistSrc", histSrc);
	imshow("HistDst", histDst);
	waitKey(0);
	return 0;
}