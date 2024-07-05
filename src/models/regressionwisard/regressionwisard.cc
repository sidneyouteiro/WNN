/*!
 * @brief Classe para um modelo de regressão utilizando Wisard.
 */
class RegressionWisard: public RegressionModel {
public:
    /*!
     * @brief Construtor padrão.
     */
    RegressionWisard() {}

    /*!
     * @brief Construtor que inicializa o modelo com um objeto JSON.
     * @param c Objeto do tipo nl::json contendo a configuração inicial do modelo.
     */
    RegressionWisard(nl::json c) {
        srand(randint(0, 1000000));
        nl::json value;

        value = c["addressSize"];
        addressSize = value.is_null() ? 3 : value.get<int>();

        value = c["completeAddressing"];
        completeAddressing = value.is_null() ? true : value.get<bool>();

        value = c["orderedMapping"];
        orderedMapping = value.is_null() ? false : value.get<bool>();

        value = c["minZero"];
        minZero = value.is_null() ? 0 : value.get<int>();

        value = c["minOne"];
        minOne = value.is_null() ? 0 : value.get<int>();

        value = c["steps"];
        steps = value.is_null() ? 0 : value.get<int>();

        value = c["numberOfTrainings"];
        numberOfTrainings = value.is_null() ? 0 : value.get<int>();

        value = c["entrySize"];
        entrySize = value.is_null() ? 0 : value.get<int>();

        value = c["mapping"];
        mapping = value.is_null() ? std::vector<int>() : value.get<std::vector<int>>();

        value = c["data"];
        std::vector<nl::json> data = value.is_null() ? std::vector<nl::json>() : value.get<std::vector<nl::json>>();

        for (size_t i = 0; i < data.size(); i++) {
            rams.push_back(RegressionRAM(data[i]));
        }
    }
    
    /*!
     * @brief Construtor que inicializa o modelo com um tamanho de endereço especificado.
     * @param addressSize Tamanho do endereço.
     */
    RegressionWisard(int addressSize) : addressSize(addressSize) {}

    /*!
     * @brief Destrutor da classe que limpa a memória.
     */
    ~RegressionWisard() {
        rams.clear();
    }

    /*!
     * @brief Treina o modelo com um conjunto de dados.
     * @param dataset Conjunto de dados utilizado para o treinamento.
     */
    void train(const DataSet& dataset) {
        for (size_t i = 0; i < dataset.size(); i++) {
            train(dataset[i], dataset.getY(i));
        }

        for (int j = 0; j < steps; j++) {
            for (size_t i = 0; i < dataset.size(); i++) {
                calculateFit(dataset[i], dataset.getY(i));
            }
            applyFit();
        }   
    }

    /*!
     * @brief Treina o modelo com uma entrada binária e um valor y.
     * @param input Entrada binária.
     * @param y Valor associado à entrada.
     */
    void train(const BinInput& input, const double y) {
        if (rams.empty()) setRAMShuffle(input.size());
        checkEntrySize(input.size());
        numberOfTrainings++;
        
        for (size_t i = 0; i < rams.size(); i++) {
            rams[i].train(input, y);
        }
    }
    
    /*!
     * @brief Faz a predição para uma entrada binária.
     * @param input Entrada binária.
     * @return O valor predito.
     */
    double predict(const BinInput& input) const {
        checkEntrySize(input.size());
        std::vector<regression_content_t> outputRams(rams.size());
        for (size_t i = 0; i < rams.size(); i++) {
            outputRams[i] = rams[i].getVote(input);
        }
        return mean->calculate(outputRams);
    }

    /*!
     * @brief Faz a predição para um conjunto de dados.
     * @param dataset Conjunto de dados.
     * @return Vetor com os valores preditos.
     */
    std::vector<double> predict(const DataSet& dataset) const {
        std::vector<double> output(dataset.size());

        for (size_t i = 0; i < dataset.size(); i++) {
            output[i] = predict(dataset[i]);
        }

        return output;
    }

    /*!
     * @brief Calcula o ajuste para uma entrada binária e um valor y.
     * @param input Entrada binária.
     * @param y Valor associado à entrada.
     */
    void calculateFit(const BinInput& input, const double y) {
        double yPredicted = predict(input);
        for (size_t i = 0; i < rams.size(); i++) {
            rams[i].calculateFit(input, (y - yPredicted));
        }
    }

    /*!
     * @brief Aplica o ajuste calculado.
     */
    void applyFit() {
        for (size_t i = 0; i < rams.size(); i++) {
            rams[i].applyFit();
        }
    }

protected:
    /*!
     * @brief Configura o embaralhamento das RAMs.
     * @param entrySize Tamanho da entrada.
     */
    void setRAMShuffle(int entrySize) {
        this->entrySize = entrySize;
        checkAddressSize(entrySize, addressSize);

        if (mapping.size() == 0) {
            int numberOfRAMS = entrySize / addressSize;
            int remain = entrySize % addressSize;
            int indexesSize = entrySize;
            if (completeAddressing && remain > 0) {
                numberOfRAMS++;
                indexesSize += addressSize - remain;
            }

            rams.resize(numberOfRAMS);
            mapping.resize(indexesSize);

            for (int i = 0; i < entrySize; i++) {
                mapping[i] = i;
            }
            for (size_t i = entrySize; i < mapping.size(); i++) {
                mapping[i] = randint(0, entrySize - 1, false);
            }

            if (!orderedMapping) {
                random_shuffle(mapping.begin(), mapping.end());
            }
        } else {
            rams.resize(mapping.size() / addressSize);
        }

        for (size_t i = 0; i < rams.size(); i++) {
            std::vector<int> subIndexes(mapping.begin() + (((int)i) * addressSize), mapping.begin() + ((((int)i) + 1) * addressSize));
            rams[i] = RegressionRAM(subIndexes, minZero, minOne);
        }
    }

    /*!
     * @brief Verifica o tamanho da entrada.
     * @param entry Tamanho da entrada.
     */
    void checkEntrySize(const int entry) const {
        if (entrySize != entry) {
            throw Exception("The entry size defined on creation of RAM is different of entry size given as input!");
        }
    }

    /*!
     * @brief Verifica o tamanho do endereço.
     * @param entrySize Tamanho da entrada.
     * @param addressSize Tamanho do endereço.
     */
    void checkAddressSize(const int entrySize, const int addressSize) const {
        if (addressSize < 2) {
            throw Exception("The address size can't be less than 2!");
        }
        if (entrySize < 2) {
            throw Exception("The entry size can't be less than 2!");
        }
        if (entrySize < addressSize) {
            throw Exception("The address size can't be bigger than entry size!");
        }
    }

    /*!
     * @brief Verifica os valores mínimos de zeros e uns.
     * @param min0 Valor mínimo de zeros.
     * @param min1 Valor mínimo de uns.
     */
    void checkMinZeroOne(int min0, int min1) {
        if (min0 + min1 > addressSize) {
            throw Exception("minZero + minOne is bigger than addressSize!");
        }
    }

private:
    int addressSize; ///< Tamanho do endereço.
    bool completeAddressing; ///< Endereçamento completo.
    bool orderedMapping; ///< Mapeamento ordenado.
    Mean* mean; ///< Objeto para cálculo de média.
    int minZero; ///< Número mínimo de zeros.
    int minOne; ///< Número mínimo de uns.
    int steps; ///< Número de passos.
    int numberOfTrainings; ///< Número de treinamentos.
    int entrySize; ///< Tamanho da entrada.
    std::vector<RegressionRAM> rams; ///< Vetor de RAMs de regressão.
    std::vector<int> mapping; ///< Mapeamento dos índices.
};
