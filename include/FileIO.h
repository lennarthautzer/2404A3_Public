/*
* Lennart Hautzer
* 10 100 5235
* COMP2404 A3
* All rights reserved.
*/

#ifndef FileIOHeader
  #define FileIOHeader

  #include <iostream>
  #include <unordered_map>
  #include <string>
  #include <functional>
  #include <algorithm>
  #include <fstream>
  #include <memory>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include "ConditionalIncludes.h"

  class FileIO
  {
    public:
      FileIO();

      std::string makeUpperCase(std::string line) const;
      std::string makeLowerCase(std::string line) const;
      std::string makeTitleCase(std::string line) const;
      std::unique_ptr<std::ifstream> * openInputStream(std::string inputFileName);
      std::unique_ptr<std::ofstream> * openOutputStream(std::string outputFileName);
      void closeInputStream(std::string streamToClose);
      void closeOutputStream(std::string streamToClose);
      void resetInputStreamToFileStart(std::string streamToReset);
      void kill();
      void setWorkingDirs(std::string newRootDir);
      std::string findWorkingDir();

      void setFileName(std::string keyToSet, std::string fileNameToSet);

      std::string getFileName(std::string keyToLookUp);
      std::string getWorkingDir() const;
      std::string getDataDir() const;
      std::unordered_map<std::string, std::string> getAllFileNames();
      std::unique_ptr<std::ifstream> * getInputStream(std::string streamToGet);
      std::unique_ptr<std::ofstream> * getOutputStream(std::string streamToGet);
      std::unordered_map< std::string, std::unique_ptr<std::ifstream> > * getAllInputStreams();
      std::unordered_map< std::string, std::unique_ptr<std::ofstream> > * getAllOutputStreams();

    private:
      std::unordered_map<std::string, std::string> fileNames;
      std::unordered_map< std::string, std::unique_ptr<std::ifstream> > inputFileStreams;
      std::unordered_map< std::string, std::unique_ptr<std::ofstream> > outputFileStreams;
      std::string workingDir;
      std::string dataDir;
  };

#endif
