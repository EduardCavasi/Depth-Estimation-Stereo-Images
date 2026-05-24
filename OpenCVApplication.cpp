// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>
#include<bitset>
#include <fstream>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <intrin.h>
#include <vector>
#include <cstdio>

using namespace std;
wchar_t* projectPath;

void testOpenImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image",src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName)==0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName,"bmp");
	while(fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(),src);
		if (waitKey()==27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	_wchdir(projectPath);

	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", IMREAD_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, COLOR_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	waitKey(0);
}

void testNegativeImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]
		
		Mat src = imread(fname,IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height,width,CV_8UC1);
		// Accessing individual pixels in an 8 bits/pixel image
		// Inefficient way -> slow
		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				uchar val = src.at<uchar>(i,j);
				uchar neg = 255 - val;
				dst.at<uchar>(i,j) = neg;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image",src);
		imshow("negative image",dst);
		waitKey();
	}
}

void testNegativeImageFast()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = src.clone();

		double t = (double)getTickCount(); // Get the current time [s]

		// The fastest approach of accessing the pixels -> using pointers
		uchar *lpSrc = src.data;
		uchar *lpDst = dst.data;
		int w = (int) src.step; // no dword alignment is done !!!
		for (int i = 0; i<height; i++)
			for (int j = 0; j < width; j++) {
				uchar val = lpSrc[i*w + j];
				lpDst[i*w + j] = 255 - val;
			}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image",src);
		imshow("negative image",dst);
		waitKey();
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src = imread(fname);

		int height = src.rows;
		int width = src.cols;

		Mat dst = Mat(height,width,CV_8UC1);

		// Accessing individual pixels in a RGB 24 bits/pixel image
		// Inefficient way -> slow
		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i,j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst.at<uchar>(i,j) = (r+g+b)/3;
			}
		}
		
		imshow("input image",src);
		imshow("gray image",dst);
		waitKey();
	}
}

void testBGR2HSV()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int height = src.rows;
		int width = src.cols;

		// HSV components
		Mat H = Mat(height, width, CV_8UC1);
		Mat S = Mat(height, width, CV_8UC1);
		Mat V = Mat(height, width, CV_8UC1);

		// Defining pointers to each matrix (8 bits/pixels) of the individual components H, S, V 
		uchar* lpH = H.data;
		uchar* lpS = S.data;
		uchar* lpV = V.data;

		Mat hsvImg;
		cvtColor(src, hsvImg, COLOR_BGR2HSV);

		// Defining the pointer to the HSV image matrix (24 bits/pixel)
		uchar* hsvDataPtr = hsvImg.data;

		for (int i = 0; i<height; i++)
		{
			for (int j = 0; j<width; j++)
			{
				int hi = i*width * 3 + j * 3;
				int gi = i*width + j;

				lpH[gi] = hsvDataPtr[hi] * 510 / 360;	// lpH = 0 .. 255
				lpS[gi] = hsvDataPtr[hi + 1];			// lpS = 0 .. 255
				lpV[gi] = hsvDataPtr[hi + 2];			// lpV = 0 .. 255
			}
		}

		imshow("input image", src);
		imshow("H", H);
		imshow("S", S);
		imshow("V", V);

		waitKey();
	}
}

void testResize()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1,dst2;
		//without interpolation
		resizeImg(src,dst1,320,false);
		//with interpolation
		resizeImg(src,dst2,320,true);
		imshow("input image",src);
		imshow("resized image (without interpolation)",dst1);
		imshow("resized image (with interpolation)",dst2);
		waitKey();
	}
}

void testCanny()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src,dst,gauss;
		src = imread(fname,IMREAD_GRAYSCALE);
		double k = 0.4;
		int pH = 50;
		int pL = (int) k*pH;
		GaussianBlur(src, gauss, Size(5, 5), 0.8, 0.8);
		Canny(gauss,dst,pL,pH,3);
		imshow("input image",src);
		imshow("canny",dst);
		waitKey();
	}
}

