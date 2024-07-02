class RegressionWisard: public RegressionModel{
public:
    //pq vc?   
    RegressionWisard(){};

    RegressionWisard(nl::json c){
      srand(randint(0,1000000));
      nl::json value;

      value = c["addressSize"];
      int addressSize = value.is_null() ? 3 : value.get<int>();

      value = c["completeAddressing"];
      bool completeAddressing = value.is_null() ? true : value.get<bool>();

      value = c["orderedMapping"];
      bool orderedMapping = value.is_null() ? false : value.get<bool>();

      value = c["minZero"];
      int minZero = value.is_null() ? 0 : value.get<int>();

      value = c["minOne"];
      int minOne = value.is_null() ? 0 : value.get<int>();

      value = c["steps"];
      int steps = value.is_null() ? 0 : value.get<int>();

      value = c["numberOfTrainings"];
      int numberOfTrainings = value.is_null() ? 0 : value.get<int>();

      value = c["entrySize"];
      int entrySize = value.is_null() ? 0 : value.get<int>();

      value = c["mapping"];
      std::vector<int> mapping = value.is_null() ? std::vector<int>() : value.get<std::vector<int>>();

      //value = c["mean"];
      //mean = MeanHelper::load(value);

      value = c["data"];
      std::vector<nl::json> data = value.is_null() ? std::vector<nl::json>() : value.get<std::vector<nl::json>>();

      for (size_t i = 0; i < data.size(); i++){
        rams.push_back(RegressionRAM(data[i]));
      }
    }
    
    RegressionWisard(int addressSize) : addressSize(addressSize) {}

    ~RegressionWisard(){
        rams.clear();
    }

    void train(const DataSet& dataset) {
        for (size_t i = 0; i < dataset.size(); i++){
        train(dataset[i], dataset.getY(i));
        }

        for (int j = 0; j < steps; j++){
            for (size_t i = 0; i < dataset.size(); i++){
                calculateFit(dataset[i], dataset.getY(i));
            }
            applyFit();
        }   
    }

    void train(const BinInput& input, const double y){
        //se as rams estao vazias, shuffle nelas
        if (rams.empty()) setRAMShuffle(input.size());

        checkEntrySize(input.size());
        numberOfTrainings++;
        
        for (size_t i = 0; i < rams.size(); i++){
        rams[i].train(input, y);
        }
    }
    
    double predict(const BinInput& input) const {
        checkEntrySize(input.size());
        std::vector<regression_content_t> outputRams(rams.size());
        for (size_t i = 0; i < rams.size(); i++){
            outputRams[i] = rams[i].getVote(input);
        }
        return mean->calculate(outputRams);
    }

    std::vector<double> predict(const DataSet& dataset) const {
        std::vector<double> output(dataset.size());

        for (size_t i = 0; i < dataset.size(); i++){
        output[i] = predict(dataset[i]);
        }

        return output;
    }

    void calculateFit(const BinInput& input, const double y){
        double yPredicted = predict(input);
        for (size_t i = 0; i < rams.size(); i++){
            rams[i].calculateFit(input, (y - yPredicted));
        }
    }

    void applyFit(){
        for (size_t i = 0; i < rams.size(); i++){
            rams[i].applyFit();
        }
  }

protected:
    void setRAMShuffle(int entrySize){
      this->entrySize = entrySize;
      checkAddressSize(entrySize, addressSize);

      if(mapping.size()==0){
          int numberOfRAMS = entrySize / addressSize;
          int remain = entrySize % addressSize;
          int indexesSize = entrySize;
          if(completeAddressing && remain > 0) {
              numberOfRAMS++;
              indexesSize += addressSize-remain;
          }

          rams.resize(numberOfRAMS);
          mapping.resize(indexesSize);

          for (int i = 0; i < entrySize; i++){
              mapping[i]=i;
          }
          for (size_t i=entrySize; i<mapping.size(); i++){
              mapping[i] = randint(0, entrySize-1, false);
          }

          if(!orderedMapping){
              random_shuffle(mapping.begin(), mapping.end());
          }
      }
      else{
        rams.resize(mapping.size()/addressSize);
      }

      for (size_t i = 0; i < rams.size(); i++){
        std::vector<int> subIndexes(mapping.begin() + (((int)i)*addressSize), mapping.begin() + ((((int)i)+1)*addressSize));
        rams[i] = RegressionRAM(subIndexes, minZero, minOne);
      }
    }

    void checkEntrySize(const int entry) const {
      if(entrySize != entry){
        throw Exception("The entry size defined on creation of RAM is different of entry size given as input!");
      }
    }

    void checkAddressSize(const int entrySize, const int addressSize) const{
      if( addressSize < 2){
        throw Exception("The address size cann't be lesser than 2!");
      }
      if( entrySize < 2 ){
        throw Exception("The entry size cann't be lesser than 2!");
      }
      if( entrySize < addressSize){
        throw Exception("The address size cann't be bigger than entry size!");
      }
    }

    void checkMinZeroOne(int min0, int min1){
      if(min0+min1 > addressSize){
        throw Exception("minZero + minOne is bigger than addressSize!");
      }
    }
    
    int addressSize;
    bool completeAddressing;
    bool orderedMapping;
    Mean* mean;
    int minZero;
    int minOne;
    int steps;

    int numberOfTrainings;
    int entrySize;
    //chamo regressionRAN
    std::vector<RegressionRAM> rams;
    std::vector<int> mapping;
};


