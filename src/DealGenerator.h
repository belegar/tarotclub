/*=============================================================================
 * TarotClub - DealGenerator.h
 *=============================================================================
 * Create, load and save specific or random deals
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
#ifndef DEAL_GENERATOR_H
#define DEAL_GENERATOR_H

#include <string>
#include "Deck.h"
#include "JsonValue.h"

/*****************************************************************************/
/**
 * @brief The DealFile class
 *
 * Cette classe utilitaire permet de créer des donnes:
 *  - Soit à partir d'un fichier d'entrée au format JSON
 *  - Soit au hasard
 *
 */
class DealGenerator
{
public:
    DealGenerator();

    bool LoadFile(const std::string &fileName);
    void SaveFile(const std::string &fileName);
    std::string ToString();
    void Build(JsonObject &obj);

    void Clear();

    const Deck &GetDogDeck() const;
    void SetDogDeck(const Deck &deck);
    const Deck &GetPlayerDeck(Place p) const;
    void SetPlayerDeck(Place p, const Deck &deck);
    std::uint32_t GetSeed() { return mSeed; }

    void SetFirstPlayer(Place p);
    Place GetFirstPlayer() const { return mFirstPlayer; }

    std::uint8_t GetNumberOfPlayers() { return mNbPlayers; }

    bool IsValid(std::uint8_t numberOfPlayers);

    bool CreateRandomDeal(std::uint8_t numberOfPlayers, std::uint32_t seed);
    bool CreateRandomDeal(std::uint8_t numberOfPlayers);

    static Place RandomPlace(std::uint8_t numberOfPlayers);
private:
    Deck    mPlayers[5]; //!< five players max in Tarot
    Deck    mDogDeck;
    Place   mFirstPlayer;
    std::uint8_t mNbPlayers;
    std::uint32_t mSeed;
};

#endif // DEAL_GENERATOR_H

//=============================================================================
// End of file DealGenerator.h
//=============================================================================
