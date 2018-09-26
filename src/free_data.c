
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


GList *free_schedule(struct aras_data *data)
{
  
  GList *pointer;
  struct aras_schedule *schedule_element;

  pointer = data->schedule;
  while (pointer != NULL) {
    schedule_element = pointer->data;
    free(schedule_element);
    pointer = pointer->next;
  }
  
  g_list_free(data->schedule);
  data->schedule = NULL;
  
  return(data->schedule);
  
}


GList *free_blocks(struct aras_data *data)
{
  
  GList *pointer;
  struct aras_block *block_element;
  
  pointer = data->blocks;
  while (pointer != NULL) {
    block_element = pointer->data;
    free(block_element);
    pointer = pointer->next;
  }
  
  g_list_free(data->blocks);
  data->blocks = NULL;
  
  return(data->blocks);

}


GList *free_playlist(GList *playlist)
{
  
  GList *pointer;
  char *playlist_element;
  
  pointer = playlist;
  while (pointer != NULL) {
    playlist_element = pointer->data;
    free(playlist_element);
    pointer = pointer->next;
  }
  
  g_list_free(playlist);
  playlist = NULL;
  
  return(playlist);

}

