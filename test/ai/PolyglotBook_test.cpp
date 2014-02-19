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
