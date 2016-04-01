void init()
{
    gSystem->Load("libGeom");
    TGeoManager::Import("ALICE_ITS_tgeo.root");
}

TGeoNode* getNode(TString name)
{
    TGeoNode* root = gGeoManager->GetTopNode();
    return dfs(root, name);
}


TGeoNode* dfs(TGeoNode* node, TString name)
{
    if(node->GetName() == name) return node;
    if(node->GetNdaughters() == 0) return NULL;
    
    for(int i = 0; i < node->GetNdaughters(); ++i)
    {
        return dfs(node->GetDaughter(i), name);
    }
}

void export()
{
    /*
    TGeoNode* node1 = getNode("ITSULayer0_1");
    TGeoNode* node2 = getNode("Barrel0_1");

    TGeoVolume* vol = node1->GetVolume();
    vol->RemoveNode(node2);

    TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
    TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum); 
    TGeoVolume* world = gGeoManager->MakeBox("TOPBOX", Vacuum, 1000, 1000, 1000);
    world->AddNode(vol, 0);*/

    //gGeoManager->SetTopVolume(world);
    gROOT->ProcessLine("TGDMLWrite::StartGDMLWriting(gGeoManager,\"all.gdml\",\"\")");
}