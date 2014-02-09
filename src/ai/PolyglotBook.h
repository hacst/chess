#ifndef POLYGLOTBOOK_H
#define POLYGLOTBOOK_H

#include <vector>
#include <random>
#include <stdint.h>
#include <boost/optional.hpp>

#include "core/Logging.h"
#include "misc/helper.h"
#include "logic/ChessTypes.h"

/** 
 * @brief Single entry in in polyglot book adjusted for this engine.
 */
struct PolyglotBookEntry {
    uint64_t key;

    struct Move {
        Field from;
        Field to;
        PieceType promotion_piece;
        
        bool operator==(const Move& other) const;
        std::string toString() const;
    } move;

    uint16_t weight;

    bool isPromotion() const;
    //! Returns true if this might be a castling move
    bool mightBeCastlingMove() const;
    //! Returns the target of the king piece for this move if it were a castling one.
    Field getKingCastlingTarget() const;
    
    bool operator==(const PolyglotBookEntry& other) const;
    std::string toString() const;
};

/**
 * @brief Class able to lookup entries from a polyglot opening file.
 * Format as described on http://hgm.nubati.net/book_format.html.
 * @note Book is held in memory for the lifetime of the object.
 */
class PolyglotBook {
public:
    /**
     * @brief Creates a new book instance.
     * @see open
     * @param seed Seed which to use for selections with chance.
     */
    explicit PolyglotBook(int seed = 5235);
    
    //! Opens a given book file and reads it into memory.
    bool open(const std::string& book);
    
    //! Performs a lookup for the given key and returns all entries with matching key.
    std::vector<PolyglotBookEntry> lookup(uint64_t key) const;
    
    /**
     * @brief Selects a entry based on the relative weights of the results from a lookup of key.
     * A turns probability of being chosen is weight/sum(weights)
     * @param key Zobrist hash for position
     * @return boost::none if no turn found.
     */
    boost::optional<PolyglotBookEntry> getWeightedEntry(uint64_t key);
    
    /**
     * @brief Selects the entry with maximum weight from a lookup of key
     * @param key Zobrist hash for position
     * @return boost::none if no turn found.
     */
    boost::optional<PolyglotBookEntry> getBestEntry(uint64_t key) const;
    
    //! Returns the number of entries in the book.
    size_t getNumberOfEntries() const;
    
private:
    /** 
     * @brief Book as a vector of entries sorted by key
     */
    std::vector<PolyglotBookEntry> m_book;
    
    std::mt19937 m_rng;
    Logging::Logger m_log;
};

#endif //POLYGLOTBOOK_H
