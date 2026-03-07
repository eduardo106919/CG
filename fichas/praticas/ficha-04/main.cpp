#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include <vector>

// 0 = immediate mode
// 1 = VBOs without indices
// 2 = VBOs with indices: duplicates top and bottom central vertices
// 3 = VBOs with indices, greedy version: the top and bottom central vertices appear only once in the array

int mode = 0;

float alfa = 0.0f, beta = 0.0f, radius = 5.0f;
float camX, camY, camZ;

GLuint verticesVBO, verticeCountVBO;
GLuint verticesVBOInd, verticeCountVBOInd;
GLuint verticesVBOIndGreedy, verticeCountVBOIndGreedy;

GLuint indices;
unsigned int indexCount;
GLuint indicesGreedy;
unsigned int indexCountGreedy;

int timebase;
float frames;

void spherical2Cartesian()
{
	camX = radius * cos(beta) * sin(alfa);
	camY = radius * sin(beta);
	camZ = radius * cos(beta) * cos(alfa);
}

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

/*-----------------------------------------------------------------------------------
	Draw Cylinder with strips and fans
	  parameters: radius, height, sides
-----------------------------------------------------------------------------------*/

void cylinder0(float radius, float height, int sides)
{
	int i;
	float step;

	step = 360.0 / sides;

	glColor3f(1, 0, 0);
	glBegin(GL_TRIANGLE_FAN);

	glVertex3f(0, height * 0.5, 0);
	for (i = 0; i <= sides; i++)
	{
		glVertex3f(cos(i * step * M_PI / 180.0) * radius, height * 0.5, -sin(i * step * M_PI / 180.0) * radius);
	}
	glEnd();

	glColor3f(0, 1, 0);
	glBegin(GL_TRIANGLE_FAN);

	glVertex3f(0, -height * 0.5, 0);
	for (i = 0; i <= sides; i++)
	{
		glVertex3f(cos(i * step * M_PI / 180.0) * radius, -height * 0.5, sin(i * step * M_PI / 180.0) * radius);
	}
	glEnd();

	glColor3f(0, 0, 1);
	glBegin(GL_TRIANGLE_STRIP);

	for (i = 0; i <= sides; i++)
	{
		glVertex3f(cos(i * step * M_PI / 180.0) * radius, height * 0.5, -sin(i * step * M_PI / 180.0) * radius);
		glVertex3f(cos(i * step * M_PI / 180.0) * radius, -height * 0.5, -sin(i * step * M_PI / 180.0) * radius);
	}
	glEnd();
}

/*-----------------------------------------------------------------------------------
	Draw Cylinder
		parameters: radius, height, sides
-----------------------------------------------------------------------------------*/

void cylinder(float radius, float height, int sides)
{
	int i;
	float step;

	step = 360.0 / sides;

	glBegin(GL_TRIANGLES);

	// top
	for (i = 0; i < sides; i++)
	{
		glVertex3f(0, height * 0.5, 0);
		glVertex3f(cos(i * step * M_PI / 180.0) * radius, height * 0.5, -sin(i * step * M_PI / 180.0) * radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0) * radius, height * 0.5, -sin((i + 1) * step * M_PI / 180.0) * radius);
	}

	// bottom
	for (i = 0; i < sides; i++)
	{
		glVertex3f(0, -height * 0.5, 0);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0) * radius, -height * 0.5, -sin((i + 1) * step * M_PI / 180.0) * radius);
		glVertex3f(cos(i * step * M_PI / 180.0) * radius, -height * 0.5, -sin(i * step * M_PI / 180.0) * radius);
	}

	// body
	for (i = 0; i <= sides; i++)
	{
		glVertex3f(cos(i * step * M_PI / 180.0) * radius, height * 0.5, -sin(i * step * M_PI / 180.0) * radius);
		glVertex3f(cos(i * step * M_PI / 180.0) * radius, -height * 0.5, -sin(i * step * M_PI / 180.0) * radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0) * radius, height * 0.5, -sin((i + 1) * step * M_PI / 180.0) * radius);

		glVertex3f(cos(i * step * M_PI / 180.0) * radius, -height * 0.5, -sin(i * step * M_PI / 180.0) * radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0) * radius, -height * 0.5, -sin((i + 1) * step * M_PI / 180.0) * radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0) * radius, height * 0.5, -sin((i + 1) * step * M_PI / 180.0) * radius);
	}
	glEnd();
}

