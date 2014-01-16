#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <array>
#include <boost/optional.hpp>
#include <sstream>

#include "logic/ChessTypes.h"
#include "logic/Turn.h"

/**
 * @brief Single entry in transposition table
 * @see TranspositionTable
 */
struct TranspositionTableEntry {
    Hash hash;
    Turn turn;
    Score score;
    
    enum BoundType {
        LOWER,
        UPPER,
        EXACT
    } boundType;

    size_t depth;
    
    bool isLowerBound() const { return boundType == LOWER; }
    bool isUpperBound() const { return boundType == UPPER; }
    bool isExactBound() const { return boundType == EXACT; }
    
    std::string toString() const {
        std::stringstream ss;
        ss << std::hex << "0x" << hash << std::dec << " " << turn
           << " Score est.: " << score;
        if (boundType == LOWER) ss << " LOWER";
        else if (boundType == UPPER) ss << " UPPER";
        else ss << " EXACT";
        ss << " from depth " << depth;
        
        return ss.str();
    }
};

/**
 * @brief Transposition table with fixed size.
 * Hashed on hash of transposition table entry. Offers internal
 * collision detection by checking hash in entry before returning.
 * @note Zobrist hash collisions can't be detected
 */

class TranspositionTable {
public:
    //! Creates an empty transposition table
    TranspositionTable(size_t tablesize = 1048583)
        : m_table(tablesize)
        , m_tablesize(tablesize) {
        // Empty
    }
    
    //! Set entry in table
    void update(TranspositionTableEntry entry) {
        m_table[entry.hash % m_tablesize] = entry;
    }
    
    /**
     * @brief Lookup hash in table.
     * @note Not secure against zobrist hash collisions.
     * @return Entry if in table.
     */
    boost::optional<TranspositionTableEntry> lookup(Hash hash) const {
        TranspositionTableEntry entry = m_table[hash % m_tablesize];
        if (entry.hash != hash)
            return boost::none;
        
        return entry;
    }

    size_t getTableSize() const {
        return m_tablesize;
    }
    
private:
    
    //! Hashtable with transpositions
    std::vector<TranspositionTableEntry> m_table;
    
    //! Size set for this table
    const size_t m_tablesize;
};


#endif //TRANSPOSITION_TABLE_H
