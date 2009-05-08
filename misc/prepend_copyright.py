#!/usr/bin/python

import sys
import string

copyright =	"/* GOSM - the Gtk OpenStreetMap Tool\n" + \
		" *\n" + \
		" * Copyright (C) 2009  Sebastian Kuerten\n" + \
		" *\n" + \
		" * This file is part of Gosm.\n" + \
		" *\n" + \
		" * Gosm is free software: you can redistribute it and/or modify\n" + \
		" * it under the terms of the GNU General Public License as published by\n" + \
		" * the Free Software Foundation, either version 3 of the License, or\n" + \
		" * (at your option) any later version.\n" + \
		" *\n" + \
		" * Gosm is distributed in the hope that it will be useful,\n" + \
		" * but WITHOUT ANY WARRANTY; without even the implied warranty of\n" + \
		" * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" + \
		" * GNU General Public License for more details.\n" + \
		" *\n" + \
		" * You should have received a copy of the GNU General Public License\n" + \
		" * along with Gosm.  If not, see <http://www.gnu.org/licenses/>.\n" + \
		" */\n"

args = len(sys.argv)
if not args > 1:
	print "usage: prepend_copyright.py file1 [file2, ..., fileN]"
	exit()	

for x in sys.argv[1:]:
	print x
	f = open(x, "r")
	c = f.read()
	f.close()
	f = open(x, "w")
	f.write(copyright)
	f.write("\n")
	f.write(c)
	f.close()

#f_h = open(names[0]+".h", "w")
#f_c = open(names[0]+".c", "w")
#f_h.write(text_h)
#f_c.write(text_c)
#f_h.close()
#f_c.close()
