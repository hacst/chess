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
#ifndef ABSTRACTPLAYER_H
#define ABSTRACTPLAYER_H

#include <future>
#include "logic/interface/AbstractGameObserver.h"

/**
 * @brief Class a player has to implement to interact with the GameLogic.
 * Every player is also a AbstractGameObserver which is notified of
 * relevant game events. You do not need to register the player as an
 * observer for this to happen.
 * 
 * @note A Observer is only required to stay in a valid state for one game.
 * It is free to halt its operations after the end of the game.
 *  
 * @warning None of the functions in the class must block.
 */
class AbstractPlayer : public AbstractGameObserver {
public:
    virtual ~AbstractPlayer() { /* Nothing */ }

    /**
     * @brief Notifies that player what color he will be playing.
     * Called before onGameStart.
     * @param color Color the player has.
     */
    virtual void onSetColor(PlayerColor color) = 0;
    
    /**
     * @brief Asks the player to make his turn.
     * @warning This function must not block. It is to return immediatly.
     *       The players turn is to be set on the returned future.
     * @note The game logic can abort its request for a player to make
     *       his turn using the doAbortTurn function at any time.
     * @param state Current state of the game.
     * @return A future to the turn to make.
     */
    virtual std::future<Turn> doMakeTurn(GameState state) = 0;
    
    /**
     * @brief Asks the player to abort a turn asked for with doMakeTurn.
     * When this is called the GameLogic will no longer react to the
     * completion of the future for that turn. A use of this function
     * is the abortion of a turn due to timeout.
     */
	virtual void doAbortTurn() = 0;
};

using AbstractPlayerPtr = std::shared_ptr<AbstractPlayer>;


#endif // ABSTRACTPLAYER_H
