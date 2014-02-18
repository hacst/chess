#ifndef CHESSSET_H
#define CHESSSET_H

#include <boost/signals2.hpp>
#include <array>
#include <vector>

#include "gui/AssimpHelper.h"
#include "gui/Model.h"
#include "logic/ChessTypes.h"
#include "logic/Turn.h"

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

/**
 * @brief The ChessSet holds all the figures together with the board needed for the chess game.
 */
class ChessSet {
public:
	ChessSet();
	~ChessSet();

	/**
	 * @brief Sets the new chess state.
	 * @note You need to call this only, when there's a visible change like moving a
	 *       figure from field A to field B.
	 * @param state The state of the current board. Only the given pieces on the fields
	 *              will be drawn.
	 */
	void setState(std::array<Piece, 64> state, PlayerColor lastPlayer, Turn lastTurn);

	enum TileStyle {
		NORMAL,
		CURSOR,
		MOVE,
		CASTLE
	};

	void drawActionTileAt(Field which, TileStyle style);

	/**
	 * @brief Returns the number of big resources which must be loaded for initializing
	 *        the ChessSet.
	 * @note This can be used for a progress bar.
	 * @return The number of big resources.
	 */
	int getResourcesCount();

	/**
	 * @brief Registers a function as callback.
	 * @param callback The function which will be called when a resource was
	 *        successfully loaded.
	 */
	void registerLoadCallback(const boost::function<void(std::string)>& callback);

	/**
	 * @brief Loads all resources, builds the models and the chess board.
	 * @note If you've registered a function as callback, you will be informed
	 *       on each resource which is loaded.
	 */
	void loadResources();

	/**
	* @brief Draws the whole ChessSet. This includes all models and the chess board.
	*        Depending in the current state.
	*/
	void draw();

private:
	int m_tileWidth, m_tileHeight;

	struct Coord3D {
		int x;
		int y;
		int z;
	};

	// each figure is one time present in memory, so we are stupid here and only draw what we get with the bitboard
	ModelPtr king, pawn, queen, bishop, knight, rook;
	std::array<ModelPtr, 6> m_models;
	std::vector<std::string> m_extResources;

    struct CorrectionValue {
        int x, y, z;
        float scale;
        int rotX, rotY, rotZ;
    };

	std::array<CorrectionValue, 6> m_extCorrectionValues;	// 6 models a 7 parameters

	// OpenGL DisplayLists
	GLuint m_boardList;				// 8x8 fields
	GLuint m_modelList[12];			// first 6 white, last 6 black
	GLuint m_modelsList;			// models translated and models itself
	std::array<Piece, 64> m_state;	// whole chess state

	unsigned int m_turnMoveShowDuration;
	unsigned int m_turnMoveShownSince;
	bool m_lastTurnAvailable;
	Turn m_lastTurn;
	PlayerColor m_lastPlayer;

	void createChessBoardList();
	void createModelsList(bool withoutTurnDependentModels);
	void animateModelTurn();
	void drawModels();
	void drawBoard();
	void drawTile(int x, int y, int z, bool odd, TileStyle style);
	void moveModelToTile(ModelPtr model, int row, int col);
	Coord3D ChessSet::calcCoordinatesForTileAt(Field which);

	using Signal = boost::signals2::signal<void(std::string)>;
	Signal m_loadCallback;
};

using ChessSetPtr = std::shared_ptr<ChessSet>;

#endif // CHESSSET_H
