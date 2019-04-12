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

REM Set up \Microsoft Visual Studio 2013, where <arch> is \c amd64, \c x86, etc.&nbsp;
CALL "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" <arch>&nbsp;
SET _ROOT=C:\qt\qt-5&nbsp;
SET PATH=%_ROOT%\qtbase\bin;%_ROOT%\gnuwin32\bin;%PATH%&nbsp;
REM Uncomment the below line when using a git checkout of the source repository&nbsp;
REM SET PATH=%_ROOT%\qtrepotools\bin;%PATH%&nbsp;
SET _ROOT=&nbsp;

