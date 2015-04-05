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

#include "httpd.h"
#include "handler.h"

#define SSID "Whatever"
#define PASSWORD "Something"

static char test_page[] =
"HTTP/1.0 200 OK\r\n"
"Connection: close\r\n"
"\r\n"
"<html>"
"<head></head>"
"<body>"
"<h1>Hello!</h1>"
"<a href=\"/second\">Click!</a>"
"</body>"
"</html>";

static char test_page2[] =
"HTTP/1.0 200 OK\r\n"
"Connection: close\r\n"
"\r\n"
"<html>"
"<head></head>"
"<body>"
"<h1>It works!</h1>"
"<a href=\"/\">Back!</a>"
"</body>"
"</html>";

struct httpd_config config_httpd = {
	.port = 80,
	.urls = (struct httpd_url[]){
		{"/", HANDLER_STATIC(test_page)},
		{"/second", HANDLER_STATIC(test_page2)},
		{NULL, {}}
	}
};

ICACHE_FLASH_ATTR
void user_init(void)
{
	struct station_config config = {
		.ssid = SSID,
		.password = PASSWORD,
	};

	wifi_set_opmode(STATION_MODE);
	wifi_station_set_config(&config);
	wifi_station_connect();

	httpd_init(&config_httpd);
}
