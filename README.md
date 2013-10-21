# Dissertation Project 2013 #
## Fire and Lighting in Interactive Applications ##

This repository contains the source code developed as part of my MSc dissertation project, carried out July-September 2013 at the University of Warwick.

The project report is available for download [here](http://drwalton.github.io/assets/Dissertation/Dissertation.pdf) (PDF, ~8MB).

### Project Topic ###

The aim of this project to investigate ways in which both fire itself, and its effect on the lighting of a scene can be rendered in real-time 3D apps. The programming component involved developing a particle-based fire simulation, and investigating ways of linking it to a variety of lighting models to light an model in a convincing way.

### Source Code ###

The source for this project is organised as follows:

* `/demos/` contains a number of applications demonstrating the fire and lighting simulations.
* `/src/` contains the source code for `fire-framework`, the library developed to construct the demo applications.
* `/shaders/` contains the GLSL shader source files.

### Dependencies ###

This project makes use of the following libraries:

* Assimp: http://assimp.sourceforge.net/
* FreeGLUT: http://freeglut.sourceforge.net/
* GLEW: http://glew.sourceforge.net/
* GLM: http://glm.g-truc.net
* SOIL: http://www.lonesock.net/soil.html
* GLSW: http://prideout.net/blog/?p=11
