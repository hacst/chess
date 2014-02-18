#include "gui/ArrowNavigationHandler.h"

ArrowNavigationHandler::ArrowNavigationHandler(bool inverseNavigation) {

	m_config.inverseNavigation = inverseNavigation;
	m_config.throttleMilliseconds = 200;
	
	if (m_config.inverseNavigation) {
		m_tileCursor = 59;
	} else {
		m_tileCursor = 4;
	}

	m_throttling[ArrowKey::UP] = false;
	m_throttling[ArrowKey::RIGHT] = false;
	m_throttling[ArrowKey::DOWN] = false;
	m_throttling[ArrowKey::LEFT] = false;

	m_timeStart[ArrowKey::UP] = std::chrono::system_clock::now();
	m_timeStart[ArrowKey::RIGHT] = std::chrono::system_clock::now();
	m_timeStart[ArrowKey::DOWN] = std::chrono::system_clock::now();
	m_timeStart[ArrowKey::LEFT] = std::chrono::system_clock::now();
}

void ArrowNavigationHandler::checkTimeBetweenKeyStrokes(ArrowKey direction) {
	m_throttling[direction] = false;

	m_timeNow[direction] = std::chrono::system_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(m_timeNow[direction] - m_timeStart[direction]).count() <= m_config.throttleMilliseconds) {
		m_throttling[direction] = true;
	}
}

void ArrowNavigationHandler::onKey(ArrowKey direction) {
	checkTimeBetweenKeyStrokes(direction);
	if (m_throttling[direction])
		return;

	m_timeStart[direction] = std::chrono::system_clock::now();

	switch (direction) {
		case ArrowKey::UP:
			moveCursorUp();
			break;
		case ArrowKey::RIGHT:
			moveCursorRight();
			break;
		case ArrowKey::DOWN:
			moveCursorDown();
			break;
		case ArrowKey::LEFT:
			moveCursorLeft();
			break;
		default: break;
	}
}

void ArrowNavigationHandler::moveCursorVertical(int steps) {
	int newTileCursor = m_tileCursor + steps;

	// check boundary before moving the cursor
	if (newTileCursor >= 0 && newTileCursor <= 63) {
		m_tileCursor = newTileCursor;
	}
}

void ArrowNavigationHandler::moveCursorHorizontal(int steps) {
	int newTileCursor = m_tileCursor + steps;

	// check boundary before moving the cursor
	if (steps == -1) {
		// left key pressed
		switch (newTileCursor) {
			// all right side fields
			case 63:
			case 55:
			case 47:
			case 39:
			case 31:
			case 23:
			case 15:
			case 7:
			case -1:
				/* border corssed -> do nothing - keep old value */
				break;
			default:
				m_tileCursor = newTileCursor;
				break;
		}
	} else {
		// right key pressed
		switch (newTileCursor) {
			// all left side fields
			case 64:
			case 56:
			case 48:
			case 40:
			case 32:
			case 24:
			case 16:
			case 8:
				/* border corssed -> do nothing - keep old value */
				break;
			default:
				m_tileCursor = newTileCursor;
				break;
		}
	}
}

void ArrowNavigationHandler::moveCursorUp() {
	if (m_config.inverseNavigation) {
		moveCursorVertical(-8);
	}
	else {
		moveCursorVertical(8);
	}
}

void ArrowNavigationHandler::moveCursorRight() {
	if (m_config.inverseNavigation) {
		moveCursorHorizontal(-1);
	}
	else {
		moveCursorHorizontal(1);
	}
}

void ArrowNavigationHandler::moveCursorDown() {
	if (m_config.inverseNavigation) {
		moveCursorVertical(8);
	}
	else {
		moveCursorVertical(-8);
	}
}

void ArrowNavigationHandler::moveCursorLeft() {
	if (m_config.inverseNavigation) {
		moveCursorHorizontal(1);
	}
	else {
		moveCursorHorizontal(-1);
	}
}

Field ArrowNavigationHandler::getCursorPosition() {
	return static_cast<Field>(m_tileCursor);
}