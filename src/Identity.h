/*=============================================================================
 * TarotClub - Identity.h
 *=============================================================================
 * Identity of one player
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


#ifndef IDENTITY_H
#define IDENTITY_H

#include <QtCore>
#include "defines.h"

/*****************************************************************************/
class Identity
{
public:
   Identity() {
      sex = MALE;
   }

   QString  name;    // nom du joueur
   QString  quote;   // citation
   QString  avatar;  // Avatar (nom du fichier ressource)
   SexType  sex;     // homme=1, femme=0, comme à la sécu

   // surcharge opérateurs pour envoyer la classe sur le réseau
   friend QDataStream &operator<<( QDataStream &out, Identity &ident) {
      QFileInfo fi(ident.avatar);

      out << ident.name
          << fi.fileName()
          << ident.quote
          << (quint8)ident.sex;
      return out;
   }

   friend QDataStream &operator>>( QDataStream &in, Identity &ident) {
      quint8 var8;

      in >> ident.name;
      in >> ident.avatar;
      in >> ident.quote;
      in >> var8;
      ident.sex = (SexType)var8;

      return in;
   }

};

#endif // IDENTITY_H

//=============================================================================
// End of file Identity.h
//=============================================================================
