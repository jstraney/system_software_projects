################################################################################
### parser ### code ### generator ##############################################
################################################################################
# by Frank Volk and Jeffrey Straney

# for assignment 2, we actually built a parse tree that lexed as it parsed
# (though we had not implemented any Symbol table logic, or grammar errors)
# for this reason, the lexer and parser are bundled up in analyzer.c
#
# the VM and Lexer have slight changes since they were done (commenting out
# printing to make results more in line with the rubric requirements)
#
# to summarize, we know it's ugly, but we love it anyway.

# to compile run the following
gcc *.c -o main

# then to run the compiler, do the following
./main [-l -a -v] <your source code file here>
