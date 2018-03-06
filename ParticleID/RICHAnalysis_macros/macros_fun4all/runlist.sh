#!/bin/bash

NUMBER_EVENTS=10

for P in {2..70..2}; do
    OUTPUT_FILE="eval_RICH_PID_p${P}.root"
    root -b -q Fun4All_G4_EICDetector_RICH.C\(${NUMBER_EVENTS},${P}\)
    mv eval_RICH_PID.root ${OUTPUT_FILE}
done