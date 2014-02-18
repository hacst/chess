#include <iostream>
#include <SDL.h>

// ObsDisProx include Must be first to be before all windows.h
// includes in other headers to satisfy the angry gods
#include "logic/threading/ObserverDispatcherProxy.h"
#include "logic/threading/PlayerDispatcherProxy.h"

#include "gui/states/GamePlay.h"
#include "gui/GuiObserver.h"
#include "gui/Menu2DItem.h"
#include "gui/StateMachine.h"
#include "gui/AssimpHelper.h"
#include "gui/AnimationHelper.h"
#include "gui/ObjectHelper.h"
#include "gui/GuiWindow.h"
#include "gui/GUIPlayer.h"

#include "ai/AIPlayer.h"
#include "misc/DebugTools.h"

#include "core/Globals.h"

using namespace std;
using namespace Logging;

GamePlay::GamePlay(GameMode mode, PlayerColor humanPlayerColor)
	: m_fsm(StateMachine::getInstance())
	, m_gameMode(mode)
    , m_humanPlayerColor(humanPlayerColor)
    , m_nextState(States::KEEP_CURRENT)
    , m_log(initLogger("GUI:GamePlay"))
	, m_playerState(PlayerState::NONE)
	, m_lastPlayer(PlayerColor::NoPlayer)
	, m_lastTurn(Turn()) {
}

void GamePlay::initMessageBox() {
	m_messageBox.width = 600;
	m_messageBox.height = 40;
	m_messageBox.padding = 10;
	m_messageBox.text = "";
	m_messageBox.showDuration = global_config.timeBetweenTurnsInSeconds * 1000;	// this should be the same as timeBetweenTurnsInSeconds of the GameConfiguration

	// precalculate absolute position
	m_messageBox.windowPosX = (m_fsm.window->getWidth() / 2) - (m_messageBox.width / 2);
	m_messageBox.windowPosY = 10;

	// create rectangle OGL list for faster drawing
	m_fsm.window->set2DMode();

	m_messageBox.displayList = ObjectHelper::create2DGradientRectList(
									static_cast<float>(m_messageBox.width), static_cast<float>(m_messageBox.height),
									static_cast<float>(m_messageBox.windowPosX), static_cast<float>(m_messageBox.windowPosY),
									0.0f, 0.0f, 0.4f,
									0.0f, 0.0f, 0.3f
	);
}

void GamePlay::resetCapturedPieces() {
	for (int i = 0; i < 6; ++i) {
		m_capturedPieces.countBlack[i] = 0;
		m_capturedPieces.countWhite[i] = 0;
	}
}

void GamePlay::initCapturedPieces() {
	resetCapturedPieces();

	m_capturedPieces.blackBar = ObjectHelper::create2DRectList(150.0f, 20.0f, 10.0f, 10.0f, 1.0f, 1.0f, 1.0f);
	m_capturedPieces.whiteBar = ObjectHelper::create2DRectList(150.0f, 20.0f, m_fsm.window->getWidth() - 160.0f, 10.0f, 0.0f, 0.0f, 0.0f);
}

void GamePlay::enter() {
	// set background color to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	initChessSet();
	initMenuPause();
	initAnimationHelpers();
	initLighting();
	initMessageBox();
	initCapturedPieces();
	initCamera();	// init camera always after chessSet, otherwise it causes some trouble when placing the models.

	// connect gui with ai and logic
	initPlayers();
	initGameLogic();
}

void GamePlay::initMenuPause() {
	m_pauseMenu = make_shared<Menu2D>(m_fsm.window->getWidth(), m_fsm.window->getHeight());
	m_pauseMenu->addButton("ResumeGame.png")->onClick(boost::bind(&GamePlay::onResumeGame, this));

	// saving a game is only available in Player vs AI mode.
	if (m_gameMode == PLAYER_VS_AI) {
		m_pauseMenu->addButton("SaveGame.png")->onClick(boost::bind(&GamePlay::onSaveGame, this));
	}
	
	m_pauseMenu->addButton("BackToMainMenu.png")->onClick(boost::bind(&GamePlay::onLeaveGame, this));
}

