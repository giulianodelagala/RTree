#pragma once

#pragma once

#include <vector>
#include <array>
#include <queue>
#include <algorithm>


using std::vector; using std::queue;
using std::array; using std::cout;

typedef array<float, 3> Point3d;
typedef vector<Point3d> VecPoint3d;

/*
#define nodo0 (*nodo_padre)->vec_nodos[0]
#define nodo1 (*nodo_padre)->vec_nodos[1]
#define nodo2 (*nodo_padre)->vec_nodos[2]
#define nodo3 (*nodo_padre)->vec_nodos[3]

#define nodo4 (*nodo_padre)->vec_nodos[4]
#define nodo5 (*nodo_padre)->vec_nodos[5]
#define nodo6 (*nodo_padre)->vec_nodos[6]
#define nodo7 (*nodo_padre)->vec_nodos[7]
*/

class Node;
class OcTree;


struct Node
{
	bool isDivided = false;
	float x_min, x_max, y_min, y_max, z_max, z_min;
	Node* vec_nodos[8]; //Vector de Puntero a Nodos hijo 
							   // 0 : NE(arriba izquierda), 1: NO, 2: SE, 3:SO
	VecPoint3d vec_puntos; //Cambiar a 3 cuando sea OcTree

	Node::Node(int max_puntos)
	{
		for (int i = 0; i < 8; ++i)
			vec_nodos[i] = nullptr;
		vec_puntos.resize(max_puntos);
	}
};

class OcTree
{
public:
	int max_puntos;
	Node* root;
	float qx_max, qy_max, qz_max,
		qx_min, qy_min, qz_min;
	VecPoint3d todo_puntos;
	vector <Node*> vec_regiones;
	vector <Point3d> vec_inside;
	vector <Node*> vec_hojas;

	void OcTree::Impresion(queue<Node*> cola, int contador)
	{
		if (!cola.empty())
		{

			Node* v = cola.front();
			cola.pop();
			if (v != nullptr)
			{
				for (int i = 0; i < 8; ++i)
					cola.push(v->vec_nodos[i]);

				if (v->isDivided)
				{
					cout << "[*]";
				}
				else
				{
					cout << "[";
					for (int i = 0; i < v->vec_puntos.size(); ++i)
						cout << "(" << v->vec_puntos[i][0] << "," << v->vec_puntos[i][1] << "," << v->vec_puntos[i][2] << ")";
					cout << "]";
				}
			}
			else
				cout << "[N]";
			Impresion(cola, contador++);
		}
	}

	void GenVecRegion(queue<Node*> cola)
	{
		if (!cola.empty())
		{

			Node* v = cola.front();
			cola.pop();
			if (v != nullptr)
			{
				for (int i = 0; i < 8; ++i)
					cola.push(v->vec_nodos[i]);

				if (v->isDivided)
				{
					//Es nodo padre sirve para dibujar regiones
					//displayRegion(v);
					vec_regiones.push_back(v);
				}
			}
			GenVecRegion(cola);
		}
	}

	void DisplayPuntos()
	{
		glColor3ub(82, 95, 212);
		for (int i = 0; i < todo_puntos.size(); ++i)
		{
			displayPoint(todo_puntos[i], 2);
		}
	}

	void DisplayRegion()
	{
		for (auto i = vec_regiones.begin(); i != vec_regiones.end(); ++i)
			displayRegion(*i);
	}

