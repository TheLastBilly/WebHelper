#include <iostream>

#include <webhelper.hpp>

int main(int argc, char const *argv[])
{
    WebHelper web("www.example.com");
    if(
        web.GetStatus() != WebHelper::A_OK || 
        web.RequestPage("index.html") != WebHelper::A_OK
    )
    {
        std::cout << web.GetStatusDescription() << std::endl;
    }
    else
    {
        std::cout << web.GetResponse() << std::endl;
    }
    
    return 0;
}
