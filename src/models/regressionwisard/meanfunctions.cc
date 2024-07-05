/*!
 * @brief Classe base abstrata para cálculo de médias.
 */
class Mean {
public:
    /*!
     * @brief Cria uma cópia do objeto.
     * @return Ponteiro para a cópia do objeto.
     */
    virtual Mean* clone() const = 0;

    /*!
     * @brief Calcula a média com base nos dados fornecidos.
     * @param outputRams Dados de saída das RAMs.
     * @return A média calculada.
     */
    virtual double calculate(const std::vector<std::vector<double>>& outputRams) = 0;

    /*!
     * @brief Obtém a configuração do objeto em formato JSON.
     * @return Configuração do objeto em JSON.
     */
    virtual nl::json getJSON() const = 0;
};

/*!
 * @brief Classe para cálculo de média simples.
 */
class SimpleMean: public Mean {
public:
    /*!
     * @brief Calcula a média simples com base nos dados fornecidos.
     * @param outputRams Dados de saída das RAMs.
     * @return A média simples calculada.
     */
    double calculate(const std::vector<std::vector<double>>& outputRams) {
        double sumCounters = 0;
        double sumY = 0;
        for (unsigned int i = 0; i < outputRams.size(); i++) {
            sumCounters += outputRams[i][0];
            sumY += outputRams[i][1];
        }

        if (sumCounters > 0)
            return sumY / sumCounters;

        return 0;
    }

    /*!
     * @brief Cria uma cópia do objeto SimpleMean.
     * @return Ponteiro para a cópia do objeto SimpleMean.
     */
    Mean* clone() const {
        return new SimpleMean();
    }

    /*!
     * @brief Obtém o nome da classe.
     * @return Nome da classe.
     */
    std::string className() const {
        return "SimpleMean";
    }

    /*!
     * @brief Obtém a configuração do objeto em formato JSON.
     * @return Configuração do objeto em JSON.
     */
    nl::json getJSON() const {
        nl::json config = {
            {"type", className()}
        };
        return config;
    }
};

/*!
 * @brief Classe para cálculo de média de potência.
 */
class PowerMean: public Mean {
private:
    int power;
public:
    /*!
     * @brief Construtor da classe PowerMean.
     * @param n Potência a ser utilizada no cálculo.
     */
    PowerMean(int n) : power(n) {}

    /*!
     * @brief Calcula a média de potência com base nos dados fornecidos.
     * @param outputRams Dados de saída das RAMs.
     * @return A média de potência calculada.
     */
    double calculate(const std::vector<std::vector<double>>& outputRams) {
        double sumy = 0;
        int counter = 0;
        for (unsigned int i = 0; i < outputRams.size(); i++) {
            if (outputRams[i][0] != 0) {
                sumy += pow(outputRams[i][1] / outputRams[i][0], power);
                counter++;
            }
        }
        if (counter > 0)
            return pow(sumy / counter, 1.0 / power);

        return 0;
    }

    /*!
     * @brief Cria uma cópia do objeto PowerMean.
     * @return Ponteiro para a cópia do objeto PowerMean.
     */
    Mean* clone() const {
        return new PowerMean(power);
    }

    /*!
     * @brief Obtém o nome da classe.
     * @return Nome da classe.
     */
    std::string className() const {
        return "PowerMean";
    }

    /*!
     * @brief Obtém a configuração do objeto em formato JSON.
     * @return Configuração do objeto em JSON.
     */
    nl::json getJSON() const {
        nl::json config = {
            {"type", className()},
            {"power", power}
        };
        return config;
    }
};

/*!
 * @brief Classe para cálculo de mediana.
 */
class Median: public Mean {
public:
    /*!
     * @brief Calcula a mediana com base nos dados fornecidos.
     * @param outputRams Dados de saída das RAMs.
     * @return A mediana calculada.
     */
    double calculate(const std::vector<std::vector<double>>& outputRams) {
        std::vector<double> means(outputRams.size());
        for (unsigned int i = 0; i < outputRams.size(); i++) {
            means[i] = outputRams[i][1] / outputRams[i][0];
        }
        std::sort(means.begin(), means.end());
        if (means.size() % 2 == 0) {
            int index = means.size() / 2;
            return (means[index] + means[index + 1]) / 2.0;
        }
        else {
            int index = (means.size() - 1) / 2 + 1;
            return means[index];
        }
    }

