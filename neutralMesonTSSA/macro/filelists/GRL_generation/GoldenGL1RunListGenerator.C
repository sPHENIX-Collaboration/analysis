#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

struct Packet {
    std::string name;
    int eventNumber;
};

bool checkRunSEB(const std::vector<Packet>& packets, std::ofstream& errorFile) {
  bool isgood = true;

  if (packets[0].name != "gl1daq") {
    errorFile << "  Run has no gl1daq packets." << std::endl;
    isgood = false;
  }
  for (int i = 0; i <= 17; ++i) {
    std::string sebName = std::string("seb") + (i < 10 ? "0" : "") + std::to_string(i);
    bool found = false;
    for (const auto& packet : packets) {
      if (packet.name == sebName) {
        found = true;
        break;
      }
    }
    if (!found) {
      errorFile << "  Run has no " << sebName << " packets." << std::endl;
      isgood = false;
    }
  }

  return isgood;
}

bool chekRunEvent(const std::vector<Packet>& packets, std::ofstream& errorFile) {
  bool isgood = true;

  int gl1daqEvents = packets[0].eventNumber;
  if (gl1daqEvents < 500000) {
    errorFile << "  Run has gl1daq event number " << gl1daqEvents << " which is too small." << std::endl;
    isgood = false;
  }

  return isgood;
}

bool checkRunGL1(const std::vector<Packet>& packets, std::ofstream& errorFile) {
  bool isgood = true;

  int gl1daqEvents = packets[0].eventNumber;
  int failedpackets = 0;
  for (size_t i = 1; i < packets.size(); ++i) {
    if (packets[i].eventNumber != gl1daqEvents - 1) {
      int diff = packets[i].eventNumber - gl1daqEvents + 1;
      if (diff < -500) {
        errorFile << "  Packet " << packets[i].name << " has event number " << packets[i].eventNumber << " while big difference in gl1daq with " << gl1daqEvents << std::endl;
        isgood = false;
      } else {
        errorFile << "  Packet " << packets[i].name << " has event number " << packets[i].eventNumber << " while gl1daq has " << gl1daqEvents << std::endl;
        failedpackets++;
      }
    }
  }
  if (failedpackets > 2) {
    errorFile << "  Run has " << failedpackets << " packets with wrong event numbers." << std::endl;
    isgood = false;
  }

  return isgood;
}

void analyzeRuns(const std::string& inputFileName, const std::string& outputFileName1) {
  std::ifstream inputFile(inputFileName);
  std::ofstream outputFile1(outputFileName1);
  std::ofstream errorFile("list_eventnumber_errorlog.txt");

  if (!inputFile.is_open() || !outputFile1.is_open() || !errorFile.is_open()) {
    std::cerr << "Error opening file(s)." << std::endl;
    return;
  }

  std::map<int, std::vector<Packet>> runPackets;
  std::string line;

  while (std::getline(inputFile, line)) {
    std::istringstream iss(line);
    int runNumber, eventNumber;
    std::string packetName;

    if (!(iss >> runNumber >> packetName >> eventNumber)) {
      std::cerr << "Error reading line: " << line << std::endl;
      continue;
    }

    runPackets[runNumber].push_back({packetName, eventNumber});
  }

  int passedRunCount = 0;
  for (const auto& run : runPackets) {
    errorFile << run.first << std::endl;
    if (checkRunSEB(run.second, errorFile) && chekRunEvent(run.second, errorFile) && checkRunGL1(run.second, errorFile)) {
      outputFile1 << run.first << std::endl;
      passedRunCount++;
    }
  }
  std::cout << "Number of GL1 GOLDEN runs saved to " << outputFileName1 << ": " << passedRunCount << std::endl;

  inputFile.close();
  outputFile1.close();
  errorFile.close();
}

void GoldenGL1RunListGenerator() {
  analyzeRuns("EventNumberList_GoldenEmcalRuns.txt", "GoldenGL1RunList.txt");
}


