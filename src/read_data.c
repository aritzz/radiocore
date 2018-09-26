/*
    ARAS - A Radio Automation System
    Copyright (C) 2015 Erasmo Alonso Iglesias

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/


#include "aras.h"


struct aras_configuration *read_configuration(struct aras_data *data)
{

  FILE *configuration_file;
  char configuration_line[CONFIGURATION_LINE_MAX_SIZE];
  char configuration_variable[CONFIGURATION_VARIABLE_MAX_SIZE];
  char configuration_value[CONFIGURATION_VALUE_MAX_SIZE];
  char *pointer_1;
  char *pointer_2;

  configuration_file = fopen(data->configuration_file_name, "r");

  if (configuration_file) {

    /* Initialization of some default configuration values */
    strcpy(data->configuration->schedule_file_name, "aras.schedule");
    strcpy(data->configuration->block_file_name, "aras.block");
    strcpy(data->configuration->log_file_name, "aras.log");
    strcpy(data->configuration->block_player_output, "autoaudiosink");
    strcpy(data->configuration->block_player_device, "default");
    strcpy(data->configuration->time_signal_player_output, "autoaudiosink");
    strcpy(data->configuration->time_signal_player_device, "default");
    strcpy(data->configuration->recorder_input, "autoaudiosrc");
    strcpy(data->configuration->recorder_device, "default");
    strcpy(data->configuration->recorder_quality, "0.3");
    strcpy(data->configuration->recorder_storage, "Off");
    strcpy(data->configuration->schedule_support, "Hard");
    strcpy(data->configuration->time_signal, "Hour");
    strcpy(data->configuration->time_signal_path, "file:///home/radio/ARAS/media/time_signal.wav");
    strcpy(data->configuration->interval_signal, "On");
    strcpy(data->configuration->interval_signal_block, "IntervalSignal");
    strcpy(data->configuration->block_player_volume, "5E-1");
    strcpy(data->configuration->time_signal_player_volume, "5E-1");
    strcpy(data->configuration->fade_out_time, "4000");
    strcpy(data->configuration->fade_out_resolution, "100");
    strcpy(data->configuration->fade_out_slope, "1E-1");
    strcpy(data->configuration->default_timeout, "100");
    strcpy(data->configuration->live_timeout, "55");
    strcpy(data->configuration->time_signal_timeout, "100");
    strcpy(data->configuration->configuration_timeout, "1000");
    strcpy(data->configuration->instance, "1");


    while (!feof(configuration_file)) {
      if(fgets(configuration_line, CONFIGURATION_LINE_MAX_SIZE, configuration_file) != NULL) {
        /* The line is not a comment and the line is not an end of line */
        if ((memcmp(configuration_line, "#", 1) != 0) && (memcmp(configuration_line, "\n", 1)) != 0) {
          /* Retrieve variable and value */
          sscanf(configuration_line, "%s%s\n", configuration_variable, configuration_value);
          
          if (strcmp(configuration_variable, "ScheduleFile") == 0) {
            strcpy(data->configuration->schedule_file_name, configuration_value);
            /* Retrieve value when there are quotation marks */
            pointer_1 = strstr(configuration_line, "\"");
            if ((pointer_1 != NULL)) {
              pointer_2 = strstr(pointer_1+1, "\"");
              if (pointer_2 != NULL) {
                memset(pointer_2, 0, 1);
                strcpy(data->configuration->schedule_file_name, pointer_1+1);
              }
            }
          } else if (strcmp(configuration_variable, "BlockFile") == 0) {
            strcpy(data->configuration->block_file_name, configuration_value);
            /* Retrieve value when there are quotation marks */
            pointer_1 = strstr(configuration_line, "\"");
            if ((pointer_1 != NULL)) {
              pointer_2 = strstr(pointer_1+1, "\"");
              if (pointer_2 != NULL) {
                memset(pointer_2, 0, 1);
                strcpy(data->configuration->block_file_name, pointer_1+1);
              }
            }
          } else if (strcmp(configuration_variable, "LogFile") == 0) {
            strcpy(data->configuration->log_file_name, configuration_value);
            /* Retrieve value when there are quotation marks */
            pointer_1 = strstr(configuration_line, "\"");
            if ((pointer_1 != NULL)) {
              pointer_2 = strstr(pointer_1+1, "\"");
              if (pointer_2 != NULL) {
                memset(pointer_2, 0, 1);
                strcpy(data->configuration->log_file_name, pointer_1+1);
              }
            }
          } else if (strcmp(configuration_variable, "BlockPlayerOutput") == 0) {
            strcpy(data->configuration->block_player_output, configuration_value);
          } else if (strcmp(configuration_variable, "BlockPlayerDevice") == 0) {
            strcpy(data->configuration->block_player_device, configuration_value);
            /* Retrieve value when there are quotation marks */
            pointer_1 = strstr(configuration_line, "\"");
            if ((pointer_1 != NULL)) {
              pointer_2 = strstr(pointer_1+1, "\"");
              if (pointer_2 != NULL) {
                memset(pointer_2, 0, 1);
                strcpy(data->configuration->block_player_device, pointer_1+1);
              }
            }
          } else if (strcmp(configuration_variable, "TimeSignalPlayerOutput") == 0) {
            strcpy(data->configuration->time_signal_player_output, configuration_value);
          } else if (strcmp(configuration_variable, "TimeSignalPlayerDevice") == 0) {
            strcpy(data->configuration->time_signal_player_device, configuration_value);
            /* Retrieve value when there are quotation marks */
            pointer_1 = strstr(configuration_line, "\"");
            if ((pointer_1 != NULL)) {
              pointer_2 = strstr(pointer_1+1, "\"");
              if (pointer_2 != NULL) {
                memset(pointer_2, 0, 1);
                strcpy(data->configuration->time_signal_player_device, pointer_1+1);
              }
            }
          } else if (strcmp(configuration_variable, "RecorderInput") == 0) {
            strcpy(data->configuration->recorder_input, configuration_value);
          } else if (strcmp(configuration_variable, "RecorderDevice") == 0) {
            strcpy(data->configuration->recorder_device, configuration_value);
          } else if (strcmp(configuration_variable, "RecorderQuality") == 0) {
            strcpy(data->configuration->recorder_quality, configuration_value);
          } else if (strcmp(configuration_variable, "RecorderStorage") == 0) {
            strcpy(data->configuration->recorder_storage, configuration_value);
          } else if (strcmp(configuration_variable, "ScheduleSupport") == 0) {
            strcpy(data->configuration->schedule_support, configuration_value);
          } else if (strcmp(configuration_variable, "TimeSignal") == 0) {
            strcpy(data->configuration->time_signal, configuration_value);
          } else if (strcmp(configuration_variable, "TimeSignalPath") == 0) {
            strcpy(data->configuration->time_signal_path, configuration_value);
            /* Retrieve value when there are quotation marks */
            pointer_1 = strstr(configuration_line, "\"");
            if ((pointer_1 != NULL)) {
              pointer_2 = strstr(pointer_1+1, "\"");
              if (pointer_2 != NULL) {
                memset(pointer_2, 0, 1);
                strcpy(data->configuration->time_signal_path, pointer_1+1);
              }
            }
          } else if (strcmp(configuration_variable, "IntervalSignal") == 0) {
            strcpy(data->configuration->interval_signal, configuration_value);
          } else if (strcmp(configuration_variable, "IntervalSignalBlock") == 0) {
            strcpy(data->configuration->interval_signal_block, configuration_value);
          } else if (strcmp(configuration_variable, "BlockPlayerVolume") == 0) {
            strcpy(data->configuration->block_player_volume, configuration_value);
          } else if (strcmp(configuration_variable, "TimeSignalPlayerVolume") == 0) {
            strcpy(data->configuration->time_signal_player_volume, configuration_value);
          } else if (strcmp(configuration_variable, "FadeOutTime") == 0) {
            strcpy(data->configuration->fade_out_time, configuration_value);
          } else if (strcmp(configuration_variable, "FadeOutResolution") == 0) {
            strcpy(data->configuration->fade_out_resolution, configuration_value);
          } else if (strcmp(configuration_variable, "FadeOutSlope") == 0) {
            strcpy(data->configuration->fade_out_slope, configuration_value);
          } else if (strcmp(configuration_variable, "DefaultTimeout") == 0) {
            strcpy(data->configuration->default_timeout, configuration_value);
          } else if (strcmp(configuration_variable, "TimeSignalTimeout") == 0) {
            strcpy(data->configuration->time_signal_timeout, configuration_value);
          } else if (strcmp(configuration_variable, "ConfigurationTimeout") == 0) {
            strcpy(data->configuration->configuration_timeout, configuration_value);
          } else if (strcmp(configuration_variable, "DBHost") == 0) {
            strcpy(data->configuration->db_host, configuration_value);
          } else if (strcmp(configuration_variable, "DBName") == 0) {
            strcpy(data->configuration->db_name, configuration_value);
          } else if (strcmp(configuration_variable, "DBUser") == 0) {
            strcpy(data->configuration->db_user, configuration_value);
          } else if (strcmp(configuration_variable, "DBPass") == 0) {
            strcpy(data->configuration->db_pass, configuration_value);
          } else if (strcmp(configuration_variable, "Instance") == 0) {
            strcpy(data->configuration->instance, configuration_value);
          }
        }
      }
    }

    fclose(configuration_file);
    return(data->configuration);

  } else {
    printf("File %s could not be opened\n", data->configuration_file_name);
    return(NULL);
  }
}

