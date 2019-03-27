#include "ffmpeg_utils.h"

std::string to_string(const AVFrame * frame)
{
    std::string _frame = "AVFrame {";

    _frame.append(" metadata {");
    AVDictionaryEntry *tag = NULL;
    while ((tag = av_dict_get(frame->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
    {
        _frame.append(tag->key);
        _frame.append(":");
        _frame.append(tag->value);
        _frame.append(", ");
    }
    _frame.append("}");

    _frame.append("}");

    return _frame;
}
