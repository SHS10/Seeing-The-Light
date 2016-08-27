#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include <opencv2\core\core.hpp>
#include "iostream"
#include "windows.h" 

// *Please ensure that webcam app is open before the running this program*

using namespace cv;
using namespace std;

int main(int argc, const char** argv)
{ 
	int maxWidth = GetSystemMetrics(SM_CXSCREEN);	//get the width and height of the screen itself
	int maxHeight = GetSystemMetrics(SM_CYSCREEN);

	VideoCapture cap(0); //open camera
	if (!cap.isOpened())
		return -1;
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, maxHeight); //set the maximum width and height of the screen the same as the screen width and height
	cap.set(CV_CAP_PROP_FRAME_WIDTH, maxWidth);

	Mat frame; //store the frame of captured in matrix frame

	double tempBGR[3]; //create a temp array
	double L, M, S, LShift, MShift, SShift; 
	//create an infinite loop for processing each frame
	for (;;){
		cap >> frame; //let the captured frame of 3x3 matrices from the camera feed 'cap' be assigned as 'frame'
		for (int i = 1; i < frame.rows; i++){
			for (int j = 1; j < frame.cols; j++){ //for the dimensions of the matrix

				//values from the camera are transfered to vector values to be stored in tempBGR

				tempBGR[0] = frame.at<Vec3b>(i, j)[0]; //store the blue value from the frame captured
				tempBGR[1] = frame.at<Vec3b>(i, j)[1]; //store the green value from the frame captured
				tempBGR[2] = frame.at<Vec3b>(i, j)[2]; //store the red value from the frame captured

				// make a separate float matrix to store LMS and then convert to Mat format later
				L = 17.8824 * tempBGR[2] + 43.5161 * tempBGR[1] + 4.1193 * tempBGR[0];
				M = 3.45570 * tempBGR[2] + 27.1554 * tempBGR[1] + 3.8671 * tempBGR[0];
				S = 0.02996 * tempBGR[2] + 0.18431 * tempBGR[1] + 1.4670 * tempBGR[0];

				/*
				// LMS to L'M'S' {Tritanopia}
				LShift = 1 * L + 0 * M + 0 * S;
				MShift = 0 * L + 1 * M + 0 * S;
				SShift = -0.395913 * L + 0.801109 * M + 0 * S;
				*/

				/*
				// LMS to L'M'S' {Deuteranopia}
				LShift = 1 * L + 0 * M + 0 * S;
				MShift = 0.494207 * L + 0 * M + 1.24827 * S;
				SShift = 0 * L + 0 * M + 1 * S;
				*/

				// LMS to L'M'S' {Protanopia} //values for matrix shifting gotten from daltonize.py --note that this is a value for simulating pure protanipia
				LShift = 0 * L + 2.02344 * M - 2.52581 * S;
				MShift = 0 * L + 1 * M + 0 * S;
				SShift = 0 * L + 0 * M + 1 * S;

				// l'M'S' to BGR
				tempBGR[2] = 0.080944942 * LShift - 0.130505254 * MShift + 0.116728267 * SShift;
				tempBGR[1] = -0.010248719 * LShift + 0.05401967 * MShift - 0.11362094 * SShift;
				tempBGR[0] = -0.000365487 * LShift - 0.004121628 * MShift + 0.693554394 * SShift;


				frame.at<Vec3b>(i, j)[0] = tempBGR[0]; //convert back to frame after doing relevant color conversion
				frame.at<Vec3b>(i, j)[1] = tempBGR[1]; 
				frame.at<Vec3b>(i, j)[2] = tempBGR[2];

			}

		}
		if (waitKey(30) >= 0) break;
		imshow("conversion", frame); //display converted frame with window title of "conversion"
	}

}
