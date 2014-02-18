#ifndef ARROWNAVIGATIONHANDLER_H
#define ARROWNAVIGATIONHANDLER_H

#include <chrono>
#include <memory>
#include "logic/ChessTypes.h"

class ArrowNavigationHandler {
public:
	ArrowNavigationHandler(bool inverseNavigation);
	
	enum ArrowKey {
		UP,
		RIGHT,
		DOWN,
		LEFT
	};

	void onKey(ArrowKey direction);
	Field getCursorPosition();

private:
	struct Config {
		unsigned int throttleMilliseconds;
		bool inverseNavigation;
	} m_config;

	unsigned int m_tileCursor;
	std::chrono::time_point<std::chrono::system_clock> m_timeStart[4], m_timeNow[4];
	bool m_throttling[4];

	void checkTimeBetweenKeyStrokes(ArrowKey direction);
	void moveCursorVertical(int steps);
	void moveCursorHorizontal(int steps);
	void moveCursorUp();
	void moveCursorRight();
	void moveCursorDown();
	void moveCursorLeft();
};

using ArrowNavigationHandlerPtr = std::shared_ptr<ArrowNavigationHandler>;

#endif // ARROWNAVIGATIONHANDLER_H