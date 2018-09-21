/**
	Quiz3 
	Preston Goulet
	Taylor Greeff
	https://speakerdeck.com/javiergs/ser431-lecture-04
 **/

#include <stdlib.h>
#include <GL/glut.h>
#include <fstream>
#include "mesh.h"
#include "texture.h"
#include "render.h"
#include "controls.h"

#define PI 3.14159

 // global
Mesh *mesh1, *mesh2, *mesh3, *mesh4, *mesh5, *mesh6;
GLuint display3, display5;
GLuint textures[6];

// init
void init() {
	//addMenu();

	// mesh
	mesh3 = createCube(1);
	mesh5 = createSkyBox(6000);

	// normals
	calculateNormalPerFace(mesh3);
	calculateNormalPerFace(mesh5);
	calculateNormalPerVertex(mesh3);
	calculateNormalPerVertex(mesh5);

	// textures
	codedTexture(textures, 0, 0); //Sky texture - noise multiscale. Type=0
	loadBMP_custom(textures, "../BMP_files/cubesky.bmp", 1);

	// display lists
	display3 = meshToDisplayList(mesh3, 3, textures[0]);
	display5 = meshToDisplayList(mesh5, 5, textures[1]);


	// configuration
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	// STENCIL|STEP 2. NEW LINES
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearStencil(0); //define the value to use as clean.

	dot_vertex_floor.push_back(Vec3<GLfloat>(-3000.0, -3000.0, 0.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(3000.0, -3000.0, 0.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(3000.0, 3000.0, 0.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(-3000.0, 3000.0, 0.0));
	calculate_floor_normal(&floor_normal, dot_vertex_floor);

	// light
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	if (isFogOn) {
		glEnable(GL_FOG);
	}
	glFogi(GL_FOG_MODE, GL_LINEAR);
	GLfloat fogColor[4] = { 0.5, 0.5, 0.5, 1.0 };
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.75);
	glFogf(GL_FOG_START, 10.0);
	glFogf(GL_FOG_END, 3000);
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

	// STENCIL-STEP 3. enable and configure
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// light source position
	light_position[0] = 500 * cos(lightAngle) + light_x;
	light_position[1] = light_y;
	light_position[2] = 500 * sin(lightAngle)+ light_z;
	light_position[3] = 0.0; // directional light
	lightAngle = 0.5 * PI;
	
	// Calculate Shadow matrix
	shadowMatrix(shadow_matrix, floor_normal, light_position);

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

	// camera
	gluLookAt(camera_x, camera_y, camera_z, camera_viewing_x, camera_viewing_y, camera_viewing_z, 0.0f, 1.0f, 0.0f);
	glScalef(scale, scale, scale);
	glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
	glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);

	//=====================================
	//		Shadows
	//=====================================
	// draw
	glPushMatrix();
	// Tell GL new light source position
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	// Shadows
	if (renderShadow) {
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	}
	// Draw floor using blending to blend in reflection
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0, 1.0, 1.0, 0.3);
	
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glTranslatef(-3000, -3000, -1000);
	glCallList(display5);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	glDisable(GL_BLEND);
	// Shadows
	
	if (renderShadow) {
		glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
		glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
		//glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); 
		//  To eliminate depth buffer artifacts, use glEnable(GL_POLYGON_OFFSET_FILL);
		// Render 50% black shadow color on top of whatever the floor appareance is
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING);  ///* Force the 50% black.
		glColor4f(0.0, 0.0, 0.0, 0.5);
		glPushMatrix();
		// Project the shadow
		glMultMatrixf((GLfloat *)shadow_matrix);
		// boxes
		glDisable(GL_DEPTH_TEST);

		glTranslatef(box_x, box_y, box_z);
		glCallList(display3);

		glEnable(GL_DEPTH_TEST);
		glPopMatrix();

		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
		// To eliminate depth buffer artifacts, use glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_STENCIL_TEST);
	}

	// Enable Culling
	//glEnable(GL_CULL_FACE);

	// box 3
	glPushMatrix();
	glTranslatef(box_x, box_y, box_z);
	glCallList(display3); //Box under camera
	glPopMatrix();


	drawLightArrow();
	// end
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
	renderBitmapString(0.0, window_height - 13.0f, 0.0f, "Use [Arrows] to move the box");
	renderBitmapString(0.0, window_height - 26.0f, 0.0f, "Use [W, A, S, and D] to move the light");
	renderBitmapString(0.0, window_height - 39.0f, 0.0f, "Use [Right Click] to open the menu");
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	//glDisable(GL_CULL_FACE);
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
		light_y += (10);
		break;
	case 's': case 'S':
		light_y -= (10);
		break;
	case 'a': case 'A':
		light_x -= (10);
		break;
	case 'd': case 'D':
		light_x += (10);
		break;
	}
}

// callback function for arrows
void specialkeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		//total_moving_angle += -0.01;
		//rotate_point(-0.01);

		box_x -= 10;
		//camera_viewing_x -= 10;
	}
	else if (key == GLUT_KEY_RIGHT) {
		//total_moving_angle += 0.01;
		//rotate_point(0.01);
		box_x += 10;
		//camera_viewing_x += 10;
	}
	else if (key == GLUT_KEY_DOWN) {
		//printf("Down key is pressed\n");
		box_y -= 10;
		//camera_viewing_z += 10;
		//camera_x += (-10) * sin(total_moving_angle);//*0.1;
		//camera_z += (-10) * -cos(total_moving_angle);//*0.1;
		//camera_viewing_y -= 10;
		//camera_viewing_x += (-10) * sin(total_moving_angle);//*0.1;
		//camera_viewing_z += (-10) * -cos(total_moving_angle);//*0.1;
	}
	else if (key == GLUT_KEY_UP) {
		//printf("Up key is pressed\n");
		box_y += 10;
		//camera_viewing_z -= 10;
		//camera_x += (10) * sin(total_moving_angle);//*0.1;
		//camera_z += (10) * -cos(total_moving_angle);//*0.1;
		//camera_viewing_x += (10) * sin(total_moving_angle);//*0.1;
		//camera_viewing_z += (10) * -cos(total_moving_angle);//*0.1;
		//camera_viewing_y += 10;
	}
}

// main
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Sky Box");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutSpecialFunc(specialkeys);
	glutKeyboardFunc(callbackKeyboard);

	init();
	glutMainLoop();
	return 0;
}