void GamePlay::initPlayers() {
	if (m_gameMode == PLAYER_VS_AI) {
		m_kCounter.keyReturn = std::chrono::system_clock::now();

		// Player vs. AI
		LOG(info) << "Starting Player vs. AI game with seed: " << global_seed;

        AIConfiguration aicfg = global_config.ai[global_config.aiSelected];
        auto firstPlayer = make_shared<AIPlayer>(aicfg, "AIPlayer", global_seed);
		firstPlayer->start();
		m_firstPlayer = firstPlayer;

		auto secondPlayer = make_shared<GUIPlayer>(*this);
		m_secondPlayer = secondPlayer;

		m_playerProxy = make_shared<PlayerDispatcherProxy>(m_secondPlayer);

		// white color begins the match, so it depends on the color to
		// set the correct internal game state
		if (m_humanPlayerColor == PlayerColor::Black) {
			// AI is first player -> AI starts the game
			m_internalState = AI_ON_TURN;
			m_arrowNavHandler = make_shared<ArrowNavigationHandler>(true);
		} else {
			m_internalState = PLAYER_ON_TURN;
			m_arrowNavHandler = make_shared<ArrowNavigationHandler>(false);
		}
	} else if (m_gameMode == AI_VS_AI) {
		m_kCounter.keyR = std::chrono::system_clock::now();

		// AI vs. AI
        LOG(info) << "Starting AI vs. AI game with seed: " << global_seed;

        AIConfiguration aicfg = global_config.ai[global_config.aiSelected];
        auto firstPlayer = make_shared<AIPlayer>(aicfg, "AIPlayer (white)", global_seed);
		firstPlayer->start();
		m_firstPlayer = firstPlayer;

        auto secondPlayer = make_shared<AIPlayer>(aicfg, "AIPlayer (black)", global_seed + 1);
		secondPlayer->start();
		m_secondPlayer = secondPlayer;

		m_internalState = AI_ON_TURN;
	}
    
    // Make sure we don't re-use the seed for the next game.
    global_seed += 2; 
}

void GamePlay::initGameLogic() {
	GameConfigurationPtr config = make_shared<GameConfiguration>(global_config);

    GameState initialGameState(ChessBoard::fromFEN(config->initialGameStateFEN)); // FIXME: fromFEN isn't robust

	if (m_humanPlayerColor == PlayerColor::White) {
		m_gameLogic = make_shared<GameLogic>(m_secondPlayer /* White */, m_firstPlayer /* Black */, config, initialGameState);
	} else{
		m_gameLogic = make_shared<GameLogic>(m_firstPlayer /* White */, m_secondPlayer /* Black */, config, initialGameState);
	}
	m_observer = make_shared<GuiObserver>(m_chessSet, *this);

	m_observerProxy = make_shared<ObserverDispatcherProxy>(m_observer);
	m_gameLogic->addObserver(m_observerProxy);

	m_gameLogic->start();
}

void GamePlay::initCamera() {
	if (m_gameMode == AI_VS_AI) {
		m_rotateFrom = 180;
		m_rotateTo = 0;
		setCameraPosition(180.0f);

		m_lockCamera = false;
	}

	// we lock the camera, if the mode is Player vs. AI
	if (m_gameMode == PLAYER_VS_AI) {
		if (m_humanPlayerColor == PlayerColor::White) {
			setCameraPosition(0.0f);
		} else {
			setCameraPosition(180.0f);
		}

		m_lockCamera = true;
	}

	// no camera rotation on first turn
	m_firstTurn = true;
}

// create a whole new ChessSet (2x6 models + 1 board)
void GamePlay::initChessSet() {
	m_chessSet = make_shared<ChessSet>();

	m_resourcesLoaded = 0;
	m_resourcesTotal = m_chessSet->getResourcesCount();

	m_chessSet->registerLoadCallback(boost::bind(&GamePlay::onBeforeLoadNextResource, this, _1));
	m_chessSet->loadResources();
}

