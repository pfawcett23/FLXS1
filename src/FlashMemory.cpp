#include <Arduino.h>
#include "FlashMemory.h"
#define FILE_EXISTS 0
#define SAVEFILE_DOES_NOT_EXIST 1
#define READ_JSON_ERROR 2
/*
¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING!
This file contains code that enables saving and loading of patterns. Changing this file could result in an inability to read existing save files.
¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING!
*/
FlashMemory saveFile;
FlashMemory::FlashMemory(){};

void FlashMemory::initialize(Sequencer (*sequenceArray)[4]){
  Serial.println("Initializing Flash Memory");
  this->sequenceArray = sequenceArray;

  if (!SD.begin(SD_CS_PIN)){
    Serial.println("SD Card initialization failed!");
    return;
  }
  // THIS IS THE LINE THAT DELETES THE DATAFILE EVERY TIME!
   //deleteSaveFile();
  if (SD.exists("data.txt")) {
    Serial.println("data.txt exists.");
  } else {
    Serial.println("data.txt does not exist, creating file...");
    saveData = SD.open("data.txt", FILE_WRITE);
    saveData.close();
  }

  Serial.println("SD Card and save file initialization complete.");

  loadPattern(0, 0b1111);

  Serial.println("Flash Memory Initialization Complete");
}

void FlashMemory::changePattern(uint8_t pattern, uint8_t channelSelector, boolean saveFirst, boolean instant){
	//Serial.println("currentPattern: " + String(currentPattern) + "\tsequenceCount: " + String(sequenceCount));
	if(saveFirst){
    for(int i=0; i < sequenceCount; i++){
  		//saveChannelPattern(i);
      saveSequenceJSON(i, currentPattern);
    }
    Serial.println("=*-.-*= Pattern " + String(currentPattern) + " saved. =*-.-*= ");
	}

  if (instant || !playing) {
    Serial.println("Changing pattern instantly: " + String(pattern) + " instant: " + String(instant) + " playing: " + String(playing) );
    loadPattern(pattern, channelSelector);
  } else {
    queuePattern = pattern;
    Serial.println("Queueing pattern: " + String(pattern));
  }
}

void FlashMemory::deleteSaveFile(){
  for(int i=0; i<16; i++){
    for(int n=0; n<sequenceCount; n++){
      sequenceArray[n]->initNewSequence(i, n);
    }

  }
  loadPattern(0, 0b1111);
}

//void FlashMemory::saveSequenceJSON(Sequencer& sequence, char *pattern ){
void FlashMemory::saveSequenceJSON(uint8_t channel, uint8_t pattern){
  //http://stackoverflow.com/questions/15179996/how-should-i-allocate-memory-for-c-string-char-array
  StaticJsonBuffer<16384> jsonBuffer;
  char *fileNameChar = (char *) malloc(sizeof(char) * 12);
  strcpy(fileNameChar, "p");
  strcat(fileNameChar, String(pattern).c_str());
  strcat(fileNameChar, "ch");
  strcat(fileNameChar, String(channel).c_str());
  strcat(fileNameChar,".jsn");

  elapsedMillis flashTimer = 0;

  // following ArduinoJSON serialize example: https://githtub.com/bblanchon/ArduinoJson/wiki/FAQ#whats-the-best-way-to-use-the-library
  JsonObject& root = jsonBuffer.createObject();

  root["stepCount"]     = sequenceArray[channel]->stepCount;
  root["beatCount"]     = sequenceArray[channel]->beatCount;
  root["quantizeKey"]   = sequenceArray[channel]->quantizeKey;
  root["instrument"]    = sequenceArray[channel]->instrument;
  root["instType"]      = sequenceArray[channel]->instType;
  root["volume"]        = sequenceArray[channel]->volume;
  root["bank"]          = sequenceArray[channel]->bank;
  root["channel"]       = sequenceArray[channel]->channel;
  root["patternIndex"]  = sequenceArray[channel]->patternIndex;

  JsonArray& stepDataArray = root.createNestedArray("stepData");

  for (int i=0; i< 128; i++){
    JsonObject& stepDataObj = jsonBuffer.createObject();
    stepDataObj["i"] = i ;
    stepDataObj["p"] = sequenceArray[channel]->stepData[i].pitch ;
    stepDataObj["gl"] = sequenceArray[channel]->stepData[i].gateLength ;
    stepDataObj["gt"] = sequenceArray[channel]->stepData[i].gateType ;
    stepDataObj["v"] = sequenceArray[channel]->stepData[i].velocity ;
    stepDataObj["g"] = sequenceArray[channel]->stepData[i].glide ;
    stepDataArray.add(stepDataObj);
  }

  SD.remove(fileNameChar);
  jsonFile = SD.open(fileNameChar, FILE_WRITE);
  jsonFile.seek(0);
  root.printTo(jsonFile);
  Serial.println("fileSize: " + String(jsonFile.size()));
  jsonFile.close();
  Serial.print("fileNameChar: ");
  Serial.println(fileNameChar);

  free(fileNameChar);

  Serial.println("flash Timer after write: " + String(flashTimer) );

//  Serial.println(String(jsonSaveTimer))

}

