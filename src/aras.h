
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

#ifndef ARAS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <math.h>
#include <sys/stat.h>
#include <ctype.h>
#include <gst/gst.h>
#include <stdbool.h>
#include <glib.h>
#include <my_global.h>
#include <mysql.h>

#define CONFIGURATION_LINE_MAX_SIZE 1024
#define CONFIGURATION_VARIABLE_MAX_SIZE 256
#define CONFIGURATION_VALUE_MAX_SIZE 256

#define SCHEDULE_LINE_MAX_SIZE 1024
#define SCHEDULE_DAY_MAX_SIZE 256
#define SCHEDULE_TIME_MAX_SIZE 256
#define SCHEDULE_BLOCK_MAX_SIZE 1024

#define BLOCK_LINE_MAX_SIZE 1024
#define BLOCK_NAME_MAX_SIZE 1024
#define BLOCK_DESC_MAX_SIZE 2048
#define BLOCK_TYPE_MAX_SIZE 256
#define BLOCK_DATA_MAX_SIZE 1024

#define PLAYLIST_LINE_MAX_SIZE 1024
#define PLAYLIST_DATA_MAX_SIZE 1024

#define LOG_MAX_DATE_SIZE 32

#define NO  0
#define YES 1

#define RC_VERSION   "1.2 beta"

#define TIME_SIGNAL_HOUR_MINUTES 59
#define TIME_SIGNAL_HOUR_SECONDS 55
#define TIME_SIGNAL_HALF_MINUTES 29
#define TIME_SIGNAL_HALF_SECONDS 55

/* RadioCloud signals */
#define SIGNALID_ISLIVE 1



struct aras_configuration {
  char schedule_file_name[CONFIGURATION_VALUE_MAX_SIZE];
  char block_file_name[CONFIGURATION_VALUE_MAX_SIZE];
  char log_file_name[CONFIGURATION_VALUE_MAX_SIZE];
  char block_player_output[CONFIGURATION_VALUE_MAX_SIZE];
  char block_player_device[CONFIGURATION_VALUE_MAX_SIZE];
  char time_signal_player_output[CONFIGURATION_VALUE_MAX_SIZE];
  char time_signal_player_device[CONFIGURATION_VALUE_MAX_SIZE];
  char recorder_input[CONFIGURATION_VALUE_MAX_SIZE];
  char recorder_device[CONFIGURATION_VALUE_MAX_SIZE];
  char recorder_quality[CONFIGURATION_VALUE_MAX_SIZE];
  char recorder_storage[CONFIGURATION_VALUE_MAX_SIZE];
  char schedule_support[CONFIGURATION_VALUE_MAX_SIZE];
  char time_signal[CONFIGURATION_VALUE_MAX_SIZE];
  char time_signal_path[CONFIGURATION_VALUE_MAX_SIZE];
  char interval_signal[CONFIGURATION_VALUE_MAX_SIZE];
  char interval_signal_block[CONFIGURATION_VALUE_MAX_SIZE];
  char block_player_volume[CONFIGURATION_VALUE_MAX_SIZE];
  char time_signal_player_volume[CONFIGURATION_VALUE_MAX_SIZE];
  char fade_out_time[CONFIGURATION_VALUE_MAX_SIZE];
  char fade_out_resolution[CONFIGURATION_VALUE_MAX_SIZE];
  char fade_out_slope[CONFIGURATION_VALUE_MAX_SIZE];
  char default_timeout[CONFIGURATION_VALUE_MAX_SIZE];
  char live_timeout[CONFIGURATION_VALUE_MAX_SIZE];
  char time_signal_timeout[CONFIGURATION_VALUE_MAX_SIZE];
  char configuration_timeout[CONFIGURATION_VALUE_MAX_SIZE];
  char db_host[CONFIGURATION_VALUE_MAX_SIZE];
  char db_name[CONFIGURATION_VALUE_MAX_SIZE];
  char db_user[CONFIGURATION_VALUE_MAX_SIZE];
  char db_pass[CONFIGURATION_VALUE_MAX_SIZE];
  char instance[CONFIGURATION_VALUE_MAX_SIZE];
};


