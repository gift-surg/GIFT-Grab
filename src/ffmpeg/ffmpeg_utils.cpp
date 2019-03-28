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

void set_metadata(AVFrame * frame, std::string header)
{
    std::cout << header << std::endl;

    // metadata: human-readable timestamp
    int ret;
    auto ts = std::chrono::system_clock::now();
    std::time_t tm = std::chrono::system_clock::to_time_t(ts);
    char *_tm = new char[1024];
    strcpy(_tm, std::ctime(&tm));

    // metadata in metadata field
    ret = av_dict_set(&frame->metadata,
                      "human-time", _tm,
                      0);
    if (ret < 0)
        std::cerr << "Could not add metadata due to FFmpeg error code: "
                  << ret << std::endl;

    // metadata in opaque field
    if (not frame->opaque)
    {
        std::cout << "alloc" << std::endl;
        frame->opaque = malloc(strlen(_tm));
    }
    char *opaque = static_cast<char *>(frame->opaque);
    strcpy(opaque, _tm);

    // cleanup
    delete []_tm;
    std::cout << "timestamped: " << to_string(frame) << std::endl;
}

void get_metadata(const AVFrame * frame, std::string header)
{
    std::cout << header << std::endl;
    std::cout << "frame: " << to_string(frame) << std::endl;
}
