//
// Created by Rémi MEVAERE on 19/08/2023.
// Copyright (c) 2023.
//
// Github : mevaere
// Twitter : MelectronVolt
//

#pragma once

double getFileSize(const std::wstring& filename);

bool fileExistsAndReadable(const std::wstring& filename);

bool isBinary(const std::wstring& filename);

bool isFileHumanReadable(const std::wstring& filename);
