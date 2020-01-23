// OpenRooms - webhelper.hpp
//
// Copyright (c) 2020, TheLastBilly
// All rights reserved.
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#ifndef WEBHELPER_HPP_
#define WEBHELPER_HPP_

#include <iostream>
#include <sstream>
#include <vector>

#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include <errno.h>
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

class WebHelper
{
public:
    enum StatusType
    {
        SOCKET_ERROR,
        NO_HOSTNAME_ERROR,
        HOSTNAME_NOT_FOUND_ERROR,
        CONNECTION_ERROR,
        NETWORK_SEND_ERROR,
        NETWORK_RECEIVED_ERROR,
        RESPONSE_BUFFER_FULL_ERROR,
        SSL_CERTIFICATE_ERROR,
        SSL_ACCEPT_ERROR,
        NO_RESPONSE,
        NOT_CONNECTED,
        NOT_INIT,
        A_OK
    };

    enum ConnectionType
    {
        HTTP,
        HTTPS
    };

    enum RequestType
    {
        GET,
        POST
    };
    
    struct Header
    {
        std::string
            name, content;
    };
    typedef std::vector<WebHelper::Header> HeaderList;

    WebHelper( std::string domain, ConnectionType type );
    WebHelper( ConnectionType type );
    ~WebHelper();

    //Initialize WebHelper object
    StatusType Init();

    //End WebHelper object
    StatusType End();

    //Change current url
    StatusType ChangeUrl( std::string domain );

    //Connect to server
    StatusType ConnectSocket();
    
    //Disconnect from server
    StatusType DisconnectSocket();

    //Get current value of the status variable
    StatusType GetStatus();

    //Get description of status variable
    std::string GetStatusDescription(  );
    static std::string GetStatusDescription( StatusType st, WebHelper * web);

    //Compose HTTP request    
    std::string ComposeRequest( std::string request );
    std::string ComposeRequest( RequestType type, std::string page, HeaderList header_list, std::string body);

    //Send request to server
    StatusType SendRequest();   
    StatusType SendRequest( std::string request, int request_len );    

    //Wait for and receive response
    StatusType ReceiveResponse();

    //Send request, wait for and receive response
    StatusType ExchangeRequest( std::string request );
    StatusType ExchangeRequest( );

    //Request a page from domain
    StatusType RequestPage( std::string request );

    //Return recived response    
    std::string GetResponse();
    
    //Retreive headers from response
    static WebHelper::HeaderList ParseHeaders( std::istream &raw );

    //Socket Handlers for HTTP/HTTPS
    class SocketHandler
    {
    public:
        SocketHandler();
        virtual ~SocketHandler();
        virtual StatusType Init( int fd );
        StatusType End();
        virtual size_t WriteToSocket( int fd, const void * buf, size_t n );
        virtual size_t ReadFromSocket(int fd, void * buf, size_t n, int flags );
        int sckt;
        StatusType status = A_OK;
        bool is_init = false;
    };

    class SslSocketHandler : public SocketHandler
    {
    public:
        SslSocketHandler( );
        ~SslSocketHandler( );
        StatusType Init( int fd ) override;
        StatusType End();
        size_t WriteToSocket( int fd, const void * buf, size_t n ) override;
        size_t ReadFromSocket(int fd, void * buf, size_t n, int flags ) override;
        static void InitSsl();
        static void EndSsl();
        int sckt;
        SSL_CTX * ssl_ctx = nullptr;
        SSL * c_ssl = nullptr;

    };
    
private:

    //Networking
    struct addrinfo honstname_addr = {};
    static const int 
        HTTP_PORT = 80,
        HTTPS_PORT = 443,
        IP_MAX_BUFFER = 18,
        MAX_RESPONSE_BUFFER = 8192000,
        MAX_REQUEST_BUFFER = 1024;
    static const char *
        HttpRequestTemplate;
    int sckt = 0;
    char 
        hostname_ip[IP_MAX_BUFFER] = {0},
        response_buffer[MAX_RESPONSE_BUFFER] = {0},
        request_buffer[MAX_REQUEST_BUFFER] = {0};
    int 
        tcp_bytes_moved = 0, 
        tbm = 0,
        request_size = 0,
        current_port = HTTP_PORT;

    //Internals
    std::string url;
    StatusType status = A_OK;
    ConnectionType connection_type = HTTP;
    bool 
        is_init = false,
        is_connected = false,
        has_response = false;
    SocketHandler * socket_handler = nullptr;

    //Create a socket based on a hostname
    StatusType OpenSocketToHostname();
    
    void SetConnectionType( ConnectionType type );
};

#endif