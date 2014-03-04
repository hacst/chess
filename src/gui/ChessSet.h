/*
    Copyright (c) 2013-2014, Patrick Hillert <silent@gmx.biz>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CHESSSET_H
#define CHESSSET_H

#include <boost/signals2.hpp>
#include <array>
#include <vector>

#include "gui/AssimpHelper.h"
#include "gui/AnimationHelper.h"
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
    unsigned int m_turnMoveShowDuration;
    unsigned int m_turnMoveShownSince;

    enum InternalState {
        ANIMATING,
        STATIC
    } m_internalState;

    float m_animationElevationHeight, m_animationElevationStrikeHeight;
    bool m_firstRun;

    struct Coord3D {
        float x;
        float y;
        float z;
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
    std::array<Piece, 64> m_state, m_lastState;	// whole chess state

    struct AnimationCapsule {
        Piece piece;
        Field field;
        Turn turn;
    };
    std::vector<AnimationCapsule> m_animationCapsules;
    AnimationHelperPtr m_animationHelperModelX, m_animationHelperModelY, m_animationHelperModelZ, m_animationHelperModelStrike;
    enum Elevation {
        UP,
        DOWN
    };
    Elevation m_animationDirectionY;


    struct StrikedModel {
        Piece piece;
        Field field;
    };
    std::vector<StrikedModel> m_modelStrikes;


    Turn m_lastTurn;
    PlayerColor m_lastPlayer;

    void createChessBoardList();
    void createModelsList(bool withoutTurnDependentModels);
    void drawModelAt(Field field, PieceType type, PlayerColor color);
    void drawModelAt(Coord3D coords, PieceType type, PlayerColor color);
    void animateModelTurn();
    void drawModels();
    void drawBoard();
    void drawTile(Coord3D coords, bool odd, TileStyle style);
    void moveModelToTile(ModelPtr model, int row, int col);
    Coord3D calcCoordinatesForTileAt(Field which);

    void animateModelStrike(Coord3D coords, Piece piece);
    void animateModelTurn(Coord3D coords, AnimationCapsule animCapsule);

    using Signal = boost::signals2::signal<void(std::string)>;
    Signal m_loadCallback;
};

using ChessSetPtr = std::shared_ptr<ChessSet>;

#endif // CHESSSET_H