// creates new animation helpers for camera movement and background fading
void GamePlay::initAnimationHelpers() {
	m_animationHelperCamera = make_shared<AnimationHelper>(1000);
	m_animationHelperBackground = make_shared<AnimationHelper>(1000);
}

void GamePlay::onBeforeLoadNextResource(string resourceName) {
	// swap the frame buffer for the first time to
	if (m_resourcesLoaded == 0) {
		m_fsm.window->swapFrameBufferNow();
	}

	// print what is loaded (resource name + progress bar)
	++m_resourcesLoaded;

	int windowWidth = m_fsm.window->getWidth();
	float percentLoaded = m_resourcesLoaded / static_cast<float>(m_resourcesTotal);

	array<float, 2> topLeftVertex		= { 0.0f, 0.0f	};
	array<float, 2> bottomLeftVertex	= { 0.0f, 10.0f };
	array<float, 2> bottomRightVertex	= { windowWidth * percentLoaded, 10.0f };
	array<float, 2> topRightVertex		= { windowWidth * percentLoaded, 0.0f };

	m_fsm.window->set2DMode();
	
	glPushMatrix();
		glBegin(GL_QUADS);
			// left side is grey
			glColor3f(0.6f, 0.6f, 0.6f);
			glVertex3f(topLeftVertex[0], topLeftVertex[1], -0.1f);
			glVertex3f(bottomLeftVertex[0], bottomLeftVertex[1], -0.1f);

			// right side is white
            glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(bottomRightVertex[0], bottomRightVertex[1], -0.1f);
			glVertex3f(topRightVertex[0], topRightVertex[1], -0.1f);
		glEnd();
	glPopMatrix();
	
	m_fsm.window->printText(10, m_fsm.window->getHeight() - 30, 1.0, 1.0, 1.0, resourceName + " (" + to_string(m_resourcesLoaded) + " of " + to_string(m_resourcesTotal) + ")");

	// we must now swap the frame buffer
	m_fsm.window->swapFrameBufferNow();

	m_fsm.window->set3DMode();
}

void GamePlay::onPauseGame() {
	m_lastInternalState = m_internalState;
	m_internalState = PAUSE;
}

void GamePlay::onResumeGame() {
	m_internalState = m_lastInternalState;
	m_pauseMenu->resetAnimation();
}

void GamePlay::onSaveGame() {
	m_internalState = SAVE_GAME;
}

void GamePlay::onLeaveGame() {
	m_firstPlayer->doAbortTurn();
	m_secondPlayer->doAbortTurn();

	m_nextState = BACK_TO_MENU;
}

void GamePlay::onPlayerIsOnTurn(PlayerColor who) {
	// internal state changes only if the human player is on turn
	if (m_humanPlayerColor == who) {
		m_internalState = PLAYER_ON_TURN;
	} else {
		m_internalState = AI_ON_TURN;
	}
}

std::future<Turn> GamePlay::doMakePlayerTurn() {
	m_promisedPlayerTurn = promise<Turn>();

	return m_promisedPlayerTurn.get_future();
}

void GamePlay::onPlayerAbortTurn() {
	m_internalState = AI_ON_TURN;
}

AbstractState* GamePlay::run() {
	handleEvents();
	
	// Execute all pending calls from the observer and player
	m_observerProxy->poll();
	m_playerProxy->poll();

	this->draw();

	AbstractState* nextState;
	switch (m_nextState) {
	case BACK_TO_MENU:
		nextState = new MenuMain();
		break;
	case KEEP_CURRENT:
		nextState = this;
		break;
	default:
		nextState = this;
		break;
	}

	return nextState;
}

