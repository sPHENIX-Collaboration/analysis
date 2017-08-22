#!/bin/tcsh -f

root -l -q -b -x macro/pa2pr.C\(\"data/pa_XEMC/PA.root\",\"e^\{-\}\",\"data/de_XEMC/DE00011_EEMC.root\",\"data/de_XEMC/DE00011_CEMC.root\",\"data/de_XEMC/DE00011_FEMC.root\",\"data/pr_XEMC/PR00011.pdf\"\)
root -l -q -b -x macro/pa2pr.C\(\"data/pa_XEMC/PA.root\",\"e^\{+\}\",\"data/de_XEMC/DE10011_EEMC.root\",\"data/de_XEMC/DE10011_CEMC.root\",\"data/de_XEMC/DE10011_FEMC.root\",\"data/pr_XEMC/PR10011.pdf\"\)
