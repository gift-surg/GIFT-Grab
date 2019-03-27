#include <chrono>
#include <ctime>
#include <iostream>

#include "ffmpeg_utils.h"

std::string to_string(const AVFrame * frame)
{
    std::string _frame = "AVFrame {";

    _frame.append(" metadata {");
    AVDictionaryEntry *tag = NULL;
    while ((tag = av_dict_get(frame->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
    {
        _frame.append(" \"");
        _frame.append(tag->key);
        _frame.append("\": \"");
        _frame.append(tag->value);
        _frame.append("\", ");
    }
    _frame.append("} ");
    _frame.append(", opaque: \"");
    if (frame->opaque)
    {
        char *opaque = NULL;
        opaque = static_cast<char*>(frame->opaque);
        if (strlen(opaque) > 0)
            _frame.append(opaque);
    }
    _frame.append("\" ");

    _frame.append("}");

    return _frame;
}

void set_metadata(AVFrame * frame)
{
    int ret;
    auto ts = std::chrono::system_clock::now();
    std::time_t tm = std::chrono::system_clock::to_time_t(ts);
    std::cout << "before timestamping: " << to_string(frame) << std::endl;
    char *_tm = new char[1024];
    strcpy(_tm, std::ctime(&tm));
    ret = av_dict_set(&frame->metadata,
                      "human-time", _tm,
                      0);
    if (not frame->opaque)
    {
        std::cout << "alloc" << std::endl;
        frame->opaque = malloc(strlen(_tm));
    }
    char *opaque = static_cast<char *>(frame->opaque);
    strcpy(opaque, _tm);
    delete []_tm;
    std::cout << "after timestamping: " << to_string(frame) << std::endl;
    if (ret < 0)
        std::cerr << "Could not add metadata due to FFmpeg error code: "
                  << ret << std::endl;
}

void get_metadata(const AVFrame * frame)
{
    std::cout << "frame: " << to_string(frame) << std::endl;
}
