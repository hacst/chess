#include "GuiWindow.h"

using namespace std;

GuiWindow::GuiWindow(const char* title, const bool fullscreen, const int width, const int height) {
	this->conf = { (char *)title, fullscreen, width, height };

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cerr << "Failed to init SDL: " << SDL_GetError() << endl;
		//return 1;		// how to handle errors?
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	menu = new Menu2D(this->conf.width, this->conf.height);
}

void GuiWindow::handleEvents() {
	while (SDL_PollEvent(&this->evt)) {
		switch (this->evt.type) {
			case SDL_QUIT:
				cout << "Quit" << endl;
				this->quit = true;
				break;
			case SDL_KEYDOWN:
				if (this->evt.key.keysym.sym == SDLK_ESCAPE) {
					this->quit = true;
				}

				cout << "Keydown: " << this->evt.key.keysym.sym << endl;
				break;
			case SDL_MOUSEMOTION:
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);

				menu->mouseMoved(mouseX, mouseY);
				break;
			case SDL_MOUSEBUTTONDOWN:
				menu->mouseClicked();
				break;
			case SDL_MOUSEBUTTONUP:
				menu->mouseReleased();
				break;
			default: break;
		}
	}
}

void GuiWindow::exec() {
	const int additional_config_flags = this->conf.fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

	this->window = SDL_CreateWindow(this->conf.title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		this->conf.width, this->conf.height,
		SDL_WINDOW_OPENGL | additional_config_flags);

	if (this->window == nullptr) {
		cerr << "Failed to create window: " << SDL_GetError() << endl;
		//return 1;
	}

	this->ogl = SDL_GL_CreateContext(this->window);

	SDL_GL_SetSwapInterval(1);

	// init OpenGL
	//this->resetProjection();
	//this->resetModelViewMatrix();

	// OpenGL's possible light models:
	// GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_EMISSIVE
	glShadeModel(GL_SMOOTH);		// Shading models: GL_SMOOTH, GL_FLAT
	glEnable(GL_DEPTH_TEST);		// Tiefentest (mit dem Z-Buffer) aktivieren
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// specify implementation-specific hints
	glEnable(GL_COLOR_MATERIAL);

	// ===== config the lights =====
	// light position
	GLfloat lightpos[] = { 0, 0, 0, 100.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	// ambient + diffuse + specular = illumination
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glEnable(GL_LIGHTING);	// enable lights in this OpenGL context
	glEnable(GL_LIGHT0);	// enable 1 of 8 possible lights

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glClearColor(0.6, 0.21, 0, 0.0);					// set background color

	const uint32_t start = SDL_GetTicks();
	const float pi = static_cast<float>(M_PI);

	int fpsCount = 0;
	unsigned int startTime = SDL_GetTicks();
	while (!this->quit) {
		//this->clearScreen();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	  // Clear Screen and Depth Buffer
		glLoadIdentity();

		++fpsCount;
		int elapsedTime = SDL_GetTicks() - startTime;
		if (elapsedTime > 1000) {
			cout << "FPS: " << (fpsCount / (static_cast<float>(elapsedTime) / 1000.0)) << endl;
			fpsCount = 0;
			startTime = SDL_GetTicks();
		}

		this->draw2D();

		SDL_GL_SwapWindow(this->window);

		this->handleEvents();
	}

	SDL_GL_DeleteContext(this->ogl);
	SDL_DestroyWindow(this->window);

	SDL_Quit();
}

void GuiWindow::set2DMode() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, this->conf.width, this->conf.height, 0, 0, 128);

	glTranslatef(0, 0, 0);	// move camera to the initial position out
}

void GuiWindow::set3DMode() {
	glMatrixMode(GL_PROJECTION);	// Kamera-Matrix
	glLoadIdentity();				// ... zurücksetzen

	this->makeFrustum(60, this->conf.width / this->conf.height, 0.1, 128);	// perspektivischer Sichtbereich

	glRotatef(22.5, 1.0, 0.0, 0.0);	// Neigungswinkel der Kamera 22,5° ("nach unten")
	glTranslatef(-cX, -cY, -cZ);	// move camera to the initial position out

	glMatrixMode(GL_MODELVIEW); 	// Transformationsmatrix
	glLoadIdentity();             	// zurücksetzen
}

void GuiWindow::resetModelViewMatrix() {
	glMatrixMode(GL_MODELVIEW); 	// Transformationsmatrix
	glLoadIdentity();             	// zurücksetzen
}

void GuiWindow::makeFrustum(double fovY, double aspectRatio, double front, double back) {
	const double DEG2RAD = 3.1415926535 / 180;

	double tangent = tan(fovY / 2 * DEG2RAD);   // tangent of half fovY
	double height = front * tangent;          // half height of near plane
	double width = height * aspectRatio;      // half width of near plane

	// params: left, right, bottom, top, near, far
	glFrustum(-width, width, -height, height, front, back);
}

void GuiWindow::drawCube(float size) {
	glBegin(GL_QUADS);
		/*
		GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
		GLfloat cyan[] = {0.f, .8f, .8f, 1.f};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
		glMaterialfv(GL_FRONT, GL_SPECULAR, white);*/
		//glMaterialfv(GL_FRONT, GL_SPECULAR, white);

		GLfloat shininess[] = { 0 };
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

		//front face
		glColor3f(1, 0, 0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(size / 2, size / 2, size / 2);
		glVertex3f(-size / 2, size / 2, size / 2);
		glVertex3f(-size / 2, -size / 2, size / 2);
		glVertex3f(size / 2, -size / 2, size / 2);

		//left face
		glColor3f(0, 0, 1);
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f(-size / 2, size / 2, size / 2);
		glVertex3f(-size / 2, size / 2, -size / 2);
		glVertex3f(-size / 2, -size / 2, -size / 2);
		glVertex3f(-size / 2, -size / 2, size / 2);

		//back face
		glColor3f(0, 1, 0);
		glNormal3f(0.0, 0.0, -1.0);
		glVertex3f(size / 2, size / 2, -size / 2);
		glVertex3f(-size / 2, size / 2, -size / 2);
		glVertex3f(-size / 2, -size / 2, -size / 2);
		glVertex3f(size / 2, -size / 2, -size / 2);

		//right face
		glColor3f(1, 0, 1);
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(size / 2, size / 2, -size / 2);
		glVertex3f(size / 2, size / 2, size / 2);
		glVertex3f(size / 2, -size / 2, size / 2);
		glVertex3f(size / 2, -size / 2, -size / 2);

		//top face
		glColor3f(1, 1, 0);
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(size / 2, size / 2, size / 2);
		glVertex3f(-size / 2, size / 2, size / 2);
		glVertex3f(-size / 2, size / 2, -size / 2);
		glVertex3f(size / 2, size / 2, -size / 2);

		//bottom face
		glColor3f(1, 1, 1);
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(size / 2, -size / 2, size / 2);
		glVertex3f(-size / 2, -size / 2, size / 2);
		glVertex3f(-size / 2, -size / 2, -size / 2);
		glVertex3f(size / 2, -size / 2, -size / 2);
	glEnd();
}

void GuiWindow::draw3D() {
	this->set3DMode();

	glPushMatrix();
		glTranslatef(-2, -2, -1.5);
		this->drawCube(3);
	glPopMatrix();
}

void GuiWindow::draw2D() {
	this->set2DMode();
	
	glPushMatrix();
		menu->draw();
	glPopMatrix();
}