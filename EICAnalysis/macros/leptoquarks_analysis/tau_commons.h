namespace tau_commons
{
  /* particle selection */
  TCut select_true_uds("evtgen_is_tau==0");

  TCut select_true_tau("evtgen_is_tau==1 && sqrt( (evtgen_tau_eta-jet_eta)*(evtgen_tau_eta-jet_eta) + (evtgen_tau_phi-jet_phi)*(evtgen_tau_phi-jet_phi) ) < 0.1");

  TCut select_accept_jet("abs(jet_eta)<1.0 && jet_ptrans > 5");

  TCut select_tau("tracks_count_r04 == 3 && tracks_chargesum_r04 == -1 && tracks_rmax_r04 < 0.20 && jetshape_radius < 0.15");
}
