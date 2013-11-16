#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "logic/threading/PlayerDispatcherProxy.h"
#include "core/GameConfiguration.h"

using namespace std;

class MockObserver : public AbstractGameObserver {
public:
	MOCK_METHOD2(onGameStart, void(State,GameConfiguration));
	MOCK_METHOD1(onTurnStart, void(PlayerColor));
	MOCK_METHOD3(onTurnEnd, void(PlayerColor, Turn, State));
	MOCK_METHOD2(onTurnTimeout, void(PlayerColor, std::chrono::seconds));
	MOCK_METHOD2(onGameOver, void(State, PlayerColor));
};

class MockPlayer : public AbstractPlayer {
public:
	// AbstractObserver Interface
	MOCK_METHOD2(onGameStart, void(State,GameConfiguration));
	MOCK_METHOD1(onTurnStart, void(PlayerColor));
	MOCK_METHOD3(onTurnEnd, void(PlayerColor, Turn, State));
	MOCK_METHOD2(onTurnTimeout, void(PlayerColor, std::chrono::seconds));
	MOCK_METHOD2(onGameOver, void(State, PlayerColor));
	
	// AbstractPlayer Interface
	MOCK_METHOD1(onSetColor, void(PlayerColor));
	//MOCK_METHOD1(doMakeTurn, std::future<Turn>(State));
	std::future<Turn> doMakeTurn(State state) override {
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
	
	State s;
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
	
	State s;
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
