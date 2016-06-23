#ifndef UNICODE
#define UNICODE
#endif 

#include "FaceDetector.h"
#define GetCurrentDir _getcwd

HINSTANCE g_hInst;  // The handle to the instance of the current module
bool startDetectAndBuild;
bool startSearch;
bool showSearchDlg;
int rate;
int numFaces;
int numSearch;
char videoPath[200];//video path
char queryFile[200];
char videoFileName[100];

int countFile(const string& dirName_in){
	//count the number of jpg files in a given path
	string str;
	str = dirName_in +  "\\*.jpg";
	wchar_t temp[200];
	MultiByteToWideChar(CP_ACP,0,str.c_str(),-1,temp,sizeof(temp));
	int count = 0;
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(temp, &fd);
	if (h != INVALID_HANDLE_VALUE) {
		do {
			count++; 
		} while (FindNextFile(h, &fd));
    FindClose(h);
	}
	return count;
}

bool dirExists(const string& dirName_in){
	//check if the directory exists
    DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

void getVideoName(){
	//Get the name of the video from video path
	int i,j;
	int count = 0;
	char temp[100];
	for(j=199;videoPath[j]!='.';j--);
	j--;
	for(;videoPath[j]!='\\';j--,count++)
		temp[count] = videoPath[j];
	for(i = 0;i<count;i++)
		videoFileName[i] = temp[count-1-i];
	videoFileName[i] = '\0';
}

string getFacesPath(){
	//Get the path of the faces folder
	stringstream s;
	char cCurrentPath[200];
	GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));
	s.str("");
	s.clear();
	s<<cCurrentPath<<"\\"<<videoFileName<<"\\faces";
	return s.str();
}

BOOL OnInitSearchDialog(HWND hWnd, HWND hwndFocus, LPARAM lParam)
{	
	stringstream s;
	wchar_t temp[200];	
	s<<"In total, we have found "<<numFaces<<" face(s).";
	MultiByteToWideChar(CP_ACP,0,s.str().c_str(),-1,temp,sizeof(temp));
	SetDlgItemText(hWnd,IDC_EDIT3,temp);
	if(numFaces>20)
		SetDlgItemInt(hWnd,IDC_EDIT2,20,FALSE);
	else
		SetDlgItemInt(hWnd,IDC_EDIT2,numFaces,FALSE);
    return TRUE;
}

BOOL OnInitWaitDialog(HWND hWnd, HWND hwndFocus, LPARAM lParam)
{
    return TRUE;
}

void OnClose(HWND hWnd)
{
    EndDialog(hWnd, 0);
}

void OnCommandSearch(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
    switch (id)
    {
	case IDC_BUTTON1:
		{
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
				COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr))
			{
				IFileOpenDialog *pFileOpen;

				// Create the FileOpenDialog object.
				hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
						IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

				if (SUCCEEDED(hr))
				{
					IShellItem *pItem;
					ITEMIDLIST *pidl;
					wchar_t temp[200];
					MultiByteToWideChar(CP_ACP,0,getFacesPath().c_str(),-1,temp,sizeof(temp));
					SHParseDisplayName(temp,NULL,&pidl,0,0);
					SHCreateShellItem(NULL,NULL,pidl,&pItem);
					pFileOpen->SetFolder(pItem);
					// Show the Open dialog box.
					hr = pFileOpen->Show(NULL);

					// Get the file name from the dialog box.
					if (SUCCEEDED(hr))
					{
						
						hr = pFileOpen->GetResult(&pItem);
						if (SUCCEEDED(hr))
						{
							PWSTR pszFilePath;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

							// Display the file name to the user.
							if (SUCCEEDED(hr))
							{
								SetDlgItemText(hWnd,IDC_EDIT1,pszFilePath);
								CoTaskMemFree(pszFilePath);
							}
							pItem->Release();
						}
					}
					pFileOpen->Release();
				}
				CoUninitialize();
			}
        }
		break;

	case IDC_BUTTON2:
		{
			wchar_t buf1[200];
			BOOL success1;
			numSearch = GetDlgItemInt(hWnd,IDC_EDIT2,&success1,false);
			if(!success1){
				MessageBox(NULL,L"Please indicate the number of images that will be displayed.",L"Hint",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
			if(numSearch>numFaces){
				MessageBox(NULL,L"N is too large.",L"Hint",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
			if (!GetDlgItemText(hWnd, IDC_EDIT1, buf1, 200)){
				MessageBox(NULL,L"Please select a query image.",L"Hint",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
			WideCharToMultiByte(CP_ACP,0,buf1,-1,queryFile,sizeof(queryFile),NULL,NULL);

			startSearch = true;	
			EndDialog(hWnd, 0);
		}
		break;

    case IDOK:
    case IDCANCEL:
        EndDialog(hWnd, 0);
        break;
    }
}

INT_PTR CALLBACK searchDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitDateTimePickDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitSearchDialog);

		// Handle the WM_COMMAND message in OnCommandSearch
        HANDLE_MSG (hWnd, WM_COMMAND, OnCommandSearch);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;	// Let system deal with msg
    }
    return 0;
}

INT_PTR CALLBACK waitDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitDateTimePickDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitWaitDialog);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;	// Let system deal with msg
    }
    return 0;
}

