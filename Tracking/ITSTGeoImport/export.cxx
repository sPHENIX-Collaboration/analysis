void init()
{
    gSystem->Load("libGeom");
    TGeoManager::Import("ALICE_ITS_tgeo.root");
}

typedef TGeoNode* pTGeoNode;
pTGeoNode getNode(pTGeoNode root, TString name)
{
    pTGeoNode node = NULL;
    dfs(root, name, node);

    cout << name;
    if(node != NULL)
    {
        cout << " Found!" << endl;
    }
    else
    {
        cout << " NOT Found!" << endl;
    }
    return node;
}


void dfs(pTGeoNode node, TString name,  pTGeoNode& res)
{
    if(res != NULL) return;
    if(node->GetName() == name) res = node;
    if(node->GetNdaughters() == 0) return;
    
    for(int i = 0; i < node->GetNdaughters(); ++i)
    {
        dfs(node->GetDaughter(i), name, res);
    }
}

void export()
{
    pTGeoNode root = gGeoManager->GetTopNode();

    pTGeoNode node_inner = getNode(root, "ITSUWrapVol0_1");
    pTGeoNode nodeb1 = getNode(node_inner, "Barrel0_1");
    pTGeoNode nodeb2 = getNode(node_inner, "Barrel1_1");
    pTGeoNode nodeb3 = getNode(node_inner, "Barrel2_1");

    TGeoVolume* vol_inner = node_inner->GetVolume();
    vol_inner->RemoveNode(nodeb1);
    vol_inner->RemoveNode(nodeb2);
    vol_inner->RemoveNode(nodeb3);

    pTGeoNode node_mid = getNode(root, "ITSUWrapVol1_1");
    pTGeoNode nodeb4 = getNode(node_mid, "Barrel3_1");
    pTGeoNode nodeb5 = getNode(node_mid, "Barrel4_1");

    TGeoVolume* vol_mid = node_mid->GetVolume();
    vol_mid->RemoveNode(nodeb4);
    vol_mid->RemoveNode(nodeb5);

    pTGeoNode node_outer = getNode(root, "ITSUWrapVol2_1");
    pTGeoNode nodeb6 = getNode(node_outer, "Barrel5_1");
    pTGeoNode nodeb7 = getNode(node_outer, "Barrel6_1");

    TGeoVolume* vol_outer = node_outer->GetVolume();
    vol_outer->RemoveNode(nodeb6);
    vol_outer->RemoveNode(nodeb7);   

    pTGeoNode top = getNode(root, "ITSV_2");
    gGeoManager->SetTopVolume(top->GetVolume());
    gROOT->ProcessLine("TGDMLWrite::StartGDMLWriting(gGeoManager,\"all.gdml\",\"\")");
}