rm rootlumi.txt -f
for i in {0..63}; do
    root -l -q -b 'get_luminosity.C("fullgoodrunlist.list",'${i}',0)' >> rootlumi.txt
done

cat rootlumi.txt | grep pb > alltriglumi.txt
