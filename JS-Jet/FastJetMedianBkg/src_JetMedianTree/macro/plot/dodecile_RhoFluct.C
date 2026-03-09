root -l <<EOF
  .L ../loc_lib/loc_libs.h
  .L decile_RhoFluct.cc
  RhoFluct loop;
  loop.Loop("$0");
EOF
