//
//modified by:Manvir Bal
//date:8/29/2019
//

#include <iostream>
using namespace std;
#include <climits>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include "perlin.hh"
#include "texture.hh"

//some structures
struct Vec
{
	float x, y, z;
	Vec() : z(0), y(0), x(0){};
	Vec(float x1, float y1, float z1) : x(x1), y(y1), z(z1) {}
	void setPoints(float x1, float y1, float z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}
};
struct Terrain
{
	Vec upper;
	Vec bottom_l;
	Vec bottom_r;
	Vec color;
};

class Global
{
public:
	int xres, yres;
	int n;
	Global();
} g;
class X11_wrapper
{
private:
	Display *dpy;
	Window win;
	GLXContext glc;

public:
	~X11_wrapper();
	X11_wrapper();
	void set_title();
	bool getXPending();
	XEvent getXNextEvent(XEvent *);
	void swapBuffers();
} x11;

//global vars
const int MAX_PARTICLES = 10000;
float ter[400][400];
Vec particles[MAX_PARTICLES];
Vec tri_strip[30000];
int tri_scount = 0;

//Function prototypes
void init_opengl(void);
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void makeParticles();
void makeTerrian();
void render();
void get_Perlin(float);

//=====================================
// MAIN FUNCTION IS HERE
//=====================================

int main()
{

	srand(time(NULL));
	init_opengl();
	//get the values for the perlin noise
	get_Perlin(0.05);
	//Main animation loop
	int done = 0;
	makeParticles();
	//makeTerrian();
	while (!done)
	{
		//Process external events.
		while (x11.getXPending())
		{
			XEvent e;
			x11.getXNextEvent(&e);
			//x11.check_resize(&e);
			check_mouse(&e);
			done = check_keys(&e);
		}
		render();
		x11.swapBuffers();
	}
	return 0;
}

//-----------------------------------------------------------------------------
//Global class functions
//-----------------------------------------------------------------------------
Global::Global()
{
	xres = 800;
	yres = 800;
}

//-----------------------------------------------------------------------------
//X11_wrapper class functions
//-----------------------------------------------------------------------------
X11_wrapper::~X11_wrapper()
{
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

X11_wrapper::X11_wrapper()
{
	GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
	int w = g.xres, h = g.yres;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL)
	{
		cout << "\n\tcannot connect to X server\n"
			 << endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL)
	{
		cout << "\n\tno appropriate visual found\n"
			 << endl;
		exit(EXIT_FAILURE);
	}
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask |
		PointerMotionMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
						InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void X11_wrapper::set_title()
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "3350 Lab1");
}

bool X11_wrapper::getXPending()
{
	//See if there are pending events.
	return (XPending(dpy));
}

XEvent X11_wrapper::getXNextEvent(XEvent *e)
{
	//Get a pending event.
	// XEvent e;
	// XNextEvent(dpy, &e);
	// return e;
	XNextEvent(dpy, e);
}

void X11_wrapper::swapBuffers()
{
	glXSwapBuffers(dpy, win);
}
//-----------------------------------------------------------------------------

