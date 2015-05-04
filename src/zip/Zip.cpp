
#define ZIP_CPP_INCLUDED
#include "miniz.c"
#include "Zip.h"
#include <fstream>
#include <cstring>

Zip::Zip()
    : mIsValid(false)
    , mNumberOfFiles(0U)
{

}

Zip::~Zip()
{
    Close();
}


bool Zip::Open(const std::string &fileName)
{
    mz_bool status;
    mIsValid = false;

    mNumberOfFiles = 0U;

    std::memset(&zip_archive, 0, sizeof(zip_archive));
    status = mz_zip_reader_init_file(&zip_archive, fileName.c_str(), 0);
    if (status)
    {
        // Get and print information about each file in the archive.
        for (std::uint32_t i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++)
        {
            mz_zip_archive_file_stat file_stat;
            if (mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
            {
                mNumberOfFiles++;
                //printf("Filename: \"%s\", Comment: \"%s\", Uncompressed size: %u, Compressed size: %u\n", file_stat.m_filename, file_stat.m_comment, (std::uint32_t)file_stat.m_uncomp_size, (std::uint32_t)file_stat.m_comp_size);
            }
        }
    }

    if (mNumberOfFiles == mz_zip_reader_get_num_files(&zip_archive))
    {
        mIsValid = true;
    }

    return mIsValid;
}

void Zip::Close()
{
    if (mIsValid)
    {
        mz_zip_reader_end(&zip_archive);
    }
}

bool Zip::GetFile(const std::string &fileName, std::string &contents)
{
    bool ret = false;
    if (mIsValid)
    {
        size_t size;
        char *p = (char *)mz_zip_reader_extract_file_to_heap(&zip_archive, fileName.c_str(), &size, 0);
        contents.assign(p, size);
        free(p);
    }
    return ret;
}

