/*=============================================================================
 * TarotClub - DealFile.cpp
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

#include <random>
#include <sstream>
#include "JsonReader.h"
#include "JsonWriter.h"
#include "DealGenerator.h"
#include "Common.h"
#include "Log.h"

static const std::string DEAL_FILE_VERSION  = "3";

/*****************************************************************************/
DealGenerator::DealGenerator()
    : mNbPlayers(4U)
    , mSeed(0U)
{

}
/*****************************************************************************/
void DealGenerator::SetFirstPlayer(Place p)
{
    mFirstPlayer = p;
}
/*****************************************************************************/
bool DealGenerator::IsValid(std::uint8_t numberOfPlayers)
{
    bool valid = true;
    std::uint8_t cardsInHand = Tarot::NumberOfCardsInHand(numberOfPlayers);

    for (std::uint8_t i = 0U; i < numberOfPlayers; i++)
    {
        if (mPlayers[i].Size() != cardsInHand)
        {
            valid = false;
        }
    }

    if (mDogDeck.Size() != Tarot::NumberOfDogCards(numberOfPlayers))
    {
        valid = false;
    }

    return valid;
}
/*****************************************************************************/
bool DealGenerator::CreateRandomDeal(std::uint8_t numberOfPlayers, std::uint32_t seed)
{
    mNbPlayers = numberOfPlayers;
    mSeed = seed;

    bool valid = true;
    Deck deck;
    deck.CreateTarotDeck();
    deck.Shuffle(seed);

    Clear();

    // Simple protection
    if (mNbPlayers > 5U)
    {
        TLogError("Number of players not supported.");
        mNbPlayers = 4U;
    }

    int n = Tarot::NumberOfCardsInHand(mNbPlayers);
    for (std::uint32_t i = 0U; i < mNbPlayers; i++)
    {
        mPlayers[i].Append(deck.Mid(i * n, n));

        if (mPlayers[i].HasOnlyOneOfTrump())
        {
            valid = false;
            TLogInfo("Petit sec detected!");
        }
    }

    // Remaining cards go to the dog
    mDogDeck = deck.Mid(mNbPlayers * n);
    return valid;
}
/*****************************************************************************/
bool DealGenerator::CreateRandomDeal(std::uint8_t numberOfPlayers)
{
    std::chrono::system_clock::rep seed = std::chrono::system_clock::now().time_since_epoch().count(); // rep is long long
    return CreateRandomDeal(numberOfPlayers, static_cast<std::uint32_t>(seed));
}
/*****************************************************************************/
Place DealGenerator::RandomPlace(std::uint8_t numberOfPlayers)
{
    std::chrono::system_clock::rep seed = std::chrono::system_clock::now().time_since_epoch().count(); // rep is long long
    std::default_random_engine generator(static_cast<unsigned int>(seed));
    std::uniform_int_distribution<std::uint32_t> distribution(0, numberOfPlayers - 1U);
    return Place(static_cast<std::uint8_t>(distribution(generator)));
}
/*****************************************************************************/
const Deck &DealGenerator::GetDogDeck() const
{
    return mDogDeck;
}
/*****************************************************************************/
void DealGenerator::SetDogDeck(const Deck &deck)
{
    mDogDeck = deck;
}
/*****************************************************************************/
const Deck &DealGenerator::GetPlayerDeck(Place p) const
{
    return mPlayers[p.Value()];
}
/*****************************************************************************/
void DealGenerator::SetPlayerDeck(Place p, const Deck &deck)
{
    mPlayers[p.Value()] = deck;
}
/*****************************************************************************/
bool DealGenerator::LoadFile(const std::string &fileName)
{
    JsonValue json;

    Clear();

    bool ret = JsonReader::ParseFile(json, fileName);
    if (ret)
    {
        std::string stringval;
        if (json.GetValue("version", stringval))
        {
            if (stringval == DEAL_FILE_VERSION)
            {
                if (json.GetValue("Dog", stringval))
                {
                    (void) mDogDeck.SetCards(stringval);
                    if (json.GetValue("FirstPlayer", stringval))
                    {
                        mFirstPlayer = Place(stringval);
                        if (mFirstPlayer != Place(Place::NOWHERE))
                        {
                            ret = true;
                        }
                        else
                        {
                            ret = false;
                        }
                    }
                    else
                    {
                        TLogError("Cannot get first player");
                        ret = false;
                    }

                    mNbPlayers = 0U;
                    for (std::uint32_t i = 0U; i < 5U; i++)
                    {
                        if (ret)
                        {
                            Place player(i);
                            if (json.GetValue(player.ToString(), stringval))
                            {
                                (void) mPlayers[i].SetCards(stringval);
                                if (stringval.size() > 0)
                                {
                                    mNbPlayers++;
                                }
                            }
                            else
                            {
                                TLogError("Cannot get dog deck");
                                ret = false;
                            }
                        }
                        else
                        {
                            TLogError("Cannot parse deck string");
                            ret = false;
                            break;
                        }
                    }
                }
                else
                {
                    TLogError("Cannot get dog deck");
                }
            }
            else
            {
                TLogError("Wrong deal file version");
                ret = false;
            }
        }
        else
        {
            TLogError("Wrong deal file version");
            ret = false;
        }
    }

    return ret;
}
/*****************************************************************************/
void DealGenerator::SaveFile(const std::string &fileName)
{
    JsonObject json;
    Build(json);

    if (!JsonWriter::SaveToFile(json, fileName))
    {
        TLogError("Saving custom deal file failed.");
    }
}
/*****************************************************************************/
std::string DealGenerator::ToString()
{
    JsonObject json;
    Build(json);

    return json.ToString(0);
}
/*****************************************************************************/
void DealGenerator::Build(JsonObject &obj)
{
    obj.AddValue("version", DEAL_FILE_VERSION);
    obj.AddValue("FirstPlayer", mFirstPlayer.ToString());
    obj.AddValue("Dog", mDogDeck.ToString());

    for (std::uint32_t i = 0; i < 5; i++)
    {
        obj.AddValue(Place(i).ToString(), mPlayers[i].ToString());
    }
}
/*****************************************************************************/
void DealGenerator::Clear()
{
    for (std::uint32_t i = 0; i < 5; i++)
    {
        mPlayers[i].Clear();
    }

    mDogDeck.Clear();
}

//=============================================================================
// End of file DealFile.cpp
//=============================================================================
