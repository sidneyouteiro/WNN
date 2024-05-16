/*!
* @brief Classe representando a RAM.
*
*Classe utilizada para modelar as rams que serão utilizadas no treinamento
*/
class RegressionRam{
public:
    	RegressionRam(nl:json c)
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
	~RegressionRam()
	{
		mapping.clear();
		memory.clear();
	}

	void train()
	{
	
	}
};
