/*
    RadioCloud: RadioCore player
    2017 Aritz Olea Zubikarai <aritzolea@gmail.com>

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



void configuration_support(struct aras_data *data)
{
  if (data->configuration != NULL) {
    data->configuration = read_configuration(data);
  }

  if (data->schedule != NULL) {
    data->schedule = free_schedule(data);
    data->schedule = read_schedule(data);
  }

  if (data->blocks != NULL) {
    data->blocks = free_blocks(data);
    data->blocks = read_blocks(data);
  }
}


void live_signal_support(struct aras_data *data)
{
  char block_name[BLOCK_DATA_MAX_SIZE];
  db_live_signal_status(data);

  if (!data->live_active && data->is_live)
  {  // stop everything and add live signal to pipeline
        printf("Playing live stream");
        sprintf(block_name, "Live");
        data->current_playlist_element = get_playlist(data, block_name);
        
        data->live_active = 1;
        strcpy(data->block_player->uri, data->current_playlist_element->data);
        data->block_player->volume = atof(data->configuration->block_player_volume);
        g_object_set(G_OBJECT(data->block_player->pipeline), "volume", data->block_player->volume, NULL);
        play_uri(data->block_player->uri, data->block_player->pipeline);
     
  } 
  if (data->live_active && !data->is_live)
  {
        printf("Stopping live stream");
        data->current_playlist_element = get_playlist(data, data->configuration->interval_signal_block);;
        data->block_player->volume = atof(data->configuration->block_player_volume);
        g_object_set(G_OBJECT(data->block_player->pipeline), "volume", data->block_player->volume, NULL);
        strcpy(data->block_player->uri, data->current_playlist_element->data);
        play_uri(data->block_player->uri, data->block_player->pipeline);
        data->live_active = 0;
  }
  

}

void time_signal_support(struct aras_data *data)
{
  struct timeval tv;
  struct timezone tz;
  struct tm *date;
  char current_signal_file[100];
  char current_hour_string[5];
  GstState time_signal_player_state;
  
  gettimeofday(&tv, &tz);
  date = localtime((time_t*)&tv);

  if (strcasecmp(data->configuration->time_signal, "hour") == 0 || strcasecmp(data->configuration->time_signal, "half") == 0) {
    if ((date->tm_min == TIME_SIGNAL_HOUR_MINUTES) && (date->tm_sec == TIME_SIGNAL_HOUR_SECONDS)) {

      gst_element_get_state(data->time_signal_player->pipeline, &time_signal_player_state, NULL, GST_CLOCK_TIME_NONE);

      if (time_signal_player_state != GST_STATE_PLAYING) {
        data->time_signal_player->volume = atof(data->configuration->time_signal_player_volume);
        g_object_set(G_OBJECT(data->time_signal_player->pipeline), "volume", data->time_signal_player->volume, NULL);
        strcpy(current_signal_file, data->configuration->time_signal_path);
        sprintf(current_signal_file, "%s/%d", current_signal_file, date->tm_hour+1);
        strcpy(data->time_signal_player->uri, current_signal_file);

        log_printf(data, "Hour");
        log_printf(data, current_signal_file);
        play_uri(data->time_signal_player->uri, data->time_signal_player->pipeline);
      }

    }
  }
    
  if (strcasecmp(data->configuration->time_signal, "half") == 0) {
    if ((date->tm_min == TIME_SIGNAL_HALF_MINUTES) && (date->tm_sec == TIME_SIGNAL_HALF_SECONDS)) {

      gst_element_get_state(data->time_signal_player->pipeline, &time_signal_player_state, NULL, GST_CLOCK_TIME_NONE);

      if (time_signal_player_state != GST_STATE_PLAYING) {
        data->time_signal_player->volume = atof(data->configuration->time_signal_player_volume);
        g_object_set(G_OBJECT(data->time_signal_player->pipeline), "volume", data->time_signal_player->volume, NULL);
        strcpy(data->time_signal_player->uri, data->configuration->time_signal_path);
        play_uri(data->time_signal_player->uri, data->time_signal_player->pipeline);
      }

    }
  }
}



gboolean configuration_support_function(struct aras_data *data)
{
  if (!data->is_live) {
  g_mutex_lock(data->configuration_mutex);
  configuration_support(data);
  g_mutex_unlock(data->configuration_mutex);
  }
  return((gboolean)TRUE);
}


gboolean time_signal_support_function(struct aras_data *data)
{
  if (!data->is_live) {
    g_mutex_lock(data->configuration_mutex);
    time_signal_support(data);
    g_mutex_unlock(data->configuration_mutex); 
  }
  return((gboolean)TRUE);
}

gboolean live_signal_support_function(struct aras_data *data)
{
  g_mutex_lock(data->configuration_mutex);
  live_signal_support(data);
  g_mutex_unlock(data->configuration_mutex);
  return((gboolean)TRUE);
}


gboolean schedule_support_function(struct aras_data *data)
{
  if (!data->is_live) {
    g_mutex_lock(data->configuration_mutex);
    schedule_support(data);
    g_mutex_unlock(data->configuration_mutex);
  }
  return((gboolean)TRUE);
}

int jingleman_enabled(struct aras_data *data) {
 MYSQL_RES *res;
 MYSQL_ROW row;
 char buffer[1024];
 sprintf(buffer, "SELECT *  FROM jingles WHERE type='end' OR type='endp' AND enabled=1");

   if (!mysql_query(data->dbcon, buffer)) {

    res = mysql_store_result(data->dbcon);

    if ((row = mysql_fetch_row(res)) != NULL) {
     mysql_free_result(res);
     return 1;
    }
   }

 return 0;
}

int jingleman_is_more(struct aras_data *data) {
 MYSQL_RES *res;
 MYSQL_ROW row;
 char buffer[1024];
 sprintf(buffer, "SELECT *  FROM jingles WHERE type='end' OR type='endp' AND enabled=1 AND id > %d", data->last_jingle_id);

   if (!mysql_query(data->dbcon, buffer)) {

    res = mysql_store_result(data->dbcon);

    if ((row = mysql_fetch_row(res)) != NULL) {
     mysql_free_result(res);
     return 1;
    }
   }

 return 0;
}

int jingleman_get_next(struct aras_data *data, char *destfile, int *prob)
{
 MYSQL_RES *res;
 MYSQL_ROW row;
 char buffer[1024];
 char bufferdir[1024];
 GList *playlist = NULL;

 char radiocore_dir[1024];
 sprintf(buffer, "SELECT *  FROM jingles WHERE type='end' OR type='endp' AND enabled=1 AND id > %d LIMIT 1", data->last_jingle_id);

 if (!mysql_query(data->dbcon, "SELECT dirpath FROM dirs WHERE dirname='radiocore_dir' LIMIT 1")) {
    res = mysql_store_result(data->dbcon);
     if ((row = mysql_fetch_row(res)) != NULL) {
       sprintf(radiocore_dir, "%s", row[0]);
     }
   mysql_free_result(res);
  }
 
   if (!mysql_query(data->dbcon, buffer)) {

    res = mysql_store_result(data->dbcon);

    if ((row = mysql_fetch_row(res)) != NULL) {
     data->last_jingle_id = atoi(row[0]);
     sprintf(buffer, "file://%s/%s", radiocore_dir, row[4]);
     sprintf(bufferdir, "/%s/%s", radiocore_dir, row[4]);
     *prob = atoi(row[5]);
     if (!strcasecmp(row[3], "File")) {
        playlist = g_list_prepend(playlist, buffer);
     } else if (!strcasecmp(row[3], "RandomFile")) { 
        playlist = read_directory(bufferdir);
        playlist = shuffle_playlist(playlist);
     }
     
     strcpy(destfile, playlist->data);
     mysql_free_result(res);

     return 1;
    }
   }

 return 0;
}
