#include <gtest/gtest.h>

#include "ai/TranspositionTable.h"

TEST(TranspositionTable, lookup) {
    TranspositionTable tbl;
    
    // Check it's empty in the beginning
    EXPECT_FALSE(tbl.lookup(10ULL));
    EXPECT_FALSE(tbl.lookup(tbl.getTableSize() + 100));
    EXPECT_FALSE(tbl.lookup(tbl.getTableSize() / 2));
    
    TranspositionTableEntry entry;
    entry.hash = 5;
    entry.score = 5;
    entry.depth = 0;
    entry.boundType = TranspositionTableEntry::EXACT;
    tbl.update(entry);
    
    //TODO: Improve this
}
