#include "ImageProcess.hpp"
#include <opencv2\opencv.hpp>

using namespace cv;

namespace ImageProcess {
	/**
	 * �N��i��v���|�X�b�@�_�A�����C�⬰�զ�ɷ��z����
	 * @param imgA �Ĥ@�i�n�|�X���v��
	 * @param imgB �ĤG�i�n�|�X���v��
	 * @return imgA �M imgB ��i�|�X�᪺�v��
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

		// �Ыؤ@�� 640x480 ���m��Ϲ��ACV_8UC3 ��� 8 �줸�L�Ÿ��A3 �q�D�]BGR�^
		Mat imgAB(cols, rows, CV_8UC3, Scalar(0xFF, 0xFF, 0xFF)); // BGR �榡�A�w�]�񺡥զ�


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