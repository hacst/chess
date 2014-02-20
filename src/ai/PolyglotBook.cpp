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
#include "PolyglotBook.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <array>
#include <boost/detail/endian.hpp>
#include <stdlib.h>
#include <boost/random.hpp>

using namespace std;
using namespace Logging;

bool PolyglotBookEntry::isPromotion() const {
    return move.promotion_piece != NoType;
}

bool PolyglotBookEntry::mightBeCastlingMove() const {
    /*
    white short      e1h1
    white long       e1a1
    black short      e8h8
    black long       e8a8
    */
    return (   (move.from == E1 && move.to == H1)
            || (move.from == E1 && move.to == A1)
            || (move.from == E8 && move.to == H8)
            || (move.from == E8 && move.to == A8));
}

Field PolyglotBookEntry::getKingCastlingTarget() const {
    assert(mightBeCastlingMove());
    
    switch (move.to) {
        case H1: return G1;
        case A1: return B1;
        case H8: return G8;
        case A8: return B8;
        default: return ERR;
    }
    
    return ERR;
}

bool PolyglotBookEntry::operator==(const PolyglotBookEntry &other) const {
    return other.key == key
        && other.move == move
        && other.weight == weight;
}

string PolyglotBookEntry::toString() const {
    stringstream ss;
    ss << std::hex << key;
    ss << std::dec << " " << move << " weight " << weight;
    
    return ss.str();
}


bool PolyglotBookEntry::Move::operator==(const PolyglotBookEntry::Move &other) const {
    return other.from == from
        && other.to == to
        && other.promotion_piece == promotion_piece;
}

string PolyglotBookEntry::Move::toString() const {
    stringstream ss;
    
    ss << from << to;
    
    if (promotion_piece != NoType) {
        ss << " promotion piece: " << promotion_piece;
    }
    
    return ss.str();
}

PolyglotBook::PolyglotBook(int seed)
    : m_book()
    , m_rng(seed)
    , m_log(initLogger("PolyglotBook")) {
    
    LOG(info) << "Seeded with " << seed;
}

bool PolyglotBook::open(const std::string &book) {
    ifstream ifs;
    ifs.open(book, ios::binary);
    if (!ifs.is_open()) {
        LOG(error) << "Could not open opening book '" << book << "'";
        return false;
    }
    
    struct {
        uint64_t key;
        uint16_t move;
        uint16_t weight;
        uint32_t learn;
    } entry;
    
    while (ifs.peek() != EOF) {
        // Since MSVC13 doesn't support C++11 packing take and bitfields
        // are absolutely non-portable take the safe route.
        ifs.read(reinterpret_cast<char*>(&entry.key), 8);
        ifs.read(reinterpret_cast<char*>(&entry.move), 2);
        ifs.read(reinterpret_cast<char*>(&entry.weight), 2);
        ifs.read(reinterpret_cast<char*>(&entry.learn), 4);
        
        if (ifs.fail()) {
            LOG(error) << "Book '" << book << "' has invalid format";
            m_book.clear();
            return false;
        }
        
        #ifdef BOOST_LITTLE_ENDIAN
        // Book is big endian, need to convert
            #if defined(__GNUC__) || defined(__clang__)
        entry.key = __builtin_bswap64(entry.key);
        entry.move = __builtin_bswap16(entry.move);
        entry.weight = __builtin_bswap16(entry.weight);
        entry.learn = __builtin_bswap32(entry.learn);
           #elif defined(_MSC_VER)
        entry.key = _byteswap_uint64(entry.key);
        entry.move = _byteswap_ushort(entry.move);
        entry.weight = _byteswap_ushort(entry.weight);
        entry.learn = _byteswap_ulong(entry.learn);
            #else
                #error Need bit swap intrinsics
            #endif
        #endif
        
        PolyglotBookEntry polyEntry;
        polyEntry.key = entry.key;
        polyEntry.weight = entry.weight;
        
        const File to_file = static_cast<File>(entry.move & 0x7);
        const Rank to_rank = static_cast<Rank>((entry.move >> 3) & 0x7);
        const File from_file = static_cast<File>((entry.move >> 6) & 0x7);
        const Rank from_rank = static_cast<Rank>((entry.move >> 9) & 0x7);
        uint16_t promotion_piece = (entry.move >> 12) & 0x7;

        polyEntry.move.from = fieldFor(from_file, from_rank);
        polyEntry.move.to = fieldFor(to_file, to_rank);
        
        array<PieceType, 5> piece_mapping = { NoType, Knight, Bishop, Rook, Queen };
        polyEntry.move.promotion_piece = piece_mapping[promotion_piece];
        
        m_book.emplace_back(polyEntry);
    }
    
    LOG(info) << "Loaded '" << book << "' opening book with " << getNumberOfEntries() << " entries";
    return true;
}

std::vector<PolyglotBookEntry> PolyglotBook::lookup(uint64_t key) const {
    const PolyglotBookEntry dummy = { key };
    
    auto cmp = [](const PolyglotBookEntry& a, const PolyglotBookEntry& b) {
        return a.key < b.key;
    };

    auto range = equal_range(begin(m_book), end(m_book), dummy, cmp);
    
    vector<PolyglotBookEntry> result;
    result.insert(end(result), range.first, range.second);
    
    return result;
}

boost::optional<PolyglotBookEntry> PolyglotBook::getWeightedEntry(uint64_t key) {
    vector<PolyglotBookEntry> options = lookup(key);
    
    if (options.empty())
        return boost::none;

    LOG(trace) << "Weighted turn request for " << key;
    vector<double> weights;
    for (PolyglotBookEntry& option: options) {
        LOG(trace) << "  " << option;
        weights.push_back(option.weight);
    }
    
    boost::random::discrete_distribution<> dist(begin(weights), end(weights));
    
    PolyglotBookEntry result = options[dist(m_rng)];
    LOG(trace) << "Choose: " << result;
    
    return result;
}

boost::optional<PolyglotBookEntry> PolyglotBook::getBestEntry(uint64_t key) const {
    vector<PolyglotBookEntry> options = lookup(key);
    
    if (options.empty())
        return boost::none;

    return options.front();
}

size_t PolyglotBook::getNumberOfEntries() const {
    return m_book.size();
}


