#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include "../include/bitarray.h"

/*!
* @brief Descrição breve do que faz a função
*seconds.
* @param[in] varParaLer,
* @param[out] varParaEscrever
* true if using force mode
* @return float value
*/

using namespace std;

class Discriminator {
public:
    //constructor
    Discriminator(int entrySize, int tupleSize) :
    //tupleSize = ramSize
    entrySize(entrySize), tupleSize(tupleSize)
    {
        //calcula o número de rams e checa se a divisão tem resto, se sim soma mais 1
        numRams = entrySize/tupleSize + ((entrySize%tupleSize) > 0);
        //calloc retorna um * void
        //(int *) -> casting/troca de tipo 
        //Troca o * void para * int, pois vamos lidar com inteiros
        //Bem como inicializa o array com 0
        tuplesMapping = (int *)calloc(entrySize, sizeof(int));
        
        //Generating pseudo-random mapping
        int i;
        //Percorre esse array
        for (i = 0; i < entrySize; i++) {
            //mapeia cada tupla, neste caso é parecido com uma enumeração
            tuplesMapping[i] = i;
        }
        
        //embaralha
        std::shuffle(tuplesMapping, tuplesMapping + entrySize, std::default_random_engine(std::random_device{}()));

        //Aloca memória ram
        rams = (bitarray_t *) calloc(numRams, sizeof(bitarray_t));
        
        int num_bits = (1UL << tupleSize);//2^tuple_size
        long int bitarray_size = (num_bits >> 6); //divide by 64 bits
        //verifica bits restantes
        bitarray_size += ((bitarray_size & 0x3F) > 0); //ceil quotient. If remainder > 0 then sum by 1

        //percorre as rams alocadas
        //preenchendo os elementos com informações
        //várias rams iguais
        //mesmas propriedades
        for (i = 0; i < numRams; i++) {
            rams[i].num_bits = num_bits;
            rams[i].bitarray_size = bitarray_size;
            rams[i].bitarray = (uint64_t *)calloc(rams[i].bitarray_size, sizeof(uint64_t));
        }
    }

    //Destructor
    ~Discriminator() 
    {
        //cout << "Destructor" << endl;
        free(rams);
        free(tuplesMapping);
    }

    //Treino do discriminante?
    void train(bitarray_t * data)
    {
        //vars auxiliares
        int i, j, k = 0, i1, i2;
        //addr_pos?
        int addr_pos;
        uint64_t addr;

        //percorre o número de rams
        for (i = 0; i < numRams; i++) {
            //tupleSize - 1?
            addr_pos = tupleSize-1;
            addr = 0;
            
            //Percorre o tamanho da tupla??
            for (j = 0; j < tupleSize; j++) {
                //arnazena a divisão de tuplesMapping[i] pro 64
                i1 = tuplesMapping[k] >> 6;//Divide by 64 to find the bitarray id
                //armazena o restante para 
                i2 = tuplesMapping[k] & 0x3F;//Obtain remainder to access the bitarray position
                //cria um espécie de endereço utilizando esta operação
                addr |= (((data->bitarray[i1] & (1UL << i2)) >> i2) << addr_pos);
                //vai para a tupla anterior
                addr_pos--;
                k++;
            }

            i1 = addr >> 6;//Divide by 64 to find the bitarray id
            i2 = addr & 0x3F;//Obtain remainder to access the bitarray position
            rams[i].bitarray[i1] |= (1UL << i2); 
        }
    }

    //recebe um vetor de booleano?
    void train(const vector<bool>& data)
    {
        int i, j, k = 0, i1, i2;
        int addr_pos;
        uint64_t addr;

        //percorre as rams
        for (i = 0; i < numRams; i++) {
            //tupleSize-1 ??
            addr_pos = tupleSize-1;
            addr = 0;
            //j && k?
            for (j = 0; (j < tupleSize) && (k < entrySize); j++) {
                addr |= (data[tuplesMapping[k]] << addr_pos);
                addr_pos--;
                k++;
            }

            i1 = addr >> 6;//Divide by 64 to find the bitarray id
            i2 = addr & 0x3F;//Obtain remainder to access the bitarray position
            rams[i].bitarray[i1] |= (1UL << i2); 
        }
    }

