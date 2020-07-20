/*=============================================================================
 * TarotClub - TournamentConfig.cpp
 *=============================================================================
 * Tournament options and parameters
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
#include "TournamentConfig.h"
#include "Log.h"
#include "System.h"

static const std::string TOURNAMENT_CONFIG_VERSION  = "1"; // increase the version to force any incompatible update in the file structure
const std::string TournamentConfig::DEFAULT_FILE_NAME  = "tournament.json";


/*****************************************************************************/
TournamentConfig::TournamentConfig()
    : mOptions(GetDefault())
    , mLoaded(false)
{

}
/*****************************************************************************/
TournamentOptions &TournamentConfig::GetOptions()
{
    if (!mLoaded)
    {
        mOptions = GetDefault();
    }
    return mOptions;
}
/*****************************************************************************/
void TournamentConfig::SetOptions(const TournamentOptions &newOptions)
{
    mOptions = newOptions;
}
/*****************************************************************************/
bool TournamentConfig::Load(const std::string &fileName)
{
    JsonValue json;

    bool ret = JsonReader::ParseFile(json, fileName);
    if (ret)
    {
        std::string value;
        if (json.GetValue("version", value))
        {
            if (value == TOURNAMENT_CONFIG_VERSION)
            {
                // Setup tournament configuration
                mOptions.turns.clear();
                JsonValue tournament = json.FindValue("tournament");
                if (tournament.GetArray().Size() > 0U)
                {
                    for (JsonArray::iterator iter = tournament.GetArray().begin(); iter != tournament.GetArray().end(); ++iter)
                    {
                        if (iter->IsObject())
                        {
                            JsonValue value = iter->GetObj().GetValue("type");
                            Tarot::Distribution shuffle;
                            if (value.IsString())
                            {
                                if (value.GetString() == "custom")
                                {
                                    shuffle.mType = Tarot::Distribution::CUSTOM_DEAL;
                                    value = iter->GetObj().GetValue("file");
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
                                    shuffle.mSeed = iter->GetObj().GetValue("number").GetInteger();
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
                                mOptions.turns.push_back(shuffle);
                            }
                        }
                    }
                }
                else
                {
                    TLogError("No tournament details");
                    ret = false;
                }
            }
            else
            {
                TLogError("Wrong tournament configuration file version");
                ret = false;
            }
        }
        else
        {
            TLogError("Cannot read tournament configuration file version");
            ret = false;
        }
    }
    else
    {
        TLogError("Cannot open tournament configuration file" + fileName);
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
bool TournamentConfig::Save(const std::string &fileName)
{
    bool ret = true;

    JsonObject json;

    json.AddValue("version", TOURNAMENT_CONFIG_VERSION);

    JsonArray tournament;
    for (std::vector<Tarot::Distribution>::iterator iter =  mOptions.turns.begin(); iter !=  mOptions.turns.end(); ++iter)
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

    if (!JsonWriter::SaveToFile(json, fileName))
    {
        ret = false;
        TLogError("Saving tournament's configuration failed.");
    }
    return ret;
}
/*****************************************************************************/
TournamentOptions TournamentConfig::GetDefault()
{
    TournamentOptions opt;

    // Default tournament is some random deals
    for (std::uint32_t i = 0U; i < DEFAULT_NUMBER_OF_TURNS; i++)
    {
        opt.turns.push_back(Tarot::Distribution());
    }
    return opt;
}

//=============================================================================
// End of file TournamentConfig.cpp
//=============================================================================
