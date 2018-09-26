
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


gboolean block_bus_call(GstBus *bus, GstMessage *msg, gpointer user_data)
{
  struct aras_data *data;

  data = (struct aras_data*)user_data;

  switch (GST_MESSAGE_TYPE(msg)) {

    case GST_MESSAGE_EOS:
      gst_element_set_state(GST_ELEMENT(data->block_player->pipeline), GST_STATE_NULL);
      break;

    case GST_MESSAGE_ERROR:
      gst_element_set_state(GST_ELEMENT(data->block_player->pipeline), GST_STATE_NULL);
      break;

    case GST_MESSAGE_BUFFERING:
      gst_message_parse_buffering(msg, &data->block_player->buffer_percent);
      if (data->block_player->buffer_percent < 100) {
        gst_element_set_state(GST_ELEMENT(data->block_player->pipeline), GST_STATE_PAUSED);
      } else {
        gst_element_set_state(GST_ELEMENT(data->block_player->pipeline), GST_STATE_PLAYING);
      }
      break;

    default:
      break;
  }
  return((gboolean)TRUE);
}


gboolean time_signal_bus_call(GstBus *bus, GstMessage *msg, gpointer user_data)
{
  struct aras_data *data;

  data = (struct aras_data*)user_data;

  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
      gst_element_set_state(GST_ELEMENT(data->time_signal_player->pipeline), GST_STATE_NULL);
      break;
    case GST_MESSAGE_ERROR:
      gst_element_set_state(GST_ELEMENT(data->time_signal_player->pipeline), GST_STATE_NULL);
      break;
    case GST_MESSAGE_BUFFERING:
      gst_message_parse_buffering(msg, &data->time_signal_player->buffer_percent);
      if (data->time_signal_player->buffer_percent < 100) {
        gst_element_set_state(GST_ELEMENT(data->time_signal_player->pipeline), GST_STATE_PAUSED);
      } else {
        gst_element_set_state(GST_ELEMENT(data->time_signal_player->pipeline), GST_STATE_PLAYING);
      }
      break;

    default:
      break;
  }
  return((gboolean)TRUE);
}


void get_block_player_audio_sink(struct aras_data *data)
{
  if (strcasecmp(data->configuration->block_player_output, "autoaudiosink") == 0) {
    data->block_player->audio_sink = gst_element_factory_make("autoaudiosink", "block_player");
  } else if (strcasecmp(data->configuration->block_player_output, "pulsesink") == 0) {
    data->block_player->audio_sink = gst_element_factory_make("pulsesink", "block_player");
    g_object_set(G_OBJECT(data->block_player->audio_sink), "device", data->configuration->block_player_device, NULL);
  } else if (strcasecmp(data->configuration->block_player_output, "alsasink") == 0) {
    data->block_player->audio_sink = gst_element_factory_make("alsasink", "block_player");
    g_object_set(G_OBJECT(data->block_player->audio_sink), "device", data->configuration->block_player_device, NULL);
  } else if (strcasecmp(data->configuration->block_player_output, "osssink") == 0) {
    data->block_player->audio_sink = gst_element_factory_make("osssink", "block_player");
    g_object_set(G_OBJECT(data->block_player->audio_sink), "device", data->configuration->block_player_device, NULL);
  } else if (strcasecmp(data->configuration->block_player_output, "oss4sink") == 0) {
    data->block_player->audio_sink = gst_element_factory_make("oss4sink", "block_player");
    g_object_set(G_OBJECT(data->block_player->audio_sink), "device", data->configuration->block_player_device, NULL);
  } else if (strcasecmp(data->configuration->block_player_output, "jackaudiosink") == 0) {
    data->block_player->audio_sink = gst_element_factory_make("jackaudiosink", "block_player");
    g_object_set(G_OBJECT(data->block_player->audio_sink), "server", data->configuration->block_player_device, NULL);
    g_object_set(G_OBJECT(data->block_player->audio_sink), "connect", 0, NULL);
  } else if (strcasecmp(data->configuration->block_player_output, "filesink") == 0) {
    data->block_player->audio_sink = gst_element_factory_make("filesink", "block_player");
    g_object_set(G_OBJECT(data->block_player->audio_sink), "buffer-mode", 1, NULL);
    g_object_set(G_OBJECT(data->block_player->audio_sink), "buffer-size",1000000, NULL);
    g_object_set(G_OBJECT(data->block_player->audio_sink), "location", data->configuration->block_player_device, NULL);
  } else {
    data->block_player->audio_sink = gst_element_factory_make("autoaudiosink", "block_player");
  }
}