void cylinderVBO(float radius, float height, int sides)
{
	// criar um vector com as coordenadas dos pontos
	std::vector<float> p;

	int i;
	float step;

	step = 360.0 / sides;

	// top
	for (i = 0; i < sides; i++)
	{
		p.push_back(0);
		p.push_back(height * 0.5);
		p.push_back(0);

		p.push_back(cos(i * step * M_PI / 180.0) * radius);
		p.push_back(height * 0.5);
		p.push_back(-sin(i * step * M_PI / 180.0) * radius);

		p.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
		p.push_back(height * 0.5);
		p.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);
	}

	// bottom
	for (i = 0; i < sides; i++)
	{
		p.push_back(0);
		p.push_back(-height * 0.5);
		p.push_back(0);

		p.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
		p.push_back(-height * 0.5);
		p.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);

		p.push_back(cos(i * step * M_PI / 180.0) * radius);
		p.push_back(-height * 0.5);
		p.push_back(-sin(i * step * M_PI / 180.0) * radius);
	}

	// body
	for (i = 0; i <= sides; i++)
	{
		p.push_back(cos(i * step * M_PI / 180.0) * radius);
		p.push_back(height * 0.5);
		p.push_back(-sin(i * step * M_PI / 180.0) * radius);

		p.push_back(cos(i * step * M_PI / 180.0) * radius);
		p.push_back(-height * 0.5);
		p.push_back(-sin(i * step * M_PI / 180.0) * radius);

		p.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
		p.push_back(height * 0.5);
		p.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);

		p.push_back(cos(i * step * M_PI / 180.0) * radius);
		p.push_back(-height * 0.5);
		p.push_back(-sin(i * step * M_PI / 180.0) * radius);

		p.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
		p.push_back(-height * 0.5);
		p.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);

		p.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
		p.push_back(height * 0.5);
		p.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);
	}

	verticeCountVBO = p.size() / 3;

	// criar o VBO
	glGenBuffers(1, &verticesVBO);

	// copiar o vector para a memória gráfica
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(
		GL_ARRAY_BUFFER,		  // tipo do buffer, só é relevante na altura do desenho
		sizeof(float) * p.size(), // tamanho do vector em bytes
		p.data(),				  // os dados do array associado ao vector
		GL_STATIC_DRAW);		  // indicativo da utilização (estático e para desenho)
}

void cylinderVBOIndices(float radius, float height, int sides)
{
	std::vector<unsigned int> ind;
	std::vector<float> p;
	float halfH = height * 0.5f;
	float step = 2.0f * M_PI / sides;

	p.push_back(0.0f);
	p.push_back(halfH);
	p.push_back(0.0f);

	p.push_back(0.0f);
	p.push_back(-halfH);
	p.push_back(0.0f);

	for (int i = 0; i < sides; i++)
	{
		float x = cos(i * step) * radius;
		float z = -sin(i * step) * radius;

		p.push_back(x);
		p.push_back(halfH);
		p.push_back(z);
		p.push_back(x);
		p.push_back(-halfH);
		p.push_back(z);
	}

	for (int i = 0; i < sides; i++)
	{
		int next = (i + 1) % sides;

		int topCurr = 2 + (i * 2);
		int topNext = 2 + (next * 2);
		int botCurr = 3 + (i * 2);
		int botNext = 3 + (next * 2);

		ind.push_back(topNext);
		ind.push_back(0);
		ind.push_back(topCurr);

		ind.push_back(botCurr);
		ind.push_back(1);
		ind.push_back(botNext);

		ind.push_back(topNext);
		ind.push_back(topCurr);
		ind.push_back(botCurr);

		ind.push_back(topNext);
		ind.push_back(botCurr);
		ind.push_back(botNext);
	}

	verticeCountVBOInd = p.size() / 3;

	// criar o VBO
	glGenBuffers(1, &verticesVBOInd);

	// copiar o vector para a memória gráfica
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBOInd);
	glBufferData(
		GL_ARRAY_BUFFER,		  // tipo do buffer, só é relevante na altura do desenho
		sizeof(float) * p.size(), // tamanho do vector em bytes
		p.data(),				  // os dados do array associado ao vector
		GL_STATIC_DRAW);		  // indicativo da utilização (estático e para desenho)

	indexCount = ind.size();
	// criar o VBO com indices
	glGenBuffers(1, &indices);

	// copiar o vector para a memória gráfica
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 sizeof(unsigned int) * ind.size(),
				 ind.data(),
				 GL_STATIC_DRAW);
}

