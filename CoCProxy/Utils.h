#pragma once
#include <random>

class cUtils
{
public:

    static int RandomInt( const int min, const int max )
    {
        std::random_device device;
        std::mt19937 mersenne_twister = std::mt19937( device() );
        std::uniform_int_distribution<int> distribution( min, max );
        return distribution( mersenne_twister );
    }

    template<typename T>
    static void Swap( T* p1, T* p2 )
    {
        T temp = *p1;
        *p1 = *p2;
        *p2 = temp;
    }

    static int IntFromBytes( char* bytes, size_t byte_count, bool little_endian )
    {
        const unsigned char* start = reinterpret_cast<unsigned char*>( bytes );
        const unsigned char* end = reinterpret_cast<unsigned char*>( bytes ) + byte_count - 1;
        int incrementor = 1;

        if( !little_endian ) {
            Swap( &start, &end );
            incrementor = -1;
        }

        int pos;

        // Get the first significant byte
        for( pos = 0; pos < byte_count; ++pos ) {
            if( *( end + ( ( -incrementor ) * pos ) ) != 0x00 )
                break;
        }

        size_t significant_bytes = byte_count - pos;

        unsigned int result = 0;
        for( pos = 0; pos < significant_bytes; ++pos ) {
            result |= *( start + ( incrementor * pos ) ) << ( pos * 8 );
        }

        return result;
    }
};