BOOL OnInitDialog(HWND hWnd, HWND hwndFocus, LPARAM lParam)
{
	SetDlgItemInt(hWnd,IDC_EDIT_RATE,10,FALSE);
    return TRUE;
}

void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDC_BUTTON_EXIT:
        EndDialog(hWnd, 0);
        break;
	
	case IDC_BUTTON_OPEN:
		{
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
				COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr))
			{
				IFileOpenDialog *pFileOpen;

				// Create the FileOpenDialog object.
				hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
						IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

				if (SUCCEEDED(hr))
				{
					// Show the Open dialog box.
					hr = pFileOpen->Show(NULL);

					// Get the file name from the dialog box.
					if (SUCCEEDED(hr))
					{
						IShellItem *pItem;
						hr = pFileOpen->GetResult(&pItem);
						if (SUCCEEDED(hr))
						{
							PWSTR pszFilePath;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

							// Display the file name to the user.
							if (SUCCEEDED(hr))
							{
								SetDlgItemText(hWnd,IDC_EDIT_PATH,pszFilePath);
								CoTaskMemFree(pszFilePath);
							}
							pItem->Release();
						}
					}
					pFileOpen->Release();
				}
				CoUninitialize();
			}
        }
		break;

	case IDC_BUTTON_START:
		{
			wchar_t buf1[200];
			BOOL success1;
			rate = GetDlgItemInt(hWnd,IDC_EDIT_RATE,&success1,false);
			if(!success1){
				MessageBox(NULL,L"Please indicate the sampling rate.",L"Hint",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
			if (!GetDlgItemText(hWnd, IDC_EDIT_PATH, buf1, 200)){
				MessageBox(NULL,L"Please indicate the video path.",L"Hint",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
			WideCharToMultiByte(CP_ACP,0,buf1,-1,videoPath,sizeof(videoPath),NULL,NULL);

			getVideoName();
			if(dirExists(videoFileName))
				showSearchDlg = true;
			else
				startDetectAndBuild = true;	
			EndDialog(hWnd, 0);
		}
		break;

    case IDOK:
    case IDCANCEL:
        EndDialog(hWnd, 0);
        break;
    }
}

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitDialog);

        // Handle the WM_COMMAND message in OnCommand
        HANDLE_MSG (hWnd, WM_COMMAND, OnCommand);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;
    }
    return 0;
}

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR    lpCmdLine,
                      int       nCmdShow)
{
	for(int i=0;i<200;i++)
		videoPath[i] = 0;
	startDetectAndBuild = false;
	startSearch = false;
	showSearchDlg = false;
    g_hInst = hInstance;
	rate = -1;
	numFaces = 0;
	FaceDetector fd;
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
	if(startDetectAndBuild){
		HWND hDlg = CreateDialog(g_hInst, 
            MAKEINTRESOURCE(IDD_DIALOG_WAIT), 
            0, waitDlgProc);
        if (hDlg)
        {
            ShowWindow(hDlg, SW_SHOW);
        }

		fd.jniInit();
		if(!fd.jni_init)
			return -1;
		fd.getVideoRootPath(videoPath);
		fd.detect(g_hInst,hDlg,videoPath,videoFileName,rate,&numFaces);
		fd.buildIndex(hDlg,videoFileName);
		EndDialog(hDlg, 0);

		showSearchDlg = true;
	}
	if(showSearchDlg){
		if(!fd.jni_init){
			fd.jniInit();
			numFaces = countFile(getFacesPath());
		}	
		if(!fd.jni_init)
			return -1;
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_SEARCH), NULL, searchDlgProc);

		if(startSearch){
			fd.searchIndex(queryFile,videoFileName,numSearch);
		}
		fd.jniFree();
	}
	return GetLastError();
}

