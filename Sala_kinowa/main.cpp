#include "glut.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "model3DS.h"
#include <direct.h>

// sta�e do obs�ugi menu podr�cznego

enum
{
	FULL_WINDOW, // aspekt obrazu - ca�e okno
	ASPECT_1_1, // aspekt obrazu 1:1
	EXIT // wyj�cie
};

// aspekt obrazu

int Aspect = FULL_WINDOW;

int windowWidth = 800, windowHeight = 600;
// wp�rz�dne po�o�enia obserwatora

GLdouble eyex = 0;
GLdouble eyey = 0;
GLdouble eyez = 3;

// wsp�rz�dne punktu w kt�rego kierunku jest zwr�cony obserwator,

GLdouble centerx = 0;
GLdouble centery = 0;
GLdouble centerz = -100;

float roomWidth, roomLength, roomHeight;
float c_x, c_y, c_z, zNear, zFar, diam;
float left, right, bottom, top;

float chairWidth, platformWidth, platformHeight, stairsWidth;
int rows, cols;


// funkcja generuj�ca scen� 3D

struct model_w_skladzie {
	char * filename;
	model3DS * model;
	struct model_w_skladzie *wsk;
};
struct model_w_skladzie* sklad_modeli = NULL;

void dodajModel(model3DS * _model, char* file_name)
{
	struct model_w_skladzie* tmp;
	tmp = (struct model_w_skladzie *) malloc(sizeof(struct model_w_skladzie));
	tmp->filename = (char *)malloc(strlen(file_name) + 1);
	strcpy(tmp->filename, file_name);
	tmp->model = _model;
	tmp->wsk = sklad_modeli;
	sklad_modeli = tmp;
}

model3DS * pobierzModel(char* file_name)
{
	struct model_w_skladzie* sklad_tmp = sklad_modeli;
	while (sklad_tmp){
		if (!_stricmp(sklad_tmp->filename, file_name)) return sklad_tmp->model;
		char file_name_full[_MAX_PATH];
		strcpy(file_name_full, file_name);
		strcat(file_name_full, ".3ds");
		if (!_stricmp(sklad_tmp->filename, file_name_full)) return sklad_tmp->model;

		sklad_tmp = sklad_tmp->wsk;
	}
	return NULL;
}

void rysujModel(char * file_name, int tex_num = -1)
{
	model3DS * model_tmp;
	if (model_tmp = pobierzModel(file_name))
	{
		std::cout << "Model " << file_name << " found" << std::endl;
		if (tex_num == -1)
			model_tmp->draw();
		else
			model_tmp->draw(tex_num, false);
	}

}

void ladujModele()
{
	WIN32_FIND_DATA *fd;
	HANDLE fh;
	model3DS * model_tmp;
	char directory[_MAX_PATH];
	if (_getcwd(directory, _MAX_PATH) == NULL) return;
	strcat(directory, "\\data\\*.3ds");

	fd = (WIN32_FIND_DATA *)malloc(sizeof(WIN32_FIND_DATA));
	fh = FindFirstFile((LPCSTR)directory, fd);
	if (fh != INVALID_HANDLE_VALUE)
		do {
			if (fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){	// katalogi ignorujemy
				if (FindNextFile(fh, fd)) continue; else break;
			}
			// ladowanie obiektu i dodanie do kontenera
			char filename[_MAX_PATH];
			strcpy(filename, "data\\");
			strcat(filename, fd->cFileName);
			model_tmp = new model3DS(filename, 1, false);
			dodajModel(model_tmp, fd->cFileName);
			std::cout << "[3DS] Model " << fd->cFileName << " stored" << std::endl;
		} while (FindNextFile(fh, fd));
}

void Display()
{
	// kolor t�a - zawarto�� bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru
	glClear(GL_COLOR_BUFFER_BIT);

	// wyb�r macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// ustawienie obserwatora
	//gluLookAt(eyex-1, eyey+1, eyez-2, centerx, centery, centerz, 0, 1, 0);
	gluLookAt(-1, 2.0, eyez - 2, c_x, c_y, centerz+20, 0.0, 1.0, 0.0);

	glColor3f(0.0, 0.0, 0.0);

	glBegin(GL_QUADS);         // Rysujemy kwadraty

	glVertex3f(0.0, 0.0, -roomLength);
	glVertex3f(roomWidth, 0.0, -roomLength);
	glVertex3f(roomWidth, roomHeight, -roomLength);
	glVertex3f(0.0, roomHeight, -roomLength);

	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(roomWidth, 0.0, -roomLength);
	glVertex3f(roomWidth, 0.0, 0.0);
	glVertex3f(roomWidth, roomHeight, 0.0);
	glVertex3f(roomWidth, roomHeight, -roomLength);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, -roomLength);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(roomWidth, 0.0, 0.0);
	glVertex3f(roomWidth, 0.0, -roomLength);


	// koniec definicji prymitywu
	glEnd();

	glPushMatrix();
	glTranslatef(5, -1, -1);
	rysujModel("chair");
	glPopMatrix();
	glPushMatrix();
	glTranslatef(8, -1, -1);
	rysujModel("chair");
	glPopMatrix();

	// skierowanie polece� do wykonania
	glFlush();

	// zamiana bufor�w koloru
	glutSwapBuffers();
}

