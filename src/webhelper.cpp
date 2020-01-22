#include "webhelper.hpp"

const char *WebHelper::HttpRequestTemplate = 
    "%s /%s HTTP/1.0\r\nHost: %s\r\n%s\r\n%s";

WebHelper::WebHelper( std::string host ):
url( host )
{
    Init();
}

WebHelper::WebHelper( )
{}

WebHelper::~WebHelper( )
{
    End();
}

WebHelper::StatusType WebHelper::Init()
{
    if(is_init)
    {
        End();
    }

    if(url.empty())
        return ( status = NO_HOSTNAME_ERROR );
    
    is_init = true;
    return A_OK;
}

WebHelper::StatusType WebHelper::ExchangeRequest( std::string request)
{
    if(!is_init)
    {
        return (status = NOT_INIT);
    }
    if(!is_connected)
    {
        return (status = NOT_CONNECTED);
    }

    if(SendRequest(request, request.length()) != A_OK)
    {
        return status;
    }

    if(ReceiveResponse() != A_OK)
    {
        return status;
    }

    return (status = A_OK);
}

WebHelper::StatusType WebHelper::ExchangeRequest( )
{
    return ExchangeRequest( std::string(request_buffer) );
}

WebHelper::StatusType WebHelper::RequestPage( std::string page )
{
    if(ConnectSocket() != A_OK)
        return status;
    ComposeRequest( page );
    if(ExchangeRequest() != A_OK)
        return status;
    
    DisconnectSocket();

    return (status = A_OK);
}

std::string WebHelper::GetResponse()
{
    if(has_response)
        return std::string( response_buffer );
    
    status = NO_RESPONSE;
    return std::string("");
}

std::string WebHelper::ComposeRequest( RequestType type, std::string page, HeaderList header_list, std::string body)
{
    memset( request_buffer, 0, MAX_REQUEST_BUFFER );
    
    //Get the type as a string
    char s_type[6] = {0};
    switch (type)
    {
    case GET:
        snprintf(s_type, 5, "GET");
        break;
    case POST:
        snprintf(s_type, 5, "POST");
        break;
    }

    //Get the headers as a string
    std::string header_buffer = "";
    for(Header header : header_list)
    {
        header_buffer += 
            header.name + ": " + header.content + "\r\n";
    }

    if(body.length() > 0)
    {

        char body_length_str[21] = {0};
        snprintf(body_length_str, 20, "%ld", body.length());
        header_buffer +=
            std::string("Content-Type:  text/html\r\n") +
            std::string("Content-Length:  ") + std::string(body_length_str);
    }

    (request_size = 
        snprintf(
            request_buffer,
            MAX_REQUEST_BUFFER,
            HttpRequestTemplate,
            s_type,
            page.c_str(),
            url.c_str(),
            header_buffer.c_str(),
            body.c_str()
        )
    );
    return std::string(request_buffer);
}

std::string WebHelper::ComposeRequest(std::string page)
{
    HeaderList list;
    return ComposeRequest(GET, page, list, "");
}

WebHelper::StatusType WebHelper::SendRequest()
{
    return SendRequest( std::string(request_buffer), request_size );
}

WebHelper::StatusType WebHelper::SendRequest(std::string request, int request_len)
{
    if(!is_connected)
    {
        return (status = NOT_CONNECTED);
    }
    const char * rq_ptr = request.c_str();
    tcp_bytes_moved = 0, tbm = 0;
    do{
        tbm = 
            write( sckt, rq_ptr + tcp_bytes_moved, request_len - tcp_bytes_moved );
        if(tbm < 0)
            return (status = NETWORK_SEND_ERROR);
        else if(tbm == 0)
            break;
        tcp_bytes_moved += tbm;
    }while(tcp_bytes_moved < request_len);

    return (status = A_OK);
}

WebHelper::StatusType WebHelper::ReceiveResponse()
{
    tcp_bytes_moved = 0, tbm = 0;
    memset(response_buffer, 0, MAX_RESPONSE_BUFFER);
    do{
        tbm = 
            recv( sckt, response_buffer + tcp_bytes_moved, MAX_RESPONSE_BUFFER - tcp_bytes_moved, 0 );
        if(tbm < 0)
            return (status = NETWORK_SEND_ERROR);
        else if(tbm == 0)
        {
            break;
        }
        tcp_bytes_moved += tbm;
    }while(tcp_bytes_moved < MAX_RESPONSE_BUFFER);

    has_response = true;

    if(tcp_bytes_moved == MAX_RESPONSE_BUFFER)
        return (status = RESPONSE_BUFFER_FULL_ERROR);

    return (status = A_OK);
}

