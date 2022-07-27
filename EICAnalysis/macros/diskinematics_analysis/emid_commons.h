namespace emid_commons
{
  /* particle selection */
  /* select true electron */
  TCut select_true_ele("(em_evtgen_pid == 11)");

  /* select charged pions */
  TCut select_true_cpi("(abs(em_evtgen_pid) == 211)");

  /* select candidates with matching track found */
  TCut select_match("em_track_cluster_dr == em_track_cluster_dr");

  /* selectrion of reconstructed electron */
  TCut select_electron("em_cluster_prob > 0.01");
}
