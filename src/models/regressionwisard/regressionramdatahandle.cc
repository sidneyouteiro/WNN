/*!
 * @class RegressionRAMDataHandle
 * @brief A class to handle RAM data for regression tasks.
 */
class RegressionRAMDataHandle {
public:
  /*!
   * @brief Constructor that initializes the object with input data.
   * @param inputdata The input data as a string.
   */
  RegressionRAMDataHandle(std::string inputdata) {
    setData(inputdata);
  }

  /*!
   * @brief Constructor that initializes the object with a single regression RAM data.
   * @param data A single regression RAM data.
   */
  RegressionRAMDataHandle(regression_ram_t data) {
    ramdata[0] = data;
  }

  /*!
   * @brief Constructor that initializes the object with multiple regression RAM data.
   * @param data A vector containing multiple regression RAM data.
   */
  RegressionRAMDataHandle(std::vector<regression_ram_t> data) {
    for (size_t i = 0; i < data.size(); i++) {
      ramdata[i] = data[i];
    }
  }

  /*!
   * @brief Default constructor.
   */
  RegressionRAMDataHandle() {}

  /*!
   * @brief Gets the content of a specific RAM at a specific address.
   * @param r The RAM index.
   * @param addr The address within the RAM.
   * @return The content at the specified RAM and address.
   */
  regression_content_t get(int r, int addr) {
    return ramdata[r][addr];
  }

  /*!
   * @brief Gets the entire content of a specific RAM.
   * @param r The RAM index.
   * @return The entire RAM content.
   */
  regression_ram_t get(int r) {
    return ramdata[r];
  }

  /*!
   * @brief Sets the content at a specific RAM and address.
   * @param r The RAM index.
   * @param addr The address within the RAM.
   * @param value The value to set at the specified RAM and address.
   */
  void set(int r, int addr, regression_content_t value) {
    ramdata[r][addr] = value;
  }

  /*!
   * @brief Sets the entire content of a specific RAM.
   * @param r The RAM index.
   * @param value The value to set for the specified RAM.
   */
  void set(int r, regression_ram_t value) {
    ramdata[r] = value;
  }

  /*!
   * @brief Compares this object with another RegressionRAMDataHandle object.
   * @param other The other RegressionRAMDataHandle object to compare with.
   * @return True if both objects are equal, otherwise false.
   */
  bool compare (RegressionRAMDataHandle& other) {
    if(ramdata.size() != other.ramdata.size()) return false;
    for(std::unordered_map<int,regression_ram_t>::iterator it = ramdata.begin(); it != ramdata.end(); ++it) {
      std::unordered_map<int,regression_ram_t>::iterator ito = other.ramdata.find(it->first);
      if(ito == other.ramdata.end() || it->second.size() != ito->second.size()) return false;

      for(regression_ram_t::iterator itram = it->second.begin(); itram != it->second.end(); ++itram) {
        regression_ram_t::iterator itramo = ito->second.find(itram->first);
        if(itramo == ito->second.end() || itram->second != itramo->second) return false;
      }
    }
    return true;
  }

  /*!
   * @brief Gets the entire data as a string.
   * @return The entire data as a string.
   */
  std::string data() {
    std::string out;
    for(unsigned int i=0; i<ramdata.size(); i++) {
      out += (i != 0 ? "." : "") + data(i);
    }
    return out;
  }

  /*!
   * @brief Gets the data of a specific RAM as a string.
   * @param r The RAM index.
   * @return The data of the specified RAM as a string.
   */
  std::string data(int r) {
    int blockSize = (sizeof(addr_t)+(3*sizeof(double)));
    std::string out(ramdata[r].size()*blockSize,0);
    int k=0;
    for(auto j=ramdata[r].begin(); j!=ramdata[r].end(); ++j) {
      out.replace(k,sizeof(addr_t),convertToBytes(j->first));
      for (size_t i = 0; i < j->second.size(); i++) {
        out.replace(k+sizeof(addr_t)+(i*sizeof(double)),sizeof(double),convertToBytes(j->second[i]));
      }
      k += blockSize;
    }
    return Base64::encode(out);
  }

  /*!
   * @brief Saves the data to a file with a given prefix.
   * @param prefix The prefix for the filename.
   */
  void save(std::string prefix) {
    std::string filename = prefix + ramdata_sufix;
    std::ofstream dataFile;
    dataFile.open(filename, std::ios::app);

    for(unsigned int i=0; i<ramdata.size(); i++) {
      dataFile << (i != 0 ? "." : "") + data(i);
    }

    dataFile.close();
  }

private:
  /*!
   * @brief Sets the data from an input string.
   * @param inputdata The input data as a string.
   */
  void setData(std::string inputdata) {
    int s = ramdata_sufix.size();
    if(inputdata.substr(inputdata.size()-s,s).compare(ramdata_sufix) == 0) {
      std::ifstream dataFile;
      dataFile.open(inputdata);
      if(dataFile.is_open()) {
        while(true) {
          if(dataFile.eof()) break;

          std::string rdata="";
          std::getline(dataFile,rdata,'.');
          ramdata[ramdata.size()] = dataToRam(rdata);
        }
        dataFile.close();
      }
    }
    else {
      int pos=0;
      unsigned int found = 0;
      while(found < inputdata.size()) {
        found = inputdata.find('.',pos);
        ramdata[ramdata.size()] = dataToRam(inputdata.substr(pos,found-pos));
        pos = found+1;
      }
    }
  }

  /*!
   * @brief Converts a string to RAM data.
   * @param inputdata The input data as a string.
   * @return The converted RAM data.
   */
  regression_ram_t dataToRam(const std::string& inputdata) {
    regression_ram_t ramout;
    std::string decodedData = Base64::decode(inputdata);
    const int blockSize = (sizeof(addr_t)+(3*sizeof(double)));
    if(decodedData.size()%blockSize != 0) return ramout;

    for(unsigned long i=0; i<decodedData.size(); i+=blockSize) {
      addr_t address = convertToValue<addr_t>(decodedData.substr(i,sizeof(addr_t)));
      regression_content_t content;
      for (size_t j = 0; j < 3; j++) {
        double value = convertToValue<double>(decodedData.substr(i+sizeof(addr_t)+(j*sizeof(double)),sizeof(double)));
        content.push_back(value);
      }
      ramout[address]=content;
    }
    return ramout;
  }

  std::unordered_map<int, regression_ram_t> ramdata; ///< Container for the RAM data.
};

// Helper functions to convert data to bytes and back, and Base64 encoding/decoding should be defined elsewhere