WebHelper::StatusType WebHelper::OpenSocketToHostname( )
{
    memset( hostname_ip, 0, sizeof(char) * IP_MAX_BUFFER );

    struct addrinfo *addr_list, hints = {};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    char port_s[16] = {0};
    snprintf( port_s, 15, "%d", HTTP_PORT );
    
    if(getaddrinfo( url.c_str(), port_s, &hints, &addr_list ) != 0)
        return (status = HOSTNAME_NOT_FOUND_ERROR);

    for(struct addrinfo * addr = addr_list; addr != NULL; addr = addr->ai_next)
    {
        sckt = socket( addr->ai_family, addr->ai_socktype, addr->ai_protocol );
        if(sckt < 0)
            break;
        else
        {
            freeaddrinfo(addr_list);
            memcpy( &honstname_addr, addr, sizeof(*addr) );
            return (status = A_OK);
        }
    }
    if(addr_list != NULL)
        freeaddrinfo(addr_list);
    return SOCKET_ERROR;
}

WebHelper::StatusType WebHelper::ConnectSocket()
{
    if( is_connected )
    {
        DisconnectSocket();
    }
    if(OpenSocketToHostname() != A_OK)
        return status;
    if(connect(sckt, honstname_addr.ai_addr, honstname_addr.ai_addrlen) < 0)
        return ( status = CONNECTION_ERROR );
    is_connected = true;
    return (status = A_OK);
}

WebHelper::StatusType WebHelper::DisconnectSocket()
{
    if( is_connected )
    {
        close( sckt );
        is_connected = false;
        return (status = A_OK);
    }
    return (status = NOT_CONNECTED);
}

WebHelper::StatusType WebHelper::End()
{
    if( is_connected )
    {
        DisconnectSocket();
    }
    sckt = -1;
    is_init =
    is_connected =
    has_response = false;
    tcp_bytes_moved = 
    tbm =
    request_size = 0;
    memset(hostname_ip, 0, IP_MAX_BUFFER);
    memset(response_buffer, 0, MAX_RESPONSE_BUFFER);
    memset(request_buffer, 0, MAX_REQUEST_BUFFER);

    return (status = A_OK);
}

WebHelper::StatusType WebHelper::GetStatus()
{
    return status;
}

std::string WebHelper::GetStatusDescription( )
{
    return GetStatusDescription(status, this);
}

std::string WebHelper::GetStatusDescription( StatusType index, WebHelper * web )
{
    std::string description;
    switch (index)
    {
    case SOCKET_ERROR:
        description = "Error with network socket.";
        break;
    case NO_HOSTNAME_ERROR:
        description = "Hostname (url) not specified.";
        break;
    case HOSTNAME_NOT_FOUND_ERROR:
        description += "Hostname (" + web->url + ") not found.";
        break;
    case CONNECTION_ERROR:
        description = "Cannot connect to host " + web->url + ".";
        break;
    case NETWORK_SEND_ERROR:
        description = "Error sending data to host " + web->url + ".";
        break;
    case NETWORK_RECEIVED_ERROR:
        description = "Error receiving data from host " + web->url + ".";
        break;
    case RESPONSE_BUFFER_FULL_ERROR:
        description = "Cannot store complete response on buffer.";
        break;
    case NO_RESPONSE:
        description = "Not response has been received from host.";
        break;
    case NOT_CONNECTED:
        description = "Not connected to host.";
        break;
    case NOT_INIT:
        description = "WebHelper object not initialized.";
        break;
    case A_OK:
        description = "No errors detected.";
        break;
    }
    return description;
}

WebHelper::HeaderList WebHelper::ParseHeaders( std::string raw )
{
    HeaderList header_list;
    Header header;
    std::istringstream raw_stream(raw);
    std::string curr_str;

    int 
        empty_line = 0, dots = 0;
    while(empty_line < 1)
    {
        curr_str ="";
        std::getline( raw_stream, curr_str );
        if(curr_str.empty())
        {
            empty_line ++;
        }
        else
        {
            if((dots = curr_str.find(": ")) > 0 && curr_str[0] != ' ')
            {
                header = (Header){};
                header.name = curr_str.substr(0, dots);
                header.content = curr_str.substr(dots+2, curr_str.length() - dots-2);
                header_list.push_back(header);
                dots =-1;
            }
        }
    }    
    return header_list;
}

WebHelper::StatusType WebHelper::ChangeUrl( std::string domain )
{
    url = domain;
    return Init();
}