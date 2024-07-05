/*!
 * @brief Classe representando a RAM.
 *
 * Classe utilizada para modelar as rams que serão utilizadas no treinamento.
 */
class RegressionRAM
{
public:
    /*!
     * @brief Construtor padrão.
     */
    RegressionRAM() {}

    /*!
     * @brief Construtor da classe que inicializa a RAM com um objeto JSON.
     * @param c Objeto do tipo nl::json contendo a configuração inicial da RAM.
     */
    RegressionRAM(nl::json c)
    {
        nl::json value;

        value = c["memory"];
        std::string data = value.is_null() ? "" : value.get<std::string>();
        if (data.size() > 0) {
            RegressionRAMDataHandle dh(data);
            memory = dh.get(0);
        }

        value = c["mapping"];
        mapping = value.is_null() ? std::vector<int>() : value.get<std::vector<int>>();

        value = c["minZero"];
        minZero = value.is_null() ? 0 : value.get<int>();

        value = c["minOne"];
        minOne = value.is_null() ? 0 : value.get<int>();
    }

    /*!
     * @brief Construtor da classe que inicializa a RAM com parâmetros específicos.
     * @param mapping Vetor de inteiros representando o mapeamento.
     * @param minZero Número mínimo de zeros.
     * @param minOne Número mínimo de uns.
     */
    RegressionRAM(const std::vector<int> mapping, const int minZero = 0, const int minOne = 0) : mapping(mapping), minZero(minZero), minOne(minOne) {}

    /*!
     * @brief Destrutor da classe que limpa a memória.
     */
    ~RegressionRAM()
    {
        mapping.clear();
        memory.clear();
    }

    /*!
     * @brief Treina a RAM com a imagem binária e o valor y.
     * @param image Imagem binária utilizada para o treinamento.
     * @param y Valor associado à imagem.
     */
    void train(const BinInput& image, const double y)
    {
        addr_t index = std::get<0>(getIndex(image));

        auto it = memory.find(index);
        if (it == memory.end())
            memory.insert(it, std::pair<addr_t, regression_content_t>(index, {1, y, 0}));
        else {
            it->second[0]++;
            it->second[1] += y;
        }
    }

    /*!
     * @brief Obtém o voto da RAM para a imagem binária fornecida.
     * @param image Imagem binária.
     * @return Conteúdo da RAM associado à imagem.
     */
    regression_content_t getVote(const BinInput& image) const
    {
        std::tuple<addr_t, bool> result = getIndex(image);
        if (std::get<1>(result)) {
            return {0, 0};
        }
        addr_t index = std::get<0>(result);

        auto it = memory.find(index);
        if (it == memory.end())
            return {0, 0};
        else
            return {it->second[0], it->second[1]};
    }

    /*!
     * @brief Calcula o ajuste da RAM para a imagem binária e o valor yFit.
     * @param image Imagem binária.
     * @param yFit Valor de ajuste.
     */
    void calculateFit(const BinInput& image, const double yFit)
    {
        addr_t index = std::get<0>(getIndex(image));

        auto it = memory.find(index);
        it->second[2] += yFit;
    }

    /*!
     * @brief Aplica o ajuste calculado.
     */
    void applyFit()
    {
        for (auto it = memory.begin(); it != memory.end(); it++)
        {
            it->second[1] += it->second[2] / it->second[0];
            it->second[2] = 0;
        }
    }

    /*!
     * @brief Define o valor mínimo de zeros.
     * @param value Valor mínimo de zeros.
     */
    void setMinZero(const int value)
    {
        minZero = value;
    }

    /*!
     * @brief Define o valor mínimo de uns.
     * @param value Valor mínimo de uns.
     */
    void setMinOne(const int value)
    {
        minOne = value;
    }

    /*!
     * @brief Obtém a memória da RAM.
     * @return Memória da RAM.
     */
    regression_ram_t getMemory() const
    {
        return memory;
    }

    /*!
     * @brief Obtém o mapeamento da RAM.
     * @return Vetor de inteiros representando o mapeamento.
     */
    std::vector<int> getMapping() const
    {
        return mapping;
    }

    /*!
     * @brief Obtém a configuração da RAM em formato JSON.
     * @return Objeto JSON representando a configuração da RAM.
     */
    nl::json getJSON() const
    {
        RegressionRAMDataHandle dh(memory);
        nl::json config =
        {
            {"minZero", minZero},
            {"minOne", minOne},
            {"mapping", mapping},
            {"memory", memory},
        };
        return config;
    }

    /*!
     * @brief Obtém o tamanho da RAM em bytes.
     * @return Tamanho da RAM em bytes.
     */
    long getsizeoff() const
    {
        long size = sizeof(RegressionRAM);
        size += mapping.size() * sizeof(addr_t);
        size += memory.size() * (sizeof(addr_t) + sizeof(regression_content_t));
        return size;
    }

protected:
    /*!
     * @brief Obtém o índice da imagem binária.
     * @param image Imagem binária.
     * @return Tupla contendo o índice e um booleano indicando se os critérios mínimos foram atendidos.
     */
    std::tuple<addr_t, bool> getIndex(const BinInput& image) const
    {
        addr_t index = 0;
        addr_t p = 1;
        int countOne = 0;
        for (unsigned int i = 0; i < mapping.size(); i++)
        {
            int bin = image[mapping[i]];
            countOne += bin;
            index += bin * p;
            p *= 2;
        }
        if ((countOne < minOne) || (((int)mapping.size() - countOne) < minZero))
        {
            return std::make_tuple(index, true);
        }

        return std::make_tuple(index, false);
    }

private:
    regression_ram_t memory; ///< Memória da RAM.
    std::vector<int> mapping; ///< Mapeamento da RAM.
    int minZero; ///< Número mínimo de zeros.
    int minOne; ///< Número mínimo de uns.
};
