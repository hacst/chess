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
    /**
     * @brief Creates a new chess set.
     */
    ChessSet();

    ~ChessSet();

    /**
     * @brief Sets the new chess state.
     * @note You need to call this only, when there's a visible change like moving a
     * figure from field A to field B.
     * @param state The state of the current board. Only the given pieces on the fields
     * will be drawn.
     */
    void setState(std::array<Piece, 64> state, PlayerColor lastPlayer, Turn lastTurn);

    //! The style type for a tile.
    enum TileStyle {
        NORMAL,
        CURSOR,
        MOVE,
        CASTLE
    };

    /**
     * @brief Draws an action tile at a given field with a given style.
     * @param which The field where the tile should be drawn.
     * @param style The action tile's style.
     */
    void drawActionTileAt(Field which, TileStyle style);

    /**
     * @brief Returns the number of big resources which must be loaded for initializing
     * the ChessSet.
     * @note This can be used for a progress bar.
     * @return The number of big resources.
     */
    int getResourcesCount();

    /**
     * @brief Registers a function as callback.
     * @param callback The function which will be called when a resource was
     * successfully loaded.
     */
    void registerLoadCallback(const boost::function<void(std::string)>& callback);

    /**
     * @brief Loads all resources, builds the models and the chess board.
     * @note If you've registered a function as callback, you will be informed
     * on each resource which is loaded.
     */
    void loadResources();

    /**
     * @brief Draws the whole ChessSet. This includes all models and the chess board.
     * Depending in the current state.
     */
    void draw();

private:
    //! The proportions of the tiles.
    int m_tileWidth, m_tileHeight;
    //! The duration for the animation of a turn. 
    unsigned int m_turnMoveShowDuration;
    //! The timestamp of when the animation was started.
    unsigned int m_turnMoveShownSince;

    //! Internal states
    enum InternalState {
        ANIMATING,
        STATIC
    } m_internalState;

    //! The maximum height to lift a model when animating.
    float m_animationElevationHeight, m_animationElevationStrikeHeight;

    //! Flag to set if this is the first run.
    bool m_firstRun;

    //! Simple 3D Coord container.
    struct Coord3D {
        float x;
        float y;
        float z;
    };

    //! Simple (smart pointer) model array which holds the references to the six model types.
    std::array<ModelPtr, 6> m_models;
    //! Vector which hold the resources for all external resources (3D models).
    std::vector<std::string> m_extResources;

    //! Coordination-correction container to statically adjust unproper 3D models.
    struct CorrectionValue {
        int x, y, z;
        float scale;
        int rotX, rotY, rotZ;
    };

    //! Simple array with correction values for each 3D model.
    std::array<CorrectionValue, 6> m_extCorrectionValues;

    //! OpenGL display list index to the 3D board representation. 8x8 fields.
    GLuint m_boardList;
    //! OpenGL display lists indexes to the 3D models representation for black and white color.
    //! First 6 white, last 6 black.
    GLuint m_modelList[12];
    //! OpenGL display list index to all 3D models and translation combined. Models 
    //! translated and models itself.
    GLuint m_modelsList;

    //! Whole chess state
    std::array<Piece, 64> m_state, m_lastState;

    //! Struct for turn animation.
    struct AnimationCapsule {
        Piece piece;
        Field field;
        Turn turn;
    };

    //! One or more turn animation capsules.
    std::vector<AnimationCapsule> m_animationCapsules;

    //! Animation helper objects.
    AnimationHelperPtr m_animationHelperModelX, m_animationHelperModelY, m_animationHelperModelZ, m_animationHelperModelStrike;

    //! The elevation direction.
    enum Elevation {
        UP,
        DOWN
    } m_animationDirectionY;

    //! Striked model for animation.
    struct StrikedModel {
        Piece piece;
        Field field;
    };

    //! Striked model container.
    std::vector<StrikedModel> m_modelStrikes;

    //! Last player turn.
    Turn m_lastTurn;
    //! Last player color.
    PlayerColor m_lastPlayer;

    /**
     * @brief Creates a new chess board as OpenGL display list.
     */
    void createChessBoardList();

    /**
     * @brief Creates a OpenGL display list with all models at the positions
     * as saved in the board state.
     * @param withoutTurnDependentModels If true all models are positioned in
     * the display list without those models on the start and destination field
     * of the current turn. If false, all models are precompiled in the OGL list.
     */
    void createModelsList(bool withoutTurnDependentModels);

    /**
     * @brief Draws a precompiled OpenGL display list model at the given field.
     * @param field The field where the model will be drawn.
     * @param type The model type to draw.
     * @param color The color of the model.
     */
    void drawModelAt(Field field, PieceType type, PlayerColor color);

    /**
     * @brief Draws a precompiled OpenGL display list model at the given coordinates.
     * @note This is used for animation purposes only.
     * @param coords The 3D coordinates where the model will be drawn.
     * @param type The model type to draw.
     * @param color The color of the model.
     */
    void drawModelAt(Coord3D coords, PieceType type, PlayerColor color);

    /**
     * @brief Animates the turn for the source and destination field.
     */
    void animateModelTurn();

    /**
     * @brief Draws the models via the OpenGL display list and checks if turn
     * animations are needed.
     */
    void drawModels();

    /**
     * @brief Draws the board via the OpenGL display list.
     */
    void drawBoard();

    /**
     * @brief Draws a tile at the given coordinates.
     * @param coords The 3D world coordinates to draw the tile at.
     * @param odd True if odd, false if even.
     * @param style The tile style (e.g. CURSOR).
     */
    void drawTile(Coord3D coords, bool odd, TileStyle style);

    /**
     * @brief Calculates the coordinates for a given field.
     * @param which The field for which the coordinates are calculated from.
     * @return The 3D coordinates.
     */
    Coord3D calcCoordinatesForTileAt(Field which);

    /**
     * @brief Animates the model strike/bash at the given coordinates.
     * @note This function is time-dependent and must be called on each
     * loop-step to see any effect.
     * @param coords The coordinates where to animate the strike.
     * @param piece The type of the piece to animate. 
     */
    void animateModelStrike(Coord3D coords, Piece piece);

    /**
     * @brief Animates the model turn at the given coordinates.
     * @param coords The coordinates where to animate the turn.
     * @param animCapsule The animation capsule to animate.
     */
    void animateModelTurn(Coord3D coords, AnimationCapsule animCapsule);

    //! boost::signals2 callback Signal to call when a resource will be loaded.
    using Signal = boost::signals2::signal<void(std::string)>;
    Signal m_loadCallback;
};

using ChessSetPtr = std::shared_ptr<ChessSet>;

#endif // CHESSSET_H
