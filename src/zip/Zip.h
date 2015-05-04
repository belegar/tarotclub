#ifndef ZIP_H
#define ZIP_H

#include <cstdint>
#include <string>
#include <vector>

#ifndef ZIP_CPP_INCLUDED
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"
#endif

class Zip
{
public:
    Zip();
    ~Zip();

    bool Open(const std::string &fileName);
    std::uint32_t NumberOfFiles() { return mNumberOfFiles; }
    bool GetFile(const std::string &fileName, std::string &contents);
    void Close();

private:
    std::int32_t cZipMaxSize = 2 * 1024 * 1024; // 2MB max

    mz_zip_archive zip_archive;
    bool mIsValid;
    std::uint32_t mNumberOfFiles;
};

#endif // ZIP_H
