/*
 * HappyHTTP - a simple HTTP library
 * Version 0.1
 * 
 * Copyright (c) 2006 Ben Campbell
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software in a
 * product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 * be misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */


#include "Http.h"
#include "ByteArray.h"
#include "Log.h"

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <assert.h>

#include <string>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>


//---------------------------------------------------------------------
//
// Connection
//
//---------------------------------------------------------------------

namespace http
{

Connection::Connection(const char* host, int port , IEvent &eventHandler)
    : mEventHandler(eventHandler)
    , m_State( IDLE)
    , mHost(host)
    , mPort(port)
    , mConnected(false)
{
    mClient.Initialize();
}


bool Connection::connect()
{
    return mClient.Connect(mHost, mPort);
}


void Connection::close()
{
    mClient.Close();

	// discard any incomplete responses
	while( !m_Outstanding.empty() )
	{
		delete m_Outstanding.front();
		m_Outstanding.pop_front();
	}
}


Connection::~Connection()
{
	close();
}

void Connection::request( const char* method,
	const char* url,
    const std::map<std::string, std::string> &headers,
    const char* body,
	int bodysize )
{
	bool gotcontentlength = false;	// already in headers?

	// check headers for content-length
	// TODO: check for "Host" and "Accept-Encoding" too
	// and avoid adding them ourselves in putrequest()
    if(headers.size() > 0)
	{
        for (std::map<std::string, std::string>::const_iterator iter = headers.begin();
             iter != headers.end();
             ++iter)
        {
            if (iter->second.size() > 0)
            {
                std::string ContentLength = iter->first;
                std::transform(ContentLength.begin(), ContentLength.end(), ContentLength.begin(),::tolower);

                if( ContentLength  == "content-length")
                {
                    gotcontentlength = true;
                }
            }
            else
            {
                TLogError("HTTP header option value cannot be null!");
            }
		}
	}

	putrequest( method, url );

	if( body && !gotcontentlength )
		putheader( "Content-Length", bodysize );

    if(headers.size() > 0)
	{
        for (std::map<std::string, std::string>::const_iterator iter = headers.begin();
             iter != headers.end();
             ++iter)
        {
            putheader(iter->first.c_str(), iter->second.c_str());
        }
	}
	endheaders();

	if( body )
    {
		send( body, bodysize );
    }
    // Wait to receive the reply
    while( outstanding() )
    {
        if (pump())
        {
            break; // timeout
        }
    }
}


void Connection::putrequest( const char* method, const char* url )
{
	if( m_State != IDLE )
    {
        std::cout << "Request already issued" << std::endl;
        return;
    }

	m_State = REQ_STARTED;

	char req[ 512 ];
	std::sprintf( req, "%s %s HTTP/1.1", method, url );
	m_Buffer.push_back( req );

    putheader( "Host", mHost.c_str() );	// required for HTTP1.1

	// don't want any fancy encodings please
	putheader("Accept-Encoding", "identity");

	// Push a new response onto the queue
    Response *r = new Response( method, mEventHandler );
	m_Outstanding.push_back( r );
}


void Connection::putheader( const char* header, const char* value )
{
	if( m_State != REQ_STARTED )
    {
        std::cout << "putheader() failed" << std::endl;
        return;
    }
    m_Buffer.push_back( std::string(header) + ": " + std::string( value ) );
}

void Connection::putheader( const char* header, int numericvalue )
{
	char buf[32];
	sprintf( buf, "%d", numericvalue );
	putheader( header, buf );
}

void Connection::endheaders()
{
	if( m_State != REQ_STARTED )
    {
        std::cout << "Cannot send header" << std::endl;
        return;
    }
	m_State = IDLE;

	m_Buffer.push_back( "" );

    std::string msg;
    std::vector< std::string>::const_iterator it;
	for( it = m_Buffer.begin(); it != m_Buffer.end(); ++it )
    {
		msg += (*it) + "\r\n";
    }

	m_Buffer.clear();

//	printf( "%s", msg.c_str() );
    send( msg.c_str(), msg.size() );
}



void Connection::send( const char* buf, int numbytes )
{
    if (!mClient.IsValid())
    {
		connect();
    }
    std::string bytes;
    bytes.append(buf, static_cast<size_t>(numbytes));
    (void) mClient.Send(bytes);
}


bool Connection::pump()
{
    if (m_Outstanding.empty())
        return false;		// no requests outstanding

    if (!mClient.DataWaiting(2U))
        return true;				// recv will block

    std::string buf;
    if (!mClient.Recv(buf))
	{
		// connection has closed
		Response* r = m_Outstanding.front();
		r->notifyconnectionclosed();
		assert( r->completed() );
		delete r;
		m_Outstanding.pop_front();

		// any outstanding requests will be discarded
		close();
	}
	else
	{
		int used = 0;
        int size = buf.size();
        while( used < size && !m_Outstanding.empty() )
		{
			Response* r = m_Outstanding.front();
            int u = r->pump( (char *)buf.c_str() + used, size - used );

			// delete response once completed
			if( r->completed() )
			{
				delete r;
				m_Outstanding.pop_front();
			}
			used += u;
		}

		// NOTE: will lose bytes if response queue goes empty
		// (but server shouldn't be sending anything if we don't have
		// anything outstanding anyway)
        assert( used == size );	// all bytes should be used up by here.
	}

    return false;
}






//---------------------------------------------------------------------
//
// Response
//
//---------------------------------------------------------------------


Response::Response(const char* method, IEvent &eventHandler )
    : mState( STATUSLINE )
    , mEventHandler(eventHandler)
    , m_Method( method )
    , m_Version( 0 )
    , m_Status(0)
    , m_BytesRead(0)
    , m_Chunked(false)
    , m_ChunkLeft(0)
    , m_Length(-1)
    , m_WillClose(false)
{
}


const char* Response::getheader( const char* name ) const
{
	std::string lname( name );
	std::transform( lname.begin(), lname.end(), lname.begin(), tolower );

	std::map< std::string, std::string >::const_iterator it = m_Headers.find( lname );
	if( it == m_Headers.end() )
		return 0;
	else
		return it->second.c_str();
}


int Response::getstatus() const
{
	// only valid once we've got the statusline
    assert( mState != STATUSLINE );
	return m_Status;
}


const char* Response::getreason() const
{
	// only valid once we've got the statusline
    assert( mState != STATUSLINE );
	return m_Reason.c_str();
}



// Connection has closed
void Response::notifyconnectionclosed()
{
    if( mState == COMPLETE )
    {
		return;
    }

	// eof can be valid...
    if( mState == BODY &&
		!m_Chunked &&
		m_Length == -1 )
	{
		Finish();	// we're all done!
	}
	else
	{
        std::cout << "Connection closed unexpectedly" << std::endl;
	}
}


int Response::pump( const char* data, int datasize )
{
	assert( datasize != 0 );
	int count = datasize;

    while( count > 0 && mState != COMPLETE )
	{
        if( mState == STATUSLINE ||
            mState == HEADERS ||
            mState == TRAILERS ||
            mState == CHUNKLEN ||
            mState == CHUNKEND )
		{
			// we want to accumulate a line
			while( count > 0 )
			{
				char c = (char)*data++;
				--count;
				if( c == '\n' )
				{
					// now got a whole line!
                    switch( mState )
					{
						case STATUSLINE:
							ProcessStatusLine( m_LineBuf );
							break;
						case HEADERS:
							ProcessHeaderLine( m_LineBuf );
							break;
						case TRAILERS:
							ProcessTrailerLine( m_LineBuf );
							break;
						case CHUNKLEN:
							ProcessChunkLenLine( m_LineBuf );
							break;
						case CHUNKEND:
							// just soak up the crlf after body and go to next state
							assert( m_Chunked == true );
                            mState = CHUNKLEN;
							break;
						default:
							break;
					}
					m_LineBuf.clear();
					break;		// break out of line accumulation!
				}
				else
				{
					if( c != '\r' )		// just ignore CR
						m_LineBuf += c;
				}
			}
		}
        else if( mState == BODY )
		{
			int bytesused = 0;
			if( m_Chunked )
            {
				bytesused = ProcessDataChunked( data, count );
            }
			else
            {
				bytesused = ProcessDataNonChunked( data, count );
            }
			data += bytesused;
			count -= bytesused;
		}
	}

	// return number of bytes used
	return datasize - count;
}



void Response::ProcessChunkLenLine( std::string const& line )
{
	// chunklen in hex at beginning of line
	m_ChunkLeft = strtol( line.c_str(), NULL, 16 );
	
	if( m_ChunkLeft == 0 )
	{
		// got the whole body, now check for trailing headers
        mState = TRAILERS;
		m_HeaderAccum.clear();
	}
	else
	{
        mState = BODY;
	}
}


// handle some body data in chunked mode
// returns number of bytes used.
int Response::ProcessDataChunked( const char* data, int count )
{
	assert( m_Chunked );

	int n = count;
	if( n>m_ChunkLeft )
		n = m_ChunkLeft;

	// invoke callback to pass out the data
    mEventHandler.ResponseData(this, data, n );

	m_BytesRead += n;

	m_ChunkLeft -= n;
	assert( m_ChunkLeft >= 0);
	if( m_ChunkLeft == 0 )
	{
		// chunk completed! now soak up the trailing CRLF before next chunk
        mState = CHUNKEND;
	}
	return n;
}

// handle some body data in non-chunked mode.
// returns number of bytes used.
int Response::ProcessDataNonChunked(const char *data, int count )
{
	int n = count;
	if( m_Length != -1 )
	{
		// we know how many bytes to expect
		int remaining = m_Length - m_BytesRead;
		if( n > remaining )
			n = remaining;
	}

	// invoke callback to pass out the data
    mEventHandler.ResponseData(this, data, n );

	m_BytesRead += n;

	// Finish if we know we're done. Else we're waiting for connection close.
	if( m_Length != -1 && m_BytesRead == m_Length )
		Finish();

	return n;
}


void Response::Finish()
{
    mState = COMPLETE;

	// invoke the callback
    mEventHandler.ResponseComplete(this);
}


void Response::ProcessStatusLine( std::string const& line )
{
	const char* p = line.c_str();

	// skip any leading space
	while( *p && *p == ' ' )
		++p;

	// get version
	while( *p && *p != ' ' )
		m_VersionString += *p++;
	while( *p && *p == ' ' )
		++p;

	// get status code
	std::string status;
	while( *p && *p != ' ' )
		status += *p++;
	while( *p && *p == ' ' )
		++p;

	// rest of line is reason
	while( *p )
		m_Reason += *p++;

	m_Status = atoi( status.c_str() );
	if( m_Status < 100 || m_Status > 999 )
    {
        std::cout << "BadStatusLine: " << line.c_str() << std::endl;
    }

/*
	printf( "version: '%s'\n", m_VersionString.c_str() );
	printf( "status: '%d'\n", m_Status );
	printf( "reason: '%s'\n", m_Reason.c_str() );
*/

	if( m_VersionString == "HTTP:/1.0" )
		m_Version = 10;
	else if( 0==m_VersionString.compare( 0,7,"HTTP/1." ) )
		m_Version = 11;
	else
    {
        std::cout << "UnknownProtocol" << m_VersionString << std::endl ;
    }

	// OK, now we expect headers!
    mState = HEADERS;
	m_HeaderAccum.clear();
}


// process accumulated header data
void Response::FlushHeader()
{
	if( m_HeaderAccum.empty() )
		return;	// no flushing required

	const char* p = m_HeaderAccum.c_str();

	std::string header;
	std::string value;
	while( *p && *p != ':' )
		header += tolower( *p++ );

	// skip ':'
	if( *p )
		++p;

	// skip space
	while( *p && (*p ==' ' || *p=='\t') )
		++p;

	value = p; // rest of line is value

	m_Headers[ header ] = value;
//	printf("header: ['%s': '%s']\n", header.c_str(), value.c_str() );	

	m_HeaderAccum.clear();
}


void Response::ProcessHeaderLine( std::string const& line )
{
	const char* p = line.c_str();
	if( line.empty() )
	{
		FlushHeader();
		// end of headers

		// HTTP code 100 handling (we ignore 'em)
		if( m_Status == CONTINUE )
            mState = STATUSLINE;	// reset parsing, expect new status line
		else
			BeginBody();			// start on body now!
		return;
	}

	if( isspace(*p) )
	{
		// it's a continuation line - just add it to previous data
		++p;
		while( *p && isspace( *p ) )
			++p;

		m_HeaderAccum += ' ';
		m_HeaderAccum += p;
	}
	else
	{
		// begin a new header
		FlushHeader();
		m_HeaderAccum = p;
	}
}


void Response::ProcessTrailerLine( std::string const& line )
{
	// TODO: handle trailers?
	// (python httplib doesn't seem to!)
	if( line.empty() )
		Finish();

	// just ignore all the trailers...
}



// OK, we've now got all the headers read in, so we're ready to start
// on the body. But we need to see what info we can glean from the headers
// first...
void Response::BeginBody()
{

	m_Chunked = false;
	m_Length = -1;	// unknown
	m_WillClose = false;

	// using chunked encoding?
	const char* trenc = getheader( "transfer-encoding" );
	if( trenc && 0==std::strcmp( trenc, "chunked") )
	{
		m_Chunked = true;
		m_ChunkLeft = -1;	// unknown
	}

	m_WillClose = CheckClose();

	// length supplied?
	const char* contentlen = getheader( "content-length" );
	if( contentlen && !m_Chunked )
	{
		m_Length = atoi( contentlen );
	}

	// check for various cases where we expect zero-length body
	if( m_Status == NO_CONTENT ||
		m_Status == NOT_MODIFIED ||
		( m_Status >= 100 && m_Status < 200 ) ||		// 1xx codes have no body
		m_Method == "HEAD" )
	{
		m_Length = 0;
	}


	// if we're not using chunked mode, and no length has been specified,
	// assume connection will close at end.
	if( !m_WillClose && !m_Chunked && m_Length == -1 )
		m_WillClose = true;



	// Invoke the user callback, if any
    mEventHandler.ResponseBegin(this);

/*
	printf("---------BeginBody()--------\n");
	printf("Length: %d\n", m_Length );
	printf("WillClose: %d\n", (int)m_WillClose );
	printf("Chunked: %d\n", (int)m_Chunked );
	printf("ChunkLeft: %d\n", (int)m_ChunkLeft );
	printf("----------------------------\n");
*/
	// now start reading body data!
	if( m_Chunked )
        mState = CHUNKLEN;
	else
        mState = BODY;
}


// return true if we think server will automatically close connection
bool Response::CheckClose()
{
	if( m_Version == 11 )
	{
		// HTTP1.1
		// the connection stays open unless "connection: close" is specified.
		const char* conn = getheader( "connection" );
		if (conn && 0 == strcmp(conn, "close"))
			return true;
		else
			return false;
	}

	// Older HTTP
	// keep-alive header indicates persistant connection 
	if( getheader( "keep-alive" ) )
		return false;

	// TODO: some special case handling for Akamai and netscape maybe?
	// (see _check_close() in python httplib.py for details)

	return true;
}



}	// end namespace http


