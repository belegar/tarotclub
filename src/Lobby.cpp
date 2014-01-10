/*=============================================================================
 * TarotClub - Lobby.cpp
 *=============================================================================
 * Manage temporary connections to join free game tables
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

#include "Lobby.h"
#include "Log.h"

/*****************************************************************************/
Lobby::Lobby()
    : mTcpServer(*this)
{
    saloons[0].name = "Noobs";
    saloons[1].name = "Normal";

    for (int j = 0; j < SERVER_MAX_SALOONS; j++)
    {
        for (int i = 0; i < SERVER_MAX_TABLES; i++)
        {
            saloons[j].tables[i].name = "Table " + QString().setNum(i + 1);
        }
    }
}
/*****************************************************************************/
void Lobby::Initialize()
{
    TarotDeck::Initialize();

    Game::Shuffle sh;
    sh.type = Game::RANDOM_DEAL;
    int tcpPort = DEFAULT_PORT;

    for (int j = 0; j < SERVER_MAX_SALOONS; j++)
    {
        for (int i = 0; i < SERVER_MAX_TABLES; i++)
        {
            saloons[j].tables[i].table.LoadConfiguration(tcpPort);
            saloons[j].tables[i].table.CreateGame(Game::ONE_DEAL);

            // Start all threads
            saloons[j].tables[i].thread.start();

            // Each table has a unique port
            tcpPort++;
        }
    }

    mTcpServer.listen(QHostAddress::Any, 4242);
    if (!mTcpServer.isListening())
    {
        cerr << "Failed to bind to port 4242";
        qApp->quit();
    }
    connect(&mTcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}
/*****************************************************************************/
void Lobby::WaitForEnd()
{
    mTcpServer.
}
/*****************************************************************************/
void Lobby::ClientClosed(int socket)
{

}
/*****************************************************************************/
void Lobby::ReadData(const std::string &data)
{
    // This slot is called when the client sent data to the server. The
    // server looks if it was a get request and sends a very simple ASCII
    QTcpSocket *s = qobject_cast<QTcpSocket *>(sender());
    if (s->canReadLine())
    {
        QString line = s->readLine();
        // remove new line character
        line.remove('\n');
        QStringList tokens = line.split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
        QTextStream os(s);
        if (tokens[0] == "GET")
        {
            if (tokens[1] == "INFOS")
            {
                QStringList list;

                for (int i = 0; i < SERVER_MAX_SALOONS; i++)
                {
                    list += saloons[i].name;
                }

                os << "SALOON:";
                os << list.join(',');
                os << "\n";
                os.flush();
            }
            else if (tokens[1] == "TABLES")
            {
                QStringList list;

                for (int i = 0; i < SERVER_MAX_SALOONS; i++)
                {
                    if (saloons[i].name == tokens[2])
                    {
                        for (int j = 0; j < SERVER_MAX_TABLES; j++)
                        {
                            list += saloons[i].tables[j].name;
                        }
                    }
                }

                os << "TABLES:";
                os << list.join(',');
                os << "\n";
                os.flush();
            }
            else if (tokens[1] == "PORT")
            {
                QStringList param = tokens[2].split(',', QString::SkipEmptyParts, Qt::CaseSensitive);

                for (int i = 0; i < SERVER_MAX_SALOONS; i++)
                {
                    if (saloons[i].name == param[0])
                    {
                        for (int j = 0; j < SERVER_MAX_TABLES; j++)
                        {
                            if (saloons[i].tables[j].name == param[1])
                            {
                                os << "PORT:";
                                os << QString().setNum(saloons[i].tables[j].table.GetServer().GetTcpPort());
                                os << "\n";
                                os.flush();
                            }
                        }
                    }
                }
            }
        }
    }
}
/*****************************************************************************/
void Lobby::NewConnection(int socket)
{

}

//=============================================================================
// End of file Lobby.cpp
//=============================================================================
