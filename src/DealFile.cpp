/*=============================================================================
 * TarotClub - DealFile.cpp
 *=============================================================================
 * Manage saving and loading XML files of custom deals
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

#include <sstream>
#include "JsonReader.h"
#include "JsonWriter.h"
#include "DealFile.h"
#include "Common.h"
#include "Log.h"

static const std::string DEAL_FILE_VERSION  = "2.0";

/*****************************************************************************/
DealFile::DealFile()
{

}
/*****************************************************************************/
Deck &DealFile::GetNorthDeck()
{
    return mPlayers[Place::NORTH];
}
/*****************************************************************************/
void DealFile::SetFirstPlayer(Place p)
{
    mFirstPlayer = p;
}
/*****************************************************************************/
bool DealFile::IsValid(std::uint8_t numberOfPlayers)
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
Deck &DealFile::GetSouthDeck()
{
    return mPlayers[Place::SOUTH];
}
/*****************************************************************************/
Deck &DealFile::GetEastDeck()
{
    return mPlayers[Place::EAST];
}
/*****************************************************************************/
Deck &DealFile::GetWestDeck()
{
    return mPlayers[Place::WEST];
}
/*****************************************************************************/
Deck &DealFile::GetDogDeck()
{
    return mDogDeck;
}
/*****************************************************************************/
bool DealFile::LoadFile(const std::string &fileName)
{
    JsonReader json;

    bool ret = json.Open(fileName);
    if (ret)
    {
        mDogDeck.Clear();
        for (int i = 0; i < 5; i++)
        {
            mPlayers[i].Clear();
        }

        std::string stringval;
        if (json.GetValue("version", stringval))
        {
            if (stringval == DEAL_FILE_VERSION)
            {
                if (json.GetValue("Dog", stringval))
                {
                    (void) mDogDeck.SetCards(stringval);
                }
                else
                {
                    TLogError("Cannot get dog deck");
                    ret = false;
                }

                if (json.GetValue("FirstPlayer", stringval))
                {
                    mFirstPlayer = Place(stringval);
                    if (mFirstPlayer != Place::NOWHERE)
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

                for (std::uint32_t i = 0U; i < 5U; i++)
                {
                    if (ret)
                    {
                        Place player(i);
                        if (json.GetValue(player.ToString(), stringval))
                        {
                            (void) mPlayers[i].SetCards(stringval);
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
    json.Close();

    return ret;
}
/*****************************************************************************/
void DealFile::SaveFile(const std::string &fileName)
{
    JsonWriter json;

    json.CreateValuePair("version", DEAL_FILE_VERSION);
    json.CreateValuePair("generator", "Generated by TarotClub " + TAROT_VERSION);
    json.CreateValuePair("FirstPlayer", mFirstPlayer.ToString());
    json.CreateValuePair("Dog", mDogDeck.ToString());

    for (std::uint32_t i = 0; i < 5; i++)
    {
        Place player(i);
        json.CreateValuePair(player.ToString(), mPlayers[i].ToString());
    }

    if (!json.SaveToFile(fileName))
    {
        TLogError("Saving custom deal file failed.");
    }
}
/*****************************************************************************/
void DealFile::Clear()
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
