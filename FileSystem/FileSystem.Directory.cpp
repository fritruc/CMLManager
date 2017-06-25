#include "FileSystem.Directory.h"

#include "FileSystem.CMakeListsFile.h" 

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


bool 
cDirectory::IsThereOSSpecificFileInContent( cFile::eOS iOS ) const
{
	for( unsigned int i = 0; i < mContent.size(); ++i )
	{
		cFile* file = dynamic_cast< cFile* >( mContent[ i ] );
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

	for( unsigned int i = 0; i < mContent.size(); ++i )
		mCMakeFile->ReadFileProperty( mContent[ i ] );

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

	cMakeListsFile << "# ------CMakesLists file generated with CMLManager ----- \n\n";

	// Writes directories first
	for( unsigned int i = 0; i < mContent.size(); ++i )
	{
		if( mContent[ i ]->IsDirectory() )
			mContent[ i ]->PrintInCMakeListFile( cMakeListsFile );
		else
			break; //Assuming dir are given first and files then
	}

	cMakeListsFile << "\n\n";

	// Writes generic sources first
	for( unsigned int i = 0; i < mContent.size(); ++i )
	{
		if( !mContent[ i ]->IsDirectory() )
		{
			cFile* file = dynamic_cast< cFile* >( mContent[ i ] );
			if( file && file->FileType() == cFile::eType::kSource && file->FileOS() == cFile::eOS::kNone )
				file->PrintInCMakeListFile( cMakeListsFile );
		} 
	}

	cMakeListsFile << "\n\n";

	// Writes generic headers then
	for( unsigned int i = 0; i < mContent.size(); ++i )
	{
		if( !mContent[ i ]->IsDirectory() )
		{
			cFile* file = dynamic_cast< cFile* >( mContent[ i ] );
			if( file && file->FileType() == cFile::eType::kHeader && file->FileOS() == cFile::eOS::kNone )
				file->PrintInCMakeListFile( cMakeListsFile );
		}
	}

	cMakeListsFile << "\n\n";

	if( IsThereOSSpecificFileInContent( cFile::eOS::kLinux ) )
	{
		cMakeListsFile << "IFDEF( LINUX )\n";
		for( unsigned int i = 0; i < mContent.size(); ++i )
		{
			if( !mContent[ i ]->IsDirectory() )
			{
				cFile* file = dynamic_cast< cFile* >( mContent[ i ] );
				if( file && file->FileOS() == cFile::eOS::kLinux )
					file->PrintInCMakeListFile( cMakeListsFile );
			}
		}
		cMakeListsFile << "ENDIF\n\n";
	}

	if( IsThereOSSpecificFileInContent( cFile::eOS::kMacosx ) )
	{
		cMakeListsFile << "IFDEF( MACOSX )\n";
		for( unsigned int i = 0; i < mContent.size(); ++i )
		{
			if( !mContent[ i ]->IsDirectory() )
			{
				cFile* file = dynamic_cast< cFile* >( mContent[ i ] );
				if( file && file->FileOS() == cFile::eOS::kMacosx )
					file->PrintInCMakeListFile( cMakeListsFile );
			}
		}
		cMakeListsFile << "ENDIF\n\n";
	}

	if( IsThereOSSpecificFileInContent( cFile::eOS::kWindows ) )
	{
		cMakeListsFile << "IFDEF( WINDOWS )\n";
		for( unsigned int i = 0; i < mContent.size(); ++i )
		{
			if( !mContent[ i ]->IsDirectory() )
			{
				cFile* file = dynamic_cast< cFile* >( mContent[ i ] );
				if( file && file->FileOS() == cFile::eOS::kWindows )
					file->PrintInCMakeListFile( cMakeListsFile );
			}
		}
		cMakeListsFile << "ENDIF\n\n";
	}

	cMakeListsFile << "\n\n";
	cMakeListsFile << "SET SOURCE_FILES( ${ SOURCE_FILES } + )\n";
	cMakeListsFile << "SET HEADERS_FILES( ${ HEADERS_FILES } + )\n"; 

	cMakeListsFile.close();

	if( iRecursive )
	{
		for( unsigned int i = 0; i < mContent.size(); ++i )
		{
			if( mContent[ i ]->IsDirectory() )
				dynamic_cast< cDirectory* >( mContent[ i ] )->CreateCMakeListFile( iRecursive );
			else
				break; //Assuming dir are given first and files then
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

