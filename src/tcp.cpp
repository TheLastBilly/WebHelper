#include "webhelper.hpp"

void WebHelper::SslSocketHandler::InitSsl()
{
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}

void WebHelper::SslSocketHandler::EndSsl()
{
    ERR_free_strings();
    EVP_cleanup();
}

WebHelper::SocketHandler::SocketHandler( )
{}

WebHelper::SocketHandler::~SocketHandler()
{
    End();
}

WebHelper::StatusType WebHelper::SocketHandler::Init( int fd )
{
    sckt = fd;
    return (status = A_OK);
}

WebHelper::StatusType WebHelper::SocketHandler::End()
{
    return (status = A_OK);
}

size_t WebHelper::SocketHandler::WriteToSocket(int fd, const void * buf, size_t n )
{
    return write( fd, buf, n );
}

size_t WebHelper::SocketHandler::ReadFromSocket(int fd, void * buf, size_t n, int flags )
{
    return recv( fd, buf, n, flags);
}

WebHelper::SslSocketHandler::SslSocketHandler(  )
{
    ssl_ctx = SSL_CTX_new( TLS_client_method());
    
    if(ssl_ctx == nullptr)
    {
        status = SSL_CERTIFICATE_ERROR;
    }
    else
    {
        status = A_OK;
    }
}

WebHelper::SslSocketHandler::~SslSocketHandler()
{
    End();
}


WebHelper::StatusType WebHelper::SslSocketHandler::Init( int fd )
{
    if(is_init)
        End();
    if(status != A_OK)
        return status;
    
    sckt = fd;
    c_ssl = SSL_new(ssl_ctx);
    if(c_ssl == nullptr)
    {
        return (status = SSL_ACCEPT_ERROR);
    }
    SSL_set_fd( c_ssl, sckt );
    if(SSL_connect(c_ssl) <= 0)
    {
        End();
        return (status = SSL_ACCEPT_ERROR);
    }

    is_init = true;
    return (status = A_OK);
}

WebHelper::StatusType WebHelper::SslSocketHandler::End()
{
    if(is_init)
    {
        if(c_ssl != nullptr)
        {
            SSL_shutdown(c_ssl);
            SSL_free(c_ssl);
            c_ssl = nullptr;
        }
        is_init = false;
        return (status = A_OK);
    }
    else
        return (status = NOT_INIT);
}

size_t WebHelper::SslSocketHandler::WriteToSocket(int fd, const void * buf, size_t n )
{
    if(is_init && c_ssl != nullptr)
    {
        return SSL_write( c_ssl, buf, n );
    }
    return -1;
}

size_t WebHelper::SslSocketHandler::ReadFromSocket(int fd, void * buf, size_t n, int flags )
{
    if(is_init && c_ssl != nullptr)
    {
        return SSL_read( c_ssl, buf, n );
    }
    return -1;
}