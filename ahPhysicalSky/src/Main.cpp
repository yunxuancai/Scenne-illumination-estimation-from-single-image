#include "Main.h"
//#include "engine.h"
//#pragma comment(lib, "libeng.lib")
//#pragma comment(lib, "libmx.lib")
//#pragma comment(lib, "libmat.lib")
#define EXPORTBUILD
using namespace cv;
//Vector3f * generateSky(float phi, float theta, float turbidity, float exposure = 1.0f)
//{
//	Renderer render;
//
//	// Options
//	render.Width = 3328;
//	render.Height = 1664;
//	render.Colorspace = sRGB;
//	render.SunSize = 0.02f;
//	render.Turbidity = turbidity;
//	render.SunTint = Vector3f(1);
//	render.SkyTint = Vector3f(1);
//	render.GroundAlbedo = Vector3f(0.3);
//	render.SunColor = Vector3f(20000);
//	render.bEnableSun = 1;
//	render.Samples = 1;
//	Vector3f dir(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
//
//	std::cout << ">>> Rendering Colorspaces, 0 - XYZ, 1 - sRGB, 2 - ACEScg, 3 = ACES 2065-1 " << std::endl;
//	std::cout << ">>> Rendering with colorspace : " << render.Colorspace << std::endl;
//	unsigned i = 0;
//
//	char filename[1080 * 3];
//	sprintf(filename, "./skydome.1%03d.exr", i);
//	float angle = i / float(render.Samples - 1.0f) * M_PI * -0.6f;
//	fprintf(stderr, ">>> Rendering Hosek-Wilkie Sky Model image %d. Angle = %0.2f\n", i, angle * 180.0f / M_PI);
//
//	SkyModel sky;
//	sky.SetupSky(dir,
//		render.SunSize,
//		render.SunColor,
//		render.GroundAlbedo,
//		render.Turbidity,
//		render.Colorspace);
//	Vector3f *skyImage = new Vector3f[render.Width * render.Height], *p = skyImage;
//	memset(skyImage, 0x0, sizeof(Vector3f) * render.Width * render.Height);
//	for (unsigned j = 0; j < render.Height; ++j) {
//		float y = 2.0f * (j + 0.5f) / float(render.Height - 1) - 1.0f;
//		for (unsigned i = 0; i < render.Width; ++i, ++p) {
//			float x = 2.0f * (i + 0.5f) / float(render.Width - 1) - 1.0f;
//			float z2 = x * x + y * y;
//			if (j <= render.Height / 2) {
//				float phi = i * 2 * M_PI / render.Width;//std::atan2(y, x);//0-2pi
//				float theta = j * M_PI / render.Height;//std::acos(1 - z2);
//				Vector3f dir(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
//				// 1 meter above sea level
//				*p = exposure * sky.Sample(dir, render.bEnableSun, render.SkyTint, render.SunTint);
//			}
//		}
//	}
//
//	
//
//	return skyImage;
//
//
//}
//
//inline unsigned char ftouc(float f, float gamma)
//{
//	int i = static_cast<int>(255.0f * powf(f, 1.0f / gamma));
//	if (i > 255) i = 255;
//	if (i < 0) i = 0;
//
//	return static_cast<unsigned char>(i);
//}
//
//
//template<typename _Tp>
//vector<_Tp> convertMat2Vector(const Mat &mat)
//{
//	return (vector<_Tp>)(mat.reshape(1, 1));//通道数不变，按行转为一行
//}
//
//
//double compute_SSE(cv::Mat Mat1, cv::Mat Mat2)
//{
//
//	cv::Mat M1 = Mat1.clone();
//	cv::Mat M2 = Mat2.clone();
//
//	int rows = M2.rows;
//	int cols = M2.cols;
//	// 确保它们的大小是一致的
//	cv::resize(M1, M1, cv::Size(cols, rows));
//
//	M1.convertTo(M1, CV_32F);
//	M2.convertTo(M2, CV_32F);
//	// compute PSNR
//	Mat Diff;
//		// Diff一定要提前转换为32F，因为uint8格式的无法计算成平方
//	Diff.convertTo(Diff, CV_32F);
//	cv::absdiff(M1, M2, Diff); //  Diff = | M1 - M2 |
//
//	Diff = Diff.mul(Diff);            //  　　 | M1 - M2 |.^2
//	Scalar S = cv::sum(Diff);  // 分别计算每个通道的元素之和
//
//	double sse;   // square error
//	sse = S.val[0] + S.val[1] + S.val[2];  // sum of all channels
//	M1.release();
//	M2.release();
//	Diff.release();
//	return sse;
//}
//
//double compute_exposure(cv::Mat pic, cv::Mat sky)
//{
//	cv::Mat M1 = pic.clone();
//	cv::Mat M2 = sky.clone();
//
//	int rows = M2.rows;
//	int cols = M2.cols;
//	// 确保它们的大小是一致的
//	cv::resize(M1, M1, cv::Size(cols, rows));
//
//	M1.convertTo(M1, CV_32F);
//	M2.convertTo(M2, CV_32F);
//	Scalar S1 = cv::sum(pic.mul(sky));
//	Scalar S2 = cv::sum(sky.mul(sky));
//	double exp;   // square error
//	exp = (S1.val[0] + S1.val[1] + S1.val[2])/ (S2.val[0] + S2.val[1] + S2.val[2]);  // sum of all channels
//	M1.release();
//	M2.release();
//	return exp;
//}
//
//double calSky(double turbidity)
//{
//	float phi = 44.3496529;
//	float theta = -143.0590763;
//	auto h = generateSky(phi, theta, turbidity);
//	Mat sky;
//	//加载图片
//	sky = imread("a.png", IMREAD_COLOR);
//	Mat mask = imread("a.png", IMREAD_COLOR);
//	Mat land = imread("tt.png", IMREAD_COLOR);
//	//检测是否加载成功
//	if (!sky.data)  //or == if(src.empty())
//	{
//		std::cout << "Could not open or find the image" << std::endl;
//	}
//	int nl = sky.rows; //行数  
//	int nc = sky.cols * sky.channels();
//	for (int row = 0, i = 0; row < sky.rows; row++)
//	{
//		// data 是 uchar* 类型的, m.ptr(row) 返回第 row 行数据的首地址
//		// 需要注意的是该行数据是按顺序存放的,也就是对于一个 3 通道的 Mat, 一个像素3个通道值, [B,G,R][B,G,R][B,G,R]... 
//		// 所以一行长度为:sizeof(uchar) * m.cols * m.channels() 个字节 
//		uchar* data = sky.ptr(row);
//		uchar* maskData = mask.ptr(row);
//		uchar* landData = land.ptr(row);
//		for (int col = 0; col < sky.cols; col++, i++)
//		{
//			if (maskData[col * 3] != 0)
//			{
//				data[col * 3] = ftouc(h[i].z, 2.2);     //第row行的第col个像素点的第一个通道值 Blue
//
//				data[col * 3 + 1] = ftouc(h[i].y, 2.2); // Green
//
//				data[col * 3 + 2] = ftouc(h[i].x, 2.2); // Red
//			}
//			else
//			{
//				data[col * 3] = landData[col * 3];     //第row行的第col个像素点的第一个通道值 Blue
//
//				data[col * 3 + 1] = landData[col * 3 + 1]; // Green
//
//				data[col * 3 + 2] = landData[col * 3 + 2]; // Red
//			}
//		}
//	}
//	double sse = compute_SSE(sky, land);
//	sky.release();
//	land.release();
//	mask.release();
//	return sse;
//}

