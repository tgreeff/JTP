/**
 * SER 431
 * https://speakerdeck.com/javiergs/ser431-lecture-04
 **/

#pragma once

#include <math.h>
#include <GL/glut.h>

int window_width = 800, window_height = 800;
const float window_ratio = 1.0;

// controling parameters
int mouse_button;
int mouse_x = 0;
int mouse_y = 0;
float scale = 0.3;
float x_angle = 0.0;
float y_angle = 0.0;

// camera
float camera_x = 0.0;
float camera_y = 0.0;
float camera_z = 0.0;

float camera_viewing_x = 0.0;
float camera_viewing_y = 0.0;
float camera_viewing_z = -300.0;

float box_x = 0.0;
float box_y = 0.0;
float box_z = 0.0;

float light_x = 0.0;
float light_y = 0.0;
float light_z = -10.0;

float total_moving_angle = 0.0;

// motion
void motion(int x, int y) {
	if (mouse_button == GLUT_LEFT_BUTTON) {
		y_angle += (float(x - mouse_x) / window_width) *360.0;
		x_angle += (float(y - mouse_y) / window_height)*360.0;
	}
	if (mouse_button == GLUT_RIGHT_BUTTON) {
		scale += (y - mouse_y) / 100.0;
		if (scale < 0.1) scale = 0.1;
		if (scale > 7)	scale = 7;
	}
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}

// mouse
void mouse(int button, int state, int x, int y) {
	mouse_x = x;
	mouse_y = y;
	mouse_button = button;
}