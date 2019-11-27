/*
* Lennart Hautzer
* 10 100 5235
* COMP2404 A3
* All rights reserved.
*/

#include "CommandLine.h"

//This object is global in order to facilitate communication between
// the SQL callback function and the CommandLine object. The nature of the
// callback function unfortunately requires this work-around. It is the only global.

std::stringstream interObjectCommunicationStream;

CommandLine::CommandLine() :
  runningStatus(true),
  isLoggingInput(false),
  isLoggingOutput(false),
  outputSQLToConsole(false)
{
  fileIO.setFileName("Help", "help Rev3.txt");
  fileIO.openInputStream( fileIO.getFileName("Help") );
  fileIO.setFileName("quickHelp", "quickHelp.txt");
  fileIO.openInputStream( fileIO.getFileName("quickHelp") );

  this->setDataBaseName("A3.db");
  this->openDataBase();

  std::cout << std::endl << std::endl <<
  "---------------------------------------------------------------------------------------------"
  << std::endl <<
  "|                           Welcome to the SighTunes Music Library!                         |"
  << std::endl <<
  "|   Use the .help or .quickhelp commands at any time to see a list of available commands.   |"
  << std::endl <<
  "---------------------------------------------------------------------------------------------"
  << std::endl << std::endl << std::endl << std::endl << std::endl;

  while( getRunningStatus() )
  {
    getUserInput();
    parseInstructionFromInput();
    executeParsedInstruction();
  }
}

CommandLine::~CommandLine()
{
  this->fileIO.closeInputStream( fileIO.getFileName("Help") );
  this->fileIO.closeInputStream( fileIO.getFileName("quickHelp") );
  this->fileIO.kill();
  this->closeDataBase();
}

void CommandLine::getUserInput()
{
  std::cout << ">. ";
  getline(std::cin, this->rawInput);
  if( this->getInputLoggingStatus() )
  {
    this->log.push_back(this->rawInput);
  }
}

