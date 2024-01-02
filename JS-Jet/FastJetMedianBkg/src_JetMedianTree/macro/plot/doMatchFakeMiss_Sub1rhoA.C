root -l <<EOF
  .L ../loc_lib/loc_libs.h
  .L ../loc_lib/JetIndicesMatcher.cc
  .L MatchFakeMiss_Sub1rhoA.cc
  Sub1rhoA loop;
  loop.Loop("$0");
EOF
