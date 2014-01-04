#include "gui/GuiWindow.h"

#include "misc/helper.h"

using namespace std;

GuiWindow::GuiWindow(string title, bool fullscreen, int width, int height)
	: m_title(title)
	, m_fullscreen(fullscreen)
	, m_width(width)
	, m_height(height)
	, m_fsm(StateMachine::getInstance()) {

	// front and back pane
	m_zNear = 0.1f;
	m_zFar = 400.0f;

	// initial position of the camera
	m_cX = 0.0f;
	m_cY = 200.0f;
	m_cZ = m_zFar / 2.0f;

	m_cameraAngleX = 40.0f;
	m_cameraAngleY = 0.0f;
	m_cameraAngleZ = 0.0f;

	m_fov = 60.0;

	// init other parameters
	m_depthBits = 16;
	m_antiAlias = 8;

	m_reloadState = false;
}

GuiWindow::~GuiWindow() {
}

int GuiWindow::getWidth() {
	return m_width;
}

int GuiWindow::getHeight() {
	return m_height;
}

int GuiWindow::getCameraDistanceToOrigin() {
	return static_cast<int>(m_zFar / 2.0f);
}

bool GuiWindow::isFullscreen() {
	return m_fullscreen;
}

void GuiWindow::switchWindowMode(WindowMode mode) {
	if (mode == WindowMode::FULLSCREEN && m_fullscreen != true) {
		SDL_SetWindowFullscreen(window, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
		
		// save current resolution settings
		m_widthOld = m_width;
		m_heightOld = m_height;

		// get window size and set it to new resolution
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		m_width = width;
		m_height = height;

		// change viewport and correct aspect ratio
		glViewport(0, 0, m_width, m_height);
		glMatrixMode(GL_PROJECTION);
		
		float aspect = (float)m_width / (float)m_height;
		glOrtho(-aspect, aspect, -1, 1, m_zNear, m_zFar);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		m_fullscreen = true;
	} else {
		// restore previous resolution
		m_width = m_widthOld;
		m_height = m_heightOld;

		m_fullscreen = false;

		// we're recreating a new context
		SDL_GL_DeleteContext(ogl);
		SDL_DestroyWindow(window);
		
		init();

		// make sure to reinit current state of state machine
		// to reload all textures and objects for OGL
		// do not call the run() of the current state again! -> leads into confusing state
		m_reloadState = true;
	}
}

void GuiWindow::init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cerr << "Failed to init SDL: " << SDL_GetError() << endl;
		//return 1;		// how to handle errors?
	}

	// framebuffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_depthBits);

	// anti-aliasing
	if (m_antiAlias > 1) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_antiAlias);
	} else {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
	}

	// window flags
	int flags = 0;
	if (m_fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	
	window = SDL_CreateWindow(m_title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_width,
		m_height,
		SDL_WINDOW_OPENGL | flags);

	if (window == nullptr) {
		cerr << "Failed to create window: " << SDL_GetError() << endl;
		//return 1;
	}

	ogl = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);

	loadFonts();
}

void GuiWindow::loadFonts() {
	fontHeadline.init("resources/Lato-Bla.ttf", fontSize::HEADLINE);
	fontSubHeadline.init("resources/Lato-BlaIta.ttf", fontSize::SUB_HEADLINE);
	fontText.init("resources/Signika.ttf", fontSize::TEXT);
	fontTextSmall.init("resources/Signika.ttf", fontSize::TEXT_SMALL);
}

void GuiWindow::terminate() {
	SDL_GL_DeleteContext(ogl);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

void GuiWindow::handleEvents() {
	m_fsm.eventmap.mouseMoved = false;
	m_fsm.eventmap.mouseUp = false;
	m_fsm.eventmap.mouseDown = false;

	while (SDL_PollEvent(&evt)) {
		switch (evt.type) {
			case SDL_QUIT:
				exit();
				break;
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
					case SDLK_ESCAPE:
						m_fsm.eventmap.keyEscape = true;
						break;
					case SDLK_LEFT:
						m_fsm.eventmap.keyLeft = true;
						break;
					case SDLK_RIGHT:
						m_fsm.eventmap.keyRight = true;
						break;
					case SDLK_UP:
						m_fsm.eventmap.keyUp = true;
						break;
					case SDLK_DOWN:
						m_fsm.eventmap.keyDown = true;
						break;
					case SDLK_0:
						m_fsm.eventmap.key0 = true;
						break;
					case SDLK_1:
						m_fsm.eventmap.key1 = true;
						break;
					case SDLK_a:
						m_fsm.eventmap.keyA = true;
						break;
					case SDLK_y:
						m_fsm.eventmap.keyY = true;
						break;
					default:
						break;
				}
				break;
			case SDL_KEYUP:
				switch (evt.key.keysym.sym) {
				case SDLK_ESCAPE:
					m_fsm.eventmap.keyEscape = false;
					break;
				case SDLK_LEFT:
					m_fsm.eventmap.keyLeft = false;
					break;
				case SDLK_RIGHT:
					m_fsm.eventmap.keyRight = false;
					break;
				case SDLK_UP:
					m_fsm.eventmap.keyUp = false;
					break;
				case SDLK_DOWN:
					m_fsm.eventmap.keyDown = false;
					break;
				case SDLK_0:
					m_fsm.eventmap.key0 = false;
					break;
				case SDLK_1:
					m_fsm.eventmap.key1 = false;
					break;
				case SDLK_a:
					m_fsm.eventmap.keyA = false;
					break;
				case SDLK_y:
					m_fsm.eventmap.keyY = false;
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				// save old values
				m_oldMouseX = m_fsm.eventmap.mouseX;
				m_oldMouseY = m_fsm.eventmap.mouseY;

				// get new values
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);

				m_fsm.eventmap.mouseMoved = true;
				m_fsm.eventmap.mouseX = mouseX;
				m_fsm.eventmap.mouseY = mouseY;
				break;
			case SDL_MOUSEBUTTONDOWN:
				m_fsm.eventmap.mouseDown = true;
				break;
			case SDL_MOUSEBUTTONUP:
				m_fsm.eventmap.mouseUp = true;
				break;
			default: break;
		}
	}
}