void CommandLine::parseDotCommand()
{
  if(this->rawInput.substr( 0, strlen(".quit") ) == ".quit")
  {
    this->setRunningStatus(false);

    this->outputBuffer.clear();
    this->outputBuffer.str( std::string () );
    this->parsedInstruction = "";
    this->rawInput = "";
  }
  else if(this->rawInput.substr( 0, strlen(".endsWith") ) == ".endsWith")
  {
    std::string dataString = this->rawInput.substr( strlen(".endsWith") );
    std::vector<std::string> tmpVec = splitOnQuote(dataString);

    if( tmpVec[1].length() > tmpVec[0].length() )
    {
      this->outputBuffer << "False" << std::endl;
      return;
    }
    if( tmpVec[0].substr( tmpVec[0].length() - tmpVec[1].length() ) == tmpVec[1] )
    {
      this->outputBuffer << "True" << std::endl;
    }
    else
    {
      this->outputBuffer << "False" << std::endl;
    }
  }
  else if(this->rawInput.substr( 0, strlen(".startsWith") ) == ".startsWith")
  {
    std::string dataString = this->rawInput.substr( strlen(".startsWith") );
    std::vector<std::string> tmpVec = splitOnQuote(dataString);

    if( tmpVec[0].substr( 0, tmpVec[1].length() ) == tmpVec[1] )
    {
      this->outputBuffer << "True" << std::endl;
    }
    else
    {
      this->outputBuffer << "False" << std::endl;
    }
  }
  else if(this->rawInput.substr( 0, strlen(".trim") ) == ".trim")
  {
    std::string dataString = this->rawInput.substr( strlen(".trim") );

    stripChar(dataString, '\"');
    trimString(dataString);

    this->outputBuffer << dataString << std::endl;
  }
  else if(this->rawInput.substr( 0, strlen(".toTitleCase") ) == ".toTitleCase")
  {
    std::string dataString = this->rawInput.substr( strlen(".toTitleCase") );
    stripChar(dataString, '\"');

    this->outputBuffer << fileIO.makeTitleCase(dataString) << std::endl;
  }
  else if(this->rawInput.substr( 0, strlen(".help") ) == ".help")
  {
    std::unique_ptr<std::ifstream> * helpFileStream = fileIO.getInputStream(
      fileIO.getFileName("Help") );
    std::string line;
    long long counter = 0;

    while( getline(**helpFileStream, line) )
    {
      counter++;
      this->outputBuffer << std::setw(4) << std::left << counter << "| " << line << std::endl;
    }
    fileIO.resetInputStreamToFileStart( fileIO.getFileName("Help") );
  }
  else if(this->rawInput.substr( 0, strlen(".quickhelp") ) == ".quickhelp")
  {
    std::unique_ptr<std::ifstream> * quickHelpFileStream = fileIO.getInputStream(
      fileIO.getFileName("quickHelp") );
    std::string line;
    long long counter = 0;

    while( getline(**quickHelpFileStream, line) )
    {
      counter++;
      this->outputBuffer << std::setw(4) << std::left << counter << "| " << line << std::endl;
    }
    fileIO.resetInputStreamToFileStart( fileIO.getFileName("quickHelp") );
  }
  else if(this->rawInput.substr( 0, strlen(".read") ) == ".read")
  {
    stripChar(this->rawInput, ' ');
    stripChar(this->rawInput, '\"');

    fileIO.setFileName( "Input", this->rawInput.substr( strlen(".read") ) );

    std::unique_ptr<std::ifstream> * inputFileStream =
      fileIO.openInputStream( fileIO.getFileName("Input") );

    this->outputBuffer << this->rawInput << std::endl;

    while( getline(**inputFileStream, this->rawInput) )
    {
      this->outputBuffer << "Read: " << this->rawInput << std::endl;
      std::cout << this->outputBuffer.str();
      this->outputBuffer.clear();
      this->outputBuffer.str( std::string() );

      if(this->rawInput.find("//") != std::string::npos)
      {
        this->rawInput = this->rawInput.substr( 0, this->rawInput.find("//") );
      }
      if(this-> rawInput.size() == 0)
      {
        continue;
      }
      if( this->getInputLoggingStatus() )
      {
        this->log.push_back(this->rawInput);
      }
      parseInstructionFromInput();
      executeParsedInstruction();
    }
    fileIO.closeInputStream( fileIO.getFileName("Input") );
  }
  else if(this->rawInput.substr( 0, strlen(".log") ) == ".log")
  {
    stripChar(this->rawInput, ' ');
    stripChar(this->rawInput, '\"');
    if( this->rawInput.substr( strlen(".log") ) == "clear" )
    {
      this->log.clear();
      this->outputBuffer << "Log has been cleared." << std::endl;
    }
    else if( this->rawInput.substr( strlen(".log") ) == "start" )
    {
      this->setInputLoggingStatus(false);
      this->setOutputLoggingStatus(false);
      this->setInputLoggingStatus(true);
      this->outputBuffer << "Now logging ONLY input." << std::endl;
    }
    else if( this->rawInput.substr( strlen(".log") ) == "start_output" )
    {
      this->setInputLoggingStatus(false);
      this->setOutputLoggingStatus(false);
      this->setOutputLoggingStatus(true);
      this->outputBuffer << "Now logging ONLY output." << std::endl;
      if(!this->outputSQLToConsole)
      {
        this->outputBuffer << "SQL output is off. To log it, invoke >. .sql" << std::endl;
      }
    }
    else if( this->rawInput.substr( strlen(".log") ) == "start_both" )
    {
      this->setInputLoggingStatus(true);
      this->setOutputLoggingStatus(true);
      this->outputBuffer << "Now logging BOTH input AND output." << std::endl;
      if(!this->outputSQLToConsole)
      {
        this->outputBuffer << "SQL output is off. To log it, invoke >. .sql" << std::endl;
      }
    }
    else if( this->rawInput.substr( strlen(".log") ) == "stop" )
    {
      this->setInputLoggingStatus(false);
      this->setOutputLoggingStatus(false);
      this->outputBuffer << "Logging has been stopped for input AND output." << std::endl;
    }
    else if( this->rawInput.substr( strlen(".log"), strlen("save") ) == "save" )
    {
      fileIO.setFileName( "Output", this->rawInput.substr( strlen(".logsave") ) );
      std::unique_ptr<std::ofstream> * outputFileStream =
        fileIO.openOutputStream( fileIO.getFileName("Output") );

      for(std::string s : this->log)
      {
        **outputFileStream << s << std::endl;
      }

      fileIO.closeOutputStream( fileIO.getFileName("Output") );

      this->outputBuffer << "Save operation completed." << std::endl;
      this->outputBuffer << "File is at: " << this->fileIO.getDataDir() <<
        fileIO.getFileName("Output") << std::endl;
    }
    else if( this->rawInput.substr( strlen(".log") ) == "show" )
    {
      for(std::string s : this->log)
      {
        this->outputBuffer << s << std::endl;
      }
    }
  }
  else if(this->rawInput.substr( 0, strlen(".sql") ) == ".sql")
  {
    if(this->outputSQLToConsole)
    {
      this->outputSQLToConsole = false;
      this->outputBuffer << "SQL Output is now OFF" << std::endl;
    }
    else
    {
      this->outputSQLToConsole = true;
      this->outputBuffer << "SQL Output is now ON" << std::endl;
    }
  }
  if( this->getOutputLoggingStatus() )
  {
    this->log.push_back( this->outputBuffer.str() );
  }

  std::cout << this->outputBuffer.str();
  this->outputBuffer.clear();
  this->outputBuffer.str( std::string() );
  this->rawInput = "";
  this->parsedInstruction = "";
  this->parsedTags.clear();
}

