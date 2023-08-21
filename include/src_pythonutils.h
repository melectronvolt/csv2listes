//
// Created by Rémi MEVAERE on 19/08/2023.
// Copyright (c) 2023.
//
// Github : mevaere
// Twitter : MelectronVolt
//

#pragma once

void transformToPythonVar(std::vector<std::string>& vec);

void transformThePythonList(std::vector<std::string>& vec);

std::string transformToPEP8(const std::string& input);

bool isValidPythonNumber(const std::string& s);

