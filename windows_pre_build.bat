vcpkg new --application
vcpkg add port fftw3
vcpkg add port freetype
vcpkg add port glm
vcpkg install --triplet=x64-windows-static
