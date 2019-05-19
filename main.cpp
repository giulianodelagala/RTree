#define GLUT_DISABLE_ATEXIT_HACK
#include <windows.h>
#include <iostream>
#include <sstream>
#include <math.h>
#include <fstream>
#include <string>
#include <time.h>
#include <chrono>

#include<GL/glut.h>

#include "OcTree.h"

#define KEY_ESC 27

using namespace std::chrono;
using std::cout; using std::string;

int mx = 0;
int my = 0;

float ax = 30.0;
float ay = -10.0;
float speed = .2;
float speed_key = 5;

int max_x = 150, min_x = -150;
int max_y = 150, min_y = -150;
int max_z = 150, min_z = -150;

//Point3d centro = { 0.0,0.0,0.0 };

//float radio = 100;
//float radio_cuad = radio*radio;

OcTree Q(150, max_x, max_y, max_z, min_x, min_y, min_z);

void ReadObjFile(string archivo, OcTree* arbol)
{
	std::ifstream obj_file(archivo);
	string line;
	string temp;
	float x, y, z;

	float tiempo_tomado = 0;
	clock_t start, end;

	if (obj_file.is_open())
	{
		while (!obj_file.eof())
		{
			std::getline(obj_file, line);
			std::stringstream ss(line);
			ss >> temp >> x >> y >> z;
			if (temp == "v")
			{
				// Insertar Point

				start = clock();

				arbol->InsertNode(x * 20, y * 20, z * 20);

				end = clock();
				tiempo_tomado += float(end - start) / float(CLOCKS_PER_SEC);


				//displayPoint(x, y, z);
				//cout << "temp " << temp << " x " << x << " y " << y;
			}
			else
				continue;
		}
	}
	obj_file.close();
	cout << "Tiempo creacion OcTree : " << tiempo_tomado << "sec\n";
}

//dibuja un simple gizmo
void displayGizmo()
{
	glBegin(GL_LINES);
	glColor3d(255, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(60, 0, 0);
	glColor3d(0, 255, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 60, 0);
	glColor3d(0, 0, 255);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 60);
	glEnd();
}

bool r = false;
////////////////////////////
// FUNCION DIBUJAR ESFERA //
// TODO NO ACTUALIZA ///
/*
void displaySphere(Point3d center, float radius)
{
	glColor3ub(80, 76, 76);
	glBegin(GL_POINTS);
	glPushMatrix();
	glTranslatef(centro[0], centro[1], centro[2]);
	glutSolidSphere(radius, 10, 10);
	glPopMatrix();
	glEnd();
	glFlush();
	glutPostRedisplay();
}
*/

void OnMouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mx = x;
		my = y;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		r = true;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		r = false;
	}
}


void OnMouseMotion(int x, int y)
{
	int dx = mx - x;
	int dy = my - y;

	mx = x;
	my = y;

	ax += dx * speed;
	ay += dy * speed;
}

void idle() { // AGREGAR ESTA FUNCION
	glutPostRedisplay();
}

//funcion llamada a cada imagen
void glPaint(void) {

	//El fondo de la escena al color initial
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //CAMBIO
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	//gluPerspective(45.0, 1.0, 1.0, 500.0);

	glOrtho(-250, 250, -250, 250, -250, 350);

	glTranslatef(0, 0, -100.0);
	//glRotatef(10, 0, 0, 1);
	glRotatef(ax, 0, 1, 0);
	glRotatef(ay, 1, 0, 0);

	displayGizmo();

	queue<Node*> cola;
	cola.push(Q.root);
	Q.GenVecRegion(cola);
	//displaySphere(centro, radio);
	glPointSize(1.0f);
	//glTranslatef(0, 0, 0);
	Q.DisplayPuntos();

	// DIBUJAR REGION ///
	glPointSize(1);
	Q.DisplayRegion();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	//Q.DisplaySphere(centro, radio);
	/*
	glColor3ub(80, 76, 76);
	glPushMatrix();
	glTranslatef(centro[0], centro[1], centro[2]);
	glutSolidSphere(radio, 10, 10);
	glPopMatrix();
	//glEnd();
	*/
	//glTranslatef(20, 20, 0);
	//glColor3ub(0, 0, 180);
	//glutSolidCube(400);

	//glFlush();
	//glutPostRedisplay();

	//doble buffer, mantener esta instruccion al fin de la funcion
	glutSwapBuffers();
}

