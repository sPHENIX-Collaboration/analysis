#!/bin/bash
# Switch Jet_resp_Def.h between the first-pass and closure configurations.
# Usage: ./set_config.sh firstpass|closure
cd "$(dirname "$0")"

case "$1" in
  firstpass) src=Jet_resp_Def_firstpass.h ;;
  closure)   src=Jet_resp_Def_closure.h ;;
  *) echo "Usage: $0 firstpass|closure"
     grep -m1 "configuration" Jet_resp_Def.h 2>/dev/null && echo "(current Jet_resp_Def.h shown above)"
     exit 1 ;;
esac

cp "$src" Jet_resp_Def.h
echo "Installed $src -> Jet_resp_Def.h"
grep -E "config_firstpass|n_jetradius = |n_zvertex = " Jet_resp_Def.h
