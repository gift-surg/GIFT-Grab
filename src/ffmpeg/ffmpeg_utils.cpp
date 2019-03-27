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

    _frame.append("}");

    return _frame;
}

void set_metadata(AVFrame * frame)
{
    int ret;
    auto ts = std::chrono::system_clock::now();
    std::time_t tm = std::chrono::system_clock::to_time_t(ts);
    std::cout << "before timestamping: " << to_string(frame) << std::endl;
    ret = av_dict_set(&frame->metadata,
                      "human-time", std::ctime(&tm),
                      0);
    std::cout << "after timestamping: " << to_string(frame) << std::endl;
    if (ret < 0)
        std::cerr << "Could not add metadata due to FFmpeg error code: "
                  << ret << std::endl;
}

void get_metadata(const AVFrame * frame)
{
    AVDictionaryEntry *dict_entry = NULL;
    dict_entry = av_dict_get(frame->metadata,
                             "human-time", NULL,
                             0);
    std::cout << "frame: " << to_string(frame) << std::endl;
}
