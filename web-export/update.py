#!/usr/bin/env python

import os,re,string

xmlto = "/usr/bin/xmlto"
cvs = "http://cvs.freedesktop.org/"
git = "http://cgit.freedesktop.org/"
specindex = "specs.idx"

try:
	f = open(specindex, 'r')
	lastname = ''
	lastpath = ''
	for line in f.readlines():
		line = line.strip()
		if not line or line.startswith('#'):
			continue

		(file, revision, version, path) = string.split(line)
		use_git = False
		if file.startswith("git:"):
			use_git = True
			git_data = file.split(":")
			git_repo = git_data[1]
			file = git_data[2]
		name = os.path.splitext(os.path.split(file)[1])[0]
		# Strip version from name
		if re.search("\d\.\d+$", name):
			name = re.sub("^(.*)-([^/]*)$", "\\1", name)

		if use_git:
			url = '%s%s/plain/%s?id=%s' % (git, git_repo, file, revision)
		else:
			url = '%s%s?rev=%s' % (cvs, file, revision)

		if re.search("\.xml$", file):
			os.system("mkdir %s 2> /dev/null" % (path))
			if lastpath != path and lastname != name:
				os.system("rm -f %s/%s-latest.html" % (path, name))
				os.system("cd %s; ln -s %s-%s.html %s-latest.html" % (path,name,version,name))
				os.system("rm -f %s/latest" % (path))
				os.system("cd %s; ln -s %s latest" % (path,version))

			# if ( lastpath == path and lastname == name and os.path.isfile("%s/%s-%s.xml" % (path, name, version))):
			#	print "Updating", file, "Version", version, "rev", revision, "skipped."
			#	continue

			if os.system("wget -q '%s' -O wget.xml && (diff -q wget.xml %s/%s-%s.xml || mv wget.xml %s/%s-%s.xml)" % (url, path, name, version, path, name, version)):
				print "Updating", file, "Version", version, "rev", revision, "FAILED."
                        os.system("chmod g+w wget.xml");

			print "Updating", file, "Version", version, "rev", revision, "ok"
				
		elif re.search("\.txt$", file):
			os.system("mkdir %s 2> /dev/null" % (path))
			if lastpath != path and lastname != name:
				os.system("rm -f %s/%s-latest.txt" % (path, name))
				os.system("cd %s; ln -s %s-%s.txt %s-latest.txt" % (path,name,version,name))

			if ( lastpath == path and lastname == name and os.path.isfile("%s/%s-%s.txt" % (path, name, version))):
				print "Updating", file, "Version", version, "rev", revision, "skipped."
				continue

			if os.system("wget -q '%s' -O wget.txt && (diff -q wget.txt %s/%s-%s.txt || mv wget.txt %s/%s-%s.txt)" % (url, path, name, version, path, name, version)):
				print "Updating", file, "Version", version, "rev", revision, "FAILED."
                        os.system("chmod g+w wget.txt");

			print "Updating", file, "Version", version, "rev", revision, "ok"

		elif re.search("\.dtd$", file):
			os.system("mkdir %s 2> /dev/null" % (path))
			if lastpath != path and lastname != name:
				os.system("rm -f %s/%s-latest.dtd" % (path, name))
				os.system("cd %s; ln -s %s-%s.dtd %s-latest.dtd" % (path,name,version,name))

			if ( lastpath == path and lastname == name and os.path.isfile("%s/%s-%s.dtd" % (path, name, version))):
				print "Updating", file, "Version", version, "rev", revision, "skipped."
				continue

			if os.system("wget -q '%s' -O wget.dtd && (diff -q wget.dtd %s/%s-%s.dtd || mv wget.dtd %s/%s-%s.dtd)" % (url, path, name, version, path, name, version)):
				print "Updating", file, "Version", version, "rev", revision, "FAILED."
                        os.system("chmod g+w wget.dtd");

			print "Updating", file, "Version", version, "rev", revision, "ok"
		else:
			print "Skipping", file, ", unknown file."
			continue

		lastname = name
		lastpath = path

except IOError:
	print "Can't open", specindex


specs = os.listdir(".")

for spec in specs:
	if not os.path.isdir(spec):
		continue
	versions = os.listdir(spec)
	for file in versions:
		if re.search("\.xml$", file):
			tmp = re.sub("(.*)(\.xml)$", "\\1", file)
			name = re.sub("^(.*)-([^/]*)$", "\\1", tmp)
			ver = re.sub("^(.*)-([^/]*)$", "\\2", tmp)
			
			print "Check", os.path.join(spec,ver), os.path.isdir(os.path.join(spec,ver))
			print "Check", os.path.join(spec,name+"-"+ver+".html"), os.path.isfile(os.path.join(spec,name+"-"+ver+".html"))

			if (	not os.path.isdir(os.path.join(spec,ver))
			    	or not os.path.isfile(os.path.join(spec,name+"-"+ver+".html"))
				or os.path.getmtime(os.path.join(spec,file)) > os.path.getmtime(os.path.join(spec,name+"-"+ver+".html"))):
				os.system("rm -fR %s/%s" % (spec,ver))
				os.system("rm -f %s/%s-%s.html" % (spec,name,ver))
				os.system("mkdir %s/%s" % (spec,ver))
				os.system("cd %s/%s; %s html ../%s" % (spec,ver,xmlto,file))
				# os.system("mv index.html %s/%s-%s.html" % (spec,name,ver))
				# os.system("sed -i %s/%s-%s.html -e 's/index.html/%s-%s.html/;'" % (spec,name,ver,name,ver))
				os.system("cd %s;%s html-nochunks %s" % (spec,xmlto,file))
		elif re.search("(?<!latest)\.html$", file) and not os.path.isfile(os.path.join(spec,re.sub("html","xml",file))):
				tmp = re.sub("(.*)(\.html)$", "\\1", file)
				name = re.sub("^(.*)-([^/]*)$", "\\1", tmp)
				ver = re.sub("^(.*)-([^/]*)$", "\\2", tmp)
				os.system("rm -fR %s/%s" % (spec,ver))
				os.system("rm -f %s/%s-%s.html" % (spec,name,ver))
	for file in versions:
		if re.search("-latest\.dtd$", file):
			# Do nothing
			print "Skipping", file
		elif re.search("\.dtd$", file):
			tmp = re.sub("(.*)(\.dtd)$", "\\1", file)
			name = re.sub("^(.*)-([^/]*)$", "\\1", tmp)
			ver = re.sub("^(.*)-([^/]*)$", "\\2", tmp)
			
			print "Check", os.path.join(spec,ver), os.path.isdir(os.path.join(spec,ver))
			print "Check", os.path.join(spec,name+"-"+ver+".html"), os.path.isfile(os.path.join(spec,name+"-"+ver+".html"))

			os.system("mkdir %s/%s" % (spec,ver))
			os.system("cp %s/%s-%s.dtd %s/%s/%s.dtd" % (spec,name,ver,spec,ver,name))
