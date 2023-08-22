#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#define MAX_SIZE_LINE 1024
#define MIN_LINE_COUNT 2
#define MAX_LINE_COUNT 67000
#define MAX_SIZE_FILE 1000000

extern std::stringstream memStreamFile;
extern std::string separateur;
extern unsigned int nbLigneEntete;
extern unsigned int nbLigneDonnees;
extern std::wstring filenameCSV;
extern std::shared_ptr<spdlog::logger> logger;
extern unsigned int nbColonnes;
extern bool hasHeader;
extern std::vector<std::string> nomsColonnes;
extern std::vector<std::vector<std::string>> donnees;
extern std::stringstream sortie;
extern std::wstringstream wsortie;
extern unsigned int memStreamLine;

bool checkMaxSizeLine();

bool isNotTooBig(const std::wstring& filename);

bool isMemStreamEmpty();

bool notTooLessLines(const std::wstring& filename);

bool isNotTooManyLines(const std::wstring& filename);

bool loadIntoMemStream(const std::wstring& filename);

std::vector<std::string> split(const std::string& s, const std::string& delimiter);

bool readNextLineMemStream(std::string& line);

double getLineCount();

std::string getOneLine(int LineNumber);

bool isFileText(const std::wstring& filename);


