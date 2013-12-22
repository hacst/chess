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
	void startShowText(std::string text);

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

	int m_rotateFrom, m_rotateTo;

	struct MessageBox {
		unsigned int width;
		unsigned int height;
		unsigned int padding;
		unsigned int showDuration;
		std::string text;

		unsigned int shownSince;
		unsigned int windowPosX;
		unsigned int windowPosY;

		GLuint displayList;
	} m_messageBox;

	int m_resourcesTotal;
	int m_resourcesLoaded;
	GLfloat m_lightPos0[3];
	GLfloat m_lightPos1[3];

	AbstractPlayerPtr m_firstPlayer, m_secondPlayer;
	AbstractGameLogicPtr m_gameLogic;
	GuiObserverPtr m_observer;
	ObserverDispatcherProxyPtr m_observerProxy;

	// debug
	int cameraView;
	std::string debugText;
	void drawCoordinateSystem();
	
	int posNumber;
	
	// methods
	void initMessageBox();
	void fadeBackgroundForOneTime();
	void rotateCamera();
	void drawMessageBox();
	void initLighting();
	void createChessSet();
	void enableLighting();
	void disableLighting();
};

using GamePlayPtr = std::shared_ptr<GamePlay>;

#endif // GAMEPLAY_H