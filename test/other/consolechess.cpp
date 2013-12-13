#include <iostream>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

#define LOG	BOOST_LOG_TRIVIAL

#include "misc/helper.h"
#include "logic/Evaluators.h"
#include "ai/Negamax.h"

using namespace std;
namespace po = boost::program_options;

using namespace std;

int main(int argn, char **argv) {
    // Boost program options for commandline parsing

    po::options_description desc("consolechess");
    desc.add_options()
        ("help", "Print help message")
        ("turns", po::value<int>()->default_value(numeric_limits<int>::max()), "Number of turns after which to abort game")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argn, argv, desc), vm);

    if (vm.count("help")) {
        cerr << desc << endl;
        return 1;
    }

    const int turnlimit = vm["turns"].as<int>();
    LOG(info) << "Turns limited to: " << turnlimit;

    GameState gameState;
    auto eval = make_shared<MaterialEvaluator>();
    Negamax<> negamax(eval);

    LOG(info) << "Initial state";
    LOG(info) << gameState.getChessBoard();

    for (int i = 0; i < turnlimit; ++i) {
        LOG(info) << "== " << gameState.getNextPlayer() << " ==";
        LOG(info) << "Calculating turn " << i << "...";

        auto result = negamax.search(gameState, 4);
        LOG(info) << "Completed calculation";

        if (!result.turn) {
            LOG(info) << "No more moves possible";
            break;
        }

        auto turn = result.turn.get();
        LOG(info) << "Turn: " << turn;

        gameState.applyTurn(turn);
        LOG(debug) << gameState.getChessBoard();
    }

    LOG(info) << "Finale state: ";
    LOG(info) << gameState.getChessBoard();

    return 0;
}
