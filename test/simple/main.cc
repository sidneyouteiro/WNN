#include "./src/wisard.cc"

int main(){
	std::vector<bool> a = {1,0,0,0,0,0,1,0,1,0,1,0,1,1,1,1,0,0,1,0};
	std::vector<bool> b = {0,0,1,1,0,1,0,0,0,0,0,1,0,1,1,1,0,1,0,0};
	std::vector<int> y = {0,1};

	printf("DISCRIMINATOR:\n");
	Discriminator * disc = new Discriminator(20, 4);
	disc->train(a);
	cout << "Rank=" << dec << disc->rank(a) << endl;
	disc->info();
	
	printf("WISARD\n");
	Wisard * wisard = new Wisard(20, 4 ,2);
	std::vector<vector<bool>> c = {};
	c.push_back(a);
	c.push_back(b);

	wisard->train(c, y);
	cout << "Rank=" << endl;

	std::vector<size_t> r = wisard->rank(c);
	
	for (int register i =0; i < 2; i++){
		cout << r[i] << endl;
	}
	wisard->info();
}
 