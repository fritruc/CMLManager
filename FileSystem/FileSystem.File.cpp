#include "FileSystem.File.h"

#include <locale>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>




namespace nFileSystem
{


cFile::~cFile()
{
}

nFileSystem::cFile::cFile( const std::string & iPath ) :
	tSuperClass( iPath )
{
	ReadOS();
}


cFile::eOS
cFile::FileOS() const
{
	return  mFileOS;
}


int 
cFile::ReadOS()
{
	std::string name = Name();
	std::size_t dashPos = name.find_last_of( '-' );

	// No - in the file name = not OS specific
	if( dashPos == std::string::npos )
		return  0;

	std::string os = name.substr( dashPos + 1 );
 
	// This would be, just a - like File-
	if( os.size() <= 1 )
		return  0;

	// going to lower, so we can easily compare
	for( unsigned int i = 0; i < os.size(); ++i )
		os[ i ] = std::tolower( os[ i ], std::locale() );

	std::size_t found = os.find( "linux" );
	if( found != std::string::npos )
	{
		mFileOS = kLinux;
		return  0;
	}  

	found = os.find( "mac" );
	if( found != std::string::npos )
	{
		mFileOS = kMacosx;
		return  0;
	}

	found = os.find( "cocoa" );
	if( found != std::string::npos )
	{
		mFileOS = kMacosx;
		return  0;
	} 

	found = os.find( "win" );
	if( found != std::string::npos )
	{
		mFileOS = kWindows;
		return  0;
	} 

	mFileOS = kNone;

	return 0;
}


bool 
cFile::IsDirectory() const
{
	return  false;
}


int 
cFile::PrintInCMakeListFile( std::ofstream& iOFStream ) const
{
	if( !IsCompiled() )
		iOFStream << "# ";

	iOFStream << "SET INCLUDE( " << Path() << " )\n";
	return 0;
}


int
cFile::DebugPrint() const
{
	printf( "File : %s", Name().c_str() );

	if( IsCompiled() )
		printf( " %*c", 20 - Name().size(), 'C' );
	if( IsNewFile() )
		printf( "  N" );

	switch( mFileOS )
	{
		case kLinux :
		{
			printf( " -- Linux File" );
			break;
		}
		case kMacosx :
		{
			printf( " -- MacOSX File" );
			break;
		}
		case kWindows:
		{
			printf( " -- Windows File" );
			break;
		}
		default :
			break;
	}

	printf( "\n" );

	return 0;
}


}

 