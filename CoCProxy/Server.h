#pragma once
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include "Client.h"
#include <boost/algorithm/hex.hpp>
#include "Utils.h"

using boost::asio::ip::tcp;

class cServer
{
public:

    cServer( boost::asio::io_service& service ) : io_service_( service )
    {

    }

    void HandleClient( boost::shared_ptr<tcp::socket> session )
    {
        try {

            std::cout << "New Client: " << session->remote_endpoint().address() << ":" << session->remote_endpoint().port() << '\n';

            cClient client( io_service_, session );
            client.Connect( "gamea.clashofclans.com", "9339" );

            char header[ 7 ];
                                       
            while( true ) {
                boost::asio::read( *session, boost::asio::buffer( header, 7 ) );
               
                unsigned short type = _byteswap_ushort( *reinterpret_cast<unsigned short*>( header ) );
                unsigned int length = cUtils::IntFromBytes( header + 2, 3, false );

                std::cout << "[Client -> Server] " << type << " Length: " << length << '\n';

                char* buffer = new char[ length ];
                boost::asio::read( *session, boost::asio::buffer( buffer, length ) );
                
                std::ofstream dump_file( std::to_string( type ) + "_CLIENT_" + std::to_string( cUtils::RandomInt( 0, 999999 ) ) + ".bin", std::ios::binary );

                dump_file.write( header, 7 );
                dump_file.write( buffer, length );

                client.Send( 7, header );
                client.Send( length, buffer );
                delete[] buffer;
            }
        }
        catch( std::exception& exception ) {
            std::cout << "Server error: " << exception.what() << '\n';
        }
    }

    void Run()
    {
        tcp::acceptor acceptor = tcp::acceptor( io_service_, tcp::endpoint( tcp::v4(), 9339 ) );
        
        std::cout << "Started Server. Waiting for Connections\n";

        while( true ) {
            boost::shared_ptr<tcp::socket> session = boost::shared_ptr<tcp::socket>( new tcp::socket( io_service_ ) );
            acceptor.accept( *session );
            boost::thread t( boost::bind( &cServer::HandleClient, this, session ) );
        }
    }

private:
    boost::asio::io_service& io_service_;
};