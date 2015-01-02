//=============================================================================
// TarotClub - Defines.h
//=============================================================================
// Basic definitions of global parameters (need a separate file to allow parsing)
//=============================================================================
// TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
// Copyright (C) 2003-2999 - Anthony Rabine
// anthony@tarotclub.fr
//
// TarotClub is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TarotClub is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
//
//=============================================================================


#ifndef DEFINES_H
#define DEFINES_H

#define VERSION_STRING "2.5.1"

#ifndef ISPP_INVOKED
/// @brief The version string uses Semantic Versioning format
/// @see http://semver.org
static const std::string TAROT_VERSION  = VERSION_STRING;
static const std::string TAROT_TITLE    = "TarotClub";
static const std::string TAROT_VNAME    = "Pirate!";

static const std::uint32_t NB_LANGUAGE  = 2U;
#endif

#endif // DEFINES_H

//=============================================================================
// End of file Defines.h
//=============================================================================
