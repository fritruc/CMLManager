#include "FileSystem.CMakeListsFile.h"

#include "FileSystem.Directory.h"


#include <fstream>
#include <iostream>
#include <regex>

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
cCMakeListsFile::ReadFileProperty( cFileBase* iFile )
{
    std::ifstream file( Path() );

    std::string fileLine;
    bool isCompiled = true;
    bool isNewFile = true;

    std::regex fileSeeking( "[ /(]" + iFile->Name() + "[ )]*$" );

    if( file.is_open() )
    {
        cCMakeListsFile* makeList = dynamic_cast< cCMakeListsFile* >( iFile );
        if( makeList )
        {
            while( getline( file, fileLine ) )
            {
                // CMakeLists files attributes are all on top of the file,
                // so whenever you encounter the first subdirectory or $relativedir line, you can stop
                std::size_t found = fileLine.find( "SUBDIRECTORY" );
                if( found != std::string::npos )
                    break;
                found = fileLine.find( "${RELATIVE_DIR}" );
                if( found != std::string::npos )
                    break;

                found = fileLine.find( "#INCLUDEFILE" );
                if( found != std::string::npos )
                {
                    cFile* file = new  cFile( fileLine.substr( fileLine.find_first_of( '=' ) + 1 ) );
                    makeList->ReadFileProperty( file );
                    mExtraIncludes.insert( file );
                }
                else
                {
                    found = fileLine.find( "#INCLUDEDIR" );
                    if( found != std::string::npos )
                    {
                        cDirectory* dir = new  cDirectory( fileLine.substr( fileLine.find_first_of( '=' ) + 1 ) );
                        makeList->ReadFileProperty( dir );
                        mExtraIncludes.insert( dir );
                    }
                }
            }
        }
        else
        {
            while( getline( file, fileLine ) )
            {
                std::size_t found;
                // found = fileLine.find( iFile->Name() );
                // if( found != std::string::npos )
                // {
                if( std::regex_search( fileLine, fileSeeking ) )
                {
                    isNewFile = false;

                    found = fileLine.find( "#TARGET" );
                    if( found != std::string::npos )
                    {
                        iFile->IsTargeted( true );
                        iFile->TargetName( "TARGET" );
                        continue;
                    }


                    found = fileLine.find( '#' );
                    if( found != std::string::npos )
                        isCompiled = false;

                    break;
                }
            }
        }

        file.close();
    }

    iFile->IsNewFile( isNewFile );
    iFile->IsCompiled( isCompiled );
    return  0;
}


std::string
cCMakeListsFile::ExtractTarget( std::ifstream& iIFStream ) const
{
    return  std::string();
}


std::string
cCMakeListsFile::ExtractIncludeFile( std::ifstream& iIFStream ) const
{
    return std::string();
}


std::string
cCMakeListsFile::ExtractIncludeDir( std::ifstream& iIFStream ) const
{
    return std::string();
}


int
cCMakeListsFile::PrintInCMakeListFile( std::ofstream& iOFStream, int iIntentTabs ) const
{
    //#INCLUDEFILE=file
    //#INCLUDEDIR=file

    if( mExtraIncludes.size() == 0 )
        return  0;

    for( std::set< cFileBase* >::iterator i( mExtraIncludes.begin() ); i != mExtraIncludes.end(); ++i )
    {
        cFileBase* file = *i;
        if( file->IsDirectory() )
            iOFStream << "SUBDIRECTORY( " << file->Name() << ")\n";
    }

    iOFStream << "\n\n";

    WriteSetSourcePart( iOFStream, 0 );
    for( std::set< cFileBase* >::iterator i( mExtraIncludes.begin() ); i != mExtraIncludes.end(); ++i )
    {
        cFile* file = dynamic_cast<cFile*>( *i );
        if( file->FileType() == cFile::eType::kSource )
            file->PrintInCMakeListFile( iOFStream, 1 );
    }
    iOFStream << ")\n\n";

    WriteSetHeaderPart( iOFStream, 0 );
    for( std::set< cFileBase* >::iterator i( mExtraIncludes.begin() ); i != mExtraIncludes.end(); ++i )
    {
        cFile* file = dynamic_cast<cFile*>( *i );
        if( file->FileType() == cFile::eType::kHeader )
            file->PrintInCMakeListFile( iOFStream, 1 );
    }
    iOFStream << ")\n\n";

    return  0;
}


int
cCMakeListsFile::DebugPrint() const
{
    printf( "CMakeFile : %s\n", Name().c_str() );

    return 0;
}


}

