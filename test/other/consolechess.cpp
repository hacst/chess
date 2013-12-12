#include <iostream>
#include <boost/program_options.hpp>

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
    cout << "Turns limited to: " << turnlimit << endl;

    GameState gameState;
    auto eval = make_shared<MaterialEvaluator>();
    Negamax<> negamax(eval);

    cout << "Initial state" << endl;
    cout << gameState.getChessBoard() << endl;

    for (int i = 0; i < turnlimit; ++i) {
        cout << "== " << gameState.getNextPlayer() << " ==" << endl;
        cout << "Calculating turn " << i << "...";
        cout.flush();

        auto result = negamax.search(gameState, 4);
        cout << "Done" << endl;
        cout.flush();
        if (!result.turn)
            break;

        auto turn = result.turn.get();
        cout << "Turn: " << turn << endl;

        gameState.applyTurn(turn);
        cout << gameState.getChessBoard() << endl << endl;
    }
    cout << "No more moves possible" << endl;
    return 0;
}
