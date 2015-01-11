/*=============================================================================
 * TarotClub - ServerConfig.cpp
 *=============================================================================
 * Classe de gestion du fichier d'options en XML (serveur)
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
#include "ServerConfig.h"
#include "Log.h"
#include "System.h"

static const std::string SERVER_CONFIG_VERSION  = "2.2"; // increase the version to force any incompatible update in the file structure
const std::string ServerConfig::DEFAULT_SERVER_CONFIG_FILE  = "tcds.json";

/*****************************************************************************/
ServerConfig::ServerConfig()
    : mOptions(GetDefault())
    , mLoaded(false)
{

}
/*****************************************************************************/
ServerOptions &ServerConfig::GetOptions()
{
    if (!mLoaded)
    {
        mOptions = GetDefault();
    }
    return mOptions;
}
/*****************************************************************************/
void ServerConfig::SetOptions(const ServerOptions &newOptions)
{
    mOptions = newOptions;
}
/*****************************************************************************/
bool ServerConfig::Load(const std::string &fileName)
{
    JsonReader json;

    bool ret = json.Open(fileName);
    if (ret)
    {
        std::string value;
        if (json.GetValue("version", value))
        {
            if (value == SERVER_CONFIG_VERSION)
            {
                // The general strategy is to be tolerant on the values.
                // If they are not in the acceptable range, we set the default value
                // without throwing any error
                std::uint32_t unsignedVal;
                if (json.GetValue("delay", unsignedVal))
                {
                    if (unsignedVal > 9000U)
                    {
                        unsignedVal = DEFAULT_DELAY;
                    }
                    mOptions.timer = unsignedVal;
                }
                if (json.GetValue("game_tcp_port", unsignedVal))
                {
                    mOptions.game_tcp_port = unsignedVal;
                }

                if (json.GetValue("web_tcp_port", unsignedVal))
                {
                    mOptions.web_tcp_port = unsignedVal;
                }

                if (json.GetValue("lobby_max_conn", unsignedVal))
                {
                    mOptions.lobby_max_conn = unsignedVal;
                }

                if (json.GetValue("tournament_turns", unsignedVal))
                {
                    mOptions.tournamentTurns = static_cast<std::uint8_t>(unsignedVal);
                    if (mOptions.tournamentTurns > MAX_NUMBER_OF_TURNS)
                    {
                        mOptions.tournamentTurns = DEFAULT_NUMBER_OF_TURNS;
                    }
                }

                for (std::uint32_t i = 1U; i < 4U; i++)
                {
                    std::string botPos = Place(i).ToString();
                    if (json.GetValue(botPos + ":name", value))
                    {
                        mOptions.bots[i].identity.name = value;
                    }
                    if (json.GetValue(botPos + ":avatar", value))
                    {
                        mOptions.bots[i].identity.avatar = value;
                    }
                    if (json.GetValue(botPos + ":gender", value))
                    {
                        if (value == "female")
                        {
                            mOptions.bots[i].identity.gender = Identity::FEMALE;
                        }
                        else
                        {
                            mOptions.bots[i].identity.gender = Identity::MALE;
                        }

                    }

                    if (json.GetValue(botPos + ":bot_file_path", value))
                    {
                        mOptions.bots[i].scriptFilePath = value;
                    }
                }

                mOptions.tables.clear();
                std::vector<JsonValue> tables = json.GetArray("tables");
                if (tables.size() > 0U)
                {
                    for (std::vector<JsonValue>::iterator iter = tables.begin(); iter != tables.end(); ++iter)
                    {
                        if (iter->GetTag() == IJsonNode::STRING)
                        {
                            mOptions.tables.push_back(iter->GetString());
                        }
                    }
                }
                else
                {
                    mOptions.tables.push_back("Default");
                }
            }
            else
            {
                TLogError("Wrong server configuration file version");
                ret = false;
            }
        }
        else
        {
            TLogError("Cannot read server configuration file version");
            ret = false;
        }
    }
    else
    {
        TLogError("Cannot open server configuration file");
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
bool ServerConfig::Save(const std::string &fileName)
{
    bool ret = true;

    JsonWriter json;

    json.CreateValuePair("version", SERVER_CONFIG_VERSION);
    json.CreateValuePair("generator", "Generated by TarotClub " + TAROT_VERSION);
    json.CreateValuePair("delay", mOptions.timer);
    json.CreateValuePair("game_tcp_port", mOptions.game_tcp_port);
    json.CreateValuePair("lobby_max_conn", mOptions.lobby_max_conn);
    json.CreateValuePair("tournament_turns", mOptions.tournamentTurns);

    for (std::uint32_t i = 1U; i < 4U; i++)
    {
        Place bot(i);
        JsonObject *obj = json.CreateObjectPair(bot.ToString());
        obj->CreateValuePair("name", mOptions.bots[i].identity.name);

        // Make sure to use unix path separator
        Util::ReplaceCharacter(mOptions.bots[i].identity.avatar, "\\", "/");

        obj->CreateValuePair("avatar", mOptions.bots[i].identity.avatar);
        std::string text;
        if (mOptions.bots[i].identity.gender == Identity::MALE)
        {
            text = "male";
        }
        else
        {
            text = "female";
        }

        obj->CreateValuePair("gender", text);

        // Make sure to use unix path separator
        Util::ReplaceCharacter(mOptions.bots[i].scriptFilePath, "\\", "/");
        obj->CreateValuePair("bot_file_path", mOptions.bots[i].scriptFilePath);
    }

    JsonArray *array = json.CreateArrayPair("tables");
    for (std::vector<std::string>::iterator iter =  mOptions.tables.begin(); iter !=  mOptions.tables.end(); ++iter)
    {
        array->CreateValue(*iter);
    }

    if (!json.SaveToFile(fileName))
    {
        ret = false;
        TLogError("Saving server's configuration failed.");
    }
    return ret;
}
/*****************************************************************************/
ServerOptions ServerConfig::GetDefault()
{
    ServerOptions opt;

    opt.timer           = DEFAULT_DELAY;
    opt.game_tcp_port   = DEFAULT_GAME_TCP_PORT;
    opt.web_tcp_port    = DEFAULT_WEB_TCP_PORT;
    opt.lobby_max_conn  = DEFAULT_LOBBY_MAX_CONN;
    opt.tournamentTurns = DEFAULT_NUMBER_OF_TURNS;
    opt.tables.push_back("Default"); // default table name (one table minimum)

    opt.bots[Place::WEST].identity.name     = "Leela";
    opt.bots[Place::WEST].identity.avatar   = ":/avatars/14.svg";
    opt.bots[Place::WEST].identity.gender   = Identity::FEMALE;
    opt.bots[Place::WEST].scriptFilePath    = System::ScriptPath() + "noob.json";

    opt.bots[Place::NORTH].identity.name    = "Bender";
    opt.bots[Place::NORTH].identity.avatar  = ":/avatars/03.svg";
    opt.bots[Place::NORTH].identity.gender  = Identity::MALE;
    opt.bots[Place::NORTH].scriptFilePath   = System::ScriptPath() + "noob.json";

    opt.bots[Place::EAST].identity.name     = "Amy";
    opt.bots[Place::EAST].identity.avatar   = ":/avatars/18.svg";
    opt.bots[Place::EAST].identity.gender   = Identity::FEMALE;
    opt.bots[Place::EAST].scriptFilePath    = System::ScriptPath() + "noob.json";

    return opt;
}

//=============================================================================
// End of file ServerConfig.cpp
//=============================================================================
