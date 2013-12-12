#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "gui/interface/AbstractState.h"
#include "gui/Menu2D.h"
#include "gui/ChessSet.h"

class StateMachine;

class GamePlay : public AbstractState {
public:
	GamePlay();

	// interface methods
	void enter() override;
	AbstractState* run() override;
	void exit() override;

	// draw method
	void draw();
	
	void createChessSet();
	void onBeforeLoadNextResource(std::string resourceName);	// callback method for ChessSet

	// state methods
	void onBackToMenu();

private:
	StateMachine& fsm;

	enum States {
		KEEP_CURRENT,

		BACK_TO_MENU
	};
	States m_nextState;

	// smart pointers
	AnimationHelperPtr animationHelper;
	ChessSetPtr chessSet;

	GLfloat m_lightpos1[4], m_lightpos2[4], m_lightpos3[4], m_lightpos4[4];
	GLuint m_cube1, m_cube2, m_cube3, m_cube4;

	int m_rotateFrom, m_rotateTo;
	bool moveCamera;

	int m_resourcesTotal;
	int m_resourcesLoaded;

	// debug
	int cameraView;
	std::string debugText;
	void drawCoordinateSystem();
	GLfloat ambientLight[4];
	GLfloat diffuseLight[4];
	GLfloat specularLight[4];

	// methods
	void rotateCamera();
};

#endif // GAMEPLAY_H