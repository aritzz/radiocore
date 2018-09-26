
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


/*
    If you want to learn more about ARAS please visit ARAS website or write an email to the author:
    ARAS - A Radio Automation System <http://aras.sourceforge.net>
    Erasmo Alonso Iglesias <erasmo1982@users.sourceforge.net>
    
    Compile ARAS Daemon as follows:
    gcc -Wall main.c read_data.c free_data.c aras.c player.c -o ../bin/aras-daemon $(pkg-config --cflags --libs gstreamer-1.0) -lm
*/


#include "aras.h"


gint main(gint argc, gchar *argv[])
{
  struct aras_data *data;

  if (argc == 2) {

    data = (struct aras_data*)malloc(sizeof(struct aras_data));

    /* Load configuration file name */
    data->configuration_file_name = argv[1];
    data->live_active = 0;
    data->is_live = 0;
    data->last_jingle_id = 0;
    /* Create configuration */
    data->configuration = (struct aras_configuration*)malloc(sizeof(struct aras_configuration));
    data->configuration = read_configuration(data);
    connect_database(data);
    log_db(data, "Init completed, starting main loop", "info");

    if (data->configuration != NULL) {

      data->schedule = read_schedule(data);

      if (data->schedule != NULL) {

        data->blocks = read_blocks(data);

        if (data->blocks != NULL) {

          /* Loop initialization */
          data->loop = g_main_loop_new(NULL, FALSE);

          /* GStreamer initialization */
          gst_init(&argc, &argv);

          /* Block player initialization */
          init_block_player(data);
          gst_element_set_state(GST_ELEMENT(data->block_player->pipeline), GST_STATE_NULL);

          /* Time signal player initialization */
          init_time_signal_player(data);
          gst_element_set_state(GST_ELEMENT(data->time_signal_player->pipeline), GST_STATE_NULL);

          /* Playlist initialization */
          data->playlist = NULL;
          data->current_playlist_element = NULL;

          /* Create data->configuration_mutex */
          data->configuration_mutex = malloc(sizeof(GStaticMutex));
          g_mutex_init(data->configuration_mutex);
//          g_mutex_unlock(data->configuration_mutex);

          /* Create data->log_file_mutex */
          data->log_file_mutex = malloc(sizeof(GStaticMutex));
          g_mutex_init(data->log_file_mutex);
//          g_mutex_unlock(data->log_file_mutex);

          /* Run the functions */
          g_timeout_add(atoi(data->configuration->configuration_timeout), (GSourceFunc)configuration_support_function, data);
          g_timeout_add(atoi(data->configuration->default_timeout), (GSourceFunc)schedule_support_function, data);
          g_timeout_add(atoi(data->configuration->time_signal_timeout), (GSourceFunc)time_signal_support_function, data);
          g_timeout_add(atoi(data->configuration->live_timeout), (GSourceFunc)live_signal_support_function, data);

          printf("[RadioCore] Running...\n");

          /* Run main loop */
          g_main_loop_run(data->loop);

          printf("[RadioCore] Closing...\n");
          log_db(data, "Loop finished. Closing application", "info");

          disconnect_database(data);

          /* Clean up */
          gst_element_set_state(data->block_player->pipeline, GST_STATE_NULL);
          gst_object_unref(GST_OBJECT(data->block_player->pipeline));

          g_mutex_unlock(data->configuration_mutex);
          g_mutex_clear(data->configuration_mutex);
          g_mutex_unlock(data->log_file_mutex);
          g_mutex_clear(data->log_file_mutex);

        } else {
          printf("[RadioCore] Block definition file could not be read\n");
        }

      } else {
        printf("[RadioCore] Schedule file could not be read\n");
      }

    } else {
      printf("[RadioCore] Configuration file could not be read\n");
    }
  } else {
    printf("RadioCore v-%s\n", RC_VERSION);
    printf("Usage: %s <configuration file>\n", argv[0]);
  }
  exit(0);
}

