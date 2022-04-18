# SFML-based 3D Render Engine by Igor Popov
This is my implementation of a simple 3D Render Engine using SFML library tools.
You can move around the space using WASD and Mouse.
In order for the render engine to process Objects (.obj files) they should contain only vertex information and their faces should be triangulated.
To change:
	* Object - in the main.cpp change name of the object on line 33
	* Colour - in the main.cpp change the RGB components on lines 182-184
	* Lighting - in the main.cpp change the lighting vector on line 175

Included files:
	* README
	* 3DRenderEngine.exe - Program Executable
	* main.cpp - Source code of the Engine
	* VectorMatrix.h - Utility Functions for Vectors and Matrix
	* sphere.obj - test object
	* teapot.obj - test object
Instructions:
	* Running:
		1. Download the .exe file and the .obj into the same folder and run (Program displays object with name "teapot.obj" by default)
	* Editing:
 		1. Download the SFML Library https://www.sfml-dev.org/download/sfml/2.5.1/
		2. Follow the tutorial to link the library (depends on your OS and IDE) https://www.sfml-dev.org/tutorials/2.5/
		3. Open the code using your IDE
Notes:
	* Resizing the Window without conserving the ratio may yield buggy behaviour.
	* Arrows move the camera in along absolute X and Z axis

References & Acknowledgements:
	The idea of the project and the implementation of a clipping algorithm are based on tutorial by Javidx9 (OneLoneCoder) 3D Console Engine Tutorial https://www.youtube.com/watch?v=HXSuNxpCzdM&list=PLrOv9FMX8xJE8NgepZR1etrsU63fDDGxO&index=24
	
