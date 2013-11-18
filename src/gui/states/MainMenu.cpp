#include <iostream>
#include <SDL.h>

#include "gui/states/MainMenu.h"

using namespace std;

void MainMenu::enter() {
	std::cout << "entered main menu!" << std::endl;
}

AbstractState* MainMenu::run() {
	SDL_Delay(1000);
	std::cout << "run main menu! (sleep 1000ms)" << std::endl;

	return this;
}

void MainMenu::exit() {
	std::cout << "left main menu!" << std::endl;
}