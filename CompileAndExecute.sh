#!/bin/bash

# Compile the file. Remember, if we have multiple C++ files to compile together, we type their
# names next to main.cpp.
echo "Compiling the file now!"

g++ -std=c++11 -I "/home/012/a/al/alr150630/OperatingSystemConcepts/ProjectOne" main.cpp -o main.exe

rm *~

# Execute the file and put it in less mode. If we ever need to give an argument to our program, we
# hand put it after the "./main.exe" part.
#echo "Executing the program now!"

#./main.exe -l 5
