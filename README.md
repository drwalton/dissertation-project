# Dissertation Project: Accompanying DVD #

This DVD is designed to accompany the dissertation report "An Investigation into
the Representation of Fire and Lighting in Interactive Applications", written by
David R. Walton, supervised by Prof. Abhir Bhalerao, 2013.

## Contents ##

This DVD contains the source code and assets used to carry out this dissertation
project, in addition to some precompiled demo application binaries.

## Demos ##

Demo applications are included in the ./bin/ directory. These are .exe files, 
compiled using Visual Studio 2010, and should run on Microsoft Windows PCs
capable of OpenGL 3.2 or higher. It was not possible to support OpenGL 3.1 and 
below due to the use of geometry shaders in the majority of the demos.

Please note that the PRT demos make use of large amounts of graphics memory, and
this may cause problems on machines with more limited graphics hardware. For 
this reason, it is recommended that the following demos should only be run on
machines with 512MB of dedicated GPU memory or more.

In the event that demos do not run (or cannot run at acceptable framerates), 
videos of many of the demos are provided in ./videos .

* fire-lighting-demo (if PRT options are chosen).
* cubemap-sh-demo
* fire-sh-demo

### Controls ###

All demos use the following camera control system. Some demos have specific 
controls, which are described along with the demos.

Camera control is performed via the keyboard. The 'c' key is used to switch
between 'centred' and 'free' camera modes. The initial mode is centred.

#### Centred Mode ####

This mode rotates the camera about the origin. The camera is always oriented to
face towards the origin.

* 'w', 's' Move towards/away from origin.
* 'i', 'j', 'k', 'l' Rotate camera about origin.

#### Free Mode ####

This mode allows the camera to be moved freely, rather than centering about a
particular point.

* 'w', 'a', 's', 'd' Move forwards/backwards or strafe left/right.
* 'i', 'j', 'k', 'l' Rotate camera.

### List of Demos ###

#### sh-plot-demo.exe ####

This allows the user to view a variety of plots of SH functions, chosen from the
console-based menu provided at the start of the application.

#### fire-demo.exe ####

This application was used to benchmark and compare the three fire rendering 
techniques implemented. Choose a rendering method, and number of particles via
the console to start the application. Please note that the fire systems are 
designed to look their best when a few hundred (~400) particles are used. Using 
fewer particle may make the flames look quite 'sparse', and more may result in 
oversaturated, 'crowded'-looking flames.

#### fire-lighting-demo.exe ####

This is the application which was used to benchmark the various fire-lighting
combination methods implemented in the project. Choose a lighting method and 
(where appropriate) a number of lights to use via the console menu to start the
demo.

#### fire-forces-demo.exe ####

This demo is designed to demonstrate the capability of the particle system to 
simulate external forces, and the lighting system to accurately respond to them.
The fire is subjected to a force along the x axis which varies sinusoidally with
time. The bunny is rendered using Blinn-Phong lighting. The 'f' key may be used
to switch the fire rendering mode between 'procedural' and 'scrolling textures'.

#### cubemap-sh-demo.exe ####

This demo displays a bunny illuminated by a fire via the 'cubemap sh' method.
The 'p' key may be used to toggle a small SH plot of the projected cubemap. The
'r' key renders the cubemap used for illumination to six bitmap files, labelled 
'cubemap0.bmp', ... , 'cubemap5.bmp', saved in the current directory. These 
files correspond to the +x, -x, +y, -y, +z, -z faces of the cubemap 
respectively. The 'f' key may be used to switch the fire rendering mode between 
'procedural' and 'scrolling textures'.

#### ao-and-phong-demo.exe ####

This demo may be used to compare Blinn-Phong lighting with bent and surface AO.
The 'm' key may be used to switch between the three modes (see console output to
determine the currently engaged rendering mode). The 'p' key toggles the fire 
rendering method, and the 't', 'f', 'g', 'h' keys may be used to rotate the 
bunny.

#### fire-sh-demo.exe ####

This demo is similar to ao-and-phong-demo, but instead compares shadowed and 
unshadowed PRT, using the SH rotated lights method. 'm' switches between 
shadowed and unshadowed PRT (see console for currently engaged mode). 'p' 
displays an SH plot of the lighting environment generated. 'f' switches the 
particle rendering mode.

## Source ##

The source consists of the following:

* Source for a static library designed to render scenes consisting of lights, 
    models and particle systems. Contained in ./src/ .
* Source for a number of demo applications using the above library. Contained in
    ./demos/ .
* Shader source code used by the applications. Contained in ./shaders/ .

## Assets ##

These consist of a variety of textures and models, contained in the ./textures/ 
and ./models/ directories respectively. All textures were produced by the
author. All models are simplified versions of the 'Stanford Bunny' test model. 
Some are preprocessed files used for PRT or AO.

## Building ##

The CD contains Visual Studio 2010 files and libraries to build fire-framework
and the included demos from source. To do so, it is recommended that the 
contents of the DVD first be copied to local storage. It is not necessary to 
copy the contents of ./videos .

Please then load the ./build/vs2010/fire-framework.sln file in Visual Studio 
2010 or newer (note that if newer versions of Visual Studio are used, conversion
of the project and solution files may be required).

Please note that the application makes use of OpenMP, and for this reason will
(at the time of writing) not compile as-is on Express versions of Visual Studio.

Once the solution file is loaded, it should be possible to build in the usual 
fashion (for example, via Build->Build Solution). For best performance, please 
use the "Release" configuration when building.

The compiled applications will look for shaders, models and textures in the 
directories "../shaders", "../models" and "../textures", relative to their 
current directory. Because of this, it is recommended to make copies of the 
directories ./models, ./shaders and ./textures in ./build/vs2010 .

Note that in order to simplify building from source, this DVD contains libraries
written by a number of other authors. These are included in the ./lib directory, 
and their associated licenses may be found in ./lib/licenses . The author makes 
no claims to ownership of any of the material in this folder.

## Note ##

The dissertation report mentions the use of the boost C++ libraries. These
libraries are considerable in size, and were only used for a single string
replacement during application loading. For this reason, the author has written 
some code to perform this task, and the boost libraries are no longer used. This
function is only used during loading of shader source, and will not affect 
application performance.

## Contact ##

If any problems are encountered, please feel free to contact the author at 
davidwalton3142@gmail.com .
