#pragma once

#include <opencv2\opencv.hpp>
using namespace cv;

namespace ImageProcess {
	/**
	* �N��i��v���|�X�b�@�_�A�����C�⬰�զ�ɷ��z����
	* @param imgA �Ĥ@�i�n�|�X���v��
	* @param imgB �ĤG�i�n�|�X���v��
	* @return imgA �M imgB ��i�|�X�᪺�v��
	*/
	Mat StackImage(Mat& imgA, Mat& imgB);

	Mat addBorder(Mat& img, int borderSize, const Scalar& borderColor);
}