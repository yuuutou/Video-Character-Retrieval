#include"FaceDetector.h"
#define GetCurrentDir _getcwd

void FaceDetector::getVideoRootPath(char* path){
	//Get the video root path
	int j;
	for(j=199;j>=0;j--){
		if(path[j]!='\\')
			videoRootPath[j] = 0;
		else{
			videoRootPath[j] = '\0';
			j--;
			break;
		}		
	}
	for(;j>=0;j--){
		if(path[j]=='\\')
			videoRootPath[j] = '/';
		else
			videoRootPath[j] = path[j];
	}
}

FaceDetector::FaceDetector(){
	jni_init = false;
}

int FaceDetector::detect(HINSTANCE hInstance,HWND hDlg, char* path, char* name, int _rate, int* nfaces){

	// Load and register Progress control class
    INITCOMMONCONTROLSEX iccx;
    iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccx.dwICC = ICC_PROGRESS_CLASS;
    if (!InitCommonControlsEx(&iccx))
        return -1;

    // Create the progress bar control
    RECT rc = { 15, 170, 500, 40 };
    HWND hProgress = CreateWindowEx(0, PROGRESS_CLASS, 0, 
        WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rc.left, rc.top, rc.right, rc.bottom, 
        hDlg, (HMENU)0, hInstance, 0);

	SetDlgItemText(hDlg,IDC_EDIT_LINE1,"Detecting: Frame0...");
	SetDlgItemText(hDlg,IDC_EDIT_LINE2,"0% Finished.");
	SetDlgItemText(hDlg,IDC_EDIT_LINE3,"0 Face(s) Detected.");
	UpdateWindow(hDlg);

	VideoCapture cap(path); 
    if(!cap.isOpened()) {
		MessageBox(NULL,"Open video file failed.","Hint",MB_OK|MB_ICONEXCLAMATION);
		return -1;
	}

	//get current directory
	char cCurrentPath[200];
	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))){
		return -1;
    }

	jni.SetClass("jnitest/Facepp");
	jni.SelectMethod("detect","(Ljava/lang/String;II)[D");

	int rate = _rate;
    Mat frame;
	Mat oldFrame;
	bool isDetecting = true;
	int frameCount = 0;
	int frameHeight;
	int frameWidth;
	int numDetected = 0;
	int totalFrame = cap.get(CV_CAP_PROP_FRAME_COUNT);

	SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, totalFrame));
  
    SendMessage(hProgress, PBM_SETSTEP, (WPARAM) 1, 0);

	mkdir(name);
	s.str("");
	s.clear();
	s<<name<<"\\faces";
	mkdir(s.str().c_str());

	for(;;){
		int progress = (int)frameCount*100/totalFrame;
		s.str("");
		s.clear();
		s<<progress<<"% Finished.";
		SetDlgItemText(hDlg,IDC_EDIT_LINE2,s.str().c_str());

		UpdateWindow(hDlg);

		if(!cap.grab()){
			cout<<"finished"<<endl;
			break;
		}			
		if(frameCount%rate==0){

			s.str("");
			s.clear();
			s<<"Detecting: Frame"<<frameCount<<"...";
			SetDlgItemText(hDlg,IDC_EDIT_LINE1,s.str().c_str());

			if(frame.data){
				oldFrame = frame.clone();
			}
			cap.retrieve(frame);
			if(oldFrame.data){
				double frameDistance;
				frameDistance = calculateFrameDistance(oldFrame,frame);
				cout<<"frame"<<frameCount<<"distance:"<<frameDistance<<endl;
				if(frameDistance>75)//A new key frame comes!
					isDetecting = true;
			}
			else{
				frameHeight = frame.rows;
				frameWidth = frame.cols;
			}
			if(isDetecting){
				//do detection
				s.str("");
				s.clear();
				s<<"frame"<<frameCount<<".jpg";
				imwrite(s.str(),frame);
				s.str("");
				s.clear();
				s<<cCurrentPath<<"\\frame"<<frameCount<<".jpg";
				jstring arg = jni.NewJString(s.str().c_str());
				jdoubleArray ja = (jdoubleArray)jni.CallMethod(arg,frameHeight,frameWidth);
				jint n = jni.GetArrayLength(ja);
				remove(s.str().c_str());
				if(n!=1){
					isDetecting = false;
					jboolean isCopy1;
					jdouble* faces = jni.GetDoubleArrayElements(ja, &isCopy1);
					int numFaces = (int)faces[0];
					numDetected += numFaces;
					s.str("");
					s.clear();
					s<<numDetected<<" Face(s) Detected.";
					SetDlgItemText(hDlg,IDC_EDIT_LINE3,s.str().c_str());
					for(int i=0;i<numFaces;i++){
						float faceWidth = frameWidth*faces[5*i+4]*0.01;
						float faceHeight = frameWidth*faces[5*i+3]*0.01;
						float faceCenterX = frameWidth*faces[5*i+1]*0.01;
						float faceCenterY = frameHeight*faces[5*i+2]*0.01;
						Rect faceRect = Rect(Point2f(faceCenterX-0.5*faceWidth,faceCenterY-0.5*faceHeight), Size2f(faceWidth,faceHeight));
						Mat faceROI = frame( faceRect );
						s.str("");
						s.clear();
						s<<name<<"/faces/frame"<<frameCount<<"face"<<i<<".jpg";
						imwrite(s.str(),faceROI);
					}
					if (isCopy1 == JNI_TRUE) 	
						jni.ReleaseDoubleArrayElements(ja, faces);				
				}		
			}
		}
		frameCount++;
		SendMessage(hProgress, PBM_STEPIT, 0, 0);
    }
	cap.release();

	*nfaces = numDetected;

	return 0;
}

