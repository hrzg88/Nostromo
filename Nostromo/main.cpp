//
//  main.cpp
//  RealtimeRending
//
//  Created by Philipp Lensing on 22.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//


#include <iostream>
#include <math.h>

#include "Camera.h"
#include "model.h"
#include "texture.h"
#include "vector.h"
#include "ShaderProgram.hpp"
#include "Spacecraft.h"
#include "Obstacle.h"

// models
const char* g_Object_ship = "models//spaceship_own.obj";

//bitmaps
const char* g_obstacle_height = "Bitmap//courseWidth.bmp";
const char* g_obstacle_width = "Bitmap//courseHeight.bmp";

//shaders
const char* g_Shader_vertex_alpha = "Shader//Vertexshader_red.glsl";
const char* g_Shader_fragment_alpha = "Shader//Pixelshader_red.glsl";
const char* g_Shader_vertex_beta = "Shader//Vertexshader_blue.glsl";
const char* g_Shader_fragment_beta = "Shader//Pixelshader_blue.glsl";

//Pfade für Mac OS X
const char* g_shader_mac_vertex = "/Users/davidherzog/Documents/XCode/Nostromo/Nostromo/Vertexshader_red.glsl";
const char* g_shader_mac_fragment = "/Users/davidherzog/Documents/XCode/Nostromo/Nostromo/Pixelshader_red.glsl";
const char* g_object_ship_mac = "/Users/davidherzog/Documents/XCode/Nostromo/Nostromo/spaceship_own.obj";



// window x and y size
const unsigned int g_WindowWidth=1024;
const unsigned int g_WindowHeight=768;

// positions
const Vector g_LightPos = Vector( 0,5,0);
const Vector g_LocalPos = Vector( 0,0,0);

Camera g_Camera;
Spacecraft g_ship;
Obstacle g_Model_obstacle;

int g_MouseButton = 0;
int g_MouseState = 0;

//movement parameter
int Movement_leftright = 0;
int Movement_updown = 0;

//time for movement
int Movement_elapsedTime;
int Movement_prevTime = 0;
bool buttonPressed_leftRight = false;
bool buttonPressed_upDown = false;


void SetupDefaultGLSettings();
void DrawScene();
void MouseCallback(int Button, int State, int x, int y);
void MouseMoveCallback(int x, int y);
void KeyboardCallback(unsigned char key, int x, int y);
void KeyboardUpCallback(unsigned char key, int x, int y);


int main(int argc, char * argv[]) {

    // initialize the glut system and create a window
    glutInitWindowSize(g_WindowWidth, g_WindowHeight);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("CG Praktikum");
#ifdef WIN32
	glewInit();
#endif

    SetupDefaultGLSettings();
    
    glutDisplayFunc(DrawScene);
    glutMouseFunc(MouseCallback);
    glutKeyboardFunc(KeyboardCallback);
	glutKeyboardUpFunc(KeyboardUpCallback);
    glutMotionFunc(MouseMoveCallback);

	//g_Model.applyMaterial("E://ComputerGrafik//PraktikumProjekte//praktikum_3//Computergrafik//models//sibenik//sibenik.mtl");


    g_ship.load(g_object_ship_mac, g_LocalPos, g_shader_mac_vertex, g_shader_mac_fragment);
	g_ship.setCamera(&g_Camera);

	//g_Model_obstacle.load(g_obstacle_width, g_obstacle_height, g_LocalPos);
    
    glutMainLoop();
}


void SetupDefaultGLSettings() {

    glClearColor(0, 0, 0, 255);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65, (double)g_WindowWidth/(double)g_WindowHeight, 0.045f, 1000.0f);    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void DrawGroundGrid() {

    const float GridSize=10.0f;
    const unsigned int GridSegments=20;
    const float GridStep=GridSize/(float)GridSegments;
    const float GridOrigin=-GridSize*0.5f;
    
    glDisable( GL_LIGHTING);
    glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 1.0f);
        for( unsigned int i=0; i<GridSegments+1; i++) {

            float itpos=GridOrigin + GridStep*(float)i;
            glVertex3f(itpos, GridOrigin, 0);
            glVertex3f(itpos, GridOrigin+GridSize, 0);
        
            glVertex3f( GridOrigin, itpos,0);
            glVertex3f( GridOrigin+GridSize, itpos,0);

        }
    glEnd();
    glEnable( GL_LIGHTING);

}

void MouseCallback(int Button, int State, int x, int y) {

    /*g_MouseButton = Button;
    g_MouseState = State;
    g_Camera.mouseInput(x,y,Button,State);*/
}

void MouseMoveCallback( int x, int y) {

    //g_Camera.mouseInput(x,y,g_MouseButton,g_MouseState);
}

void KeyboardCallback( unsigned char key, int x, int y) {


	switch (key) {
	case 'w':
		Movement_updown = 1;
		buttonPressed_upDown = true;
		break;
	case 'a':
		Movement_leftright = 1;
		buttonPressed_leftRight = true;
		break;
	case 's':
		Movement_updown = -1;
		buttonPressed_upDown = true;
		break;
	case 'd':
		Movement_leftright = -1;
		buttonPressed_leftRight = true;
		break;
	default:
		break;
	}
}

void KeyboardUpCallback(unsigned char key, int x, int y) {

	buttonPressed_leftRight = false;
	buttonPressed_upDown = false;

	Movement_updown = 0;
	Movement_leftright = 0;
}

void DrawScene() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    
    GLfloat lpos[4];
    lpos[0]=g_LightPos.X; lpos[1]=g_LightPos.Y; lpos[2]=g_LightPos.Z; lpos[3]=1;
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);

	Movement_elapsedTime = glutGet(GLUT_ELAPSED_TIME);


	g_Camera.apply();

	g_ship.navigate(Movement_updown, Movement_leftright);
	g_ship.update_Movement(Movement_elapsedTime - Movement_prevTime, buttonPressed_leftRight, buttonPressed_upDown);
	g_ship.update_Camera(Movement_elapsedTime - Movement_prevTime, buttonPressed_leftRight, buttonPressed_upDown);

	

	Movement_prevTime = Movement_elapsedTime;

	DrawGroundGrid();
	g_ship.draw();
	g_Model_obstacle.draw();
    
    glutSwapBuffers();
    glutPostRedisplay();
    
}
