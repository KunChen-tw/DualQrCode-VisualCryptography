// nested QR Code generator
// qrcode: module 1 : in black 0x00
//         module 0 : in white 0xFF
#define DEBUG
#define NOMINMAX
//#define SHOW_IMAGES // show images

#include <opencv2\opencv.hpp>
#include "BitBuffer.hpp"
#include "QrCode.hpp"
#include "ImageProcess.hpp"
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <cmath>
#include <vector>
#include <string>
#include <codecvt>
#include <locale>
#include <cassert>


using namespace cv;
using namespace std;
using namespace ImageProcess;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

namespace fs = std::filesystem;

enum moduleType { MTFinder, MTAlign, MTSeparator, MTTiming, MTFormat, MTVersion, MTDarkM, MTData };



// Function prototypes
Mat drawBinaryQRCode(const QrCode& qr, int mSize, int qzSize);
Mat dmQRCode(const QrCode& qrNear, const QrCode& qrFar, int qZsize, int mSize, int cSize);

/**
* �� Random Grid ���s�X��k�A�ھ� imgSecret �M QrCode �s�X�X���ɼv��
* @param imgSecret ���K�v��
* @param qr        �w�s�X�� QrCode
* @param qzSoze    number of quiet zone modules
* @oaram mSize     number of pixels of a module block 
* @return �s�X�᪺���ɼv��
*/
Mat EncodeRandomGridQrCode(Mat& imgSecret, const QrCode& qr, int qzSize, int mSize);

//************************************************************************************************************
int main()
{
	// QR code parameters
	int qZsize = 4;  // number of quiet zone modules
	int eccL = 1;  // LOW = 0, MEDIUM = 1, QUARTILE = 2, HIGH = 3  error tolerance = 7/15/25/30
	int mSize = 19; // number of pixels of a module block 
	int cSize = 5;  // module block centroid size : for near view module < 1/3 mSize //����Ϊ��ɭԳ]��3
		
	////unicode to utf8  �s�X�����
	//std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;

	//// near view QR code text string
	//wchar_t *sNear = L"A Near View sees Inner module";
	//cout << "Input near view QR code string:";
	//cin >> sNear;

	//vector<uint8_t> textNear;
	//for (int i = 0; i < wcslen(sNear); ++i) {
	//	std::string utf8 = convert.to_bytes(sNear[i]);
	//	for (int j = 0; j < utf8.length(); ++j) {
	//		textNear.push_back(utf8[j]);
	//	}
	//}

	//// far view QR code text string
	//wchar_t *sFar = L"A Far View sees Outer module";
	//vector<uint8_t> textFar;
	//for (int i = 0; i < wcslen(sFar); ++i) {
	//	std::string utf8 = convert.to_bytes(sFar[i]);
	//	for (int j = 0; j < utf8.length(); ++j) {
	//		textFar.push_back(utf8[j]);
	//	}
	//}

	////encoding binary QR Codes LOW = 0, MEDIUM = 1, QUARTILE = 2, HIGH = 3
	//const QrCode qrNear = QrCode::encodeBinary(textNear, (QrCode::Ecc) eccL);
	//const QrCode qrFar = QrCode::encodeBinary(textFar, (QrCode::Ecc) eccL);


	//char textNear[2954] = "https://youtu.be/MUcTeoFJ9qo"; // max Numeric: 7098; Aplphanumeric: 4296; Binary: 2953
	//char textFar[2954]  = "https://youtu.be/zNmhCabrJlg";


#pragma region DualQRCode
	char textNear[2954] = "https://www.yzu.edu.tw                      "; // max Numeric: 7098; Aplphanumeric: 4296; Binary: 2953
	char textFar[2954] = "https://www.youtube.com/watch?v=9i_UQC4znvU";

	const QrCode qrNear = QrCode::encodeText(textNear, (QrCode::Ecc)eccL);
	const QrCode qrFar = QrCode::encodeText(textFar, (QrCode::Ecc)eccL);

	// draw binary QR codes for comparison
	Mat qrNearImg = drawBinaryQRCode(qrNear, mSize, qZsize);//�N QR Code �ƾ��ഫ���G�ȤƹϹ�
	Mat qrFarImg = drawBinaryQRCode(qrFar, mSize, qZsize);

	// show QR code version
	cout << "qrNear Version: " << qrNear.getVersion() << endl;
	cout << "qrFar Version:" << qrFar.getVersion() << endl;

	int nearVer = qrNear.getVersion();
	int farVer = qrFar.getVersion();

	// In our construction the QR codes must have the same version
	if (qrNear.getVersion() != qrFar.getVersion()) {
		cout << "Version not the same" << endl;
		exit(1);
	}

	Mat qrDualImg = dmQRCode(qrNear, qrFar, qZsize, mSize, cSize);

#pragma endregion

#pragma region RandomGrid

	const string  inputPath = "input\\v" + to_string(farVer) + "\\";
	vector<string> secertFilePaths;
	vector<string> secertFileNames;

	try {
		for (const auto& entry : fs::directory_iterator(inputPath)) {
			if (entry.is_regular_file()) {  // �ˬd�O�_���@���ɮ�
				//secertFilenams.push_back(entry.path().filename().string());  // �u�s�ɮצW��
				secertFilePaths.push_back(entry.path().string());  // �O��������|�W��
				secertFileNames.push_back(entry.path().stem().string());  // �u�s���]�t���ɦW���ɮצW��
			}
		}
	}
	catch (const fs::filesystem_error& e) {
		cerr << "File System Error: " << e.what() << endl;
		return 1;
	}

	// �C�X�Ҧ��ɮצW��
	cout << "filenames:" << endl;
	int numberofFiles = secertFilePaths.size();
	for (int i = 0; i < numberofFiles; i++) {
		cout << secertFilePaths[i] << endl;
		cout << secertFileNames[i] << endl;
		string secretFilePath = secertFilePaths[i];
		Mat imgSecret = imread(secretFilePath);
		Mat imgRG = EncodeRandomGridQrCode(imgSecret, qrFar, qZsize, mSize);
		Mat mImgAb = StackImage(qrDualImg, imgRG);

#ifdef SHOW_IMAGES
		// show images
		imshow("Near QR", qrNearImg);
		imshow("Far QR", qrFarImg);
		imshow("DualQRCode", qrDualImg);
		imshow("Secret Image", imgSecret);
		imshow("Share image", imgRG);
		imshow("Stack Image", mImgAb);
#endif // SHOW_IMAGES

		// write images
		imwrite("output\\v" + to_string(farVer) + "\\"+ secertFileNames[i] +"_nQrCode.png", qrNearImg);
		imwrite("output\\v" + to_string(farVer) + "\\" + secertFileNames[i] + "_fQrCode.png", qrFarImg);
		imwrite("output\\v" + to_string(farVer) + "\\" + secertFileNames[i] + "_dQrCode.png", qrDualImg);
		imwrite("output\\v" + to_string(farVer) + "\\" + secertFileNames[i] + "_share.png", imgRG);
		imwrite("output\\v" + to_string(farVer) + "\\" + secertFileNames[i] + "_stack.png", mImgAb);

	}


	//Mat imgSecret = imread("vsimg.png");
	
	//string secretfilename = "input\\v4\\04.png";
	//Mat imgSecret = imread(secretfilename);
	//if (imgSecret.empty()) {
	//	cerr << "Error: Unable to read image! filename:"<< secretfilename << endl;
	//	exit(2);
	//}

	// Encode share image using random grid

	// Stack dual QrCode and share image
#pragma endregion
	


	waitKey(0);
	return EXIT_SUCCESS;
}



