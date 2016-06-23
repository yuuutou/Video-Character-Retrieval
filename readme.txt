This program integrates face detection function in Face++, index building and searching techniques 
in Lire and video reading function in OpenCV. The program is coded in both c++ and Java. JNI technique 
is used to call Java classes from c++ codes.

This program can only run on Windows 64bits systems. It is tested on both win7 64bits system and win8 
system.To compile and run the program, you need to:

1.Import the whole c++ project in your visual Studio. Set the "Solution Configuration" to "Release x64".Then in 
"Property Manager", make sure that "OpenCV_Release_x64" is added in "Release|x64".

2.In the project "property pages", the value of "General->Platform Toolset" should be v110 or newer. If the
toolset version is v100 or earlier, please download and setup "Windows7.1SDK" and select "Windows7.1SDK" as 
the value of the "Platform Toolset".

3.Download OpenCV. This program used OpenCV 2.4.9. If you want to use another version of OpenCV
please modify the property page of the c++ project. Change the value of "Linker->Input->Additional Dependencies"
to adapt for your version. You should make this modification in "Property Manager".

4.Please extract your OpenCV to "D:\OpenCV". Otherwise you have to modify the property page of the c++ project.
Change the value of "VC++ Directories->Include Directories" and "VC++ Directories->Library Directories" to 
adapt for your version. You should make this modification in "Property Manager".

5.Add value "D:\OpenCV\opencv\build\x64\vc11\bin" to the environment variable "path". Reboot your computer to 
make it work.

6.Download a 64bits JDK. This program used jdk1.7.0_40.If you wish to use another version of JDK, please
modify the property page of the c++ project. Change the value of "VC++ Directories->Include Directories" and 
"VC++ Directories->Library Directories" to adapt for your version. This modification should be done just
in the property pages of the project (not in the property manager). Additionally, you need to modify the relevant
code in "myJNI.cpp". 

7.Please extract the JDK to the path "C:\Program Files\Java\" and put the folder "jnitest" in the path "D:\", 
otherwise you need to modify the relevant code in "myJNI.cpp".

8.If you choose to use another version of JDK, you have to re-compile the Java files in the folder "jnitest"
using the "javac" program in the JDK of your own version. 

9.Set the environment variable "JAVA_HOME" to "C:\Program Files\Java\jdk1.7.0_40" (or your own version of JDK).
Reboot your computer to make it work.

Now you can compile and run the program. If you have any problems or concerns about the configuration, please
do not hesitate to contact: zhoujy1123@gmail.com