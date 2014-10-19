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

static const std::string SERVER_CONFIG_VERSION  = "2.0";
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
                std::int32_t intval;
                if (json.GetValue("delay", intval))
                {
                    if ((intval < 0) || (intval > 9000))
                    {
                        intval = DEFAULT_DELAY;
                    }
                    mOptions.timer = intval;
                }

                if (json.GetValue("table_tcp_port", intval))
                {
                    mOptions.table_tcp_port = intval;
                }
                if (json.GetValue("lobby_tcp_port", intval))
                {
                    mOptions.lobby_tcp_port = intval;
                }
                if (json.GetValue("lobby_max_conn", intval))
                {
                    mOptions.lobby_max_conn = intval;
                }

                for (std::uint32_t i = 1U; i < 4U; i++)
                {
                    Place bot(i);
                    if (json.GetValue(bot.ToString() + ":name", value))
                    {
                        mOptions.bots[i].name = value;
                    }
                    if (json.GetValue(bot.ToString() + ":avatar", value))
                    {
                        mOptions.bots[i].avatar = value;
                    }
                    if (json.GetValue(bot.ToString() + ":gender", value))
                    {
                        if (value == "female")
                        {
                            mOptions.bots[i].gender = Identity::FEMALE;
                        }
                        else
                        {
                            mOptions.bots[i].gender = Identity::MALE;
                        }

                    }
                    if (json.GetValue(bot.ToString() + ":quote", value))
                    {
                        mOptions.bots[i].quote = value;
                    }
                }

                mOptions.tables.clear();
                std::vector<JsonValue> tables = json.GetArray("tables", JsonValue::STRING);
                if (tables.size() > 0U)
                {
                    for (std::vector<JsonValue>::iterator iter = tables.begin(); iter != tables.end(); ++iter)
                    {
                        mOptions.tables.push_back(iter->GetString());
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
    json.CreateValuePair("table_tcp_port", mOptions.table_tcp_port);
    json.CreateValuePair("lobby_tcp_port", mOptions.lobby_tcp_port);
    json.CreateValuePair("lobby_max_conn", mOptions.lobby_max_conn);

    for (std::uint32_t i = 1U; i < 4U; i++)
    {
        Place bot(i);
        JsonObject *obj = json.CreateObjectPair(bot.ToString());
        obj->CreateValuePair("name", mOptions.bots[i].name);
        obj->CreateValuePair("avatar", mOptions.bots[i].avatar);
        std::string text;
        if (mOptions.bots[i].gender == Identity::MALE)
        {
            text = "male";
        }
        else
        {
            text = "female";
        }

        obj->CreateValuePair("gender", text);
        obj->CreateValuePair("quote", mOptions.bots[i].quote);
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
    opt.table_tcp_port  = DEFAULT_TABLE_TCP_PORT;
    opt.lobby_tcp_port  = DEFAULT_LOBBY_TCP_PORT;
    opt.lobby_max_conn  = DEFAULT_LOBBY_MAX_CONN;
    opt.tables.push_back("Default"); // default table name (one table minimum)

    opt.bots[Place::WEST].name     = "Leela";
    opt.bots[Place::WEST].avatar   = ":/avatars/14.svg";
    opt.bots[Place::WEST].quote    = "No, this isn't mutant language. We use a lot more profanity.";
    opt.bots[Place::WEST].gender   = Identity::FEMALE;

    opt.bots[Place::NORTH].name    = "Bender";
    opt.bots[Place::NORTH].avatar  = ":/avatars/03.svg";
    opt.bots[Place::NORTH].quote   = "Afterlife? If I'd thought I had to go through a whole another life, I'd kill myself right now.";
    opt.bots[Place::NORTH].gender  = Identity::MALE;

    opt.bots[Place::EAST].name     = "Amy";
    opt.bots[Place::EAST].avatar   = ":/avatars/18.svg";
    opt.bots[Place::EAST].quote    = "oooh! nice boots! Do they come in women's sizes?";
    opt.bots[Place::EAST].gender   = Identity::FEMALE;

    return opt;
}

//=============================================================================
// End of file ServerConfig.cpp
//=============================================================================
