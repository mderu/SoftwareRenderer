// SoftwareRenderer.cpp : Defines the entry point for the console application.
//
// Mark De Ruyter
// Fall 2014 
// For Fun #1
//
////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "math.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <limits>
#include <fstream>
#include <stdlib.h>
#include <GL/glut.h>
#include <string>
#include <sstream>

#include "Vector3F.h"
#include "Vertex3F.h"
#include "Matrix4F.h"
#include "Util.h"
#include "Mesh.h"
#include "Light.h"

//End Includes

using std::vector;
using std::string;

//Window/Veiw Data
float distMove = 10;
float distMoveInit = 10;
float distMoveIncr = 1;



//Polygon Data
Mesh* curMesh = 0;
unsigned curMeshCounter = 0;
//Input
char lastKeyPress = 'w';
int mButtonPressed = -1;
enum Keys{ 
	NONE = 0,
	KEYS_W = 1 << 0, 
	KEYS_A = 1 << 1,
	KEYS_S = 1 << 2,
	KEYS_D = 1 << 3,
	KEYS_Q = 1 << 4,
	KEYS_E = 1 << 5
};
Keys keysPressed = NONE;

int fmx = -1, fmy = -1;

void GL_render()
{
	glClear(GL_COLOR_BUFFER_BIT); 
	for (int x = 0; x < WINDOW_WIDTH; x++)
	{
		for (int y = 0; y < WINDOW_HEIGHT; y++)
		{
			z_buf[x][y] = float_max;
		}
	}
	glBegin(GL_POINTS);
	for (unsigned q = 0; q < sceneMeshes.size(); q++)
	{
		sceneMeshes[q]->drawMesh();
	}
	

	glEnd();

	glutSwapBuffers();
}


// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0)
{
	// ...
	// Complete this function
	// ...
}

void rightMouseButton(int button, int state, int x, int y, bool fromMotion)
{
	if (!fromMotion && state == GLUT_DOWN)
	{
		fmx = x;
		fmy = y;
		return;
	}

	eyeRot.x += (y - fmy) / 2.0f;
	eyeRot.y -= (x - fmx) / 2.0f;

	for (unsigned i = 0; i < sceneMeshes.size(); i++)
	{
		sceneMeshes[i]->calculateVerts();
	}

	if (!fromMotion && state == GLUT_UP)
	{
		fmx = -1;
		fmy = -1;
	}
	else
	{
		fmx = x;
		fmy = y;
	}
	glutPostRedisplay();
}

void leftMouseButton(int button, int state, int x, int y, bool fromMotion)
{
	if (!fromMotion && state == GLUT_DOWN)
	{
		fmx = x;
		fmy = y;
		return;
	}

	if (lastKeyPress == 'w')
	{
		curMesh->p.x += x - fmx;
		curMesh->p.y += y - fmy;
		curMesh->calculateVerts();
		fmx = x;
		fmy = y;
	}
	else if (lastKeyPress == 'e')
	{
		curMesh->r.x += (y - fmy) / 2;
		curMesh->r.y -= (x - fmx) / 2;
		
		curMesh->calculateVerts();

		fmx = x;
		fmy = y;
	}
	else if (lastKeyPress == 'r')
	{
		curMesh->s *= (curMesh->p.x - x) * (curMesh->p.x - x) / WINDOW_WIDTH;
		curMesh->s /= (curMesh->p.x - fmx) * (curMesh->p.x - fmx) / WINDOW_WIDTH;

		curMesh->calculateVerts();
	}
	if (!fromMotion && state == GLUT_UP)
	{
		fmx = 0;
		fmy = 0;
	}
	glutPostRedisplay();
}

