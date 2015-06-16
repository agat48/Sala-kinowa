#include "glut.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "model3DS.h"
#include <direct.h>

// sta³e do obs³ugi menu podrêcznego

enum
{
	FULL_WINDOW, // aspekt obrazu - ca³e okno
	ASPECT_1_1, // aspekt obrazu 1:1
	EXIT // wyjœcie
};

// aspekt obrazu

int Aspect = FULL_WINDOW;

int windowWidth = 800, windowHeight = 600;
// wpó³rzêdne po³o¿enia obserwatora

GLdouble eyex = 0;
GLdouble eyey = 0;
GLdouble eyez = 3;

// wspó³rzêdne punktu w którego kierunku jest zwrócony obserwator,

GLdouble centerx = 0;
GLdouble centery = 0;
GLdouble centerz = -100;

float roomWidth, roomLength, roomHeight;
float c_x, c_y, c_z, zNear, zFar, diam;
float left, right, bottom, top;

float chairWidth, platformWidth, platformHeight, stairsWidth;
int rows, cols;


// funkcja generuj¹ca scenê 3D

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
	// kolor t³a - zawartoœæ bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru
	glClear(GL_COLOR_BUFFER_BIT);

	// wybór macierzy modelowania
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

	// skierowanie poleceñ do wykonania
	glFlush();

	// zamiana buforów koloru
	glutSwapBuffers();
}

// zmiana wielkoœci okna

void Reshape(int width, int height)
{
	// obszar renderingu - ca³e okno
	glViewport(0, 0, width, height);

	// wybór macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bry³y obcinania
	if (Aspect == ASPECT_1_1)
	{
		// wysokoœæ okna wiêksza od wysokoœci okna
		if (width < height && width > 0)
			glFrustum(-2.0, 2.0, -2.0 * height / width, 2.0 * height / width, 1.0, 5.0);
		else

			// szerokoœæ okna wiêksza lub równa wysokoœci okna
			if (width >= height && height > 0)
				glFrustum(-2.0 * width / height, 2.0 * width / height, -2.0, 2.0, 1.0, 5.0);

	}
	else
		//glFrustum(0.0, 2.0, 0.0, 2.0, 1.0, 5.0);
		glFrustum(left, right, bottom, top, zNear, zFar);

	// generowanie sceny 3D
	Display();
}

// obs³uga klawiatury

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

// obs³uga klawiszy funkcyjnych i klawiszy kursora

void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
		// kursor w lewo
	case GLUT_KEY_LEFT:
		eyex += 0.1;
		break;

		// kursor w górê
	case GLUT_KEY_UP:
		eyey -= 0.1;
		break;

		// kursor w prawo
	case GLUT_KEY_RIGHT:
		eyex -= 0.1;
		break;

		// kursor w dó³
	case GLUT_KEY_DOWN:
		eyey += 0.1;
		break;
	}

	// odrysowanie okna
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

// obs³uga menu podrêcznego

void Menu(int value)
{
	switch (value)
	{
		// obszar renderingu - ca³e okno
	case FULL_WINDOW:
		Aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		Aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// wyjœcie
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

	// rozmiary g³ównego okna programu
	glutInitWindowSize(windowWidth, windowHeight);
	ladujModele();

	// utworzenie g³ównego okna programu
#ifdef WIN32

	glutCreateWindow("Sala kinowa");
#else

	glutCreateWindow("Sala kinowa");
#endif

	// do³¹czenie funkcji generuj¹cej scenê 3D
	glutDisplayFunc(Display);

	// do³¹czenie funkcji wywo³ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// do³¹czenie funkcji obs³ugi klawiatury
	glutKeyboardFunc(Keyboard);

	// do³¹czenie funkcji obs³ugi klawiszy funkcyjnych i klawiszy kursora
	glutSpecialFunc(SpecialKeys);

	// utworzenie menu podrêcznego
	glutCreateMenu(Menu);

	// dodanie pozycji do menu podrêcznego
#ifdef WIN32

	glutAddMenuEntry("Aspekt obrazu - ca³e okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);
	glutAddMenuEntry("Wyjœcie", EXIT);
#else

	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// okreœlenie przycisku myszki obs³uguj¹cej menu podrêczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// wprowadzenie programu do obs³ugi pêtli komunikatów
	glutMainLoop();
	return 0;
}