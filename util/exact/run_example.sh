echo "=========================================="
echo "./exact ../../dataset/pendigits.txt -t -k 10 -o -A -1"
./exact ../../dataset/pendigits.txt -t -k 10 -o -A -1
mv ../../dataset/pendigits.out ../../dataset/pendigits.exact.out

echo "=========================================="
echo "../../main ../../dataset/pendigits.txt -t -k 10 -o -A 2"
../../main ../../dataset/pendigits.txt -t -k 10 -o -A 2
mv ../../dataset/pendigits.out ../../dataset/pendigits.cmsa.out

echo "=========================================="
echo "head ../../dataset/pendigits.exact.out"
head ../../dataset/pendigits.exact.out
echo "=========================================="
echo "head ../../dataset/pendigits.cmsa.out"
head ../../dataset/pendigits.cmsa.out
