prdfin=$1
fileout=$2
percentile=$3

./ddump_file_generator.sh $prdfin $fileout $percentile
./modecalc.sh $fileout  
