/*
* Lennart Hautzer
* 10 100 5235
* COMP2404 A3
* All rights reserved.
*/

#include "FileIO.h"

FileIO::FileIO()
{
  setWorkingDirs( findWorkingDir() );
}

std::string FileIO::findWorkingDir()
{
  char tmpBuffer[FILENAME_MAX];
  getCurrentDir(tmpBuffer, FILENAME_MAX);
  std::string ret(tmpBuffer);

  return ret;
}

void FileIO::setWorkingDirs(std::string newRootDir)
{
  struct stat relativeDirectoryCheck;
  std::string parentDir;
  std::string pathToCheck;

  workingDir = newRootDir;

  #ifdef WINDOWS

    pathToCheck = workingDir + "\\data";

    if(stat(pathToCheck.c_str(), &relativeDirectoryCheck) == 0
      && relativeDirectoryCheck.st_mode & S_IFDIR)
    {
      dataDir = workingDir + "\\data\\";
      return;
    }
    else
    {
      parentDir = workingDir.substr(0, workingDir.find_last_of("/"));
      pathToCheck = parentDir + "/data";

      if(stat(pathToCheck.c_str(), &relativeDirectoryCheck) == 0
        && relativeDirectoryCheck.st_mode & S_IFDIR)
      {
        dataDir = parentDir + "/data/";
        return;
      }
      else
      {
        perror ("Error: ");
      }
    }

  #else

    pathToCheck = workingDir + "/data";
    stat(pathToCheck.c_str(), &relativeDirectoryCheck);

    if(relativeDirectoryCheck.st_mode & S_IFDIR)
    {
      dataDir = workingDir + "/data/";
      return;
    }
    else
    {
      parentDir = workingDir.substr(0, workingDir.find_last_of("/"));
      pathToCheck = parentDir + "/data";
      stat(pathToCheck.c_str(), &relativeDirectoryCheck);

      if(relativeDirectoryCheck.st_mode & S_IFDIR)
      {
        dataDir = parentDir + "/data/";
        return;
      }
      else
      {
        perror ("Error: ");
      }
    }

  #endif

}

std::string FileIO::makeUpperCase(std::string line) const
{
  std::transform( line.begin(), line.end(), line.begin(),
    std::function<int(int)>(::toupper) );

  return line;
}

std::string FileIO::makeLowerCase(std::string line) const
{
  std::transform( line.begin(), line.end(), line.begin(),
    std::function<int(int)>(::tolower) );

  return line;
}

std::string FileIO::makeTitleCase(std::string line) const
{
  std::string retLine = "";

  if(line.substr(0, 1).compare(" ") == 0)
  {
    for(size_t i = 0; i < line.length() - 1; i++)
    {
      line.replace(i, 1, line.substr(i + 1, 1) );
    }
    line = line.substr(0, line.length() - 1);
  }

  line.replace(0, 1, makeUpperCase( line.substr(0, 1) ) );

  for(size_t i = 1; i <= line.length(); i++)
  {
    if(line.substr(i - 1, 1).compare(" ") == 0
      || line.substr(i - 1, 1).compare("\"") == 0)
    {
      line.replace(i, 1, makeUpperCase( line.substr(i, 1) ) );
    }
    else
    {
      line.replace(i, 1, makeLowerCase( line.substr(i, 1) ) );
    }
  }

  if(line.substr(0, 4).compare("The ") == 0)
  {
    retLine += line.substr(4) + ", The ";

    return retLine;
  }

  return line;
}

std::unique_ptr<std::ifstream> * FileIO::openInputStream(std::string inputFileName)
{
  inputFileStreams[inputFileName] = std::unique_ptr<std::ifstream>
    ( new std::ifstream(getDataDir() + inputFileName, std::ifstream::in) );

  if(!inputFileStreams[inputFileName])
  {
    std::cout << "Error: Could not open file '" <<
      inputFileName << "'." << std::endl;

    return(NULL);
  }
  return getInputStream(inputFileName);
}

