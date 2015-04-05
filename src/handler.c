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

#include "handler.h"

ICACHE_FLASH_ATTR
void handler_static_reqf(struct espconn *conn, void *args)
{
	uint8_t *str = (uint8_t *)args;
	espconn_sent(conn, str, os_strlen(str));
	espconn_disconnect(conn);
}
