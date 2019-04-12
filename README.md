# Vizify
## A simple viewer using VTK, ITK and Qt 

Vizify is an application that uses ITK for loading of images, VTK for display of these images and Qt for handling the GUI.

This application was designed with the following goals in mind:
a. Use of Object Oriented Design to build an application from the ground up. 
b. Trying out some of the Design patterns that I have read in the Head First books and realize them in Code
c. General practice of VTK and Qt.
d. Learn some of the ITK modules and features.
e. To have a application available to quickly prototype if needed.


## Development Environment/Build install instructions

This application is tested for use with both Windows and Linux. 
VTK versions used - 5.10, and 6.2. I haven't tested latest versions but I think it should work
ITK - tested for 4.11
Qt - works with 4 as well as 5

## Building Qt (Windows).

Download the latest Qt from the online repository.

Also install the Visual Studio 2017 Express version which can be downloaded for free from the Microsoft website.

After install, open the Native VS Tools prompt for either x86 or x64 as you prefer.

Set the enviroment variables like shown below:

REM Set up \Microsoft Visual Studio 2013, where <arch> is \c amd64, \c x86, etc.
  
CALL "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" <arch>
  
SET _ROOT=C:\qt\qt-5

SET PATH=%_ROOT%\qtbase\bin;%_ROOT%\gnuwin32\bin;%PATH%

REM Uncomment the below line when using a git checkout of the source repository

REM SET PATH=%_ROOT%\qtrepotools\bin;%PATH%&

SET _ROOT=

Also set the Python path in the prompt. The Python version to used is Python 2.7

Set the path to python - Make sure it is a python 2 distribution

### Make a build directory
	mkdir qtbin
	cd qtbin
	
### Use the following configure command
	..\qt-everywhere-opensource-src-5.9.1\configure -debug -nomake examples -opensource -skip qtwebengine -opengl dynamic -prefix .
	
### Build the source
	nmake
	nmake install
	
### Set environment variables

QT_QPA_PLATFORM_PLUGIN_PATH C:\Installations\qt-everywhere-opensource-src-5.9.1\QTBin\plugins\platforms

PATH  C:\Installations\qt-everywhere-opensource-src-5.9.1\QTBin\bin
