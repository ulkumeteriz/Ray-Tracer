#include <iostream>
#include "parser.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "matrix4.hpp"

#define PI 3.14159265359

using namespace parser;
using namespace std;

parser::Scene scene;
static GLFWwindow* win = NULL;

static const float CAMERA_TRANS_SPEED = 0.05f;
// in radian 
static const float ROTATION_ANGLE = 0.5 * ( PI / 180 );
static bool lightState;

static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void setCamera();

// turns on all the lights
void turnOnLights();

// turns off all the lights
void turnOffLights();

// 0-index
void turnOnLight(int lightId);

// 0-index
void turnOffLight(int lightId);

// 1-index
void toggleLight(int lightId);

Matrix4 createTransformationMatrixForRotation(Vector3 rotationAxis, float rotationAngle);

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	// if action is not press, do take no action
	if(action != GLFW_PRESS)
		return;
		
	switch(key)
	{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		
		// keys that change the direction of the camera
		case GLFW_KEY_A:
		{
			// TODO look left, rotate gaze vector 0.5 degrees around up
			
			Matrix4 transformationMatrix = createTransformationMatrixForRotation(scene.camera.up, ROTATION_ANGLE);
			scene.camera.gaze = transformationMatrix * scene.camera.gaze;
			scene.camera.gaze.normalize();
			break;
		}
		case GLFW_KEY_D:
		{
			// TODO look right, rotate gaze vector -0.5 degrees around up
			Matrix4 transformationMatrix = createTransformationMatrixForRotation(scene.camera.up, -1 * ROTATION_ANGLE);
			scene.camera.gaze = transformationMatrix * scene.camera.gaze;
			scene.camera.gaze.normalize();
			
			break;
		}
		case GLFW_KEY_U:
		{
			// TODO look up, rotate gaze vector 0.5 degrees around left vector
			Vector3 leftVector = scene.camera.gaze * scene.camera.up;
			
			Matrix4 transformationMatrix = createTransformationMatrixForRotation(leftVector, ROTATION_ANGLE);
			scene.camera.gaze = transformationMatrix * scene.camera.gaze;
			scene.camera.gaze.normalize();
			
			break;
		}
		case GLFW_KEY_J:
		{
			// TODO look down, rotate gaze vector -0.5 degrees around left vector
			Vector3 leftVector = scene.camera.gaze * scene.camera.up;
			
			Matrix4 transformationMatrix = createTransformationMatrixForRotation(leftVector, -1 * ROTATION_ANGLE);
			scene.camera.gaze = transformationMatrix * scene.camera.gaze;
			scene.camera.gaze.normalize();
			
			break;
		}
		
		// keys that change the position of camera
		case GLFW_KEY_W:
		{
			scene.camera.position.x += scene.camera.gaze.x * CAMERA_TRANS_SPEED;
			scene.camera.position.y += scene.camera.gaze.y * CAMERA_TRANS_SPEED;
			scene.camera.position.z += scene.camera.gaze.z * CAMERA_TRANS_SPEED;
			break;
		}
			
		case GLFW_KEY_S:
		{
			scene.camera.position.x -= scene.camera.gaze.x * CAMERA_TRANS_SPEED;
			scene.camera.position.y -= scene.camera.gaze.y * CAMERA_TRANS_SPEED;
			scene.camera.position.z -= scene.camera.gaze.z * CAMERA_TRANS_SPEED;
			break;
		}
		
		// light
		case GLFW_KEY_0:
			if(lightState)
			{
				turnOffLights();
				lightState = false;
			}
			else
			{
				turnOnLights();
				lightState = true;
			}
				
			break;
		case GLFW_KEY_1:
			toggleLight(1);
			break;
		case GLFW_KEY_2:
			toggleLight(2);
			break;
		case GLFW_KEY_3:
			toggleLight(3);
			break;
		case GLFW_KEY_4:
			toggleLight(4);
			break;
		case GLFW_KEY_5:
			toggleLight(5);
			break;
		case GLFW_KEY_6:
			toggleLight(6);
			break;
		case GLFW_KEY_7:
			toggleLight(7);
			break;
		case GLFW_KEY_8:
			toggleLight(8);
			break;
		case GLFW_KEY_9:
			toggleLight(9);
			break;
	}
	
	// set camera again
	setCamera();
}

