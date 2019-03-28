#include <chrono>
#include <ctime>
#include <iostream>

#include "ffmpeg_utils.h"

std::string to_string(const AVFrame * frame)
{
    std::string _frame = "AVFrame {";

    // metadata field
    _frame.append(" metadata: {");
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

    // opaque field
    _frame.append(", opaque: \"");
    if (frame->opaque)
    {
        char *opaque = NULL;
        opaque = static_cast<char*>(frame->opaque);
        if (strlen(opaque) > 0)
            _frame.append(opaque);
    }
    _frame.append("\" ");

    // opaque_ref field
    _frame.append(", opaque_ref: {");
    if (frame->opaque_ref)
    {
        _frame.append("data: \"");
	// TODO: improve this cast
        _frame.append(reinterpret_cast<char*>(frame->opaque_ref->data));
	_frame.append("\", ");
	_frame.append("size: ");
	_frame.append(std::to_string(frame->opaque_ref->size));
	_frame.append(", ");
    }
    _frame.append("} ");

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

    // remove trailing newline
    int size = strlen(_tm);
    _tm[size - 2] = '\0';
    size--;

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
        frame->opaque = malloc(size);
    }
    char *opaque = static_cast<char *>(frame->opaque);
    strcpy(opaque, _tm);

    // metadata in opaque_ref field
    if (not frame->opaque_ref)
    {
        int size = strlen(_tm);
        uint8_t *data = reinterpret_cast<uint8_t*>(_tm);
        frame->opaque_ref = av_buffer_create(data, size, av_buffer_default_free, NULL, 0);
        // _tm will be freed by FFmpeg as part of AVBuffer's lifecycle management
    }

    // print timestamped frame
    std::cout << "timestamped: " << to_string(frame) << std::endl;
}

void get_metadata(const AVFrame * frame, std::string header)
{
    std::cout << header << std::endl;
    std::cout << "frame: " << to_string(frame) << std::endl;
}