void init_block_player(struct aras_data *data)
{

  if ((data->block_player = (struct aras_player*)malloc(sizeof(struct aras_player))) != NULL) {

    /* Playbin */
    data->block_player->pipeline = gst_element_factory_make("playbin", "play");

    /* Audio convert */
    data->block_player->audio_convert = gst_element_factory_make("audioconvert", "audioconvert");

    /* Audio sink */
    get_block_player_audio_sink(data);

    /* Audio convert and audio sink bin */
    data->block_player->audio_sink_bin = gst_bin_new("audio_sink_bin");
    gst_bin_add_many(GST_BIN(data->block_player->audio_sink_bin), data->block_player->audio_convert, data->block_player->audio_sink, NULL);

    data->block_player->audio_caps = gst_caps_new_simple("audio/x-raw", "channels", G_TYPE_INT, 2, NULL);
    gst_element_link_filtered(data->block_player->audio_convert, data->block_player->audio_sink, data->block_player->audio_caps);
    gst_caps_unref(data->block_player->audio_caps);

    data->block_player->audio_sink_pad = gst_element_get_static_pad(data->block_player->audio_convert, "sink");
    data->block_player->audio_sink_ghost_pad = gst_ghost_pad_new("sink", data->block_player->audio_sink_pad);
    gst_pad_set_active(data->block_player->audio_sink_ghost_pad, TRUE);
    gst_element_add_pad (data->block_player->audio_sink_bin, data->block_player->audio_sink_ghost_pad);
    gst_object_unref(data->block_player->audio_sink_pad);

    /* Video convert */
    data->block_player->video_convert = gst_element_factory_make("videoconvert", "videoconvert");

    /* Video sink */
    data->block_player->video_sink = gst_element_factory_make("autovideosink", "block_player");

    /* Video sink bin */
    data->block_player->video_sink_bin = gst_bin_new("video_sink_bin");
    gst_bin_add_many(GST_BIN(data->block_player->video_sink_bin), data->block_player->video_convert, data->block_player->video_sink, NULL);

    data->block_player->video_caps = gst_caps_new_simple("video/x-raw", "width", G_TYPE_INT, 1920, "height", G_TYPE_INT, 1080, "format", G_TYPE_STRING, "I420", NULL);
    gst_element_link_filtered(data->block_player->video_convert, data->block_player->video_sink, data->block_player->video_caps);
    gst_caps_unref(data->block_player->video_caps);

    data->block_player->video_sink_pad = gst_element_get_static_pad(data->block_player->video_convert, "sink");
    data->block_player->video_sink_ghost_pad = gst_ghost_pad_new("sink", data->block_player->video_sink_pad);
    gst_pad_set_active(data->block_player->video_sink_ghost_pad, TRUE);
    gst_element_add_pad(data->block_player->video_sink_bin, data->block_player->video_sink_ghost_pad);
    gst_object_unref(data->block_player->video_sink_pad);

    /* Link audio sink and video sink bins */
    g_object_set(G_OBJECT(data->block_player->pipeline), "audio-sink", data->block_player->audio_sink_bin, NULL);
    g_object_set(G_OBJECT(data->block_player->pipeline), "video-sink", data->block_player->video_sink_bin, NULL);

    data->block_player->bus = gst_pipeline_get_bus(GST_PIPELINE(data->block_player->pipeline));
    gst_bus_add_watch(data->block_player->bus, block_bus_call, (gpointer)data);
    gst_object_unref(data->block_player->bus);

    g_object_set(G_OBJECT(data->block_player->pipeline), "volume", atof(data->configuration->block_player_volume), NULL);

  } else {
    printf("%s\n", "Unable to create the block player");
  }

}


