/*=============================================================================
 * TarotClub - Table.h
 *=============================================================================
 * Manage places around the Tarot table
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
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