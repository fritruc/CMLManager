#include "FileSystem/FileSystem.Directory.h"
#include "FileSystem/FileSystem.FileBase.h"
#include "FileSystem/FileSystem.FileSystem.h"

#include <cstdio>
#include <iostream>


int
main( int argc, char** argv )
{
    ::nFileSystem::cFileSystem* fileSystem = new ::nFileSystem::cFileSystem();

    if( !fileSystem )
        return  1; 

    for( unsigned int i = 0; i < fileSystem->FavoriteCount(); ++i )
    {
        ::nFileSystem::cDirectory* mainDir = ::nFileSystem::cFileSystem::ReadDirectory( fileSystem->FavoritePath( i ) );

        if( !mainDir )
            return  0;

        mainDir->DebugPrint();

        char input;

        std::cout << "Generate CMakeLists ? (y/n)\n";
        std::cin >> input;

        if( input == 'y' || input == 'Y' )
        {
            std::cout << "Generating CMakeLists ...\n";
            mainDir->CreateCMakeListFile( true );
            std::cout << "Done\n";
        }

        delete  mainDir;
    }
    delete  fileSystem;

    return 0;
}

