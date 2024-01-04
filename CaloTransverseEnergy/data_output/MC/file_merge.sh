#! ./bin/bash
LAST=1500
END=$(( $LAST+$1 ))
FINAL=59999
FLIST=""
while (( LAST < FINAL )); do
	for i in {0..200}; do
		j=$(( $LAST + $i ))
		FLIST+="Transverse_Energy_run_7_segment_$j.root "
	done
	hadd -k -j 8 "ET_seg_$LAST.root" $FLIST
	LAST=$END
	END=$(( $LAST+$1 ))
done
hadd -k -j 8 "Transverse_Energy_MC.root" "ET_seg_*"

