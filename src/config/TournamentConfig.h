/*=============================================================================
 * TarotClub - TournamentConfig.h
 *=============================================================================
 * Tournament options and parameters
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */
#ifndef TOURNAMENT_CONFIG_H
#define TOURNAMENT_CONFIG_H

// Game includes
#include "Common.h"

/*****************************************************************************/
struct TournamentOptions
{
    std::vector<Tarot::Distribution> turns; // one game parameter per turn (vector size is the number of turns)
};

/*****************************************************************************/
class TournamentConfig
{

public:
    static const std::string    DEFAULT_FILE_NAME;
    static const std::uint8_t   DEFAULT_NUMBER_OF_TURNS     = 5U;
    static const std::uint8_t   MAX_NUMBER_OF_TURNS         = 40U;

    TournamentConfig();

    // Helpers
    bool     Load(const std::string &fileName);
    bool     Save(const std::string &fileName);

    // Getters
    TournamentOptions &GetOptions();

    // Setters
    void        SetOptions(const TournamentOptions &newOptions);
    static TournamentOptions GetDefault();

private:
    TournamentOptions mOptions;
    bool mLoaded;

};

#endif // TOURNAMENT_CONFIG_H

//=============================================================================
// End of file TournamentConfig.h
//=============================================================================
