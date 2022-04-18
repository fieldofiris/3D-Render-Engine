//Code by Igor Popov
//3D Render Engine v1.0
#include <iostream>
#include <fstream>
#include <strstream>
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "VectorMatrix.h"

int main()
{	
	//Setup SFML RenderWindow

	//enable smoothing
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	//set the size of the window
	float fScreenHeight = 600.0f;
	float fScreenWidth = 800.0f;

	//create window
	sf::RenderWindow window(sf::VideoMode((unsigned int)fScreenWidth, (unsigned int)fScreenHeight), "Test", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	//Initialize object
	mesh meshObj;
	meshObj.LoadFromObjectFile("teapot.obj");

	//Create Camera & initialize its parameters
	vector3D vCamera;
	//Position
	vCamera.x = 0.0f;
	vCamera.y = 0.0f;
	vCamera.z = 0.0f;
	vector3D LookDir;
	float fYaw = 0.0f;
	float fPitch = 0.0f;

	//Create Projection Matrix
	mat4x4 matProj;
	matProj = matProj.mProject(90.0f, fScreenHeight / fScreenWidth, 0.1f, 1000.0f);
	mat4x4 matRotZ, matRotX;

	//Create Clock
	float fTheta = 0.0f;
	sf::Clock clock;
	float elapsed = clock.getElapsedTime().asSeconds();

	while (window.isOpen()) {
		sf::Event event;
		//time between frames
		float elapsed = clock.restart().asSeconds();

		//Translation Matrix
		mat4x4 matTrans;
		matTrans = matTrans.mTranslate(0.0f, 0.0f, 8.0f);

		//World Matrix
		mat4x4 matWorld;
		matWorld = matWorld.mIdentity();
		matWorld = matWorld * matTrans;

		//Vectors that allow us to implement camera movement
		vector3D vOrthDir = { 1, 0, 0 };
		vector3D vLookDir = { 0, 0, 1 };
		vector3D vUp = { 0, 1, 0 };
		vector3D vTarget = { 0, 0, 1 };
		vector3D vTargetPerpendicular = { 1, 0, 0 };
		mat4x4 matCameraRotYaw = matCameraRotYaw.mRotateY(fYaw);//Rotate the Camera around Y-axis
		mat4x4 matCameraRotPitch = matCameraRotPitch.mRotateX(fPitch); //Rotate the Camera around X-axis
		//Create Camera Rotation Matrix
		mat4x4 matCameraRot = matCameraRotYaw * matCameraRotPitch;
		
		//Calculate Direction of Camera
		vLookDir = vectorMatrixProduct(vTarget,matCameraRot);
		vOrthDir = vectorMatrixProduct(vTargetPerpendicular,matCameraRot);
		vTarget = vCamera + vLookDir;
		vTargetPerpendicular = vCamera + vOrthDir;

		//Arrows move the camera relative to Absolute X and Y axis
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			vCamera.y += 8.0f * elapsed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			vCamera.y -= 8.0f * elapsed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			vCamera.x += 8.0f * elapsed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			vCamera.x -= 8.0f * elapsed;
		}

		//WASD moves the camera relative to camera X and Z axis
		vector3D vForward = vLookDir * (8.0f * elapsed);
		vector3D vRight = vOrthDir * (8.0f * elapsed);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			vCamera = vCamera + vForward;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			vCamera = vCamera - vForward;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			vCamera = vCamera + vRight;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			vCamera = vCamera - vRight;
		}
		
		//Mouse Camera Control Implementation
		sf::Vector2i localPosition = sf::Mouse::getPosition(window);
		if (localPosition.x > 0 && localPosition.x < fScreenWidth && localPosition.y > 0 && localPosition.y < fScreenHeight) {
			fYaw = 2 * asin((localPosition.x - fScreenWidth/2) / (fScreenWidth / 2));
			fPitch = 2 * asin((localPosition.y - fScreenHeight/2) / (fScreenHeight / 2));
		}

		mat4x4 matCamera = matCamera.mPointAt(vCamera, vTarget, vUp);

		//Matrix CameraView
		mat4x4 matView = matView.mQuickInverse(matCamera);

		while (window.pollEvent(event)) {
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
			// resize feature is buggy if initial ratio is not conserved
			if (event.type == sf::Event::Resized) {
				fScreenHeight = (float)event.size.height;
				fScreenWidth = (float)event.size.width;
				matProj = matProj.mProject(90.0f, fScreenHeight / fScreenWidth, 0.1f, 1000.0f);
			}
		}
		//clean frame
		window.clear(sf::Color::White);
		
		//Vector of Triangles
		//We are using this vector to order the projections
		std::vector<triangle> vecTrianglesToRaster;

		//Transform and project triangles
		for (auto tri : meshObj.tris) {
			triangle triTransformed;
			triangle triProjected;
			triangle triViewed;

			triTransformed.p[0] = vectorMatrixProduct(tri.p[0],matWorld);
			triTransformed.p[1] = vectorMatrixProduct(tri.p[1],matWorld);
			triTransformed.p[2] = vectorMatrixProduct(tri.p[2],matWorld);

			vector3D normal;
			vector3D line1;
			vector3D line2;

			line1 = triTransformed.p[1] - triTransformed.p[0];
			line2 = triTransformed.p[2] - triTransformed.p[0];

			normal = line1.vCrossProduct(line2);
			normal = normal.vNormalise();

			vector3D vCameraRay = triTransformed.p[0] - vCamera;

			if (normal.vDotProduct(vCameraRay) < 0.0f) {
				//lighting
				vector3D light_direction = { 0.0f, 0.5f, -1.0f };
				light_direction = light_direction.vNormalise();

				// How similar is normal to light direction
				float dp = std::max(0.1f, light_direction.vDotProduct(normal));

				//Initialize colors of the model and change the intenisty based on lighting info
				int iRcomp = 150;
				int iGcomp = 0;
				int iBcomp = 150;
				triViewed.clr.r = (sf::Uint8)(iRcomp * dp);
				triViewed.clr.g = (sf::Uint8)(iGcomp * dp);
				triViewed.clr.b = (sf::Uint8)(iBcomp * dp);
				//Colors cannot be over 255
				if (iRcomp * dp > 255)
					triViewed.clr.r = 255;
				if (iGcomp * dp > 255)
					triViewed.clr.g = 255;
				if (iBcomp * dp > 255)
					triViewed.clr.b = 255;
				triViewed.clr.a = tri.clr.a;

				triViewed.p[0] = vectorMatrixProduct(triTransformed.p[0],matView);
				triViewed.p[1] = vectorMatrixProduct(triTransformed.p[1],matView);
				triViewed.p[2] = vectorMatrixProduct(triTransformed.p[2],matView);


				//Clip Triangles against the Screen of the Camera
				int nClippedTriangles = 0;
				triangle clipped[2];
				nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

				for (int n = 0; n < nClippedTriangles; n++) {

					//Project from 3D -> 2D
					triProjected.p[0] = vectorMatrixProduct(clipped[n].p[0],matProj);
					triProjected.p[1] = vectorMatrixProduct(clipped[n].p[1],matProj);
					triProjected.p[2] = vectorMatrixProduct(clipped[n].p[2],matProj);
					triProjected.clr = clipped[n].clr;

					triProjected.p[0] = triProjected.p[0]/triProjected.p[0].w;
					triProjected.p[1] = triProjected.p[1]/triProjected.p[1].w;
					triProjected.p[2] = triProjected.p[2]/triProjected.p[2].w;

					vector3D vOffsetView = { 1.0f, 1.0f, 0.0f };

					triProjected.p[0] = triProjected.p[0] + vOffsetView;
					triProjected.p[1] = triProjected.p[1] + vOffsetView;
					triProjected.p[2] = triProjected.p[2] + vOffsetView;
					triProjected.p[0].x *= 0.5f * fScreenWidth;
					triProjected.p[0].y *= 0.5f * fScreenHeight;
					triProjected.p[1].x *= 0.5f * fScreenWidth;
					triProjected.p[1].y *= 0.5f * fScreenHeight;
					triProjected.p[2].x *= 0.5f * fScreenWidth;
					triProjected.p[2].y *= 0.5f * fScreenHeight;

					vecTrianglesToRaster.push_back(triProjected);
				}
			}
		}

		//Sort Based on which triangle is closer to the screen
		sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2) {
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
			return z1 > z2;
			});

		//Clip the Triangles Against the 4 Sides of the CameraView
		for (auto& triToRaster : vecTrianglesToRaster) {

			triangle clipped[2];
			std::list<triangle> listTriangles;
			listTriangles.push_back(triToRaster);
			int nNewTriangles = 1;
			//For every edge of the screen we apply a clipping algorithm
			for (int p = 0; p < 4; p++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					//Take triangle from front of queue
					triangle test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					//Clip the traingle against 4 sides of the Screen
					switch (p)
					{
					case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, fScreenHeight - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ fScreenWidth - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					}

					//Add new triangles to the queue, since we might have to clip them against other sides of the Screen
					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(clipped[w]);
				}
				nNewTriangles = listTriangles.size();
			}

			//Create a Temporary Polygon for Drawing
			sf::VertexArray triPoly(sf::Triangles, 3);
			for (auto& t : listTriangles)
			{
				triPoly[0].position = sf::Vector2f(t.p[0].x, t.p[0].y);
				triPoly[1].position = sf::Vector2f(t.p[1].x, t.p[1].y);
				triPoly[2].position = sf::Vector2f(t.p[2].x, t.p[2].y);

				triPoly[0].color = t.clr;
				triPoly[1].color = t.clr;
				triPoly[2].color = t.clr;

				window.draw(triPoly);
			}

		}
		window.display();
	}
	return 0;
}