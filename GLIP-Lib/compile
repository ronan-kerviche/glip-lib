clear
echo Compiling...
cmake . -G"Unix Makefiles"
make
if [ $? = 0 ]
then
  echo -e "Compilation : \e[1;32m[OK]\e[0m"; tput sgr0
else
  echo -e "Compilation : \e[0;31m[FAILED]\e[0m"; tput sgr0
fi
rm ./CMakeCache.txt ./cmake_install.cmake
rm ./CMakeFiles -r
