# clean up any old files
rm example_IO/*.txt
# echo input of program to in.txt
more program.p > example_IO/in.txt 

# redirect full output of program to out.txt
../main -l -a -v ./program.p > example_IO/out.txt
