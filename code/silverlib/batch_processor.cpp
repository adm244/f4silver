/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

//IMPORTANT(adm244): SCRATCH VERSION JUST TO GET IT UP WORKING

#ifndef BATCH_PROCESSOR_H
#define BATCH_PROCESSOR_H

#define BATCH_DEFAULT "@default"
#define BATCH_INTERIOR "@interior"
#define BATCH_EXTERIOR "@exterior"
#define BATCH_INTERIOR_ONLY "@interioronly"
#define BATCH_EXTERIOR_ONLY "@exterioronly"
#define BATCH_RANDOM_EXCLUDE "@randomexclude"

//TODO(adm244): remove this
//TODO(adm244): move outside
#define BATCH_SAVEGAME "@savegame"
#define BATCH_TELEPORT "@teleport"
#define BATCH_TIMEOUT "@timeout"

#define EXEC_DEFAULT 0
#define EXEC_INTERIOR 1
#define EXEC_EXTERIOR 2
#define EXEC_INTERIOR_ONLY 3
#define EXEC_EXTERIOR_ONLY 4

struct BatchData {
  char filename[MAX_FILENAME];
  char description[MAX_DESCRIPTION];
  
  uint64 offset;
  int timerIndex;
  
  int key;
  uint executionState;
  bool excludeRandom;
  
  bool enabled;
};

struct CustomCommand {
  int key;
  bool enabled;
};

/*struct BatchGroup {
  uint length;
  uint batches[MAX_BATCHES];
};*/

internal BatchData batches[MAX_BATCHES];
//internal BatchGroup randomBatchGroup;
internal CustomCommand CommandToggle;
internal CustomCommand CommandRandom;

internal bool keys_active = true;
internal bool not_initialized = true;
internal int batches_count = 0;

/*internal BatchData * GetRandomBatch(BatchGroup *group)
{
  BatchData *result = 0;
  
  if( group && group->length > 0 ) {
    int index = RandomInt(0, group->length - 1);
    result = &(group->batches[index]);
  }
  
  return result;
}*/

//FIX(adm244): loading into interior cell and teleporting crashing the game
// (wrong TESWorldSpace pointer is passed into FindExteriorCell)
internal void Teleport()
{
  TESWorldSpace *worldspace = GetPlayerCurrentWorldSpace();
  if( !worldspace ) {
    //TODO(adm244): remember the last one?
    worldspace = *TES_GetWorldSpaceArray();
  }
  
  /*FILE *file = 0;
  fopen_s(&file, "f4silver.log", "a");
  
  fprintf_s(file, "Teleport function started.\n");*/
  
  TESCell *targetCell;
  int minX = Min(worldspace->NWCellX, worldspace->SECellX);
  int maxX = Max(worldspace->NWCellX, worldspace->SECellX);
  int minY = Min(worldspace->NWCellY, worldspace->SECellY);
  int maxY = Max(worldspace->NWCellY, worldspace->SECellY);
  
  uint grid_width = (maxX - minX + 1);
  uint grid_height = (maxY - minY + 1);
  uint cells_count = grid_width * grid_height;
  
  uint loop_count = 0;
  
  TESPlayer *playerRef = TES_GetPlayer();
  TESCell *player_cell = ((TESObjectReference *)playerRef)->parentCell;
  
  int player_cell_x = -999;
  int player_cell_y = -999;
  
  if (player_cell && player_cell->coordinates) {
    player_cell_x = player_cell->coordinates->x;
    player_cell_y = player_cell->coordinates->y;
  }
  
  //uint64 old_seed = random_seed;
  RandomInitializeSeed(GetTickCount64());
  
  do {
    if( loop_count++ > cells_count ) {
      //fprintf_s(file, "Loop limit reached.\n");
      break;
    }
  
    /*uint index = RandomInt(0, cells_count - 1);
    int cellY = (int)(index / grid_width);
    int cellX = (index - (cellY * grid_width));
    cellY += minY;
    cellX += minX;*/
    
    int cellX = RandomInt(minX, maxX);
    int cellY = RandomInt(minY, maxY);
    
    uint cell_distance_sqr = Sqr(cellX - player_cell_x) + Sqr(cellY - player_cell_y);
    if (cell_distance_sqr < 144) {
      continue;
    }
    
    targetCell = TESWorldSpace_FindExteriorCell(worldspace, cellX, cellY);
    
    /*if( IsCellWithinBorderRegion(targetCell) ) {
      fprintf_s(file, "Success [%d, %d]\n", cellX, cellY);
    } else {
      fprintf_s(file, "Failure [%d, %d]\n", cellX, cellY);
    }*/
  } while( !IsCellWithinBorderRegion(targetCell) );
  
  //RandomInitializeSeed(old_seed);
  
  /*fprintf_s(file, "Teleport function ended.\n");
  fclose(file);*/
  
  if( targetCell ) {
    TESObjectReference_MoveToCell((TESObjectReference *)TES_GetPlayer(), 0, targetCell);
  }
}