	bool OcTree::FindNode(float x, float y, float z, Node * *&p, Node * *&nodo_padre, int& cuadrante)
	{
		p = &root;
		//nodo_padre = p;

		while (*p)
		{
			bool xmid = x <= ((*p)->x_max + (*p)->x_min) / 2;
			bool ymid = y >= ((*p)->y_max + (*p)->y_min) / 2;

			nodo_padre = p;
			if ((*p)->isDivided)
			{
				//Cara Frontal
				if (z <= ((*p)->z_max + (*p)->z_min) / 2)
				{
					//Cero y Segundo Cuadrante
					if (xmid) //izquierdo
					{
						if (ymid) //arriba
						{
							p = &((*p)->vec_nodos[0]); //NE(arriba izquierda)
							cuadrante = 0;
							continue;
						}
						else //abajo
						{
							p = &((*p)->vec_nodos[2]); //SE
							cuadrante = 2;
							continue;
						}
					}
					//Primero y Tercer cuadrante
					else // derecho
					{
						if (ymid) //arriba
						{
							p = &((*p)->vec_nodos[1]); //NE(arriba derecha)
							cuadrante = 1;
							continue;
						}
						else //abajo
						{
							p = &((*p)->vec_nodos[3]); //SO
							cuadrante = 3;
							continue;
						}
					}
				}
				else //Cara Posterior
				{
					//Cuarto y Sexto Cuadrante Posterior
					if (xmid) //izquierdo
					{
						if (ymid) //arriba
						{
							p = &((*p)->vec_nodos[4]); //NE(arriba izquierda)
							cuadrante = 4;
							continue;
						}
						else //abajo
						{
							p = &((*p)->vec_nodos[6]); //SE
							cuadrante = 6;
							continue;
						}
					}
					//Quinto y Setimo cuadrante Posterior
					else
					{
						if (ymid)
						{
							p = &((*p)->vec_nodos[5]); //NE(arriba derecha)
							cuadrante = 5;
							continue;
						}
						else
						{
							p = &((*p)->vec_nodos[7]); //SO
							cuadrante = 7;
							continue;
						}
					}
				}

			}
			else //No esta dividido por lo tanto contiene puntos
			{
				for (auto i = (*p)->vec_puntos.begin(); i != (*p)->vec_puntos.end(); ++i)
				{
					if (i->at(0) == x && i->at(1) == y && i->at(2) == z)
						return true;
					//else
					//	return false;
				}
				return false;
			}
		}
		return false;
	}

	bool OcTree::InsertNode(float x, float y, float z) //modificar a x,y,z en OcTree
	{
		Node** p;
		Node** nodo_padre;
		Point3d punto = { x,y,z };
		int cuadrante;
		if (FindNode(x, y, z, p, nodo_padre, cuadrante)) return false;
		todo_puntos.push_back(punto); //Agregar a vector total de puntos
		if (*p && !(*p)->isDivided)
		{
			(*p)->vec_puntos.push_back(punto);
			if ((*p)->vec_puntos.size() > max_puntos)
				SplitNode(p);
			else
				return true;
		}
		return false;
	}

	void OcTree::NodosHoja(queue<Node*> cola)
	{
		if (!cola.empty())
		{

			Node* v = cola.front();
			cola.pop();
			if (v != nullptr)
			{
				for (int i = 0; i < 8; ++i)
					cola.push(v->vec_nodos[i]);

				if (!v->isDivided)
				{
					//Es nodo hoja contiene puntos
					vec_hojas.push_back(v);
				}
			}
			NodosHoja(cola);
		}
	}

	void DisplaySphere(Point3d & center, float radius)
	{
		//displaySphere(center, radius);
		glColor3ub(255, 51, 51);
		for (int i = 0; i < vec_inside.size(); ++i)
		{
			displayPoint(vec_inside[i], 5);
		}
	}

	void GenVecInside(Point3d center, float radius)
	{
		vec_inside.clear();
		int total = 0;
		for (auto it = vec_hojas.begin(); it != vec_hojas.end(); ++it)
		{
			if (InsideRegion(center, *it, radius, total))
			{
				if (total == 8)
				{
					//Nodo totalmente incluido en esfera
					std::copy((*it)->vec_puntos.begin(), (*it)->vec_puntos.end(), std::back_inserter(vec_inside));
				}
				else
				{
					//Nodo parcialmente incluido revisar contenido
					for (auto i = (*it)->vec_puntos.begin(); i != (*it)->vec_puntos.end(); ++i)
					{
						if (InsideRadius(center, *i, radius))
							vec_inside.push_back(*i);
					}
				}
			}
			else //Nodo alejado de esfera
				continue;
		}
	}


	OcTree::OcTree(int max_puntos, float x_max, float y_max, float z_max,
		float x_min, float y_min, float z_min)
	{
		this->max_puntos = max_puntos;
		root = new Node(max_puntos);
		root->isDivided = false;
		//Iniciando nodo raiz con sus extensiones
		root->x_max = x_max;
		root->y_max = y_max;
		root->z_max = z_max;
		root->x_min = x_min;
		root->y_min = y_min;
		root->z_min = z_min;
		//root->nivel = 0;
	}

