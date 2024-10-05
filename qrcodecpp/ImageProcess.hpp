#pragma once

#include <opencv2\opencv.hpp>
using namespace cv;

namespace ImageProcess {
	/**
	* 將兩張投影片疊合在一起，像素顏色為白色時當做透明色
	* @param imgA 第一張要疊合的影像
	* @param imgB 第二張要疊合的影像
	* @return imgA 和 imgB 兩張疊合後的影像
	*/
	Mat StackImage(Mat& imgA, Mat& imgB);

	Mat addBorder(Mat& img, int borderSize, const Scalar& borderColor);
}