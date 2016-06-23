#pragma region Includes and Manifest Dependencies
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include <assert.h>
#include <shobjidl.h>
#include <Shlobj.h>

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>
#include <direct.h>
#include "myJNI.h"

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

using namespace cv;
using namespace std;

class FaceDetector{
public:
	FaceDetector();
	int detect(HINSTANCE hInstance, HWND hDlg, char* path, char* name, int _rate, int* nfaces);
	int buildIndex(HWND hDlg, char* name);
	int searchIndex(char* path, char* name, int numSearch);
	void getVideoRootPath(char* path);
	void GetDesktopResolution(int& horizontal, int& vertical);
	void jniFree();
	void jniInit();
	bool jni_init;
private:
	double calculateFrameDistance(Mat oldFrame, Mat frame);
	JNI jni;
	stringstream s;
	char videoRootPath[200];	
};