	OcTree::~OcTree() {	}

private:
	void OcTree::SplitNode(Node * *nodo_padre)
	{
		//Creacion de 8 nodos vacios hijos (8 en Octree)
		(*nodo_padre)->isDivided = true;
		for (int i = 0; i < 8; ++i)
		{
			(*nodo_padre)->vec_nodos[i] = new Node(max_puntos);
		}
		//Actualizacion de frontera de cada nodo hijo

		Node*& nodo0 = (*nodo_padre)->vec_nodos[0];
		Node*& nodo1 = (*nodo_padre)->vec_nodos[1];
		Node*& nodo2 = (*nodo_padre)->vec_nodos[2];
		Node*& nodo3 = (*nodo_padre)->vec_nodos[3];

		Node*& nodo4 = (*nodo_padre)->vec_nodos[4];
		Node*& nodo5 = (*nodo_padre)->vec_nodos[5];
		Node*& nodo6 = (*nodo_padre)->vec_nodos[6];
		Node*& nodo7 = (*nodo_padre)->vec_nodos[7];

		int x_min = (*nodo_padre)->x_min;
		int x_medio = ((*nodo_padre)->x_max + (*nodo_padre)->x_min) / 2;
		int x_max = (*nodo_padre)->x_max;

		int y_min = (*nodo_padre)->y_min;
		int y_medio = ((*nodo_padre)->y_max + (*nodo_padre)->y_min) / 2;
		int y_max = (*nodo_padre)->y_max;

		int z_min = (*nodo_padre)->z_min;
		int z_medio = ((*nodo_padre)->z_max + (*nodo_padre)->z_min) / 2;
		int z_max = (*nodo_padre)->z_max;

		//Actualizacion de fronteras de Nodos Hijo
		nodo0->x_min = nodo2->x_min = nodo4->x_min = nodo6->x_min = x_min; //x_min

		nodo0->x_max = nodo2->x_max = nodo4->x_max = nodo6->x_max =
			nodo1->x_min = nodo3->x_min = nodo5->x_min = nodo7->x_min = x_medio; //comparten x_medio

		nodo1->x_max = nodo3->x_max = nodo5->x_max = nodo7->x_max = x_max; //x_max

		nodo2->y_min = nodo3->y_min = nodo6->y_min = nodo7->y_min = y_min; //y_min

		nodo0->y_min = nodo1->y_min = nodo4->y_min = nodo5->y_min =
			nodo2->y_max = nodo3->y_max = nodo6->y_max = nodo7->y_max = y_medio; //comparten y_medio

		nodo0->y_max = nodo1->y_max = nodo4->y_max = nodo5->y_max = y_max; //y_max

		nodo0->z_min = nodo1->z_min = nodo2->z_min = nodo3->z_min = z_min; //z_min

		nodo0->z_max = nodo1->z_max = nodo2->z_max = nodo3->z_max =
			nodo4->z_min = nodo5->z_min = nodo6->z_min = nodo7->z_min = z_medio;

		nodo4->z_max = nodo5->z_max = nodo6->z_max = nodo7->z_max = z_max;

		//Ubicacion de cada punto en su nuevo nodo

		for (int i = 0; i < (*nodo_padre)->vec_puntos.size(); ++i)
		{
			Point3d punto;
			punto[0] = (*nodo_padre)->vec_puntos[i][0];
			punto[1] = (*nodo_padre)->vec_puntos[i][1];
			punto[2] = (*nodo_padre)->vec_puntos[i][2];

			bool frontal = punto[2] <= z_medio;
			bool izquierdo = punto[0] <= x_medio;
			bool arriba = punto[1] >= y_medio;

			if (frontal)  //Cara Frontal
			{
				//Cero y Segundo cuadrante
				if (izquierdo)
				{
					if (arriba)
					{
						nodo0->vec_puntos.push_back(punto); //NE
					}
					else //abajo
					{
						nodo2->vec_puntos.push_back(punto); //SE
					}
				}
				//Primero y Tercer cuadrante
				else //derecha
				{
					if (arriba)
					{
						nodo1->vec_puntos.push_back(punto); //NO
					}
					else //abajo
					{
						nodo3->vec_puntos.push_back(punto); //SO
					}
				}
			}
			else //Posterior
			{
				//Cuarto y Sexto cuadrante
				if (izquierdo)
				{
					if (arriba)
					{
						nodo4->vec_puntos.push_back(punto); //NE
					}
					else //abajo
					{
						nodo6->vec_puntos.push_back(punto); //SE
					}
				}
				//Quinto y Setimo cuadrante
				else //derecho
				{
					if (arriba)
					{
						nodo5->vec_puntos.push_back(punto); //NO
					}
					else //abajo
					{
						nodo7->vec_puntos.push_back(punto); //SO
					}
				}
			}
		}

		//Recursividad de SplitNode
		for (int i = 0; i < 8; ++i)
		{
			if ((*nodo_padre)->vec_nodos[i]->vec_puntos.size() > max_puntos)
				SplitNode(&(*nodo_padre)->vec_nodos[i]);
		}
		//Eliminacion del vector de puntos en el nodo padre
		if ((*nodo_padre)->isDivided)
			(*nodo_padre)->vec_puntos.clear();
	}