Matrix4 createTransformationMatrixForRotation(Vector3 rotationAxis, float rotationAngle)
{
  Vector3 & u = rotationAxis.normalize();
            
  Vector3 v = u.generateDifferentlyDirectedVector();
  v.normalize();
            
  Vector3 w = u * v;
  w.normalize();
            
  // then, correct v
  v = w * u;
  v.normalize();
                    
  float MArray[4][4] = {
    { u.getX(), u.getY(), u.getZ(), 0 },
    { v.getX(), v.getY(), v.getZ(), 0 },
    { w.getX(), w.getY(), w.getZ(), 0 },
    {        0,        0,        0, 1 }
  };    

  float MInverseArray[4][4] = {
    { u.getX(), v.getX(), w.getX(), 0 },
    { u.getY(), v.getY(), w.getY(), 0 },
    { u.getZ(), v.getZ(), w.getZ(), 0 },
    {        0,        0,        0, 1 }
  };

  float cosine = cos(rotationAngle);
  float sine = sin(rotationAngle); 
            
  float rotationArray[4][4] = {
    { 1,      0,      0, 0 },
    { 0, cosine,  -sine, 0 },
    { 0,   sine, cosine, 0 },
    { 0,      0,      0, 1 }
  };      
  
  Matrix4 M(MArray);
  Matrix4 inverseM(MInverseArray);

  Matrix4 transformationMatrix = inverseM * Matrix4(rotationArray) * M;         

  return transformationMatrix;
}

void loadTransformation(const Transformation & transformation)
{
	switch(transformation.type)
	{
		case TRANSLATION:
			glTranslatef(
				scene.translations[transformation.id].x,
				scene.translations[transformation.id].y,
				scene.translations[transformation.id].z
			);
			break;
		case ROTATION:
			glRotatef(
				scene.rotations[transformation.id].x,
				scene.rotations[transformation.id].y,
				scene.rotations[transformation.id].z,
				scene.rotations[transformation.id].w
			);
			break;
		case SCALING:
			glScalef(
				scene.scalings[transformation.id].x,
				scene.scalings[transformation.id].y,
				scene.scalings[transformation.id].z
			);
			break;
	}
	
	return;
}

void toggleLight(int lightId)
{
	// 0-index
	lightId--;

	if(lightId >= (int)scene.point_lights.size())
		return;
	
	PointLight & pLight = scene.point_lights[lightId];
	
	if(pLight.status)
		turnOffLight(lightId);
	else
		turnOnLight(lightId);	
}

// 0-index
void turnOnLight(int lightId)
{
	
	if(lightId >= (int)scene.point_lights.size())
		return;

	PointLight & pLight = scene.point_lights[lightId];
	
	pLight.status = true;
	
	GLfloat ambient[] = {
		scene.ambient_light.x,
		scene.ambient_light.y,
		scene.ambient_light.z
	};
	
	glEnable(GL_LIGHT0 + lightId);
		
		GLfloat intensity[] = {
			pLight.intensity.x,
			pLight.intensity.y,
			pLight.intensity.z,
			1.0f
		};
		
		GLfloat position[] = {
			pLight.position.x,
			pLight.position.y,
			pLight.position.z,
			1.0f
		};
		
		glLightfv(GL_LIGHT0 + lightId, GL_POSITION, position);
		glLightfv(GL_LIGHT0 + lightId, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0 + lightId, GL_DIFFUSE, intensity);
		glLightfv(GL_LIGHT0 + lightId, GL_SPECULAR, intensity);
}

// 0-index
void turnOffLight(int lightId)
{	
	if(lightId >= (int)scene.point_lights.size())
		return;
	
	PointLight & pLight = scene.point_lights[lightId];
	
	pLight.status = false;

	glDisable(GL_LIGHT0 + lightId);
}

void turnOffLights()
{
	lightState = false;
	
	for(int i = 0; i < (int)scene.point_lights.size(); i++)
	{
		turnOffLight(i);
	}
}

void turnOnLights()
{
	lightState = true;
	
	
	for(int i = 0; i < (int)scene.point_lights.size(); i++)
	{
		turnOnLight(i);
	}
}


void loadVertex(int vertexId)
{
	glNormal3f(
		scene.vertex_data[vertexId].normal.getX(),
		scene.vertex_data[vertexId].normal.getY(),
		scene.vertex_data[vertexId].normal.getZ()
	);
			
	glVertex3f(
		scene.vertex_data[vertexId].position.getX(),
		scene.vertex_data[vertexId].position.getY(),
		scene.vertex_data[vertexId].position.getZ()
	);
}

