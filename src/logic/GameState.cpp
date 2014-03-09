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
#include "GameState.h"
#include <algorithm>

GameState::GameState() {
    init();
}

GameState::GameState(const ChessBoard &chessBoard)
    : m_chessBoard(chessBoard) {
    init();
}

void GameState::init() {
    m_turnGen.initFlags(m_chessBoard);
    m_turnGen.generateTurns(getNextPlayer(), m_chessBoard);
}

std::vector<Turn> GameState::getTurnList() const {
    return m_turnGen.getTurnList();
}

void GameState::applyTurn(const Turn& turn) {
    m_chessBoard.applyTurn(turn);
    m_turnGen.generateTurns(getNextPlayer(), m_chessBoard);
}

PlayerColor GameState::getNextPlayer() const {
    return m_chessBoard.getNextPlayer();
}

const ChessBoard& GameState::getChessBoard() const {
    return m_chessBoard;
}

bool GameState::isGameOver() const {
    return m_chessBoard.isGameOver();
}

bool GameState::isDrawDueTo50MovesRule() const {
    return m_chessBoard.isDrawDueTo50MovesRule();
}

PlayerColor GameState::getWinner() const {
    return m_chessBoard.getWinner();
}

Score GameState::getScore(size_t depth) const {
    return m_chessBoard.getScore(m_chessBoard.getNextPlayer(), depth);
}

Piece GameState::getLastCapturedPiece() const {
    return m_chessBoard.getLastCapturedPiece();
}

bool GameState::operator==(const GameState& other) const {
    return m_chessBoard == other.getChessBoard();
}

bool GameState::operator!=(const GameState& other) const {
    return !(*this == other);
}

Hash GameState::getHash() const {
    return m_chessBoard.getHash();
}

std::string GameState::toString() const {
    return m_chessBoard.toString();
}

GameState GameState::fromFEN(const std::string& fen) {
    return GameState(ChessBoard::fromFEN(fen));
}

std::string GameState::toFEN() const {
    return m_chessBoard.toFEN();
}
