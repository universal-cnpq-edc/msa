if [[ $# -ne 5 ]]; then
    echo "=========================================="
    echo "Usage  : ./run-all.sh directory datasetname numrecords k numq"
    echo "           datasetname: name of the dataset without the .txt"
    echo "           numrecords: number of records of the dataset"
    echo "           k: number of nearest neighbors"
    echo "           numq: number of queries"
    echo " "
    echo "Example: pendigits dataset, 2000 records, 100-nearest neighbors, 5 queries"
    echo "         ./run-all.sh /mnt/data pendigits 2000 100 5"
    echo "=========================================="
    exit 2
fi

echo "=========================================="
echo "../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 16 -A -1"
../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 16 -A -1
mv $1/$2.out $1/$2.exact.out
echo "====="
echo "head $1/$2.exact.out"
head $1/$2.exact.out
echo "=========================================="
echo "../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 16 -A 2"
../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 16 -A 2
mv $1/$2.out $1/$2.msa.16r.out
echo "====="
echo "head $1/$2.msa.16r.out"
head $1/$2.msa.16r.out
echo "=========================================="
echo "../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 32 -A 2"
../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 32 -A 2
mv $1/$2.out $1/$2.msa.32r.out
echo "====="
echo "head $1/$2.msa.32r.out"
head $1/$2.msa.32r.out
echo "=========================================="
echo "../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 64 -A 2"
../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 64 -A 2
mv $1/$2.out $1/$2.msa.64r.out
echo "====="
echo "head $1/$2.msa.64r.out"
head $1/$2.msa.64r.out
echo "=========================================="
echo "../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 128 -A 2"
../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 128 -A 2
mv $1/$2.out $1/$2.msa.128r.out
echo "====="
echo "head $1/$2.msa.128r.out"
head $1/$2.msa.128r.out
echo "=========================================="
echo "../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 256 -A 2"
../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 256 -A 2
mv $1/$2.out $1/$2.msa.256r.out
echo "====="
echo "head $1/$2.msa.256r.out"
head $1/$2.msa.256r.out
echo "=========================================="
echo "../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 512 -A 2"
../../main $1/$2.txt -t -n $3 -k $4 -o -q $5 -r 512 -A 2
mv $1/$2.out $1/$2.msa.512r.out
echo "====="
echo "head $1/$2.msa.512r.out"
head $1/$2.msa.512r.out
echo "=========================================="
echo "$5 queries"                                                    > $1/$2-$4nn-$3records.compare.txt
echo "./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.16r.out $3 $5"  >> $1/$2-$4nn-$3records.compare.txt
      ./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.16r.out $3 $5   >> $1/$2-$4nn-$3records.compare.txt
echo "=========================================="                   >> $1/$2-$4nn-$3records.compare.txt
echo "./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.32r.out $3 $5"  >> $1/$2-$4nn-$3records.compare.txt
      ./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.32r.out $3 $5   >> $1/$2-$4nn-$3records.compare.txt
echo "=========================================="                   >> $1/$2-$4nn-$3records.compare.txt
echo "./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.64r.out $3 $5"  >> $1/$2-$4nn-$3records.compare.txt
      ./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.64r.out $3 $5   >> $1/$2-$4nn-$3records.compare.txt
echo "=========================================="                   >> $1/$2-$4nn-$3records.compare.txt
echo "./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.128r.out $3 $5" >> $1/$2-$4nn-$3records.compare.txt
      ./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.128r.out $3 $5  >> $1/$2-$4nn-$3records.compare.txt
echo "=========================================="                   >> $1/$2-$4nn-$3records.compare.txt
echo "./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.256r.out $3 $5" >> $1/$2-$4nn-$3records.compare.txt
      ./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.256r.out $3 $5  >> $1/$2-$4nn-$3records.compare.txt
echo "=========================================="                   >> $1/$2-$4nn-$3records.compare.txt
echo "./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.512r.out $3 $5" >> $1/$2-$4nn-$3records.compare.txt
      ./compare $1/$2.txt $1/$2.exact.out $1/$2.msa.512r.out $3 $5  >> $1/$2-$4nn-$3records.compare.txt
echo "=========================================="                   >> $1/$2-$4nn-$3records.compare.txt
cat $1/$2-$4nn-$3records.compare.txt
