#include <cstdlib>
#include <chrono>
#include <iostream>
#include <cstring>
#ifdef USE_FFMPEG
extern "C" {
#include <libswscale/swscale.h>
}
#endif
#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#endif

using namespace std;
using namespace std::chrono;

bool rgb_same_as_bgra(const unsigned char *rgb,
                      const unsigned char *bgra,
                      size_t l)
{
    for (size_t i = 0, j = 0; i < l; i += 4, j += 3)
        if (rgb[j] != bgra[i+2] or rgb[j+1] != bgra[i+1] or
            rgb[j+2] != bgra[i])
            return false;
    return true;
}

int main(int argc, char *argv[])
{
    // RGB with random values
    size_t w = 1920, h = 1080;
    size_t l;
    unsigned char *rgb = nullptr;
    bool compose = true;
    if (argc == 2)
    {
#ifdef USE_OPENCV
        cv::Mat orig = cv::imread(argv[1]);
        cv::imwrite("bgr_orig.png", orig);
        w = orig.cols;
        h = orig.rows;
        compose = false;
        l = 3 * w * h;
        rgb = reinterpret_cast<unsigned char *>(malloc(l * sizeof(unsigned char)));
        for (size_t i = 0; i < l; i +=3)
        {
            rgb[i] = orig.data[i+2];
            rgb[i+1] = orig.data[i+1];
            rgb[i+2] = orig.data[i];
        }
#endif
    }
    else if (argc == 3)
    {
        w = atoi(argv[1]);
        h = atoi(argv[2]);
    }
    cout << "Profiling RGB => BGRA conversion for "
         << w << " x " << h << " image" << endl;
    if (compose)
    {
        l = 3 * w * h;
        rgb = reinterpret_cast<unsigned char *>(malloc(l * sizeof(unsigned char)));
        for (size_t i = 0; i < l; i++)
        {
            rgb[i] = i % 32;
            rgb[i+1] = rgb[i] * 4;
            rgb[i+2] = i % 256;
        }
    }

    // RGB => BGRA in a strided loop
    l = 4 * w * h;
    unsigned char *bgra_loop = nullptr;
    bgra_loop = reinterpret_cast<unsigned char *>(malloc(l * sizeof(unsigned char)));
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for (size_t i = 0, j = 0; i < l; i += 4, j += 3)
    {
        bgra_loop[i] = rgb[j+2];
        bgra_loop[i+1] = rgb[j+1];
        bgra_loop[i+2] = rgb[j];
        bgra_loop[i+3] = 255;
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>( t2 - t1 ).count();
    cout << "Loop (" << (rgb_same_as_bgra(rgb, bgra_loop, l) ? "success" : "failure")
         << ") took: " << duration << " usec" << endl;
#ifdef USE_OPENCV
	{
        cv::Mat _bgra(h, w, CV_8UC4, bgra_loop);
        cv::imwrite("bgra_loop.png", _bgra);
	}
#endif
    free(bgra_loop);

    // simple memcopy (no RGB => BGRA)
    l = 3 * w * h;
    unsigned char *rgb_memcpy = nullptr;
    rgb_memcpy = reinterpret_cast<unsigned char *>(malloc(l * sizeof(unsigned char)));
    t1 = high_resolution_clock::now();
    memcpy(rgb_memcpy, rgb, l * sizeof(unsigned char));
    t2 = high_resolution_clock::now();
    duration = duration_cast<microseconds>( t2 - t1 ).count();
    cout << "memcpy took: " << duration << " usec" << endl;
    free(rgb_memcpy);

#ifdef USE_FFMPEG
    l = 4 * w * h;
    // RGB => BGRA using FFmpeg
    unsigned char *bgra_ffmpeg = nullptr;
    bgra_ffmpeg = reinterpret_cast<unsigned char *>(malloc(l * sizeof(unsigned char)));
    unsigned char *srcSlice[] = {rgb};
    int srcStride[] = {static_cast<int>(3 * w)};
    int srcSliceY = 0;
    int srcSliceH = h;
    unsigned char *dst[] = {bgra_ffmpeg};
    int dstStride[] = {static_cast<int>(4 * w)};
    int srcW = w, srcH = h, dstW = w, dstH = h;
    AVPixelFormat srcFormat = AV_PIX_FMT_RGB24, dstFormat = AV_PIX_FMT_BGRA;
    SwsContext *c = sws_getCachedContext(nullptr, srcW, srcH, srcFormat, dstW, dstH, dstFormat, 0, nullptr, nullptr, nullptr);
    t1 = high_resolution_clock::now();
    sws_scale(c, srcSlice, srcStride, srcSliceY, srcSliceH, dst, dstStride);
    t2 = high_resolution_clock::now();
    duration = duration_cast<microseconds>( t2 - t1 ).count();
    cout << "FFmpeg (" << (rgb_same_as_bgra(rgb, bgra_ffmpeg, l) ? "success" : "failure")
         << ") took: " << duration << " usec" << endl;
#ifdef USE_OPENCV
    {
        cv::Mat _bgra(h, w, CV_8UC4, bgra_ffmpeg), bgr;
        cv::cvtColor(_bgra, bgr, cv::COLOR_BGRA2BGR);
        cv::imwrite("bgra_ffmpeg.png", bgr);
    }
#endif
    free(bgra_ffmpeg);
#endif

    // free all memory
    free(rgb);
    return EXIT_SUCCESS;
}