//NOTE(adm244): overloaded version for CustomCommand structure
internal bool IsActivated(CustomCommand *cmd)
{
  return(IsActivated(cmd->key, &cmd->enabled));
}

internal void ReadBatchFile(BatchData *batch)
{
  FILE *src = NULL;
  fopen_s(&src, batch->filename, "r");
  
  if( src ){
    char line[4096];
    
    fgets(line, sizeof(line), src);
    
    uint32 lineLen = strlen(line);
    if(line[lineLen - 1] == '\n'){
      line[lineLen - 1] = 0;
    }
    strcpy(batch->description, line);
    
    while( fgets(line, sizeof(line), src) ){
      uint32 lineLen = strlen(line);
      
      if(line[lineLen - 1] == '\n'){
        line[lineLen - 1] = 0;
      }
    
      if( strcmp(line, BATCH_EXTERIOR_ONLY) == 0 ) {
        batch->executionState = EXEC_EXTERIOR_ONLY;
      } else if( strcmp(line, BATCH_INTERIOR_ONLY) == 0 ) {
        batch->executionState = EXEC_INTERIOR_ONLY;
      } else if (strcmp(line, BATCH_RANDOM_EXCLUDE) == 0 ) {
        batch->excludeRandom = true;
      }
    }

    fclose(src);
  } else {
    //NOTE(adm244): couldn't read a file
  }
}

//NOTE(adm244): loads a list of batch files and keys that activate them
// filename and associated keycode stored in a BatchData structure pointed to by 'batches'
//
// returns true if at least 1 bat file loaded successefully
// returns false if no bat files were loaded
internal bool InitBatchFiles(HMODULE module, BatchData *batches, int *num)
{
  char buf[MAX_SECTION];
  char *str = buf;
  int index = 0;
  
  IniReadSection(module, CONFIG_FILE, "batch", buf, MAX_SECTION);
  
  while( true ){
    char *p = strrchr(str, '=');
    
    if( p && (index < MAX_BATCHES) ){
      char *endptr;
      *p++ = '\0';
      
      strcpy(batches[index].filename, str);
      strcpy(batches[index].description, str);
      batches[index].timerIndex = -1;
      batches[index].offset = 0;
      batches[index].key = (int)strtol(p, &endptr, 0);
      batches[index].excludeRandom = false;
      batches[index].enabled = true;
      
      ReadBatchFile(&batches[index]);
      
      str = strchr(p, '\0');
      str++;
      
      index++;
    } else{
      break;
    }
  }
  
  *num = index;
  return(index > 0);
}

/*internal void ReadBatchDescriptions(BatchData *batches)
{
  FILE *file = NULL;
  char line[MAX_DESCRIPTION];
  
  for( int i = 0; i < batches_count; ++i ) {
    fopen_s(&file, batches[i].filename, "r");
    
    if( file ) {
      if( fgets(line, sizeof(line), file) ) {
        uint32 lineLen = strlen(line);
        if(line[lineLen - 1] == '\n'){
          line[lineLen - 1] = 0;
        }
        
        strcpy(batches[i].description, line);
      }
      
      fclose(file);
    }
  }
}

//IMPORTANT(adm244): get rid off duplicated file readings!
internal uint8 GetBatchExecState(char *filename)
{
  uint8 result = EXEC_DEFAULT;

  FILE *src = NULL;
  fopen_s(&src, filename, "r");
  
  if( src ){
    char line[4096];
    
    //FIX(adm244): hack
    fgets(line, sizeof(line), src);
    
    while( fgets(line, sizeof(line), src) ){
      uint32 lineLen = strlen(line);
      
      if(line[lineLen - 1] == '\n'){
        line[lineLen - 1] = 0;
      }
    
      if( strcmp(line, BATCH_EXTERIOR_ONLY) == 0 ) {
        result = EXEC_EXTERIOR_ONLY;
        break;
      } else if( strcmp(line, BATCH_INTERIOR_ONLY) == 0 ) {
        result = EXEC_INTERIOR_ONLY;
        break;
      }
    }

    fclose(src);
  }

  return result;
}*/

