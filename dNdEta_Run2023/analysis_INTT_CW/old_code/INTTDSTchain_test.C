#include "INTTDSTchain.C"

void INTTDSTchain_test()
{
    string folder_direction = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC/";
    vector <string> file_list = {"INTTRecoClusters_test_400_0.root","INTTRecoClusters_test_400_2.root"};

    TChain * chain_in = new TChain("EventTree");
    INTTDSTchain inttDSTread(chain_in,folder_direction,file_list);
    std::printf("inttDSTread N event : %lli \n", chain_in->GetEntries());

    for (int i = 399; i < 401; i++) {
        inttDSTread.LoadTree(i);
        inttDSTread.GetEntry(i);

        cout<<"----------------------------------------------- -----------------------------------------------"<<endl;
        printf(" event : %i, NTruthVtx : %i, NClus : %i, NTrkrhits : %i \n", inttDSTread.event, inttDSTread.NTruthVtx, inttDSTread.NClus, inttDSTread.NTrkrhits);
        printf(" size of ClusX : %i \n", inttDSTread.ClusX -> size());
        printf("triggered VTX ? : %.2f, %.2f, %.2f \n", inttDSTread.TruthPV_trig_x, inttDSTread.TruthPV_trig_y, inttDSTread.TruthPV_trig_z);
        for (int vtx_i = 0; vtx_i < inttDSTread.TruthPV_x -> size(); vtx_i++){
            printf("VTX vec ID-%i : %.2f, %.2f, %.2f \n", vtx_i, inttDSTread.TruthPV_x -> at(vtx_i), inttDSTread.TruthPV_y -> at(vtx_i), inttDSTread.TruthPV_z -> at(vtx_i));
        }
        // cout<<"~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "<<endl;
        // for (int clu_i = 0; clu_i < ClusX -> size(); clu_i++){
        //     printf("cluster %zu : X %.2f, Y %.2f, Z %.2f, sizePhi : %f, sizeZ : %f,  \n", clu_i, ClusX -> at(clu_i), ClusY -> at(clu_i), ClusZ -> at(clu_i), ClusPhiSize -> at(clu_i), ClusZSize -> at(clu_i));
        // }
    }

}