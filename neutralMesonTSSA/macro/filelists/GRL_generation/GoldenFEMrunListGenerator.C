#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

bool check_clock_file(const std::string &clock_file_path) {
  std::ifstream clock_file(clock_file_path);

  std::string line;
  std::vector<int> section_positions(18, -1);
  std::vector<int> section_clock_value(18, -1);
  int line_number = 0;

  std::regex section_regex(R"(^[0-9]+$)");
  std::regex clock_value_regex(R"(^\s+[0-9]+(\s+[0-9]+)*$)");

  while (std::getline(clock_file, line)) {
    line_number++;

    if (std::regex_match(line, section_regex)) {
      int section_number = std::stoi(line);

      if (section_number < 0 || section_number > 17) {
        return false;
      }
      if (section_positions[section_number] != -1) {
        return false;
      }

      section_positions[section_number] = line_number;

      for (int i = 0; i < 8; ++i) {
        if (!std::getline(clock_file, line)) {
          return false;
        }
        line_number++;
        if (!std::regex_match(line, clock_value_regex)) {
          return false;
        }
        std::istringstream clock_line(line);
        int clock_value;
        while (clock_line >> clock_value) {
          if (section_clock_value[section_number] == -1) {
            section_clock_value[section_number] = clock_value;
          } else if (clock_value != section_clock_value[section_number]) {
            return false;
          }
        }
      }
    }
  }

  for (int i = 0; i < 18; ++i) {
    if (section_positions[i] == -1) {
      return false;
    }
  }

  return true;
}

void GoldenFEMrunListGenerator() {
  std::string runnumber_file_path = "GoldenGL1RunList.txt";
  std::string output_file_path = "GoldenFEMrunList.txt";
  std::ifstream runnumber_file(runnumber_file_path);
  std::ofstream output_file(output_file_path);

  int passedRunCount = 0;
  std::string runnumber;
  while (std::getline(runnumber_file, runnumber)) {
    std::string clock_file_path = "list_allFEM_clock/FEM_clock_" + runnumber + ".txt";
    if (check_clock_file(clock_file_path)) {
      output_file << runnumber << std::endl;
      passedRunCount++;
    }
  }

  std::cout << "Number of FEM GOLDEN runs saved to " << output_file_path << ": " << passedRunCount << std::endl;
  runnumber_file.close();
  output_file.close();
}