int FlashMemory::readSequenceJSON(uint8_t channel, uint8_t pattern){
    char *fileNameChar = (char *) malloc(sizeof(char) * 12);
    strcpy(fileNameChar, "P");
    strcat(fileNameChar, String(pattern).c_str());
    strcat(fileNameChar, "CH");
    strcat(fileNameChar, String(channel).c_str());
    strcat(fileNameChar,".JSN");
    char* charBuffer;                              // Declare a pointer to your buffer.
    if (!SD.exists(fileNameChar)){
      Serial.println("save file " + String(fileNameChar) + "does not exist.");
      return SAVEFILE_DOES_NOT_EXIST;
    } else {
      Serial.println("reading json file: " + String(fileNameChar));
      StaticJsonBuffer<16384> jsonBuffer;

      jsonFile = SD.open((const char *)fileNameChar, FILE_READ);
      if (jsonFile)
      {
          unsigned int fileSize = jsonFile.size();  // Get the file size.
          charBuffer = (char*)malloc(fileSize + 1);  // Allocate memory for the file and a terminating null char.
          jsonFile.read(charBuffer, fileSize);         // Read the file into the buffer.
          charBuffer[fileSize] = '\0';               // Add the terminating null char.
          //Serial.println(charBuffer);                // Print the file to the serial monitor.
          jsonFile.close();                         // Close the file.
      }
      // *** Use the buffer as needed here. ***
      JsonObject& jsonReader = jsonBuffer.parseObject(charBuffer);
      free(charBuffer);                              // Free the memory that was used by the buffer.
      free(fileNameChar);
      if (!jsonReader.success()) {
        Serial.println("parseObject() failed");
        return READ_JSON_ERROR;
      }
       sequenceArray[channel]->stepCount    = jsonReader["stepCount"];
       sequenceArray[channel]->beatCount    = jsonReader["beatCount"];
       sequenceArray[channel]->quantizeKey  = jsonReader["quantizeKey"];
       sequenceArray[channel]->instrument   = jsonReader["instrument"];
       sequenceArray[channel]->instType     = jsonReader["instType"];
       sequenceArray[channel]->volume       = jsonReader["volume"];
       sequenceArray[channel]->bank         = jsonReader["bank"];
       sequenceArray[channel]->channel      = jsonReader["channel"];
       sequenceArray[channel]->patternIndex = jsonReader["patternIndex"];

       JsonArray& stepDataArray = jsonReader["stepData"];

      for (int i=0; i< 128; i++){
         if (i != int(stepDataArray[i]["i"]) ) {
           Serial.println("Step Data Index Mismatch Error");
           return READ_JSON_ERROR;
         };
         sequenceArray[channel]->stepData[i].pitch  = stepDataArray[i]["p"];
         sequenceArray[channel]->stepData[i].gateLength  = stepDataArray[i]["gl"];
         sequenceArray[channel]->stepData[i].gateType  = stepDataArray[i]["gt"];
         sequenceArray[channel]->stepData[i].velocity  = stepDataArray[i]["v"];
         sequenceArray[channel]->stepData[i].glide  = stepDataArray[i]["g"];
      }
    }
    Serial.println("JSON file read complete");
    return FILE_EXISTS;
}

