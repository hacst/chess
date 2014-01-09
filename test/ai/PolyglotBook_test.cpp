#include <gtest/gtest.h>
#include <vector>
#include <map>

#include "ai/PolyglotBook.h"

using namespace std;

TEST(PolyglotBook, open) {
    {
        PolyglotBook book;
        EXPECT_TRUE(book.open("resources/Book.bin"));
        EXPECT_EQ(360125, book.getNumberOfEntries());
    }
    {
        PolyglotBook book;
        EXPECT_FALSE(book.open("bookthatdoesnotexist"));
        EXPECT_EQ(0, book.getNumberOfEntries());
    }
}

TEST(PolyglotBook, lookup) {
    PolyglotBook book;
    ASSERT_TRUE(book.open("resources/Book.bin"));
    
    {
        // Initial position: 0x463b96181691fc9c
        vector<PolyglotBookEntry> entries = {
            { 0x463b96181691fc9cULL, { E2, E4, NoType }, 181},
            { 0x463b96181691fc9cULL, { D2, D4, NoType }, 157},
            { 0x463b96181691fc9cULL, { C2, C4, NoType }, 16}
        };
        
        EXPECT_EQ(entries, book.lookup(0x463b96181691fc9cULL));
    }
    {
        vector<PolyglotBookEntry> entries = {
            { 0x2913395f747cULL, { D4, D5, NoType }, 6 }
        };
        EXPECT_EQ(entries, book.lookup(0x2913395f747cULL));
    }
}

TEST(PolyglotBook, getBest) {
    PolyglotBook book;
    ASSERT_TRUE(book.open("resources/Book.bin"));
    
    // Initial position: 0x463b96181691fc9c
    PolyglotBookEntry best = { 0x463b96181691fc9cULL, { E2, E4, NoType }, 181};
    auto entry = book.getBestEntry(0x463b96181691fc9cULL);
    ASSERT_TRUE(entry);
    EXPECT_EQ(best, entry.get());

    EXPECT_FALSE(book.getBestEntry(0x00));
}

TEST(PolyglotBook, getWeighted) {
    PolyglotBook book;
    ASSERT_TRUE(book.open("resources/Book.bin"));
    
    struct PolyEntryLessNOTGENERIC {
        bool operator()(const PolyglotBookEntry& a, const PolyglotBookEntry& b) const {
                return a.weight < b.weight;
        }
    };
    
    map<PolyglotBookEntry, int, PolyEntryLessNOTGENERIC> entries = {
        {{ 0x463b96181691fc9cULL, { E2, E4, NoType }, 181}, 0},
        {{ 0x463b96181691fc9cULL, { D2, D4, NoType }, 157}, 0},
        {{ 0x463b96181691fc9cULL, { C2, C4, NoType }, 16}, 0}
    };
   
    double total_weight = 181+157+16;
    
    const size_t tries = 10000;
    for (size_t i = 0; i < tries; ++i) {
        auto entry = book.getWeightedEntry(0x463b96181691fc9cULL);
        ASSERT_TRUE(entry);
        
        ++entries[entry.get()];
    }
    
    for (auto& entry: entries) {
        const PolyglotBookEntry& bookEntry = entry.first;
        const double expected = bookEntry.weight / total_weight * tries;
        const double dev = expected / 10.;
        
        EXPECT_TRUE(entry.second < dev + expected && entry.second > expected - dev)
                << bookEntry << " expected " << expected << " actual " << entry.second;
    }
}
