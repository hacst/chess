#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "logic/threading/PlayerDispatcherProxy.h"
#include "core/GameConfiguration.h"

TEST(Misc, togglePlayerColor) {
    EXPECT_EQ(Black, togglePlayerColor(White));
    EXPECT_EQ(White, togglePlayerColor(Black));
}

TEST(Misc, rankFor) {
    for (Rank r = One; r < Eight; r = nextRank(r)) {
        for (File f = A; f < H; f = nextFile(f)) {
            const Field field = fieldFor(f, r);
            EXPECT_EQ(r, rankFor(field)) << "Field: " << field;
        }
    }
}

TEST(Misc, fileFor) {
    for (Rank r = One; r < Eight; r = nextRank(r)) {
        for (File f = A; f < H; f = nextFile(f)) {
            const Field field = fieldFor(f, r);
            EXPECT_EQ(f, fileFor(field)) << "Field: " << field;
        }
    }
}

TEST(Misc, nextField) {
    EXPECT_EQ(B1, nextField(A1));
    EXPECT_EQ(A5, nextField(H4));
    EXPECT_EQ(H8, nextField(G8));
}

TEST(Misc,prevField) {
    EXPECT_EQ(A1, prevField(B1));
    EXPECT_EQ(H4, prevField(A5));
    EXPECT_EQ(G8, prevField(H8));
}

TEST(Misc, nextFile) {
    EXPECT_EQ(B, nextFile(A));
    EXPECT_EQ(C, nextFile(B));
    EXPECT_EQ(D, nextFile(C));
    EXPECT_EQ(E, nextFile(D));
    EXPECT_EQ(F, nextFile(E));
    EXPECT_EQ(G, nextFile(F));
    EXPECT_EQ(H, nextFile(G));
}

TEST(Misc, prevFile) {
    EXPECT_EQ(G, prevFile(H));
    EXPECT_EQ(F, prevFile(G));
    EXPECT_EQ(E, prevFile(F));
    EXPECT_EQ(D, prevFile(E));
    EXPECT_EQ(C, prevFile(D));
    EXPECT_EQ(B, prevFile(C));
    EXPECT_EQ(A, prevFile(B));
}

TEST(Misc, nextRank) {
    EXPECT_EQ(Two, nextRank(One));
    EXPECT_EQ(Three, nextRank(Two));
    EXPECT_EQ(Four, nextRank(Three));
    EXPECT_EQ(Five, nextRank(Four));
    EXPECT_EQ(Six, nextRank(Five));
    EXPECT_EQ(Seven, nextRank(Six));
    EXPECT_EQ(Eight, nextRank(Seven));
}

TEST(Misc, prevRank) {
    EXPECT_EQ(Seven, prevRank(Eight));
    EXPECT_EQ(Six, prevRank(Seven));
    EXPECT_EQ(Five, prevRank(Six));
    EXPECT_EQ(Four, prevRank(Five));
    EXPECT_EQ(Three, prevRank(Four));
    EXPECT_EQ(Two, prevRank(Three));
    EXPECT_EQ(One, prevRank(Two));
}

TEST(Misc, flipHorizontal) {
    EXPECT_EQ(A1, flipHorizontal(A8));
    EXPECT_EQ(A8, flipHorizontal(A1));

    EXPECT_EQ(C4, flipHorizontal(C5));
    EXPECT_EQ(C5, flipHorizontal(C4));
    
    EXPECT_EQ(H8, flipHorizontal(H1));
    EXPECT_EQ(H1, flipHorizontal(H8));
}

TEST(Turn, getPromotionPieceType) {
    Piece p(White, Pawn);
    EXPECT_EQ(Bishop, Turn::promotionBishop(p, A8).getPromotionPieceType());
    EXPECT_EQ(Queen, Turn::promotionQueen(p, A8).getPromotionPieceType());
    EXPECT_EQ(Rook, Turn::promotionRook(p, A8).getPromotionPieceType());
    EXPECT_EQ(Knight, Turn::promotionKnight(p, A8).getPromotionPieceType());
}

TEST(Turn, isPromotion) {
    Piece p(White, Pawn);
    EXPECT_TRUE(Turn::promotionBishop(p, A8).isPromotion());
    EXPECT_TRUE(Turn::promotionQueen(p, A8).isPromotion());
    EXPECT_TRUE(Turn::promotionRook(p, A8).isPromotion());
    EXPECT_TRUE(Turn::promotionKnight(p, A8).isPromotion());
    EXPECT_FALSE(Turn::move(p, A7, A8).isPromotion());
}

