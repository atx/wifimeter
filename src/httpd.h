/*
 * GNU General Public License v3
 *
 * Copyright (c) 2015 Josef Gajdusek
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HTTPD_H_
#define _HTTPD_H_

#include <user_interface.h>
#include <espconn.h>

struct httpd_handler {
	// Called on connection start and first header read
	void (*init)(struct espconn *conn, void *args, char *method, char *url);
	// Called for each header received
	void (*header)(struct espconn *conn, void *args, char *name, char *value);
	// Called after all headers have been received
	void (*reqf)(struct espconn *conn, void *args);
	// Called on disconnect
	void (*disconnect)(struct espconn *conn, void *args);
	// Data used by the handler functions
	void *data;
};

struct httpd_url {
	char *url;
	struct httpd_handler handler;
	void *args;
};

struct httpd_config {
	uint16_t port;
	struct httpd_url *urls;

	struct espconn conn;
	esp_tcp tcp;
};

enum httpd_conn_state {
	HTTPD_CONN_INIT,
	HTTPD_CONN_HEADERS,
	HTTPD_CONN_DATA,
};

struct httpd_conn_private {
	enum httpd_conn_state state;
	struct httpd_handler *handler;
	void *hdlpriv;
};


void httpd_init(struct httpd_config *cfg);

#endif