#pragma region check function
vector<int> getAPPositions(int ver) {
	if (ver == 1)
		return vector<int>();
	else {
		int numAlign = ver / 7 + 2;
		int step;
		if (ver != 32) {
			// ceil((size - 13) / (2*numAlign - 2)) * 2
			step = (ver * 4 + numAlign * 2 + 1) / (2 * numAlign - 2) * 2;
		}
		else {
			step = 26;
		}

		vector<int> result;
		for (int i = 0, pos = ver * 4 + 10; i < numAlign - 1; i++, pos -= step)
			result.insert(result.begin(), pos);
		result.insert(result.begin(), 6);
		return result;
	}
}

bool isFinderP(int y, int x, int ver)
{
	int modulesPerRow = 17 + ver * 4;

	if ((y < 7) && (x < 7)) return true;      //left top pattern
	if ((y >= modulesPerRow - 7) && (x < 7)) return true; // left bottom
	if ((y < 7) && (x >= modulesPerRow - 7)) return true; //right top

	return false;
}
bool isAlignP(int y, int x, int ver)
{
	int modulesPerRow = 17 + ver * 4;

	if ((x <= 8 && y <= 8) || (x <= 8 && y >= modulesPerRow - 9) || (x >= modulesPerRow - 9 && y <= 8)) {
		return false;
	}

	vector<int> ap = getAPPositions(ver);
	for (int i = 0; i < ap.size(); ++i) {
		if (abs(y - ap[i]) <= 2) {
			for (int j = 0; j < ap.size(); ++j) {
				if (abs(x - ap[j]) <= 2) {
					return true;
				}
			}
		}
	}
	return false;
}
// check whether a module is a Timing Pattern
bool isTimingP(int y, int x, int ver)
{
	int modulesPerRow = 17 + ver * 4;

	if ((y == 6 && ((x >= 8) && (x <= modulesPerRow - 9)))) return true;
	if ((x == 6 && ((y >= 8) && (y <= modulesPerRow - 9)))) return true;

	return false;
}

