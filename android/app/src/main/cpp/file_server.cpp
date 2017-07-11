

#include <cstdint>
#include <string>
#include <regex>
#include <sstream>

#include "TcpSocket.h"
#include "TcpServer.h"



extern "C" {
const char *find_embedded_file(const char *name, size_t *size, const char **mime);
}


class BasicFileHandler : public tcp::TcpServer::IEvent
{
public:
    BasicFileHandler()
    {

    }

    void NewConnection(const tcp::Conn &conn) {
        (void) conn;
    }

    void ReadData(const tcp::Conn &conn)
    {
        std::string resource = Match(conn.payload, "GET (.*) HTTP");

        std::cout << "Get file: " << resource << std::endl;

        if (resource == "/")
        {
            resource = "/index.html";
        }

        const char *data;
        const char *mime;
        size_t size;

        data = find_embedded_file(resource.c_str(), &size, &mime);

        std::string output;
        if (data != NULL)
        {
            std::cout << "File found, size: " << size << " MIME: " << mime << std::endl;

            std::stringstream ss;

            ss << "HTTP/1.1 200 OK\r\n";
            ss << "Content-type: " << std::string(mime) << "\r\n";
            ss << "Content-length: " << (int)size << "\r\n\r\n";
            ss << std::string(data, size) << std::flush;

            output = ss.str();
        }
        else
        {
            std::stringstream ss;
            std::string html = "<html><head><title>Not Found</title></head><body>404</body><html>";

            ss << "HTTP/1.1 404 Not Found\r\n";
            ss << "Content-type: text/html\r\n";
            ss << "Content-length: " << html.size() << "\r\n\r\n";
            ss << html << std::flush;

            output = ss.str();
        }
        tcp::TcpSocket::SendToSocket(output, conn.peer);
    }

    void ClientClosed(const tcp::Conn &conn)
    {
        (void) conn;
    }

    void ServerTerminated(tcp::TcpServer::IEvent::CloseType type)
    {
        (void) type;
    }


    std::string Match(const std::string &msg, const std::string &patternString)
    {
        std::regex pattern(patternString);
        std::smatch matcher;
        std::string subMatch;

        std::regex_search(msg, matcher, pattern);

        if (matcher.size() == 2)
        {
            // Extracted value is located at index 1
            subMatch = matcher[1].str();
        }
        return subMatch; // empty string if not found
    }

};

BasicFileHandler fileHandler;
tcp::TcpServer server(fileHandler);

extern "C" void StartFileServer()
{
    server.Start(10, true, 8000);
}

extern "C" void StopFileServer()
{
    server.Stop();
}


