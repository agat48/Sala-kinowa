#include <windows.h>
#include "glut.h"
#include "model3DS.h"
#include <time.h>
#include <direct.h>
#include <GL/glu.h>


GLint oknoLewe = 1;

double kameraX;
double kameraY;
double kameraZ;
double kameraPunktY;
double kameraPredkoscPunktY;
double kameraPredkosc;
bool kameraPrzemieszczanie;		// przemieszczanie lub rozgl¹danie
double kameraKat;				// kat patrzenia
double kameraPredkoscObrotu;

void windowInit()
{
	glClearColor(0.2, 0.2, 1.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
	GLfloat  ambient[4] = { 0.3, 0.3, 0.3, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

	GLfloat  diffuse[4] = { 0.9, 0.9, 0.9, 1 };
	GLfloat  specular[4] = { 0.9, 0.9, 0.9, 1 };
	GLfloat	 position[4] = { 30, 30, -30, 1 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);  // œwiatlo sceny

}

struct model_w_skladzie {
	char * filename;
	model3DS * model;
	struct model_w_skladzie *wsk;
};
struct model_w_skladzie* sklad_modeli = NULL;

model3DS * pobierzModel(char* file_name)
{
	struct model_w_skladzie* sklad_tmp = sklad_modeli;
	while (sklad_tmp){
		if (!_stricmp(sklad_tmp->filename, file_name)) return sklad_tmp->model;
		char file_name_full[_MAX_PATH];
		strcpy_s(file_name_full, file_name);
		strcat_s(file_name_full, ".3ds");
		if (!_stricmp(sklad_tmp->filename, file_name_full)) return sklad_tmp->model;

		sklad_tmp = sklad_tmp->wsk;
	}
	return NULL;
}

void rysujModel(char * file_name, int tex_num = -1)
{
	model3DS * model_tmp;
	if (model_tmp = pobierzModel(file_name))
		if (tex_num == -1)
			model_tmp->draw();
		else
			model_tmp->draw(tex_num, false);

}

void rysujRamke(bool prawa)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Kasowanie ekranu
	glLoadIdentity();
	gluLookAt(kameraX, kameraY, kameraZ, kameraX + 100 * sin(kameraKat), 3 + kameraPunktY, kameraZ - 100 * cos(kameraKat), 0, 1, 0); // kamera


#define _RYSOWANIE
//#include "rysowanie.cpp"	// rysowanie 

	glFlush();
	glPopMatrix();
}


void rysuj()
{
	rysujRamke(false);
	glutSwapBuffers();
}

void resetKamery(){
	kameraX = 0;
	kameraY = 4;
	kameraZ = 40;
	kameraKat = -0.4;
	kameraPunktY = -15;
	kameraPredkoscPunktY = 0;
	kameraPredkosc = 0;
	kameraPredkoscObrotu = 0;
	kameraPrzemieszczanie = true;
}

int mainn(int argc, char **argv)
{
	int stereoTryb, oknoSzerkosc, oknoWysokosc;
	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 's') 	// poprawki w konfiguracji na podstawie parametró (te maj¹ pierwszeñstwo)
	{
		stereoTryb = 2;
		oknoSzerkosc = 800;
		oknoWysokosc = 600;
	}
	glutInit(&argc, argv);    	// INIT - wszystkie funkcje obs³ugi okna i przetwzrzania zdarzeñ realizuje GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	/*
	if (stereoTryb == 2) {
		glutInitWindowSize(oknoSzerkosc - 8, oknoWysokosc);
		glutInitWindowPosition(0, 0);
		oknoLewe = glutCreateWindow("Lewe");  // ==1
		HWND hwnd = FindWindow(NULL, "Lewe");
		SetWindowLong(hwnd, GWL_STYLE, WS_BORDER | WS_MAXIMIZE);
		glutSetWindow(oknoLewe);
		windowInit();

		glutReshapeFunc(rozmiarLewe);						// def. obs³ugi zdarzenia resize (GLUT)
		glutKeyboardFunc(KlawiszKlawiaturyWcisniety);		// def. obs³ugi klawiatury
		glutSpecialFunc(KlawiszSpecjalnyWcisniety);		// def. obs³ugi klawiatury (klawisze specjalne)
		glutMouseFunc(PrzyciskMyszyWcisniety); 			// def. obs³ugi zdarzenia przycisku myszy (GLUT)
		glutMotionFunc(RuchKursoraMyszy);					// def. obs³ugi zdarzenia ruchu myszy (GLUT)
		glutDisplayFunc(rysuj);								// def. funkcji rysuj¹cej 
		glutInitWindowSize(oknoSzerkosc - 8, oknoWysokosc);
		glutInitWindowPosition(oknoSzerkosc + 4, 0);
		oknoPrawe = glutCreateWindow("Prawe"); // ==2
		glutSetWindow(oknoPrawe);
		windowInit();
		hwnd = FindWindow(NULL, "Prawe");
		SetWindowLong(hwnd, GWL_STYLE, WS_BORDER | WS_MAXIMIZE);
		glutReshapeFunc(rozmiarPrawe);						// def. obs³ugi zdarzenia resize (GLUT)
		glutKeyboardFunc(KlawiszKlawiaturyWcisniety);		// def. obs³ugi klawiatury
		glutSpecialFunc(KlawiszSpecjalnyWcisniety);		// def. obs³ugi klawiatury (klawisze specjalne)
		glutMouseFunc(PrzyciskMyszyWcisniety); 			// def. obs³ugi zdarzenia przycisku myszy (GLUT)
		glutMotionFunc(RuchKursoraMyszy);					// def. obs³ugi zdarzenia ruchu myszy (GLUT)
		glutDisplayFunc(rysuj);								// def. funkcji rysuj¹cej 
	}
	else {  // jedno okno 
	*/
		glutInitWindowSize(oknoSzerkosc, oknoWysokosc);
		glutInitWindowPosition(0, 0);
		oknoLewe = glutCreateWindow("Sala kinowa");
		windowInit();
/*		glutReshapeFunc(rozmiar);						// def. obs³ugi zdarzenia resize (GLUT)
		glutKeyboardFunc(KlawiszKlawiaturyWcisniety);		// def. obs³ugi klawiatury
		glutSpecialFunc(KlawiszSpecjalnyWcisniety);		// def. obs³ugi klawiatury (klawisze specjalne)
		glutMouseFunc(PrzyciskMyszyWcisniety); 			// def. obs³ugi zdarzenia przycisku myszy (GLUT)
		glutMotionFunc(RuchKursoraMyszy);					// def. obs³ugi zdarzenia ruchu myszy (GLUT) */
		glutDisplayFunc(rysuj);								// def. funkcji rysuj¹cej
	}
	/*
	if (stereoTryb == 1 || !timing100FPS)
		glutIdleFunc(timer);
	else
		glutTimerFunc(10, syncTimer, 10);
		*/
	resetKamery();
	//srand( (unsigned)time( NULL ) ); // generator liczb losowych
	//ladujModele();
	if (oknoFullScreen && stereoTryb != 2) glutFullScreen();
	glutMainLoop();
	return(0);
}