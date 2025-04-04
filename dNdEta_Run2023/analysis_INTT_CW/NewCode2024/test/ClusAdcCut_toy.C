float ClusAdcCut_toy()
{
    std::vector<int> inner_cluster_phi; inner_cluster_phi.clear();
    std::vector<int> outer_cluster_phi; outer_cluster_phi.clear();

    TRandom3 * r = new TRandom3(0);

    int number_of_tracks = 50000;
    double clus_drop_ratio = 0.05; 

    for (int i = 0; i < number_of_tracks; i++) // note : define number of tracks 
    {
        inner_cluster_phi.push_back(i);
        outer_cluster_phi.push_back(i);
    }
    // note : inner_cluster_phi[i] should pair with outer_cluster_phi[i]

    std::cout<<"set number_of_tracks = "<<number_of_tracks<<std::endl;
    std::cout<<"drop fraction: "<<clus_drop_ratio<<std::endl;
    std::cout<<"before hit dropping, inner_cluster_phi.size(): "<<inner_cluster_phi.size()<<", outer_cluster_phi.size(): "<<outer_cluster_phi.size()<<std::endl;

    for (int i = 0; i < inner_cluster_phi.size(); i++)
    {
        if (r -> Uniform(0,1) < clus_drop_ratio)
        {
            inner_cluster_phi.erase(inner_cluster_phi.begin() + i);
            i--;
        }
    }

    for (int i = 0; i < outer_cluster_phi.size(); i++)
    {
        if (r -> Uniform(0,1) < clus_drop_ratio)
        {
            outer_cluster_phi.erase(outer_cluster_phi.begin() + i);
            i--;
        }
    }

    std::cout<<"post hit dropping, inner_cluster_phi.size() = "<<inner_cluster_phi.size()<<std::endl;
    std::cout<<"post hit dropping, outer_cluster_phi.size() = "<<outer_cluster_phi.size()<<std::endl;

    int remaining_track_count = 0;
    for (auto clu_inner : inner_cluster_phi)
    {
        for (auto clu_outer : outer_cluster_phi)
        {
            // std::cout<<"clu_inner = "<<clu_inner<<", clu_outer = "<<clu_outer<<std::endl;
            if (clu_inner == clu_outer)
            {
                remaining_track_count++;
            }
        }
    }

    std::cout<<"remaining_track_count = "<<remaining_track_count<<std::endl;
    std::cout<<"ratio: "<<(double)remaining_track_count/(double)number_of_tracks<<std::endl;


    return 0;
}