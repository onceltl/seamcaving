#ifndef SEAMCARVING_H
#define SEAMCARVING_H
#include <opencv2/opencv.hpp>
class Seamcarving
{
	cv::Mat image_gray,Timage,Rimage;//
	cv::Mat energyx, energyy,energy;//dx,dy,energy=abs(dx)+abs(dy)
	std::vector<int> rec;//记录dp方案
	std::vector<int> F;//DP
	std::vector<int> record;
	std::vector<int> seamtot;
	bool need;//是否消除对象
	void GetSobelEnergy(cv::Mat &image);//计算Sobel能量
	void GetLaplacianEnergy(cv::Mat &image);//计算Laplacian能量
	int search(int x, int y,int H);//寻找最优值
	void find(int x, int y,int H);//寻找方案
	void narrow(cv::Mat &image);//沿seam缩小
	void enlarge(cv::Mat &image,int K);//沿seam扩大
	bool still();//是否继续消除对象
	void changeRimage();
	void Enlargeimage(cv::Mat &image, int K);//放大图像K列像素
public:
	cv::Mat image;
	void Load(const char *filename);
	int Seamlinev(cv::Mat &image);//垂直
	int Seamlineh(cv::Mat &image);//水平
	void Resize(int H,int W);//改变图片尺寸
	void GetSize(int &H, int &W);//获得图片大小
	void Remove();//移除对象
};
#endif // !