void CommandLine::parseShowCommand()
{
  if( this->rawInput.substr( strlen("show "), strlen("recordings") ) == "recordings")
  {
    this->outputBuffer << "Recordings ( ID | Title | Artist | Year | Producer ): " << std::endl;

    std::string showRecordingsTable =
      "SELECT * FROM recordings ";

    this->doSQL(showRecordingsTable);
  }
  if( this->rawInput.substr( strlen("show "), strlen("songs") ) == "songs")
  {
    std::vector<std::string> const splitInput = splitString(this->rawInput, '-');

    parseTagsFromInput(splitInput);

    if(splitInput.size() > 1)
    {
      if( this->parsedTags.find("u") != this->parsedTags.end()
        && this->parsedTags.find("p") != this->parsedTags.end() )
      {
        std::vector<std::string> & tagVecU = this->parsedTags.find("u")->second;
        std::vector<std::string> & tagVecP = this->parsedTags.find("p")->second;

        this->outputBuffer << "Songs ( ID | Title | Composer ): " << std::endl;

        std::string showSongsTable =
          "SELECT * FROM songs WHERE sid IN "
          "(SELECT songID FROM tracks WHERE tid IN "
          "(SELECT track_id FROM playlist_tracks WHERE \"user_id\" = \"" + tagVecU[0] +
          "\" AND playlist_name = \"" + tagVecP[0] + "\"))";

        this->doSQL(showSongsTable);
      }
      else
      {
        this->outputBuffer << std::endl <<
          "Insufficient arguments to show Songs." << std::endl <<
          "Expected Format: show songs" << std::endl <<
          "Expected Format: show songs -u User_ID -p \"Playlist_Name\"" << std::endl << std::endl;
      }
    }
    else
    {
      this->outputBuffer << "Songs ( ID | Title | Composer ): " << std::endl;

      std::string showSongsTable =
        "SELECT * FROM songs ";

      this->doSQL(showSongsTable);
    }
  }
  if( this->rawInput.substr( strlen("show "), strlen("tracks") ) == "tracks")
  {
    this->outputBuffer << "Tracks ( ID | Album ID | Song ID | Track Number ): " << std::endl;

    std::string showTracksTable =
      "SELECT * FROM tracks ";

    this->doSQL(showTracksTable);
  }
  if( this->rawInput.substr( strlen("show "), strlen("users") ) == "users")
  {
    this->outputBuffer << "Users ( ID | Name ): " << std::endl;

    std::string showUsersTable =
      "SELECT * FROM users ";

    this->doSQL(showUsersTable);
  }
  if( this->rawInput.substr( strlen("show "), strlen("playlists") ) == "playlists")
  {
    std::vector<std::string> const splitInput = splitString(this->rawInput, '-');

    parseTagsFromInput(splitInput);

    if(this->parsedTags.find("u") == this->parsedTags.end() )
    {
      this->outputBuffer << std::endl <<
        "Insufficient arguments to show Playlists." << std::endl <<
        "Expected Format: show playlists -u User_ID " << std::endl << std::endl;
    }
    else
    {
      std::vector<std::string> & tagVecU = this->parsedTags.find("u")->second;

      this->outputBuffer << "Playlists ( User ID | Name ): " << std::endl;

      std::string showPlaylistsTable =
        "SELECT * FROM playlists WHERE \"user_id\" = \"" + tagVecU[0] + "\"";

      this->doSQL(showPlaylistsTable);
    }
  }
  if( this->rawInput.substr( strlen("show "), strlen("playlist_tracks") ) == "playlist_tracks")
  {
    std::vector<std::string> const splitInput = splitString(this->rawInput, '-');

    parseTagsFromInput(splitInput);

    if(this->parsedTags.find("u") == this->parsedTags.end() )
    {
      this->outputBuffer << std::endl <<
        "Insufficient arguments to show Playlist Tracks." << std::endl <<
        "Expected Format: show playlist_tracks -u User_ID " << std::endl << std::endl;
    }
    else
    {
      std::vector<std::string> & tagVecU = this->parsedTags.find("u")->second;

      this->outputBuffer << "Playlist Tracks ( User ID | Name | Track ID): " << std::endl;

      std::string showPlaylistTracksTable =
        "SELECT * FROM playlist_tracks WHERE \"user_id\" = \"" + tagVecU[0] + "\"";

      this->doSQL(showPlaylistTracksTable);
    }
  }

  if( this->getOutputLoggingStatus() )
  {
    this->log.push_back( this->outputBuffer.str() );
  }

  std::cout << this->outputBuffer.str();
  this->outputBuffer.clear();
  this->outputBuffer.str( std::string() );
  this->rawInput = "";
  this->parsedInstruction = "";
  this->parsedTags.clear();
}

void CommandLine::parseInstructionFromInput()
{
  if( this->rawInput.empty() )
  {
    return;
  }
  if( this->rawInput.at(0) == '.' )
  {
    parseDotCommand();
    return;
  }
  if( this->rawInput.substr(0, strlen("show") ) == "show" )
  {
    parseShowCommand();
    return;
  }
  std::vector<std::string> const splitInput = splitString(this->rawInput, '-');
  this->parsedInstruction = splitInput[0];

  parseTagsFromInput(splitInput);
}

