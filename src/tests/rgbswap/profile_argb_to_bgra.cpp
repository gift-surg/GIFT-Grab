#include <cstdlib>
#include <chrono>
#include <iostream>
#include <cstring>
#include <algorithm>
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

template<typename T>
void copy_strided(T begin , T end, T dest, int length)
{
    int num_copied = 0;
    copy_if(begin, end, dest, [&num_copied,length]( double x )
    {
        return (num_copied++ % length == 0);
    }
    );
}

bool argb_same_as_bgra(const unsigned char *argb,
                       const unsigned char *bgra,
                       size_t l)
{
    for (size_t i = 0; i < l; i += 4)
        if (argb[i] != bgra[i+3] or argb[i+1] != bgra[i+2] or
            argb[i+2] != bgra[i+1] or argb[i+3] != bgra[i])
            return false;
    return true;
}

int main(int argc, char *argv[])
{
    // ARGB with random values
    size_t w = 1920, h = 1080;
    size_t l;
    unsigned char *argb = nullptr;
    bool compose = true;
    if (argc == 2)
    {
#ifdef USE_OPENCV
        cv::Mat orig = cv::imread(argv[1]);
        cv::imwrite("bgr_orig.png", orig);
        w = orig.cols;
        h = orig.rows;
        compose = false;
        l = 4 * w * h;
        argb = reinterpret_cast<unsigned char *>(malloc(l * sizeof(unsigned char)));
        for (size_t i = 0, j = 0; i < l; i +=4, j += 3)
        {
            argb[i] = 255;
            argb[i+1] = orig.data[j+2];
            argb[i+2] = orig.data[j+1];
            argb[i+3] = orig.data[j];
        }
#endif
    }
    else if (argc == 3)
    {
        w = atoi(argv[1]);
        h = atoi(argv[2]);
    }
    cout << "Profiling ARGB => BGRA conversion for "
         << w << " x " << h << " image" << endl;
    if (compose)
    {
        l = 4 * w * h;
        argb = reinterpret_cast<unsigned char *>(malloc(l * sizeof(unsigned char)));
        for (size_t i = 0; i < l; i++)
        {
            argb[i] = i % 32;
            argb[i+1] = argb[i] * 4;
            argb[i+2] = i % 256;
            argb[i+3] = 255;
        }
    }

    // ARGB => BGRA in a strided loop
    unsigned char *bgra_loop = nullptr;
    bgra_loop = reinterpret_cast<unsigned char *>(malloc(l * sizeof(unsigned char)));
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for (size_t i = 0; i < l; i += 4)
    {
        bgra_loop[i] = argb[i+3];
        bgra_loop[i+1] = argb[i+2];
        bgra_loop[i+2] = argb[i+1];
        bgra_loop[i+3] = argb[i];
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>( t2 - t1 ).count();
    cout << "Loop (" << (argb_same_as_bgra(argb, bgra_loop, l) ? "success" : "failure")
         << ") took: " << duration << " usec" << endl;
    free(bgra_loop);
#ifdef USE_OPENCV
	{
        cv::Mat _bgra(h, w, CV_8UC4, bgra_loop), bgr;
        cv::cvtColor(_bgra, bgr, cv::COLOR_BGRA2BGR);
        cv::imwrite("bgra_loop.png", bgr);
	}
#endif

    // simple memcopy (no ARGB => BGRA)
    unsigned char *argb_memcpy = nullptr;
    argb_memcpy = reinterpret_cast<unsigned char *>(malloc(l * sizeof(unsigned char)));
    t1 = high_resolution_clock::now();
    memcpy(argb_memcpy, argb, l * sizeof(unsigned char));
    t2 = high_resolution_clock::now();
    duration = duration_cast<microseconds>( t2 - t1 ).count();
    cout << "memcpy took: " << duration << " usec" << endl;
    free(argb_memcpy);

    // ARGB => BGRA with a function
    unsigned char *bgra_function = nullptr;
    bgra_function = reinterpret_cast<unsigned char *>(malloc(l * sizeof(unsigned char)));
    t1 = high_resolution_clock::now();
    copy_strided(argb + 3, argb + l, bgra_function, 4);
    copy_strided(argb + 2, argb + l, bgra_function + 1, 4);
    copy_strided(argb + 1, argb + l, bgra_function + 2, 4);
    copy_strided(argb, argb + l, bgra_function + 3, 4);
    t2 = high_resolution_clock::now();
    duration = duration_cast<microseconds>( t2 - t1 ).count();
    cout << "Function (" << (argb_same_as_bgra(argb, bgra_function, l) ? "success" : "failure")
         << ") took: " << duration << " usec" << endl;
    free(bgra_function);

#ifdef USE_FFMPEG
    // ARGB => BGRA using FFmpeg
    unsigned char *bgra_ffmpeg = nullptr;
    bgra_ffmpeg = reinterpret_cast<unsigned char *>(malloc(l * sizeof(unsigned char)));
    unsigned char *srcSlice[] = {argb};
    int srcStride[] = {4 * w};
    int srcSliceY = 0;
    int srcSliceH = h;
    unsigned char *dst[] = {bgra_ffmpeg};
    int dstStride[] = {4 * w};
    int srcW = w, srcH = h, dstW = w, dstH = h;
    AVPixelFormat srcFormat = AV_PIX_FMT_ARGB, dstFormat = AV_PIX_FMT_BGRA;
    SwsContext *c = sws_getCachedContext(nullptr, srcW, srcH, srcFormat, dstW, dstH, dstFormat, 0, nullptr, nullptr, nullptr);
    t1 = high_resolution_clock::now();
    sws_scale(c, srcSlice, srcStride, srcSliceY, srcSliceH, dst, dstStride);
    t2 = high_resolution_clock::now();
    duration = duration_cast<microseconds>( t2 - t1 ).count();
    cout << "FFmpeg (" << (argb_same_as_bgra(argb, bgra_ffmpeg, l) ? "success" : "failure")
         << ") took: " << duration << " usec" << endl;
    free(bgra_ffmpeg);
#endif
#ifdef USE_OPENCV
    {
        cv::Mat _bgra(h, w, CV_8UC4, bgra_ffmpeg), bgr;
        cv::cvtColor(_bgra, bgr, cv::COLOR_BGRA2BGR);
        cv::imwrite("bgra_ffmpeg.png", bgr);
    }
#endif

    // free all memory
    free(argb);
    return EXIT_SUCCESS;
}
