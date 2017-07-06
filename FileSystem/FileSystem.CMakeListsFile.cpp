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

nFileSystem::cCMakeListsFile::cCMakeListsFile( const std::string& iPath ) :
    tSuperClass( iPath ),
    mExcluded( false )
{
}


bool 
cCMakeListsFile::Excluded() const
{
    return  mExcluded;
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

    std::string fileEntry;
    iFile->BuildCMakeListEntryString( &fileEntry );

    // Regex is powerfull but kinda slow
    //std::string fileName = iFile->Name();
    //EscapeAllRegChar( fileName );
    //std::regex fileSeeking( "(?:TARGET=\/|}\/| |\()" + iFile->Name() + "[ )]*$" ); 

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
                    std::string fileName = fileLine.substr( fileLine.find_first_of( '=' ) + 1 );
                    if( fileName.size() == 0 )
                        return  0;

                    cFile* file = new  cFile( fileName );
                    // Now, because this extra include will give a path like : #INCLUDEFILE=/../../someting/file.cpp
                    // cFile constructor will cut the name to file.cpp
                    // But in this special case, we actually don't want to simply print ${RELATIVE_DIR}/file.cpp
                    // we want to print the whole thing ${RELATIVE_DIR}/../../someting/file.cpp
                    // So we put here fileName, being the path, as it's actual name
                    if( fileName[ 0 ] == '/' )
                        fileName = fileName.substr( 1 );
                    file->Name( fileName );

                    makeList->ReadFileProperty( file );
                    mExtraIncludes.push_back( file );
                    continue;
                }

                found = fileLine.find( "#INCLUDEDIR" );
                if( found != std::string::npos )
                {
                    std::string dirName = fileLine.substr( fileLine.find_first_of( '=' ) + 1 );
                    if( dirName.size() == 0 )
                        return  0;

                    cDirectory* dir = new  cDirectory( dirName );
                    // Now, because this extra include will give a path like : #INCLUDEFILE=/../../someting/file.cpp
                    // cFile constructor will cut the name to file.cpp
                    // But in this special case, we actually don't want to simply print ${RELATIVE_DIR}/file.cpp
                    // we want to print the whole thing ${RELATIVE_DIR}/../../someting/file.cpp
                    // So we put here fileName, being the path, as it's actual name
                    if( dirName[ 0 ] == '/' )
                        dirName = dirName.substr( 1 );
                    dir->Name( dirName );

                    makeList->ReadFileProperty( dir );
                    mExtraIncludes.push_back( dir );
                    continue;
                }

                found = fileLine.find( "#EXCLUDE" );
                if( found != std::string::npos )
                {
                    mExcluded = true;
                    continue;
                }
            }
        }
        else
        {
            while( getline( file, fileLine ) )
            {
                //#TARGET=Name:STREQUAL:TVPDB
                if( fileLine.find( "#TARGET" ) != std::string::npos )
                {
                    fileLine = fileLine.substr( fileLine.find_first_of( '=' ) + 1 );
                    size_t firstDDotPos = fileLine.find_first_of( ':' );
                    size_t lastDDotPos = fileLine.find_last_of( ':' );

                    if( fileLine.substr( 0, firstDDotPos )  == iFile->Name() )
                    {
                        iFile->IsTargeted( true );
                        iFile->TargetName( fileLine.substr( lastDDotPos + 1 ) );
                        iFile->TargetOperator( fileLine.substr( firstDDotPos + 1, lastDDotPos - 1 - firstDDotPos ) );
                    }
                }
                else if( fileLine.find( fileEntry ) != std::string::npos )
                {
                //if( std::regex_search( fileLine, fileSeeking ) )
                //{
                    isNewFile = false;

                    if( fileLine.find( '#' ) != std::string::npos )
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
cCMakeListsFile::EscapeAllRegChar( std::string & oString )
{
    for( size_t i = 0; i < oString.size(); ++i )
    {
        if( oString[ i ] == '.' || oString[ i ] == '/' )
        {
            oString.insert( i, "\\" );
            ++i;
        }
    }

    return  0;
}


int
cCMakeListsFile::PrintInCMakeListFile( std::ofstream& iOFStream, int iIntentTabs ) const
{
    //#INCLUDEFILE=file
    //#INCLUDEDIR=file

    if( mExtraIncludes.size() == 0 )
        return  0;

    iOFStream << "#---------------------EXTRA INCLUDES---------------------\n\n"; 
    for( std::vector< cFileBase* >::const_iterator i( mExtraIncludes.begin() ); i != mExtraIncludes.end(); ++i )
    { 
        if( ( *i )->IsDirectory() )
            iOFStream << "#INCLUDEDIR=" + ( *i )->Path() + "\n";
        else
            iOFStream << "#INCLUDEFILE=" + ( *i )->Path() + "\n";
    }

    iOFStream << "\n\n\n";

    for( std::vector< cFileBase* >::const_iterator i( mExtraIncludes.begin() ); i != mExtraIncludes.end(); ++i )
    {
        cFileBase* file = *i;
        if( file->IsDirectory() )
            iOFStream << "SUBDIRECTORY( " << file->Path() << ")\n";
    }

    iOFStream << "\n\n";

    WriteSetSourcePart( iOFStream, 0 );
    for( std::vector< cFileBase* >::const_iterator i( mExtraIncludes.begin() ); i != mExtraIncludes.end(); ++i )
    {
        cFile* file = dynamic_cast<cFile*>( *i );
        if( file && file->FileType() == cFile::eType::kSource )
            file->PrintInCMakeListFile( iOFStream, 1 );
    }
    iOFStream << ")\n\n";

    WriteSetHeaderPart( iOFStream, 0 );
    for( std::vector< cFileBase* >::const_iterator i( mExtraIncludes.begin() ); i != mExtraIncludes.end(); ++i )
    {
        cFile* file = dynamic_cast<cFile*>( *i );
        if( file && file->FileType() == cFile::eType::kHeader )
            file->PrintInCMakeListFile( iOFStream, 1 );
    }
    iOFStream << ")\n\n\n";

    return  0;
}


int
cCMakeListsFile::DebugPrint() const
{
    printf( "CMakeFile : %s\n", Name().c_str() );

    std::string tabs;
    for( int i = 0; i < Depth() + 1; ++i )
        tabs.append( "      " );

    for( std::vector< cFileBase* >::const_iterator i( mExtraIncludes.begin() ); i != mExtraIncludes.end(); ++i )
    {
        printf( "%s EXTRA_INCLUDE :  %s\n", tabs.c_str(), (*i)->Path().c_str() );
    }

    return 0;
}


}

