#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "Server.h"
#include <random>

void SetupConsole()
{
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
    SetConsoleTitleA( "CoCProxy by Lasertrap" );
}

int main()
{
    SetupConsole();

    boost::asio::io_service io_service;

    cServer Server( io_service );
    Server.Run();
    io_service.run();

    return 0;
}