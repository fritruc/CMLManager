#include "FileSystem.Directory.h"

#include "FileSystem.CMakeListsFile.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <string.h>

namespace nFileSystem
{


cDirectory::~cDirectory()
{
    for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
        delete  *i;

    delete  mCMakeFile;
}


cDirectory::cDirectory( const std::string & iPath ) :
    tSuperClass( iPath ),
    mCMakeFile( 0 )
{
    ReadOS();
}


bool
cDirectory::IsDirectory() const
{
    return  true;
}


int
cDirectory::IncDepth()
{
    tSuperClass::IncDepth();

    for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
        (*i)->IncDepth();

    return 0;
}


int
cDirectory::PrintInCMakeListFile( std::ofstream& iOFStream, int iIntentTabs ) const
{
    std::string tabs;               // The tabs as string
    std::string tabsSUBDORECTORY;   // The tabs to use when writing the SUBDIRECTORY line, can be different form tabs if OSSpecific
    for( int i = 0; i < iIntentTabs; ++i )
        tabs.append( "    " );

    tabsSUBDORECTORY = tabs;

    std::string os = "";
    switch( FileOS() )
    {
        case  kLinux :
        {
            os = "LINUX";
            break;
        }
        case  kMacosx:
        {
            os = "MACOSX";
            break;
        }
        case  kWindows:
        {
            os = "WINDOWS";
            break;
        }
        default :
        {
            break;
        }
    }

    if( FileOS() != kNone )
    {
        iOFStream << tabs << "\nIF( " + os + " )\n";
        tabsSUBDORECTORY.append( "    " );
    }

    if( !IsCompiled() )
        iOFStream << "# ";

    iOFStream << tabsSUBDORECTORY << "SUBDIRECTORY( " << Name() << " )\n";

    if( FileOS() != kNone )
    {
        iOFStream << tabs << "ENDIF( " + os + " )\n\n";
    }

    return 0;
}


int
cDirectory::ReadOS()
{
    std::string name = Name();

    if( name.size() <= 0 )
        return  0;

    // going to lower, so we can easily compare
    for( unsigned int i = 0; i < name.size(); ++i )
        name[ i ] = std::tolower( name[ i ], std::locale() );

    if( !strcmp( name.c_str(), "linux" ) )
    {
        FileOS( kLinux );
        return  0;
    }

    if( !strcmp( name.c_str(), "macosx" ) || !strcmp( name.c_str(), "cocoa" ) )
    {
        FileOS( kMacosx );
        return  0;
    }

    if( !strcmp( name.c_str(), "windows" ) || !strcmp( name.c_str(), "win32" ) || !strcmp( name.c_str(), "win64" ) )
    {
        FileOS( kWindows );
        return  0;
    }

    FileOS( kNone );

    return 0;
}


int
cDirectory::AddContent( cFileBase* iFile )
{
    // If we meet an os specific file and if this directory is OS specific,
    // then it'll be specific to that same OS
    cFileOSSpecific* fileOSSpecific = dynamic_cast< cFileOSSpecific* >( iFile );
    if( fileOSSpecific && FileOS() != kNone )
        fileOSSpecific->FileOS( FileOS() );

    iFile->IncDepth();
    mContent.push_back( iFile );
    return 0;
}


int
cDirectory::SetMakeFile( cCMakeListsFile* iCMakeFile )
{
    mCMakeFile = iCMakeFile;
    return  0;
}


bool
cDirectory::IsThereOSSpecificFileInContent( cFileOSSpecific::eOS iOS ) const
{
    for( std::vector< cFileBase* >::const_iterator i( mContent.begin() ) ; i != mContent.end() ; ++i )
    {
        cFile* file = dynamic_cast< cFile* >( *i );
        if( file && file->FileOS() == iOS )
            return  true;
    }

    return false;
}


int
cDirectory::ReadAllPropertiesFromCMakeListsFile()
{
    if( !mCMakeFile )
        return  0;

    for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
        mCMakeFile->ReadFileProperty( *i );

    return 0;
}

static
bool
CompareFiles( const  cFileBase* iRHS, const  cFileBase* iLHS )
{
    return  iRHS->Name() < iLHS->Name();
}

int
cDirectory::SortAlphabetically()
{
    std::sort( mContent.begin(), mContent.end(), &CompareFiles );
    return 0;
}


int
cDirectory::CreateCMakeListFile( bool iRecursive )
{
    // If directory is empty, we don't create a CMakeLists file
    if( mContent.size() == 0 )
        return  0;

    //Opening or creating CMakeLists file in the directory
    std::ofstream cMakeListsFile;
    cMakeListsFile.open( Path() + "/CMakeLists.txt", std::ios::in | std::ios::trunc );

    cMakeListsFile << "# ------CMakesLists file generated with CMLManager ------ \n\n";

    mCMakeFile->PrintInCMakeListFile( cMakeListsFile, 0 );

    cMakeListsFile << "#-----------------------DIRECTORIES----------------------\n";
    cMakeListsFile << "\n\n";

    // Writes directories first
    for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
    {
        if( (*i)->IsDirectory() )
            (*i)->PrintInCMakeListFile( cMakeListsFile, 0 );
        else
            break; //Assuming dir are given first and files then
    }

    cMakeListsFile << "\n\n";
    cMakeListsFile << "#-------------------------FILES--------------------------\n";
    cMakeListsFile << "\n\n";

    cMakeListsFile << "FILE( RELATIVE_PATH RELATIVE_DIR ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )\n\n";

    // Writes generic sources first
    WriteSetSourcePart( cMakeListsFile, 0 );
    for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
    {
        if( !(*i)->IsDirectory() )
        {
            cFile* file = dynamic_cast< cFile* >( *i );
            if( file && file->FileType() == cFile::eType::kSource && file->FileOS() == cFileOSSpecific::eOS::kNone )
                file->PrintInCMakeListFile( cMakeListsFile, 1 );
        }
    }
    cMakeListsFile << ")\n\n";

    // Writes generic headers then
    WriteSetHeaderPart( cMakeListsFile, 0 );
    for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
    {
        if( !(*i)->IsDirectory() )
        {
            cFile* file = dynamic_cast< cFile* >( *i );
            if( file && file->FileType() == cFile::eType::kHeader && file->FileOS() == cFileOSSpecific::eOS::kNone )
                file->PrintInCMakeListFile( cMakeListsFile, 1 );
        }
    }
    cMakeListsFile << ")\n\n";
    cMakeListsFile << "#-------------------------OSFILES------------------------\n";
    cMakeListsFile << "\n\n";


    if( IsThereOSSpecificFileInContent( cFileOSSpecific::eOS::kLinux ) )
    {
        cMakeListsFile << "IF( LINUX )\n";

        //Sources
        WriteSetSourcePart( cMakeListsFile, 1 );
        for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
        {
            if( !(*i)->IsDirectory() )
            {
                cFile* file = dynamic_cast< cFile* >( *i );
                if( file && file->FileOS() == cFileOSSpecific::eOS::kLinux && file->FileType() == cFile::eType::kSource )
                    file->PrintInCMakeListFile( cMakeListsFile, 2 );
            }
        }
        cMakeListsFile << "    )\n\n";

        //Headers
        WriteSetHeaderPart( cMakeListsFile, 1 );
        for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
        {
            if( !(*i)->IsDirectory() )
            {
                cFile* file = dynamic_cast< cFile* >( *i );
                if( file && file->FileOS() == cFileOSSpecific::eOS::kLinux && file->FileType() == cFile::eType::kHeader )
                    file->PrintInCMakeListFile( cMakeListsFile, 2 );
            }
        }
        cMakeListsFile << "    )\n";

        cMakeListsFile << "ENDIF( LINUX )\n\n";
    }

    if( IsThereOSSpecificFileInContent( cFileOSSpecific::eOS::kMacosx ) )
    {
        cMakeListsFile << "IF( MACOSX )\n";

        //Sources
        WriteSetSourcePart( cMakeListsFile, 1 );
        for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
        {
            if( !(*i)->IsDirectory() )
            {
                cFile* file = dynamic_cast< cFile* >( *i );
                if( file && file->FileOS() == cFileOSSpecific::eOS::kMacosx && file->FileType() == cFile::eType::kSource )
                    file->PrintInCMakeListFile( cMakeListsFile, 2 );
            }
        }
        cMakeListsFile << "    )\n\n";

        //Headers
        WriteSetHeaderPart( cMakeListsFile, 1 );
        for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
        {
            if( !(*i)->IsDirectory() )
            {
                cFile* file = dynamic_cast< cFile* >( *i );
                if( file && file->FileOS() == cFileOSSpecific::eOS::kMacosx && file->FileType() == cFile::eType::kHeader )
                    file->PrintInCMakeListFile( cMakeListsFile, 2 );
            }
        }
        cMakeListsFile << "    )\n";

        cMakeListsFile << "ENDIF( MACOSX )\n\n";
    }

    if( IsThereOSSpecificFileInContent( cFileOSSpecific::eOS::kWindows ) )
    {
        cMakeListsFile << "IF( WINDOWS )\n";

        //Sources
        WriteSetSourcePart( cMakeListsFile, 1 );
        for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
        {
            if( !(*i)->IsDirectory() )
            {
                cFile* file = dynamic_cast< cFile* >( *i );
                if( file && file->FileOS() == cFileOSSpecific::eOS::kWindows && file->FileType() == cFile::eType::kSource )
                    file->PrintInCMakeListFile( cMakeListsFile, 2 );
            }
        }
        cMakeListsFile << "    )\n\n";

        //Headers
        WriteSetHeaderPart( cMakeListsFile, 1 );
        for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
        {
            if( !(*i)->IsDirectory() )
            {
                cFile* file = dynamic_cast< cFile* >( *i );
                if( file && file->FileOS() == cFileOSSpecific::eOS::kWindows && file->FileType() == cFile::eType::kHeader )
                    file->PrintInCMakeListFile( cMakeListsFile, 2 );
            }
        }
        cMakeListsFile << "    )\n";
        cMakeListsFile << "ENDIF( WINDOWS )\n\n";
    }

    cMakeListsFile << "#------------------Set in parent scope-------------------";
    cMakeListsFile << "\n\n";

    WriteSetSourcePart( cMakeListsFile, 0 );
    cMakeListsFile << "    PARENT_SCOPE\n";
    cMakeListsFile << ")\n\n";

    WriteSetHeaderPart( cMakeListsFile, 0 );
    cMakeListsFile << "    PARENT_SCOPE\n";
    cMakeListsFile << ")\n\n";

    cMakeListsFile << "SET(\n";
    cMakeListsFile << "    INCLUDE_DIRECTORIES\n";
    cMakeListsFile << "    ${INCLUDE_DIRECTORIES}\n";
    cMakeListsFile << "    ${CMAKE_CURRENT_SOURCE_DIR}\n";
    cMakeListsFile << "    PARENT_SCOPE\n";
    cMakeListsFile << ")\n";

    cMakeListsFile.close();

    if( iRecursive )
    {
        for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
        {
            if( (*i)->IsDirectory() )
                dynamic_cast< cDirectory* >( *i )->CreateCMakeListFile( iRecursive );
            else
                break; //Assuming dir are given first and files then
        }
    }

    /* Target naming

    IF( ${CMAKE_BUILD_TYPE} STREQUAL "TVPDB" )
        ADD_SUBDIRECTORY( TVPDB )
    ENDIF()
    IF( ${CMAKE_BUILD_TYPE} STRNEQUAL "TVPDB" )
        ADD_SUBDIRECTORY( TVPA )
    ENDIF()
    */

    return 0;
}


int
cDirectory::DebugPrint() const
{
    DebugPrintContent();

    return 0;
}


int
cDirectory::DebugPrintContent() const
{
    printf( "Directory : %s", Name().c_str() );
    printf( "%*c", 95 - int( Name().size() ) - Depth() * 6, ' ' );

    if( IsCompiled() )
        printf( "C  " );
    else
        printf( "NC " );

    if( IsNewFile() )
        printf( "  N" );
    else
        printf( "   " );

    // Skip the file type
    printf( "               " );

    switch( FileOS() )
    {
        case kLinux:
        {
            printf( "   -- Linux Dir" );
            break;
        }
        case kMacosx:
        {
            printf( "   -- MacOSX Dir" );
            break;
        }
        case kWindows:
        {
            printf( "   -- Windows Dir" );
            break;
        }
        default:
            break;
    }

    printf( "\n" );
    std::string tabs;
    for( int i = 0; i < Depth() + 1; ++i )
        tabs.append( "      " );

    for( std::vector< cFileBase* >::const_iterator i( mContent.begin() ); i != mContent.end(); ++i )
    {
        printf( "%s", tabs.c_str() );
        (*i)->DebugPrint();
    }

    if( mCMakeFile )
    {
        printf( "%s", tabs.c_str() );
        mCMakeFile->DebugPrint();
    }

    return 0;
}

}

