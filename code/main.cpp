#include"seamcarving.h"
int main()
{	
	Seamcarving seamcarving;
	seamcarving.Load("1.jpg");
	int H, W;
	seamcarving.GetSize(H,W);
	seamcarving.Resize(H+100,W+100);
	//seamcarving.Seamlinev(seamcarving.image);
//	seamcarving.Remove();
	return 0;
}