//int main()
//{
//	generateSky(47.69776925, 3.452260585, 3.5, 0.042352);
//}

//int main()
//{
//	float phi = 44.3496529;
//	float theta = -143.0590763;
//	float turbidity = 10.0f;
//	auto h = generateSky(phi, theta, turbidity);
//	Mat sky;
//	//加载图片
//	sky = imread("a.png", IMREAD_COLOR);
//	Mat mask = imread("a.png", IMREAD_COLOR);
//	Mat land = imread("tt.png", IMREAD_COLOR);
//	//检测是否加载成功
//	if (!sky.data)  //or == if(src.empty())
//	{
//		std::cout << "Could not open or find the image" << std::endl;
//		return -1;
//	}
//	int nl = sky.rows; //行数  
//	int nc = sky.cols * sky.channels();
//	for (int row = 0, i = 0; row < sky.rows; row++)
//	{
//		// data 是 uchar* 类型的, m.ptr(row) 返回第 row 行数据的首地址
//		// 需要注意的是该行数据是按顺序存放的,也就是对于一个 3 通道的 Mat, 一个像素3个通道值, [B,G,R][B,G,R][B,G,R]... 
//		// 所以一行长度为:sizeof(uchar) * m.cols * m.channels() 个字节 
//		uchar* data = sky.ptr(row);
//		uchar* maskData = mask.ptr(row);
//		uchar* landData = land.ptr(row);
//		for (int col = 0; col < sky.cols; col++,i++)
//		{
//			if (maskData[col * 3] != 0)
//			{
//				data[col * 3] = ftouc(h[i].z, 2.2);     //第row行的第col个像素点的第一个通道值 Blue
//
//				data[col * 3 + 1] = ftouc(h[i].y, 2.2); // Green
//
//				data[col * 3 + 2] = ftouc(h[i].x, 2.2); // Red
//			}
//			else
//			{
//				data[col * 3] = landData[col * 3];     //第row行的第col个像素点的第一个通道值 Blue
//
//				data[col * 3 + 1] = landData[col * 3 + 1]; // Green
//
//				data[col * 3 + 2] = landData[col * 3 + 2]; // Red
//			}
//		}
//	}
//	std::cout.setf(std::ios::fixed, std::ios::floatfield);
//	std::cout << compute_SSE(sky,land) << std::endl;
//	// 创建窗口
//	namedWindow("Display");
//	//显示图像
//	imshow("Display", sky);
//
//	//暂停，等待按键结束
//	waitKey(0);
//
//	return 0;
//}

