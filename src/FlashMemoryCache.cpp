#include <Arduino.h>
#include "FlashMemory.h"


void FlashMemory::setCacheStatus(int index, int status){
  cacheStatus[index] = status;
  // Serial.println("cache index " + String(index) + " set to " + String(status)  );
}

int FlashMemory::getCacheIndex(int channel,int pattern){
  return pattern*4+channel;
};

int  FlashMemory::getCacheStatus(int index){
  return cacheStatus[index];
};

bool FlashMemory::are_all_cache_statuses_zeroed(){
  for(int cacheIndex =0; cacheIndex < CACHE_COUNT; cacheIndex++){
    if(cacheStatus[cacheIndex] != 0){
      return false; // there was a cache status that was not zero
    };
  }
  return true;
}

bool FlashMemory::doesSeqDataExist(){
  char* fileName = (char *) malloc(sizeof(char) * 12);
  fileName =strdup("seqData");
  if (spiFlash->exists(fileName)) {
    return 1;
  } else {
    return 0;
  }
}

bool FlashMemory::is_erased(char *data, unsigned int len)
{
	while (len > 0) {
		if (*data++ != 255) return false;
		len = len - 1;
	}
	return true;
}

void FlashMemory::initializeCache(){
  Serial.println("initializing save file cache... ");
  char* fileName = (char *) malloc(sizeof(char) * 12);

  fileName = strdup( "seqCache");
  if (!spiFlash->exists(fileName)) {
    Serial.println("Creating Cache File: " + String(fileName) );
    spiFlash->createErasable(fileName, FLASHFILESIZE);
  } else {
        Serial.println("Cache File exists, erasing nonzero sectors...");
    file = spiFlash->open(fileName);   //open cache file
    
    for(int location = 0; location < FLASHFILESIZE; location += SECTORSIZE){
      file.seek(location);
      while (!spiFlash->ready()){ Serial.println("waiting for spiflash"); delay(25);}
      char * fileBuffer = (char*)calloc(SECTORSIZE, sizeof(char) );
      file.read(fileBuffer, SECTORSIZE); // fill buffer with cache file
      if ( !is_erased(fileBuffer,4096) ){
        Serial.println("erasing sector " + String(location));
        file.erase4k();
      }
      free(fileBuffer);
      fileBuffer = NULL;
    }
    file.close();
  }
  free(fileName);
  fileName = NULL;
  while(!spiFlash->ready()){
    Serial.println("waiting for cache..");
    delay(100);
  }
  //Randomize a cache offset so that EEPROM is used evenly across all bytes
  //EEPROM will begin to degrade after 100000 writes, so this will increase longevity of the device.
  //Identify previous cache offset value:
  cacheNum = 0;
  spiFlashBusy = false;
  //listFiles();
  Serial.println("initializing save file cache complete. ");

};

