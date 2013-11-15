#include "GuiWindow.h"
#include "helper.h"

using namespace std;

GuiWindow::GuiWindow(string title, bool fullscreen, int width, int height) {
	m_title = title;
	m_fullscreen = fullscreen;
	m_width = width;
	m_height = height;

	// initial position of the camera
	m_cX = -10.0;
	m_cY = 10.0;
	m_cZ = 20.0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cerr << "Failed to init SDL: " << SDL_GetError() << endl;
		//return 1;		// how to handle errors?
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}

void GuiWindow::handleEvents() {
	while (SDL_PollEvent(&evt)) {
		switch (evt.type) {
			case SDL_QUIT:
				cout << "Quit" << endl;
				m_quit = true;
				break;
			case SDL_KEYDOWN:
				if (evt.key.keysym.sym == SDLK_ESCAPE) {
					m_quit = true;
				}

				LOG(debug) << "Keydown: " << evt.key.keysym.sym;
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
	const int additional_config_flags = m_fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

	window = SDL_CreateWindow(m_title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_width, m_height,
		SDL_WINDOW_OPENGL | additional_config_flags);

	if (window == nullptr) {
		cerr << "Failed to create window: " << SDL_GetError() << endl;
		//return 1;
	}

	ogl = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(1);

	menu = make_shared<Menu2D>(m_width, m_height, *this);

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
	GLfloat light_ambient[] = { 0.9, 0.9, 0.9, 1.0 };
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

	int fpsCount = 0;
	unsigned int startTime = SDL_GetTicks();
	while (!m_quit) {
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

		draw2D();
		// here should the state machine be ... State(enter, run, leave ...)

		SDL_GL_SwapWindow(window);

		handleEvents();
	}

	SDL_GL_DeleteContext(ogl);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

void GuiWindow::set2DMode() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_width, m_height, 0, 0, 128);

	glTranslatef(0, 0, 0);	// move camera to the initial position out
}

void GuiWindow::set3DMode() {
	glMatrixMode(GL_PROJECTION);	// Kamera-Matrix
	glLoadIdentity();				// ... zurücksetzen

	this->makeFrustum(60, m_width / m_height, 0.1, 128);	// perspektivischer Sichtbereich

	glRotatef(22.5, 1.0, 0.0, 0.0);		// Neigungswinkel der Kamera 22,5° ("nach unten")
	glTranslatef(-m_cX, -m_cY, -m_cZ);	// move camera to the initial position out

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
