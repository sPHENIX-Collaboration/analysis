#include <vector>

void init()
{
    gSystem->Load("libGeom");
    TGeoManager::Import("ALICE_ITS_tgeo.root");
}

std::vector<TGeoNode*> path;
bool nodeFound = false;
void getNode(TString name)
{
    nodeFound = false;
    path.clear();

    dfs(gGeoManager->GetTopNode(), name);

    cout << "Node path: ";
    for(int i = 0; i < path.size(); ++i)
    {
        cout << path[i]->GetName() << " ";
    }
    cout << endl;
}

void dfs(TGeoNode* node, TString name)
{
    //if(nodeFound) return;

    path.push_back(node);
    if(node->GetName() == name)
    {
        nodeFound = true;
        return;
    }

    for(int i = 0; i < node->GetNdaughters(); ++i)
    {
        dfs(node->GetDaughter(i), name);
        if(nodeFound) return;
    }
    path.pop_back();
}

void translation(TString name, TString mothername = "")
{
    //Get the node and path from world
    getNode(name);

    //Get mother node index
    int idx_mother = -1;
    if(mothername == "")
    {
        idx_mother = 0;
    }
    else
    {
        for(int i = 0; i < path.size(); ++i)
        {
            if(path[i]->GetName() == mothername)
            {
                idx_mother = i;
                break;
            }
        }
    }

    //Get the transformation matrix level-by-level
    TGeoHMatrix mat;
    for(int i = idx_mother+1; i < path.size(); ++i)
    {
        mat *= (*path[i]->GetMatrix());
    }
    
    cout << "Translation from " << name << " to " << mothername << endl;
    mat.Print(); 
}