//All of the need mouse information gets funneled into this function.
//NOTE: 'button' and 'state' will be -1 when 'fromMotion' is true.
//Before doing anything with button and state, check to make sure
//'fromMotion' is false.
void allMouseFunction(int button, int state, int x, int y, bool fromMotion)
{
	//Fix the upside down thing.
	y = WINDOW_HEIGHT - y;

	//Transition of pseudo state machine
	//Place this at the bottom so the state == GLUT_UP conditions can be reached
	//(mButtonPressed needs to be true when state == GLUT_UP to do the finalization
	// of the transformations).
	//-1 is not a GLUT value.
	if (!fromMotion)
	{
		mButtonPressed = button;
	}

	if (mButtonPressed == GLUT_RIGHT_BUTTON)
	{
		rightMouseButton(button, state, x, y, fromMotion);
	}
	else if (mButtonPressed == GLUT_LEFT_BUTTON)
	{
		leftMouseButton(button, state, x, y, fromMotion);
	}

	if (!fromMotion && state == GLUT_UP)
	{
		mButtonPressed = -1;
		//Clears any lingering keys that were used 
		//together with mouse input. This way the 
		//user won't accidently begin translating the
		//model after moving the viewport.
		keysPressed = NONE;
	}
}

void mouseHandler(int button, int state, int x, int y)
{
	allMouseFunction(button, state, x, y, false);
}

void mouseMotionHandler(int x, int y)
{
	//-1's assigned for no reason. They shouldn't matter.
	allMouseFunction(-1, -1, x, y, true);
}

void setKey(Keys& k, Keys n, bool setOn)
{
	//In case you haven't noticed, I really like the ternary operator
	k = static_cast<Keys>(setOn ? (k | n) : (k & ~n));
}

void keyboardHandler(unsigned char key, int x, int y, bool keyDown)
{
	y = WINDOW_HEIGHT - y;

	if (mButtonPressed == GLUT_RIGHT_BUTTON)
	{
		switch (key)
		{
		case 'w':
			setKey(keysPressed, KEYS_W, keyDown); break;
		case 'a':
			setKey(keysPressed, KEYS_A, keyDown); break;
		case 's':
			setKey(keysPressed, KEYS_S, keyDown); break;
		case 'd':
			setKey(keysPressed, KEYS_D, keyDown); break;
		case 'q':
			setKey(keysPressed, KEYS_Q, keyDown); break;
		case 'e':
			setKey(keysPressed, KEYS_E, keyDown); break;
		}
	}

}

void keyboardUp(unsigned char key, int x, int y)
{
	keyboardHandler(key, x, y, false);
}
void keyboardDown(unsigned char key, int x, int y)
{
	keyboardHandler(key, x, y, true);
	if (key == 'q' && mButtonPressed != GLUT_RIGHT_BUTTON)
	{
		wireFrame = !wireFrame;
		glutPostRedisplay();
	}
	else if (key == 'x' && mButtonPressed != GLUT_RIGHT_BUTTON)
	{
		renderPhong = !renderPhong;
		glutPostRedisplay();
	}
	else if (key == 'z' && mButtonPressed != GLUT_RIGHT_BUTTON)
	{
		smoothTriangles = !smoothTriangles;
		glutPostRedisplay();
	}
	else if (key == 'c' && mButtonPressed != GLUT_RIGHT_BUTTON)
	{
		barryPhong = !barryPhong;
		glutPostRedisplay();
	}
	else if (key == '0' && mButtonPressed != GLUT_RIGHT_BUTTON)
	{
		curMesh = sceneMeshes[(++curMeshCounter % sceneMeshes.size())];
		glutPostRedisplay();
	}
	else if (mButtonPressed == -1)
	{
		lastKeyPress = key;
	}
}

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	// ...
	// Complete this function
	// ...
	glutCreateWindow("Smooth Renderering Test 1 - Mark De Ruyter");

	// The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
	// For the purposes of this lab, this is set to the number of pixels
	// in each dimension.
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	glutDisplayFunc(GL_render);

	glutPostRedisplay();
}
//Note: Objs must have vertex points and their normals in the same order.
//Having them out of order is completely counterintuitive to smooth shading
//and realism. If they are out of order: oh well.

//This program also expects vertex points to be before vertex normals.
//It will not accept it in any other way.