void FlashMemory::loadPattern(uint8_t pattern, uint8_t channelSelector) {

  Serial.println("========= LOADING PATTERN: " + String(pattern));
//  printPattern();

	for(int i=0; i < sequenceCount; i++){

    if ( !(channelSelector & (1 << i) ) ){
      Serial.println("skipping loading channel " + String(i));
      continue; // if channel is not selected to be loaded, don't load the channel!
    } else {
      Serial.println("Loading channel " + String(i));
    };

    int readJsonReturn = this->readSequenceJSON(i, pattern);

    if ( readJsonReturn == 1 )  {
      Serial.println("saveData not available, initializing sequence");
      sequenceArray[i]->initNewSequence(pattern, i);
    } else if (readJsonReturn == 2) {
      Serial.println("READ JSON ERROR - info above");
    }

    Serial.println("reading complete!");

    //sam2695.programChange(0, i, sequenceArray[i]->instrument);
    //sam2695.setChannelVolume(i, sequenceArray[i]->volume);

    sequenceArray[i]->quantizeKey = 1;
  }

  Serial.println("changing current pattern from " + String(currentPattern) + " to " + String(pattern) + " and also this is queuePattern: " + String(queuePattern));
  currentPattern = pattern;
  Serial.println("Pattern " + String(pattern) + " loaded");
//  printPattern();
}

void FlashMemory::printDirectory(File dir, int numTabs) {
   while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());

     String temp = String(entry.name());

     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }

     entry.close();

   }
}

void FlashMemory::printPattern(){
  Serial.println("Printing Data for pattern: " + String(currentPattern));
  for(int i=0; i < sequenceCount; i++){
    Serial.print("sc:\t"+String(sequenceArray[i]->stepCount) +"\tbc:\t"+String(sequenceArray[i]->beatCount) +"\tqk:\t"+String(sequenceArray[i]->quantizeKey)+"\tinst:\t"+String(sequenceArray[i]->instrument)+"\tit:\t"+String(sequenceArray[i]->instType) + "\t");
    for(int n=0; n<16; n++){
      Serial.print( String(sequenceArray[i]->stepData[n].pitch) + "\t" );
    }
    Serial.println("");
  }
}

void FlashMemory::deleteAllFiles(){
 File root= SD.open("/");
 this->rm(root, "/");
 root.close();

}

void FlashMemory::rm(File dir, String tempPath) {
  int DeletedCount = 0;
  int FolderDeleteCount = 0;
  int FailCount = 0;
  String rootpath = "/";



  while(true) {
    File entry =  dir.openNextFile();
    String localPath;

    Serial.println("");
    if (entry) {
      if ( entry.isDirectory() )
      {
        localPath = tempPath + entry.name() + rootpath + '\0';
        char folderBuf[localPath.length()];
        localPath.toCharArray(folderBuf, localPath.length() );
        rm(entry, folderBuf);


        if( SD.rmdir( folderBuf ) )
        {
          Serial.print("Deleted folder ");
          Serial.println(folderBuf);
          FolderDeleteCount++;
        }
        else
        {
          Serial.print("Unable to delete folder ");
          Serial.println(folderBuf);
          FailCount++;
        }
      }
      else
      {
        localPath = tempPath + entry.name() + '\0';
        char charBuf[localPath.length()];
        localPath.toCharArray(charBuf, localPath.length() );

        if( SD.remove( charBuf ) )
        {
          Serial.print("Deleted ");
          Serial.println(localPath);
          DeletedCount++;
        }
        else
        {
          Serial.print("Failed to delete ");
          Serial.println(localPath);
          FailCount++;
        }

      }
    }
    else {
      // break out of recursion
      break;
    }
  }
}