void GuiWindow::exit() {
	m_quit = true;
}

void GuiWindow::exec() {
	init();

	// init finite state machine (make sure that your OpenGL context is already created and initialized!)
	m_fsm.window = this;
	m_fsm.setStartState(new MenuMain());

	// ===== frame counter =====
	int fpsCount = 0;
	unsigned int startTime = SDL_GetTicks();

	// ===== game main loop =====
	while (!m_quit) {
		unsigned int start = SDL_GetTicks();

		++fpsCount;
		int elapsedTime = SDL_GetTicks() - startTime;
		if (elapsedTime > 1000) {
			//cout << "FPS: " << (fpsCount / (static_cast<float>(elapsedTime) / 1000.0)) << endl;
			fpsCount = 0;
			startTime = SDL_GetTicks();
		}

		// this is for resolution changes
		if (m_reloadState) {
			m_fsm.setNextState(new MenuMain());
			m_reloadState = false;
		}

		// exit if state is nullptr
		if (m_fsm.run() == nullptr) {
			m_quit = true;
		}

		swapFrameBufferNow();
		handleEvents();											// we handle events after the first frame is drawn

		// limit the fps rate
		int tp1f = 1000 / 30;
		int tpf = SDL_GetTicks() - start;
		if (tpf < tp1f) {
			SDL_Delay(tp1f - tpf);
		}
	}

	this->terminate();
}

void GuiWindow::swapFrameBufferNow() {
	SDL_GL_SwapWindow(window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// clear Screen and depth buffer
}

void GuiWindow::set2DMode() {
	//glDisable(GL_DEPTH_TEST);

	glOrtho(0, m_width, m_height, 0, 0, 128);

	glTranslatef(0, 0, 0);	// move camera to the initial position out

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void GuiWindow::set3DMode() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	makeFrustum(m_fov, m_width / m_height, m_zNear, m_zFar);	// perpective view

	glRotatef(m_cameraAngleX, 1.0, 0.0, 0.0);	// rotate around x axis
	glRotatef(m_cameraAngleY, 0.0, 1.0, 0.0);	// rotate around y axis
	glRotatef(m_cameraAngleZ, 0.0, 0.0, 1.0);	// rotate around z axis

	glTranslatef(-m_cX, -m_cY, -m_cZ);	// move camera to the initial position out

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void GuiWindow::makeFrustum(double fovY, double aspectRatio, double front, double back) {
	const double DEG2RAD = 3.1415926535 / 180;

	double tangent = tan(fovY / 2 * DEG2RAD);   // tangent of half fovY
	double height = front * tangent;			// half height of near plane
	double width = height * aspectRatio;		// half width of near plane

	// params: left, right, bottom, top, near, far
	glFrustum(-width, width, -height, height, front, back);
}

void GuiWindow::printText(int x, int y, float red, float green, float blue, std::string text) {
	fontObject fo = {
		x, y,
		red, green, blue,
		fontSize::TEXT,
		fontText,
		text
	};

	drawText(fo);
}

void GuiWindow::printTextSmall(int x, int y, float red, float green, float blue, std::string text) {
	fontObject fo = {
		x, y,
		red, green, blue,
		fontSize::TEXT_SMALL,
		fontTextSmall,
		text
	};

	drawText(fo);
}


void GuiWindow::printHeadline(std::string text) {
	fontObject fo = {
		10, 10,
		0.8f, 0.8f, 0.8f,
		fontSize::HEADLINE,
		fontHeadline,
		text
	};

	drawText(fo);
}

void GuiWindow::printSubHeadline(std::string text) {
	fontObject fo = {
		40, fontSize::HEADLINE + 20,
		0.8f, 0.8f, 0.8f,
		fontSize::SUB_HEADLINE,
		fontSubHeadline,
		text
	};

	drawText(fo);
}

void GuiWindow::printTextCenter(float red, float green, float blue, std::string text) {
	fontObject fo = {
        (m_width / 2) - (((int)strlen(text.c_str()) * fontSize::TEXT) / 2) /* x */,
        (m_height / 2) - (fontSize::TEXT / 2) /* y */,
		red, green, blue,
		fontSize::TEXT,
		fontText,
		text
	};

	drawText(fo);
}

void GuiWindow::drawText(fontObject fo) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
		glColor3f(fo.red, fo.green, fo.blue);
		int inverseOffset = m_height - fo.fontSize;
		freetype::print(fo.font, static_cast<float>(fo.x), static_cast<float>(inverseOffset - fo.y), fo.text.c_str());
	glPopMatrix();

	set2DMode();
}