struct aras_schedule {
  char schedule_day[SCHEDULE_DAY_MAX_SIZE];
  char schedule_time[SCHEDULE_TIME_MAX_SIZE];
  char schedule_block[SCHEDULE_BLOCK_MAX_SIZE];
};


struct aras_block {
  char block_desc[BLOCK_DESC_MAX_SIZE];
  char block_name[BLOCK_NAME_MAX_SIZE];
  char block_type[BLOCK_TYPE_MAX_SIZE];
  char block_data[BLOCK_DATA_MAX_SIZE];
};


struct aras_player {
  char uri[BLOCK_DATA_MAX_SIZE];
  char desc[BLOCK_DESC_MAX_SIZE];
  double volume;
  gint buffer_percent;

  GstElement *pipeline;

  GstElement *audio_sink_bin;
  GstPad *audio_sink_pad;
  GstPad *audio_sink_ghost_pad;
  GstCaps *audio_caps;
  GstElement *audio_convert;
  GstElement *audio_sink;

  GstElement *video_sink_bin;
  GstPad *video_sink_pad;
  GstPad *video_sink_ghost_pad;
  GstCaps *video_caps;
  GstElement *video_convert;
  GstElement *video_sink;

  GstBus *bus;
};


struct aras_data {
  char *configuration_file_name;
  struct aras_configuration *configuration;
  GList *schedule;
  GList *blocks;
  struct aras_player *block_player;
  struct aras_player *time_signal_player;
  GList *playlist;
  GList *current_playlist_element;
  GMutex *configuration_mutex;
  GMutex *log_file_mutex;
  GMainLoop *loop;
  int is_live;
  int live_active;
  MYSQL *dbcon;
  int last_jingle_id;
};


gboolean time_signal_bus_call (GstBus *bus,GstMessage *msg,gpointer pipeline);
gboolean block_bus_call (GstBus *bus,GstMessage *msg,gpointer pipeline);

struct aras_configuration *read_configuration(struct aras_data *data);
GList *read_schedule(struct aras_data *data);
GList *read_blocks(struct aras_data *data);
GList *read_directory(char *directory);
GList *read_playlist_file(char *playlist_file_name);

struct aras_configuration *free_configuration(struct aras_data *data);
GList *free_schedule(struct aras_data *data);
GList *free_blocks(struct aras_data *data);
GList *free_playlist(GList *playlist);

float mod(float x, float y);
float get_current_week_time();
char *get_current_time();
void log_printf(struct aras_data *data, char *string);
void play_uri(gchar *uri, GstElement *pipeline);

void init_block_player(struct aras_data *data);
void init_time_signal_player(struct aras_data *data);

GList *get_playlist(struct aras_data *data, char *schedule_block);
GList *shuffle_playlist(GList *playlist);

struct aras_schedule *get_current_schedule_element(struct aras_data *data);
struct aras_schedule *get_next_schedule_element(struct aras_data *data);
float get_schedule_element_week_time(struct aras_schedule *schedule_element);
void fade_out(struct aras_data *data);

void configuration_support(struct aras_data *data);
void time_signal_support(struct aras_data *data);
void schedule_support(struct aras_data *data);
void connect_database(struct aras_data *data);
void disconnect_database(struct aras_data *data);

void live_signal_support(struct aras_data *data);
void db_live_signal_status(struct aras_data *data);

/* log.c */

void log_db(struct aras_data *data, char *info, char *type);
void player_log(struct aras_data *data, char *info);

/* support.c */

gboolean configuration_support_function(struct aras_data *data);
gboolean time_signal_support_function(struct aras_data *data);
gboolean live_signal_support_function(struct aras_data *data);
gboolean schedule_support_function(struct aras_data *data);

int jingleman_enabled(struct aras_data *data);
int jingleman_get_next(struct aras_data *data, char*, int *prob);
int jingleman_is_more(struct aras_data *data);
#endif