void CommandLine::parseTagsFromInput(std::vector<std::string> const & in)
{
  for(size_t i = 1; i < in.size(); i++)
  {
    std::vector<std::string> tmpVec = splitString(in[i], ' ');
    int quoteFound = -1;

    for(int j = 0; j < ( (int) tmpVec.size() ); j++)
    {
      if( tmpVec[j].find('\"') != std::string::npos)
      {
        if( tmpVec[j].substr( tmpVec[j].find('\"') ).length() > 1
          && tmpVec[j].substr( tmpVec[j].find('\"') ).find('\"') !=
          std::string::npos)
        {
          quoteFound = -1;
        }
        if(quoteFound != -1)
        {
          for(int k = quoteFound + 1; k <= j; k++)
          {
            tmpVec[quoteFound] += (" " + tmpVec[k]);
            tmpVec[k] = "";
          }

          tmpVec[quoteFound] = this->fileIO.makeTitleCase( tmpVec[quoteFound] );
          quoteFound = -1;
        }
        else
        {
          quoteFound = j;
        }
      }
    }

    for(std::string s : tmpVec)
    {
      if(s.find_first_not_of(' ') != std::string::npos)
      {
        continue;
      }
      tmpVec.erase( std::remove(tmpVec.begin(), tmpVec.end(), s), tmpVec.end() );
    }

    if(tmpVec.size() > 1)
    {
      std::vector<std::string> tmpVec2( tmpVec.begin() + 1, tmpVec.end() );

      for(std::string & s : tmpVec2)
      {
        stripChar(s, '\"');
      }

      this->parsedTags.insert( { tmpVec[0], tmpVec2 } );
    }
  }
}

