#include "Renderer.h"

#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

void Renderer::RenderSkyDome(Vector3f sunDir, const char *filename, unsigned width, unsigned height, float sunSize, Vector3f sunColor, Vector3f groundAlbedo, float turbidity, Vector3f skyTint, Vector3f sunTint, bool bEnableSun, ColorSpace colorspace, float exposure)
{
	SkyModel sky;
	sky.SetupSky(sunDir,
		sunSize,
		sunColor,
		groundAlbedo,
		turbidity,
		colorspace);

#if 1
	Vector3f *image = new Vector3f[width * height], *p = image;
	memset(image, 0x0, sizeof(Vector3f) * width * height);
	for (unsigned j = 0; j < height; ++j) {
		float y = 2.0f * (j + 0.5f) / float(height - 1) - 1.0f;
		for (unsigned i = 0; i < width; ++i, ++p) {
			float x = 2.0f * (i + 0.5f) / float(width - 1) - 1.0f;
			float z2 = x * x + y * y;
			if (j <= height/2) {
				float phi = i*2*M_PI/width;//std::atan2(y, x);//0-2pi
				float theta = j * M_PI/height;//std::acos(1 - z2);
				Vector3f dir(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
				// 1 meter above sea level
				*p = sky.Sample(dir, bEnableSun, skyTint, sunTint,exposure);
			}
		}
	}
#endif
	// EXR
	EXRHeader header;
	InitEXRHeader(&header);
	EXRImage img;
	InitEXRImage(&img);
	img.num_channels = 3;
	std::vector<float> images[3];
	images[0].resize(width * height);
	images[1].resize(width * height);
	images[2].resize(width * height);
	// EXR

	p = image;
	int a = 0;
	for (unsigned j = 0; j < height; ++j) {
		for (unsigned i = 0; i < width; ++i, ++p) {

			// EXR
			images[0][a] = (*p)[0];
			images[1][a] = (*p)[1];
			images[2][a] = (*p)[2];
			// EXR
			a += 1;

		}
	}

	// EXR
	float *image_ptr[3];
	image_ptr[0] = &(images[2].at(0));	// B
	image_ptr[1] = &(images[1].at(0));	// G
	image_ptr[2] = &(images[0].at(0));	// R

	img.images = (unsigned char**)image_ptr;
	img.width = width;
	img.height = height;

	header.num_channels = 3;
	header.channels = (EXRChannelInfo *)malloc(sizeof(EXRChannelInfo) * header.num_channels);
	// Must be (A)BGR order, since most of EXR viewers expect this channel order.
	strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
	strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
	strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';

	header.pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
	header.requested_pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
	for (int i = 0; i < header.num_channels; ++i) {
		header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;	// pixel type of the input
		header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF;	// Pixel type of output image to be stored in .exr
	}

	const char *err = nullptr;
	int ret = SaveEXRImageToFile(&img, &header, filename, &err);
	if (ret != TINYEXR_SUCCESS) {
		fprintf(stderr, "EXR SAVE ERRROR: %s\n", err);
		FreeEXRErrorMessage(err);	// Frees buffer for an error message
		std::cout << ret << std::endl;
	}

	free(header.channels);
	free(header.pixel_types);
	free(header.requested_pixel_types);
	// EXR
}