//Our renderer also does not deal with texture coordinates or w values.
//For now, at least.
Mesh* processFile(char* filename)
{
	std::ifstream is; is.open(filename, std::ifstream::in);

	if (!is) {std::cout << "File " << filename << " not found.\n"; exit(1);}

	char line[256];

	Mesh* m = new Mesh;
	Mesh* retMe = new Mesh;
	vector<float> u;
	vector<float> v;

	unsigned vcounter = 0;
	unsigned vncounter = 0;
	unsigned fcounter = 0;

	while (is.getline(line, 256))
	{
		std::stringstream ss(line);

		std::string token; ss >> token;
		if (token.size() == 0){ continue; }
		if (token[0] == 'v')
		{
			//code is "v"
			if (token.size() == 1)
			{
				float x, y, z; ss >> x; ss >> y; ss >> z;
				//std::cout << "Vertex added. Point3f( " << x << ", " << y << ", " << z << " ).\n";
				m->oVertArray.push_back(
					Vertex3F(
						(Vector3F(x, y, z)),
						Vector3F(Vector3F::up),
						Vector3F(
							1 - static_cast <float> (rand()) / static_cast <float> (RAND_MAX)/8.0f,
							1 - static_cast <float> (rand()) / static_cast <float> (RAND_MAX)/8.0f,
							1 - static_cast <float> (rand()) / static_cast <float> (RAND_MAX)/8.0f
						)
				));
				retMe->oVertArray.push_back(m->oVertArray[vcounter]);
				retMe->vertArray.push_back(m->oVertArray[vcounter]);
				vcounter++;
				std::cout << vcounter << std::endl;
			}
			//code is "vn"
			else if (token.size() == 2 && token[1] == 'n')
			{
				float x, y, z; ss >> x; ss >> y; ss >> z;
				if (m->oVertArray.size() <= vncounter)
				{
					std::cerr << filename << " not loaded, it looks like you have your\n"
						<< "vertex normals before your vertexes. You should fix that.\n";
					exit(1);
				}


				m->oVertArray[vncounter].n = Vector3F(x, y, z); 
				vncounter++;
			}
			else if (token.size() == 2 && token[1] == 't')
			{
				float x, y; ss >> x; ss >> y;
				u.push_back(x);
				v.push_back(y);
			}
		}
		//code is "f"
		else if (token[0] == 'f')
		{
			Face face;
			for (unsigned i = 0; i < 4; i++)
			{
				ss >> token;
				//std::cerr << "Token: " << token << std::endl;
				unsigned delimAt = token.find("/");
				std::string strVert = token.substr(0, delimAt);
				token = token.substr(token.find("/")+1);
				delimAt = token.find("/");
				std::string strTex = token.substr(0, delimAt);

				//token is now the normal
				token = token.substr(delimAt+1);
				unsigned vertAt = stoi(strVert) - 1;
				if (strVert.size() == 0){ break; }

				if (vertAt < m->oVertArray.size())
				{
					if (i == 3)
					{
						face.vert2 = face.vert3;
						face.u2 = face.u3;
						face.v2 = face.v3;
					}
					retMe->vertArray[vertAt].n = retMe->oVertArray[vertAt].n = m->oVertArray[stoi(token) - 1].n;
					if (i == 0)
					{
						face.vert1 = &retMe->vertArray[vertAt];
						face.u1 = u[stoi(strTex) - 1];
						face.v1 = u[stoi(strTex) - 1];
					}
					else if (i == 1)
					{
						face.vert2 = &retMe->vertArray[vertAt];
						face.u2 = u[stoi(strTex) - 1];
						face.v2 = u[stoi(strTex) - 1];
					}
					else
					{
						face.vert3 = &retMe->vertArray[vertAt];
						face.u3 = u[stoi(strTex) - 1];
						face.v3 = u[stoi(strTex) - 1];
					}
					if (i >= 2)
					{
						retMe->faces.push_back(face);
						fcounter++;
					}
					
				}
				else
				{
					std::cerr << filename << " not loaded, it seems that " << filename 
						<< "has a face with an out of bounds vertex." << std::endl;
					std::cerr << strVert << std::endl;
				}
			}

		}

	}
	
	delete m;

	float largestSqrDist = 0;
	for (unsigned i = 0; i < retMe->oVertArray.size(); i++)
	{
		float dist = retMe->oVertArray[i].p.sqrMagnitude();
		if (dist > largestSqrDist)
		{
			largestSqrDist = dist;
		}
	}
	largestSqrDist = sqrt(largestSqrDist);
	for (unsigned i = 0; i < retMe->oVertArray.size(); i++)
	{
		retMe->oVertArray[i].p /= largestSqrDist;
	}

	retMe->setScale(Vector3F(300, 300, 300));
	
	return retMe;
}

