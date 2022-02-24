/*********
This program is based on showing line segment intersection mathematically
 
Author: Sumeyye Kurtulus
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  1000 // Period for the timer.
#define TIMER_ON         0 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define MAX 10
/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

//This program is designed as a state machine in order to manage creation of the points and lines
#define CLEAR 0
#define START 1
#define END 2
#define ADD 3

//initial status of the program
int status = CLEAR;

typedef struct{
	float x, y;
} point_t;

typedef struct{
	point_t start, end;
	float t;
}line_t;

//Declaring a color type of structure in order to manage the color of the additional lines according to intersection 
typedef struct{
	float r, g, b;
}color_t;

color_t yes = { 205.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0 }; //if intersect
color_t no = { 0.0 / 255.0, 153.0 / 255.0, 153.0 / 255.0 }; //if do not intersect

line_t firstLine;
line_t secondLine;
line_t thirdLine;
line_t extra_lines[MAX];

bool setStart = false, setEnd = false, addLine = false;
int numClicks = 0;
int numLines = 0;
bool Sclicked = false, Eclicked = false, extraSclicked = false, extraEclicked = false;


//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, const char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void drawSegment(float x1, float y1, float x2, float y2){
	glLineWidth(5);
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
	glLineWidth(1);
}

void displayBackground(){

	glColor3ub(212, 238, 235);
	glRectf(-winWidth / 2, winHeight / 2, winWidth / 2, -winHeight / 2);
	

	glLineWidth(pow(10.0,-50000.0)); //creating background with thinner lines 

	//Smaller Grid
	glColor3ub(193, 11, 11);
	glBegin(GL_LINES);
	for (int i = -winWidth / 2 / 40 * 40; i <= winWidth / 2 / 40 * 40; i += 40){
		glVertex2f(i, winHeight / 2);
		glVertex2f(i, -winHeight / 2);
	}
	glEnd();

	glBegin(GL_LINES);
	for (int i = winHeight / 2 / 40 * 40; i >= -winHeight / 2 / 40 * 40; i -= 40){
		glVertex2f(-winWidth / 2, i);
		glVertex2f(winWidth / 2, i);
	}
	glEnd();


	//Larger grid
	glColor3ub(21, 87, 80);
	glBegin(GL_LINES);
	for (int i = -winWidth / 2 /80 * 80; i <= winWidth / 2/80*80; i += 80){
		glVertex2f(i, winHeight / 2);
		glVertex2f(i, -winHeight / 2);
	}
	glEnd();

	glBegin(GL_LINES);
	for (int i = winHeight / 2 / 80 * 80; i >= -winHeight / 2 / 80 * 80; i -= 80){
		glVertex2f(-winWidth / 2, i);
		glVertex2f(winWidth / 2, i);
	}
	glEnd();

	glLineWidth(1);

	//Designing the messages at the top
	glColor3ub(21, 87, 80);
	glRectf(-winWidth / 2, winHeight / 2, winWidth / 2, winHeight / 2 - 40);
	glColor3ub(171, 223, 215);
	vprint(-winWidth / 2 + 20, winHeight / 2 - 20, GLUT_BITMAP_9_BY_15, "Sumeyye Kurtulus - 21902787");
	vprint(winWidth / 2 - 200, winHeight / 2 - 20, GLUT_BITMAP_9_BY_15, "#HW2 2020-2021 Spring");

	//Designing the messages at the bottom
	glColor3ub(21, 87, 80);
	glRectf(winWidth / 2, -winHeight / 2, -winWidth / 2, -winHeight / 2 + 40);
	glColor3ub(171, 223, 215);
	vprint(-winWidth / 2 + 20, -winHeight / 2 + 20, GLUT_BITMAP_9_BY_15, "F1: Set Start Position");
	vprint(-winWidth / 2 + 270, -winHeight / 2 + 20, GLUT_BITMAP_9_BY_15, "F2: Set End Position");
	vprint(-winWidth / 2 + 520, -winHeight / 2 + 20, GLUT_BITMAP_9_BY_15, "F3: Add Lines");


}

bool Intersection(line_t l1, line_t l2){

	float x1 = l1.start.x,
		x2 = l1.end.x,
		x3 = l2.start.x,
		x4 = l2.end.x;

	float y1 = l1.start.y,
		y2 = l1.end.y,
		y3 = l2.start.y,
		y4 = l2.end.y;

	float denominator;
	denominator = ((x4 - x3)*(y2 - y1) - (x2 - x1)*(y4 - y3));

	l1.t = ((x4 - x3)*(y3 - y1) - (y4 - y3)*(x3 - x1)) / denominator;
	l2.t = ((x2 - x1)*(y3 - y1) - (y2 - y1)*(x3 - x1)) / denominator;


	if (denominator != 0 /*if they are not parallel*/ 
		&& ((l1.t <= 1 && l1.t >= 0) && (l2.t <= 1 && l2.t >= 0)))
	{
		return true;
	}
	else
		return false;


}

