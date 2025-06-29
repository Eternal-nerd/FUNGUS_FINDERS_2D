cmake --build build
echo "compiling shaders"
cd shaders/
compile.bat
cd ..
bin\FUNGUS_FINDERS_2D.exe
