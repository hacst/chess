#include <iostream>
#include <chrono>
#include <thread>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

#define LOG	BOOST_LOG_TRIVIAL

#include "misc/helper.h"
#include "logic/Evaluators.h"
#include "ai/Negamax.h"

using namespace std;
using namespace std::chrono;
namespace po = boost::program_options;

using namespace std;

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

    const int turnlimit = vm["turns"].as<int>();
    const int depthWhite = vm["depthw"].as<int>();
    const int depthBlack = vm["depthb"].as<int>();
    const int delay  = vm["delay"].as<int>();

    LOG(info) << "Turns limited to: " << turnlimit;
    LOG(info) << "Depth limited to: " << depthWhite << " for White";
    LOG(info) << "Depth limited to: " << depthBlack << " for Black";

    GameState gameState;
    auto eval = make_shared<MaterialEvaluator>();
    Negamax<> negamax(eval);

    LOG(info) << "Initial state";
    LOG(info) << gameState.getChessBoard();

    for (int i = 0; i < turnlimit; ++i) {
        LOG(info) << "== " << gameState.getNextPlayer() << " ==";
        LOG(info) << "Calculating turn " << i << "...";

        const int depth = (gameState.getNextPlayer() == White) ? depthWhite : depthBlack;
        auto result = negamax.search(gameState, depth);
        LOG(info) << "Completed calculation";

        if (!result.turn) {
            LOG(info) << "No more moves possible";
            break;
        }

        auto turn = result.turn.get();
        LOG(info) << "Turn: " << turn;
        LOG(info) << "Score: " << result.score;

        gameState.applyTurn(turn);
        LOG(debug) << gameState.getChessBoard();

        if (delay != 0) {
            this_thread::sleep_for(milliseconds(delay));
        }
    }

    LOG(info) << "Finale state: ";
    LOG(info) << gameState.getChessBoard();

    return 0;
}
