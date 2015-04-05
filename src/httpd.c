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

#include <user_interface.h>
#include <osapi.h>
#include <mem.h>

#include "httpd.h"
#include "utils.h"

// TODO: Global variabless are EVIL!
static struct httpd_config *config;

ICACHE_FLASH_ATTR
static const char httpd_response_400[] =
	"HTTP/1.0 400 Bad Request\r\n"
	"Connection: close\r\n"
	"\r\n";

ICACHE_FLASH_ATTR
static const char httpd_response_404[] =
	"HTTP/1.0 404 Not Found\r\n"
	"Connection: close\r\n"
	"\r\n";

ICACHE_FLASH_ATTR
static void httpd_callback_recv(void *arg, char *pdata, unsigned short len)
{
	struct espconn *conn = arg;
	struct httpd_conn_private *cpriv = conn->reverse;
	char *tok = NULL;
	int at = 0;
	int i;
	int k;

	if (cpriv->state == HTTPD_CONN_INIT) {
		// Parse path

		k = 2;
		for (; at < len && k > 0; at++) {
			if (pdata[at] == ' ') {
				k--;
				if (k == 1) {
					if (at + 1 >= len)
						break;
					tok = &pdata[at + 1];
				}
				else if (k == 0) {
					pdata[at] = '\0';
					break;
				}
			}
		}

		if (k > 0) {
			espconn_sent(conn, (uint8_t *)httpd_response_400,
					ARRAY_SIZE(httpd_response_400));
			espconn_disconnect(conn);
			return;
		}

		// Find the matching path
		i = 0;
		while (config->urls[i].url != NULL) {
			// TODO: Add proper matcher with wildcards and stuff
			if (os_strcmp(config->urls[i].url, tok) == 0) {
				cpriv->handler = &config->urls[i].handler;
				// TODO: Implement HTTP method parsing
				if (config->urls[i].handler.init != NULL)
					config->urls[i].handler.init(conn, config->urls[i].args, NULL,
						tok);
				break;
			}
			i++;
		}

		if (config->urls[i].url == NULL) {
			espconn_sent(conn, (uint8_t *)httpd_response_404,
					ARRAY_SIZE(httpd_response_404));
			espconn_disconnect(conn);
			return;
		} else {
			cpriv->state = HTTPD_CONN_HEADERS;
		}
	}
	if (cpriv->state == HTTPD_CONN_HEADERS) {
		// TODO: Actual header parsing
		for (; at < len - 3; at++) {
			if (pdata[at] == '\r' && pdata[at + 1] == '\n' &&
				pdata[at + 2] == '\r' && pdata[at + 3] == '\n') {
				if (cpriv->handler->reqf != NULL)
					cpriv->handler->reqf(conn, cpriv->handler->data);
				cpriv->state = HTTPD_CONN_DATA;
			}
		}
	}
}

ICACHE_FLASH_ATTR
static void httpd_callback_disconnect(void *arg)
{
	struct espconn *conn = arg;
	struct httpd_conn_private *cpriv= conn->reverse;

	if (cpriv->handler != NULL && cpriv->handler->disconnect != NULL)
		cpriv->handler->disconnect(conn, cpriv->handler->data);
	os_free(conn->reverse);
}

ICACHE_FLASH_ATTR
static void httpd_callback_connect(void *arg)
{
	struct espconn *conn = (struct espconn *)(arg);
	struct httpd_conn_private *cpriv = (struct httpd_conn_private *)
		os_zalloc(sizeof(struct httpd_conn_private));

	cpriv->state = HTTPD_CONN_INIT;

	conn->reverse = cpriv;

	espconn_regist_recvcb(conn, httpd_callback_recv);
	espconn_regist_disconcb(conn, httpd_callback_disconnect);
	espconn_regist_time(conn, 15, 0);
}

ICACHE_FLASH_ATTR
void httpd_init(struct httpd_config *cfg)
{
	config = cfg;
	espconn_create(&cfg->conn);
	config->conn.type = ESPCONN_TCP;
	config->conn.state = ESPCONN_NONE;

	config->conn.proto.tcp = &config->tcp;
	config->conn.proto.tcp->local_port = config->port;

	espconn_regist_connectcb(&config->conn, httpd_callback_connect);

	espconn_accept(&config->conn);

	espconn_regist_time(&config->conn, 1500, 0);
}