std::unique_ptr<std::ofstream> * FileIO::openOutputStream(std::string outputFileName)
{
  outputFileStreams[outputFileName] = std::unique_ptr<std::ofstream>
    ( new std::ofstream(getDataDir() + outputFileName, std::ofstream::out) );

  if(!outputFileStreams[outputFileName])
  {
    std::cout << "Error: Could not open file '" <<
      outputFileName << "'." << std::endl;

    return(NULL);
  }
  return getOutputStream(outputFileName);
}

void FileIO::closeInputStream(std::string streamToClose)
{
  std::unique_ptr<std::ifstream> * inputFileStream = getInputStream(streamToClose);
  std::unordered_map< std::string, std::unique_ptr<std::ifstream> > * inputFileStreams = getAllInputStreams();

  (*inputFileStream)->close();
  inputFileStreams->erase(streamToClose);

  return;
}

void FileIO::closeOutputStream(std::string streamToClose)
{
  std::unique_ptr<std::ofstream> * outputFileStream = getOutputStream(streamToClose);
  std::unordered_map< std::string, std::unique_ptr<std::ofstream> > * outputFileStreams = getAllOutputStreams();

  (*outputFileStream)->close();
  outputFileStreams->erase(streamToClose);

  return;
}

void FileIO::resetInputStreamToFileStart(std::string streamToReset)
{
  std::unique_ptr<std::ifstream> * inputFileStream = getInputStream(streamToReset);

  (*inputFileStream)->clear();
  (*inputFileStream)->seekg(0, std::ios::beg);
}

void FileIO::kill()
{
  std::unordered_map< std::string, std::unique_ptr<std::ifstream> >::iterator inputStreamIterator;
  std::unordered_map< std::string, std::unique_ptr<std::ofstream> >::iterator outputStreamIterator;
  std::unordered_map< std::string, std::unique_ptr<std::ifstream> > * inputFileStreams;
  std::unordered_map< std::string, std::unique_ptr<std::ofstream> > * outputFileStreams;

  inputFileStreams = getAllInputStreams();
  outputFileStreams = getAllOutputStreams();

  inputStreamIterator = inputFileStreams->begin();

  while(inputStreamIterator != inputFileStreams->end())
  {
    inputStreamIterator->second->close();
    inputStreamIterator++;
  }

  outputStreamIterator = outputFileStreams->begin();

  while(outputStreamIterator != outputFileStreams->end())
  {
    outputStreamIterator->second->close();
    outputStreamIterator++;
  }
}

void FileIO::setFileName(std::string keyToSet, std::string nameToSet)
  { fileNames[keyToSet] = nameToSet; }

std::string FileIO::getFileName(std::string keyToLookUp)
  { return fileNames.find(keyToLookUp)->second; }

std::unordered_map<std::string, std::string> FileIO::getAllFileNames()
  { return fileNames; }

std::unique_ptr<std::ifstream> * FileIO::getInputStream(std::string streamToGet)
{
  std::unordered_map< std::string, std::unique_ptr<std::ifstream> > * inputFileStreams = getAllInputStreams();
  return &( (*inputFileStreams).find(streamToGet)->second );
}

std::unique_ptr<std::ofstream> * FileIO::getOutputStream(std::string streamToGet)
{
  std::unordered_map< std::string, std::unique_ptr<std::ofstream> > * outputFileStreams = getAllOutputStreams();
  return &( (*outputFileStreams).find(streamToGet)->second );
}

std::unordered_map<std::string, std::unique_ptr<std::ifstream>> * FileIO::getAllInputStreams()
{ return &inputFileStreams; }

std::unordered_map<std::string, std::unique_ptr<std::ofstream>> * FileIO::getAllOutputStreams()
{ return &outputFileStreams; }

std::string FileIO::getWorkingDir() const
{ return workingDir; }

std::string FileIO::getDataDir() const
{ return dataDir; }
