/*=============================================================================
 * TarotClub - Protocol.cpp
 *=============================================================================
 * Network tarot game protocol
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

#include "Protocol.h"
#include <QCoreApplication>

/*****************************************************************************/
QByteArray Protocol::BuildCommand(QDataStream &in, Command cmd)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)cmd
        << in // command data
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    return block;
}

/*****************************************************************************/
void Protocol::DecodePacket(QDataStream &in, qint64 bytes)
{
    quint16 blockSize = 0;
    bool morePacket = false;
    unsigned int total = 0;

    for (;;)
    {
        if (blockSize == 0)
        {
            if (bytes < (qint64)sizeof(quint16))
            {
                break;
            }
            in >> blockSize;
        }

        // end of packet?
        if (blockSize == 0xFFFF)
        {
            if (morePacket == true)
            {
                blockSize = 0;
                morePacket = false;
                continue;
            }
            else
            {
                break;
            }
        }

        total += blockSize + 2;
        if (bytes < total)
        {
            break;
        }
        else if (bytes > total)
        {
            morePacket = true;
        }
        else
        {
            morePacket = false;
        }

        // Perform data management
        DoAction(in);
        blockSize = 0;
    }
}

//=============================================================================
// End of file Protocol.cpp
//=============================================================================
