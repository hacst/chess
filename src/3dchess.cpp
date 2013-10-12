#include <iostream>
#include <SDL.h>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

using namespace std;

int main(int argn, char **argv) {
	SDL_Window *window;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cerr << "Failed to init SDL: " << SDL_GetError() << endl;
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("Hello chess",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640, 480,
		SDL_WINDOW_OPENGL);

	if (window == nullptr) {
		cerr << "Failed to create window: " << SDL_GetError() << endl;
		return 1;
	}

	SDL_GLContext ogl = SDL_GL_CreateContext(window);
	
	SDL_GL_SetSwapInterval(1);

	bool quit = false;
	SDL_Event evt;

	const uint32_t start = SDL_GetTicks();
	const float pi = static_cast<float>(M_PI);

	while (!quit) {
		const float elapsed = static_cast<float>(SDL_GetTicks() - start) / 1000;

		const float cycletime = 5.0f;
		const float r = sinf(elapsed * (2 * pi) / cycletime);
		const float g = cosf(elapsed * (2 * pi) / (cycletime + 1));
		const float b = sinf(elapsed * (2 * pi) / (cycletime + 2) + pi / 2);


		glClearColor(r,g,b, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(window);

		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_QUIT:
				cout << "Quit" << endl;
				quit = true;
				break;
			case SDL_KEYDOWN:
				cout << "Keydown" << endl;
				break;
			default: break;
			}
		}
	}

	SDL_GL_DeleteContext(ogl);
	SDL_DestroyWindow(window);

	SDL_Quit();
    return 0;
}
