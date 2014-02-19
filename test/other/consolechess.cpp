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
#include <chrono>
#include <thread>
#include <boost/program_options.hpp>

#include "misc/helper.h"
#include "ai/Negamax.h"
#include "core/Logging.h"

using namespace std;
using namespace std::chrono;
namespace po = boost::program_options;

using namespace std;
using namespace Logging;

int main(int argn, char **argv) {
    // Boost program options for commandline parsing

    po::options_description desc("consolechess");
    desc.add_options()
        ("help", "Print help message")
        ("turns", po::value<int>()->default_value(numeric_limits<int>::max()), "Number of turns after which to abort game")
        ("depthw", po::value<int>()->default_value(4), "Search depth for Negamax search for White player")
        ("depthb", po::value<int>()->default_value(4), "Search depth for Negamax search for Black player")
        ("delay", po::value<int>()->default_value(0), "Sleep in ms between moves")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argn, argv, desc), vm);

    if (vm.count("help")) {
        cerr << desc << endl;
        return 1;
    }

    initializeLogging();
    addLoggingConsoleSink(debug);
    addLoggingFileSink("consolechess.log", debug);

    Logger log = initLogger("consolechess");
    
    const int turnlimit = vm["turns"].as<int>();
    const int depthWhite = vm["depthw"].as<int>();
    const int depthBlack = vm["depthb"].as<int>();
    const int delay  = vm["delay"].as<int>();

    GLOG(info) << "Turns limited to: " << turnlimit;
    GLOG(info) << "Depth limited to: " << depthWhite << " for White";
    GLOG(info) << "Depth limited to: " << depthBlack << " for Black";

    GameState gameState;
    std::array<Negamax<>, NUM_PLAYERS> negamax;

    GLOG(info) << "Initial state";
    GLOG(info) << gameState.getChessBoard();

    for (int i = 0; i < turnlimit; ++i) {
        GLOG(info) << "== " << gameState.getNextPlayer() << " ==";
        GLOG(info) << "Calculating turn " << i << "...";

        PlayerColor next = gameState.getNextPlayer();
        PlayerColor opp = togglePlayerColor(next);
        const int depth = (next == White) ? depthWhite : depthBlack;
        auto result = negamax[next].search(gameState, depth);
        GLOG(info) << "Completed calculation";

        if (!result.turn) {
            GLOG(info) << "No more moves possible";
            break;
        }

        auto turn = result.turn.get();
        GLOG(info) << "Turn: " << turn;
        GLOG(info) << "Score: " << result.score;

        gameState.applyTurn(turn);
        GLOG(info) << gameState.getChessBoard();

        if (gameState.getChessBoard().getKingInCheck()[opp]) {
            GLOG(info) << "Player " << opp << " in CHECK";
        }
        if (gameState.getChessBoard().getStalemate()) {
            GLOG(info) << "Stalemate, NO WINNER, GAME OVER";
            break;
        }
        if (gameState.getChessBoard().getCheckmate()[opp]) {
            GLOG(info) << opp << " is checkmate, " << next << " is the WINNER, GAME OVER";
            break;
        }

        if (delay != 0) {
            this_thread::sleep_for(milliseconds(delay));
        }
    }

    GLOG(info) << "Finale state: ";
    GLOG(info) << gameState.getChessBoard();

    return 0;
}
