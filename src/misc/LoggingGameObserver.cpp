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

#include "LoggingGameObserver.h"
#include "misc/helper.h"
#include <thread>

using namespace std;
using namespace Logging;

LoggingGameObserver::LoggingGameObserver()
    : m_log(initLogger("LoggingGameObserver")) {
    // Empty
}

void LoggingGameObserver::onGameStart(GameState state, GameConfiguration config) {
	LOG(info) << "Game started";
	LOG(info) << config;
	LOG(info) << endl << state;
}

void LoggingGameObserver::onTurnStart(PlayerColor who) {
	LOG(info) << who << " should now perform his turn" << endl;
}

void LoggingGameObserver::onTurnEnd(PlayerColor who, Turn turn, GameState newState) {
	LOG(info) << who << " performed: " << turn;
	LOG(info) << endl << newState << endl;
}

void LoggingGameObserver::onTurnTimeout(PlayerColor who, std::chrono::seconds timeout) {
	LOG(info) << who << " timed out after " << timeout.count() << " seconds";
}

void LoggingGameObserver::onGameOver(GameState state, PlayerColor winner) {
	LOG(info) << "Game Over. Winner: " << winner;
	LOG(info) << endl << state << endl;
}

