/*
    sha1.h - header of

    ============
    SHA-1 in C++
    ============

    100% Public Domain.

    Original C Code
        -- Steve Reid <steve@edmweb.com>
    Small changes to fit into bglibs
        -- Bruce Guenter <bruce@untroubled.org>
    Translation to simpler C++ Code
        -- Volker Grabsch <vog@notjusthosting.com>
*/

#ifndef SHA1_HPP
#define SHA1_HPP


#include <iostream>
#include <string>

/*****************************************************************************/
class Sha1
{
public:
    Sha1();
    void Update(const std::string &s);
    void Update(std::istream &is);
    std::string Final();
    static std::string FromFile(const std::string &filename);

private:
    static const unsigned int DIGEST_INTS = 5;  /* number of 32bit integers per SHA1 digest */
    static const unsigned int BLOCK_INTS = 16;  /* number of 32bit integers per SHA1 block */
    static const unsigned int BLOCK_BYTES = BLOCK_INTS * 4;

    std::uint32_t digest[DIGEST_INTS];
    std::string buffer;
    std::uint64_t transforms;

    void Reset();
    void Transform(std::uint32_t block[BLOCK_BYTES]);

    static void BufferToBlock(const std::string &buffer, std::uint32_t block[BLOCK_BYTES]);
    static void Read(std::istream &is, std::string &s, int max);
};


#endif /* SHA1_HPP */