    int rank(bitarray_t * data)
    {
        int rank = 0;
        int i, j, k = 0, i1, i2;
        int addr_pos;
        uint64_t addr;

        for (i = 0; i < numRams; i++) {
            addr_pos = tupleSize-1;
            addr = 0;
            
            for (j = 0; j < tupleSize; j++) {
                i1 = tuplesMapping[k] >> 6;//Divide by 64 to find the bitarray id
                i2 = tuplesMapping[k] & 0x3F;//Obtain remainder to access the bitarray position
            
                addr |= (((data->bitarray[i1] & (1UL << i2)) >> i2) << addr_pos);
                addr_pos--;    
                k++;
            }

            i1 = addr >> 6;//Divide by 64 to find the bitarray id
            i2 = addr & 0x3F;//Obtain remainder to access the bitarray position
            rank += (rams[i].bitarray[i1] & (1UL << i2)) >> i2; 
        }

        return rank;
    }

    int rank(const vector<bool>& data)
    {
        int rank = 0;
        int i, j, k = 0, i1, i2;
        int addr_pos;
        uint64_t addr;

        for (i = 0; i < numRams; i++) {
            addr_pos = tupleSize-1;
            addr = 0;
            
            for (j = 0; (j < tupleSize) && (k < entrySize); j++) {
                addr |= (data[tuplesMapping[k]] << addr_pos);
                addr_pos--;    
                k++;
            }

            i1 = addr >> 6;//Divide by 64 to find the bitarray id
            i2 = addr & 0x3F;//Obtain remainder to access the bitarray position
            rank += (rams[i].bitarray[i1] & (1UL << i2)) >> i2; 
        }

        return rank;
    }

    void info() 
    {
        int i;
        long int totalBits = 0;
        cout << "Entry = " << entrySize << ", Tuples = " << tupleSize << ", RAMs = " << numRams << ", RAM size = " << rams[0].num_bits << " bits" << endl;
        
        for (i = 0; i < numRams; i++) {
           // cout << "RAM " << i << " - " << rams[i].num_bits << " bits" << endl;

            /*for (j = 0; j < rams[i].bitarray_size; j++) {
                cout << rams[i].bitarray[j] << ", ";
            }
            cout << endl;*/

            totalBits += rams[i].num_bits;
        }

        cout << "Total Bits = " << totalBits << endl;
    }

    void reset()
    {
        //Generating pseudo-random mapping
        int i, j;

        for (i = 0; i < entrySize; i++) {
            tuplesMapping[i] = i;
        }
        
        std::shuffle(tuplesMapping, tuplesMapping + entrySize, std::default_random_engine(std::random_device{}()));

        for (i = 0; i < numRams; i++) {
            for (j = 0; j < rams[i].bitarray_size; j++) {
                rams[i].bitarray[j] = 0;
            }
        }    
    }

    long int getRamBits() 
    {
        return rams[0].num_bits;
    }

    int getNumRams()
    {
        return numRams;
    }

private:
    int entrySize;
    int tupleSize;
    int numRams;
    int * tuplesMapping;
    bitarray_t * rams;
};
/*
int main(){

    Discriminator * disc = new Discriminator(1024, 16);
    
    vector<bool> data = vector<bool>(1024);
    int i;
    
    for (i = 0; i < 1024; i++) {
        data[i] = i&1;
    }

    for (i = 0; i < 1024; i++) {
        cout << data[i];
    }    
    cout << endl;

    disc->train(data);

    cout << "Rank=" << dec << disc->rank(data) << endl;
    disc->info();

    delete disc;
    return 0;
}
*/

