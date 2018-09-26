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

void connect_database(struct aras_data *data) {
  data->dbcon = mysql_init(NULL);

  if (data->dbcon == NULL) 
  {
      fprintf(stderr, "[RadioCore] DB Init error: %s\n", mysql_error(data->dbcon));
      exit(1);
  }

  if (mysql_real_connect(data->dbcon, data->configuration->db_host, data->configuration->db_user, data->configuration->db_pass, 
          data->configuration->db_name, 0, NULL, 0) == NULL) 
  {
      fprintf(stderr, "[RadioCore] Connect error: %s\n", mysql_error(data->dbcon));
      mysql_close(data->dbcon);
      exit(1);
  }  


  fprintf(stderr, "[RadioCore] DB Initialized\n");
}

void disconnect_database(struct aras_data *data) {
  fprintf(stderr, "[RadioCore] Closing database\n");
  mysql_close(data->dbcon);
}


void db_live_signal_status(struct aras_data *data)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char buffer[1024];
    sprintf(buffer, "SELECT status, info  FROM signals WHERE id = %d", SIGNALID_ISLIVE);

 if (!mysql_query(data->dbcon, buffer)) {

    res = mysql_store_result(data->dbcon);

    if ((row = mysql_fetch_row(res)) != NULL) 
      {if (atoi(row[0]) == YES) // live is live
        data->is_live = YES;   
      else
        data->is_live = NO;}
  
    mysql_free_result(res);
  } else 
    data->is_live = 0;
}