bool isFormat(int y, int x, int ver)
{
	int modulesPerRow = 17 + ver * 4;

	if ((y == 8 && (x < 9 || x > modulesPerRow - 9) && (x != 6))) return true;
	if ((x == 8 && (y < 9 || y > modulesPerRow - 8) && (y != 6))) return true;

	return false;
}
bool isVersion(int y, int x, int ver)
{
	int modulesPerRow = 17 + ver * 4;

	if (ver < 7) return false; // only for version >=7
	if (((y == modulesPerRow - 9) || (y == modulesPerRow - 10) || (y == modulesPerRow - 11)) && (x < 6))
		return true;
	if (((x == modulesPerRow - 9) || (x == modulesPerRow - 10) || (x == modulesPerRow - 11)) && (y < 6))
		return true;

	return false;
}
bool isDarkModule(int y, int x, int ver)
{
	int modulesPerRow = 17 + ver * 4;

	if ((y == modulesPerRow - 8) && (x == 8)) return true;

	return false;
}
bool isSeparator(int y, int x, int ver)
{
	int modulesPerRow = 17 + ver * 4;

	//horizontal
	if ((y == 7) && ((x < 8) || (x > modulesPerRow - 9))) return true;
	if ((y == modulesPerRow - 8) && (x < 8)) return true;

	//vertical
	if ((x == 7) && ((y < 8) || (y > modulesPerRow - 9))) return true;
	if ((x == modulesPerRow - 8) && (y < 8)) return true;

	return false;
}

bool isFunctionPatterns(int y, int x, int v) {
	return (isFinderP(y, x, v) || isAlignP(y, x, v) || isTimingP(y, x, v) || isDarkModule(y, x, v) || isSeparator(y, x, v));
}
#pragma endregion

// drawing binary QR Code with Module Size mSize ( int qZsize = 4 ; int mSize = 11 ; qr.getsize = 33)
Mat drawBinaryQRCode(const QrCode& qr, int mSize, int qzSize)
{

	int baseQRImgPixels = mSize * (qr.getSize() + 2 * qzSize);   // image size

	//cout << "qr.getSize = " << qr.getSize() << endl;

	//0xFF -> white  0x00 -> black

	Mat qrImg(baseQRImgPixels, baseQRImgPixels, CV_8UC3, Scalar(0xFF, 0xFF, 0xFF));

	for (int y = 0; y < baseQRImgPixels; y++) {
		int my = y / mSize;
		for (int x = 0; x < baseQRImgPixels; x++) {
			int mx = x / mSize;

			if ((mx < qzSize) || (mx > qr.getSize() + qzSize) || (my < qzSize) || (my > qr.getSize() + qzSize)) {  // quiet zone pixel
				qrImg.at<Vec3b>(y, x)[0] = 0xFF;
				qrImg.at<Vec3b>(y, x)[1] = 0xFF;
				qrImg.at<Vec3b>(y, x)[2] = 0xFF;
			}
			else {
				uchar color = (qr.getModule(mx - qzSize, my - qzSize) ? 0x00 : 0xFF);
				qrImg.at<Vec3b>(y, x)[0] = color;
				qrImg.at<Vec3b>(y, x)[1] = color;
				qrImg.at<Vec3b>(y, x)[2] = color;
			}
		}
	}

	return qrImg;
}

