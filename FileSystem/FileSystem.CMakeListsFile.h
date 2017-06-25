#pragma once

#include "FileSystem.File.h"

namespace nFileSystem
{

class cCMakeListsFile :
	public cFile
{
	typedef cFile			tSuperClass;
	typedef cCMakeListsFile	tSelfClass;


public:
/**@Name Construction/Destruction */
//@{
	virtual  ~cCMakeListsFile();
	cCMakeListsFile( const std::string & iPath );
//@}

public:
/**@Name FileBase Overrides */
//@{
	virtual  bool	IsDirectory()  const;

	virtual  int	PrintInCMakeListFile( std::ofstream& iOFStream ) const;
//@}

public:
/**@Name CMakeLists reading */
//@{
	int				ReadFileProperty( cFileBase* iFile )  const; 
//@}

public:
/**@Name Debug */
//@{
	virtual  int  DebugPrint() const;
//@}

};


}


