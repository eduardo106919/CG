#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>

double t_x = 0.0;
double t_y = 0.0;
double t_z = 0.0;
double angle_y = 0.0, angle_x = 0.0, angle_z = 0.0;
double scale_y = 1.0, scale_x = 1.0, scale_z = 1.0;

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

void renderScene(void)
{
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(5.0, 5.0, 5.0,
			  0.0, 0.0, 0.0,
			  0.0f, 1.0f, 0.0f);

	// put axis drawing in here

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

	// put the geometric transformations here
	glTranslatef(t_x, t_y, t_z);
	
	// rotate around the y axis
	glRotatef(angle_y, 0.0f, 1.0f, 0.0f);
	// rotate around the x axis
	glRotatef(angle_x, 1.0f, 0.0f, 0.0f);
	// rotate around the z axis
	glRotatef(angle_z, 0.0f, 0.0f, 1.0f);

	// scale the axis
	glScalef(scale_x, scale_y, scale_z);

	// put pyramid drawing instructions here
	glBegin(GL_TRIANGLES);

	// base

	// white
	glColor3f(1.0f, 1.0f, 1.0f);

	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);

	glVertex3f(-1.0f, 0.0f, -1.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);

	// sides

	// grey
	glColor3f(0.5f, 0.5f, 0.5f);

	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glVertex3f(0.0f, 1.5f, 0.0f);

	// green
	glColor3f(0.0f, 1.0f, 0.0f);

	glVertex3f(-1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.5f, 0.0f);

	// red
	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3f(0.0f, 1.5f, 0.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);

	// blue
	glColor3f(0.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 1.5f, 0.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);

	glEnd();

	// End of frame
	glutSwapBuffers();
}

// write function to process keyboard events

void processSpecialKeys(int key_code, int x, int y)
{
	switch (key_code)
	{
	case GLUT_KEY_LEFT:
		t_z += 0.1;
		break;
	case GLUT_KEY_RIGHT:
		t_z -= 0.1;
		break;
	case GLUT_KEY_UP:
		t_x -= 0.1;
		break;
	case GLUT_KEY_DOWN:
		t_x += 0.1;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void processDefaultKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
		angle_y -= 1;
		break;
	case 'l':
		angle_y += 1;
		break;
	case 'm':
		angle_x += 1;
		break;
	case 'n':
		angle_x -= 1;
		break;
	case 'x':
		angle_z += 1;
		break;
	case 'c':
		angle_z -= 1;
		break;
	case 'u':
		t_y += 0.1;
		break;
	case 'd':
		t_y -= 0.1;
		break;
	case 'e':
		scale_x += 0.1;
		break;
	case 'w':
		scale_x -= 0.1;
		break;
	case 't':
		scale_y += 0.1;
		break;
	case 'y':
		scale_y -= 0.1;
		break;
	case 'i':
		scale_z += 0.1;
		break;
	case 'o':
		scale_z -= 0.1;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(500, 200);
	glutInitWindowSize(1500, 1500);
	glutCreateWindow("CG@DI-UM");

	// Required callback registry
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	// put here the registration of the keyboard callbacks
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processDefaultKeys);

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
