#include "../../src/models/wisard/wisard.cc"

int main(){
	std::vector<bool> a = {1,1,1,0,0,0,0,0};
	std::vector<bool> b = {1,1,1,1,0,0,0,0};
	std::vector<bool> c = {0,0,0,0,1,1,1,1};
	std::vector<bool> d = {0,0,0,0,0,1,1,1};
	std::vector<bool> z = {0,1,1,1,1,1,1,1};
	std::vector<int> y = {0,0,1,1};

	//printf("DISCRIMINATOR:\n");
	//Discriminator * disc = new Discriminator(8, 4);
	//disc->train(a);
	//cout << "Rank=" << dec << disc->rank(a) << endl;
	//disc->info();
	
	printf("WISARD\n");
	Wisard * wisard = new Wisard(8, 4 ,2);
	std::vector<vector<bool>> e = {};
	e.push_back(a);
	e.push_back(b);
	e.push_back(c);
	e.push_back(d);
	e.push_back(z);

	wisard->train(e, y);
	cout << "Rank=" << endl;

	std::vector<size_t> r = wisard->rank(e);
	
	for (int register i =0; i < 5; i++){
		cout << r[i] << endl;
	}
	wisard->info();
}
 
