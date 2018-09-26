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

void log_printf(struct aras_data *data, char *string)
{
  FILE *log_file;

  log_file = fopen(data->configuration->log_file_name, "a");

  if (log_file) {
    fprintf(log_file, "%s", string);
    fclose(log_file);
  } else {
    printf("File %s could not be opened\n", data->configuration->log_file_name);
  }
}

void log_db(struct aras_data *data, char *info, char *type) {
  char dbquery[800];
  sprintf(dbquery, "INSERT INTO log (id, date, type, data) VALUES (NULL, NOW(), '%s', '[RadioCore] %s')", type, info);
  if (mysql_query(data->dbcon, dbquery)) {
      fprintf(stderr, "[RadioCore] LOG error: %s\n", mysql_error(data->dbcon));
  }
}

void player_log(struct aras_data *data, char *info)
{
  char dbquery[800];
  char info2[800];
  mysql_real_escape_string(data->dbcon, info2, info, strlen(info));
  sprintf(dbquery, "INSERT INTO player_log (id, date, info) VALUES (NULL, NOW(), '%s')", info2);
  if (mysql_query(data->dbcon, dbquery)) {
      fprintf(stderr, "[RadioCore] PLAYER LOG error: %s\n", mysql_error(data->dbcon));
  }
}

