################################################################################
### parser ### code ### generator ##############################################
################################################################################

# by Frank Volk and Jeffrey Straney

# for assignment 2, we actually built a parse tree that lexed as it parsed
# (though we had not implemented any Symbol table logic, or grammar errors)
# for this reason, the lexer AND PARSER are bundled up in analyzer.c
#
# the VM and Lexer have slight changes since they were done (commenting out
# printing to make results more in line with the rubric requirements)
#
# to summarize, we know it's ugly, but we love it anyway.

# to compile run the following
gcc *.c -o main

# then to run the compiler, do the following
./main [-l -a -v] <your source code file here>

# IMPORTANT INFORMATION TO SAVE TIME
# all error test cases exist in error_examples. Inside, there is a
# shell script that will generate all test cases for you. once inside
# the directory just run
./run_all_errors.sh

# this will generate case by case errors inside of out.txt. in.txt contains
# the content of all .p files in the error_examples directory

# SIMILARLY IMPORTANT
# there is an example script inside the example_script directory. 
# use the following command to run the script with full output
./run_example.sh

# this will basically just call
echo ./example.p > ./example_IO/in.txt && ../main -l -a -v ./run_example.sh > ./example_IO/out.txt