void CommandLine::executeParsedInstruction()
{
  if( this->parsedInstruction.substr( 0, strlen("add") ) == "add")
  {
    if( this->parsedTags.find("r") != this->parsedTags.end() )
    {
      std::vector<std::string> & tagVec = this->parsedTags.find("r")->second;

      if(tagVec.size() < 5)
      {
        this->outputBuffer << std::endl <<
          "Insufficient arguments to add Recording." << std::endl <<
          "Expected Format: add -r Recording_ID " <<
          "\"Title\" \"Artist\" \"Producer\" Year" << std::endl << std::endl;
      }
      else
      {
        std::string addRecordingToRecordingsTable =
          "INSERT OR IGNORE INTO recordings (id, title, artist, producer, year) "
          "VALUES (\"" + tagVec[0] + "\", \"" +
          fileIO.makeTitleCase( tagVec[1] ) + "\", \"" +
          fileIO.makeTitleCase( tagVec[2] ) + "\", \"" +
          fileIO.makeTitleCase( tagVec[3] ) + "\", \""  + tagVec[4] +
        "\")";

        this->doSQL(addRecordingToRecordingsTable);
      }
    }
    if( this->parsedTags.find("s") != this->parsedTags.end() )
    {
      std::vector<std::string> & tagVec = this->parsedTags.find("s")->second;

      if(tagVec.size() < 3)
      {
        this->outputBuffer << std::endl <<
          "Insufficient arguments to add Song." << std::endl <<
          "Expected Format: add -s Song_ID " <<
          "\"Title\" \"Composer\"" << std::endl << std::endl;
      }
      else
      {
        std::string addSongToSongsTable =
          "INSERT OR IGNORE INTO songs (sid, title, composer) "
          "VALUES (\"" + tagVec[0] + "\", \"" +
          fileIO.makeTitleCase( tagVec[1] ) + "\", \"" +
          fileIO.makeTitleCase( tagVec[2] ) +
        "\")";

        this->doSQL(addSongToSongsTable);
      }
    }
    if( this->parsedTags.find("t") != this->parsedTags.end() )
    {
      std::vector<std::string> & tagVec = this->parsedTags.find("t")->second;

      if(tagVec.size() < 4)
      {
        this->outputBuffer << std::endl <<
          "Insufficient arguments to add Track." << std::endl <<
          "Expected Format: add -t Track_ID " <<
          "Album_ID Song_ID Track_Number" << std::endl << std::endl;
      }
      else
      {
        std::string addTrackToTracksTable =
          "INSERT OR IGNORE INTO tracks (tid, albumID, songID, track_number) "
          "VALUES (\"" + tagVec[0] + "\", \"" + tagVec[1] +
          "\", \"" + tagVec[2] + "\", \"" + tagVec[3] + "\")";

        this->doSQL(addTrackToTracksTable);
      }
    }
    if( this->parsedTags.find("u") != this->parsedTags.end() )
    {
      std::vector<std::string> & tagVec = this->parsedTags.find("u")->second;

      if(tagVec.size() < 2)
      {
        this->outputBuffer << std::endl <<
          "Insufficient arguments to add User." << std::endl <<
          "Expected Format: add -u User_ID " << "\"Name\"" << std::endl << std::endl;
      }
      else
      {
        std::string addUserToUsersTable =
          "INSERT OR IGNORE INTO users (user_id, name) "
          "VALUES (\"" + tagVec[0] + "\", \"" + tagVec[1] + "\")";

        this->doSQL(addUserToUsersTable);
      }
    }
    if( this->parsedTags.find("p") != this->parsedTags.end() )
    {
      std::vector<std::string> & tagVec = this->parsedTags.find("p")->second;

      if(tagVec.size() < 2)
      {
        this->outputBuffer << std::endl <<
          "Insufficient arguments to add Playlist." << std::endl <<
          "Expected Format: add -p User_ID " << "\"Playlist_Name\"" << std::endl << std::endl;
      }
      else
      {
        std::string addPlaylistToPlaylistsTable =
        "INSERT OR IGNORE INTO playlists (user_id, playlist_name) "
        "VALUES (\"" + tagVec[0] + "\", \"" + tagVec[1] + "\")";

        this->doSQL(addPlaylistToPlaylistsTable);
      }
    }
    if( this->parsedTags.find("l") != this->parsedTags.end() )
    {
      std::vector<std::string> & tagVec = this->parsedTags.find("l")->second;

      if(tagVec.size() < 3)
      {
        this->outputBuffer << std::endl <<
          "Insufficient arguments to add Track to Playlist." << std::endl <<
          "Expected Format: add -l User_ID " << "\"Playlist_Name\"" <<
          " Track_ID" << std::endl << std::endl;
      }
      else
      {
        std::string addTrackToPlaylistTracksTable =
          "INSERT OR IGNORE INTO playlist_tracks (user_id, playlist_name, track_id) "
          "VALUES (\"" + tagVec[0] + "\", \"" + tagVec[1] + "\", \"" + tagVec[2] + "\")";

        this->doSQL(addTrackToPlaylistTracksTable);
      }
    }
  }
  if( this->parsedInstruction.substr( 0, strlen("delete") ) == "delete")
  {
    if( this->parsedTags.find("r") != this->parsedTags.end() )
    {
      std::vector<std::string> & tagVec = this->parsedTags.find("r")->second;

      if(tagVec.size() < 1)
      {
        this->outputBuffer << std::endl <<
          "Insufficient arguments to delete Recording." << std::endl <<
          "Expected Format: delete -r Recording_ID " << std::endl << std::endl;
      }
      else
      {
        std::string deleteDereferencedTrackFromTracksTable =
          "DELETE FROM tracks "
          "WHERE \"albumID\" = \"" + tagVec[0] + "\"";

        this->doSQL(deleteDereferencedTrackFromTracksTable);

        std::string deleteRecordingFromRecordingsTable =
          "DELETE FROM recordings "
          "WHERE \"id\" = \"" + tagVec[0] + "\"";

        this->doSQL(deleteRecordingFromRecordingsTable);
      }
    }
    else if( this->parsedTags.find("s") != this->parsedTags.end() )
    {
      std::vector<std::string> & tagVecS = this->parsedTags.find("s")->second;

      if(tagVecS.size() < 1)
      {
        this->outputBuffer << std::endl <<
          "Insufficient arguments to delete Song." << std::endl <<
          "Expected Format: delete -s Song_ID " << std::endl <<
          "Insufficient arguments to delete Track." << std::endl <<
          "Expected Format: delete -Global -s Track_ID -p \"Playlist_Name\"" <<
          " -u \"User_Name\"" << std::endl <<
          "Expected Format: delete -s Track_ID -p \"Playlist_Name\"" <<
          " -u \"User_Name\"" << std::endl <<std::endl;
      }
      else
      {
        if( this->parsedTags.find("p") != this->parsedTags.end()
          && this->parsedTags.find("u") != this->parsedTags.end() )
        {
          if( this->parsedTags.find("Global") != this->parsedTags.end() )
          {
            std::string deleteDereferencedTrackFromPlaylistTracksTable =
              "DELETE FROM playlist_tracks "
              "WHERE \"track_id\" = \"" + tagVecS[0] + "\"";

            this->doSQL(deleteDereferencedTrackFromPlaylistTracksTable);

            std::string deleteTrackFromTracksTable =
              "DELETE FROM tracks "
              "WHERE \"tid\" = \"" + tagVecS[0] + "\"";

            this->doSQL(deleteTrackFromTracksTable);
          }
          else
          {
            std::vector<std::string> & tagVecU = this->parsedTags.find("u")->second;
            std::vector<std::string> & tagVecP = this->parsedTags.find("p")->second;

            std::string deleteTrackFromPlaylistTracksTable =
              "DELETE FROM playlist_tracks "
              "WHERE \"track_id\" = \"" + tagVecS[0] +
              "\" AND \"user_id\" = \"" + tagVecU[0] +
              "\" AND playlist_name = \"" + tagVecP[0] + "\"";

            this->doSQL(deleteTrackFromPlaylistTracksTable);
          }
        }
        else
        {
          std::string deleteDereferencedTrackFromPlaylistTracksTable =
            "DELETE FROM playlist_tracks "
            "WHERE \"track_id\" IN "
            "(SELECT \"tid\" FROM tracks WHERE \"songID\" = \"" + tagVecS[0] + "\" )";

          this->doSQL(deleteDereferencedTrackFromPlaylistTracksTable);

          std::string deleteDereferencedSongFromTracksTable =
            "DELETE FROM tracks "
            "WHERE \"songID\" = \"" + tagVecS[0] + "\"";

          this->doSQL(deleteDereferencedSongFromTracksTable);

          std::string deleteSongFromSongsTable =
            "DELETE FROM songs "
            "WHERE \"sid\" = \"" + tagVecS[0] + "\"";

          this->doSQL(deleteSongFromSongsTable);
        }
      }
    }
    else if( this->parsedTags.find("t") != this->parsedTags.end() )
    {
      std::vector<std::string> & tagVecT = this->parsedTags.find("t")->second;

      if(tagVecT.size() < 1)
      {
        this->outputBuffer << std::endl <<
          "Insufficient arguments to delete Track." << std::endl <<
          "Expected Format: delete -t Track_ID" << std::endl <<
          "Expected Format: delete -Global -s Track_ID -p \"Playlist_Name\"" <<
          " -u \"User_Name\"" << std::endl <<
          "Expected Format: delete -s Track_ID -p \"Playlist_Name\"" <<
          " -u \"User_Name\"" << std::endl <<std::endl;
      }
      else
      {
        if( this->parsedTags.find("p") != this->parsedTags.end()
          && this->parsedTags.find("u") != this->parsedTags.end() )
        {
          if( this->parsedTags.find("Global") != this->parsedTags.end() )
          {
            std::string deleteDereferencedTrackFromPlaylistTracksTable =
              "DELETE FROM playlist_tracks "
              "WHERE \"track_id\" = \"" + tagVecT[0] + "\"";

            this->doSQL(deleteDereferencedTrackFromPlaylistTracksTable);

            std::string deleteTrackFromTracksTable =
              "DELETE FROM tracks "
              "WHERE \"tid\" = \"" + tagVecT[0] + "\"";

            this->doSQL(deleteTrackFromTracksTable);
          }
          else
          {
            std::vector<std::string> & tagVecU = this->parsedTags.find("u")->second;
            std::vector<std::string> & tagVecP = this->parsedTags.find("p")->second;

            std::string deleteDereferencedTrackFromPlaylistTracksTable =
              "DELETE FROM playlist_tracks "
              "WHERE \"track_id\" = \"" + tagVecT[0] +
              "\" AND \"user_id\" = \"" + tagVecU[0] +
              "\" AND playlist_name = \"" + tagVecP[0] + "\"";

            this->doSQL(deleteDereferencedTrackFromPlaylistTracksTable);
          }
        }
        else
        {
          std::string deleteTrackFromPlaylistTracksTable =
            "DELETE FROM playlist_tracks "
            "WHERE \"track_id\" = \"" + tagVecT[0] + "\"";

          this->doSQL(deleteTrackFromPlaylistTracksTable);

          std::string deleteTrackFromTracksTable =
            "DELETE FROM tracks "
            "WHERE \"tid\" = \"" + tagVecT[0] + "\"";

          this->doSQL(deleteTrackFromTracksTable);
        }
      }
    }
    else if( this->parsedTags.find("u") != this->parsedTags.end()
      && this->parsedTags.find("p") == this->parsedTags.end())
    {
      std::vector<std::string> & tagVecU = this->parsedTags.find("u")->second;

      std::string deleteUserFromPlaylistsTable =
        "DELETE FROM playlists "
        "WHERE \"user_id\" = \"" + tagVecU[0] + "\"";

      this->doSQL(deleteUserFromPlaylistsTable);

      std::string deleteUserFromPlaylistTracksTable =
        "DELETE FROM playlist_tracks "
        "WHERE \"user_id\" = \"" + tagVecU[0] + "\"";

      this->doSQL(deleteUserFromPlaylistTracksTable);

      std::string deleteUserFromUsersTable =
        "DELETE FROM users "
        "WHERE \"user_id\" = \"" + tagVecU[0] + "\"";

      this->doSQL(deleteUserFromUsersTable);
    }
    else if( this->parsedTags.find("p") != this->parsedTags.end()
      && this->parsedTags.find("u") != this->parsedTags.end() )
    {
      std::vector<std::string> & tagVecU = this->parsedTags.find("u")->second;
      std::vector<std::string> & tagVecP = this->parsedTags.find("p")->second;

      std::string deletePlaylistFromPlaylistsTable =
        "DELETE FROM playlists "
        "WHERE playlist_name = \"" + tagVecP[0] + "\"" +
        "AND \"user_id\" = \"" + tagVecU[0] + "\"";

      this->doSQL(deletePlaylistFromPlaylistsTable);

      std::string deletePlaylistFromPlaylistTracksTable =
        "DELETE FROM playlist_tracks "
        "WHERE playlist_name = \"" + tagVecP[0] + "\"" +
        "AND \"user_id\" = \"" + tagVecU[0] + "\"";

      this->doSQL(deletePlaylistFromPlaylistTracksTable);
    }
  }

  if( this->getOutputLoggingStatus() )
  {
    this->log.push_back( this->outputBuffer.str() );
  }

  std::cout << this->outputBuffer.str();
  this->outputBuffer.clear();
  this->outputBuffer.str( std::string() );
  this->rawInput = "";
  this->parsedInstruction = "";
  this->parsedTags.clear();
}

