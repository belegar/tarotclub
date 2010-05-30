/*=============================================================================
 * TarotClub - DealEditorFile.h
 *=============================================================================
 * Manage saving and loading XML files of custom deals
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
#ifndef DEALEDITORFILE_H
#define DEALEDITORFILE_H

#include <QString>
#include "Deck.h"

/*****************************************************************************/
class DealEditorFile
{
public:
   DealEditorFile();

   void loadFile(QString fileName);
   void saveFile(QString fileName);

   Deck  southDeck;
   Deck  northDeck;
   Deck  westDeck;
   Deck  eastDeck;
   Deck  chienDeck;
   
};

#endif // DEALEDITORFILE_H

//=============================================================================
// End of file DealEditorFile.h
//=============================================================================
