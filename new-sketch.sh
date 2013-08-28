#!/bin/bash

arduino_project_dir="`dirname \"$0\"`"
name=$1

function make_readme()
{
	echo $name > README.md
	echo "===========" >> README.md
	echo >> README.md
	echo "A Panstamp/Arduino sketch" >> README.md
	echo >> README.md
}

function make_project()
{
	mkdir $arduino_project_dir/$name
	cp -Rf $arduino_project_dir/template/* $arduino_project_dir/$name
	old_cd=$(pwd)
	cd $arduino_project_dir/$name
	rm -Rf obj/ obj.* bin/ bin.* src/
	sed -i "s/template.h/$name.h/g" template.cpp
	sed -i "s/Template/$name/g" template.cpp
	mv template.cpp $name.cpp
	mv template.h $name.h
	rm -Rf template.*
	rm -Rf tags*
	rm -f README.md
	make_readme
	cd $old_cd
}


make_project

exit 0