// dual message QR code
Mat dmQRCode(const QrCode& qrNear, const QrCode& qrFar, int qzSize, int mSize, int cSize) // cSize = 3;
{
	int qrNearSize = qrNear.getSize();  // module number  per row/column
	int qrFarSize = qrFar.getSize();   // module number  per row/column
	int vNear = qrNear.getVersion();

	int baseQRImgPixels = mSize * qrNear.getSize();   // image size without quiet zone
	Mat qrImg(baseQRImgPixels, baseQRImgPixels, CV_8UC3, Scalar(0xFF, 0xFF, 0xFF));


	float dist = 0;
	//unsigned char color;


	// generate dual-message QR code
	for (int y = 0; y < qrNearSize; ++y) {
		for (int x = 0; x < qrNearSize; ++x) {

			int bitNear = qrNear.getModule(x, y);//�ˬd�Ҷ���m�O�_���¦�Υզ�,�¦⬰1,�զ⬰0
			int bitFar = qrFar.getModule(x, y);

			//cout << "bitNear = " << bitNear << "  " << "bitFar = " << bitFar << endl;

			uchar color;

			//function patterns
			// draw in black and white modules
			if (isFinderP(y, x, vNear) || isAlignP(y, x, vNear) || isTimingP(y, x, vNear) || isDarkModule(y, x, vNear) || isSeparator(y, x, vNear)) {
				if (1 == bitNear) {
					color = 0x00;
				}
				else {
					color = 0xFF;
				}
				//       for (int m = 0; m < mSize; ++m) {
				   //		for (int n = 0; n < mSize; ++n) {
				   //			qrImg.at<Vec3b>(y*mSize + m, x*mSize + n)[0] = color;//�קﹳ������
				   //			qrImg.at<Vec3b>(y*mSize + m, x*mSize + n)[1] = color;
				   //			qrImg.at<Vec3b>(y*mSize + m, x*mSize + n)[2] = color;
				   //		}
				   //	}
				   //		
				   //}

				for (int m = 0; m < mSize; ++m) {
					for (int n = 0; n < mSize; ++n) {
						qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(color, color, color);//�קﹳ������
					}
				}

			}
			//  also include isFormat(y, x, vNear) || isVersion(y, x, vNear)
			else {  // data module
				if (0 == bitNear && 0 == bitFar) {
					/*for (int m = 0; m < mSize; ++m) {
						for (int n = 0; n < mSize; ++n) {
							qrImg.at<Vec3b>(y*mSize + m, x*mSize + n)[0] = 0xFF;
							qrImg.at<Vec3b>(y*mSize + m, x*mSize + n)[1] = 0xFF;
							qrImg.at<Vec3b>(y*mSize + m, x*mSize + n)[2] = 0xFF;
						}
					}*/

					for (int m = 0; m < mSize; ++m) {
						for (int n = 0; n < mSize; ++n) {
							qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0xFF, 0xFF, 0xFF);//�קﹳ������
						}
					}

				}
				else if (1 == bitNear && 1 == bitFar) {
					for (int m = 0; m < mSize; ++m) {
						for (int n = 0; n < mSize; ++n) {
							qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0x00, 0x00, 0x00);
						}
					}
				}
				//else if (0 == bitNear && 1 == bitFar) {
				//	for (int m = 0; m < mSize; ++m) {
				//		for (int n = 0; n < mSize; ++n) {
				//			// centroid region
				//			if ((m >= (mSize - cSize) / 2) && (m < (mSize - cSize) / 2 + cSize) && (n >= (mSize - cSize) / 2) && (n < (mSize - cSize) / 2 + cSize)) {
				//				qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0xFF, 0xFF, 0xFF); //�קﹳ������
				//			}
				//			else {
				//				qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0x00, 0x00, 0x00);
				//			}
				//			
				//		}
				//	}
				//}
				//else if (0 == bitNear && 1 == bitFar) {
				//	for (int m = 0; m < mSize; ++m) {
				//		for (int n = 0; n < mSize; ++n) {
				//			 //�٧Ϊ����ߦ�m
				//			int centerM = mSize / 2;
				//			int centerN = mSize / 2;
				//			 //�p��Z���ӧP�_�O�_�b�٧Τ�
				//			int Distance = abs(m - centerM) + abs(n - centerN);
				//			if (Distance <= cSize / 2) {
				//				qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0xFF, 0xFF, 0xFF); // �]�m���զ�
				//			}
				//			else {
				//				qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0x00, 0x00, 0x00); // �]�m���¦�
				//			}
				//		}
				//	}
				//}

				else if (0 == bitNear && 1 == bitFar) {
					//��ߪ�����
					float centerM = (mSize - 1) / 2.0;
					float centerN = (mSize - 1) / 2.0;
					float radius = cSize / 2.0;  // �ꪺ�b�|

					for (int m = 0; m < mSize; ++m) {
						for (int n = 0; n < mSize; ++n) {
							//�p��C���I�O�_�b��Τ�
							float distSquared = (m - centerM) * (m - centerM) + (n - centerN) * (n - centerN);

							//�ϥζꪺ�����P�_
							if (distSquared <= radius * radius) {
								qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0xFF, 0xFF, 0xFF);  // �]�m���զ�
							}
							else {
								qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0x00, 0x00, 0x00);  // �]�m���¦�
							}
						}
					}
				}


				//else if (1 == bitNear && 0 == bitFar) {
				//	for (int m = 0; m < mSize; ++m) {
				//		for (int n = 0; n < mSize; ++n) {
				//			// centroid region
				//			if ((m >= (mSize - cSize) / 2) && (m < (mSize - cSize) / 2 + cSize) && (n >= (mSize - cSize) / 2) && (n < (mSize - cSize) / 2 + cSize)) {
				//				qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0x00, 0x00, 0x00);
				//			}
				//			else {
				//				qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0xFF, 0xFF, 0xFF);
				//			}
				//		}
				//	}
				//}
				//else if (1 == bitNear && 0 == bitFar) {
				//	for (int m = 0; m < mSize; ++m) {
				//		for (int n = 0; n < mSize; ++n) {
				//			// �٧Ϊ����ߦ�m
				//			int centerM = mSize / 2;
				//			int centerN = mSize / 2;
				//			// �p��ҫ��y�Z���ӧP�_�O�_�b�٧Τ�
				//			int Distance = abs(m - centerM) + abs(n - centerN);
				//			if (Distance <= cSize / 2) {
				//				qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0x00, 0x00, 0x00);  
				//			}
				//			else {
				//				qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0xFF, 0xFF, 0xFF); 
				//			}
				//		}
				//	}
				//}

				else if (1 == bitNear && 0 == bitFar) {
					// ��ߪ�����
					float centerM = (mSize - 1) / 2.0;
					float centerN = (mSize - 1) / 2.0;
					float radius = cSize / 2.0;  // �ꪺ�b�|

					for (int m = 0; m < mSize; ++m) {
						for (int n = 0; n < mSize; ++n) {
							// �p��C���I�O�_�b��Τ�
							float distSquared = (m - centerM) * (m - centerM) + (n - centerN) * (n - centerN);

							// �ϥζꪺ�����P�_
							if (distSquared <= radius * radius) {
								qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0x00, 0x00, 0x00);
							}
							else {
								qrImg.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(0xFF, 0xFF, 0xFF);
							}
						}
					}
				}




			}
		}

	}


	int borderW = mSize * qzSize;
	copyMakeBorder(qrImg, qrImg, borderW, borderW, borderW, borderW, BORDER_CONSTANT, Scalar(255, 255, 255));

	return qrImg;
}

