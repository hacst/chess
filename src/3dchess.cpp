#include <iostream>
#include <SDL.h>
#include <cmath>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/signals2.hpp>

#include "helper.h"
#include "GameLogic.h"
#include "ConsolePlayer.h"
#include "ConsoleObserver.h"
#include "DummyPlayer.h"
#include "GameConfiguration.h"
#include "ObserverDispatcherProxy.h"

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

#include <SOIL.h>

namespace po = boost::program_options;
namespace sig = boost::signals2;

using namespace std;

class FooThread : public ServiceDispatcherThread {
public:
	void someFoo(int val) {
		post([this, val]() {
			cout << "Foo called with " << val << endl;
		});
	}

	future<int> someFooWithResult() {
		return postPromise([]() {
			return 10;
		});
	}

private:
};

int main(int argn, char **argv) {
	// Boost program options for commandline parsing

	po::options_description desc("Test");
	desc.add_options()
			("help", "Print help message")
			("width", po::value<int>()->default_value(640), "Horizontal resolution")
			("height", po::value<int>()->default_value(480), "Vertical resolution")
			("fullscreen", "If set program runs in fullscreen")
			;

	GLuint tex_2d = SOIL_load_OGL_texture
		(
		"img.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

	po::variables_map vm;
	po::store(po::parse_command_line(argn, argv, desc), vm);

	if (vm.count("help")) {
		cerr << desc << endl;
		return 1;
	}
	
	auto firstPlayer = make_shared<DummyPlayer>();
	firstPlayer->start();
	auto secondPlayer = make_shared<DummyPlayer>();
	secondPlayer->start();

	auto observer = make_shared<ConsoleObserver>();

	AbstractGameLogicPtr gameLogic = make_shared<GameLogic>(firstPlayer, secondPlayer);
	gameLogic->addObserver(observer);
	auto observerProxy = make_shared<ObserverDispatcherProxy>(observer);
	gameLogic->addObserver(observerProxy);
	
	gameLogic->start();
	
	GameConfiguration config;
	config.save("config.xml");

	auto configl = GameConfiguration::load("config.xml");
	if (!configl) {
		cout << "Failed to load" << endl;
	} else {
		cout << "Dur: " << (configl->maximumTurnTimeInSeconds) << "s" << endl;
	}

	// SDL2/OpenGL for graphics
	const int width = vm["width"].as<int>();
	const int height = vm["height"].as<int>();
	const bool fullscreen = (vm.count("fullscreen") != 0);

	cout << "Width:         " << width << endl;
	cout << "Height:        " << height << endl;
	cout << "Fullscreen:    " << fullscreen << endl;

	SDL_Window *window;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cerr << "Failed to init SDL: " << SDL_GetError() << endl;
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	const int additional_config_flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

	window = SDL_CreateWindow("Hello chess",
							  SDL_WINDOWPOS_UNDEFINED,
							  SDL_WINDOWPOS_UNDEFINED,
							  width, height,
							  SDL_WINDOW_OPENGL | additional_config_flags);

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
				if (evt.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
				}

				cout << "Keydown: " << evt.key.keysym.sym << endl;
				break;
			default: break;
			}
		}
		
		// Execute all pending calls from the observer
		observerProxy->poll();
	}

	SDL_GL_DeleteContext(ogl);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}
