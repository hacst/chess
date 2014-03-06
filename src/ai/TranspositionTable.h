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
    Hash hash; //!< Hash identifying position (might collide)
    Turn turn; //!< Best turn from this position
    Score score; //!< Estimated score (@see boundType, @see depth)
    
    //! Describes the guarantees for the entry.
    enum BoundType {
        LOWER, //!< Score is lower bound to score attainable by turn.
        UPPER, //!< Score is upper bound to score attainable by turn.
        EXACT  //!< Score is exactly what is attainable by turn.
    } boundType;

    size_t depth; //!< Search depth used to evaluate position.
    
    //! Returns true if entry score is lower bound to score attainable by turn.
    bool isLowerBound() const { return boundType == LOWER; }
    //! Returns true if entry score is upper bound to score attainable by turn.
    bool isUpperBound() const { return boundType == UPPER; }
    //! Returns true if score is exactly what is attainable by turn.
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
 * Hashed on hash of transposition table entry. Offers limited internal
 * collision detection against class 2 errors by checking hash in entry
 * before returning. Class 1 errors should handled externally if problematic.
 */
class TranspositionTable {
public:
    /**
     * @brief Creates an empty transposition table of given size.
     * @param tablesize Number of independent spaces in hashtable.
     * @note To ensure even distribution tablesize should be prime.
     */
    TranspositionTable(size_t tablesize = 4000037)
        : m_table(tablesize)
        , m_tablesize(tablesize) {
        // Empty
    }
    
    /**
     * @brief Stores the given entry if it meets table replacement criteria.
     * Stores the given entry either if it belongs to a different position
     * than the current one or if its depth is greater than the previous
     * entry for this position. This relies on the assumption that deeper
     * entries most likely took more positions into account thus representing
     * a greater investment in compute time.
     * @param entry Entry to store.
     */
    void maybeUpdate(TranspositionTableEntry entry) {
        TranspositionTableEntry &oldEntry = m_table[entry.hash % m_tablesize];

        if (oldEntry.hash == entry.hash && oldEntry.depth > entry.depth)
            return;

        oldEntry = entry;
    }
    
    /**
     * @brief Lookup hash in table.
     * @note Not secure against zobrist hash collisions.
     * @return Option to entry if in table. boost::none otherwise.
     */
    boost::optional<TranspositionTableEntry> lookup(Hash hash) const {
        TranspositionTableEntry entry = m_table[hash % m_tablesize];
        if (entry.hash != hash)
            return boost::none;
        
        return entry;
    }

    //! Returns the number of possible independent table entries.
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
