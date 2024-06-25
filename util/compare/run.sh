if [[ $# -ne 5 ]]; then
    echo "=========================================="
    echo "Usage  : ./run.sh datasetname numrefs k numrecords numq"
    echo "           datasetname: name of the dataset without the .txt"
    echo "           numrefs: number of references (msa and cmsa)"
    echo "           k: number of nearest neighbors"
    echo "           numrecords: number of records of the dataset"
    echo "           numq: number of queries"
    echo " "
    echo "Example: pendigits dataset, 100 references, 10 nearest neighbors, 2000 records, 5 queries"
    echo "         ./run.sh ../../dataset/pendigits 100 10 2000 5"
    echo "=========================================="
    exit 2
fi

echo "=========================================="
echo "../../main $1.txt -t -n $4 -k $3 -o -r $2 -A -1"
../../main $1.txt -t -n $4 -k $3 -o -q $5 -r $2 -A -1
mv $1.out $1.exact.out
echo "=========================================="
echo "../../main ../../dataset/$1.txt -t -n $4 -k $3 -o -r $2 -A 2"
../../main $1.txt -t -n $4 -k $3 -o -q $5 -r $2 -A 2
mv $1.out $1.msa.out
echo "=========================================="
echo "head $1.exact.out"
head $1.exact.out
echo "=========================================="
echo "head $1.msa.out"
head $1.msa.out
echo "=========================================="
echo "./compare $1.txt $1.exact.out $1.msa.out $4" > $1-$2references-$3neighbors-$4records.compare.txt
./compare $1.txt $1.exact.out $1.msa.out $4 $5 >> $1-$2references-$3neighbors-$4records.compare.txt
cat $1-$2references-$3neighbors-$4records.compare.txt