/*
 * Read schedule info from database
 */

 GList *read_schedule(struct aras_data *data)
{

  struct aras_schedule *schedule_element;
  char schedule_day[SCHEDULE_DAY_MAX_SIZE];
  char schedule_time[SCHEDULE_TIME_MAX_SIZE];
  char schedule_block[SCHEDULE_BLOCK_MAX_SIZE];
  //char radiocore_dir[SCHEDULE_BLOCK_MAX_SIZE-20];
  char buffer[1024];
  GList *schedule;
  MYSQL_RES *res;
  MYSQL_ROW row;
  
 // log_db(data, "Reading schedule list from database");
 sprintf(buffer, "SELECT day,hour,type FROM schedule INNER JOIN instances ON schedule.calendar_id = instances.calendar_id WHERE instances.id='%s'", data->configuration->instance);
  //printf(buffer);
  if (!mysql_query(data->dbcon, buffer)){
    schedule = NULL;
    res = mysql_store_result(data->dbcon);

    while ((row = mysql_fetch_row(res)) != NULL) {
            sprintf(schedule_day, "%s", row[0]);
            sprintf(schedule_time, "%s", row[1]);
            sprintf(schedule_block, "%s", row[2]);
	  
          if ((schedule_element = (struct aras_schedule*)malloc(sizeof(struct aras_schedule))) != NULL) {
            strcpy(schedule_element->schedule_day, schedule_day);
            strcpy(schedule_element->schedule_time, schedule_time);
            strcpy(schedule_element->schedule_block, schedule_block);
            schedule = g_list_prepend(schedule, schedule_element);
          } else {
            printf("%s\n", "Out of memory, unable to add schedule element");
            printf("%s%s\n", "Day: ", schedule_day);
            printf("%s%s\n", "Time: ", schedule_time);
            printf("%s%s\n", "Block: ", schedule_block);
          }

    }

    mysql_free_result(res);
  } else {
    fprintf(stderr, "[RadioCore] Could not update schedule from DB %s\n", mysql_error(data->dbcon));
    schedule = data->schedule; // dont stop working, return the same pointer
  }


  return(g_list_reverse(schedule));
}


