#include"seamcarving.h"  
using namespace cv;
void Seamcarving::Load(const char *filename)
{
	need = 0;
	image = imread(filename, CV_LOAD_IMAGE_COLOR);
	Rimage.create(image.rows, image.cols, CV_8UC1);
	for (int i = 0;i < image.rows;i++)
		for (int j = 0;j < image.cols;j++)
			Rimage.at<uchar>(i, j) = saturate_cast<uchar>(255);

}
void Seamcarving::GetSize(int &H, int &W)
{
	H = image.rows;
	W = image.cols;
}
void Seamcarving::GetSobelEnergy(Mat &image)
{
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	cvtColor(image, image_gray, CV_RGB2GRAY);//转为灰度图
	Sobel(image_gray, energyx, ddepth, 1, 0, 3);//卷积核3X3 
	Sobel(image_gray, energyy, ddepth, 0, 1, 3);
	convertScaleAbs(energyx, energyx);//取绝对值
	convertScaleAbs(energyy, energyy);
	addWeighted(energyx, 0.5, energyy, 0.5, 0, energy);//相加
	if (need)addWeighted(energy, 0.1, Rimage, 0.9, 0, energy);//如果有对象移除，则加上需要移除的偏移量
}
void Seamcarving::GetLaplacianEnergy(Mat &image)
{
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;//避免溢出
	cvtColor(image, image_gray, CV_RGB2GRAY);
	Laplacian(image_gray, energy, ddepth, 3);
	convertScaleAbs(energy, energy);//取绝对值
	if (need)addWeighted(energy, 0.1, Rimage, 0.9, 0, energy);//如果有对象移除，则加上需要移除的偏移量
}
int Seamcarving::search(int x, int y, int maxsize)
{
	if (x == -1) return 0;
	int w = x*maxsize + y;
	if (rec[w] != -1) return F[w];//已经记录过方案
	rec[w] = y;
	F[w] = search(x - 1, y, maxsize) + energy.at<uchar>(x, y);//直接向上
	int ans = 0x7FFFFFFF;
	if (y > 0)//向左未超出范围
	{
		ans = search(x - 1, y - 1, maxsize) + energy.at<uchar>(x, y);
		if (ans < F[w])
		{
			rec[w] = y - 1;
			F[w] = ans;
		}
	}
	if (y + 1 <maxsize)//向右
	{
		ans = search(x - 1, y + 1, maxsize) + energy.at<uchar>(x, y);
		if (ans < F[w])
		{
			rec[w] = y + 1;
			F[w] = ans;
		}
	}
	return F[w];
}
void Seamcarving::find(int x, int y, int maxsize)
{
	if (x == -1) return;
	int w = x*maxsize + y;
	find(x - 1, rec[w], maxsize);
	record.push_back(y);
}
int Seamcarving::Seamlinev(Mat &image)
{
	GetSobelEnergy(image);
	//GetLaplacianEnergy();
	rec.resize(energy.rows*energy.cols);
	F.resize(energy.rows*energy.cols);
	for (int i = 0;i < rec.size();i++)
	{
		rec[i] = -1;
		F[i] = 0x7FFFFFFF;
	}
	int Min = 0x7FFFFFFF, W = -1;
	for (int j = 0;j < energy.cols;j++)
	{
		if (search(energy.rows - 1, j, energy.cols) < Min)
		{
			Min = search(energy.rows - 1, j, energy.cols);
			W = j;
		}
	}
	record.clear();
	find(energy.rows - 1, W, energy.cols);
	/*for (int i = 0;i < energy.rows;i++)
	{
	image.at<Vec3b>(i, record[i])[0] = saturate_cast<uchar>(0);
	image.at<Vec3b>(i, record[i])[1] = saturate_cast<uchar>(0);
	image.at<Vec3b>(i, record[i])[2] = saturate_cast<uchar>(255);
	}
	imwrite("S10.jpg",image);*/
	return Min;
}
int Seamcarving::Seamlineh(Mat &image)
{
	transpose(image, image);
	int Min = Seamlinev(image);
	transpose(image, image);
	//	imwrite("test.jpg", image);
	return Min;
}
void Seamcarving::narrow(Mat &image)
{
	Timage.create(image.rows, image.cols - 1, CV_8UC3);
	for (int i = 0;i < image.rows;i++)
	{
		int t = 0;
		for (int j = 0;j < image.cols;j++)
			if (j != record[i])
			{
				Timage.at<Vec3b>(i, t) = image.at<Vec3b>(i, j);
				t++;
			}

	}
	Timage.copyTo(image);
}
void Seamcarving::changeRimage()
{
	Timage.create(Rimage.rows, Rimage.cols - 1, CV_8UC1);
	for (int i = 0;i < Rimage.rows;i++)
	{
		int t = 0;
		for (int j = 0;j < Rimage.cols;j++)
			if (j != record[i])
			{
				Timage.at<uchar>(i, t) = Rimage.at<uchar>(i, j);
				t++;
			}

	}
	Timage.copyTo(Rimage);
}
void Seamcarving::enlarge(Mat &image, int K)
{
	Timage.create(image.rows, image.cols + K, CV_8UC3);
	for (int i = 0;i < image.rows;i++)
	{
		int t = 0, j = 0;
		while (j < image.cols)
		{
			if (seamtot[i*image.cols + j] == 0)
			{
				Timage.at<Vec3b>(i, t) = image.at<Vec3b>(i, j);
				t++;j++;
			}
			else
			{
				Vec3b int1 = image.at<Vec3b>(i, j);
				Vec3b int2;
				if (j != image.cols - 1)int2 = image.at<Vec3b>(i, j + 1);
				else int2 = image.at<Vec3b>(i, j - 1);
				Vec3b int3;
				int3.val[0] = (int1.val[0] + int2.val[0]) / 2;
				int3.val[1] = (int1.val[1] + int2.val[1]) / 2;
				int3.val[2] = (int1.val[2] + int2.val[2]) / 2;
				Timage.at<Vec3b>(i, t) = int3;
				t++;
				seamtot[i*image.cols + j]--;
				
			}
		}
		
	}
	Timage.copyTo(image);
}
void Seamcarving::Enlargeimage(Mat &image, int K)
{
	seamtot.resize(image.rows*image.cols);
	for (int i = 0;i < seamtot.size();i++)
		seamtot[i] = 0;
	Mat Timage;
	image.copyTo(Timage);
	for (int k = 0;k < K;k++)
	{
		Seamlinev(Timage);
		for (int i = 0;i < Timage.rows;i++)
		{
			Timage.at<Vec3b>(i, record[i])[0] = saturate_cast<uchar>(0);
			Timage.at<Vec3b>(i, record[i])[1] = saturate_cast<uchar>(0);
			Timage.at<Vec3b>(i, record[i])[2] = saturate_cast<uchar>(255);
			seamtot[i*image.cols + record[i]] ++;
		}
	}
	//imwrite("debug.jpg",Timage);
	enlarge(image, K);

}
void Seamcarving::Resize(int H, int W)
{

	if (W > image.cols) Enlargeimage(image, W - image.cols);
	if (H > image.rows)
	{
		transpose(image, image);
		Enlargeimage(image, H - image.cols);
		transpose(image, image);
	}	
	while (H != image.rows || W != image.cols)
	{
		int ansv = Seamlinev(image), ansh = Seamlineh(image);
		if (H == image.rows) ansh = 0x7fffffff;
		if (W == image.cols) ansv = 0x7fffffff;
		if (ansv < ansh)
		{
			Seamlinev(image);
			narrow(image);//缩小
		}
		else
		{
			int Rows = image.rows;
			transpose(image, image);
			narrow(image);//缩小
			transpose(image, image);
		}
	}
	imwrite("R1.jpg", image);
}
bool Seamcarving::still()
{
	for (int i = 0;i<Rimage.rows;i++)
		for (int j = 0;j < Rimage.cols;j++)
		{
			int x = Rimage.at<uchar>(i, j);
			if (x == 0) return 1;
		}
	return 0;
}
void Seamcarving::Remove()
{
	need = 1;
	Rimage = imread("remove.jpg", CV_LOAD_IMAGE_COLOR);//读入移除对象的圈
	cvtColor(Rimage, Rimage, CV_RGB2GRAY);
	int H = image.rows, W = image.cols;
	while (still())//移除对象任然有像素未移除
	{
		Seamlinev(image);//寻找seam线
		narrow(image);//缩小原图
		changeRimage();//改变移除对象的圈
	}
	need = 0;
	//Resize(H,W);
	imwrite("test.jpg", image);
}