void check_mouse(XEvent *e)
{
	static int savex = 0;
	static int savey = 0;

	//Weed out non-mouse events
	if (e->type != ButtonRelease &&
		e->type != ButtonPress &&
		e->type != MotionNotify)
	{
		//This is not a mouse event that we care about.
		return;
	}
	//
	if (e->type == ButtonRelease)
	{
		return;
	}
	if (e->type == ButtonPress)
	{
		if (e->xbutton.button == 1)
		{
			//Left button was pressed.
			int y = g.yres - e->xbutton.y;
			return;
		}
		if (e->xbutton.button == 3)
		{
			//Right button was pressed.
			return;
		}
	}
	if (e->type == MotionNotify)
	{
		//The mouse moved!
		if (savex != e->xbutton.x || savey != e->xbutton.y)
		{
			savex = e->xbutton.x;
			savey = e->xbutton.y;
			//Code placed here will execute whenever the mouse moves.
			int y = g.yres - e->xbutton.y;
			cout << savex << savey << endl;
		}
	}
}
float speedZ = 0.0;
float speedY = 0.0;
float speedX = 0.0;
int check_keys(XEvent *e)
{
	if (e->type != KeyPress && e->type != KeyRelease)
		return 0;
	int key = XLookupKeysym(&e->xkey, 0);
	speedZ = 0;
	speedY = 0;
	speedX = 0;
	if (e->type == KeyPress)
	{
		switch (key)
		{
		case XK_1:
			//Key 1 was pressed
			break;
		case XK_d:
			speedZ -= 0.1;
			break;
		case XK_a:
			speedZ += 0.1;
			break;
		case XK_w:
			speedY += 10;
			break;
		case XK_s:
			speedY -= 10;
			break;
		case XK_j:
			speedX += 10;
			break;
		case XK_l:
			speedX -= 10;
			break;
		case XK_Escape:
			//Escape key was pressed
			return 1;
		}
	}
	return 0;
}
float z = 1;
void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, g.xres, g.yres);
	//Initialize matricesm
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	//glFrustum(0, g.xres, g.yres, 0, z, 100);
	glFrustum(-g.xres / 2, g.xres / 2, -g.yres / 2, g.yres / 2, z, 100);
	//gluPerspective(45.0, g.xres/g.yres, z,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(0,0,0,0,0,-1,0,1,0);
	//Set the screen background color
	//gluLookAt(-0.5,0.05,7,-0.5,0,-7,0,20,0);
	//gluLookAt(0,0,0,)
	glClearColor(0.1, 0.1, 0.1, 1.0);
}
void get_Perlin(float incvar)
{
	float x = 0.1f;
	for (int i = 0; i < 400; i++)
	{
		float y = 0.0f;
		for (int j = 0; j < 400; j++)
		{
			float arr[2] = {y, x};
			ter[i][j] = noise2(arr);
			y += incvar;
		}
		x += incvar;
	}
}
void makeParticles()
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		particles[i].x = ((float)(rand() % g.xres)) - (g.xres / 2);
		particles[i].y = ((float)(rand() % g.yres)) - (g.yres / 2);
		particles[i].z = ((float)rand() / INT_MAX) * -100;
		//printf("z: %f\n", particles[i].z);
	}
}
void makeTerrian()
{
	int offset = 5;
	int midX = 800 / 2;
	int midY = 800 / 2;
	int index = 0;
	for (int z = 0; z < (800 / offset) - 1; z++)
	{
		for (int x = 0; x < 800 / offset; x++)
		{
			tri_strip[index].setPoints(-midX + x * offset,
									   (-midX * ter[x][z]) - 300,
									   -midX + z * offset);
			tri_strip[index + 1].setPoints(-midX + x * offset,
										   (-midX * ter[x][z + 1]) - 300,
										   -midX + (z + 1) * offset);
			index += 2;
		}
	}
}
float PI = atan(1) * 4;
float degree = PI / 3;
int midX = g.xres / 2;
int midY = g.yres / 2;
float rd = ((float)rand() / INT_MAX) * 0.1;
void render()
{

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3ub(255, 255, 255);
	glTranslatef(0, speedY, 0);
	glTranslatef(0, 0, speedZ);
	glTranslatef(speedX,0,0);

	//draw dots
	// glPushMatrix();
	// glPointSize(3);
	// glBegin(GL_POINTS);
	//     for(int i = 0; i < MAX_PARTICLES; i++){
	//         glVertex3f(particles[i].x, particles[i].y, particles[i].z);
	//     }
	// glEnd();
	// glPopMatrix();
//	get_Perlin(0.2);
	glPushMatrix();
	glScalef(3, 1, 0.01);
	int offset = 5;

	for (int z = 0; z < (800 / offset) - 1; z++)
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < 800 / offset; x++)
		{
			if (ter[x][z] >= 0.0)
				glColor3ub(128, 197, 222);
			else
				glColor3ub(10*ter[x][z], 255 * ter[x][z], 10 * ter[x][z]);
			glVertex3f(-midX + x * offset, (-midX * ter[x][z]) - 300, -midX + z * offset);
			glVertex3f(-midX + x * offset, (-midX * ter[x][z + 1]) - 300, -midX + (z + 1) * offset);
		}
		glEnd();
	}
	glPopMatrix();

	// get_Perlin();
	// glPushMatrix();
	// glScalef(3,1,0.01);
	// 	offset = 20;
	// 	glColor3ub(128,197,222);
	// 	for(int z = 0;z < (800/offset)-1; z++){
	// 		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	// 	    glBegin(GL_TRIANGLE_STRIP);
	// 		for(int x = 0; x < 800/offset; x++){

	// 			glVertex3f(-midX+x*offset, (-midX*ter[x][z])-400, -midX+z*offset);
	// 			glVertex3f(-midX+x*offset, (-midX*ter[x][z+1])-400, -midX+(z+1)*offset);
	// 		}
	// 		glEnd();
	// 	}
	// glPopMatrix();

	degree = 0;
}
