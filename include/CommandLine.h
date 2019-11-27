/*
* Lennart Hautzer
* 10 100 5235
* COMP2404 A3
* All rights reserved.
*/

#ifndef CommandLineHeader
  #define CommandLineHeader

  #include <iostream>
  #include <iomanip>
  #include <sstream>
  #include <string>
  #include <cstring>
  #include <vector>
  #include <unordered_map>
  #include <algorithm>
  #include <cassert>
  #include "FileIO.h"
  #include "sqlite3.h"

  class CommandLine
  {
    public:
      CommandLine();

      ~CommandLine();

      void openDataBase();
      void closeDataBase();
      void initializeDataBase();
      void doSQL(std::string const & in);
      void getUserInput();
      void parseDotCommand();
      void parseShowCommand();
      void parseInstructionFromInput();
      void parseTagsFromInput(std::vector<std::string> const & in);
      void executeParsedInstruction();
      void printInstruction();
      std::vector<std::string> splitString(std::string str,
        char const charToSplitOn);
      std::vector<std::string> splitOnQuote(std::string str);
      void stripChar(std::string & str, char const charToRemove);
      void trimString(std::string &input);
      std::string centerString(std::string const& str, int targetSize);

      int const getRunningStatus() const;
      int const getInputLoggingStatus() const;
      int const getOutputLoggingStatus() const;
      std::string getDataBaseName();

      void setDataBaseName(std::string const & in);
      void setRunningStatus(int const & in);
      void setInputLoggingStatus(int const & in);
      void setOutputLoggingStatus(int const & in);

      friend int SQLCallBack(void * passedData, int numberOfColumns,
        char ** valuesInRow, char ** columnNames);

    private:
      int runningStatus;
      int isLoggingInput;
      int isLoggingOutput;
      int outputSQLToConsole;
      std::string dataBaseName;
      std::string rawInput;
      std::string parsedInstruction;
      std::unordered_map< std::string, std::vector<std::string> > parsedTags;

      std::stringstream outputBuffer;
      std::vector<std::string> log;

      FileIO fileIO;
      sqlite3 * dataBase;
  };

  int SQLCallBack(void * passedData, int numberOfColumns,
    char ** valuesInRow, char ** columnNames);

#endif
