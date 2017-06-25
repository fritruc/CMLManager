#pragma once

#include "FileSystem.FileBase.h"

namespace nFileSystem
{

class cFile :
	public cFileBase
{
	typedef cFileBase	tSuperClass;
	typedef cFile		tSelfClass;

public:
	enum eOS
	{
		kNone = 0,
		kLinux,
		kMacosx,
		kWindows
	};

public:
/**@Name Construction/Destruction */
//@{
	virtual  ~cFile();
	cFile( const std::string & iPath );
//@}

public:
/**@Name Accessors/Modifiers */
//@{
	eOS  FileOS() const; 
//@}

public:
/**@Name Computing methods */
//@{
	int  ReadOS();
//@}

public:
/**@Name FileBase Overrides */
//@{
	virtual  bool	IsDirectory()  const;

	virtual  int	PrintInCMakeListFile( std::ofstream& iOFStream ) const;
//@}

public:
/**@Name Debug */
//@{
	virtual  int  DebugPrint() const;
//@}

private:
	eOS mFileOS;

};


}


