if [[ $# -ne 3 ]]; then
    echo "=========================================="
    echo "Usage  : ./run.sh datasetname numrefs k"
    echo "           datasetname: name of the dataset without the .txt"
    echo "           numrefs: number of references (msa and cmsa)"
    echo "           k: number of nearest neighbors"
    echo " "
    echo "Example: pendigits dataset, 100 references, 10 nearest neighbors"
    echo "         ./run.sh pendigits 100 10"
    echo "=========================================="
    exit 2
fi

echo "=========================================="
echo "../exact/exact ../../dataset/$1 -t -k 10 -o -A -1"
../exact/exact ../../dataset/$1.txt -t -k $3 -o -A -1
mv ../../dataset/$1.out ../../dataset/$1.exact.out

echo "=========================================="
echo "../../main ../../dataset/$1.txt -t -k $3 -o -r $2 -A 2"
../../main ../../dataset/$1.txt -t -k $3 -o -r $2 -A 2
mv ../../dataset/$1.out ../../dataset/$1.cmsa.out

#echo "=========================================="
#echo "head ../../dataset/$1.exact.out"
#head ../../dataset/$1.exact.out
#echo "=========================================="
#echo "head ../../dataset/$1.cmsa.out"
#head ../../dataset/$1.cmsa.out

echo "=========================================="
./compare ../../dataset/$1.txt ../../dataset/$1.exact.out ../../dataset/$1.cmsa.out
