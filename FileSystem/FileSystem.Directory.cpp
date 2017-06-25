#include "FileSystem.Directory.h"

#include "FileSystem.CMakeListsFile.h"
#include "FileSystem.File.h"

#include <fstream>
#include <iostream>

namespace nFileSystem
{


cDirectory::~cDirectory()
{
	for( unsigned int i = 0; i < mContent.size(); ++i )
		delete  mContent[ i ];

	delete  mCMakeFile;
}


nFileSystem::cDirectory::cDirectory( const std::string & iPath ) :
	tSuperClass( iPath ),
	mCMakeFile( 0 )
{
}


bool
cDirectory::IsDirectory() const
{
	return  true;
}


int 
cDirectory::PrintInCMakeListFile( std::ofstream& iOFStream ) const
{
	if( !IsCompiled() )
		iOFStream << "# ";

	iOFStream << "SUBDIRECTORY( " << Name() << " )\n";
	return 0;
}


int 
cDirectory::AddContent( cFileBase* iFile )
{
	iFile->Depth( Depth() + 1 );
	mContent.push_back( iFile );
	return 0;
}


int
cDirectory::SetMakeFile( cCMakeListsFile* iCMakeFile )
{
	mCMakeFile = iCMakeFile;
	return  0;
} 


int 
cDirectory::ReadAllPropertiesFromCMakeListsFile()
{
	if( !mCMakeFile )
		return  0;

	for( unsigned int i = 0; i < mContent.size(); ++i )
		mCMakeFile->ReadFileProperty( mContent[ i ] );

	return 0;
}


int 
cDirectory::CreateCMakeListFile()
{
	std::ofstream cMakeListsFile;
	cMakeListsFile.open( mCMakeFile->Path(), std::ios::in | std::ios::trunc );

	cMakeListsFile << "# ------CMakesLists file generated with CMLManager ----- \n\n";

	// Writes directories first
	for( unsigned int i = 0; i < mContent.size(); ++i )
	{
		if( mContent[ i ]->IsDirectory() )
			mContent[ i ]->PrintInCMakeListFile( cMakeListsFile );
	}

	cMakeListsFile << "\n\n";

	// Writes other files a l'arrache for now
	for( unsigned int i = 0; i < mContent.size(); ++i )
	{
		if( !mContent[ i ]->IsDirectory() )
			mContent[ i ]->PrintInCMakeListFile( cMakeListsFile );
	}

	cMakeListsFile << "\n\n";
	cMakeListsFile << "SET SOURCE_FILES( ${ SOURCE_FILES } + )\n";
	cMakeListsFile << "SET HEADERS_FILES( ${ HEADERS_FILES } + )\n"; 

	cMakeListsFile.close();
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
	printf( "Directory : %s    ", Name().c_str() );

	if( IsCompiled() )
		printf( " C" );
	if( IsNewFile() )
		printf( " N" );

	printf( "\n" );
	std::string tabs;
	for( int i = 0; i < Depth() + 1; ++i )
		tabs.append( "      " );

	for( unsigned int i = 0; i < mContent.size(); ++i )
	{
		printf( "%s", tabs.c_str() );
		mContent[ i ]->DebugPrint(); 
	}

	if( mCMakeFile )
	{
		printf( "%s", tabs.c_str() );
		mCMakeFile->DebugPrint(); 
	}

	return 0;
}

}

