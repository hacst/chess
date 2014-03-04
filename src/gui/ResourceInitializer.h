/*
    Copyright (c) 2013-2014, Patrick Hillert <silent@gmx.biz>

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

#ifndef RESOURCE_INITIALIZER_H
#define RESOURCE_INITIALIZER_H

#include "gui/ChessSet.h"
#include "gui/StateMachine.h"
#include "gui/GuiWindow.h"

/**
 * This class will initialize the chess figures/models and the chess board. It will
 * also show the status with a progress bar and the model which is loading.
 */
class ResourceInitializer {
public:
    /**
     * @brief Creates a new ResourceInitializer object.
     */
    ResourceInitializer();

    /**
     * @brief Loads the whole chess set, shows the progress bar and the file which is loaded.
     * @return The smart pointer to the chess set.
     */
    ChessSetPtr load();

private:
    //! The state machine for accessing the OpenGL context.
    StateMachine& m_fsm;

    //! The chess set to load.
    ChessSetPtr m_chessSet;

    //! The number of resources which are currently loaded.
    size_t m_resourcesLoaded;

    //! The total number of resources to load.
    size_t m_resourcesTotal;

    /**
     * @brief The callback function to call before a resource is loaded.
     * @param resourceName The name of the resource to load next.
     */
    void onBeforeLoadNextResource(string resourceName);
};

using ResourceInitializerPtr = std::shared_ptr<ResourceInitializer>;

#endif // RESOURCE_INITIALIZER_H