    /*!
     * @brief Cria uma cópia do objeto Median.
     * @return Ponteiro para a cópia do objeto Median.
     */
    Mean* clone() const {
        return new Median();
    }

    /*!
     * @brief Obtém o nome da classe.
     * @return Nome da classe.
     */
    std::string className() const {
        return "Median";
    }

    /*!
     * @brief Obtém a configuração do objeto em formato JSON.
     * @return Configuração do objeto em JSON.
     */
    nl::json getJSON() const {
        nl::json config = {
            {"type", className()}
        };
        return config;
    }
};

/*!
 * @brief Classe para cálculo de média harmônica.
 */
class HarmonicMean: public Mean {
public:
    /*!
     * @brief Calcula a média harmônica com base nos dados fornecidos.
     * @param outputRams Dados de saída das RAMs.
     * @return A média harmônica calculada.
     */
    double calculate(const std::vector<std::vector<double>>& outputRams) {
        double hmean = 0;
        int counter = 0;
        for (unsigned int i = 0; i < outputRams.size(); i++) {
            if (outputRams[i][0] != 0) {
                double amean = outputRams[i][1] / outputRams[i][0];
                if (amean != 0) {
                    hmean += 1.0 / amean;
                    counter++;
                }
            }
        }

        if (counter > 0)
            return counter / hmean;

        return 0;
    }

    /*!
     * @brief Cria uma cópia do objeto HarmonicMean.
     * @return Ponteiro para a cópia do objeto HarmonicMean.
     */
    Mean* clone() const {
        return new HarmonicMean();
    }

    /*!
     * @brief Obtém o nome da classe.
     * @return Nome da classe.
     */
    std::string className() const {
        return "HarmonicMean";
    }

    /*!
     * @brief Obtém a configuração do objeto em formato JSON.
     * @return Configuração do objeto em JSON.
     */
    nl::json getJSON() const {
        nl::json config = {
            {"type", className()}
        };
        return config;
    }
};

/*!
 * @brief Classe para cálculo de média harmônica de potência.
 */
class HarmonicPowerMean: public Mean {
private:
    int power;
public:
    /*!
     * @brief Construtor da classe HarmonicPowerMean.
     * @param n Potência a ser utilizada no cálculo.
     */
    HarmonicPowerMean(int n) : power(n) {}

    /*!
     * @brief Calcula a média harmônica de potência com base nos dados fornecidos.
     * @param outputRams Dados de saída das RAMs.
     * @return A média harmônica de potência calculada.
     */
    double calculate(const std::vector<std::vector<double>>& outputRams) {
        double hmean = 0;
        int counter = 0;
        for (unsigned int i = 0; i < outputRams.size(); i++) {
            if (outputRams[i][0] != 0) {
                double amean = outputRams[i][1] / outputRams[i][0];
                if (amean != 0) {
                    hmean += 1.0 / pow(amean, power);
                    counter++;
                }
            }
        }

        if (counter > 0)
            return pow(counter / hmean, 1.0 / power);

        return 0;
    }

    /*!
     * @brief Cria uma cópia do objeto HarmonicPowerMean.
     * @return Ponteiro para a cópia do objeto HarmonicPowerMean.
     */
    Mean* clone() const {
        return new HarmonicPowerMean(power);
    }

    /*!
     * @brief Obtém o nome da classe.
     * @return Nome da classe.
     */
    std::string className() const {
        return "HarmonicPowerMean";
    }

    /*!
     * @brief Obtém a configuração do objeto em formato JSON.
     * @return Configuração do objeto em JSON.
     */
    nl::json getJSON() const {
        nl::json config = {
            {"type", className()},