	void displayPoint(Point3d point, float size)
	{

		glPointSize(size);
		glBegin(GL_POINTS);
		glVertex3f(point[0], point[1], point[2]);
		glEnd();
	}

	void displaySphere(Point3d & center, float radius)
	{
		glColor3ub(80, 76, 76);
		glBegin(GL_POINTS);
		glPushMatrix();
		glTranslatef(center[0], center[1], center[2]);
		glutSolidSphere(radius, 10, 10);
		glPopMatrix();
		glEnd();
		//glFlush();
		//glutPostRedisplay();
	}

	void displayRegion(Node * v)
	{
		glColor3ub(185, 75, 75);
		float size_x = abs((v->x_max - v->x_min) / 2);
		float size_y = abs((v->y_max - v->y_min) / 2);
		float size_z = abs((v->z_max - v->z_min) / 2);
		float mid_x = size_x / 2;
		float mid_y = size_y / 2;
		float mid_z = size_z / 2;

		//Centros
		int x_center = (v->x_max + v->x_min) / 2;
		int y_center = (v->y_max + v->y_min) / 2;
		int z_center = (v->z_max + v->z_min) / 2;

		//Desviaciones 
		float x_desv[8] = { -mid_x, mid_x, -mid_x, mid_x, -mid_x, mid_x, -mid_x, mid_x };

		float y_desv[8] = { mid_y, mid_y, -mid_y, -mid_y, mid_y, mid_y, -mid_y, -mid_y };

		float z_desv[8] = { -mid_z, -mid_z, -mid_z, -mid_z, mid_z, mid_z, mid_z, mid_z };

		//Front
		for (int i = 0; i < 8; ++i)
		{
			glPushMatrix();
			glTranslatef(x_center + x_desv[i], y_center + y_desv[i], z_center + z_desv[i]);
			glutSolidCube(size_x); //TODO Suponiendo que es un cubo modificar para generalizar
			glPopMatrix();
		}
	}

	bool InsideRadius(Point3d center, Point3d punto, float radius)
	{
		//Dado un punto center devolver si el punto se encuentra dentro de un
		//radio al cuadrado
		float dist = 0;
		for (int i = 0; i < 3; ++i)
		{
			dist += pow(center[i] - punto[i], 2);
		}
		return dist <= radius;
	}

	bool InsideRegion(Point3d center, Node * nodo, float radius, int& total)
	{
		//Dado un nodo indicar si al menos uno de sus vertices se encuentra
		//dentro del radio
		Point3d punto;
		int temp = 0;
		punto[0] = nodo->x_min; punto[1] = nodo->y_min; punto[2] = nodo->z_min;
		if (InsideRadius(center, punto, radius)) temp++;

		punto[0] = nodo->x_min; punto[1] = nodo->y_min; punto[2] = nodo->z_max;
		if (InsideRadius(center, punto, radius)) temp++;

		punto[0] = nodo->x_min; punto[1] = nodo->y_max; punto[2] = nodo->z_min;
		if (InsideRadius(center, punto, radius)) temp++;

		punto[0] = nodo->x_min; punto[1] = nodo->y_max; punto[2] = nodo->z_max;
		if (InsideRadius(center, punto, radius)) temp++;

		punto[0] = nodo->x_max; punto[1] = nodo->y_min; punto[2] = nodo->z_min;
		if (InsideRadius(center, punto, radius)) temp++;

		punto[0] = nodo->x_max; punto[1] = nodo->y_min; punto[2] = nodo->z_max;
		if (InsideRadius(center, punto, radius)) temp++;

		punto[0] = nodo->x_max; punto[1] = nodo->y_max; punto[2] = nodo->z_min;
		if (InsideRadius(center, punto, radius)) temp++;

		punto[0] = nodo->x_max; punto[1] = nodo->y_max; punto[2] = nodo->z_max;
		if (InsideRadius(center, punto, radius)) temp++;

		total = temp;
		if (temp > 0)
		{
			return true;
		}
		else
			return false;
	}



};

