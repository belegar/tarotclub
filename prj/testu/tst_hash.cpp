
#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include <iostream>

#include "tst_hash.h"
#include "Base64.h"

LibraryHash::LibraryHash()
{

}

void LibraryHash::TestBase64()
{
    // test encoder
    std::string plainText = "Je n’ai pas peur des ordinateurs. J’ai peur qu’ils viennent à nous manquer.";
    std::string encodedText = "SmUgbuKAmWFpIHBhcyBwZXVyIGRlcyBvcmRpbmF0ZXVycy4gSuKAmWFpIHBldXIgcXXigJlpbHMgdmllbm5lbnQgw6Agbm91cyBtYW5xdWVyLg==";

    std::string result = Base64::Encode(plainText);
    std::cout << "Encoded text: " << result << std::endl;
    QCOMPARE(result, encodedText);



    // test decoder
    plainText = "Y’a pas de nous, y’a pas de notre, y’a juste moi et mon marais !!";
    encodedText = "WeKAmWEgcGFzIGRlIG5vdXMsIHnigJlhIHBhcyBkZSBub3RyZSwgeeKAmWEganVzdGUgbW9pIGV0IG1vbiBtYXJhaXMgISE=";


    result = Base64::Decode(encodedText);
    std::cout << "Decoded text: " << result << std::endl;
    QCOMPARE(result, plainText);

}

// End of file