// �ھ� imgSecret �M qrcode �s�X���ɼv��
Mat EncodeRandomGridQrCode(Mat& imgSecret, const QrCode& qr, int qzSize, int mSize) {

	int qrSize = qr.getSize();
	int qrVer = qr.getVersion();
	int rows = imgSecret.rows;
	int cols = imgSecret.cols;

	if (rows != cols) {
		throw "the secret image is not square";
	}
	if (rows != qrSize) {
		throw "the size of the secret image and QR code image are different";
	}

	int rgQrCodeSize = mSize * qrSize;   // image size without quiet zone

	Mat imgRgQr(rgQrCodeSize, rgQrCodeSize, CV_8UC3, Scalar(0xFF, 0xFF, 0xFF));

	for (int y = 0; y < cols; ++y) {
		for (int x = 0; x < rows; ++x) {

			// ���o qr code �W���s�X
			int bitQr = qr.getModule(x, y);
			// QrCode�W���C��
			uchar colorQr = (bitQr == 1) ? 0x00: 0xFF;
			// ���ɼv�����C��
			uchar colorRg = 0xFF; 
			// �ˬd�O�_�� function patterns �A�O���ܫh����qrcode�s�X�������C��
			if (isFunctionPatterns(y, x, qrVer)) {
				for (int m = 0; m < mSize; ++m) {
					for (int n = 0; n < mSize; ++n) {
						imgRgQr.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(colorQr, colorQr, colorQr);
					}
				}
			}
			else { // data module
				// ���o���K�v��������
				Vec3b s = imgSecret.at<Vec3b>(y, x);

				if (s[0] == 0xFF) {
					// �Y���K�O�զ�, �h���ɼv������ QrCode ���C��
					colorRg = colorQr;
				}
				else {
					// �Y���K�O�¦�A�h���ɼv���P QrCode ���C��ۤ�
					colorRg = (colorQr == 0xFF) ? 0x00 : 0xFF;
				}

				for (int m = 0; m < mSize; ++m) {
					for (int n = 0; n < mSize; ++n) {
						imgRgQr.at<Vec3b>(y * mSize + m, x * mSize + n) = Vec3b(colorRg, colorRg, colorRg);
					}
				}
			}
		}
	}

	int borderW = mSize * qzSize;
	copyMakeBorder(imgRgQr, imgRgQr, borderW, borderW, borderW, borderW, BORDER_CONSTANT, Scalar(255, 255, 255));

	return imgRgQr;
}