void update()
{
	if (mButtonPressed == GLUT_RIGHT_BUTTON && keysPressed)
	{
		if (keysPressed & KEYS_W)
		{
			eyePos += moveAlong(eyeRot, Vector3F::forward * distMove);
		}
		else if (keysPressed & KEYS_A)
		{
			eyePos += moveAlong(eyeRot, Vector3F::left * distMove);
		}
		else if (keysPressed & KEYS_S)
		{
			eyePos += moveAlong(eyeRot, Vector3F::back * distMove);
		}
		else if (keysPressed & KEYS_D)
		{
			eyePos += moveAlong(eyeRot, Vector3F::right * distMove);
		}
		else if (keysPressed & KEYS_Q)
		{
			eyePos += moveAlong(eyeRot, Vector3F::up * distMove);
		}
		else if (keysPressed & KEYS_E)
		{
			eyePos += moveAlong(eyeRot, Vector3F::down * distMove);
		}
		
		for (unsigned i = 0; i < sceneMeshes.size(); i++)
		{
			sceneMeshes[i]->calculateVerts();
		}

		//TODO: Make the lights work when moved. It's causing problems 
		//That I don't want to deal with right now.
		Matrix4F transform = Matrix4F::projection() * camMatrix();
		for (unsigned i = 0; i < sceneLights.size(); i++)
		{
			sceneLights[i]->origin = transform * sceneLights[i]->oOrigin;
			sceneLights[i]->direction = transform * sceneLights[i]->oDirection - transform.getTranslation();
		}

		distMove += distMoveIncr;
		glutPostRedisplay();
	}
	else{
		distMove = distMoveInit;
	}
}


int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "Please input the name of the file you wish to render.\n";
		exit(1);
	}
	for (int i = 1; i < argc; i++)
	{
		sceneMeshes.push_back(processFile(argv[i]));
		sceneMeshes[i-1]->calculateVerts();
	}
	curMesh = sceneMeshes[0];
	//curMesh->p.z = 800;

	Light* ambientLight = new Light();
	ambientLight->type = Light::LIGHT_POINT;
	ambientLight->angle = 20.0f;
	ambientLight->color = Vector3F(1.0f, 1.0f, 1.0f);
	ambientLight->intensity = 1;
	ambientLight->origin = ambientLight->oOrigin = Vector3F(400, 400, -400);
	ambientLight->direction = ambientLight->oDirection = Vector3F(0, 0, 1).normalize();
	ambientLight->range = 15000;

	sceneLights.push_back(ambientLight);

	Matrix4F transform = Matrix4F::projection() * camMatrix();
	for (unsigned i = 0; i < sceneLights.size(); i++)
	{
		sceneLights[i]->origin = transform * sceneLights[i]->oOrigin;
		sceneLights[i]->direction = transform * sceneLights[i]->oDirection - transform.getTranslation();
	}

	/*ambientLight = new Light();
	ambientLight->type = Light::LIGHT_DIRECTIONAL;
	ambientLight->angle = 20.0f;
	ambientLight->color = Vector3F(1, 1, 1);
	ambientLight->intensity = 1;
	ambientLight->origin = Vector3F(400, 400, 1000);
	ambientLight->direction = Vector3F(0, 1, -1).normalize();
	ambientLight->range = 15001;

	sceneLights.push_back(ambientLight);*/

	GLInit(&argc, argv);
	glutMouseFunc(mouseHandler);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	glutIdleFunc(update);

	glutMainLoop();

	return 0;
}