// this light source has an effect like a desk lamp and is in the middle of the chessboard, the lighting direction is downwards
void GamePlay::initLighting() {
	m_lightPos0[0] = 0.0f;
	m_lightPos0[1] = 65.0f;
	m_lightPos0[2] = -50.0f;

	m_lightPos1[0] = 0.0f;
	m_lightPos1[1] = 65.0f;
	m_lightPos1[2] = 50.0f;

	m_fsm.window->set3DMode();

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	// === global lighting model configuration ===
	GLfloat globalAmbientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);

	GLfloat mode = GLfloat(GL_TRUE);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, &mode);
	
	// === local ligthing source configuration ===
	GLfloat ambient[] = { 1.0f, 0.94f, 0.68f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);

	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);

	GLfloat specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT0, GL_POSITION, m_lightPos0);
	glLightfv(GL_LIGHT1, GL_POSITION, m_lightPos1);

	GLfloat direction0[] = { 0.0, 0.0, -1.0 };
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction0);
	GLfloat direction1[] = { 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction1);

	GLfloat angle[] = { 180.0 };
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, angle);
	glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, angle);

	GLfloat exponent[] = { 1.0 };
	glLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, exponent);
	glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, exponent);
}

void GamePlay::enableLighting() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}

void GamePlay::disableLighting() {
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
}

void GamePlay::draw() {
	enableLighting();
	fadeBackgroundForOneTime();
	
	// 3D
	draw3D();

	// 2D
	draw2D();
}

void GamePlay::draw3D() {
	m_fsm.window->set3DMode();
	m_chessSet->draw();	// chessboard and models

	// rotate the camera
	if (m_gameMode == AI_VS_AI) {
		rotateCamera();
	}

	if (m_internalState == PLAYER_ON_TURN) {
		drawPlayerActions();
	}
}

void GamePlay::draw2D() {
	m_fsm.window->set2DMode();
	disableLighting();

	// draw menu if game is paused
	if (m_internalState == PAUSE) {
		drawPauseMenu();
	}

	// draw last turns, message box and captured pieces only if we're not in pause mode
	if (m_internalState != PAUSE) {
		drawMessageBox();
		drawLastTurns();
		drawCapturedPieces();

		if (m_gameMode == AI_VS_AI) {
			string msg;
			if (m_lockCamera) {
				msg = "Rotation ist deaktiviert.";
			}
			else {
				msg = "";
			}

			drawInfoBox(msg);
		}
	}

	if (m_playerState == PlayerState::CHOOSE_PROMOTION_TURN) {
		drawInfoBox("1: Läufer, 2: Springer, 3: Dame, 4: Turm");
	}
}

