#include "windows.h"
#include "jni.h"
#include <string>
#include <iostream>
using namespace std;

class JNI{
public:
	int init();
	jstring NewJString(LPCTSTR str);
	string  JStringToCString (jstring str);	
	void SelectMethod(LPCTSTR name,LPCTSTR signature);
	void SetClass(LPCTSTR str);
	jobject CallMethod(jstring arg1,int arg2);
	jobject CallMethod(jstring arg1,jstring arg2,int arg3);
	jobject CallMethod(jstring arg1,int arg2,int arg3);
	jobject CallMethod(jstring arg1);
	jdouble* GetDoubleArrayElements(jdoubleArray ja,jboolean* jb);
	jobject GetObjectArrayElement(jobjectArray joa,int index);
	jint GetArrayLength(jdoubleArray ja);
	jint GetArrayLength(jobjectArray ja);
	void ReleaseDoubleArrayElements(jdoubleArray ja,jdouble* srcArrayElems);
	void free();
private:
	jstring newJString(JNIEnv *env, LPCTSTR str);
	string  jStringToCString (JNIEnv *env, jstring str);
	typedef jint (WINAPI *PFunCreateJavaVM)(JavaVM **, void **, void *);
    int res;
    JavaVMInitArgs vm_args;
    JavaVMOption options[3];
    JavaVM *jvm;
    JNIEnv *env;
	jclass cls;
	jobject obj;
	jmethodID mid;
	HINSTANCE hInstance;
};