int FlashMemory::cacheWriteLoop(){
  int channel = 0;
  int pattern = 0;
  int cacheStat = 0;
  int cacheIndex = 0;
  if(saveSequenceBusy){
    return SAVE_OPERATION_IN_PROGRESS;
  }
  if((globalObj->sysex_status == SYSEX_IMPORTING) && (cacheWriteSysexTimer < 5000000)){
    return SYSEX_OPERATION_IN_PROGRESS; //if a sysex message has recently been received, return as not to clog up the import
  } else {
    if(globalObj->sysex_status == SYSEX_IMPORTING){
      this->cacheWriteSwitch = true;
      globalObj->sysex_status = SYSEX_PROCESSING;
    }
  }

    
  for(int ci=0; ci< CACHE_COUNT; ci++){
    cacheStat = cacheStatus[ci];
    cacheIndex = ci;
    if (cacheStat != 0){
      break;
    }
  }

  pattern = cacheIndex / 4;
  channel = cacheIndex % 4;
  // Serial.println("Running CacheWriteLoop for " + String(channel) + "\tp: " + String(pattern)+ "\tCacheIndex: " + String(cacheIndex));

  if( cacheWriteTimer > CACHE_WRITE_DELAY ){

      char* fileName = (char *) malloc(sizeof(char) * 12);
      char* cacheFileName = (char *) malloc(sizeof(char) * 12);

      cacheFileName = strdup("seqCache");
      fileName = strdup("seqData");

      if (spiFlash->ready()){
        char * fileBuffer = (char*)calloc(SECTORSIZE, sizeof(char) );
        
        switch (cacheStatus[cacheIndex]){

        /*
        save to cache sector
        erase save sector
        copy cache sector to save sector
        erase cache sector
        */
          case SAVING_TO_CACHE_SECTOR:
           Serial.println(")))))) ____ ===== > > > erasing save sector ch:"  + String(channel) + "\tPT:" + String(pattern) + "\tCacheIndex: " + String(cacheIndex) + "\tsaveaddress: " + String(getSaveAddress(cacheIndex)) );
            //next step is to erase the save sector
            file = spiFlash->open(fileName);   //open save file for erasing
            if(file){
              file.seek(getSaveAddress(cacheIndex));
              file.erase4k();
              setCacheStatus(cacheIndex, ERASING_SAVE_SECTOR);
              cacheWriteTimer = 0;
            } else {
              Serial.println(":::{{{--->>><<<---}}}:::CACHELOOP " + String(cacheNum) + " :::{{{--->>><<<---}}}::: FILE ERASE ERROR - " + String(fileName));
            }
            file.close();
          //  Serial.println("(((((( ____ ===== < < < erasing save sector COMPLETE ch:"  + String(channel) + "\tPT:" + String(pattern) );

          break;

          case ERASING_SAVE_SECTOR:

          //  Serial.println(")))))) ____ ===== > > > copying cache sector to save sector CH:" + String(channel) + "\tPT:" + String(pattern) + "\t::");
            //next step is to copy cache sector to save sector
            file = spiFlash->open(cacheFileName); // open cache file
            if(file){
              file.seek(getCacheSaveAddress(cacheIndex));
              file.read(fileBuffer, SECTORSIZE); // fill buffer with cache file0
              file.close();    // close cache file

            } else { Serial.println(":::{{{--->>><<<---}}}:::CACHELOOP " + String(cacheNum) + " :::{{{--->>><<<---}}}::: CACHE READ ERROR - CH: "+ String(channel));
            }

            // write to save file
            file = spiFlash->open(fileName);   //open save file

            if (file){
              file.seek(getSaveAddress(cacheIndex)); //seek to address
              file.write(fileBuffer, SECTORSIZE);
              cacheWriteTimer = 0;
            //  Serial.println("Writing CH:" + String(channel) + "\tPT:" + String(pattern) + "\t" );
            //  Serial.println(fileBuffer);
              file.close();    // close cache file

            } else { Serial.println(":::{{{--->>><<<---}}}:::CACHELOOP " + String(cacheNum) + " :::{{{--->>><<<---}}}::: CACHE WRITE ERROR - CH: "+ String(channel));
          }
            setCacheStatus(cacheIndex, COPY_CACHE_TO_SAVE_SECTOR);

            spiFlashBusy = false;
          //  Serial.println("(((((( ____ ===== < < <  copying cache sector to save sector COMPLETE CH:" + String(channel) + "\tPT:" + String(pattern) );

          break;

          case COPY_CACHE_TO_SAVE_SECTOR:

      //    Serial.println(")))))) ____ ===== > > > erasing cache sector CH:" + String(channel) + "\tPT:" + String(pattern));
            // next step is to erase the cache sector.
            file = spiFlash->open(cacheFileName); // open cache file
            if (file) {
              file.seek(getCacheSaveAddress(cacheIndex));
              file.erase4k();
              cacheWriteTimer = 0;
              setCacheStatus(cacheIndex, ERASING_CACHE_SECTOR);
            } else { Serial.println(":::{{{--->>><<<---}}}:::CACHELOOP " + String(cacheNum) + " :::{{{--->>><<<---}}}::: CACHE CLEAR ERROR - CH: "+ String(channel));
          }

            file.close();    // close cache file
        //    Serial.println("(((((( ____ ===== < < < erasing cache sector COMPLETE CH:" + String(channel) + "\tPT:" + String(pattern));

          break;

          case ERASING_CACHE_SECTOR:
          //  Serial.println(")))) - _ - _ > > >  >  > ERASING CACHE SECTOR )) ) ) < < < CH:" + String(channel) + "\tPT:" + String(pattern));
            //save operation complete.
  /*          file = spiFlash->open(fileName); // open cache file
            if(file){
              for(int i=0; i<8; i++){
                file.seek(getSaveAddress(cacheIndex)+64*i);
                file.read(fileBuffer, 64); // fill buffer with cache file
                fileBuffer[64] = '\0';               // Add the terminating null char.
                Serial.println(String(i*64) + ":\t" );
                Serial.println(fileBuffer);
                free(fileBuffer);
              }
            }            */
          //  file.close();    // close cache file
            cacheStatus[cacheIndex] = CACHE_READY;
          //  Serial.println("(((((( ____ ===== < < <  ERASING CACHE SECTOR )) ) ) < < <  COMPLETE CH:" + String(channel) + "\tPT:" + String(pattern));

          break;
        }
        free(fileBuffer);
        fileBuffer = NULL;

      }
      free(fileName);
      fileName = NULL;
      free(cacheFileName);
      cacheFileName = NULL;

      if(cacheWriteSwitch && are_all_cache_statuses_zeroed()){
        // Serial.println("SAVE OPERATION COMPLETE " + String(cacheWriteTotalTimer));
        if(globalObj->sysex_status == SYSEX_PROCESSING) {
          globalObj->sysex_status = SYSEX_IMPORTCOMPLETE;
          Serial.println("All caches have been written to memory, import complete");
        }
        cacheWriteSwitch = 0;
      }

  }
}
