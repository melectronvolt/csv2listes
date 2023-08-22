//
// Created by Rémi MEVAERE on 19/08/2023.
// Copyright (c) 2023.
//
// Github : mevaere
// Twitter : MelectronVolt
//

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"


std::wstring string_to_wstring(const std::string& str);
std::string wstring_to_string(const std::wstring& wstr);
void ConvertUTF8StreamToWideStream(const std::stringstream& sortie, std::wstringstream& wsortie);
std::string replaceAllChars(const std::string& input, char oldChar, char newChar);
std::string trim(const std::string &s);
