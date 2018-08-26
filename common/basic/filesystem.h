#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <cstdlib>
#include <iomanip>
#include <direct.h>

class FileSystem
{
private:
  typedef std::string (*Builder) (const std::string& path);

public:
  static std::string getPath(const std::string& path)
  {
    static std::string(*pathBuilder)(std::string const &) = getPathBuilder();
    return (*pathBuilder)(path);
  }

  static std::string const & getRoot()
  {
	char rootPath[_MAX_PATH] = "";
	_getcwd(rootPath, _MAX_PATH);
	static std::string root = rootPath;
    
	static auto path = root.substr(0, root.find_last_of('\\'));
    return path;
  }

  //static std::string(*foo (std::string const &)) getPathBuilder()
  static Builder getPathBuilder()
  {
    if (getRoot() != "")
      return &FileSystem::getPathRelativeRoot;
    else
      return &FileSystem::getPathRelativeBinary;
  }

  static std::string getPathRelativeRoot(const std::string& path)
  {
    return getRoot() + std::string("/") + path;
  }

  static std::string getPathRelativeBinary(const std::string& path)
  {
    return "../../../" + path;
  }


};

// FILESYSTEM_H
#endif
