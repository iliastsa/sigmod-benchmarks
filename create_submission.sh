cp scripts/compile.sh .
cp scripts/run_sort.sh .

tar cvzf submission.tar.gz --exclude='./cmake-build-debug' --exclude='./scripts' --exclude='./*.tar.gz' --exclude='./create_submission.sh' --exclude='./.git' .

rm compile.sh
rm run_sort.sh