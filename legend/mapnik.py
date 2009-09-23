#!/usr/bin/python

from urllib import *
from re import *
import os
import shutil

url = "http://www.openstreetmap.org/key?layer=mapnik&zoom="


def get_legend_mapnik(zoom):
	htmlname = "mapnik_" + str(zoom) + ".html"
	dirname = "images" 
	try:
		os.stat(dirname)
	except OSError:
		os.mkdir(dirname)
	urla = url + str(zoom)
	ret = urlretrieve(urla)
	filename = ret[0]
	f = open(filename)
	content = f.read()
	occs = findall("src.*\"", content)
	for occ in occs:
		suffix = occ[5:-1]
		imgurl = "http://www.openstreetmap.org" + suffix
		imgname = suffix.split('/')[-1]
		imgname = sub(escape("?"), ".", imgname)
		newimgname = "images/" + imgname
		print suffix, newimgname
		print imgname, imgurl
		localname = dirname + "/" + imgname
		try:
			os.stat(localname)
		except OSError:
			tmpname = urlretrieve(imgurl)[0]
			shutil.copy2(tmpname, localname)
			os.chmod(localname, 0755)
		# rename images in html
		content = sub(escape(suffix), newimgname, content)
	content = sub("<h3>.*</h3>", "", content)
	content = "<html>\
<head>\
<style type=\"text/css\">\
body {-webkit-user-select: none}\
</style>\
</head>\
<body>\
" + content
	content = content + "</body>\
</html>"
	html = open(htmlname, "w")
	html.write(content)
	html.close()
		
	
if __name__ == "__main__":
	#get_legend_mapnik(14)
	for i in range(1,19):
		get_legend_mapnik(i)
