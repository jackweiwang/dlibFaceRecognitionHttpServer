/**
  Onion HTTP server library
  Copyright (C) 2010-2018 David Moreno Montero and others

  This library is free software; you can redistribute it and/or
  modify it under the terms of, at your choice:

  a. the Apache License Version 2.0.

  b. the GNU General Public License as published by the
  Free Software Foundation; either version 2.0 of the License,
  or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of both licenses, if not see
  <http://www.gnu.org/licenses/> and
  <http://www.apache.org/licenses/LICENSE-2.0>.
*/

#ifndef __ONION_HANDLER_INTERNAL_STATUS__
#define __ONION_HANDLER_INTERNAL_STATUS__

#include <onion/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Creates an auth handler that do not allow to pass unless user is authenticated using a pam name.
  onion_handler *onion_internal_status();

#ifdef __cplusplus
}
#endif
#endif