/*
 * Read block info from database
 */


GList *read_blocks(struct aras_data *data)
{

  struct aras_block *block_element;
  char block_name[BLOCK_NAME_MAX_SIZE];
  char block_type[BLOCK_TYPE_MAX_SIZE];
  char block_data[BLOCK_DATA_MAX_SIZE];
  char block_desc[BLOCK_DESC_MAX_SIZE];
  char tmp_blockdata[BLOCK_DATA_MAX_SIZE];
  char radiocore_dir[SCHEDULE_BLOCK_MAX_SIZE-20];

  GList *blocks;
  MYSQL_RES *res, *res_dir;
  MYSQL_ROW row, row2;

 // log_db(data, "Reading block list from database");
 // log_db(data, "Reading schedule list from database");
  if (!mysql_query(data->dbcon, "SELECT dirpath FROM dirs WHERE dirname='radiocore_dir' LIMIT 1")) {
    res_dir = mysql_store_result(data->dbcon);
     if ((row2 = mysql_fetch_row(res_dir)) != NULL) {
       sprintf(radiocore_dir, "%s", row2[0]);
     }
   mysql_free_result(res_dir);
  }



  if (!mysql_query(data->dbcon, "SELECT name,type,vars,blocks.desc FROM blocks WHERE enabled=1")) {

    blocks = NULL;
    res = mysql_store_result(data->dbcon);

    while ((row = mysql_fetch_row(res)) != NULL) {
	    strcpy(block_data, "");
      sprintf(block_name, "%s", row[0]);
      sprintf(block_type, "%s", row[1]);
      sprintf(block_data, "%s", row[2]);
	    sprintf(block_desc, "%s", row[3]);

      if (!strcmp(block_type, "File"))
  		if (!strstr(block_data, "http")) {
  			sprintf(tmp_blockdata, "file://%s%s", radiocore_dir, row[2]);
			  strcpy(block_data, tmp_blockdata);
		  }

      if (!strcmp(block_type, "Random") ||
		      !strcmp(block_type, "RandomFile"))
      {
        sprintf(tmp_blockdata, "%s%s/", radiocore_dir, row[2]);
        strcpy(block_data, tmp_blockdata);
      }

      if (!strcmp(block_type, "Playlist"))
      {
        sprintf(tmp_blockdata, "%s%s", radiocore_dir, row[2]);
        strcpy(block_data, tmp_blockdata);
      }

      if ((block_element = (struct aras_block*)malloc(sizeof(struct aras_block))) != NULL) {
        strcpy(block_element->block_name, block_name);
        strcpy(block_element->block_type, block_type);
        strcpy(block_element->block_data, block_data);
        strcpy(block_element->block_desc, block_desc);
        blocks = g_list_prepend(blocks, block_element);
      } else {
        printf("%s\n", "Out of memory, unable to add block element");
        printf("%s%s\n", "Name: ", block_name);
        printf("%s%s\n", "Type: ", block_type);
        printf("%s%s\n", "Data: ", block_data);
      }

    }
    mysql_free_result(res);
  } else {
    fprintf(stderr, "[RadioCore] Could not update blocks from DB %s\n", mysql_error(data->dbcon));
    blocks = data->blocks; // dont stop working, return the same pointer
  }

  return(g_list_reverse(blocks));
}




