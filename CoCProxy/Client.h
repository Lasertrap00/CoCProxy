#pragma once
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <fstream>
#include "Utils.h"

using boost::asio::ip::tcp;

class cClient
{
public:

    cClient( boost::asio::io_service& io_service, boost::shared_ptr<tcp::socket>& session ) : session_( session ), io_service_( io_service ), socket_( io_service )
    {

    }


    void ReceiveThread()
    {
        try {
            char header[ 7 ];
            
            while( true ) {

                boost::asio::read( socket_, boost::asio::buffer( header, 7 ) );

                unsigned short type = _byteswap_ushort( *reinterpret_cast<unsigned short*>( header ) );
                unsigned int length = cUtils::IntFromBytes( header + 2, 3, false );

                std::cout << "[Server -> Client] " << type << " Length: " << length << '\n';

                char* buffer = new char[ length ];
                boost::asio::read( socket_, boost::asio::buffer( buffer, length ) );

                std::ofstream dump_file( std::to_string( type ) + "_SERVER_" + std::to_string( cUtils::RandomInt( 0, 999999 ) ) + ".bin", std::ios::binary );

                dump_file.write( header, 7 );
                dump_file.write( buffer, length );

                boost::asio::write( *session_, boost::asio::buffer( header, 7 ) );
                boost::asio::write( *session_, boost::asio::buffer( buffer, length ) );
                delete[] buffer;
            }
        }
        catch( std::exception& exception ) {
            std::cout << "Client error: " << exception.what() << '\n';
        }
    }

    void Connect( const std::string& host, const std::string& port )
    {
        tcp::resolver resolver( io_service_ );
        tcp::resolver::query q( tcp::v4(), host, port );
        
        boost::asio::connect( socket_, resolver.resolve( q ) );
        boost::thread t( boost::bind( &cClient::ReceiveThread, this ) );
    }

    void Send( size_t len, const char* buffer )
    {
        try {
            socket_.write_some( boost::asio::buffer( buffer, len ) );
        }
        catch( std::exception& exception ) {
            std::cout << "Client error: " << exception.what() << '\n';
        }
    }

private:

    boost::shared_ptr<tcp::socket>& session_;
    boost::asio::io_service& io_service_;
    tcp::socket socket_;
};