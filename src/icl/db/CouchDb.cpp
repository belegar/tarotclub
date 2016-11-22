
#include "CouchDb.h"
#include "Util.h"

/*****************************************************************************/
CouchDb::CouchDb()
    : mConn( "localhost", 5984, *this )
{

}
/*****************************************************************************/
bool CouchDb::Connect()
{
    return mConn.connect();
}
/*****************************************************************************/
void CouchDb::Close()
{
    mConn.close();
}
/*****************************************************************************/
bool CouchDb::IsSuccess()
{
    bool ret = false;

    if (IsValid())
    {
        if (!GetValue("ok", ret))
        {
            if (!GetValue("error", mLastError))
            {
                mLastError = "No any valid information received";
            }
            ret = false;
        }
        else
        {
            mLastError.clear();
        }
    }
    else
    {
        mLastError = "No any valid JSON reply received";
    }

    return ret;
}
/*****************************************************************************/
void CouchDb::Get(const std::string &url)
{
    std::map<std::string, std::string> header;
    mConn.request( "GET", url.c_str(), header );
}
/*****************************************************************************/
void CouchDb::Put(const std::string &url)
{
    Put(url,std::string());
}
/*****************************************************************************/
void CouchDb::Put(const std::string &url, const std::string &body)
{
    std::map<std::string, std::string> header;

    if (body.size() == 0)
    {
        mConn.request( "PUT", url.c_str(), header, NULL, 0 );
    }
    else
    {
        mConn.request( "PUT", url.c_str(), header, body.c_str(), body.size() );
    }
}
/*****************************************************************************/
void CouchDb::Post(const std::string &url, const std::string &body)
{
    std::map<std::string, std::string> header;
    header["Content-Type"] = "application/json";
    mConn.request( "POST", url.c_str(), header, body.c_str(), body.size() );
}
/*****************************************************************************/
void CouchDb::Delete(const std::string &url)
{
    std::map<std::string, std::string> header;
    mConn.request( "DELETE", url.c_str(), header );
}
/*****************************************************************************/
// invoked when response headers have been received
void CouchDb::ResponseBegin( const http::Response* r)
{
    //printf( "BEGIN (%d %s)\n", r->getstatus(), r->getreason() );
    (void) r;
    mData.clear();
}
/*****************************************************************************/
// invoked to process response body data (may be called multiple times)
void CouchDb::ResponseData( const http::Response* r, const char* data, int n )
{
    (void) r;
    //fwrite( data,1,n, stdout );
    mData.append(data, n);
}
/*****************************************************************************/
// invoked when response is complete
void CouchDb::ResponseComplete( const http::Response* r )
{
    (void) r;
    //printf( "COMPLETE (%d bytes)\n", count );
    JsonReader::ParseString(*this, mData);
}

//=============================================================================
// End of file CouchDb.cpp
//=============================================================================
