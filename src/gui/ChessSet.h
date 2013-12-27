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

class ChessSet {
public:
	ChessSet();
	~ChessSet();

	void draw();
	void setState(std::array<Piece, 64> state);
	int getResourcesCount();

	// loads all the resources
	void loadResources();
	void registerLoadCallback(const boost::function<void(std::string)>& callback);

	enum ModelType {
		KING,
		PAWN,
		QUEEN,
		BISHOP,
		KNIGHT,
		ROOK
	};

	enum ModelColor {
		BLACK,
		WHITE
	};

	enum BoardTile {

	};

protected:
	int m_tileWidth, m_tileHeight;

private:
	// each figure is one time present in memory, so we are stupid here and only draw what we get with the bitboard
	ModelPtr king, pawn, queen, bishop, knight, rook;
	std::array<ModelPtr, 6> models;
	std::vector<std::string> extResources;

    struct CorrectionValue {
        int x, y, z;
        float scale;
        int rotX, rotY, rotZ;
    };

	std::array<CorrectionValue, 6> extCorrectionValues;	// 6 models a 7 parameters

	// OpenGL DisplayLists
	GLuint m_boardList;
	GLuint m_modelList[12];	// first 6 white, last 6 black
	std::array<Piece, 64> m_state;

	void createChessBoardList();
	void drawTile(int x, int y, int z, bool odd, bool highlight);
	void moveModelToTile(ModelPtr model, int row, int col);

	using Signal = boost::signals2::signal<void(std::string)>;
	Signal m_loadCallback;
};

using ChessSetPtr = std::shared_ptr<ChessSet>;

#endif // CHESSSET_H
