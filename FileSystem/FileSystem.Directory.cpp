#include "FileSystem.Directory.h"

#include "FileSystem.CMakeListsFile.h"
#include "FileSystem.File.h"

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
cDirectory::PrintInCMakeListFile() const
{
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
	for( unsigned int i = 0; i < mContent.size(); ++i )
	{
		if( mContent[ i ]->IsCompiled() )
			mContent[ i ]->PrintInCMakeListFile();
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