void cylinderVBOIndicesGreedy(float radius, float height, int sides)
{
	std::vector<unsigned int> ind;
	std::vector<float> p;
	float halfH = height * 0.5f;
	float step = 2.0f * M_PI / sides;

	p.push_back(0.0f);
	p.push_back(halfH);
	p.push_back(0.0f);
	int topCenterIdx = 0;

	int topCircleStart = p.size() / 3;
	for (int i = 0; i <= sides; i++)
	{
		p.push_back(cos(i * step) * radius);
		p.push_back(halfH);
		p.push_back(-sin(i * step) * radius);
	}

	int bodyStart = p.size() / 3;
	for (int i = 0; i <= sides; i++)
	{
		float x = cos(i * step) * radius;
		float z = -sin(i * step) * radius;
		// Top edge of body
		p.push_back(x);
		p.push_back(halfH);
		p.push_back(z);
		// Bottom edge of body
		p.push_back(x);
		p.push_back(-halfH);
		p.push_back(z);
	}

	int botCircleStart = p.size() / 3;
	for (int i = 0; i <= sides; i++)
	{
		p.push_back(cos(i * step) * radius);
		p.push_back(-halfH);
		p.push_back(-sin(i * step) * radius);
	}

	int botCenterIdx = p.size() / 3;
	p.push_back(0.0f);
	p.push_back(-halfH);
	p.push_back(0.0f);

	for (int i = 0; i < sides; i++)
	{
		// Top Cap
		ind.push_back(topCenterIdx);
		ind.push_back(topCircleStart + i);
		ind.push_back(topCircleStart + i + 1);

		// Bottom Cap
		ind.push_back(botCenterIdx);
		ind.push_back(botCircleStart + i + 1);
		ind.push_back(botCircleStart + i);

		// Body (Two triangles)
		int b1 = bodyStart + (i * 2);	  // Top left
		int b2 = bodyStart + (i * 2) + 1; // Bottom left
		int b3 = bodyStart + (i * 2) + 2; // Top right
		int b4 = bodyStart + (i * 2) + 3; // Bottom right

		ind.push_back(b1);
		ind.push_back(b2);
		ind.push_back(b3);
		
		ind.push_back(b2);
		ind.push_back(b4);
		ind.push_back(b3);
	}

	verticeCountVBOIndGreedy = p.size() / 3;

	// criar o VBO
	glGenBuffers(1, &verticesVBOIndGreedy);

	// copiar o vector para a memória gráfica
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBOIndGreedy);
	glBufferData(
		GL_ARRAY_BUFFER,		  // tipo do buffer, só é relevante na altura do desenho
		sizeof(float) * p.size(), // tamanho do vector em bytes
		p.data(),				  // os dados do array associado ao vector
		GL_STATIC_DRAW);		  // indicativo da utilização (estático e para desenho)

	indexCountGreedy = ind.size();
	// criar o VBO com indices
	glGenBuffers(1, &indicesGreedy);

	// copiar o vector para a memória gráfica
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesGreedy);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 sizeof(unsigned int) * ind.size(),
				 ind.data(),
				 GL_STATIC_DRAW);
}

void renderScene(void)
{
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
			  0.0, 0.0, 0.0,
			  0.0f, 1.0f, 0.0f);

	switch (mode)
	{
	case 0:
		cylinder(1, 2, 10);
		break;
	case 1:
		// just VBOs
		glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, verticeCountVBO);
		break;
	case 2:
		// VBOs with indices
		glBindBuffer(GL_ARRAY_BUFFER, verticesVBOInd);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		glDrawElements(GL_TRIANGLES,
					   indexCount,		// número de índices a desenhar
					   GL_UNSIGNED_INT, // tipo de dados dos índices
					   0);				// parâmetro não utilizado
		break;
	case 3:
		// VBOs with indices (greedy version)
		glBindBuffer(GL_ARRAY_BUFFER, verticesVBOIndGreedy);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesGreedy);
		glDrawElements(GL_TRIANGLES,
					   indexCountGreedy, // número de índices a desenhar
					   GL_UNSIGNED_INT,	 // tipo de dados dos índices
					   0);				 // parâmetro não utilizado
		break;
	}

	frames++;
	int time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000)
	{
		float fps = frames * 1000.0 / (time - timebase);
		timebase = time;
		frames = 0;

		char title[100];
		sprintf(title, "mode: %d | fps: %.2f", mode, fps);
		glutSetWindowTitle(title);
	}

	// End of frame
	glutSwapBuffers();
}

void processKeys(unsigned char c, int xx, int yy)
{
	// put code to process regular keys in here
}

void processSpecialKeys(int key, int xx, int yy)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		alfa -= 0.1;
		break;

	case GLUT_KEY_LEFT:
		alfa += 0.1;
		break;

	case GLUT_KEY_UP:
		beta += 0.1f;
		if (beta > 1.5f)
			beta = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		beta -= 0.1f;
		if (beta < -1.5f)
			beta = -1.5f;
		break;

	case GLUT_KEY_PAGE_DOWN:
		radius -= 0.1f;
		if (radius < 0.1f)
			radius = 0.1f;
		break;

	case GLUT_KEY_PAGE_UP:
		radius += 0.1f;
		break;
	case GLUT_KEY_F1:
		mode = 0;
		break;
	case GLUT_KEY_F2:
		mode = 1;
		break;
	case GLUT_KEY_F3:
		mode = 2;
		break;
	case GLUT_KEY_F4:
		mode = 3;
		break;
	}
	spherical2Cartesian();
	glutPostRedisplay();
}

void printInfo()
{
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	printf("\nUse Arrows to move the camera up/down and left/right\n");
	printf("Page Up and Page Down control the distance from the camera to the origin\n");
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
	glutSpecialFunc(processSpecialKeys);

	// init GLEW
#ifndef __APPLE__
	glewInit();
#endif

	glEnableClientState(GL_VERTEX_ARRAY);

	timebase = glutGet(GLUT_ELAPSED_TIME);

	cylinderVBO(1, 2, 400);
	cylinderVBOIndices(1, 2, 400);
	cylinderVBOIndicesGreedy(1, 2, 400);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_LINE);

	spherical2Cartesian();

	printInfo();

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