void get_time_signal_player_audio_sink(struct aras_data *data)
{
  if (strcasecmp(data->configuration->time_signal_player_output, "autoaudiosink") ==0 ) {
    data->time_signal_player->audio_sink = gst_element_factory_make("autoaudiosink", "time_signal_player");
  } else if (strcasecmp(data->configuration->time_signal_player_output, "pulsesink") == 0) {
    data->time_signal_player->audio_sink = gst_element_factory_make("pulsesink", "time_signal_player");
    g_object_set(G_OBJECT(data->time_signal_player->audio_sink), "device", data->configuration->time_signal_player_device, NULL);
  } else if (strcasecmp(data->configuration->time_signal_player_output, "alsasink") == 0) {
    data->time_signal_player->audio_sink = gst_element_factory_make("alsasink", "time_signal_player");
    g_object_set(G_OBJECT(data->time_signal_player->audio_sink), "device", data->configuration->time_signal_player_device, NULL);
  } else if (strcasecmp(data->configuration->time_signal_player_output, "osssink") == 0) {
    data->time_signal_player->audio_sink = gst_element_factory_make("osssink", "time_signal_player");
    g_object_set(G_OBJECT(data->time_signal_player->audio_sink), "device", data->configuration->time_signal_player_device, NULL);
  } else if (strcasecmp(data->configuration->time_signal_player_output, "oss4sink") == 0) {
    data->time_signal_player->audio_sink = gst_element_factory_make("oss4sink", "time_signal_player");
    g_object_set(G_OBJECT(data->time_signal_player->audio_sink), "device", data->configuration->time_signal_player_device, NULL);
  } else if (strcasecmp(data->configuration->time_signal_player_output, "jackaudiosink") == 0) {
    data->time_signal_player->audio_sink = gst_element_factory_make("jackaudiosink", "time_signal_player");
    g_object_set(G_OBJECT(data->time_signal_player->audio_sink), "server", data->configuration->time_signal_player_device, NULL);
    g_object_set(G_OBJECT(data->time_signal_player->audio_sink), "connect", 0, NULL);
  } else if (strcasecmp(data->configuration->time_signal_player_output, "filesink") == 0) {
    data->time_signal_player->audio_sink = gst_element_factory_make("filesink", "time_signal_player");
    g_object_set(G_OBJECT(data->time_signal_player->audio_sink), "buffer-mode", 1, NULL);
    g_object_set(G_OBJECT(data->time_signal_player->audio_sink), "buffer-size", 1000000, NULL);
    g_object_set(G_OBJECT(data->time_signal_player->audio_sink), "location", data->configuration->time_signal_player_device, NULL);
  } else {
    data->time_signal_player->audio_sink = gst_element_factory_make("autoaudiosink", "time_signal_player");
  }
}


void init_time_signal_player(struct aras_data *data)
{
  if ((data->time_signal_player = (struct aras_player*)malloc(sizeof(struct aras_player))) != NULL) {

    /* Playbin */
    data->time_signal_player->pipeline = gst_element_factory_make("playbin", "play");

    /* Audio convert */
    data->time_signal_player->audio_convert = gst_element_factory_make("audioconvert", "audioconvert");

    /* Audio sink */
    get_time_signal_player_audio_sink(data);

    /* Audio convert and audio sink bin */
    data->time_signal_player->audio_sink_bin = gst_bin_new("audio_sink_bin");
    gst_bin_add_many(GST_BIN(data->time_signal_player->audio_sink_bin), data->time_signal_player->audio_convert, data->time_signal_player->audio_sink, NULL);

    data->time_signal_player->audio_caps = gst_caps_new_simple("audio/x-raw", "channels", G_TYPE_INT, 2, NULL);
    gst_element_link_filtered(data->time_signal_player->audio_convert, data->time_signal_player->audio_sink, data->time_signal_player->audio_caps);
    gst_caps_unref(data->time_signal_player->audio_caps);

    data->time_signal_player->audio_sink_pad = gst_element_get_static_pad(data->time_signal_player->audio_convert, "sink");
    data->time_signal_player->audio_sink_ghost_pad = gst_ghost_pad_new("sink", data->time_signal_player->audio_sink_pad);
    gst_pad_set_active(data->time_signal_player->audio_sink_ghost_pad, TRUE);
    gst_element_add_pad (data->time_signal_player->audio_sink_bin, data->time_signal_player->audio_sink_ghost_pad);
    gst_object_unref(data->time_signal_player->audio_sink_pad);

    /* Link audio sink */
    g_object_set(G_OBJECT(data->time_signal_player->pipeline), "audio-sink", data->time_signal_player->audio_sink_bin, NULL);

    data->time_signal_player->bus = gst_pipeline_get_bus(GST_PIPELINE(data->time_signal_player->pipeline));
    gst_bus_add_watch(data->time_signal_player->bus, time_signal_bus_call, (gpointer)data);
    gst_object_unref(data->time_signal_player->bus);

    g_object_set(G_OBJECT(data->time_signal_player->pipeline), "volume", atof(data->configuration->time_signal_player_volume), NULL);

  } else {
    printf("%s\n", "Unable to create the time signal player");
  }
}


void play_uri(gchar *uri, GstElement *pipeline)
{
  gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
  if (uri) {
    g_object_set(G_OBJECT(pipeline), "uri", uri, NULL);
    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
  }
}

