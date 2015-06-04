/*=============================================================================
 * TarotClub - Client.cpp
 *=============================================================================
 * This class manages the network protocol and is used to join a online game
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

#include "Client.h"
#include "Identity.h"
#include "Log.h"

/*****************************************************************************/
Client::Client(IEvent &handler)
    : mNbPlayers(4U)
    , mTableId(0U)
    , mSequence(STOPPED)
    , mEventHandler(handler)
{

}
/*****************************************************************************/
void Client::Initialize()
{
    mSequence = STOPPED;
    mTablePlayers.clear();
    mPlace = Place::NOWHERE;
}
/*****************************************************************************/
bool Client::TestHandle(const Deck &handle)
{
    return mPlayer.TestHandle(handle);
}
/*****************************************************************************/
bool Client::TestDiscard(const Deck &discard)
{
    return mPlayer.TestDiscard(discard, mDog, mNbPlayers);
}
/*****************************************************************************/
Contract Client::CalculateBid()
{
    int total = 0;
    Contract cont;

    UpdateStatistics();

    // Set points according to the card values
    if (mStats.bigTrump == true)
    {
        total += 9;
    }
    if (mStats.fool == true)
    {
        total += 7;
    }
    if (mStats.littleTrump == true)
    {
        if (mStats.trumps == 5)
        {
            total += 5;
        }
        else if (mStats.trumps == 6 || mStats.trumps == 7)
        {
            total += 7;
        }
        else if (mStats.trumps > 7)
        {
            total += 8;
        }
    }

    // Each trump is 1 point
    // Each major trump is 1 more point
    total += mStats.trumps * 2;
    total += mStats.majorTrumps * 2;
    total += mStats.kings * 6;
    total += mStats.queens * 3;
    total += mStats.knights * 2;
    total += mStats.jacks;
    total += mStats.weddings;
    total += mStats.longSuits * 5;
    total += mStats.cuts * 5;
    total += mStats.singletons * 3;
    total += mStats.sequences * 4;

    // We can decide the bid
    if (total <= 35)
    {
        cont = Contract::PASS;
    }
    else if (total >= 36  && total <= 50)
    {
        cont = Contract::TAKE;
    }
    else if (total >= 51  && total <= 65)
    {
        cont = Contract::GUARD;
    }
    else if (total >= 66  && total <= 75)
    {
        cont = Contract::GUARD_WITHOUT;
    }
    else
    {
        cont = Contract::GUARD_AGAINST;
    }
    return cont;
}
/*****************************************************************************/
void Client::UpdateStatistics()
{
    mStats.Reset();
    mPlayer.AnalyzeTrumps(mStats);
    mPlayer.AnalyzeSuits(mStats);
}
/*****************************************************************************/
Card Client::Play()
{
    Card c;

    for (Deck::ConstIterator it = mPlayer.Begin(); it != mPlayer.End(); ++it)
    {
        c = (*it);
        if (IsValid(c) == true)
        {
            break;
        }
    }
    return c;
}
/*****************************************************************************/
bool Client::IsValid(const Card &c)
{
    return mPlayer.CanPlayCard(c, currentTrick);
}
/*****************************************************************************/
Deck Client::AutoDiscard()
{
    Deck discard = mPlayer.AutoDiscard(mDog, mNbPlayers);
    mPlayer.RemoveDuplicates(discard);
    TLogInfo("Auto discard: " + discard.ToString());
    return discard;
}
/*****************************************************************************/
bool Client::DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data)
{
    (void) src_uuid;
    bool ret = true;
    ByteStreamReader in(data);

    std::stringstream dbg;
    dbg << "Client command received: 0x" << std::hex << (int)cmd;
    TLogNetwork(dbg.str());

    switch (cmd)
    {

    case Protocol::LOBBY_CHAT_MESSAGE:
    {
        std::string message;
        in >> message;

        if (dest_uuid == mTableId)
        {
            mEventHandler.TableMessage(message);
        }
        else
        {
            mEventHandler.LobbyMessage(message);
        }
        break;
    }

    case Protocol::LOBBY_DISCONNECT:
    {
        mEventHandler.KickedFromLobby();
        break;
    }

    case Protocol::LOBBY_REQUEST_LOGIN:
    {
        std::uint32_t myUuid;
        in >> myUuid;
        mPlayer.SetUuid(myUuid);
        mEventHandler.RequestLogin();
        break;
    }

    case Protocol::LOBBY_LOGIN_RESULT:
    {
        bool accepted;
        in >> accepted;
        if (accepted)
        {
            std::uint32_t tableSize;
            in >> tableSize;

            mTables.clear();
            for (std::uint32_t i = 0U; i < tableSize; i++)
            {
                std::string name;
                std::uint32_t id;

                in >> name;
                in >> id;
                mTables[name] = id;
            }
            mEventHandler.EnteredLobby();
        }
        else
        {
            mEventHandler.Error(IEvent::ErrLobbyAccessRefused);
            TLogError("Lobby has rejected the login.");
        }
        break;
    }

    case Protocol::LOBBY_PLAYERS_LIST:
    {
        std::uint8_t size;
        in >> size;

        mLobbyUsers.clear();
        for (int i = 0; i < size; i++)
        {
            Identity ident;
            std::uint32_t uuid;

            in >> uuid;
            in >> ident;
            mLobbyUsers[uuid] = ident;
        }
        mEventHandler.LobbyPlayersList();
        break;
    }

    case Protocol::ADMIN_GAME_FULL:
    {
        bool full;
        in >> full;
        mEventHandler.AdminGameFull();
        break;
    }
    case Protocol::TABLE_ERROR_FULL:
    {
        mTableId = 0U;
        mEventHandler.Error(IEvent::ErrTableFull);
        break;
    }

    case Protocol::LOBBY_JOIN_TABLE_REPLY:
    {
        bool status;
        in >> mTableId;
        in >> status;
        in >> mPlace;
        in >> mNbPlayers;
        if (status)
        {
            mEventHandler.TableJoinEvent(mTableId);
        }
        else
        {
            mTableId = 0U;
            mEventHandler.Error(IEvent::ErrTableAccessRefused);
        }
        break;
    }

    case Protocol::TABLE_QUIT_EVENT:
    {
        Initialize();
        mEventHandler.TableQuitEvent(mTableId);
        mTableId = 0U;
        break;
    }

    case Protocol::TABLE_PLAYERS_LIST:
    {
        std::uint8_t size;
        in >> size;

        mTablePlayers.clear();
        for (int i = 0; i < size; i++)
        {
            std::uint32_t id;
            Place place;

            in >> place;
            in >> id;
            mTablePlayers[place] = id;
        }
        mEventHandler.TablePlayersList();
        break;
    }

    case Protocol::TABLE_NEW_GAME:
    {
        in >> mGame;
        mEventHandler.NewGame();
        break;
    }

    case Protocol::TABLE_NEW_DEAL:
    {
        mPlayer.Clear();
        in >> mPlayer;

        if (mPlayer.Size() == Tarot::NumberOfCardsInHand(mNbPlayers))
        {
            mPoints.Clear();
            UpdateStatistics();
            mEventHandler.NewDeal();
        }
        else
        {
            TLogError("Wrong number of cards received!");
        }
        break;
    }

    case Protocol::TABLE_REQUEST_BID:
    {
        std::uint8_t c;
        Place p;

        in >> c; // Most important contract announced before
        in >> p;
        mEventHandler.SelectPlayer(p);
        if (p == mPlace)
        {
            // The request bid is for us! We must declare something
            mEventHandler.RequestBid((Contract)c);
        }
        break;
    }

    case Protocol::TABLE_SHOW_PLAYER_BID:
    {
        std::uint8_t c, slam;
        Place p;

        in >> p;
        in >> c;
        in >> slam;
        mEventHandler.ShowBid(p, (slam == 1 ? true : false), (Contract)c);
        break;
    }

    case Protocol::TABLE_ALL_PASSED:
    {
        mEventHandler.AllPassed();
        break;
    }

    case Protocol::TABLE_SHOW_DOG:
    {
        in >> mDog;
        mDog.Sort("TDSCH");
        mSequence = SHOW_DOG;
        mEventHandler.ShowDog();
        break;
    }

    case Protocol::TABLE_ASK_FOR_DISCARD:
    {
        mSequence = BUILD_DISCARD;
        mEventHandler.BuildDiscard();
        break;
    }

    case Protocol::TABLE_START_DEAL:
    {
        Place first;
        in >> first;
        in >> mBid.taker;
        in >> mBid.contract;
        in >> mBid.slam;
        in >> mShuffle;

        currentTrick.Clear();
        UpdateStatistics(); // cards in hand can have changed due to the dog
        mSequence = SYNC_START;
        mEventHandler.StartDeal();
        break;
    }

    case Protocol::TABLE_ASK_FOR_HANDLE:
    {
        mSequence = BUILD_HANDLE;
        mEventHandler.AskForHandle();
        break;
    }

    case Protocol::TABLE_SHOW_HANDLE:
    {
        Place p;
        in >> p;
        in >> handleDeck;
        if (mBid.taker == p)
        {
            handleDeck.SetOwner(ATTACK);
        }
        else
        {
            handleDeck.SetOwner(DEFENSE);
        }
        mSequence = SHOW_HANDLE;
        mEventHandler.ShowHandle();
        break;
    }

    case Protocol::TABLE_SHOW_CARD:
    {
        std::string name;
        Place player;

        in >> player;
        in >> name;

        currentTrick.Append(Card(name));
        mSequence = SYNC_CARD;
        mEventHandler.ShowCard(player, name);
        break;
    }

    case Protocol::TABLE_PLAY_CARD:
    {
        Place p;
        in >> p;

        mEventHandler.SelectPlayer(p);
        if (p == mPlace)
        {
            // Our turn to play a card
            mSequence = PLAY_TRICK;
            mEventHandler.PlayCard();
        }
        break;
    }

    case Protocol::TABLE_END_OF_TRICK:
    {
        Place winner;
        in >> winner;
        mSequence = SYNC_TRICK;
        mEventHandler.WaitTrick(winner);
        currentTrick.Clear();
        break;
    }

    case Protocol::TABLE_END_OF_DEAL:
    {
        in >> mPoints;
        in >> mResult;

        mSequence = SHOW_SCORE;
        mEventHandler.EndOfDeal();
        break;
    }

    case Protocol::TABLE_END_OF_GAME:
    {
        Place winner;
        in >> winner;
        mEventHandler.EndOfGame(winner);
        break;
    }

    default:
        std::string msg = mIdentity.nickname + ": Unknown packet received.";
        TLogInfo(msg);
        break;
    }
    return ret;
}


//=============================================================================
// End of file Client.cpp
//=============================================================================
