#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "logic/GameLogic.h"
#include "gui/GuiObserver.h"

#include "gui/interface/AbstractState.h"
#include "gui/Menu2D.h"
#include "gui/ChessSet.h"

class StateMachine;
class ObserverDispatcherProxy;
using ObserverDispatcherProxyPtr = std::shared_ptr<ObserverDispatcherProxy>;

class GamePlay : public AbstractState {
public:
	GamePlay();

	// interface methods
	void enter() override;
	AbstractState* run() override;
	void exit() override;

	// draw method
	void draw();
	void startCameraRotation();

	// events and callbacks
	void onBeforeLoadNextResource(std::string resourceName);	// callback method for ChessSet
	void onResumeGame();
	void onSaveGame();
	void onLeaveGame();

	// state methods
	void onBackToMenu();

private:
	StateMachine& fsm;

	enum States {
		KEEP_CURRENT,

		BACK_TO_MENU
	} m_nextState;

	enum InternalState {
		NOT_PAUSED,
		PAUSED
	} m_internalState;

	// smart pointers
	AnimationHelperPtr m_animationHelperCamera, m_animationHelperBackground;
	ChessSetPtr m_chessSet;
	Menu2DPtr m_pauseMenu;

	GLfloat m_lightpos0[4], m_lightpos1[4], m_lightpos2[4], m_lightpos3[4];
	GLuint m_cube1, m_cube2, m_cube3, m_cube4;

	int m_rotateFrom, m_rotateTo;

	int m_resourcesTotal;
	int m_resourcesLoaded;

	AbstractPlayerPtr m_firstPlayer, m_secondPlayer;
	AbstractGameLogicPtr m_gameLogic;
	GuiObserverPtr m_observer;
	ObserverDispatcherProxyPtr m_observerProxy;

	// debug
	int cameraView;
	std::string debugText;
	void drawCoordinateSystem();
	GLfloat ambientLight[4];
	GLfloat diffuseLight[4];
	GLfloat specularLight[4];
	GLfloat angle[1];
	GLfloat exponent[1];

	// methods
	void fadeBackgroundForOneTime();
	void rotateCamera();
	void setLights();
	void createChessSet();
};

using GamePlayPtr = std::shared_ptr<GamePlay>;

#endif // GAMEPLAY_H