void CommandLine::printInstruction()
{
  this->outputBuffer << std::endl <<
    "Application Running Status (0 or 1): " << this->runningStatus << std::endl;
  this->outputBuffer << "Raw Input Given: " << this->rawInput << std::endl;
  this->outputBuffer << "Parsed Instruction: " << this->parsedInstruction << std::endl;

  std::unordered_map< std::string, std::vector<std::string> >::iterator
    tagIterator = this->parsedTags.begin();

  while( tagIterator != this->parsedTags.end() )
  {
    this->outputBuffer << "Tag \"" << tagIterator->first << "\" : ";

    for(std::string s : tagIterator->second)
    {
      this->outputBuffer << s << " | ";
    }

    this->outputBuffer << std::endl;
    tagIterator++;
  }
  this->outputBuffer << std::endl;
}

std::vector<std::string> CommandLine::splitString(std::string str,
  char const charToSplitOn)
{
  size_t i = 0;
  std::vector<std::string> ret;

  while ( ( i = str.find(charToSplitOn) ) != std::string::npos )
  {
    std::string subString = str.substr(0, i);
    ret.push_back(subString);
    str.erase(0, i + 1);
  }
  ret.push_back(str);

  return ret;
}

std::vector<std::string> CommandLine::splitOnQuote(std::string str)
{
  size_t i = 0;
  std::vector<std::string> ret;
  int secondQuote = false;

  while ( ( i = str.find("\"") ) != std::string::npos )
  {
    if(secondQuote)
    {
      std::string subString = str.substr(0, i);
      ret.push_back(subString);
      str.erase(0, i + 1);
      secondQuote = false;
    }
    else
    {
      str.erase( 0, i + strlen("\"") );
      secondQuote = true;
    }
  }
  ret.push_back(str);

  return ret;
}

