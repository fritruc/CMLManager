#include "FileSystem.Directory.h"

#include "FileSystem.CMakeListsFile.h"
#include "FileSystem.File.h"

namespace nFileSystem
{


cDirectory::~cDirectory()
{
	for( unsigned int i = 0; i < mContent.size(); ++i )
	{
		delete  mContent[ i ];
	}
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
	printf( "Content of directory : %s=====", Name().c_str() );

	if( IsCompiled() )
		printf( " C" );
	if( IsNewFile() )
		printf( " N" );

	printf( "\n" );

	for( unsigned int i = 0; i < mContent.size(); ++i )
	{
		mContent[ i ]->DebugPrint(); 
	}
	if( mCMakeFile )
		mCMakeFile->DebugPrint();

	printf( "===============\n" );
	return 0;
}

}