bool ixect;

// start
void manageStart(){
	glColor3ub(205, 180, 191);
	circle(firstLine.start.x, firstLine.start.y, 30);
	glColor3ub(85, 20, 51);
	vprint2(firstLine.start.x - 15, firstLine.start.y - 15, 0.3, "S");
}

//end
void manageEnd(){

	glColor3ub(205, 180, 191);
	circle(firstLine.end.x, firstLine.end.y, 30);
	glColor3ub(85, 20, 51);
	vprint2(firstLine.end.x - 15, firstLine.end.y - 15, 0.3, "E");
	
}


void showLineSegment(int i){

	if (ixect)
		glColor4f(yes.r, yes.g, yes.b, 0.7);
	else
		glColor4f(no.r, no.g, no.b, 0.7);

	glLineWidth(5);
	glBegin(GL_LINES);
	glVertex2f(extra_lines[i].start.x, extra_lines[i].start.y);
	glVertex2f(extra_lines[i].end.x, extra_lines[i].end.y);
	glEnd();
	glLineWidth(1);

}

//
// To display onto window using OpenGL commands
//



void display() {
	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	displayBackground();

	switch (status){
	case CLEAR: displayBackground(); break;
	case START: glColor4f(138.0 / 255, 16.0 / 255, 16.0 / 255, 0.5);
		circle(-winWidth / 2 + 25, -winHeight / 2 + 25, 15); 
		break;
	case END: glColor4f(138. / 255, 16. / 255, 16. / 255, 0.5);
		circle(-winWidth / 2 + 275, -winHeight / 2 + 25, 15); 
		break;
	case ADD: glColor4f(138. / 255, 16. / 255, 16. / 255, 0.5);
		circle(-winWidth / 2 + 525, -winHeight / 2 + 25, 15);
	}
	

	if (setEnd && Eclicked){
		glColor4f(124.0 / 255.0, 96.0 / 255.0, 96.0 / 255.0, 0.7);
		drawSegment(firstLine.start.x, firstLine.start.y, firstLine.end.x, firstLine.end.y);
		manageEnd();
	}

	if (setStart && Sclicked)
		manageStart();

	glColor3f(0, 0, 0);

	if (addLine){

		for (int i = 0; i < MAX; i++){
			if (numClicks > 2 * i){
				circle(extra_lines[i].start.x, extra_lines[i].start.y, 5);
			}
		}

		for (int i = 0; i < MAX; i++){
	
			glColor3f(0, 0, 0);
			ixect = Intersection(firstLine, extra_lines[i]);

			if (numClicks > 2 * i + 1){
				ixect = Intersection(firstLine, extra_lines[i]);
				circle(extra_lines[i].end.x, extra_lines[i].end.y, 5);

				glColor4f(no.r, no.g, no.b, 0.5);
				if (ixect)
					glColor4f(yes.r, yes.g, yes.b, 0.5);

				drawSegment(extra_lines[i].start.x, extra_lines[i].start.y, extra_lines[i].end.x, extra_lines[i].end.y);

			}


		}
		

	}

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	
	if (key == GLUT_KEY_F1){
		setStart = true;
		status = START;
	}

	if (key == GLUT_KEY_F2){
		setEnd = true;
		status = END;
	}
	
	if (key == GLUT_KEY_F3){
		addLine = true;
		status = ADD;
	}


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.


	// to refresh the window it calls display() function
	glutPostRedisplay();
}


//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//


void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.
	
	point_t click = { x - winWidth / 2, winHeight / 2 - y };

	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN){

		if (status == START){
			firstLine.start = click;
			Sclicked = true;
		}

		if (status == END){
			firstLine.end = click;
			Eclicked = true;
		}

		if (status == ADD){

			extraSclicked = true;

			if (numLines < MAX){

				if (numClicks == 2 * numLines){
					numClicks++;
					extra_lines[numLines].start = click;
				}
				else if (numClicks == (2 * numLines + 1)){
					numClicks++;
					extra_lines[numLines].end = click;
					numLines++;
				}

			}

		}


		// to refresh the window it calls display() function
		glutPostRedisplay();
	}
}
//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.

	

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Sumeyye Kurtulus | Assignment No:2");


	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}