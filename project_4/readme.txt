
# by Frank Volk and Jeffrey Straney

# to compile run the following
gcc *.c -o main

# then to run the compiler, do the following
./main [-l -a -v] <your source code file here>

# IMPORTANT INFORMATION TO SAVE TIME
# go into example_script and run the following. you should see example input and output
# put into the directories example_IO
./run_example.sh

# this will basically just call
echo ./example.p > ./example_IO/in.txt && ../main -l -a -v ./run_example.sh > ./example_IO/out.txt

