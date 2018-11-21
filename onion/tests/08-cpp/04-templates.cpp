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

#include "../ctest.h"
#include <onion.hpp>
#include <url.hpp>
#include <dict.hpp>
#include <response.hpp>
#include <shortcuts.hpp>

#include <assets.h>

void t01_url(){
	INIT_LOCAL();
	
	Onion::Onion o;

	// Create an empty url handler
	Onion::Url url(o);
	
	url.add("", [](Onion::Request &req, Onion::Response &res){
		return Onion::render_to_response(_04_templates_html, std::map<std::string,std::string>{{"test","ok"}} , res);
	});
	
	o.listen();
	
	END_LOCAL();
}

int main(int argc, char **argv){
  START();
	t01_url();
	
	END();
}