int FaceDetector::buildIndex(HWND hDlg, char* name){

	SetDlgItemText(hDlg,IDC_EDIT_LINE1,"Indexing...");
	SetDlgItemText(hDlg,IDC_EDIT_LINE2,"");
	SetDlgItemText(hDlg,IDC_EDIT_LINE3,"");
	UpdateWindow(hDlg);

	//get current directory
	char cCurrentPath[200];
	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))){
		return -1;
    }

	//build index
	jni.SetClass("jnitest/Indexer");	
	jni.SelectMethod("buildIndex","(Ljava/lang/String;)I");
	s.str("");
	s.clear();
	s<<cCurrentPath<<"\\"<<name<<"\\faces";
	jstring arg1 = jni.NewJString(s.str().c_str());
	jint m = (jint)jni.CallMethod(arg1);
	if(m!=0){
		cout<<"error"<<endl;
		return -1;
	}

	return 0;
}

int FaceDetector::searchIndex(char* path, char* name, int numSearch){
	//search index
	jni.SetClass("jnitest/Searcher");
	jni.SelectMethod("searchIndex","(Ljava/lang/String;Ljava/lang/String;I)[Ljava/lang/String;");
	s.str("");
	s.clear();
	s<<path;
	jstring arg2 = jni.NewJString(s.str().c_str());

	//get current directory
	char cCurrentPath[200];
	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))){
		return -1;
    }

	s.str("");
	s.clear();
	s<<cCurrentPath<<"\\"<<name<<"\\index";
	jstring arg3 = jni.NewJString(s.str().c_str());

	jobjectArray joa = (jobjectArray)jni.CallMethod(arg2,arg3,numSearch);
	int stringCount = jni.GetArrayLength(joa);
	cout<<stringCount<<endl;

	//get screen resolution
	int horizontal = 0;
    int vertical = 0;
	GetDesktopResolution(horizontal, vertical);

	//show the images
	int x = 50;
	int y = 50;
	int y_step = 0;
	for (int i=0; i<stringCount; i++){
		jstring js = (jstring)jni.GetObjectArrayElement(joa,i);
		string str = jni.JStringToCString(js);
		Mat image;
		image = imread( str, 1 );
		s.str("");
		s.clear();
		s<<"face"<<i;
		imshow(s.str(),image);
		if(x+image.cols>horizontal){
			x = 50;
			if(y_step>300)
				y += 300;
			else
				y += y_step;
		}
		moveWindow(s.str(),x,y);
		if(x==50)
			y_step = image.rows;
		x += image.cols;
	}

	waitKey(0);
	return 0;
}

void FaceDetector::jniFree(){
	if(jni_init)
		jni.free();
}

void FaceDetector::jniInit(){
	if(jni.init()!=0)
		MessageBox(NULL,"JNI init problem.","Hint",MB_OK|MB_ICONEXCLAMATION);	
	else
		jni_init = true;
}

double FaceDetector::calculateFrameDistance(Mat oldFrame, Mat frame){
	Mat dist;
	Scalar sumDist;
	absdiff(oldFrame,frame,dist);
	sumDist = sum(dist);
	return (sumDist[0]+sumDist[1]+sumDist[2])/(dist.rows*dist.cols);
}

void FaceDetector::GetDesktopResolution(int& horizontal, int& vertical){
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}