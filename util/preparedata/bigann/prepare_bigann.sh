echo "128" > bigann_learn.bvecs.txt
echo "10000" >> bigann_learn.bvecs.txt
echo "100000000" >> bigann_learn.bvecs.txt
echo "1000" >> bigann_learn.bvecs.txt
./bvectors bigann_learn.bvecs >> bigann_learn.bvecs.txt
mv bigann_learn.bvecs.txt bigann_learn.bvecs_100m.txt
../preparedata bigann_learn.bvecs_100m.txt bigann_learn.bvecs_100M.txt
# end
