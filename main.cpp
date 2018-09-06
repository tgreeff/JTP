/**
 * SER 431
 * https://speakerdeck.com/javiergs/ser431-lecture-04
 **/

#include <stdlib.h>
#include <GL/glut.h>
#include <glut.h>
#include <fstream>
#include "mesh.h"
#include "texture.h"
#include "render.h"
#include "controls.h"


// global
Mesh *mesh1, *mesh2, *mesh3, *mesh4, *mesh5, *mesh6;
GLuint display1, display2, display3, display4, display5, display6;
GLuint textures[5];

// init
void init() {
	// mesh
	mesh1 = createPlane(4000, 4000, 400);
	mesh2 = createCube();
	mesh3 = createCube();
	mesh4 = createCube();
	mesh5 = createSkyBox(6000);
	mesh6 = createPlane(6000, 6000, 600);
	
	// normals
	calculateNormalPerFace(mesh1);
	calculateNormalPerFace(mesh2);
	calculateNormalPerFace(mesh3);
	calculateNormalPerFace(mesh4);
	calculateNormalPerFace(mesh5);
	calculateNormalPerFace(mesh6);
	calculateNormalPerVertex(mesh1);
	calculateNormalPerVertex(mesh2);
	calculateNormalPerVertex(mesh3);
	calculateNormalPerVertex(mesh4);
	calculateNormalPerVertex(mesh5);
	calculateNormalPerVertex(mesh6);
	
	// textures
	loadBMP_custom(textures, "_BMP_files/brick.bmp", 0);
	loadBMP_custom(textures, "_BMP_files/oldbox.bmp", 1);
	codedTexture(textures, 2, 0); //Sky texture - noise multiscale. Type=0
	codedTexture(textures, 3, 1); //Marble texture - noise marble. Type=1
	loadBMP_custom(textures, "_BMP_files/cubesky.bmp", 4);
	codedTexture(textures, 5, 2); //Fire texture - noise fire. Type=2

	// display lists
	display1 = meshToDisplayList(mesh1, 1, textures[0]);
	display2 = meshToDisplayList(mesh2, 2, textures[1]);
	display3 = meshToDisplayList(mesh3, 3, textures[2]);
	display4 = meshToDisplayList(mesh4, 4, textures[3]);
	display5 = meshToDisplayList(mesh5, 5, textures[4]);
	display6 = meshToDisplayList(mesh6, 6, textures[5]);

	// configuration
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	
	// light
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	GLfloat light_ambient[]  = { 255.5, 255.5, 255.5, 1.0 };
	GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

// reshape
void reshape(int w, int h) {
	window_width = w;
	window_height = h;
}

// text
void renderBitmapString(float x, float y, float z, const char *string) {
	const char *c;
	glRasterPos3f(x, y, z);   // fonts position
	for (c = string; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
}

// display
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, window_width, window_height);
	gluPerspective(45, window_ratio, 10, 100000);
	// view
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// lookAt
	gluLookAt(camera_x, camera_y, camera_z, camera_viewing_x, camera_viewing_y, camera_viewing_z, 0.0f, 1.0f, 0.0f);
	// camera
	glScalef(scale, scale, scale);
	glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
	glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);

	//plane
	glPushMatrix();
	glTranslatef(-1000, 200, -1000);
	glCallList(display1);
	glPopMatrix();
	// box 1
	glPushMatrix();
	glTranslatef(moveBlock, 200, moveBlock_side);
	glCallList(display2);
	glPopMatrix();
	// box 2
	glPushMatrix();
	glTranslatef(200, 200, 0);
	glCallList(display3);
	glPopMatrix();
	// box 3
	glPushMatrix();
	glTranslatef(-200, 200, 0);
	glCallList(display4);
	glPopMatrix();
	// end
	// skybox
	glPushMatrix();
	glTranslatef(-1500, -2500, -1500);
	glCallList(display5);
	glPopMatrix();
	// end
	glPushMatrix();
	glTranslatef(-1500, 199.9, -1500);
	glCallList(display6);
	glPopMatrix();
	//
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	// texto
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, window_width, 0, window_height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	renderBitmapString(0.0, window_height - 13.0f, 0.0f, "Use [Arrows] to move in plain");
	renderBitmapString(0.0, window_height - 26.0f, 0.0f, "Use [W and S] to look up and down");
	renderBitmapString(0.0, window_height - 39.0f, 0.0f, "Use I, J, K and L to move the box");
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glutSwapBuffers();
}

// rotate what the user see
void rotate_point(float angle) {
	float s = sin(angle);
	float c = cos(angle);
	// translate point back to origin:
	camera_viewing_x -= camera_x;
	camera_viewing_z -= camera_z;
	// rotate point
	float xnew = camera_viewing_x * c - camera_viewing_z * s;
	float znew = camera_viewing_x * s + camera_viewing_z * c;
	// translate point back:
	camera_viewing_x = xnew + camera_x;
	camera_viewing_z = znew + camera_z;
}

// callback function for keyboard (alfanumeric keys)
void callbackKeyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'w': case 'W':
			camera_viewing_y += (10);
			break;
		case 's': case 'S':
			camera_viewing_y -= (10);
			break;
		case 'l': case 'L':
			moveBlock += (10);
			break;
		case 'j': case 'J':
			moveBlock -= (10);
			break;
		case 'k': case 'K':
			moveBlock_side += (10);
			break;
		case 'i': case 'I':
			moveBlock_side -= (10);
			break;
		
	}
}

// callback function for arrows
void specialkeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		total_moving_angle += -0.02;
		rotate_point(-0.02);
	} else if (key == GLUT_KEY_RIGHT) {
		total_moving_angle += 0.02; 
		rotate_point(0.02);
	} else if (key == GLUT_KEY_DOWN) {
		printf("Down key is pressed\n");
		camera_z += 10;
		// X movemment
		if (camera_x <= 1000 && camera_x >= -1000) {
			camera_x += (-10) * sin(total_moving_angle);//*0.1;
			camera_viewing_x += (-10) * sin(total_moving_angle);//*0.1;
		}

		// Z movement
		if (camera_z <= 1000 && camera_z >= -1000) {
			camera_z += (-10) * -cos(total_moving_angle);//*0.1;
			camera_viewing_z += (-10) * -cos(total_moving_angle);//*0.1;
		}	
		camera_viewing_y -= 10;
			
	} else if (key == GLUT_KEY_UP) {
		printf("Up key is pressed\n");
		camera_z -= 10;
		if (camera_x <= 1000 && camera_x >= -1000) {
			camera_x += (10) * sin(total_moving_angle);//*0.1;
			camera_viewing_x += (10) * sin(total_moving_angle);//*0.1;
		}

		if (camera_z <= 1000 && camera_z >= -1000) {
			camera_z += (10) * -cos(total_moving_angle);//*0.1;
			camera_viewing_z += (10) * -cos(total_moving_angle);//*0.1;
		}	
		camera_viewing_y += 10;
	}

	// Camera X verification
	if (camera_x > 1000) {
		camera_x = 1000;
	}
	else if (camera_x < -1000) {
		camera_x = -1000;
	}

	// Camera Z verification
	if (camera_z > 1000) {
		camera_z = 1000;
	}
	else if (camera_z < -1000) {
		camera_z = -1000;
	}
}

// main
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Sky Box");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	
	glutSpecialFunc(specialkeys);
	glutKeyboardFunc(callbackKeyboard);
	init();
	glutMainLoop();
	return 0;
}
