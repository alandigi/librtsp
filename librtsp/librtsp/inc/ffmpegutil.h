#pragma once
#include <string>

#define MAX_EXTRADATA_SIZE ((INT_MAX - 10) / 2)
#define MAX_PSET_SIZE 1024


std::string GetConfigByCodecCtx(AVCodecContext *codecCtx);