void CommandLine::stripChar(std::string & str, char const charToRemove)
{
  str.erase( std::remove(str.begin(), str.end(), charToRemove), str.end() );
}

void CommandLine::trimString(std::string &input)
{
  std::string tmpStr;

  std::unique_copy ( input.begin(), input.end(),
    std::back_insert_iterator<std::string> (tmpStr), [] (char i, char j)
  {
    return std::isspace(i) && std::isspace(j);
  } );

  input = tmpStr;
}

std::string CommandLine::centerString(std::string const& str, int targetSize)
{
  assert( targetSize >= 0 );

  int padding = targetSize - ( (int) ( str.size() ) );
  if(padding % 2 != 0)
  {
    padding++;
  }

  return padding > 0 ? (std::string(padding / 2, ' ') + str +
    std::string(padding / 2, ' ') ) : str;
}

void CommandLine::openDataBase()
{
  if( sqlite3_open( (fileIO.getDataDir() + this->getDataBaseName() ).c_str(),
    &this->dataBase ) )
  {
    std::cout << "ERROR: Could Not Open Database: " << sqlite3_errmsg(this->dataBase) << std::endl;
    this->setRunningStatus(false);
    return;
  }
  else
  {
    std::cout << "SUCCESSFULLY OPENED DATABASE: " <<
      fileIO.getDataDir() + this->getDataBaseName() << std::endl;
    this->initializeDataBase();
  }
}

