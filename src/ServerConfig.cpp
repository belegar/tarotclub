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
static const std::string SERVER_CONFIG_FILE     = "tcds.json";

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
        Load();
    }
    return mOptions;
}
/*****************************************************************************/
void ServerConfig::SetOptions(const ServerOptions &newOptions)
{
    mOptions = newOptions;
}
/*****************************************************************************/
bool ServerConfig::Load()
{
    JsonReader json;

    bool ret = json.Open(System::HomePath() + SERVER_CONFIG_FILE);
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
                if (json.GetValue("tcp_port", intval))
                {
                    mOptions.tcp_port = intval;
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
        ret = Save();
    }

    mLoaded = true;
    return ret;
}
/*****************************************************************************/
bool ServerConfig::Save()
{
    bool ret = true;

    JsonWriter json;

    json.CreateValuePair("version", SERVER_CONFIG_VERSION);
    json.CreateValuePair("generator", "Generated by TarotClub " + TAROT_VERSION);
    json.CreateValuePair("delay", mOptions.timer);
    json.CreateValuePair("tcp_port", mOptions.tcp_port);

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

    if (!json.SaveToFile(System::HomePath() + SERVER_CONFIG_FILE))
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

    opt.timer       = DEFAULT_DELAY;
    opt.tcp_port    = DEFAULT_TCP_PORT;

    opt.bots[Place::WEST].name     = "Leela";
    opt.bots[Place::WEST].avatar   = ":/images/avatars/leela.png";
    opt.bots[Place::WEST].quote    = "No, this isn't mutant language. We use a lot more profanity.";
    opt.bots[Place::WEST].gender   = Identity::FEMALE;

    opt.bots[Place::NORTH].name    = "Bender";
    opt.bots[Place::NORTH].avatar  = ":/images/avatars/bender.png";
    opt.bots[Place::NORTH].quote   = "Afterlife? If I'd thought I had to go through a whole another life, I'd kill myself right now.";
    opt.bots[Place::NORTH].gender  = Identity::MALE;

    opt.bots[Place::EAST].name     = "Zoidberg";
    opt.bots[Place::EAST].avatar   = ":/images/avatars/zoidberg.png";
    opt.bots[Place::EAST].quote    = "A buffet... oh if only I'd brought my wallet with me...";
    opt.bots[Place::EAST].gender   = Identity::MALE;

    return opt;
}

//=============================================================================
// End of file ServerConfig.cpp
//=============================================================================
