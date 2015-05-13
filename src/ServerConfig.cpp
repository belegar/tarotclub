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

static const std::string SERVER_CONFIG_VERSION  = "6"; // increase the version to force any incompatible update in the file structure
const std::string ServerConfig::DEFAULT_SERVER_CONFIG_FILE  = "tcds.json";
const std::string ServerConfig::DEFAULT_SERVER_NAME = "desktop";

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
    JsonValue json;

    bool ret = JsonReader::ParseFile(json, fileName);
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
                bool boolVal;
                if (json.GetValue("game_tcp_port", unsignedVal))
                {
                    mOptions.game_tcp_port = unsignedVal;
                }

                if (json.GetValue("console_tcp_port", unsignedVal))
                {
                    mOptions.console_tcp_port = unsignedVal;
                }

                if (json.GetValue("lobby_max_conn", unsignedVal))
                {
                    mOptions.lobby_max_conn = unsignedVal;
                }

                if (json.GetValue("local_host_only", boolVal))
                {
                    mOptions.localHostOnly = boolVal;
                }

                if (json.GetValue("name", value))
                {
                    mOptions.name = value;
                }

                // Setup tournament configuration
                mOptions.tournament.clear();
                JsonValue tournament = json.FindValue("tournament");
                if (tournament.GetArray().Size() > 0U)
                {
                    for (JsonArray::Iterator iter = tournament.GetArray().Begin(); iter != tournament.GetArray().End(); ++iter)
                    {
                        if (iter->IsObject())
                        {
                            JsonValue value = iter->GetObject().GetValue("type");
                            Tarot::Distribution shuffle;
                            if (value.IsString())
                            {
                                if (value.GetString() == "custom")
                                {
                                    shuffle.mType = Tarot::Distribution::CUSTOM_DEAL;
                                    value = iter->GetObject().GetValue("file");
                                    if (value.IsString())
                                    {
                                        shuffle.mFile = value.GetString();
                                    }
                                    else
                                    {
                                        ret = false;
                                    }
                                }
                                else if (value.GetString() == "random")
                                {
                                    shuffle.mType = Tarot::Distribution::RANDOM_DEAL;
                                }
                                else  if (value.GetString() == "numbered")
                                {
                                    shuffle.mType = Tarot::Distribution::NUMBERED_DEAL;
                                    shuffle.mSeed = iter->GetObject().GetValue("number").GetInteger();
                                    // FIXME we can add a test on the type here before setting the seed
                                }
                                else
                                {
                                    TLogError("Unsupported deal type value");
                                    ret = false;
                                }
                            }

                            if (ret)
                            {
                                mOptions.tournament.push_back(shuffle);
                            }
                        }
                    }
                }
                else
                {
                    TLogError("No tournament details");
                    ret = false;
                }

                mOptions.tables.clear();
                JsonValue tables = json.FindValue("tables");
                if (tables.GetArray().Size() > 0U)
                {
                    for (JsonArray::Iterator iter = tables.GetArray().Begin(); iter != tables.GetArray().End(); ++iter)
                    {
                        if (iter->IsString())
                        {
                            mOptions.tables.push_back(iter->GetString());
                        }
                    }
                }
                else
                {
                    TLogError("No table defined");
                    ret = false;
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
        TLogError("Cannot open server configuration file" + fileName);
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

    JsonObject json;

    json.AddValue("version", SERVER_CONFIG_VERSION);
    json.AddValue("game_tcp_port", mOptions.game_tcp_port);
    json.AddValue("console_tcp_port", mOptions.console_tcp_port);
    json.AddValue("lobby_max_conn", mOptions.lobby_max_conn);
    json.AddValue("local_host_only", mOptions.localHostOnly);
    json.AddValue("name", mOptions.name);

    JsonArray tournament;
    for (std::vector<Tarot::Distribution>::iterator iter =  mOptions.tournament.begin(); iter !=  mOptions.tournament.end(); ++iter)
    {
        std::string type;
        std::string file;
        std::uint32_t number = 0U;
        JsonObject obj;

        if (iter->mType == Tarot::Distribution::RANDOM_DEAL)
        {
            type = "random";
            file = "";
        }
        else if (iter->mType == Tarot::Distribution::CUSTOM_DEAL)
        {
            type = "custom";
            file = iter->mFile;
        }
        else
        {
            type = "numbered";
            number = iter->mSeed;
        }

        obj.AddValue("type", type);
        obj.AddValue("file", file);
        obj.AddValue("number", number);
        tournament.AddValue(obj);
    }
    json.AddValue("tournament", tournament);

    JsonArray tables;
    for (std::vector<std::string>::iterator iter =  mOptions.tables.begin(); iter !=  mOptions.tables.end(); ++iter)
    {
        tables.AddValue(*iter);
    }
    json.AddValue("tables", tables);

    if (!JsonWriter::SaveToFile(json, fileName))
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

    opt.game_tcp_port       = DEFAULT_GAME_TCP_PORT;
    opt.console_tcp_port    = DEFAULT_CONSOLE_TCP_PORT;
    opt.lobby_max_conn      = DEFAULT_LOBBY_MAX_CONN;
    opt.localHostOnly       = false;
    opt.name                = DEFAULT_SERVER_NAME;
    opt.tables.push_back("Table 1"); // default table name (one table minimum)

    // Default tournament is some random deals
    for (std::uint32_t i = 0U; i < DEFAULT_NUMBER_OF_TURNS; i++)
    {
        opt.tournament.push_back(Tarot::Distribution());
    }
    return opt;
}

//=============================================================================
// End of file ServerConfig.cpp
//=============================================================================