#define DLLEXPORT extern "C" __declspec(dllexport)

DLLEXPORT int createSky(float theta0, float phi0, float turbidity, float exposure)
{
	std::cout << theta0 << " " << phi0 << " " << turbidity << " " << exposure << std::endl;
	Renderer render;

	// Options
	render.Width = 2000;
	render.Height = 1000;
	render.Colorspace = sRGB;
	render.SunSize = 2.0f;
	render.Turbidity = turbidity;
	render.SunTint = Vector3f(50);
	render.SkyTint = Vector3f(50);
	render.GroundAlbedo = Vector3f(0.3);
	render.SunColor = Vector3f(20000, 20000, 10000);
	render.bEnableSun = 1;
	render.Samples = 1;
	float theta = (90- theta0)*3.1415 / 180;
	float phi = (180+ phi0)*3.1415 / 180;
	Vector3f dir(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
	//Vector3f dir(theta,phi,0);
	std::cout<<">>> Rendering Colorspaces, 0 - XYZ, 1 - sRGB, 2 - ACEScg, 3 = ACES 2065-1 "<<std::endl;
	std::cout<<">>> Rendering with colorspace : "<<render.Colorspace<<std::endl;

	for (unsigned i = 0; i < render.Samples; ++i) {
		char filename[1080 * 3];
		sprintf(filename, "./environment.exr", i);
		float angle = i / float(render.Samples - 1.0f) * M_PI * -0.6f;
		fprintf(stderr, ">>> Rendering Hosek-Wilkie Sky Model image %d. Angle = %0.2f\n", i, angle * 180.0f / M_PI);

		render.RenderSkyDome(dir,//Vector3f(-M_PI*0.5, cos(angle), -sin(angle)),
			filename,
			render.Width,
			render.Height,
			render.SunSize,
			render.SunColor,
			render.GroundAlbedo,
			render.Turbidity,
			render.SkyTint,
			render.SunTint,
			render.bEnableSun,
			render.Colorspace, 
			exposure);

	}

	return EXIT_SUCCESS;
}