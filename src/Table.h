/*=============================================================================
 * TarotClub - Table.h
 *=============================================================================
 * Manage places around the Tarot table
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

#ifndef TABLE_H
#define TABLE_H

#include <QtCore>
#include "defines.h"

/*****************************************************************************/
class Table
{
   QMap<Place, bool> table; // occuped places around table
public:
   Table() {
      freePlace(BROADCAST);
   }

   Place reserveFreePlace() {
      Place p = HYPERSPACE;

      QMapIterator<Place, bool> i(table);
       while (i.hasNext()) {
           i.next();
           if (i.value() == true) {
              p = i.key();
              table.insert(i.key(), false);
              break;
           }
       }
       return p;
   }

   // free one place or all places
   void freePlace(Place p) {
      if (p == BROADCAST) {
         for( int i=0; i<NB_PLAYERS; i++) {
            table.insert((Place)i, true);
         }
      } else {
         table.insert(p, true);
      }

   }

};

#endif // TABLE_H

//=============================================================================
// End of file Table.h
//=============================================================================
