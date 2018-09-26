
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


int get_day(char *day)
{
  if (strcasecmp(day, "Sunday") == 0)
    return(0);
  else if (strcasecmp(day, "Monday") == 0)
    return(1);
  else if (strcasecmp(day, "Tuesday") == 0)
    return(2);
  else if (strcasecmp(day, "Wednesday") == 0)
    return(3);
  else if (strcasecmp(day, "Thursday") == 0)
    return(4);
  else if (strcasecmp(day, "Friday") == 0)
    return(5);
  else if (strcasecmp(day, "Saturday") == 0)
    return(6);
  else
    return(0);
}


int get_week_time(struct tm *date)
{
  return(date->tm_wday*24*3600 + date->tm_hour*3600 + date->tm_min*60 + date->tm_sec);
}


char *get_block_type(struct aras_data *data, char *block_name)
{
  GList *pointer;
  struct aras_block *block;

  pointer = data->blocks;
  while (pointer != NULL) {
    block = pointer->data;

    if (strcasecmp(block->block_name, block_name) == 0)
      return(block->block_type);

    pointer = pointer->next;
  }
  return(NULL);
}


char *get_block_data(struct aras_data *data, char *block_name)
{
  GList *pointer;
  struct aras_block *block;

  pointer = data->blocks;
  while (pointer != NULL) {
    block = pointer->data;

    if (strcasecmp(block->block_name, block_name) == 0)
      return(block->block_data);

    pointer = pointer->next;
  }
  return(NULL);
}


