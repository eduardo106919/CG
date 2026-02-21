#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

float alpha = 0.5f;
float beta = 0.5f;
float radius = 10.0f;

void changeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void drawCylinder(float radius, float height, int slices)
{
	float sliceStep = 2.0f * M_PI / slices;
	float half = height / 2.0f;

	glBegin(GL_TRIANGLES);

	for (int i = 0; i < slices; i++)
	{
		float alpha = i * sliceStep;
		float nextAlpha = (i + 1) * sliceStep;

		float x1 = radius * cos(alpha);
		float z1 = radius * sin(alpha);

		float x2 = radius * cos(nextAlpha);
		float z2 = radius * sin(nextAlpha);

		// top
		glColor3f(0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, half, 0.0f);
		glVertex3f(x2, half, z2);
		glVertex3f(x1, half, z1);

		// bottom
		glColor3f(1.0f, 0.0f, 0.0f);

		glVertex3f(0.0f, -half, 0.0f);
		glVertex3f(x1, -half, z1);
		glVertex3f(x2, -half, z2);

		// sides
		glColor3f(1.0f, 1.0f, 1.0f);

		glVertex3f(x2, -half, z2);
		glVertex3f(x1, -half, z1);
		glVertex3f(x2, half, z2);

		glVertex3f(x2, half, z2);
		glVertex3f(x1, -half, z1);
		glVertex3f(x1, half, z1);
	}

	glEnd();
}

void renderScene(void)
{
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();

	float camX = radius * cos(beta) * sin(alpha);
	float camY = radius * sin(beta);
	float camZ = radius * cos(beta) * cos(alpha);

	gluLookAt(camX, camY, camZ,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);

	glBegin(GL_LINES);

	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);

	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -100.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);

	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -100.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);

	glEnd();
	
	glPolygonMode(GL_FRONT, GL_LINE);

	drawCylinder(1, 2, 10);

	glTranslatef(2, 0, 0);
	drawCylinder(1, 2, 20);

	glPolygonMode(GL_FRONT, GL_FILL);
	glTranslatef(-4, 0, 0);
	drawCylinder(1, 2, 100);

	glPolygonMode(GL_FRONT, GL_LINE);
	glTranslatef(2, 0, 2);
	drawCylinder(1, 3, 10);

	glTranslatef(0, 0, -4);
	drawCylinder(1, 3, 10);

	// End of frame
	glutSwapBuffers();
}

void processKeys(unsigned char c, int xx, int yy)
{
	float zoomStep = 0.5f;

	if (c == 'w')
		radius -= zoomStep;
	else if (c == 's')
		radius += zoomStep;

	if (radius < 1.0f)
		radius = 1.0f;

	glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy)
{
	float angleStep = 0.05f;
	float zoomStep = 0.5f;

	switch (key)
	{
	case GLUT_KEY_LEFT:
		alpha -= angleStep;
		break;

	case GLUT_KEY_RIGHT:
		alpha += angleStep;
		break;

	case GLUT_KEY_UP:
		beta += angleStep;
		if (beta > (M_PI / 2))
			beta = M_PI / 2;
		break;

	case GLUT_KEY_DOWN:
		beta -= angleStep;
		if (beta < - (M_PI / 2))
			beta = - (M_PI / 2);
		break;
	}

	glutPostRedisplay();
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
	glutReshapeFunc(changeSize);

	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
