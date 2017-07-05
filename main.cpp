#include "FileSystem/FileSystem.Directory.h"
#include "FileSystem/FileSystem.FileBase.h"
#include "FileSystem/FileSystem.FileSystem.h"

#include <cstdio>
#include <iostream>
#include <string.h>


int
main( int argc, char** argv )
{
    ::nFileSystem::cFileSystem* fileSystem = new ::nFileSystem::cFileSystem();

    if( !fileSystem )
        return  1;

    if( argc > 1 )
    { 
        bool noConfirm = ( argc > 2 && !strcmp( argv[ 2 ], "-noconfirm" ) );
        bool recursive = ( argc > 3 && !strcmp( argv[ 3 ], "-recursive" ) );
        std::cout << "Running generation over : " << argv[ 1 ] << " with parameters :\n";
        std::cout << "UserConfirm : " << !noConfirm << "\n";
        std::cout << "Recursive : " << recursive << "\n";

        fileSystem->GenerateCMakeLists( argv[ 1 ], !noConfirm, recursive );
    }
    else
    {
        for( unsigned int i = 0; i < fileSystem->FavoriteCount(); ++i )
        {
            fileSystem->GenerateCMakeLists( fileSystem->FavoritePath( i ), true, true );
        }
    }

    delete  fileSystem; 

    return 0;
}