char *get_current_time()
{
  static char current_time[LOG_MAX_DATE_SIZE];

  struct timeval tv;
  struct timezone tz;
  struct tm *current_date;
  
  gettimeofday(&tv, &tz);
  current_date = localtime((time_t*)&tv);

  sprintf(current_time, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d ", 1900 + current_date->tm_year, 1 + current_date->tm_mon, current_date->tm_mday, current_date->tm_hour, current_date->tm_min, current_date->tm_sec);

  return(current_time);
}


float get_current_week_time()
{
  struct timeval tv;
  struct timezone tz;
  struct tm *current_date;
  float current_week_time;
  
  gettimeofday(&tv, &tz);
  current_date = localtime((time_t*)&tv);

  current_week_time = current_date->tm_wday*24*3600 + current_date->tm_hour*3600 + current_date->tm_min*60 + current_date->tm_sec + tv.tv_usec*1E-6;

  return(current_week_time);
}


float get_schedule_element_week_time(struct aras_schedule *schedule_element)
{
  static struct tm schedule_element_date;
  float schedule_element_week_time;

  schedule_element_date.tm_wday = get_day(schedule_element->schedule_day);
  sscanf(schedule_element->schedule_time, "%d:%d:%d", &schedule_element_date.tm_hour, &schedule_element_date.tm_min, &schedule_element_date.tm_sec);
  schedule_element_week_time = schedule_element_date.tm_wday*24*3600 + schedule_element_date.tm_hour*3600 + schedule_element_date.tm_min*60 + schedule_element_date.tm_sec;

  return(schedule_element_week_time);
}


float mod(float x, float y)
{
  float result;

  if (y != 0)
    result = x-y*floor(x/y);
  else
    result = x;

  return(result);
}


struct aras_schedule *get_current_schedule_element(struct aras_data *data)
{
  GList *pointer;
  GList *current_schedule_element;

  pointer = data->schedule;

  current_schedule_element = pointer;
  while (pointer != NULL) {

    if (mod(get_current_week_time() - get_schedule_element_week_time(pointer->data), 7*24*3600) < mod(get_current_week_time() - get_schedule_element_week_time(current_schedule_element->data), 7*24*3600))
      current_schedule_element = pointer;

    pointer = pointer->next;
  }
  return(current_schedule_element->data);
}


struct aras_schedule *get_next_schedule_element(struct aras_data *data)
{
  GList *pointer;
  GList *next_schedule_element;

  pointer = data->schedule;

  next_schedule_element = pointer;
  while (pointer != NULL) {

    if (mod(get_schedule_element_week_time(pointer->data) - get_current_week_time(), 7*24*3600) < mod(get_schedule_element_week_time(next_schedule_element->data) - get_current_week_time(), 7*24*3600))
      next_schedule_element = pointer;

    pointer = pointer->next;
  }
  return(next_schedule_element->data);
}


GList *shuffle_playlist(GList *playlist)
{
  struct timeval time;
  int n;
  int i;
  int j;
  GList *pointer_1;
  GList *pointer_2;
  char *aux;

  if (playlist != NULL) {
    gettimeofday(&time,NULL);
    srand((time.tv_sec*1000) + (time.tv_usec/1000));
    n = g_list_length(playlist);
    for (i = n-1; i > 0; i--) {
      j = rand() % (i+1);
      pointer_1 = g_list_nth(playlist, i);
      pointer_2 = g_list_nth(playlist, j);
      if ((pointer_1 != NULL) && (pointer_2 != NULL)) {
        aux = pointer_1->data;
        pointer_1->data = pointer_2->data;
        pointer_2->data = aux;
      }
    }
  }
  return(playlist);
}


GList *get_interleave_block(struct aras_data *data, char *block_data)
{
  char block_name_1[BLOCK_DATA_MAX_SIZE];
  char block_name_2[BLOCK_DATA_MAX_SIZE];
  int n_1;
  int n_2;
  char *block_type_1;
  char *block_type_2;
  GList *playlist_1;
  GList *pointer_1;
  GList *playlist_2;
  GList *pointer_2;
  int i;
  GList *playlist;

  playlist = NULL;

  sscanf(block_data, "%s%s%d%d", block_name_1, block_name_2, &n_1, &n_2);
  n_1 = fmax(n_1, 1);
  n_2 = fmax(n_2, 1);

  block_type_1 = get_block_type(data, block_name_1);
  block_type_2 = get_block_type(data, block_name_2);

  if ((block_type_1 != NULL) && (block_type_2 != NULL)) {
    if ((strcasecmp(block_type_1, "Interleave") != 0) && (strcasecmp(block_type_2, "Interleave") != 0)) {

      playlist_1 = get_playlist(data, block_name_1);
      playlist_2 = get_playlist(data, block_name_2);

      pointer_1 = playlist_1;
      pointer_2 = playlist_2;

      while ((pointer_1 != NULL) || (pointer_2 != NULL)) {

        if (pointer_1 != NULL) {
          for (i = 1; i <= n_1; i++) {
            if (pointer_1 != NULL) {
              playlist = g_list_prepend(playlist, pointer_1->data);
              pointer_1 = pointer_1->next;
            }
          }
        }

        if (pointer_2 != NULL) {
          for (i = 1; i <= n_2; i++) {
            if (pointer_2 != NULL) {
              playlist = g_list_prepend(playlist, pointer_2->data);
              pointer_2 = pointer_2->next;
            }
          }
        }

      }

      g_list_free(playlist_1);
      g_list_free(playlist_2);

    }
  }
  return(g_list_reverse(playlist));
}


/* Load the playlist (a single file, a playlist file, files from a directory or a composite block) of an arbitrary schedule element */
GList *get_playlist(struct aras_data *data, char *schedule_block)
{
  GList *playlist;
  char *block_type;
  char *block_data;
  char *uri;

  playlist = NULL;

  block_type = get_block_type(data, schedule_block);
  block_data = get_block_data(data, schedule_block);
 
  if ((block_type != NULL)&&(block_data != NULL)) {

    if (strcasecmp(block_type, "File") == 0) {
      /* File block */
      if ((uri = malloc(BLOCK_DATA_MAX_SIZE)) != NULL) {
        strcpy(uri, block_data);
        playlist = g_list_prepend(playlist, uri);
      } else {
        printf("%s\n", "Out of memory, unable to add file");
        printf("%s%s\n", "URI: ", block_data);
      }
    } else if (strcasecmp(block_type, "Playlist") == 0) {
      /* Playlist block */
      playlist = read_playlist_file(block_data);
    } else if (strcasecmp(block_type, "Random") == 0) {
      /* Random block */
      playlist = read_directory(block_data);
      playlist = shuffle_playlist(playlist);
    } else if (strcasecmp(block_type, "RandomFile") == 0) {
      /* Randomfile block */
      playlist = read_directory(block_data);
      playlist = shuffle_playlist(playlist);
      if (playlist != NULL) {
        playlist->next = free_playlist(playlist->next);
      }
    } else if (strcasecmp(block_type, "Interleave") == 0) {
      /* Interleave block */
      playlist = get_interleave_block(data, block_data);
    }

  }
  return(playlist);
}


void block_player_support(struct aras_data *data)
{
  GstState state;
  char playerlog_buffer[1024];
  gst_element_get_state(data->block_player->pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
  char jingleplay[1024];
  int prob;
  if (state == GST_STATE_NULL) {

    /* live signal received */


    if (data->playlist != NULL) {


          if (data->current_playlist_element == NULL) {
            data->current_playlist_element = data->playlist;
          } else {
            if (!jingleman_is_more(data) && (g_list_length(data->current_playlist_element) > 1))
                data->current_playlist_element = data->current_playlist_element->next;
          }
        

      if (data->current_playlist_element != NULL) {
       /* Probak */
       data->block_player->volume = atof(data->configuration->block_player_volume);
       g_object_set(G_OBJECT(data->block_player->pipeline), "volume", data->block_player->volume, NULL);
      
       if (jingleman_enabled(data) && jingleman_is_more(data)) // want to play jingles
       {
        
         if (jingleman_get_next(data, jingleplay, &prob)){
          int probability = ((rand()%100) % prob);
          if (probability == 0) {
             play_uri(jingleplay, data->block_player->pipeline);
             log_printf(data, get_current_time());
             log_printf(data, "Playing jingle: ");
             log_printf(data, jingleplay);
             log_printf(data, "\n");
           }
        }
       
        /* Play the file */
       } else
      {  
        /*if (data->current_playlist_element != NULL)
            data->current_playlist_element = data->current_playlist_element->next;
          */ 
         if (data->current_playlist_element->data != NULL) {

          data->block_player->volume = atof(data->configuration->block_player_volume);
          g_object_set(G_OBJECT(data->block_player->pipeline), "volume", data->block_player->volume, NULL);

          strcpy(data->block_player->uri, data->current_playlist_element->data);
          play_uri(data->block_player->uri, data->block_player->pipeline);

          g_mutex_lock(data->log_file_mutex);
          log_printf(data, get_current_time());
          log_printf(data, "Playing: ");
          log_printf(data, data->current_playlist_element->data);
          log_printf(data, "\n");
         
	      sprintf(playerlog_buffer, "%s",  (char*)data->current_playlist_element->data);

	      player_log(data, playerlog_buffer);
          g_mutex_unlock(data->log_file_mutex);
          data->last_jingle_id = 0;
        
          if ((g_list_length(data->current_playlist_element) == 1))
          {
            // Reset after playing 1 element after jingles
            data->playlist = free_playlist(data->playlist);
            data->current_playlist_element = NULL;
          }
        } else {
          data->current_playlist_element = data->current_playlist_element->next;
          printf("%s\n", "Invalid data in playlist");
        }

       }
      } else {
        data->playlist = free_playlist(data->playlist);
        data->current_playlist_element = NULL;
      }
     

    } else {

      if (strcasecmp(data->configuration->interval_signal, "On") == 0) {

        /* Get the interval signal playlist */
        data->playlist = get_playlist(data, data->configuration->interval_signal_block);
        /* Set up the current playlist element */
        data->current_playlist_element = NULL;

        if (data->playlist != NULL) {
          g_mutex_lock(data->log_file_mutex);
          log_printf(data, get_current_time());
          log_printf(data, "Loading interval signal block: ");
          log_printf(data, data->configuration->interval_signal_block);
          log_printf(data, "\n");
          g_mutex_unlock(data->log_file_mutex);
        }

      }

    }

  }

}


void fade_out(struct aras_data *data)
{
  struct aras_schedule next_schedule_element;
  struct aras_schedule *next_schedule_element_aux;
  float next_schedule_element_timeout;

  float fade_out_time_count;
  float fade_out_time;
  float fade_out_resolution;

  /* Remember the next schedule element */
  next_schedule_element_aux = get_next_schedule_element(data);

  if (next_schedule_element_aux != NULL) {

    strcpy(next_schedule_element.schedule_day, next_schedule_element_aux->schedule_day);
    strcpy(next_schedule_element.schedule_time, next_schedule_element_aux->schedule_time);
    strcpy(next_schedule_element.schedule_block, next_schedule_element_aux->schedule_block);

    /* Fade out time adjustment */
    next_schedule_element_timeout = mod(get_schedule_element_week_time(&next_schedule_element)-get_current_week_time(), 7*24*3600);
    fade_out_time = next_schedule_element_timeout*1E3;

    /* Fade out initial volume */
    g_object_get(G_OBJECT(data->block_player->pipeline), "volume", &(data->block_player->volume), NULL);

    fade_out_time_count = 0;
    fade_out_resolution = atof(data->configuration->fade_out_resolution);

    while (fade_out_time_count <= fade_out_time) {
      data->block_player->volume = data->block_player->volume*(1-atof(data->configuration->fade_out_slope));
      g_object_set(G_OBJECT(data->block_player->pipeline), "volume", data->block_player->volume, NULL);
      fade_out_time_count = fade_out_time_count + fade_out_resolution;
      //g_mutex_unlock(data->configuration_mutex);
      usleep(fade_out_resolution*1E3);
      //g_mutex_lock(data->configuration_mutex);
    }

    /* Finish the fade out and get the next schedule element playlist if exists */

    gst_element_set_state(GST_ELEMENT(data->block_player->pipeline), GST_STATE_NULL);
    data->block_player->volume = atof(data->configuration->block_player_volume);
    g_object_set(G_OBJECT(data->block_player->pipeline), "volume", data->block_player->volume, NULL);

    data->playlist = free_playlist(data->playlist);
    data->current_playlist_element = NULL;

    data->playlist = get_playlist(data, next_schedule_element.schedule_block);

    /* Write log entry */
    g_mutex_lock(data->log_file_mutex);
    log_printf(data, get_current_time());
    log_printf(data, "Loading block: ");
    log_printf(data, next_schedule_element.schedule_block);
    log_printf(data, "\n");
    g_mutex_unlock(data->log_file_mutex);

  }

}


void countdown(struct aras_data *data)
{
  struct aras_schedule next_schedule_element;
  struct aras_schedule *next_schedule_element_aux;
  float next_schedule_element_timeout;
  float countdown_time_count;
  float countdown_time;
  float countdown_resolution;

  /* Remember the next schedule element */
  next_schedule_element_aux = get_next_schedule_element(data);

  if (next_schedule_element_aux != NULL) {

    strcpy(next_schedule_element.schedule_day, next_schedule_element_aux->schedule_day);
    strcpy(next_schedule_element.schedule_time, next_schedule_element_aux->schedule_time);
    strcpy(next_schedule_element.schedule_block, next_schedule_element_aux->schedule_block);

    /* Countdown time adjustment */
    next_schedule_element_timeout = mod(get_schedule_element_week_time(&next_schedule_element)-get_current_week_time(), 7*24*3600);
    countdown_time = next_schedule_element_timeout*1E3;

    countdown_time_count = 0;
    countdown_resolution = atof(data->configuration->fade_out_resolution);

    while (countdown_time_count <= countdown_time) {
      countdown_time_count = countdown_time_count + countdown_resolution;
      //g_mutex_unlock(data->configuration_mutex);
      usleep(countdown_resolution*1E3);
      //g_mutex_lock(data->configuration_mutex);
    }

    /* Finish the countdown and get the next schedule element playlist if exists */

    data->playlist = free_playlist(data->playlist);
    data->current_playlist_element = NULL;
    data->playlist = get_playlist(data, next_schedule_element.schedule_block);

    /* Write log entry */
    g_mutex_lock(data->log_file_mutex);
    log_printf(data, get_current_time());
    log_printf(data, "Loading block: ");
    log_printf(data, next_schedule_element.schedule_block);
    log_printf(data, "\n");
    g_mutex_unlock(data->log_file_mutex);

  }

}


void schedule_support(struct aras_data *data)
{
  struct aras_schedule *next_schedule_element;
  float next_schedule_element_timeout;
  float fade_out_time;


  /* Update the player if neccessary */
   block_player_support(data);

  /* Look for the next block */
  next_schedule_element = get_next_schedule_element(data);

  if (next_schedule_element != NULL) {

    /* Next schedule element exists, let's get its timeout */
    next_schedule_element_timeout = mod(get_schedule_element_week_time(next_schedule_element)-get_current_week_time(), 7*24*3600);

    /* Get fade out time */
    fade_out_time = atof(data->configuration->fade_out_time)*1E-3;

    /* Timeout display (debug) */

    /*
    printf("%f\n", next_schedule_element_timeout);
    printf("%f\n", atof(data->configuration->fade_out_time)*1E-3);
    */

    if (next_schedule_element_timeout < fade_out_time) {
      if (strcasecmp(data->configuration->schedule_support, "Hard") == 0)
        fade_out(data);
      else
        countdown(data);
    }

  
      
  }

/* Playlist report (debug) */

/*
  GList *pointer;
  pointer = data->playlist;
  printf("PLAYLIST REPORT\n");
  while (pointer != NULL)
  {
    char *pe;
    pe = pointer->data;
    printf("Elemento: %s\n", pe);
    pointer = pointer->next;
  }
  printf("END OF REPORT\n");
*/

}