//
//inicializacion de OpenGL
//
void init_GL(void) {
	//Color del fondo de la escena
	glClearColor(222.0f, 222.0f, 222.0f, 0.0f); //(R, G, B, transparencia) en este caso un fondo negro
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_TEXTURE_2D);
	//modo projeccion
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

//en el caso que la ventana cambie de tamaño
GLvoid window_redraw(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}
/*
GLvoid window_key(unsigned char key, int x, int y) {
	switch (key) {
	case KEY_ESC:
		exit(0);
		break;
	case 119: //w
		centro[2] += speed_key;
		break;
	case 115: //s
		centro[2] -= speed_key;
		break;
	default:
		break;
	}
	Q.GenVecInside(centro, radio_cuad);
	glFlush();
	//glutPostRedisplay();
}
*/
/*
void SpecialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		centro[1] +=speed_key;
		break;
	case GLUT_KEY_DOWN:
		centro[1] -=speed_key;
		break;
	case GLUT_KEY_LEFT:
		centro[0] -=speed_key;
		break;
	case GLUT_KEY_RIGHT:
		centro[0] +=speed_key;
		break;
	}
	Q.GenVecInside(centro, radio_cuad);
	glFlush();
	//displaySphere(centro, radio);
	//glutPostRedisplay();
	//cout << centro[0] << centro[1] << centro[2];
}
*/
//
//el programa principal
//
int main(int argc, char** argv) {


	Node** p; Node** q;
	int c;

	float punto[3];
	//VecPoint3d vec_puntos;

	ReadObjFile("airboat50.obj", &Q);


	/*
	srand(time(NULL));
	clock_t s, e;
	float tiempo_tomado = 0;
	for (int i = 0; i < 3000; ++i)
	{
		punto[0] = min_x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_x - min_x)));
		punto[1] = min_x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_x - min_x)));
		punto[2] = min_x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_x - min_x)));
		s = clock();
		Q.InsertNode(punto[0], punto[1], punto[2]);
		e = clock();
		tiempo_tomado += float(e - s) / float(CLOCKS_PER_SEC);
	}
	cout << "Tiempo creacion OcTree : " << tiempo_tomado << "sec\n";
	*/


	//Generacion de Vector de Nodos hoja
	/*
	queue<Node*> cola;
	cola.push(Q.root);
	Q.NodosHoja(cola);

	Q.GenVecInside(centro, radio_cuad);
	*/
	/*
	queue<Node*> cola;
	cola.push(Q.root);
	Q.Impresion(cola, 1);
	*/


	auto start = high_resolution_clock::now();
	if (Q.FindNode(-3.558465, 1.126339, 1.206358, p, q, c))
		cout << "ok\n";
	auto stop = high_resolution_clock::now();
	auto duracion = duration_cast<nanoseconds>(stop - start);
	cout << "Duracion busqueda " << duracion.count() << "nanosec\n";


	//////////////////////////////
	//Inicializacion de la GLUT //
	//////////////////////////////

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("QuadTree"); //titulo de la ventana

	init_GL(); //funcion de inicializacion de OpenGL
	glutDisplayFunc(glPaint);


	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	//glutKeyboardFunc(&window_key);
	//glutSpecialFunc(SpecialInput);
	//glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);

	glutIdleFunc(&idle);
	//glutTimerFunc(0, timer, 0);

	glutMainLoop(); //bucle de rendering
	//no escribir nada abajo de mainloop

	return 0;
}