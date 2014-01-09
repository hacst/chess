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


