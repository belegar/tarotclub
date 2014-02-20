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

static const std::string CLIENT_CONFIG_VERSION  = "2.0";
static const std::string CLIENT_CONFIG_FILE     = "tarotclub.json";

/*****************************************************************************/
ClientConfig::ClientConfig()
{
    // Initialize supported languages
    mLang.push_back("fr");
    mLang.push_back("en");
    mLoaded = false;
    SetDefault(mOptions);
}
/*****************************************************************************/
ClientOptions &ClientConfig::GetOptions()
{
    if (!mLoaded)
    {
        Load();
    }
    return mOptions;
}
/*****************************************************************************/
std::string ClientConfig::GetLocale()
{
    if (!mLoaded)
    {
        Load();
    }
    if (mOptions.language >= mLang.size())
    {
        mOptions.language = 0;
    }
    return mLang[mOptions.language];
}
/*****************************************************************************/
void ClientConfig::SetOptions(ClientOptions &newOptions)
{
    mOptions = newOptions;
}
/*****************************************************************************/
bool ClientConfig::Load()
{
    bool ret = true;
    JsonReader json;

    ret = json.Open(Config::HomePath + CLIENT_CONFIG_FILE);
    if (ret)
    {
        std::string stringval;
        std::int32_t intval;
        bool boolval;
        if (json.GetValue("", "version", stringval))
        {
            if (stringval == CLIENT_CONFIG_VERSION)
            {
                // The general strategy is to be tolerant on the values.
                // If they are not in the acceptable range, we set the default value
                // without throwing any error

                if (json.GetValue("", "show_avatars", boolval))
                {
                    mOptions.showAvatars = boolval;
                }

                if (json.GetValue("", "background_color", stringval))
                {
                    mOptions.backgroundColor = stringval;
                }

                if (json.GetValue("", "language", intval))
                {
                    mOptions.language = intval;
                    if (mOptions.language >= mLang.size())
                    {
                        mOptions.language = 0;
                    }
                }

                if (json.GetValue("", "delay_before_cleaning", intval))
                {
                    mOptions.delayBeforeCleaning = intval;
                    if (mOptions.delayBeforeCleaning > 5000)
                    {
                        mOptions.delayBeforeCleaning = 5000;
                    }
                }

                if (json.GetValue("", "click_to_clean", boolval))
                {
                    mOptions.enableDelayBeforeCleaning = boolval;
                }

                if (json.GetValue("", "cards_order", stringval))
                {
                    mOptions.cardsOrder = stringval;
                }

                // Player's identity
                if (json.GetValue("identity", "name", stringval))
                {
                    mOptions.identity.name = stringval;
                }
                if (json.GetValue("identity", "avatar", stringval))
                {
                    mOptions.identity.avatar = stringval;
                }
                if (json.GetValue("identity", "gender", stringval))
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
                if (json.GetValue("identity", "quote", stringval))
                {
                    mOptions.identity.quote = stringval;
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
        SetDefault(mOptions);
        ret = Save();
    }

    mLoaded = true;
    return ret;
}
/*****************************************************************************/
bool ClientConfig::Save()
{
    bool ret = true;

    JsonWriter json;

    json.CreateValuePair("version", CLIENT_CONFIG_VERSION);
    json.CreateValuePair("generator", "Generated by TarotClub " TAROT_VERSION);

    json.CreateValuePair("show_avatars", mOptions.showAvatars);
    json.CreateValuePair("background_color", mOptions.backgroundColor);
    json.CreateValuePair("language", mOptions.language);
    json.CreateValuePair("delay_before_cleaning", mOptions.delayBeforeCleaning);
    json.CreateValuePair("click_to_clean", mOptions.enableDelayBeforeCleaning);
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

    obj->CreateValuePair("gender", text);
    obj->CreateValuePair("quote", mOptions.identity.quote);

    if (!json.SaveToFile(Config::HomePath + CLIENT_CONFIG_FILE))
    {
        ret = false;
        TLogError("Saving client's configuration failed.");
    }
    return ret;
}
/*****************************************************************************/
void ClientConfig::SetDefault(ClientOptions &opt)
{
    opt.showAvatars = AVATARS_DEF;
    opt.backgroundColor = "#008000";
    opt.language = 0;
    opt.deckFilePath = "default";
    opt.cardsOrder = "TCHDS";
    opt.delayBeforeCleaning = CLIENT_TIMER_DEF;
    opt.enableDelayBeforeCleaning = true;

    opt.identity.name = "Moi";
    opt.identity.avatar = ":/images/avatars/inconnu.png";
    opt.identity.quote = "L'inventeur de l'escalier habitait sûrement au premier étage.";
    opt.identity.gender = Identity::MALE;
}

//=============================================================================
// End of file ClientConfig.cpp
//=============================================================================
