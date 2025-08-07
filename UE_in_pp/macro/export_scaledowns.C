#include <sstream>
#include <fstream>
#include <string>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>

void get_scaledowns(int runnumber, int scaledowns[])
{

  TSQLServer *db = TSQLServer::Connect("pgsql://sphnxdaqdbreplica:5432/daq","phnxro","");

  if (db)
  {
    printf("Server info: %s\n", db->ServerInfo());
  }
  else
  {
    printf("bad\n");
  }


  TSQLRow *row;
  TSQLResult *res;
  TString cmd = "";
  char sql[1000];


  for (int is = 0; is < 64; is++)
  {
    sprintf(sql, "select scaledown%02d from gl1_scaledown where runnumber = %d;", is, runnumber);
    printf("%s \n" , sql);

    res = db->Query(sql);

    int nrows = res->GetRowCount();

    int nfields = res->GetFieldCount();
    for (int i = 0; i < nrows; i++) {
      row = res->Next();
      for (int j = 0; j < nfields; j++) {
        scaledowns[is] = stoi(row->GetField(j));
      }
      delete row;
    }

    delete res;
  }
  delete db;
}

void export_scaledowns(const std::string &input_file, const std::string &output_file)
{
  std::ifstream infile(input_file);
  std::ofstream outfile(output_file);

  if (!infile.is_open())
  {
    std::cerr << "Failed to open input file.\n";
    return;
  }

  if (!outfile.is_open())
  {
    std::cerr << "Failed to open output file.\n";
    return;
  }

  // Write CSV header
  outfile << "runnumber";
  for (int i = 0; i < 64; ++i)
  {
    outfile << ",scaledown" << std::setw(2) << std::setfill('0') << i;
  }
  outfile << "\n";

  int runnumber;
  while (infile >> runnumber)
  {
    int scaledowns[64] = {0};
    get_scaledowns(runnumber, scaledowns);

    outfile << runnumber;
    for (int i = 0; i < 64; ++i)
    {
      outfile << "," << scaledowns[i];
    }
    outfile << "\n";
  }

  infile.close();
  outfile.close();
}
