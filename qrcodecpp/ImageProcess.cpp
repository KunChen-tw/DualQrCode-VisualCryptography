#include "ImageProcess.hpp"
#include <opencv2\opencv.hpp>

using namespace cv;

namespace ImageProcess {
	/**
	 * 將兩張投影片疊合在一起，像素顏色為白色時當做透明色
	 * @param imgA 第一張要疊合的影像
	 * @param imgB 第二張要疊合的影像
	 * @return imgA 和 imgB 兩張疊合後的影像
	 */
	Mat StackImage(Mat& imgA, Mat& imgB) {
		bool isEqualRow = imgA.rows == imgB.rows;
		bool isEqualCol = imgA.cols == imgB.cols;
		int channelA = imgA.channels();
		int channelB = imgB.channels();

		if (!(isEqualRow && isEqualCol)) {
			throw "the sizes of imgA and imgB are different";
		}
		if (channelA != 3) {
			throw "imgA is not color image";
		}
		if (channelB != 3) {
			throw "imgB is not color image";
		}

		int rows = imgA.rows;
		int cols = imgA.cols;

		// 創建一個 640x480 的彩色圖像，CV_8UC3 表示 8 位元無符號，3 通道（BGR）
		Mat imgAB(cols, rows, CV_8UC3, Scalar(0xFF, 0xFF, 0xFF)); // BGR 格式，預設填滿白色


		int strideA = imgA.step[0];
		int strideB = imgB.step[0];
		for (int y = 0; y < cols; y++) {
			for (int x = 0; x < rows; x++)
			{
				Vec3b pixelA = imgA.at<Vec3b>(y, x);
				Vec3b pixelB = imgB.at<Vec3b>(y, x);

				for(int c =0; c<3; c++)
					imgAB.at<Vec3b>(y, x)[c] = pixelA[c] & pixelB[c];
			}
		}
		return imgAB;

	}
}