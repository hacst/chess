#include <iostream>
#include <chrono>
#include <thread>
#include <boost/program_options.hpp>

#include "misc/helper.h"
#include "logic/Evaluators.h"
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
        ("depthw", po::value<int>()->default_value(6), "Search depth for Negamax search for White player")
        ("depthb", po::value<int>()->default_value(6), "Search depth for Negamax search for Black player")
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
    Negamax negamax;

    GLOG(info) << "Initial state";
    GLOG(info) << gameState.getChessBoard();

    for (int i = 0; i < turnlimit; ++i) {
        GLOG(info) << "== " << gameState.getNextPlayer() << " ==";
        GLOG(info) << "Calculating turn " << i << "...";

        const int depth = (gameState.getNextPlayer() == White) ? depthWhite : depthBlack;
        auto result = negamax.search<GameState, true, true>(gameState, depth);
        GLOG(info) << "Completed calculation";

        if (!result.turn) {
            GLOG(info) << "No more moves possible";
            break;
        }

        auto turn = result.turn.get();
        GLOG(info) << "Turn: " << turn;
        GLOG(info) << "Score: " << result.score;

        gameState.applyTurn(turn);
        GLOG(debug) << gameState.getChessBoard();

        if (delay != 0) {
            this_thread::sleep_for(milliseconds(delay));
        }
    }

    GLOG(info) << "Finale state: ";
    GLOG(info) << gameState.getChessBoard();

    return 0;
}