// zmiana wielko�ci okna

void Reshape(int width, int height)
{
	// obszar renderingu - ca�e okno
	glViewport(0, 0, width, height);

	// wyb�r macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bry�y obcinania
	if (Aspect == ASPECT_1_1)
	{
		// wysoko�� okna wi�ksza od wysoko�ci okna
		if (width < height && width > 0)
			glFrustum(-2.0, 2.0, -2.0 * height / width, 2.0 * height / width, 1.0, 5.0);
		else

			// szeroko�� okna wi�ksza lub r�wna wysoko�ci okna
			if (width >= height && height > 0)
				glFrustum(-2.0 * width / height, 2.0 * width / height, -2.0, 2.0, 1.0, 5.0);

	}
	else
		//glFrustum(0.0, 2.0, 0.0, 2.0, 1.0, 5.0);
		glFrustum(left, right, bottom, top, zNear, zFar);

	// generowanie sceny 3D
	Display();
}

// obs�uga klawiatury

void Keyboard(unsigned char key, int x, int y)
{
	// klawisz +
	if (key == '+')
		eyez -= 0.1;
	else

		// klawisz -
		if (key == '-')
			eyez += 0.1;

	// odrysowanie okna
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

// obs�uga klawiszy funkcyjnych i klawiszy kursora

void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
		// kursor w lewo
	case GLUT_KEY_LEFT:
		eyex += 0.1;
		break;

		// kursor w g�r�
	case GLUT_KEY_UP:
		eyey -= 0.1;
		break;

		// kursor w prawo
	case GLUT_KEY_RIGHT:
		eyex -= 0.1;
		break;

		// kursor w d�
	case GLUT_KEY_DOWN:
		eyey += 0.1;
		break;
	}

	// odrysowanie okna
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

// obs�uga menu podr�cznego

void Menu(int value)
{
	switch (value)
	{
		// obszar renderingu - ca�e okno
	case FULL_WINDOW:
		Aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		Aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// wyj�cie
	case EXIT:
		exit(0);
	}
}




void setDisplayCoordinates() {
	c_x = roomWidth / 2;
	c_y = roomHeight / 2;
	c_z = -roomLength / 2;

	diam = sqrt(c_y * c_y + c_x * c_x + c_z * c_z);

	left = c_x - diam;
	right = c_x + diam;
	bottom = c_y - diam;
	top = c_y + diam;

	GLdouble aspect = (GLdouble)windowWidth / windowHeight;

	if (aspect < 1.0) { // window taller than wide
		bottom /= aspect;
		top /= aspect;
	}
	else {
		left *= aspect;
		right *= aspect;
	}

	zNear = 1.0;
	zFar = zNear + diam + 1;
}


int main(int argc, char * argv[])
{
	roomWidth = 5;
	roomHeight = 1;
	roomLength = 1;

	setDisplayCoordinates();
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	// rozmiary g��wnego okna programu
	glutInitWindowSize(windowWidth, windowHeight);
	ladujModele();

	// utworzenie g��wnego okna programu
#ifdef WIN32

	glutCreateWindow("Sala kinowa");
#else

	glutCreateWindow("Sala kinowa");
#endif

	// do��czenie funkcji generuj�cej scen� 3D
	glutDisplayFunc(Display);

	// do��czenie funkcji wywo�ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// do��czenie funkcji obs�ugi klawiatury
	glutKeyboardFunc(Keyboard);

	// do��czenie funkcji obs�ugi klawiszy funkcyjnych i klawiszy kursora
	glutSpecialFunc(SpecialKeys);

	// utworzenie menu podr�cznego
	glutCreateMenu(Menu);

	// dodanie pozycji do menu podr�cznego
#ifdef WIN32

	glutAddMenuEntry("Aspekt obrazu - ca�e okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);
	glutAddMenuEntry("Wyj�cie", EXIT);
#else

	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// okre�lenie przycisku myszki obs�uguj�cej menu podr�czne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// wprowadzenie programu do obs�ugi p�tli komunikat�w
	glutMainLoop();
	return 0;
}