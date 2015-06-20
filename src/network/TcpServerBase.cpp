#include "TcpServerBase.h"

TcpServerBase::TcpServerBase()
{

}
/*****************************************************************************/
bool TcpServerBase::CreateServer(std::uint16_t port, bool localHostOnly, std::int32_t maxConnections)
{
    /*************************************************************/
    /* Create an AF_INET stream socket to receive incoming       */
    /* connections on                                            */
    /*************************************************************/
    if (!Create())
    {
        return false;
    }

    /*************************************************************/
    /* Set socket to be non-blocking.  All of the sockets for    */
    /* the incoming connections will also be non-blocking since  */
    /* they will inherit that state from the listening socket.   */
    /*************************************************************/
    if (!SetBlocking(false))
    {
        Close();
        return false;
    }

    /*************************************************************/
    /* Bind the socket                                           */
    /*************************************************************/
    if (!Bind(port, localHostOnly))
    {
        Close();
        return false;
    }

    /*************************************************************/
    /* Set the listen back log                                   */
    /*************************************************************/
    if (!Listen(maxConnections))
    {
        Close();
        return false;
    }

    return true;
}