enum ExecuteBatch_ResultCodes {
  ExecuteBatch_Fail = -1,
  ExecuteBatch_Success = 0,
  ExecuteBatch_OnDelay,
};

internal int ExecuteBatch(BatchData *batch, uint64 offset)
{
  int result = ExecuteBatch_Success;
  
  FILE *src = NULL;
  fopen_s(&src, batch->filename, "r");
  if( offset > 0 ) {
    fsetpos(src, (fpos_t *)&offset);
  }

  if( src ){
    char line[4096];
    uint8 executionState = EXEC_DEFAULT;
    
    //FIX(adm244): hack
    if( offset == 0 ) {
      fgets(line, sizeof(line), src);
    }
    
    while( fgets(line, sizeof(line), src) ){
      uint32 lineLen = strlen(line);
      
      if(line[lineLen - 1] == '\n'){
        line[lineLen - 1] = 0;
      }
      
      if( strcmp(line, BATCH_EXTERIOR) == 0 ) {
        executionState = EXEC_EXTERIOR;
      } else if( strcmp(line, BATCH_INTERIOR) == 0 ) {
        executionState = EXEC_INTERIOR;
      } else if( strcmp(line, BATCH_DEFAULT) == 0 ) {
        executionState = EXEC_DEFAULT;
      } else if( strcmp(line, BATCH_SAVEGAME) == 0 ) {
        //TODO(adm244): implement SaveGame function
        //SaveGame(Strings.SaveDisplayName, Strings.SaveFileName);
      } else if( strcmp(line, BATCH_TELEPORT) == 0 ) {
        Teleport();
      } else if( strncmp(line, BATCH_TIMEOUT, strlen(BATCH_TIMEOUT)) == 0 ) {
        char timeoutBuffer[8];
        strncpy_s(timeoutBuffer, 8, line + strlen(BATCH_TIMEOUT), 8);
        
        int timeoutValue = atoi(timeoutBuffer);
        if( timeoutValue > 0 ) {
          if( !fgetpos(src, (fpos_t *)&batch->offset) ) {
            batch->timerIndex = StartTimer(timeoutValue * 1000);
            //TODO(adm244): implement assert
            //assert(batch->timerIndex >= 0);
            result = ExecuteBatch_OnDelay;
            break;
          }
        }
      } else if( strcmp(line, BATCH_INTERIOR_ONLY) == 0 ) {
        //NOTE(adm244): should be empty
      } else if( strcmp(line, BATCH_EXTERIOR_ONLY) == 0 ) {
        //NOTE(adm244): should be empty
      } else if (strcmp(line, BATCH_RANDOM_EXCLUDE) == 0 ) {
        //NOTE(adm244): should be empty
      } else {
        if( (IsInterior && (executionState == EXEC_INTERIOR))
         || (!IsInterior && (executionState == EXEC_EXTERIOR))
         || (executionState == EXEC_DEFAULT) ) {
          if( !TES_ExecuteScriptLine(line) ) {
            result = ExecuteBatch_Fail;
            break;
          }
        }
      }
    }
    
    fclose(src);
  }

  return result;
}

//NOTE(adm244): initializes plugin variables
// returns true if atleast 1 batch file was successefully loaded
// returns false otherwise
internal int InitilizeBatches(HMODULE module)
{
  keys_active = true;
  InitBatchFiles(module, batches, &batches_count);
  //ReadBatchDescriptions(batches);
  
  //TODO(adm244): rewrite IniRead* functions so they accept full path to config file folder
  CommandToggle.key = IniReadInt(module, CONFIG_FILE, CONFIG_KEYS_SECTION, "iKeyToggle", VK_HOME);
  CommandToggle.enabled = true;
  
  CommandRandom.key = IniReadInt(module, CONFIG_FILE, CONFIG_KEYS_SECTION, "iKeyRandomBatch", VK_ADD);
  CommandRandom.enabled = true;
  
  return(batches_count);
}

#endif
