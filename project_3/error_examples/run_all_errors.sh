cat ./*.p > all_errors_IO/in.txt
for source in ./*p; do
  ../main -l > ./all_errors_IO/out.txt
done