void testVideoSequence()
{
	_wchdir(projectPath);

	VideoCapture cap("Videos/rubic.avi"); // off-line video from file
	//VideoCapture cap(0);	// live video from web cam
	if (!cap.isOpened()) {
		printf("Cannot open video capture device.\n");
		waitKey(0);
		return;
	}
		
	Mat edges;
	Mat frame;
	char c;

	while (cap.read(frame))
	{
		Mat grayFrame;
		cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
		Canny(grayFrame,edges,40,100,3);
		imshow("source", frame);
		imshow("gray", grayFrame);
		imshow("edges", edges);
		c = waitKey(100);  // waits 100ms and advances to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished\n"); 
			break;  //ESC pressed
		};
	}
}


void testSnap()
{
	_wchdir(projectPath);

	VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
	if (!cap.isOpened()) // openenig the video device failed
	{
		printf("Cannot open video capture device.\n");
		return;
	}

	Mat frame;
	char numberStr[256];
	char fileName[256];
	
	// video resolution
	Size capS = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CAP_PROP_FRAME_HEIGHT));

	// Display window
	const char* WIN_SRC = "Src"; //window for the source frame
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Snapped"; //window for showing the snapped frame
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, capS.width + 10, 0);

	char c;
	int frameNum = -1;
	int frameCount = 0;

	for (;;)
	{
		cap >> frame; // get a new frame from camera
		if (frame.empty())
		{
			printf("End of the video file\n");
			break;
		}

		++frameNum;
		
		imshow(WIN_SRC, frame);

		c = waitKey(10);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished");
			break;  //ESC pressed
		}
		if (c == 115){ //'s' pressed - snap the image to a file
			frameCount++;
			fileName[0] = NULL;
			sprintf(numberStr, "%d", frameCount);
			strcat(fileName, "Images/A");
			strcat(fileName, numberStr);
			strcat(fileName, ".bmp");
			bool bSuccess = imwrite(fileName, frame);
			if (!bSuccess) 
			{
				printf("Error writing the snapped image\n");
			}
			else
				imshow(WIN_DST, frame);
		}
	}

}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == EVENT_LBUTTONDOWN)
		{
			printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
				x, y,
				(int)(*src).at<Vec3b>(y, x)[2],
				(int)(*src).at<Vec3b>(y, x)[1],
				(int)(*src).at<Vec3b>(y, x)[0]);
		}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

