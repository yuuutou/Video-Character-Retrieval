#include"myJNI.h"

int JNI::init(){
	options[0].optionString = "-Djava.compiler=NONE";
	options[1].optionString = "-Djava.class.path=.;d:\\;d:\\jnitest\\faceppsdk.jar;d:\\jnitest\\commons-math3-3.2.jar;d:\\jnitest\\JOpenSurf.jar;d:\\jnitest\\lire.jar;d:\\jnitest\\lucene-analyzers-common-4.2.1.jar;d:\\jnitest\\lucene-core-4.2.1.jar;d:\\jnitest\\lucene-queryparser-4.2.1.jar";
	options[2].optionString = "-verbose:NONE";
	vm_args.version = JNI_VERSION_1_4;
    vm_args.nOptions = 3;
    vm_args.options = options;
	vm_args.ignoreUnrecognized = JNI_TRUE;
	hInstance = ::LoadLibrary("C:\\Program Files\\Java\\jdk1.8.0\\jre\\bin\\server\\jvm.dll");
    if (hInstance == NULL)
    {
		cout<<"errors in loading jvm.dll "<<endl;  
        cout<<"Error Code: " << GetLastError();
        return -1;
    }
	PFunCreateJavaVM funCreateJavaVM = (PFunCreateJavaVM)::GetProcAddress(hInstance, "JNI_CreateJavaVM");
	res = (*funCreateJavaVM)(&jvm, (void**)&env, &vm_args);
    if (res < 0)
    {
        return -1;
    }
	return 0;
}

void JNI::SetClass(LPCTSTR str){
	cls = env->FindClass(str);
	obj = env->AllocObject(cls);
}

string  JNI::JStringToCString(jstring str){
	return jStringToCString(env,str);
}

jstring JNI::NewJString(LPCTSTR str){
	return newJString(env,str);
}

void JNI::SelectMethod(LPCTSTR name,LPCTSTR signature){
	mid = env->GetMethodID(cls,name,signature);
}

jobject JNI::CallMethod(jstring arg1,int arg2){
	return env->CallObjectMethod(obj,mid,arg1,arg2);
}

jobject JNI::CallMethod(jstring arg1,jstring arg2,int arg3){
	return env->CallObjectMethod(obj,mid,arg1,arg2,arg3);
}

jobject JNI::CallMethod(jstring arg1,int arg2,int arg3){
	return env->CallObjectMethod(obj,mid,arg1,arg2,arg3);
}

jobject JNI::CallMethod(jstring arg1){
	return env->CallObjectMethod(obj,mid,arg1);
}

jdouble* JNI::GetDoubleArrayElements(jdoubleArray ja,jboolean* jb){
	return env -> GetDoubleArrayElements(ja, jb);
}

jobject JNI::GetObjectArrayElement(jobjectArray joa,int index){
	return env -> GetObjectArrayElement(joa,index);
}

jint JNI::GetArrayLength(jdoubleArray ja){
	return env -> GetArrayLength(ja);
}

jint JNI::GetArrayLength(jobjectArray ja){
	return env -> GetArrayLength(ja);
}

void JNI::ReleaseDoubleArrayElements(jdoubleArray ja,jdouble* srcArrayElems){
	env -> ReleaseDoubleArrayElements(ja, srcArrayElems, JNI_ABORT);
}

void JNI::free(){
	jvm->DestroyJavaVM();
    ::FreeLibrary(hInstance);
}

string JNI::jStringToCString(JNIEnv *env, jstring str)
{
    if(str==NULL)
    {
        return "";
    }
    int len = env->GetStringLength(str);
    char *c_buffer = new char[2*len+1];
    const jchar * jcharString = env->GetStringChars(str, 0);	
    ZeroMemory(c_buffer,(2*len+1)*sizeof(char));
    len = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)jcharString,len,c_buffer,2*len,NULL,NULL);
	env->ReleaseStringChars(str,jcharString);
    string cstr = c_buffer;
    delete[] c_buffer;
    
    return cstr;
}



jstring JNI::newJString(JNIEnv *env, LPCTSTR str)
{
    if(!env || !str)
    {
        return 0;
    }
    int slen = strlen(str);
    jchar* buffer = new jchar[slen];
    int len = MultiByteToWideChar(CP_ACP,0,str,strlen(str),(LPWSTR)buffer,slen);
    if(len>0 && len < slen)
    {
        buffer[len]=0;
    }
    jstring js = env->NewString(buffer,len);
    delete [] buffer;
    return js;
}