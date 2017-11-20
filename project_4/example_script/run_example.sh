# clear all error.p files
rm examples_IO/*.txt

# run through each and append output to now vacant files
for source in ./*.p; do
  more ./$source >> examples_IO/in.txt
  echo "Running test $source" >> ./examples_IO/out.txt
  ../main -l -a -v ./$source >> ./examples_IO/out.txt 
done
