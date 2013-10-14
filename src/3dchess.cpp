#include <iostream>
#include <SDL.h>
#include <cmath>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <boost/program_options.hpp>
#include <boost/signals2.hpp>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

namespace po = boost::program_options;

using namespace std;

class SerialExample {
public:
	SerialExample() : myint(0), mybool(false), myfloat(0.5) {}

private:
	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned int /*version*/) {

		ar & BOOST_SERIALIZATION_NVP(myint);
		ar & BOOST_SERIALIZATION_NVP(mybool);
		ar & BOOST_SERIALIZATION_NVP(myfloat);
	}

	int myint;
	bool mybool;
	float myfloat;
};

int main(int argn, char **argv) {

	po::options_description desc("Test");
	desc.add_options()
			("help", "Print help message")
			("width", po::value<int>()->default_value(640), "Horizontal resolution")
			("height", po::value<int>()->default_value(480), "Vertical resolution")
			("fullscreen", "If set program runs in fullscreen")
			;

	po::variables_map vm;
	po::store(po::parse_command_line(argn, argv, desc), vm);

	if (vm.count("help")) {
		cerr << desc << endl;
		return 1;
	}

	SerialExample inst;

	{
		ofstream ofs("test.xml");
		boost::archive::xml_oarchive xmlo(ofs);
		xmlo << boost::serialization::make_nvp("test", inst);
	}

	const int width = vm["width"].as<int>();
	const int height = vm["height"].as<int>();
	const bool fullscreen = vm.count("fullscreen");

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
	}

	SDL_GL_DeleteContext(ogl);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}
