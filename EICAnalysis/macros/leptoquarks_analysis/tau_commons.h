namespace tau_commons
{
  /* particle selection */
  TCut select_true_uds("evtgen_pid!=15 && evtgen_pid!=11");
  //TCut select_true_uds("evtgen_pid == 11");

  //TCut select_true_tau("evtgen_pid==15 && sqrt( (evtgen_eta-jet_eta)*(evtgen_eta-jet_eta) + (evtgen_phi-jet_phi)*(evtgen_phi-jet_phi) ) < 0.1");
  TCut select_true_tau("evtgen_pid==15");
  
  TCut select_accept_jet("jet_ptrans > 5");

  //&& tracks_rmax_r04 < 0.12

  TCut select_tau("tracks_count_r04 == 3 && tracks_chargesum_r04 == -1 && jet_eta < 0.8 && tracks_vertex > 0.03");
}
