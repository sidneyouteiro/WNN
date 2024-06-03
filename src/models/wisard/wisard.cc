#include <iostream>
#include <vector>
#include "discriminator.cc"
#include <array>

using namespace std;
/*!
 * @class Wisard
 * @brief A class to implement the Wisard algorithm using multiple discriminators.
 */
class Wisard {
public:
    /*!
     * @brief Constructor for the Wisard class.
     * @param entrySize Size of the entry.
     * @param tupleSize Size of the tuple.
     * @param numDiscriminator Number of discriminators.
     */
    Wisard(int entrySize, int tupleSize, int numDiscriminator) :
    //inicializa as variáveis com os valores definidos no construtor
    //Member variable?
    entrySize(entrySize), tupleSize(tupleSize), numDiscriminator(numDiscriminator)
    {
        int i;
        discriminators = vector<Discriminator*>(numDiscriminator);
        //Classe Discriminator

        for (i = 0; i < numDiscriminator; i++) {
            discriminators[i] = new Discriminator(entrySize, tupleSize);
            //Cria os discriminadores com tamanho de entrada e tupla
        }
    }
    
    /*!
     * @brief Destructor for the Wisard class. Frees dynamically allocated memory.
     */
    ~Wisard()
    {
        unsigned int i;
        for (i = 0; i < discriminators.size(); i++) {
            delete discriminators[i];
        }
    }

    /*!
     * @brief Adds a new discriminator to the Wisard.
     */
    void addDiscriminator()
    {
        //emplace back cria um novo último item
        discriminators.emplace_back(new Discriminator(entrySize, tupleSize));
        //incrementa o número de discriminadores
        numDiscriminator++;
    }

    //data é um vetor de vetor do tipo booleano
    //label é um vetor de inteiros
    //O que é o label?
    /*!
     * @brief Trains the Wisard with the given data and labels.
     * @param[in] data Vector of vectors of boolean data.
     * @param[in] label Vector of integer labels.
     */
    void train(const vector<vector<bool>>& data, const vector<int>& label)
    {
        unsigned int i;
        
        //percorre todo o vetor label
        for (i = 0; i < label.size(); i++) {
            //chama o método train do objeto discriminator apontado por discriminators[label[i]] e data[i] é o parâmetro
            discriminators[label[i]]->train(data[i]);
        }
    }

    //rankeamento?
    /*!
     * @brief Ranks the given data and returns the label with the highest response.
     * @param[in] data Vector of boolean data.
     * @return Label with the highest response.
     */
    int rank(const vector<bool>& data)
    {
        int i;
        int label = 0;   
        int max_resp = 0;
        int resp;

        //percorre todos os discriminadores
        for (i = 0; i < numDiscriminator; i++) {
            //chama ela mesma
            resp = discriminators[i]->rank(data);

            //o que é resp?
            //checa resp com o resp máximo
            // armazena o índice do resp máximo e retorna como label
            if (resp > max_resp) {
                max_resp = resp;
                label = i;
            }
        }

        return label;
    }

    //funçao do pybind
    //py::array_t<int> rank(const vector<vector<bool>>& data)
    //{
        //Initialize results with numpy array
        /*py::array_t<int> a({ data.size(), data.size() });
        auto result = a.mutable_unchecked();*/
        //py::array_t<int> a({data.size()});
        //auto label = a.mutable_unchecked();

        //unsigned int i;
        //int j, max_resp, resp;
        
        //for (i = 0; i < data.size(); i++) {
            //max_resp = 0;
            //for ( j = 0; j < numDiscriminator; j++) {
                //resp = discriminators[j]->rank(data[i]);

                //if (resp > max_resp) {
                    //max_resp = resp;
                    //label(i) = j;
                //}
            //}
        //}

        //return a;
    //}

    //ex-func pybind
    //preciso de voce?
    //duas funcoes rank?
    /*!
     * @brief Ranks multiple sets of data and returns a vector of labels.
     * @param[in] data Vector of vectors of boolean data.
     * @return Vector of labels with the highest responses.
     */
    std::vector<size_t> rank(const vector<vector<bool>>& data)
    {
        std::vector<size_t> a({data.size()});

        unsigned int i;
        int j, max_resp, resp;

        for(i = 0; i < data.size(); i++){
            
            max_resp = 0;
            for(j = 0; j < numDiscriminator; j++){
                resp = discriminators[j]->rank(data[i]);

                if(resp > max_resp){
                    max_resp = resp;
                    a[i] = j;
                }
            }
        }
    return a;
    }


    /*!
     * @brief Displays information about the Wisard.
     */
    void info()
    {
	int i;
        int numRams = discriminators[0]->getNumRams();
        long int ramSize = discriminators[0]->getRamBits();
        long int totalRamBits = numRams * ramSize; 
        long int totalBits = numDiscriminator * totalRamBits;

	cout << "Number of Discriminators = " << numDiscriminator << endl;
       
        for (i = 0; i < numDiscriminator; i++) {
            cout << "Discriminator " << i << ": ";
            discriminators[i]->info();
	    //size = size + discriminators[i]->getTupleSize() * discriminators[i]->getNumRams();
        }

	cout << "Size of wisard:" << totalBits << endl;

    }
    
    /*!
     * @brief Returns statistics about the Wisard.
     * @return Array of statistics including number of RAMs, RAM size, total RAM bits, and total bits.
     */
    std::array<unsigned long int, 4> stats()
    {
        std::array<unsigned long int, 4> a({4});
        //auto stats = a.mutable_unchecked();

        int numRams = discriminators[0]->getNumRams();
        long int ramSize = discriminators[0]->getRamBits();
        long int totalRamBits = numRams * ramSize; 
        long int totalBits = numDiscriminator * totalRamBits;

        a[0] = numRams;
        a[1] = ramSize;
        a[2] = totalRamBits;
        a[3] = totalBits;

        return a;
    }
    
    /*!
     * @brief Resets the Wisard by resetting all discriminators.
     */
    void reset()
    {
        int i;

        for (i = 0; i < numDiscriminator; i++) {
            discriminators[i]->reset();
        }
    }

private:
    int entrySize;
    int tupleSize;
    int numDiscriminator;
    vector<Discriminator*> discriminators;
};
