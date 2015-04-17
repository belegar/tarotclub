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

static const std::string SERVER_CONFIG_VERSION  = "5"; // increase the version to force any incompatible update in the file structure
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
                bool boolVal;
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

                if (json.GetValue("local_host_only", boolVal))
                {
                    mOptions.localHostOnly = boolVal;
                }

                // Setup tournament configuration
                mOptions.tournament.clear();
                std::vector<JsonValue> tournament = json.GetArray("tournament");
                if (tournament.size() > 0U)
                {
                    for (std::vector<JsonValue>::iterator iter = tournament.begin(); iter != tournament.end(); ++iter)
                    {
                        if (iter->IsObject())
                        {
                            JsonObject *obj = iter->GetObject();
                            std::string value;
                            Tarot::Shuffle shuffle;
                            if (JsonValue::FromNode(obj->GetNode("type"), value))
                            {
                                if (value == "custom")
                                {
                                    shuffle.type = Tarot::Shuffle::CUSTOM_DEAL;
                                    if (JsonValue::FromNode(obj->GetNode("file"), value))
                                    {
                                        shuffle.file = value;
                                    }
                                    else
                                    {
                                        ret = false;
                                    }
                                }
                                else
                                {
                                    // FIXME: add support for numbered deal (get the seed)
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
    json.CreateValuePair("game_tcp_port", mOptions.game_tcp_port);
    json.CreateValuePair("lobby_max_conn", mOptions.lobby_max_conn);
    json.CreateValuePair("local_host_only", mOptions.localHostOnly);

    JsonArray *tournament = json.CreateArrayPair("tournament");
    for (std::vector<Tarot::Shuffle>::iterator iter =  mOptions.tournament.begin(); iter !=  mOptions.tournament.end(); ++iter)
    {
        std::string type;
        std::string file;
        JsonObject *obj = tournament->CreateObject();

        if (iter->type == Tarot::Shuffle::RANDOM_DEAL)
        {
            type = "random";
            file = "";
        }
        else
        {
            // FIXME: add the numbered type
            type = "custom";
            file = iter->file;
        }
        obj->CreateValuePair("type", type);
        obj->CreateValuePair("file", file);
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

    opt.game_tcp_port   = DEFAULT_GAME_TCP_PORT;
    opt.web_tcp_port    = DEFAULT_WEB_TCP_PORT;
    opt.lobby_max_conn  = DEFAULT_LOBBY_MAX_CONN;
    opt.localHostOnly   = false;
    opt.tables.push_back("Table 1"); // default table name (one table minimum)

    // Default tournament is some random deals
    for (std::uint32_t i = 0U; i < DEFAULT_NUMBER_OF_TURNS; i++)
    {
        opt.tournament.push_back(Tarot::Shuffle());
    }
    return opt;
}

//=============================================================================
// End of file ServerConfig.cpp
//=============================================================================
