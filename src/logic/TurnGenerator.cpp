#include "TurnGenerator.h"

std::vector<Turn> TurnGenerator::generateTurns(GameStatePtr state) {

    std::array<BitBoard, 6> bitBoards = this->calcTurnBitBoards(state->m_next, state->m_board);
    std::vector<Turn> turns;

    foreach (BitBoard b, bitBoards) {
        // TODO: convert BitBoard to Turns and add it to the return vector



        //turns.push_back();
    }

    return turns;
}

std::array<BitBoard, 6> TurnGenerator::calcTurnBitBoards(PlayerColor player, ChessBoardPtr board) {
    std::array<BitBoard, 6> bitBoards;

    //bitBoards[0] = calcQueenTurns(board->m_queens[player], board->m_allPieces[player]);
    //bitBoards[1] = calcBishopTurns(board->m_bishops[player], board->m_allPieces[player]);
    //bitBoards[2] = calcRookTurns(board->m_rooks[player], board->m_allPieces[player]);
    bitBoards[3] = calcKingTurns(board->m_king[player], board->m_allPieces[player]);
    //bitBoards[4] = calcKnightTurns(board->m_knights[player], board->m_allPieces[player]);
    //bitBoards[5] = calcPawnTurns(board->m_pawns[player], board->m_allPieces[player]);

    return bitBoards;
}

BitBoard TurnGenerator::calcKingTurns(BitBoard king, BitBoard allPieces) {
    king = king & ChessBoard::clearFile(File.A);
    king = king & ChessBoard::clearFile(File.H);

    BitBoard turn1 = king << 9;
    BitBoard turn2 = king << 8;
    BitBoard turn3 = king << 7;
    BitBoard turn4 = king << 1;
    BitBoard turn5 = king >> 1;
    BitBoard turn6 = king >> 7;
    BitBoard turn7 = king >> 8;
    BitBoard turn8 = king >> 9;

    BitBoard kingTurns = turn1 | turn2 | turn3 | turn4 |
                         turn5 | turn6 | turn7 | turn8;
    kingTurns &= ~allPieces;



    return kingTurns;
}
