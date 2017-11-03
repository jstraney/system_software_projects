# clear all error.p files
rm all_errors_IO/*.txt

# run through each and append output to now vacant files
for source in ./*.p; do
  more ./$source >> all_errors_IO/in.txt
  echo "testing $source" >> ./all_errors_IO/out.txt
  ../main ./$source >> ./all_errors_IO/out.txt 
done
