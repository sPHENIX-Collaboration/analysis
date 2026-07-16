#include <fun4all/DBInterface.h>
#include <odbc++/resultset.h>
#include <odbc++/statement.h>

// https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
string exec(const char *cmd)
{
  array<char, 128> buffer;
  string result;
  unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe)
  {
    throw runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
  {
    result += buffer.data();
  }
  return result;
}

float getNEventsFromDAQ(const int& runNumber)
{
  float query = numeric_limits<float>::quiet_NaN();
  odbc::Statement *statement = DBInterface::instance()->getStatement("daq");
  string sqlcmd = "SELECT eventsinrun FROM run WHERE runnumber = " + to_string(runNumber);
  unique_ptr<odbc::ResultSet> resultSet(statement->executeQuery(sqlcmd));
  if (resultSet && resultSet->next())
  {
    query = resultSet->getFloat("eventsinrun");
  }
  return query;
}

string getRunTime(const int& runNumber, const string& begin_end = "brtimestamp")
{
  string query = "dummy value";
  odbc::Statement *statement = DBInterface::instance()->getStatement("daq");
  string sqlcmd = "SELECT " + begin_end + " FROM run WHERE runnumber = " + to_string(runNumber);
  unique_ptr<odbc::ResultSet> resultSet(statement->executeQuery(sqlcmd));
  if (resultSet && resultSet->next())
  {
    query = resultSet->getString(begin_end);
  }
  return query;
}

float getMBD(const int& runNumber)
{
  float query = numeric_limits<float>::quiet_NaN();
  odbc::Statement *statement = DBInterface::instance()->getStatement("daq");
  string sqlcmd = "SELECT avg_value FROM run_timeseries_db_summary WHERE run_number=" + to_string(runNumber) + " AND time_series_name = 'sphenix_cad_sisScaler_Hz'and labels->>'short_name' = 'MBD N\u00b7S'";
  unique_ptr<odbc::ResultSet> resultSet(statement->executeQuery(sqlcmd));
  if (resultSet && resultSet->next())
  {
    query = resultSet->getFloat("avg_value");
  }
  return query;
}


template <typename T>
string to_string_with_precision(const T a_value, const int n = 0)
{
    ostringstream out;
    out.precision(n);
    out << fixed << a_value;
    return out.str();
}

void buildLumiTable()
{
  cout.setf(ios::fixed);

  int sumTotalEvents = 0;
  int sumProceEvents = 0;
  float sumLumi = 0;

  float pp_cross_section_200GeV = 20e6;// Roughly 20 mb for MBD, written here in nb (https://arxiv.org/abs/0704.3599)

 int runList[] = {79514, 79515, 79516, 79526, 79528, 79529}; //
 //int runList[] = {79507, 79509, 79510, 79511, 79512, 79513, 79514, 79515, 79516, 79524, 79525, 79526, 79528, 79529, 79530, 79563, 79565, 79567, 79568, 79570, 79571, 79572, 79593, 79594, 79595, 79596, 79597, 79598, 79599, 79600, 79614, 79617, 79627, 79652, 79653, 79656, 79660, 79707, 79708, 79709, 79711, 79712};

  cout << "  \\begin{tabular}{p{2.5cm}p{2.5cm}p{3.0cm}p{2.5cm}p{2.4cm}}" << endl;
  cout << "    \\toprule[1pt]" << endl;
  cout << "    Run number  & No. recorded triggers [$\\times 10^{6}$] & No. processed triggers [$\\times 10^{6}$] & Analyzed $\\int \\mathcal{L} dt$ [$\\rm nb^{-1}$] & Avg. MBD rate [\\khz] \\\\" << endl;
  cout << "    \\midrule[0.2pt]" << endl;

  for (const int run : runList)
  {
    int nTotalEvents = getNEventsFromDAQ(run);

    float rate = getMBD(run)/1e3; //Average rate in kHz

    string catFileLists = "cat /sphenix/user/aopatton/Feb3Analysis/data/fileLists/file_run" + to_string(run) + "_*.list | wc -l";

    string nProceEvents = exec(catFileLists.c_str());
    nProceEvents.erase(remove(nProceEvents.begin(), nProceEvents.end(), '\n'), nProceEvents.end());

    int i_nProceEvents = stoi(nProceEvents)*1000;

    float lumi = i_nProceEvents/(pp_cross_section_200GeV);// Triggered lumi in inv nb

    cout << "    " << run << " & " << to_string_with_precision(nTotalEvents/1e6, 1) << " & " << to_string_with_precision(i_nProceEvents/1e6, 1) << " & " << to_string_with_precision(lumi, 3) << " & " + to_string_with_precision(rate, 1) + "\\\\" << endl;

    sumTotalEvents += nTotalEvents;
    sumProceEvents += i_nProceEvents;
    sumLumi += lumi;
  }

  cout << "    \\bottomrule[1pt]" << endl;
  cout << "    Total & " << to_string_with_precision(sumTotalEvents/1e6, 1) <<  " & " << to_string_with_precision(sumProceEvents/1e6, 1) << " & " << to_string_with_precision(sumLumi, 1) << " & - \\\\" << endl;
  cout << "    \\bottomrule[1pt]" << endl;
  cout << "  \\end{tabular}" << endl;
}
