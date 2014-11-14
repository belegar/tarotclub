/*=============================================================================
 * TarotClub - ClientConfig.cpp
 *=============================================================================
 * Option file parameters management for the client configuration
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
#include "ClientConfig.h"
#include "Log.h"
#include "System.h"

static const std::string CLIENT_CONFIG_VERSION  = "2.1";
const std::string    ClientConfig::DEFAULT_CLIENT_CONFIG_FILE  = "tarotclub.json";

/*****************************************************************************/
ClientConfig::ClientConfig()
{
    // Initialize supported languages
    mLang.push_back("fr");
    mLang.push_back("en");
    mLoaded = false;
    mOptions = GetDefault();
}
/*****************************************************************************/
ClientOptions ClientConfig::GetOptions()
{
    if (!mLoaded)
    {
        mOptions = GetDefault();
    }
    return mOptions;
}
/*****************************************************************************/
std::string ClientConfig::GetLocale()
{
    if (!mLoaded)
    {
        mOptions = GetDefault();
    }
    if (mOptions.language >= mLang.size())
    {
        mOptions.language = 0U;
    }
    return mLang[mOptions.language];
}
/*****************************************************************************/
void ClientConfig::SetOptions(const ClientOptions &newOptions)
{
    mOptions = newOptions;
}
/*****************************************************************************/
bool ClientConfig::Load(const std::string &fileName)
{
    JsonReader json;

    bool ret = json.Open(fileName);
    if (ret)
    {
        std::string stringval;
        std::int32_t intval;
        std::uint32_t unsignedVal;

        if (json.GetValue("version", stringval))
        {
            if (stringval == CLIENT_CONFIG_VERSION)
            {
                bool boolval;
                // The general strategy is to be tolerant on the values.
                // If they are not in the acceptable range, we set the default value
                // without throwing any error

                if (json.GetValue("show_avatars", boolval))
                {
                    mOptions.showAvatars = boolval;
                }

                if (json.GetValue("background_color", stringval))
                {
                    mOptions.backgroundColor = stringval;
                }

                if (json.GetValue("language", intval))
                {
                    mOptions.language = intval;
                    if (mOptions.language >= mLang.size())
                    {
                        mOptions.language = 0;
                    }
                }

                if (json.GetValue("delay_before_cleaning", unsignedVal))
                {
                    mOptions.delayBeforeCleaning = static_cast<std::uint16_t>(unsignedVal);
                    if (mOptions.delayBeforeCleaning > 5000U)
                    {
                        mOptions.delayBeforeCleaning = 5000U;
                    }
                }

                if (json.GetValue("click_to_clean", boolval))
                {
                    mOptions.clickToClean = boolval;
                }

                if (json.GetValue("cards_order", stringval))
                {
                    mOptions.cardsOrder = stringval;
                }

                // Player's identity
                if (json.GetValue("identity:name", stringval))
                {
                    mOptions.identity.name = stringval;
                }
                if (json.GetValue("identity:avatar", stringval))
                {
                    mOptions.identity.avatar = stringval;
                }
                if (json.GetValue("identity:gender", stringval))
                {
                    if (stringval == "female")
                    {
                        mOptions.identity.gender = Identity::FEMALE;
                    }
                    else
                    {
                        mOptions.identity.gender = Identity::MALE;
                    }

                }
                if (json.GetValue("identity:quote", stringval))
                {
                    mOptions.identity.quote = stringval;
                }

                std::vector<JsonValue> servers = json.GetArray("servers");
                mOptions.serverList.clear();
                for (std::uint32_t i = 0U; i < servers.size(); i++)
                {
                    if (servers[i].IsObject())
                    {
                        JsonObject *obj = servers[i].GetObject();
                        ServerInfo srv;

                        bool ret = JsonValue::FromNode(obj->GetNode("address"), srv.address);
                        if (ret) { ret = JsonValue::FromNode(obj->GetNode("game_port"), srv.game_tcp_port); }
                        if (ret) { ret = JsonValue::FromNode(obj->GetNode("web_port"), srv.web_tcp_port); }

                        if (ret)
                        {
                            mOptions.serverList.push_back(srv);
                        }
                    }
                }
            }
            else
            {
                TLogError("Wrong client configuration file version");
                ret = false;
            }
        }
        else
        {
            TLogError("Cannot read client configuration file version");
            ret = false;
        }
    }
    else
    {
        TLogError("Cannot open client configuration file");
    }

    if (!ret)
    {
        // Overwrite old file with default value
        mOptions = GetDefault();
        ret = Save(fileName);
    }

    mLoaded = true;
    return ret;
}
/*****************************************************************************/
bool ClientConfig::Save(const std::string &fileName)
{
    bool ret = true;

    JsonWriter json;

    json.CreateValuePair("version", CLIENT_CONFIG_VERSION);
    json.CreateValuePair("generator", "Generated by TarotClub " + TAROT_VERSION);

    json.CreateValuePair("show_avatars", mOptions.showAvatars);
    json.CreateValuePair("background_color", mOptions.backgroundColor);
    json.CreateValuePair("language", mOptions.language);
    json.CreateValuePair("delay_before_cleaning", mOptions.delayBeforeCleaning);
    json.CreateValuePair("click_to_clean", mOptions.clickToClean);
    json.CreateValuePair("cards_order", mOptions.cardsOrder);

    JsonObject *obj = json.CreateObjectPair("identity");
    obj->CreateValuePair("name", mOptions.identity.name);
    obj->CreateValuePair("avatar", mOptions.identity.avatar);
    std::string text;
    if (mOptions.identity.gender == Identity::MALE)
    {
        text = "male";
    }
    else
    {
        text = "female";
    }

    // List of servers
    JsonArray *servers = json.CreateArrayPair("servers");
    for (std::uint32_t i = 0U; i < mOptions.serverList.size(); i++)
    {
        ServerInfo inf =  mOptions.serverList[i];
        JsonObject *srv = servers->CreateObject();
        srv->CreateValuePair("address", inf.address);
        srv->CreateValuePair("game_port", (std::int32_t) inf.game_tcp_port);
        srv->CreateValuePair("web_port", (std::int32_t) inf.web_tcp_port);
    }

    obj->CreateValuePair("gender", text);
    obj->CreateValuePair("quote", mOptions.identity.quote);

    if (!json.SaveToFile(fileName))
    {
        ret = false;
        TLogError("Saving client's configuration failed.");
    }
    return ret;
}
/*****************************************************************************/
ClientOptions ClientConfig::GetDefault()
{
    const ServerInfo DefaultServers[2U] = {
      {"http://tarot.fun-center.fr", 4269, 8080 },
      {"http://tarotclub.duckdns.org", 4269, 8080 }
    };

    const std::uint32_t NumberOfServers = (sizeof(DefaultServers) / sizeof(ServerInfo));


    ClientOptions opt;

    opt.showAvatars = AVATARS_DEF;
    opt.backgroundColor = "#004f00";
    opt.language = 0;
    opt.deckFilePath = "default";
    opt.cardsOrder = "TCHDS";
    opt.delayBeforeCleaning = CLIENT_TIMER_DEF;
    opt.clickToClean = true;

    opt.identity.name = "Fry";
    opt.identity.avatar = ":/avatars/01.svg";
    opt.identity.quote = "But existing is practically all I do!";
    opt.identity.gender = Identity::MALE;

    for (std::uint32_t i = 0U; i < NumberOfServers; i++)
    {
        opt.serverList.push_back(DefaultServers[i]);
    }

    return opt;
}

//=============================================================================
// End of file ClientConfig.cpp
//=============================================================================