/* Histogram display function - display a histogram using bars (simlilar to L3 / Image Processing)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i<hist_cols; i++)
	if (hist[i] > max_hist)
		max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}


/// START

const int MAX_DISPARITY = 640;
String FILE_NAME = "jade";

cv::Mat loadPFM(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("Cannot open PFM file: " + filename);

	std::string type;
	file >> type;

	if (type != "Pf" && type != "PF")
		throw std::runtime_error("Invalid PFM header");

	bool color = (type == "PF");

	int width, height;
	file >> width >> height;

	float scale;
	file >> scale;

	// skip single whitespace/newline after header
	file.ignore(1);

	bool littleEndian = (scale < 0);
	(void)littleEndian; // usually ignored on x86 (little endian)

	scale = std::abs(scale);

	int channels = color ? 3 : 1;

	cv::Mat img(height, width, CV_32FC(channels));

	size_t dataSize = width * height * channels * sizeof(float);

	file.read(reinterpret_cast<char*>(img.data), dataSize);

	if (!file)
		throw std::runtime_error("Unexpected end of file while reading PFM");

	// PFM is stored bottom-to-top → flip it
	cv::flip(img, img, 0);

	return img;
}

typedef enum DISP_TYPE {LEFT, RIGHT};

void getNeighbourhood(int size, vector<int> &di, vector<int> &dj) {
	for (int i = -size; i <= size; ++i) {
		for (int j = -size; j <= size; ++j) {
			if (i == 0 && j == 0) continue;
			di.push_back(i);
			dj.push_back(j);
		}
	}
}

bool isInside(Mat& img, int i, int j) {
	if (i < 0 || i >= img.rows)
		return false;
	if (j < 0 || j >= img.cols)
		return false;
	return true;
}

vector<vector<uint64_t>> getCensusTrasform(Mat_<uchar>& img, int neighbourhoodSize) {
	vector<int> di, dj;
	getNeighbourhood(neighbourhoodSize, di, dj);
	assert(di.size() <= 64);
	vector<vector<uint64_t>> censusTransformResult;
	Mat_<uchar> padded;
	copyMakeBorder(img, padded, neighbourhoodSize, neighbourhoodSize, neighbourhoodSize, neighbourhoodSize, BORDER_REFLECT_101);
	for (int i = 0; i < img.rows; ++i) {
		vector<uint64_t> row;
		for (int j = 0; j < img.cols; ++j) {
			uint64_t curPixel = padded(i + neighbourhoodSize, j + neighbourhoodSize);
			uint64_t censusPixel = 0;
			for (int k = 0; k < di.size(); ++k) {
				int ii = i + di[k];
				int jj = j + dj[k];
				int curNeigh = padded(ii + neighbourhoodSize, jj + neighbourhoodSize);

				censusPixel |= (uint64_t(curPixel >= curNeigh) << k);
			}
			row.push_back(censusPixel);
		}
		censusTransformResult.push_back(row);
	}
	return censusTransformResult;
}

int hamming(uint64_t a, uint64_t b) {
	return __popcnt64(a ^ b);
}

Mat_<int> getDisparityMap(Mat_<uchar>& img1, Mat_<uchar>& img2, DISP_TYPE type) {
	if (img1.size() != img2.size()) {
		throw runtime_error("Images must have same size!");
	}
	const int NEIGH_SIZE = 3; // 5 x 5 neighbourhood
	auto censusImg1 = getCensusTrasform(img1, NEIGH_SIZE);
	auto censusImg2 = getCensusTrasform(img2, NEIGH_SIZE);
	Mat_<int> disparity(img1.size());
	disparity.setTo(-1);
	Mat_<float> best_cost(img1.size());
	best_cost.setTo(FLT_MAX);
	Mat_<int> cost(img1.size());
	int rows = censusImg1.size();
	int cols = censusImg1[0].size();
	for (int d = 0; d < MAX_DISPARITY; ++d) {
		cost.setTo(0);

		//compute hamming costs for current displacement
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				int j_displaced = (type == LEFT ? j - d : j + d);
				if (j_displaced < 0 || j_displaced >= cols) {
					cost(i, j) = 64;
					continue;
				}
				uint64_t cL = censusImg1[i][j];
				uint64_t cR = censusImg2[i][j_displaced];

				cost(i, j) = hamming(cL, cR);
			}
		}

		//compute partial sum cost matrix
		for (int i = 1; i < rows; ++i)
			cost(i, 0) += cost(i - 1, 0);
		for (int j = 1; j < cols; ++j)
			cost(0, j) += cost(0, j - 1);
		for (int i = 1; i < rows; ++i)
			for (int j = 1; j < cols; ++j)
				cost(i, j) += cost(i - 1, j) + cost(i, j - 1) - cost(i - 1, j - 1);

		//window averaging (2r+1)x(2r+1) window
		const int r = 15;
		int first_row, first_col, last_row, last_col, neighs;
		uint64_t window_cost;
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				//pixel (i,j) is the center of the window
				first_row = max(i - r, 0);
				first_col = max(j - r, 0);
				last_row = min(i + r, rows - 1);
				last_col = min(j + r, cols - 1);
				window_cost = cost(last_row, last_col) - (i - r - 1 >= 0 ? cost(i - r - 1, last_col) : 0) - (j - r - 1 >= 0 ? cost(last_row, j - r - 1) : 0) + (i - r - 1 >= 0 && j - r - 1 >= 0 ? cost(i - r - 1, j - r - 1) : 0);
				neighs = (last_row - first_row + 1) * (last_col - first_col + 1);
				float cur_cost = window_cost / float(neighs);
				if (cur_cost < best_cost(i, j)) {
					best_cost(i, j) = cur_cost;
					disparity(i, j) = d;
				}
			}
		}
	}
	
	return disparity;
}

Mat_<int> transformDisparityMapToDepthMap(Mat_<int>& disp, float baseline, float f, float doffs) {
	Mat_<int> depth(disp.size());
	for (int i = 0; i < disp.rows; ++i) {
		for (int j = 0; j < disp.cols; ++j) {
			depth(i, j) = baseline * f / (disp(i, j) + doffs);
		}
	}
	return depth;
}

uchar normalization_function(int x) {
	if (x < 0) return 0;
	return x * 255.0 / (MAX_DISPARITY - 1);
}


Mat_<int> lrCheck(Mat_<int>& leftDisp, Mat_<int>& rightDisp, int threshold = 1) {
	Mat_<int> dst = leftDisp.clone();
	for (int i = 0; i < leftDisp.rows; ++i) {
		for (int j = 0; j < leftDisp.cols; ++j) {
			int d = leftDisp(i, j);

			int jR = j - d;

			if (jR < 0 || jR >= rightDisp.cols) {
				dst(i, j) = -1; // out of bounds = occluded
				continue;
			}

			// check if right disparity agrees
			if (abs(d - rightDisp(i, jR)) > threshold) {
				dst(i, j) = -1; // mark as invalid
			}
		}
	}
	return dst;
}

Mat_<int> holeFill(Mat_<int>& src) {
	Mat_<int> dst(src.size());
	for (int i = 0; i < src.rows; ++i) {
		for (int j = 0; j < src.cols; ++j) {
			if (src(i, j) != -1) {
				dst(i, j) = src(i, j);
				continue;
			}

			// scan left
			int leftVal = -1;
			for (int k = j - 1; k >= 0; --k) {
				if (src(i, k) != -1) { leftVal = src(i, k); break; }
			}

			// scan right
			int rightVal = -1;
			for (int k = j + 1; k < src.cols; ++k) {
				if (src(i, k) != -1) { rightVal = src(i, k); break; }
			}

			if (leftVal != -1 && rightVal != -1) dst(i, j) = min(leftVal, rightVal);
			else if (leftVal != -1)              dst(i, j) = leftVal;
			else if (rightVal != -1)             dst(i, j) = rightVal;
			// if no valid neighbors found, leave as -1 for now
		}
	}
	return dst;
}

Mat_<int> medianFilter(Mat_<int> &src, int KERNEL_SIZE) {
	Mat_<int> dst(src.size());
	//only odd KERNEL_SIZE
	int HALF = KERNEL_SIZE / 2;
	Mat_<int> padded;
	copyMakeBorder(src, padded, KERNEL_SIZE / 2, KERNEL_SIZE / 2, KERNEL_SIZE / 2, KERNEL_SIZE / 2, BORDER_CONSTANT, Scalar(-1));
	vector<int> neigh;
	neigh.reserve(KERNEL_SIZE * KERNEL_SIZE);

	for (int i = 0; i < src.rows; ++i) {
		for (int j = 0; j < src.cols; ++j) {
			neigh.clear();
			for (int u = -HALF; u <= HALF; ++u) {
				for (int v = -HALF; v <= HALF; ++v) {
					if (padded(i + HALF + u, j + HALF + v) != -1)
						neigh.push_back(padded(i + HALF + u, j + HALF + v));
				}
			}
			if (neigh.size() == 0) {
				dst(i, j) = src(i, j);
				continue;
			}
			nth_element(neigh.begin(), neigh.begin() + neigh.size() / 2, neigh.end());
			int median = neigh[neigh.size() / 2];
			if (neigh.size() % 2 == 0) {
				nth_element(neigh.begin(), neigh.begin() + neigh.size() / 2 + 1, neigh.end());
				median = (median + neigh[neigh.size() / 2 + 1]) / 2;
			}
			dst(i, j) = median;
		}
	}
	return dst;
}

Mat_<uchar> normalize(Mat_<int> &src) {
	Mat_<uchar> dst(src.size());
	for (int i = 0; i < src.rows; ++i) {
		for (int j = 0; j < src.cols; ++j) {
			dst(i, j) = normalization_function(src(i, j));
		}
	}
	return dst;
}

void calculate_metrics(Mat_<int> &disparity, Mat_<int> &gt) {
	float total_epe = 0;
	float nr_valid = 0;
	float nr_total_valid_gt = 0;
	float nr_rejected = 0;
	float nr_bad = 0;
	const int T = 2;
	for (int i = 0; i < disparity.rows; ++i) {
		for (int j = 0; j < disparity.cols; ++j) {
			if (gt(i, j) < 0) continue;
			++nr_total_valid_gt;
			if (disparity(i, j) < 0) {
				++nr_rejected;
				continue;
			}
			float err = abs(disparity(i, j) - gt(i, j));
			total_epe += err;
			++nr_valid;
			if (err > T) {
				++nr_bad;
			}
		}
	}
	cout << "TOTAL PIXELS: " << disparity.rows * disparity.cols << endl;
	cout << std::fixed << std::setprecision(2) << "GT VALID PIXELS: " << (int)nr_total_valid_gt << endl;
	cout << std::fixed << std::setprecision(2) << "DISP VALID PIXELS: " << (int)nr_valid << endl;
	cout << std::fixed << std::setprecision(2) << "DISP COVERAGE: " << nr_valid / nr_total_valid_gt * 100.0 << "%" << endl;
	cout << std::fixed << std::setprecision(2) << "BAD PIXEL PERCENTAGE (predictions only): " << nr_bad / nr_valid * 100.0f << "%" << endl;
	cout << std::fixed << std::setprecision(2) << "BAD PIXEL PERCENTAGE (all GT): " << (nr_bad + nr_rejected) / nr_total_valid_gt * 100.0f << "%" << endl;
	cout << std::fixed << std::setprecision(2) << "REJECTED:                                " << nr_rejected / nr_total_valid_gt * 100.0f << "%" << endl;
	cout << std::fixed << std::setprecision(2) << "MEAN EPE: " << total_epe / nr_valid << endl;
}

int main() 
{
	//read
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
    projectPath = _wgetcwd(0, 0);
	Mat_<uchar> left_img = imread("Images/" + FILE_NAME + "_left.png", 0);
	Mat_<uchar> right_img = imread("Images/" + FILE_NAME + "_right.png", 0);
	/*float baseline = 380.135;
	float doffs = 809.195;
	float f = 3997.684;
	*/


	//process
	Mat_<int> left_disp = getDisparityMap(right_img, left_img, LEFT);
	Mat_<int> right_disp = getDisparityMap(left_img, right_img, RIGHT);
	//Mat gt, igt;
	//left_disp_GT.convertTo(igt, CV_8U);
	//Mat right_disp_GT = loadPFM("Images/disp1.pfm");
	Mat_<int> hole_mark_ld = lrCheck(left_disp, right_disp);
	//Mat_<int> hole_fill_ld = holeFill(hole_mark_ld);
	Mat_<int> filtered_ld = medianFilter(medianFilter(hole_mark_ld, 5), 5);
	//Mat_<int> filtered_ld = hole_mark_ld;

	//error
	Mat left_disp_GT_floats = loadPFM("Images/" + FILE_NAME + "_disp0.pfm");
	//swap inf with -1 and transform to int
	Mat_<int> left_disp_GT(left_disp_GT_floats.size());
	for (int i = 0; i < left_disp_GT_floats.rows; ++i) {
		for (int j = 0; j < left_disp_GT_floats.cols; ++j) {
			float v = left_disp_GT_floats.at<float>(i, j);
			left_disp_GT(i, j) = (std::isinf(v) || v <= 0) ? -1 : (int)v;
		}
	}
	calculate_metrics(filtered_ld, left_disp_GT);
	//view
	//Mat colorized;
	//applyColorMap(normalized_ld, colorized, COLORMAP_JET);
	/*imshow("initial left", left_img);
	imshow("initial right", right_img);
	imshow("left_disp", ld);
	imshow("gt", gt);
	waitKey(0);*/
	Mat_<uchar> normalized_ld = normalize(filtered_ld);
	remove(("Images/" + FILE_NAME + "_ld.bmp").c_str());
	imwrite("Images/" + FILE_NAME + "_ld.png", normalized_ld);
	return 0;
}