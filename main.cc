#include "./src/wisard.cc"

int main(){
	std::vector<bool> a = {1,0,0,0,0,0,1,0,1,0,1,0,1,1,1,1,0,0,1,0};
	std::vector<bool> b = {0,0,1,1,0,1,0,0,0,0,0,1,0,1,1,1,0,1,0,0};

	printf("DISCRIMINATOR:");
	Discriminator * disc = new Discriminator(20, 4);
	disc->train(a);
	cout << "Rank=" << dec << disc->rank(a) << endl;
	disc->info();
	
	printf("WISARD");
	Wisard * wisard = new Wisard(20, 4 ,2);
	bool c[20][20];

	wisard->train(c, );
}