/* Reads a playlist from a playlist file */
GList *read_playlist_file(char *playlist_file_name)
{

  FILE *playlist_file;
  char playlist_line[PLAYLIST_LINE_MAX_SIZE];
  char playlist_data[PLAYLIST_DATA_MAX_SIZE];
  char *playlist_element;
  char *pointer;
  GList *playlist;

  playlist = NULL;

  playlist_file = fopen(playlist_file_name, "r");

  if (playlist_file) {

    while (!feof(playlist_file)) {
      if (fgets(playlist_line, PLAYLIST_LINE_MAX_SIZE, playlist_file) != NULL) {
        
        /* The line is not a comment and the line is not an end of line */
        if ((memcmp(playlist_line, "#", 1) != 0) && (memcmp(playlist_line, "\n", 1)) != 0)
        {

          /* Remove the end of line if it exists */
          pointer = strstr(playlist_line, "\n");
          if (pointer != NULL) {
            memset(pointer, 0, 1);
          }

          /* Define the URI if there is a local path */
          if (memcmp(playlist_line, "/", 1) == 0) {
            sprintf(playlist_data, "file://");
            strcat(playlist_data, playlist_line);
          } else {
            strcpy(playlist_data, playlist_line);
          }

          if ((playlist_element = malloc(PLAYLIST_DATA_MAX_SIZE)) != NULL) {
            strcpy(playlist_element, playlist_data);
            playlist = g_list_prepend(playlist, playlist_element);
          } else {
            printf("%s\n", "Out of memory, unable to add playlist element");
            printf("%s%s\n", "URI: ", playlist_element);
          }

        }
      }
    }

    fclose(playlist_file);

  } else {
    printf("File %s could not be opened\n", playlist_file_name);
  }

  return(g_list_reverse(playlist));
}

/* Reads a playlist from a directory */
GList *read_directory(char *directory)
{

  DIR *dp;
  struct dirent *ep;
  struct stat info;
  char path[BLOCK_DATA_MAX_SIZE];
  char *uri;
  GList *playlist;

  playlist = NULL;

  dp = opendir(directory);
  if (dp != NULL) {
      
    while ((ep = readdir(dp))) {
      strcpy(path, directory);
      
      if (strcmp(path+strlen(path)-1, "/") != 0) {
        strcat(path, "/");
      }

      strcat(path, ep->d_name);
      
      if (lstat(path, &info) < 0) {
        perror(path);
      }

      if (S_ISDIR(info.st_mode) == FALSE) {

        if ((uri = malloc(BLOCK_DATA_MAX_SIZE)) != NULL) {
          strcpy(uri, "file://");
          uri = strcat(uri, path);
          playlist = g_list_prepend(playlist, uri);
        } else {
          printf("%s\n", "Out of memory, unable to add playlist element");
          printf("%s%s\n", "URI: ", uri);
        }

      } else {
        if ((strcmp(path+strlen(path)-3, "/..") != 0) && (strcmp(path+strlen(path)-2, "/.") != 0)) {
          playlist = g_list_concat(playlist, read_directory(path));
        }
      }
      
    }

    (void)closedir(dp);

  } else {
    printf("%s%s%s\n", "Directory ", directory, " cannot be opened");
  }

  return(g_list_reverse(playlist));
}

