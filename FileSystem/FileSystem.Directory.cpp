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
cDirectory::BuildCMakeListEntryString( std::string * oString ) const
{
    *oString = "ADD_SUBDIRECTORY( " + Name() + " )";
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

    //==================

    std::string stuffToWrite;

    if( FileOS() != kNone )
    {
        stuffToWrite += tabs + "\nIF( " + os + " )\n";
        tabsSUBDORECTORY.append( "    " );
    }

    if( !IsCompiled() )
        stuffToWrite += "# ";

    std::string dirEntry;
    BuildCMakeListEntryString( &dirEntry );
    stuffToWrite += tabsSUBDORECTORY + dirEntry + "\n";

    if( FileOS() != kNone )
        stuffToWrite += tabs + "ENDIF( " + os + " )\n";

    if( IsTargeted() )
        WriteTargetPart( &stuffToWrite, iIntentTabs, stuffToWrite );

    iOFStream << stuffToWrite;

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

    mCMakeFile->ReadFileProperty( mCMakeFile );

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
    //TODO: Add flags in directory to know if there is any directory, os spec file, sourcefile within
    // so we can avoid writing parts like #------ Directory ------- for nothing
    // + If a directory contains nothing, we don't add it


    // If directory is empty, we don't create a CMakeLists file
    if( mContent.size() == 0 )
        return  0;

    //Opening or creating CMakeLists file in the directory
    std::ofstream cMakeListsFile;
    cMakeListsFile.open( Path() + "/CMakeLists.txt", std::ios::in | std::ios::trunc );

    //Start of file
    cMakeListsFile << "# ------CMakesLists file generated with CMLManager ------ \n\n";

    // Output Target infos
    for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
    {
        if( ( *i )->IsTargeted() )
            cMakeListsFile << "#TARGET=" + ( *i )->Name() + ":" + ( *i )->TargetOperator() + ":" + ( *i )->TargetName() + "\n";
    }

    cMakeListsFile << "\n\n";

    // If we have a CMakeLists file in that directory, we output his things : Extra includes
    if( mCMakeFile )
        mCMakeFile->PrintInCMakeListFile( cMakeListsFile, 0 );

    cMakeListsFile << "#-----------------------DIRECTORIES----------------------\n";
    cMakeListsFile << "\n\n";

    // Writes directories first
    for( std::vector< cFileBase* >::iterator i( mContent.begin() ); i != mContent.end(); ++i )
    {
        if( (*i)->IsDirectory() )
            (*i)->PrintInCMakeListFile( cMakeListsFile, 0 );
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
        cMakeListsFile << "ENDIF( WINDOWS )\n\n\n";
    }

    cMakeListsFile << "#------------------Set in parent scope-------------------\n";
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
        }
    }

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

    if( IsTargeted() )
        printf( "   Targeted : %s", TargetName().c_str() );

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

