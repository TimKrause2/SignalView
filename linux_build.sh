#!/bin/bash

# test for a makefile
if ! [ -a Builds/LinuxMakefile/Makefile ]; then
	echo "Makefile not found. Run Projucer and save the project."
	exit
fi

# test for vcpkg
if ! [ -x ~/.vcpkg/vcpkg ]; then
	. <(curl https://aka.ms/vcpkg-init.sh -L)
	. ~/.vcpkg/vcpkg-init
fi

# test for vcpkg_installed
cd Builds/LinuxMakefile

VCPKG=~/.vcpkg/vcpkg

if ! [ -d vcpkg_installed ]; then
	$VCPKG new --application
	$VCPKG add port fftw3
	$VCPKG install --triplet=x64-linux-release
fi

export CONFIG=Release

make

