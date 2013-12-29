#ifndef DEBUG_TOOLS_H
#define DEBUG_TOOLS_H

#include <string>
#include <array>

struct Piece;

/**
 * @brief Contains functions for helping with debugging tasks.
 */
namespace DebugTools {

/**
 * @brief Returns the code needed to initialize a board to the given state
 */
std::string toInitializerList(const std::array<Piece, 64>& board);

} // namespace DebugTools


#endif // DEBUG_TOOLS_H
