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

#include <iostream>

#include "GuiObserver.h"
#include "misc/helper.h"
#include <thread>

#include "gui/states/GamePlay.h"

using namespace std;

GuiObserver::GuiObserver(ChessSetPtr chessSetPtr, GamePlay& gamePlayState)
    : m_chessSetPtr(chessSetPtr)
    , m_gamePlayState(gamePlayState) {
}

void GuiObserver::onGameStart(GameState state, GameConfiguration config) {
    m_gamePlayState.setGameState(state);
    m_gamePlayState.setState(state.getChessBoard().getBoard());
    m_gamePlayState.setCapturedPiecesList(state.getChessBoard().getCapturedPieces());
    m_gamePlayState.switchToPlayerColor(state.getNextPlayer());
}

void GuiObserver::onTurnStart(PlayerColor who) {
    m_gamePlayState.onPlayerIsOnTurn(who);
}

void GuiObserver::onTurnEnd(PlayerColor who, Turn turn, GameState newState) {
    m_gamePlayState.setGameState(newState);
    m_gamePlayState.setState(newState.getChessBoard().getBoard(), who, turn);
    m_gamePlayState.setCapturedPiecesList(newState.getChessBoard().getCapturedPieces());
    m_gamePlayState.switchToPlayerColor(newState.getNextPlayer());
}

void GuiObserver::onTurnTimeout(PlayerColor who, std::chrono::seconds timeout) {
    stringstream strs;
    strs << (who == PlayerColor::Black ? "Schwarz" : "Weiss") << " hat zu lange gebraucht.";

    m_gamePlayState.startShowText(strs.str());
}

void GuiObserver::onGameOver(GameState state, PlayerColor winner) {
    stringstream strs;
    if (winner == PlayerColor::Black) {
        strs << "Schwarz hat gewonnen. Das Spiel ist zu Ende.";
    }
    else if (winner == PlayerColor::White) {
        strs << "Weiss hat gewonnen. Das Spiel ist zu Ende.";
    }
    else {
        assert(winner == PlayerColor::NoPlayer);
        strs << "Patt! Das Spiel ist zu Ende.";
    }

    m_gamePlayState.startShowText(strs.str());
    m_gamePlayState.setGameState(state);
}