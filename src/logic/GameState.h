/*
    Copyright (c) 2013-2014, Max Stark <max.stark88@googlemail.com>

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
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "ChessTypes.h"
#include "ChessBoard.h"
#include "TurnGenerator.h"

/**
 * @brief Facade class for the game logic, holds the chessboard and
 * the turn generator. It's mainly purpose is to provide access to functions
 * relevant for the game.
 */
class GameState {
public:
    GameState();
    explicit GameState(const ChessBoard& chessBoard);

    //! Returns a list with all possible and legal turns.
    std::vector<Turn> getTurnList() const;
    //! Applies the given turn on current chessboard.
    void applyTurn(const Turn& turn);

    //! Return next player to make a turn.
    PlayerColor getNextPlayer() const;
    //! Provides access to the chessboard.
    const ChessBoard& getChessBoard() const;
    /**
     * @brief Returns the captured piece from the last turn or
     * Piece(NoPlayer, NoType) if no piece was captured
     */
    Piece getLastCapturedPiece() const;

    //! Returns true if the game is over
    bool isGameOver() const;
    /**
    * @brief Returns the winner of the game.
    * Returns Player color or NoPlayer on draw.
    */
    PlayerColor getWinner() const;

    //! Returns true if the game is draw due to the 50 moves rule
    bool isDrawDueTo50MovesRule() const;

    //! Returns current score estimate from next players POV.
    Score getScore(size_t depth = 0) const;
    //! Returns hash for current position
    Hash getHash() const;

    /**
    * @brief Create a GameState from a Forsyth-Edwards Notation string.
    * http://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    * @warning This function does no validation. Do not pass invalid FEN.
    * @param fen FEN String.
    */
    static GameState fromFEN(const std::string& fen);

    /**
    * @brief Converts the current game state into FEN notation.
    * @return State in FEN notation.
    */
    std::string toFEN() const;

    bool operator==(const GameState& other) const;
    bool operator!=(const GameState& other) const;
    std::string toString() const;

private:
    //! Initialize the turn generator with the given chessboard.
    void init();
    //! Chessboard representation and logic.
    ChessBoard m_chessBoard;
    //! Turn generator and gameover detection.
    TurnGenerator m_turnGen;
};


#endif // GAMESTATE_H
