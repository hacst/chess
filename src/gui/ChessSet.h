#ifndef CHESSSET_H
#define CHESSSET_H

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

#include <array>

#include "gui/AssimpHelper.h"
#include "gui/Model.h"

using namespace std;

class ChessSet {
public:
	ChessSet();
	void draw();
	void update(int bitboard);
	void makeDebug(int direction);

private:
	// each figure is one time present in memory, so we are stupid here and only draw what we get with the bitboard
	ModelPtr king, pawn, queen, bishop, knight, rook;

	// OpenGL DisplayLists
	GLuint m_boardList;
	GLuint m_modelsList;

	int m_tileWidth, m_tileHeight;

	int x, y, z;

	void createChessBoardList();
	void drawTile(int x, int y, int z, bool odd, bool highlight);
	void moveModelToTile(ModelPtr model, int row, int col);
};

using ChessSetPtr = std::shared_ptr<ChessSet>;

#endif // CHESSSET_H