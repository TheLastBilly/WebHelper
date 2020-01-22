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
        NO_RESPONSE,
        NOT_CONNECTED,
        NOT_INIT,
        A_OK
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
    std::string url;
    StatusType status = A_OK;
    bool 
        is_init = false,
        is_connected = false,
        has_response = false;

    WebHelper( std::string domain );
    WebHelper( );
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
    static WebHelper::HeaderList ParseHeaders( std::string raw );
    
private:
    //Networking
    struct addrinfo honstname_addr = {};
    static const int 
        HTTP_PORT = 80,
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

    //Create a socket based on a hostname
    StatusType OpenSocketToHostname();

    int 
        tcp_bytes_moved = 0, 
        tbm = 0,
        request_size = 0;
};

#endif