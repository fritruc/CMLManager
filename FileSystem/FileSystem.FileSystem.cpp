
#include "FileSystem.FileSystem.h"

#include "FileSystem.CMakeListsFile.h"
#include "FileSystem.Directory.h"
#include "FileSystem.File.h"
#include "FileSystem.FileBase.h"

#include <string.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

namespace nFileSystem
{

cFileSystem::~cFileSystem()
{
}


cFileSystem::cFileSystem()
{
}


// static
cDirectory*
cFileSystem::ReadDirectory( const std::string& iPath )
{
	DIR* directory = opendir( iPath.c_str() );
	if( directory == NULL )
	{
		printf( "Error while opening directory\n" );
		return  0;
	}

	::nFileSystem::cDirectory* mainDirectory = new ::nFileSystem::cDirectory( iPath );

	struct dirent *files;
	struct stat fileStat;

	while( ( files = readdir( directory ) ) != NULL )
	{
		if( !strcmp( files->d_name, "." ) || !strcmp( files->d_name, ".." ) )
			continue;

		std::string filePath = iPath;
		filePath.append( "/" );
		filePath.append( files->d_name );

		if( stat( filePath.c_str(), &fileStat ) == -1 )
		{
			printf( "Couldn't get file information for %s\n", files->d_name );
			break;
		}

		if( S_ISDIR( fileStat.st_mode ) )
		{
			::nFileSystem::cDirectory* directory = ReadDirectory( filePath );
			mainDirectory->AddContent( directory );
		}
		else
		{
			if( !strcmp( files->d_name, "CMakeLists.txt" ) )
			{
				::nFileSystem::cCMakeListsFile* makeFile = new ::nFileSystem::cCMakeListsFile( filePath );
				mainDirectory->SetMakeFile( makeFile );
			}
			else
			{
				// We only wanna add C++ related files, so we filter for cpp, c, mm, h, hpp files
				std::string fileName( files->d_name );
				std::size_t foundDot = fileName.find_last_of( '.' );
				if( foundDot != std::string::npos )
				{
					std::string fileExtention = fileName.substr( foundDot + 1 );
					if( !strcmp( fileExtention.c_str(), "cpp" )
						|| !strcmp( fileExtention.c_str(), "c" )
						|| !strcmp( fileExtention.c_str(), "mm" )
						|| !strcmp( fileExtention.c_str(), "h" )
						|| !strcmp( fileExtention.c_str(), "hpp" ) )
					{
						::nFileSystem::cFile* file = new ::nFileSystem::cFile( filePath );
						mainDirectory->AddContent( file );
					}
				}
			}
		}
	}

	mainDirectory->ReadAllPropertiesFromCMakeListsFile();

	return  mainDirectory;
}

}

