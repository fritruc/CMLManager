#include "FileSystem.CMakeListsFile.h"

#include <fstream>
#include <iostream>

namespace nFileSystem
{


cCMakeListsFile::~cCMakeListsFile()
{
}

nFileSystem::cCMakeListsFile::cCMakeListsFile( const std::string & iPath ) :
    tSuperClass( iPath )
{
}

bool
cCMakeListsFile::IsDirectory() const
{
    return  false;
}


int
cCMakeListsFile::ReadFileProperty( cFileBase* iFile ) const
{
    std::ifstream file( Path() );

    std::string fileLine;
    bool isCompiled = true;
    bool isNewFile = true;

    if( file.is_open() )
    {
        while( getline( file, fileLine ) )
        {
            // TODO: Not find, otherwise it'll mix Test & TestSuite & SuiteTest
            std::size_t found = fileLine.find( iFile->Name() );
            if( found != std::string::npos )
            {
                isNewFile = false;

                found = fileLine.find( '#' );
                if( found != std::string::npos )
                    isCompiled = false;

                break;
            }
        }

        file.close();
    }

    iFile->IsNewFile( isNewFile );
    iFile->IsCompiled( isCompiled );
    return  0;
}


int
cCMakeListsFile::PrintInCMakeListFile( std::ofstream& iOFStream, int iIntentTabs ) const
{
    //TODO: add exceptions for TARGET and INCLUDE :
    /*
    example :
    ${RELATIVE_DIR}/../TVPA/TVPA.MagicNumbers.h

    */
    return  0;
}


int
cCMakeListsFile::DebugPrint() const
{
    printf( "CMakeFile : %s\n", Name().c_str() );

    return 0;
}


}

