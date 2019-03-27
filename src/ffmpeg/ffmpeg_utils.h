#pragma once

#include <string>

extern "C" {
#include "libavformat/avformat.h"
}

std::string to_string(const AVFrame * frame);

void set_metadata(AVFrame * frame);