void GamePlay::handleEvents() {
	// enable/disable camera rotation
	if (m_gameMode == AI_VS_AI) {
		if (m_fsm.eventmap.keyR &&
				std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_kCounter.keyR).count() > 500) {
			m_lockCamera = !m_lockCamera;
			m_kCounter.keyR = std::chrono::system_clock::now();
		}
	}

	if (m_fsm.eventmap.key0) {
		LOG(info) << m_gameState << endl;
	}

	if (m_internalState == PAUSE) {
		if (m_fsm.eventmap.mouseMoved) {
			m_pauseMenu->mouseMoved(m_fsm.eventmap.mouseX, m_fsm.eventmap.mouseY);
		}

		if (m_fsm.eventmap.mouseDown) {
			m_pauseMenu->mousePressed();
		}

		if (m_fsm.eventmap.mouseUp) {
			m_pauseMenu->mouseReleased();
		}
	}

	if (m_internalState != PAUSE) {
		if (m_fsm.eventmap.keyEscape) {
			onPauseGame();
		}
	}

	if (m_internalState == PLAYER_ON_TURN) {
		
		if (m_playerState == PlayerState::CHOOSE_PROMOTION_TURN) {
			if (m_fsm.eventmap.key1) {
				m_promisedPlayerTurn.set_value(m_promotionTurns[Turn::Action::PromotionBishop]);
				m_playerState = PlayerState::NONE;
			} else if (m_fsm.eventmap.key2) {
				m_promisedPlayerTurn.set_value(m_promotionTurns[Turn::Action::PromotionKnight]);
				m_playerState = PlayerState::NONE;
			} else if (m_fsm.eventmap.key3) {
				m_promisedPlayerTurn.set_value(m_promotionTurns[Turn::Action::PromotionQueen]);
				m_playerState = PlayerState::NONE;
			} else if (m_fsm.eventmap.key4) {
				m_promisedPlayerTurn.set_value(m_promotionTurns[Turn::Action::PromotionRook]);
				m_playerState = PlayerState::NONE;
			}
		}

		if (m_fsm.eventmap.keyUp) {
			m_arrowNavHandler->onKey(ArrowNavigationHandler::ArrowKey::UP);
		}

		if (m_fsm.eventmap.keyRight) {
			m_arrowNavHandler->onKey(ArrowNavigationHandler::ArrowKey::RIGHT);
		}

		if (m_fsm.eventmap.keyDown) {
			m_arrowNavHandler->onKey(ArrowNavigationHandler::ArrowKey::DOWN);
		}

		if (m_fsm.eventmap.keyLeft) {
			m_arrowNavHandler->onKey(ArrowNavigationHandler::ArrowKey::LEFT);
		}

		// get all relevant turns for the chosen field
		if (m_fsm.eventmap.keyReturn &&
				std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_kCounter.keyReturn).count() > 500) {
			m_kCounter.keyReturn = std::chrono::system_clock::now();

			// 2. User *may* select one option as target field for the in (1) (see below) selected model
			if (m_possibleTurns.size() > 0) {
				// a) check if the user will do a turn
				for (auto& turn : m_possibleTurns) {
					if (turn.to == m_arrowNavHandler->getCursorPosition()) {
						// check for promotion turns
						if (turn.action == Turn::Action::PromotionBishop || turn.action == Turn::Action::PromotionKnight ||
								turn.action == Turn::Action::PromotionQueen || turn.action == Turn::Action::PromotionRook) {
							// do nothing here, just switch the state and let the user choose
							// one of the four turns
							m_promotionTurns[turn.action] = turn;
							m_playerState = PlayerState::CHOOSE_PROMOTION_TURN;
						} else {
							// this is a normal Turn
							m_promisedPlayerTurn.set_value(turn);
						}
						
						// assert: clear now all possible turns
					}
				}

				// b) if the user will not do a turn, reset possible turn list
				m_possibleTurns.clear();
			}

			// 1. User selects one model on the board by choosing a field
			if (m_possibleTurns.size() == 0) {
				for (auto& turn : m_gameState.getTurnList()) {
					if (turn.from == m_arrowNavHandler->getCursorPosition()) {
						m_possibleTurns.push_back(turn);
					}
				}
			}
		}
	}
}

// must be done in 2D mode
void GamePlay::drawMessageBox() {
	if (m_messageBox.text == "")
		return;
	
	// show only if the show time is not over
	/*if (m_messageBox.shownSince < (SDL_GetTicks() - m_messageBox.showDuration)) {
		m_messageBox.text = "";
		return;
	}*/

	glCallList(m_messageBox.displayList);

	m_fsm.window->printText(
		m_messageBox.windowPosX + m_messageBox.padding,
		m_messageBox.windowPosY + m_messageBox.padding,
		1.0f, 1.0f, 1.0f, 
		m_messageBox.text
	);
}

/*
void GamePlay::addTurn(PlayerColor who, Turn turn) {
	PlayerTurn pt;
	pt.who = who;
	pt.turn = turn;
	
	m_playerTurns.push_front(pt);
	m_lastTurn = turn;
}*/

void GamePlay::setCapturedPiecesList(std::vector<Piece> piecesList) {
	resetCapturedPieces();

	for (auto &cp : piecesList) {
		if (cp.player == PlayerColor::Black) {
			++m_capturedPieces.countBlack[cp.type];
		}

		if (cp.player == PlayerColor::White) {
			++m_capturedPieces.countWhite[cp.type];
		}
	}
}

void GamePlay::setState(std::array<Piece, 64> state, PlayerColor lastPlayer, Turn lastTurn) {
	m_lastTurn = lastTurn;
	m_lastPlayer = lastPlayer;

	PlayerTurn pt;
	pt.who = m_lastPlayer;
	pt.turn = m_lastTurn;
	m_playerTurns.push_front(pt);

	setState(state);
}

void GamePlay::setGameState(const GameState& gameState) {
    m_gameState = gameState;
}

void GamePlay::setState(std::array<Piece, 64> state) {
	m_chessBoardState = state;
	m_chessSet->setState(state, m_lastPlayer, m_lastTurn);
}