void CommandLine::initializeDataBase()
{
  std::string createSongsTable =
    "CREATE TABLE IF NOT EXISTS songs ("
      "sid INTEGER PRIMARY KEY,"
      "title TEXT NOT NULL,"
      "composer TEXT NOT NULL"
    ");";

  this->doSQL(createSongsTable);

  std::string createRecordingsTable =
    "CREATE TABLE IF NOT EXISTS recordings ("
      "id INTEGER PRIMARY KEY NOT NULL,"
      "title TEXT NOT NULL,"
      "artist TEXT NOT NULL,"
      "producer TEXT DEFAULT NULL,"
      "year INTEGER"
    ");";

  this->doSQL(createRecordingsTable);

  std::string createTracksTable =
    "CREATE TABLE IF NOT EXISTS tracks ("
      "tid INTEGER PRIMARY KEY NOT NULL,"
      "albumID INTEGER NOT NULL,"
      "songID INTEGER NOT NULL,"
      "track_number INTEGER NOT NULL,"
      "FOREIGN KEY(albumID) REFERENCES recordings(id),"
      "FOREIGN KEY(songID) REFERENCES songs(sid)"
    ");";

  this->doSQL(createTracksTable);

  std::string createUsersTable =
    "CREATE TABLE IF NOT EXISTS users ("
      "user_id TEXT PRIMARY KEY NOT NULL,"
      "name TEXT NOT NULL"
    ");";

  this->doSQL(createUsersTable);

  std::string createPlaylistsTable =
    "CREATE TABLE IF NOT EXISTS playlists ("
      "user_id TEXT NOT NULL,"
      "playlist_name TEXT NOT NULL,"
      "FOREIGN KEY(user_id) REFERENCES users(user_id)"
      "UNIQUE(user_id, playlist_name)"
    ");";

  this->doSQL(createPlaylistsTable);

  std::string createPlaylistTracksTable =
    "CREATE TABLE IF NOT EXISTS playlist_tracks ("
      "user_id TEXT NOT NULL,"
      "playlist_name TEXT NOT NULL,"
      "track_id INTEGER NOT NULL,"
      "FOREIGN KEY(user_id) REFERENCES users(user_id),"
      "FOREIGN KEY(playlist_name) REFERENCES playlists(playlist_name),"
      "FOREIGN KEY(track_id) REFERENCES tracks(tid),"
      "UNIQUE(user_id, playlist_name, track_id)"
    ");";

  this->doSQL(createPlaylistTracksTable);
}

void CommandLine::closeDataBase()
{
  sqlite3_close(this->dataBase);
}

void CommandLine::doSQL(std::string const & in)
{
  if(this->outputSQLToConsole)
  {
    this->outputBuffer << std::endl << "ATTEMPTING SQL: " << in << std::endl;
    if( this->getOutputLoggingStatus() )
    {
      this->log.push_back( this->outputBuffer.str() );
    }
  }

  char * errorMessage;
  const char * SQLCommandToDo = (const char *) in.c_str();

  if( sqlite3_exec(this->dataBase, SQLCommandToDo, &(SQLCallBack),
    (void *) ( std::string("Callback Reached").c_str() ), &errorMessage)
    != SQLITE_OK)
  {
    this->outputBuffer << "ERROR EXECUTING SQL: " << errorMessage << std::endl;
    if( this->getOutputLoggingStatus() )
    {
      this->log.push_back( this->outputBuffer.str() );
    }
    sqlite3_free(errorMessage);
  }
  else
  {
    std::string tmpString = "";
    char delimiter = '@';
    int didOutput = false;

    while( std::getline(interObjectCommunicationStream, tmpString, delimiter) )
    {
      if(!didOutput)
      {
        didOutput = true;
      }

      std::string tmpString2 = tmpString;
      stripChar(tmpString2, ' ');

      if(tmpString2 == "")
      {
        this->outputBuffer << std::endl;
        continue;
      }
      tmpString = this->centerString(tmpString, 30);
      this->outputBuffer << std::setw(32) << tmpString;
    }
    this->outputBuffer << std::endl;

    interObjectCommunicationStream.clear();
    interObjectCommunicationStream.str( std::string() );

    if(this->outputSQLToConsole)
    {
      this->outputBuffer << "SQL QUERY SUCCEEDED" << std::endl;
      if( this->getOutputLoggingStatus() )
      {
        this->log.push_back( this->outputBuffer.str() );
      }
    }
  }
  if( this->getOutputLoggingStatus() )
  {
    this->log.push_back( this->outputBuffer.str() );
  }
  std::cout << this->outputBuffer.str();
  this->outputBuffer.clear();
  this->outputBuffer.str( std::string() );
}

int SQLCallBack(void * passedData, int numberOfColumns,
  char ** valuesInRow, char ** columnNames)
{
  for(int i = 0; i < numberOfColumns; i++)
  {
    interObjectCommunicationStream << valuesInRow[i] << "@";
  }
  interObjectCommunicationStream << "@";

  return 0;
}

void CommandLine::setRunningStatus(int const & in)
{ this->runningStatus = in; }

void CommandLine::setInputLoggingStatus(int const & in)
{ this->isLoggingInput = in; }

void CommandLine::setOutputLoggingStatus(int const & in)
{ this->isLoggingOutput = in; }

void CommandLine::setDataBaseName(std::string const & in)
{ this->dataBaseName = in; }

std::string CommandLine::getDataBaseName()
{ return this->dataBaseName; }

int const CommandLine::getRunningStatus() const
{ return this->runningStatus; }

int const CommandLine::getInputLoggingStatus() const
{ return this->isLoggingInput; }

int const CommandLine::getOutputLoggingStatus() const
{ return this->isLoggingOutput; }
