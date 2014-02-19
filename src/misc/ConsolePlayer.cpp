/*
    Copyright (c) 2013-2014, Stefan Hacker <dd0t@users.sourceforge.net>

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

#include "ConsolePlayer.h"

using namespace std;

ConsolePlayer::~ConsolePlayer() {
	stop(true);
}

void ConsolePlayer::onSetColor(PlayerColor color) {
	post([=]() {
		cout << "You will be playing " << color << endl;
		m_color = color;
	});
}

void ConsolePlayer::onGameStart(GameState state, GameConfiguration config) {
	post([=]() {
		cout << "Game started" << endl;
			 //<< state << endl
			 //<< config << endl;
	});
}

future<Turn> ConsolePlayer::doMakeTurn(GameState state) {
	return postPromise([=]() {
		cout << "Your turn (press enter)" << endl;
		cin.get();
		return Turn();
	});
}

void ConsolePlayer::onTurnEnd(PlayerColor color, Turn turn, GameState newState) {
	post([=]() {
		if (color != m_color) {
			cout << turn << endl;
		}
		cout << newState << endl;
	});
}

void ConsolePlayer::doAbortTurn() {
	cout << endl << "Please end your turn (press enter)" << endl;
}

void ConsolePlayer::onGameOver(GameState state, PlayerColor winner) {
	cout << "Game over: ";

    if (winner == NoPlayer) {
		cout << "Draw" << endl;
	} else if (winner == m_color) {
		cout << "You won" << endl;
	} else {
		cout << "You lost" << endl;
	}
}

