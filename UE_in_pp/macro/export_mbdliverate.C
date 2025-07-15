#include <sstream>
#include <fstream>
#include <string>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>

void get_mbdlivecounts(int runnumber, long &live_counts)
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

  sprintf(sql, "select live from gl1_scalers where runnumber = %d and index = 10;", runnumber);
  printf("%s \n" , sql);

  res = db->Query(sql);
  if (res->GetRowCount() == 1 && res->GetFieldCount() == 1) {
    live_counts = stol(res->Next()->GetField(0));
    std::cout << "live_counts " << live_counts << std::endl; 
  } else {
    std::cout << "live_counts unexpected output: rows " << res->GetRowCount() << " fields " << res->GetFieldCount() << std::endl;
  }

  delete res;
  delete db;
}

void get_runlength(int runnumber, double &run_length)
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

  sprintf(sql, "select (extract(epoch from ertimestamp) - extract(epoch from brtimestamp)) as run_length from run where runnumber = %d;", runnumber);
  printf("%s \n" , sql);

  res = db->Query(sql);
  if (res->GetRowCount() == 1 && res->GetFieldCount() == 1) {
    run_length = stod(res->Next()->GetField(0));
    std::cout << "run_length " << run_length << std::endl; 
  } else {
    std::cout << "run_length unexpected output: rows " << res->GetRowCount() << " fields " << res->GetFieldCount() << std::endl;
  }

  delete res;
  delete db;
}

void export_mbdliverate(const std::string &input_file, const std::string &output_file)
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
  outfile << "runnumber live_rate\n";

  int runnumber;
  while (infile >> runnumber)
  {
    long live_counts;
    double run_length;
    get_mbdlivecounts(runnumber, live_counts);
    get_runlength(runnumber, run_length);
    outfile << runnumber << " " << std::fixed << live_counts/run_length << "\n";

  }

  infile.close();
  outfile.close();
}
