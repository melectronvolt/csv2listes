//
// Created by Rémi MEVAERE on 19/08/2023.
// Copyright (c) 2023.
//
// Github : mevaere
// Twitter : MelectronVolt
//
#pragma once

bool CopyClipboardToStreamUnicode(std::wstringstream& wss);

bool PasteClipboardFromStreamUnicode(const std::wstringstream& wss);