void render() 
{	
	// clear color and buffer bits
		// clearColor indicates background color, initial paint
    glClearColor(
    	scene.background_color.x / 255.0f, 
    	scene.background_color.y / 255.0f, 
    	scene.background_color.z / 255.0f,
    	1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// for all meshes
	for(int meshInd = 0; meshInd < (int)scene.meshes.size(); meshInd++)
	{
		Mesh & mesh = scene.meshes[meshInd];
		
		// load mesh type
		switch(mesh.type)
		{
			case SOLID:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			case WIREFRAME:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
		}
		
		// load material
		Material & material = scene.materials[mesh.material_id];
		
		GLfloat ambColor[4] = { material.ambient.x, material.ambient.y, material.ambient.z, 1.0};
		GLfloat diffColor[4] = { material.diffuse.x, material.diffuse.y, material.diffuse.z, 1.0};
		GLfloat specColor[4] = { material.specular.x, material.specular.y, material.specular.z, 1.0};
		GLfloat specExp[1] = { material.phong_exponent };
		
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambColor);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specColor);
		glMaterialfv(GL_FRONT, GL_SHININESS, specExp);
		
		// turn on lights
		// TODO: more work is going to be done on light on/off
		//turnOnLights();
		
		// load transformations
		glMatrixMode(GL_MODELVIEW);
		
		// push the current MODELVIEW matrix to the stack
		// .. since it carries the view matrix
		glPushMatrix(); 
		
		for(int transInd = (int)mesh.transformations.size() - 1; transInd >= 0 ; transInd--)
		{
			Transformation & transformation = mesh.transformations[transInd];
			loadTransformation(transformation);
		}
	
		// add vertices
		glBegin(GL_TRIANGLES);
		
		for(int faceId = 0; faceId < (int)mesh.faces.size(); faceId++)
		{
			Face & face = mesh.faces[faceId];
			
			loadVertex(face.v0_id);
			loadVertex(face.v1_id);
			loadVertex(face.v2_id);
		}
		
		glEnd();
		
		// pop the saved MODELVIEW matrix, which carries the view matrix
		// .. regarding to the camera
		glPopMatrix();
	}
}

void setCamera()
{
	// viewport
	glViewport(
		0,
		0,
		scene.camera.image_width,
		scene.camera.image_height
	);
	
	// modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// compute U, V, W
	Vector3 vecV(scene.camera.up.x, scene.camera.up.y, scene.camera.up.z);
    vecV.normalize();
    
    Vector3 vecW = -Vector3(scene.camera.gaze.x, scene.camera.gaze.y, scene.camera.gaze.z);
    
    vecW.normalize();
    
    vecW.normalize();
    
    Vector3 vecU = vecV * vecW; // cross product
    vecU.normalize();
   
    vecV = vecW * vecU;
    
    // in case, update the scene's gaze and up vector
    scene.camera.gaze.x = -vecW.getX();
    scene.camera.gaze.y = -vecW.getY();
    scene.camera.gaze.z = -vecW.getZ();
    
    scene.camera.up.x = vecV.getX();
    scene.camera.up.y = vecV.getY();
    scene.camera.up.z = vecV.getZ();

	
	gluLookAt(
		scene.camera.position.x,
		scene.camera.position.y,
		scene.camera.position.z,
		-vecW.getX() + scene.camera.position.x,
		-vecW.getY() + scene.camera.position.y,
		-vecW.getZ() + scene.camera.position.z,
		vecV.getX(),
		vecV.getY(),
		vecV.getZ()
	);
	
	// projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(
		scene.camera.near_plane.x,
		scene.camera.near_plane.y,
		scene.camera.near_plane.z,
		scene.camera.near_plane.w,
		scene.camera.near_distance,
		scene.camera.far_distance
	);
}

int main(int argc, char* argv[])
{
	// load scene file
    scene.loadFromXml(argv[1]);

	// set error callback
    glfwSetErrorCallback(errorCallback);

	// initialize glfw
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

	// create window
    win = glfwCreateWindow(scene.camera.image_width, scene.camera.image_height, "CENG477 HW3: Speedy Gonzales", NULL, NULL);
    
    if (!win)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    // select the windows that objects will be rendered to
    glfwMakeContextCurrent(win);

	// initialize glew
	GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }
    
	// set key callback
    glfwSetKeyCallback(win, keyCallback);

	// set camera
	setCamera();
	
	// shading mode: smooth
	glShadeModel(GL_SMOOTH);
	
	// enable depth_test and lighting
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	
	// initial case: light are turned on
	render();
	turnOnLights();
	
    while(!glfwWindowShouldClose(win))
    {
        glfwWaitEvents();

        render();
        
        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
