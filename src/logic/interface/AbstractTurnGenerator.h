#ifndef ABSTRACTTURNGENERATOR_H
#define ABSTRACTTURNGENERATOR_H

#include "../ChessTypes.h"
#include "../ChessBoard.h"


class AbstractTurnGenerator {
public:
    virtual std::vector<Turn> generateTurns(PlayerColor player, const ChessBoard& board) = 0;
};

using AbstractTurnGeneratorPtr = std::shared_ptr<AbstractTurnGenerator>;


#endif // ABSTRACTTURNGENERATOR_H