/*
using namespace std;

class MockObserver : public AbstractGameObserver {
public:
	MOCK_METHOD2(onGameStart, void(GameState,GameConfiguration));
	MOCK_METHOD1(onTurnStart, void(PlayerColor));
	MOCK_METHOD3(onTurnEnd, void(PlayerColor, Turn, GameState));
	MOCK_METHOD2(onTurnTimeout, void(PlayerColor, std::chrono::seconds));
	MOCK_METHOD2(onGameOver, void(GameState, PlayerColor));
};

class MockPlayer : public AbstractPlayer {
public:
	// AbstractObserver Interface
	MOCK_METHOD2(onGameStart, void(GameState,GameConfiguration));
	MOCK_METHOD1(onTurnStart, void(PlayerColor));
	MOCK_METHOD3(onTurnEnd, void(PlayerColor, Turn, GameState));
	MOCK_METHOD2(onTurnTimeout, void(PlayerColor, std::chrono::seconds));
	MOCK_METHOD2(onGameOver, void(GameState, PlayerColor));

	// AbstractPlayer Interface
	MOCK_METHOD1(onSetColor, void(PlayerColor));
	//MOCK_METHOD1(doMakeTurn, std::future<Turn>(GameState));
	std::future<Turn> doMakeTurn(GameState state) override {
		return std::future<Turn>();
	}
	MOCK_METHOD0(doAbortTurn, void());
};

TEST(PlayerDispatcherProxy, checkPollingBehavior) {
	using ::testing::_;

	auto mockPlayer = make_shared<MockPlayer>();

	PlayerDispatcherProxy proxy(mockPlayer);

	EXPECT_CALL(*mockPlayer, onSetColor(_)).Times(0);
	//EXPECT_CALL(*mockPlayer, doMakeTurn(_)).Times(0);
	EXPECT_CALL(*mockPlayer, doAbortTurn()).Times(0);
	EXPECT_CALL(*mockPlayer, onGameStart(_,_)).Times(0);
	EXPECT_CALL(*mockPlayer, onTurnStart(_)).Times(0);
	EXPECT_CALL(*mockPlayer, onTurnEnd(_,_,_)).Times(0);
	EXPECT_CALL(*mockPlayer, onTurnTimeout(_,_)).Times(0);
	EXPECT_CALL(*mockPlayer, onGameOver(_,_)).Times(0);

	GameState s;
	GameConfiguration c;
	Turn t;
	auto to = chrono::seconds(5);

	proxy.onGameStart(s,c);
	proxy.onTurnStart(PlayerColor::White);
	proxy.onTurnEnd(PlayerColor::Black, t, s);

	EXPECT_CALL(*mockPlayer, onGameStart(s,c)).Times(1);
	EXPECT_CALL(*mockPlayer, onTurnStart(PlayerColor::White)).Times(1);
	EXPECT_CALL(*mockPlayer, onTurnEnd(PlayerColor::Black, t, s)).Times(1);

	proxy.poll();

	proxy.onTurnTimeout(PlayerColor::White, to);
	proxy.onGameOver(s, PlayerColor::Black);

	EXPECT_CALL(*mockPlayer, onTurnTimeout(PlayerColor::White, to)).Times(1);
	EXPECT_CALL(*mockPlayer, onGameOver(s, PlayerColor::Black)).Times(1);

	proxy.poll();

	EXPECT_CALL(*mockPlayer, onSetColor(PlayerColor::Black)).Times(1);
	//EXPECT_CALL(*mockPlayer, doMakeTurn(_)).Times(1);
	EXPECT_CALL(*mockPlayer, doAbortTurn()).Times(1);

	proxy.onSetColor(PlayerColor::Black);
	proxy.doAbortTurn();

	proxy.poll();
}

TEST(ObserverDispatchProxy, checkPollingBehavior) {

	using ::testing::_;

	auto mockObserver = make_shared<MockObserver>();

	ObserverDispatcherProxy proxy(mockObserver);

	EXPECT_CALL(*mockObserver, onGameStart(_,_)).Times(0);
	EXPECT_CALL(*mockObserver, onTurnStart(_)).Times(0);
	EXPECT_CALL(*mockObserver, onTurnEnd(_,_,_)).Times(0);
	EXPECT_CALL(*mockObserver, onTurnTimeout(_,_)).Times(0);
	EXPECT_CALL(*mockObserver, onGameOver(_,_)).Times(0);

	GameState s;
	GameConfiguration c;
	Turn t;
	auto to = chrono::seconds(5);

	proxy.onGameStart(s,c);
	proxy.onTurnStart(PlayerColor::White);
	proxy.onTurnEnd(PlayerColor::Black, t, s);

	EXPECT_CALL(*mockObserver, onGameStart(s,c)).Times(1);
	EXPECT_CALL(*mockObserver, onTurnStart(PlayerColor::White)).Times(1);
	EXPECT_CALL(*mockObserver, onTurnEnd(PlayerColor::Black, t, s)).Times(1);

	proxy.poll();

	proxy.onTurnTimeout(PlayerColor::White, to);
	proxy.onGameOver(s, PlayerColor::Black);

	EXPECT_CALL(*mockObserver, onTurnTimeout(PlayerColor::White, to)).Times(1);
	EXPECT_CALL(*mockObserver, onGameOver(s, PlayerColor::Black)).Times(1);

	proxy.poll();
}

TEST(GameConfiguration, roundTrip) {
	GameConfiguration config;
	config.maximumTurnTimeInSeconds = 10;

	std::string s = "GameConfigurationRoundTrip.xml";

	config.save(s);

	auto loadedConfig = config.load(s);
	ASSERT_TRUE(loadedConfig);

	ASSERT_EQ(config, *loadedConfig);
	ASSERT_EQ(config.maximumTurnTimeInSeconds, loadedConfig->maximumTurnTimeInSeconds);

	auto nonExistingConfig = config.load("doesnotexists.xml");
	ASSERT_FALSE(nonExistingConfig);
}
*/
