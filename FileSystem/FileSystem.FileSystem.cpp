
#include "FileSystem.FileSystem.h"

#include "FileSystem.CMakeListsFile.h"
#include "FileSystem.Directory.h"
#include "FileSystem.File.h"
#include "FileSystem.FileBase.h"

#include <locale>
#include <fstream>
#include <string.h>
#include <iostream>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

namespace nFileSystem
{

cFileSystem::~cFileSystem()
{
}


cFileSystem::cFileSystem()
{
    ReadOptions();
}


const std::string
cFileSystem::FavoritePath( unsigned int iPath ) const
{
    if( iPath >= mFavoritePaths.size() )
        return  "";

    return  mFavoritePaths[ iPath ];
}


unsigned long
cFileSystem::FavoriteCount() const
{
    return  mFavoritePaths.size();
}


// static
cDirectory*
cFileSystem::ReadDirectory( const std::string& iPath )
{
    DIR* directory = opendir( iPath.c_str() );
    if( directory == NULL )
    {
        printf( "Error while opening directory\n" );
        return  0;
    }

    ::nFileSystem::cDirectory* mainDirectory = new ::nFileSystem::cDirectory( iPath );

    struct dirent *files;
    struct stat fileStat;

    while( ( files = readdir( directory ) ) != NULL )
    {
        if( !strcmp( files->d_name, "." ) || !strcmp( files->d_name, ".." ) )
            continue;

        std::string filePath = iPath;
        filePath.append( "/" );
        filePath.append( files->d_name );

        if( stat( filePath.c_str(), &fileStat ) == -1 )
        {
            printf( "Couldn't get file information for %s\n", files->d_name );
            break;
        }

        if( S_ISDIR( fileStat.st_mode ) )
        {
            // Dismisses folders starting with . : .vscode, .svn
            if( files->d_name[ 0 ] == '.' )
                continue;

            ::nFileSystem::cDirectory* directory = ReadDirectory( filePath );
            directory->SortAlphabetically();
            mainDirectory->AddContent( directory );
        }
        else
        {
            if( !strcmp( files->d_name, "CMakeLists.txt" ) )
            {
                ::nFileSystem::cCMakeListsFile* makeFile = new ::nFileSystem::cCMakeListsFile( filePath );
                mainDirectory->SetMakeFile( makeFile );
            }
            else
            {
                // We only wanna add C++ related files, so we filter for cpp, c, mm, h, hpp files
                std::string fileName( files->d_name );
                std::size_t foundDot = fileName.find_last_of( '.' );
                if( foundDot != std::string::npos )
                {
                    std::string fileExtention = fileName.substr( foundDot + 1 );
                    //Going lower case for every character
                    for( unsigned int i = 0; i < fileExtention.size(); ++i )
                        fileExtention[ i ] = std::tolower( fileExtention[ i ], std::locale() );

                    if( !strcmp( fileExtention.c_str(), "cpp" )
                        || !strcmp( fileExtention.c_str(), "c" )
                        || !strcmp( fileExtention.c_str(), "mm" )
                        || !strcmp( fileExtention.c_str(), "h" )
                        || !strcmp( fileExtention.c_str(), "hpp" ) )
                    {
                        ::nFileSystem::cFile* file = new ::nFileSystem::cFile( filePath );
                        mainDirectory->AddContent( file );
                    }
                }
            }
        }
    }

    mainDirectory->ReadAllPropertiesFromCMakeListsFile();

    return  mainDirectory;
}


int 
cFileSystem::GenerateCMakeLists( const std::string& iPath, bool iUserConfirm, bool iRecursive )
{
    ::nFileSystem::cDirectory* mainDir = ReadDirectory( iPath );
    if( !mainDir )
    {
        printf( "%s does not name a file or directory\n", iPath.c_str() );
        return  1;
    }

    mainDir->SortAlphabetically();
    mainDir->DebugPrint();

    if( iUserConfirm )
    {
        char input;

        std::cout << "Generate CMakeLists ? (y/n)\n";
        std::cin >> input;

        if( input != 'y' && input != 'Y' )
            return  0;
    }

    std::cout << "Generating CMakeLists ...\n";
    mainDir->CreateCMakeListFile( iRecursive );
    std::cout << "Done\n";

    delete  mainDir;

    return  0;
}


int
cFileSystem::ReadOptions()
{
    cFile* optionFile = new cFile( "Options/option.txt" );
    if( !optionFile )
        return  1;

    std::ifstream optionStream( optionFile->Path() );
    if( optionStream.is_open() )
    {
        std::string path;
        while( getline( optionStream, path ) )
        {
            if( path[ 0 ] != '#' )
                mFavoritePaths.push_back( path );
        }
    }
    else
    {
        printf( "Stream for option not opened\n" );
        return  1;
    }

    printf( "Option successfully read : \n" );
    for( unsigned int i = 0; i < mFavoritePaths.size() ; ++i )
        printf( "%s\n", mFavoritePaths[ i ].c_str() );

    return 0;
}

}