// must be done in 2D mode
void GamePlay::drawLastTurns() {
	// config
	int fontSize = m_fsm.window->fontSize::TEXT_SMALL;
	int numberOfTurnsToDraw = 5;
	
	// precalculations
	int lineHeight = fontSize + 4;
	int totalLineHeight = numberOfTurnsToDraw * lineHeight;
	int offsetY = m_fsm.window->getHeight() - totalLineHeight - fontSize;

	int step = 0;
	for (auto& turn : m_playerTurns) {
		if (step == numberOfTurnsToDraw)
			return;

		int relativeOffsetY = step * lineHeight;

		string turnStr = (turn.who == PlayerColor::White ? "Weiss: " : "Schwarz: ");
		turnStr += turn.turn.toString();

		m_fsm.window->printTextSmall(10, offsetY + relativeOffsetY, 1.0f, 1.0f, 1.0f, turnStr);

		++step;
	}
}

// must be done in 2D mode
void GamePlay::drawInfoBox(string msg) {
	// config
	int fontSize = m_fsm.window->fontSize::TEXT_SMALL;

	// precalculations
	int lineHeight = fontSize + 4;
	int totalLineHeight = 1 * lineHeight;
	int offsetY = m_fsm.window->getHeight() - totalLineHeight - fontSize;

	m_fsm.window->printTextSmall(m_fsm.window->getWidth() - 400, offsetY, 0.6f, 0.0f, 0.0f, msg);
}

void GamePlay::drawCapturedPieces() {
	// config
	int fontSize = m_fsm.window->fontSize::TEXT_SMALL;

	// precalculations
	int lineHeight = fontSize + 4;
	int offsetY = 40;

	// *** white: left side ***
	glCallList(m_capturedPieces.whiteBar);

	int offsetX = 10;
	for (int i = 0; i < 6; ++i) {
		int relativeOffsetY = i * lineHeight;

		stringstream strs;
		strs << m_capturedPieces.countWhite[i] << " " << getPieceName(i);

		m_fsm.window->printTextSmall(offsetX, offsetY + relativeOffsetY, 1.0f, 1.0f, 1.0f, strs.str());
	}

	// *** black: right side ***
	glCallList(m_capturedPieces.blackBar);

	offsetX = m_fsm.window->getWidth() - 100;
	for (int i = 0; i < 6; ++i) {
		int relativeOffsetY = i * lineHeight;

		stringstream strs;
		strs << m_capturedPieces.countWhite[i] << " " << getPieceName(i);

		m_fsm.window->printTextSmall(offsetX, offsetY + relativeOffsetY, 1.0f, 1.0f, 1.0f, strs.str());
	}
}

string GamePlay::getPieceName(int pieceNumber) {
	string pieceName;

	switch (pieceNumber) {
		case PieceType::Bishop:
			pieceName = "Laeufer";
			break;
		case PieceType::King:
			pieceName = "Koenig";
			break;
		case PieceType::Knight:
			pieceName = "Springer";
			break;
		case PieceType::Pawn:
			pieceName = "Bauer";
			break;
		case PieceType::Queen:
			pieceName = "Dame";
			break;
		case PieceType::Rook:
			pieceName = "Turm";
			break;
	}

	return pieceName;
}

void GamePlay::drawPlayerActions() {
	if (m_possibleTurns.size() > 0) {
		for (auto& turn : m_possibleTurns) {
			switch (turn.action) {
			case Turn::Action::Castle:
				m_chessSet->drawActionTileAt(static_cast<Field>(turn.to), ChessSet::TileStyle::CASTLE);
				break;
			case Turn::Action::Forfeit:
				// nothing to do here
				break;
			case Turn::Action::Move:
				m_chessSet->drawActionTileAt(static_cast<Field>(turn.to), ChessSet::TileStyle::MOVE);
				break;
			case Turn::Action::Pass:
				// just for testing, nothing to do
				break;
			case Turn::Action::PromotionBishop:
			case Turn::Action::PromotionKnight:
			case Turn::Action::PromotionQueen:
			case Turn::Action::PromotionRook:
				// nothing to do here as we are not showing information when the
				// user can make a promotion. We show instead an infomation box
				// with keys (1-4) to let the user choose a promotion action *after*
				// the move was made through the user. This is more intuitive.
				break;
			default:
				break;
			}
		}
	}

	m_chessSet->drawActionTileAt(static_cast<Field>(m_arrowNavHandler->getCursorPosition()), ChessSet::TileStyle::CURSOR);
}

