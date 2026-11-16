#ifndef ROOFIT_IMPORT_TTREE_H
#define ROOFIT_IMPORT_TTREE_H

// auto-creates RooRealVar of appropriate size to encompass all of the data
RooRealVar make_var(const std::string& name, const std::string& title, TTree* tree)
{
  double minval = tree->GetMinimum(name.c_str());
  double maxval = tree->GetMaximum(name.c_str());
  std::cout << "creating RooRealVar " << name << " (" << title << "), min " << minval << " max " << maxval << std::endl;
  return RooRealVar(name.c_str(),title.c_str(),minval,maxval);
}

// imports entire TTree into a RooDataSet, with all RooRealVars sized to accommodate ranges of all branch variables
// this usually crashes for large TTrees, so use more selective methods if at all possible
RooDataSet* import_full_TTree(const std::string& name, const std::string& title, TTree* tree, const std::string& cut)
{
  RooArgList branches;

  TObjArray* branchlist = tree->GetListOfBranches();
  size_t nbranches = tree->GetNbranches();
  for(size_t i=0; i<nbranches; i++)
  {
    TBranch* branch = (TBranch*)branchlist->At(i);
    branches.add(make_var(branch->GetName(),branch->GetTitle(),tree));
  }

  return new RooDataSet(name,title,branches,RooFit::Import(*tree),RooFit::Cut(cut.c_str()));
}

// imports selected branch names of TTree into a RooDataSet, with all RooRealVArs sized to accommodate ranges of corresponding branches
// include both the differential variables and the cut variables in the list of branches needed, else errors will happen
RooDataSet* import_TTree(const std::string& name, const std::string& title, const std::vector<std::string> branchlist, TTree* tree, const std::string& cut)
{
  RooArgList branches;

  for(const std::string& branchname : branchlist)
  {
    branches.add(make_var(branchname.c_str(),branchname.c_str(),tree));
  }

  return new RooDataSet(name,title,branches,RooFit::Import(*tree),RooFit::Cut(cut.c_str()));
}

#endif
