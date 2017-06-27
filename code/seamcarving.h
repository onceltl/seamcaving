#ifndef SEAMCARVING_H
#define SEAMCARVING_H
#include <opencv2/opencv.hpp>
class Seamcarving
{
	cv::Mat image_gray,Timage,Rimage;//
	cv::Mat energyx, energyy,energy;//dx,dy,energy=abs(dx)+abs(dy)
	std::vector<int> rec;//��¼dp����
	std::vector<int> F;//DP
	std::vector<int> record;
	std::vector<int> seamtot;
	bool need;//�Ƿ���������
	void GetSobelEnergy(cv::Mat &image);//����Sobel����
	void GetLaplacianEnergy(cv::Mat &image);//����Laplacian����
	int search(int x, int y,int H);//Ѱ������ֵ
	void find(int x, int y,int H);//Ѱ�ҷ���
	void narrow(cv::Mat &image);//��seam��С
	void enlarge(cv::Mat &image,int K);//��seam����
	bool still();//�Ƿ������������
	void changeRimage();
	void Enlargeimage(cv::Mat &image, int K);//�Ŵ�ͼ��K������
public:
	cv::Mat image;
	void Load(const char *filename);
	int Seamlinev(cv::Mat &image);//��ֱ
	int Seamlineh(cv::Mat &image);//ˮƽ
	void Resize(int H,int W);//�ı�ͼƬ�ߴ�
	void GetSize(int &H, int &W);//���ͼƬ��С
	void Remove();//�Ƴ�����
};
#endif // !

