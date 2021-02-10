
#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include <iostream>

#include "tst_tarot_3_and_5_players.h"
#include "Card.h"
#include "Deck.h"
#include "ServerConfig.h"
#include "Deal.h"
#include "DealFile.h"
#include "Lobby.h"
#include "BasicClient.h"

Tarot3And5Players::Tarot3And5Players()
{
}

void TestDeal(std::uint32_t nbPlayers)
{
    DealFile editor;

    bool valid = editor.CreateRandomDeal(nbPlayers);
    QCOMPARE(valid, true);

    for (std::uint32_t i = 0U; i < nbPlayers; i++)
    {
        Place p(i);
        Deck d = editor.GetPlayerDeck(p);
        QCOMPARE(d.Size(), Tarot::NumberOfCardsInHand(nbPlayers));
    }
    Deck dog = editor.GetDogDeck();
    QCOMPARE(dog.Size(), Tarot::NumberOfDogCards(nbPlayers));
}

struct TestOpponents
{
    std::vector<Reply> reply;
    BasicClient bot;
    std::uint32_t uuid;
};

static TestOpponents bots[5];
static Identity names[5] =
{
    {"riri", "", Identity::cGenderRobot},
    {"fifi", "", Identity::cGenderRobot},
    {"loulou", "", Identity::cGenderRobot},
    {"picsou", "", Identity::cGenderRobot},
    {"donald", "", Identity::cGenderRobot},
};


void Tarot3And5Players::TestFullGame(uint32_t nbPlayers)
{
    PlayingTable table; // on crée une table juste pour notre test
    Tarot::Game game; // par défaut, quick deal

    table.SetId(7);
    table.SetName(std::to_string(nbPlayers) + " joueurs");
    table.SetAdminMode(false); // démarrage automatique des parties
    table.SetupGame(game);
    table.Initialize();
    table.CreateTable(nbPlayers);

    std::uint32_t tableId = Protocol::TABLES_UID;
    std::vector<Reply> exch_data; // données transférées entre la table et les clients

    // Initialize & add the bots to the table
    for (uint32_t i = 0; i < nbPlayers; i++)
    {
        bots[i].uuid = Protocol::USERS_UID + i;
        bots[i].bot.mMyself.identity = names[i];
        uint8_t currentNbPlayers;
        Place assignedPlace = table.AddPlayer(bots[i].uuid, currentNbPlayers);

        JsonObject reply;

        reply.AddValue("cmd", "ReplyJoinTable");
        reply.AddValue("table_id", tableId);
        reply.AddValue("place", assignedPlace.ToString());
        reply.AddValue("size", nbPlayers);
        exch_data.push_back(Reply(bots[i].uuid, reply));
    }

    bool EndOfGame = false;

    // Ici pas de notion de réseau donc on transfert les données à la main !!
    do
    {
        //  Première séquence de notre réseau fictif : on fait jouer tous les joueurs
        // On passe en paramètre les données envoyées par la table de jeu (s'il y en a)
        for (uint32_t i = 0; i < nbPlayers; i++)
        {
            bots[i].reply.clear();

    //        std::cout << "------------------------------------" << std::endl;
            std::cout << "Bot " << bots[i].bot.mMyself.place.ToString() << " cards: " << bots[i].bot.mDeck.ToString() << std::endl;

            // Send data to that bot if any
            for (std::uint32_t j = 0U; j < exch_data.size(); j++)
            {
                for (std::uint32_t k = 0U; k < exch_data[j].dest.size(); k++)
                {
                    if ((exch_data[j].dest[k] == bots[i].uuid) ||
                        (exch_data[j].dest[k] == Protocol::LOBBY_UID) ||
                        (exch_data[j].dest[k] == tableId))
                    {
                        bots[i].bot.PlayRandom(Protocol::LOBBY_UID, exch_data[j].dest[k], exch_data[j].data.ToString(0U), bots[i].reply);
                    }
                }
            }
        }

        // On efface les messages créés précédemment par la table de jeu
        exch_data.clear();

        // Deuxième séquence de notre réseau fictif : on balaye les données envoyées par les joueurs
        // et on les envoie à la table
        for (uint32_t i = 0; i < nbPlayers; i++)
        {
            // Send bot replies to the lobby (destination should be always the lobby or a table)
            for (std::uint32_t j = 0U; j < bots[i].reply.size(); j++)
            {
                // No allowed broadcast for messages from clients
                std::uint32_t expected = 1UL;
                std::uint32_t value = static_cast<std::uint32_t>(bots[i].reply[j].dest.size());
                QCOMPARE(value, expected);

                EndOfGame = table.ExecuteRequest(bots[i].uuid, bots[i].reply[j].dest[0], bots[i].reply[j].data, exch_data);
            }
        }
    }
    while (!EndOfGame);

    std::cout << "========> End of deal!! <========" << std::endl;
}


void Tarot3And5Players::TestDeal3Players()
{
    TestDeal(3);
}

void Tarot3And5Players::TestDeal5Players()
{
    TestDeal(5);
}

void Tarot3And5Players::TestFullGame3Players()
{
    TestFullGame(3);
}


