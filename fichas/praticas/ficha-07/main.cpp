
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include <IL/il.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

float camX = 0.0f, camY = 0.0f, camZ = 0.0f;	// position P (Y set after terrain loads)
float lookX = 0.0f, lookY = 0.0f, lookZ = 1.0f; // look-at point L
float camAngle = 0.0f;							// horizontal orientation alpha (radians)
const float eyeHeight = 1.0f;
const float moveSpeed = 0.5f;

float teapotAngle = 0.0f;

GLuint buffer, bufferSize;

unsigned char *imageData;
int imageHeight = 0, imageWidth = 0;

float h(int x, int z)
{
	return (imageData[z * imageWidth + x] / 255.0f) * 30.0;
}

float hf(float x, float z)
{
	float px = x + imageWidth / 2.0f;
	float pz = z + imageHeight / 2.0f;

	if (px < 0)
		px = 0;
	if (pz < 0)
		pz = 0;
	if (px > imageWidth - 1)
		px = imageWidth - 1;
	if (pz > imageHeight - 1)
		pz = imageHeight - 1;

	int x1 = (int)floor(px);
	int x2 = x1 + 1;
	int z1 = (int)floor(pz);
	int z2 = z1 + 1;

	if (x2 >= imageWidth)
		x2 = imageWidth - 1;
	if (z2 >= imageHeight)
		z2 = imageHeight - 1;

	float fz = pz - z1;
	float fx = px - x1;

	float h_x1_z = h(x1, z1) * (1 - fz) + h(x1, z2) * fz;
	float h_x2_z = h(x2, z1) * (1 - fz) + h(x2, z2) * fz;

	return h_x1_z * (1 - fx) + h_x2_z * fx;
}

void changeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio
	float ratio = w * 1.0 / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45, ratio, 1, 1000);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void drawTerrain()
{
	// colocar aqui o código de desenho do terreno usando VBOs com TRIANGLE_STRIPS

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.2f, 0.6f, 0.2f);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	unsigned int vertsPerStrip = imageWidth * 2;
	for (unsigned int z = 0; z < imageHeight - 1; z++)
	{
		glDrawArrays(GL_TRIANGLE_STRIP, z * vertsPerStrip, vertsPerStrip);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void drawTree()
{
	// dark green
	glColor3f(0.0f, 0.4f, 0.0f);

	glPushMatrix();

	glRotatef(90, -1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 5.0f);
	glutSolidCone(4.0f, 9.0f, 10, 10);

	glPopMatrix();

	// brown
	glColor3f(0.6f, 0.3f, 0.1f);

	glPushMatrix();

	glRotatef(90, -1.0f, 0.0f, 0.0f);
	glutSolidCone(2.0f, 8.0f, 10, 10);

	glPopMatrix();
}

void drawForest()
{
	int numTrees = 200;
	int seed = 10;
	srand(seed);

	const float clearRadius = 50.0f;
	int placed = 0;

	while (placed < numTrees)
	{
		float x = rand() % 250 - 125;
		float z = rand() % 250 - 125;

		// check if the point lies inside the forbidden circle
		if (x * x + z * z < clearRadius * clearRadius)
			continue;

		glPushMatrix();
		glTranslatef(x, hf(x, z), z);
		drawTree();
		glPopMatrix();
		placed++;
	}
}

void drawOutTeaPots()
{
	glColor3f(1.0f, 0.0f, 0.0f);

	const int count = 20;
	const float circleR = 35.0f;

	glPushMatrix();
	// Base height of the whole ring
	glTranslatef(0, 1.5f, 0);

	for (int i = 0; i < count; ++i)
	{
		float angle = teapotAngle + (2.0f * M_PI * i) / count;
		float degrees = angle * 180.0f / M_PI;

		float posX = circleR * cos(angle);
		float posZ = -circleR * sin(angle);

		float height = hf(posX, posZ);

		glPushMatrix();

		glRotatef(degrees, 0.0f, 1.0f, 0.0f);

		glTranslatef(circleR, height, 0.0f);

		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

		glutSolidTeapot(2.0f);

		glPopMatrix();
	}

	glPopMatrix();
}

void drawInTeaPots()
{
	// blue
	glColor3f(0.0f, 0.0f, 1.0f);

	const int count = 10;
	const float circleR = 15.0f;

	glPushMatrix();
	glTranslatef(0, 2.0f, 0);

	for (int i = 0; i < count; ++i)
	{
		float angle = teapotAngle + (2.0f * M_PI * i) / count;
		float degrees = angle * 180.0f / M_PI;

		glPushMatrix();

		glRotatef(degrees, 0.0f, 1.0f, 0.0f);
		glTranslatef(circleR, 0.0f, 0.0f);
		glutSolidTeapot(2.0f);

		glPopMatrix();
	}

	glPopMatrix();
}

void drawTorus()
{
	// magenta
	glColor3f(1.0f, 0.0f, 1.0f);

	glPushMatrix();

	glTranslatef(0.0f, hf(0.0f, 0.0f) + 1.0f, 0.0f);
	glRotatef(teapotAngle * 180.0f / M_PI, 0.0f, 1.0f, 0.0f);
	glutSolidTorus(1.0f, 5.0f, 20, 20);

	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);
}