void GamePlay::drawPauseMenu() {
	// modal dialog with transparent background
	glEnable(GL_COLOR);
	glEnable(GL_BLEND);
	glPushMatrix();
		glBegin(GL_QUADS);
			glColor4f(0.0f, 0.0f, 0.0f, 0.25f);

			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(static_cast<float>(m_fsm.window->getWidth()), 0.0f, 0.0f);
			glVertex3f(static_cast<float>(m_fsm.window->getWidth()), static_cast<float>(m_fsm.window->getHeight()), 0.0f);
			glVertex3f(0.0f, static_cast<float>(m_fsm.window->getHeight()), 0.0f);
		glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_COLOR);

	m_fsm.window->printHeadline("M E N U");
	m_pauseMenu->draw();
}

void GamePlay::fadeBackgroundForOneTime() {
	m_animationHelperBackground->setStartNowOrKeepIt();

	// set background color to cool lime and fade in
	glClearColor(
		m_animationHelperBackground->ease(AnimationHelper::EASE_LINEAR, 0.0f, 0.47f),
		m_animationHelperBackground->ease(AnimationHelper::EASE_LINEAR, 0.0f, 0.64f),
		0.0,
		1.0
	);
}

void GamePlay::rotateCamera() {
	m_animationHelperCamera->setStartNowOrKeepIt();

	if (m_animationHelperCamera->hasStopped()) {
		return;
	}

	float degree = m_animationHelperCamera->ease(AnimationHelper::EASE_OUTSINE, static_cast<float>(m_rotateFrom), static_cast<float>(m_rotateTo));
	setCameraPosition(degree);
}

void GamePlay::setCameraPosition(float degree) {
	float angleRadian = degree * ((float)M_PI / 180.0f);

	float newCameraX = sinf(angleRadian) * m_fsm.window->getCameraDistanceToOrigin();
	float newCameraZ = cosf(angleRadian) * m_fsm.window->getCameraDistanceToOrigin();
	float rotationY = (atan2f(newCameraX, -newCameraZ) * 180.0f / (float)M_PI) - 180.0f;

	m_fsm.window->m_cameraAngleY = rotationY;
	m_fsm.window->m_cX = newCameraX;
	m_fsm.window->m_cZ = newCameraZ;
}

void GamePlay::startCameraRotation() {
	// only stop the reset of the rotation to smoothly end
	// the current rotation
	if (!m_lockCamera) {
		m_animationHelperCamera->reset();
	}

	// also set the coordinates to the opposite
	// warning: this must be done even if the rotation
	// is stopped! If not, the rotation may be out of sync with
	// the player color.
	m_rotateFrom = (m_rotateFrom + 180) % 360;
	m_rotateTo = (m_rotateFrom + 180) % 360;
}

void GamePlay::startShowText(std::string text) {
	m_messageBox.text = text;
	m_messageBox.shownSince = SDL_GetTicks();
}

void GamePlay::switchToPlayerColor(PlayerColor color) {
	if (m_gameMode == PLAYER_VS_AI) {
		// do nothing
	} else if (m_gameMode == AI_VS_AI && !m_firstTurn) {
		startCameraRotation();
	}

	string colorStr = (color == PlayerColor::White ? "Weiss" : "Schwarz");
	startShowText(colorStr + " ist jetzt an der Reihe.");

	m_firstTurn = false; // camera should rotate with the next turn
}

void GamePlay::onBackToMenu() {
    LOG(info) << "Returning to main menu";
	m_nextState = BACK_TO_MENU;
}

void GamePlay::exit() {
    LOG(info) << "Left GamePlay!";
}
