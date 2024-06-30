/*!
* @brief Classe representando a RAM.
*
*Classe utilizada para modelar as rams que serão utilizadas no treinamento
*/
//classe da RegressionRam
class RegressionRAM
{
public:
        /*
        Construtor da classe
        c é um objeto do tipo nl::json
        */
    	RegressionRAM(nl::json c)
	{
		nl::json value;

		value = c["memory"];

		std::string data = value.is_null() ? "" : value.get<std::string>();
		if(data.size() > 0){
			RegressionRAMDataHandle dh(data);
			memory = dh.get(0);
		}

		value = c["mapping"];
		mapping = value.is_null() ? std::vector<int>() : value.get<std::vector<int>>();

		value c["minZero"];
		minZero = value.is_null() ? 0 : value.get<int>();

		value = c["minOne"];
		minOne = value.is_null() ? 0 : value.get<int>;
	}

	//Limpar memória
	~RegressionRAM()
    {
		mapping.clear();
		memory.clear();
	}

	void train(const BinInput& image, const double y)
	{
	    addr_t index = std::get<0>(getIndex(image));

        auto it = memory.find(index);
        if (it == memory.end())
            memory.insert(it, std::pair<addr_t, regression_content_t>(index, {1, y, 0}));
	    else{
            it->second[0]++;
            it->secomd[1] += y;
        }
    }
    
    regression_content_t getVote(const BinInput& image) const
    {
        std::tuple<addr_t, bool> result = getIndex(image);
        if (std::get<1>(result)){
            return{0, 0};
        }
        addr_t index = std::get<0>(result);

        auto it = memory.find(index);
        if (it == memory.end())
            return {0, 0};
        else
            return {it->second[0], it->second[1]};
    }

    void calculateFit(const BinInput& image, const double yFit)
    {
        addr_t index = std::get<0>(getIndex(image));

        auto it = memory.find(index);
        it->second[2] += yFit;
    }

    void applyFit()
    {
        for (auto it = memory.begin(); it != memory.end(); it++)
        {
            it->second[1] += it->second[2] / it->second[0];
            it->second[2] = 0;
        }
    }

    void setMinZero(const int value)
    {
        minZero = value;
    }

    void setMinOne(const int value)
    {
        minOne = value;
    }

    regression_ram_t getMemory() const
    {
        return memory;
    }

    std::vector<int> getMapping() const
    {
        return mapping;
    }

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

    long getsizeoff() const
    {
        long size = sizeof(RegressionRAM);
        size += mapping.size()*sizeof(addr_t);
        size += memory.size()*(sizeof(addr_t)+sizeof(regression_content_t));
        return size;
    }

protected:
    std::tuple<addr_t, bool> getIndex(const BinInput& image) const
    {
        addr_t index = 0;
        addr_t p =1;
        int countOne = 0;
        for(unsigned int i=0; i<mapping.size(); i++)
        {
            int bin = image[mapping[i]];
            countOne += bin;
            index += bin*p;
            p *= 2;
        }
        if((countOne < minOne) || (((int)mapping.size() - countOne) < minZero))
        {
            return std::make_tuple(index, true);
        }

        return std::make_tuple(index, false);
    }

private:
    regression_ram_t memory;
    std::vector<int> mapping;
    int minZero;
    int minOne;

};