void drawScene()
{
	drawForest();

	drawOutTeaPots();

	drawInTeaPots();

	drawTorus();
}

void renderScene(void)
{
	float pos[4] = {-1.0, 1.0, 1.0, 0.0};

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	camY = eyeHeight + hf(camX, camZ);
	lookY = camY;
	gluLookAt(camX, camY, camZ,
			  lookX, lookY, lookZ,
			  0.0f, 1.0f, 0.0f);

	drawTerrain();

	drawScene();

	// advance animation
	teapotAngle += 0.01f;
	if (teapotAngle > 2.0f * M_PI)
		teapotAngle -= 2.0f * M_PI;

	// End of frame
	glutSwapBuffers();
}

void processKeys(unsigned char key, int xx, int yy)
{
	// Slide 3: look direction vector from angle alpha
	// L = (Px + sin(alpha), Py, Pz + cos(alpha))
	float dx = sin(camAngle);
	float dz = cos(camAngle);

	switch (key)
	{
	// Slide 4: forward/backward — move P and L along d = (dx, 0, dz)
	case 'w':
	case 'W':
		camX += moveSpeed * dx;
		camZ += moveSpeed * dz;
		lookX += moveSpeed * dx;
		lookZ += moveSpeed * dz;
		break;
	case 's':
	case 'S':
		camX -= moveSpeed * dx;
		camZ -= moveSpeed * dz;
		lookX -= moveSpeed * dx;
		lookZ -= moveSpeed * dz;
		break;

	// Slide 5: lateral — r = d x up = (dx,0,dz) x (0,1,0) = (dz, 0, -dx)
	case 'a':
	case 'A':
		camX += moveSpeed * dz;
		camZ -= moveSpeed * dx;
		lookX += moveSpeed * dz;
		lookZ -= moveSpeed * dx;
		break;
	case 'd':
	case 'D':
		camX -= moveSpeed * dz;
		camZ += moveSpeed * dx;
		lookX -= moveSpeed * dz;
		lookZ += moveSpeed * dx;
		break;

	// Slide 3: rotate camera left/right by adjusting alpha
	case 'q':
	case 'Q':
		camAngle -= 0.05f;
		lookX = camX + sin(camAngle);
		lookZ = camZ + cos(camAngle);
		break;
	case 'e':
	case 'E':
		camAngle += 0.05f;
		lookX = camX + sin(camAngle);
		lookZ = camZ + cos(camAngle);
		break;
	}

	glutPostRedisplay();
}

void processMouseButtons(int button, int state, int xx, int yy)
{
}

void processMouseMotion(int xx, int yy)
{
}

void init()
{
	// Load the height map "terreno.jpg"
	unsigned int t, tw, th;

	ilGenImages(1, &t);
	ilBindImage(t);
	// terreno.jpg is the image containing our height map
	ilLoadImage((ILstring) "terreno.jpg");
	// convert the image to single channel per pixel
	// with values ranging between 0 and 255
	ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);

	// important: check tw and th values
	// both should be equal to 256
	// if not there was an error loading the image
	// most likely the image could not be found
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);

	imageHeight = th;
	imageWidth = tw;

	printf("[tw = %d, th = %d]\n", tw, th);

	// imageData is a LINEAR array with the pixel values
	imageData = ilGetData();

	// Build the vertex arrays
	std::vector<float> p;
	float step = 1.0f;

	// Iterate over each row of the terrain (each strip connects row z and z+1)
	for (unsigned int z = 0; z < th - 1; z++)
	{
		for (unsigned int x = 0; x < tw; x++)
		{
			float fx = (x - tw / 2.0f) * step;
			float fz = (z - th / 2.0f) * step;

			p.push_back(fx);
			p.push_back(h(x, z));
			p.push_back(fz);

			p.push_back(fx);
			p.push_back(h(x, z + 1));
			p.push_back(fz + step);
		}
	}

	bufferSize = p.size() / 3;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * p.size(), p.data(), GL_STATIC_DRAW);

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);
}

int main(int argc, char **argv)
{
	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI-UM");

	// Required callback registry
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	// init GLEW
#ifndef __APPLE__
	glewInit();
#endif

	ilInit();

	init();

	// enter GLUT's main cycle
	glutMainLoop();

	return 0;
}
