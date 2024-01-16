//
// Created by Rémi MEVAERE on 17/08/2023.
// Copyright (c) 2023.
//
// Github : mevaere
// Twitter : MelectronVolt
//

#include "src_functions.h"
#include "src_stringutils.h"
#include "src_clipboardutils.h"
#include "src_pythonutils.h"
#include "src_windowsutils.h"
#include "src_filesutils.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>

#include <fstream>
#include <cerrno>
#include <chrono>


const unsigned short arraySize = 5;
const std::wstring filename[arraySize]{L"fizziq_prob.csv", L"fizziq_dot.csv", L"fizziq_comma.csv", L"avimeca.txt",
                                       L"avimeca_pv.txt"};
const int lineStartHeader[arraySize]{0, 0, 0, 0, 0};
const int lineStartData[arraySize]{0, 0, 0, 0, 0};

#include "spdlog/spdlog.h"
#include "spdlog/sinks/null_sink.h"



TEST_CASE("Python") {

    SECTION("PEP8 short") {
        std::string input = "random_numbers = [647, 237, 93]";
        std::string output = transformToPEP8(input);
        REQUIRE(output == input);
    }SECTION("PEP8 long") {
        std::string input = "random_numbers = [647, 237, 930, 346, 744, 515, 923, 181, 814, 765, 755, 550, 695, 557, 990, 169, 997, 702, 128, 724, 689, 946, 276, 251, 188, 532, 433, 597, 493, 576, 530, 907, 944, 816, 482, 961, 136, 407, 765, 640, 413, 266, 448, 745, 634, 676, 489, 735, 635, 460, 286, 499, 732, 133, 407, 903, 870, 163, 185, 912, 847, 246, 905, 169, 981, 641, 349, 245, 868, 152, 215, 793, 108, 588, 440, 692, 329, 334, 508, 579, 782, 867, 134, 737, 936, 299, 763, 437, 554, 893, 250, 908, 628, 836, 917, 521, 854, 124, 899, 695, 997, 371, 930, 141, 291, 482, 539, 999, 369, 185, 271, 222, 835, 425, 891, 429, 986, 752, 674, 197, 435, 860, 248, 890, 562, 477, 604, 405, 997, 463, 491, 899, 546, 460, 619, 833, 742, 835, 661, 464, 731, 362, 271, 926, 695, 477, 683, 180, 258, 596, 136, 394, 745, 126, 709, 726, 460, 156, 850, 460, 393, 882, 852, 144, 444, 907, 775, 985, 517, 771, 418, 462, 536, 703, 931, 990, 622, 231, 357, 732, 757, 617, 193, 843, 869, 612, 589, 108, 507, 566, 337, 960, 330, 826, 603, 619, 982, 865, 755, 309, 529, 189, 907, 811, 420, 480, 492, 442, 678, 890, 784, 173, 226, 656, 985, 448, 207, 594, 857, 797, 711, 768, 135, 135, 558, 332, 443, 705, 900, 160, 249, 981, 708, 693, 505, 727, 429, 545, 679, 615, 241, 540, 776, 579, 970, 948, 601, 330, 804, 249, 222, 837, 683, 185, 564, 750, 155, 404, 128, 572, 137, 513, 710, 723, 961, 161, 149, 358, 890, 697, 216, 150, 469, 779, 767, 344, 126, 804, 576, 900, 517, 499, 507, 347, 187, 490, 204, 818, 166, 908, 899, 172, 488, 228, 712, 865, 597, 381, 118, 577, 774, 901, 758, 916, 607, 980, 258, 373, 675, 980, 336, 562, 213, 680, 700, 469, 210, 353, 876, 513, 185, 963, 948, 442, 752, 914, 936, 788, 249, 571, 864, 431, 306, 910, 358, 692, 852, 770, 175, 291, 657, 906, 606, 903, 944, 717, 880, 938, 786, 550, 165, 730, 998, 926, 180, 782, 993, 466, 956, 932, 699, 729, 570, 189, 162, 966, 207, 972, 121, 847, 305, 998, 248, 671, 236, 504, 805, 490, 573, 127, 457, 850, 243, 517, 634, 923, 829, 573, 831, 580, 589, 656, 789, 825, 567, 723, 915, 735, 218, 572, 532, 881, 767, 103, 936, 440, 437, 594, 841, 873, 570, 156, 701, 414, 848, 703, 170, 599, 948, 215, 955, 463, 144, 470, 497, 920, 116, 178, 681, 795, 951, 416, 657, 915, 447, 535, 168, 790, 529, 555, 737, 686, 428, 831, 283, 515, 360, 625, 493, 574, 980, 213, 983, 448, 918, 361, 271, 609, 455, 779, 204, 498, 756, 976, 769, 742, 847, 970, 256, 873, 328, 287, 269, 741, 903, 273, 927, 943, 620, 300, 443, 463, 661, 884, 833, 857, 332, 916, 156, 342, 968, 524, 420, 866, 891, 390, 690]";
        std::string output = transformToPEP8(input);
        REQUIRE_FALSE(output == input);
    }SECTION("To Python Var") {
        std::vector<std::string> words = {"bonjour", "0hello", "bonjour", "welc0me", "b@nzai", "twOtwo","hello(r)"};
        std::vector<std::string> variables = {"bonjour", "_0hello", "bonjour_1", "welc0me", "b_nzai", "twOtwo","hello_r_"};
        transformToPythonVar(words);
        REQUIRE(words == variables);
    }SECTION("Valid Python Number") {
        REQUIRE(isValidPythonNumber("1.23e5"));
        REQUIRE(isValidPythonNumber("123000.0"));
        REQUIRE(isValidPythonNumber("-10"));
        REQUIRE(isValidPythonNumber("2 + 3j"));
        REQUIRE(isValidPythonNumber("3.14"));
        REQUIRE_FALSE(isValidPythonNumber("1.23f5"));
        REQUIRE_FALSE(isValidPythonNumber("123000,0"));
        REQUIRE_FALSE(isValidPythonNumber("-+10"));
        REQUIRE_FALSE(isValidPythonNumber("2 + 3h"));
        REQUIRE_FALSE(isValidPythonNumber("3,14"));
    }SECTION("Transform the python liste") {
        std::vector<std::string> words = {"5", "0hello", "4e6", "welc0me", "b@nzai", "twOtwo"};
        std::vector<std::string> variables = {"5", "\"0hello\"", "4e6", "\"welc0me\"", "\"b@nzai\"", "\"twOtwo\""};
        transformThePythonList(words);
        REQUIRE(words == variables);
    }
}

TEST_CASE("Clipboard") {
    SECTION("Paste from Clipboard") {
        std::wstringstream wsortie;
        REQUIRE(CopyClipboardToStreamUnicode(wsortie));
    }SECTION("Copy to Clipboard") {
        std::wstringstream wentree;
        wentree << L"Bonjour tous le Monde, je m'appelle Rémi";
        REQUIRE(PasteClipboardFromStreamUnicode(wentree));
    }SECTION("Copy & Paste to Clipboard") {
        std::wstringstream wentree;
        wentree << L"Bonjour tous le Monde @^^^^, je m'appelle Rémi";
        REQUIRE(PasteClipboardFromStreamUnicode(wentree));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::wstringstream wsortie;
        REQUIRE(CopyClipboardToStreamUnicode(wsortie));
        REQUIRE(wsortie.str() == wentree.str());
    }SECTION("Convert UTF8 to WideString") {
        std::stringstream sortie;
        sortie << wstring_to_string(L"Bonjour tous le Monde, je m'appelle Rémi");
        std::wstringstream wsortie;
        ConvertUTF8StreamToWideStream(sortie, wsortie);
        REQUIRE(wsortie.str() == L"Bonjour tous le Monde, je m'appelle Rémi");
    }
}



TEST_CASE("Extract tokens", "[CSV]") {
    SECTION("SIMPLE") {
        std::string input = "9;0,297;0,703;0,146";
        std::string separator = ";";

        std::vector<std::string> tokens = split(input, separator);
        REQUIRE(tokens.size() == 4);
    }SECTION("REPLACE") {
        std::string input = "9;0,297;0,703;0,146";
        char oldCharacter = ',';
        char newCharacter = '.';
        std::string output = replaceAllChars(input, oldCharacter, newCharacter);
        REQUIRE(output == "9;0.297;0.703;0.146");
    }
}

TEST_CASE("String"){
    SECTION("Trim ' bonjour'"){
        std::string chaine = " bonjour";
        REQUIRE(trim(chaine)=="bonjour");
    }
    SECTION("Trim ' bonjour '"){
        std::string chaine = " bonjour ";
        REQUIRE(trim(chaine)=="bonjour");
    }
    SECTION("Trim 'bonjour '"){
        std::string chaine = "bonjour ";
        REQUIRE(trim(chaine)=="bonjour");
    }

}

TEST_CASE("Load in memory", "[memory]") {
    SECTION("MemStream is empty") {
        REQUIRE(isMemStreamEmpty());
        REQUIRE(memStreamLine == 0);
    }

    SECTION("Load File") {
        std::wstring path = getCurrentDirectory() + L"assets\\fizziq_prob.csv";
        REQUIRE(loadIntoMemStream(path));
        REQUIRE(memStreamLine == 0);
    }

    SECTION("MemStream not empty") {
        REQUIRE(!isMemStreamEmpty());
        REQUIRE(memStreamLine == 0);
    }SECTION("Read Line Count") {
        REQUIRE(getLineCount() == 28);
    }

    SECTION("Get FirstLine") {
        REQUIRE(getOneLine(1) == "Cahier-13:45");
        REQUIRE(memStreamLine == 0);
    }

    SECTION("Get FirstLine") {
        REQUIRE(getOneLine(1) == "Cahier-13:45");
        REQUIRE(memStreamLine == 0);
    }

    SECTION("Read Line 1") {
        std::string line;
        REQUIRE(readNextLineMemStream(line));
        REQUIRE(line == "Cahier-13:45");
        REQUIRE(memStreamLine == 1);
    }

    SECTION("Read Line 2") {
        std::string line;
        REQUIRE(readNextLineMemStream(line));
        REQUIRE(line == "");
        REQUIRE(memStreamLine == 2);
    }

    SECTION("Read Line 3") {
        std::string line;
        REQUIRE(readNextLineMemStream(line));
        REQUIRE(string_to_wstring(line) == L"FIZéZIQ;");
        REQUIRE(memStreamLine == 3);
    }

    SECTION("Read too much line") {
        std::string line;
        for (int i = 0; i < 100; ++i) {
            readNextLineMemStream(line);
        }
        REQUIRE_FALSE(readNextLineMemStream(line));
        REQUIRE(string_to_wstring(line) == L"");
        REQUIRE(memStreamLine == 28);
    }

    SECTION("MaxSizeLine check") {
        REQUIRE(checkMaxSizeLine());
    }


    SECTION("Load File too long") {
        std::wstring path = getCurrentDirectory() + L"assets\\fizziq_prob_toolong.csv";
        REQUIRE(loadIntoMemStream(path));
        REQUIRE(memStreamLine == 0);
    }

    SECTION("MaxSizeLine check") {
        REQUIRE(!checkMaxSizeLine());
    }

}

TEST_CASE("Detect Size and number of line", "[file]") {

    SECTION("File Exist and readable"){
        std::wstring path = getCurrentDirectory() + L"assets\\fizziq_prob.csv";
        REQUIRE(fileExistsAndReadable(path));
    }
    SECTION("File exist and not readable"){
        std::wstring path = getCurrentDirectory() + L"assets\\fizziq_pffrob.csv";
        REQUIRE_FALSE(fileExistsAndReadable(path));
    }
    SECTION("Size of : File CSV") {
        std::wstring path = getCurrentDirectory() + L"assets\\fizziq_prob.csv";
        REQUIRE(getFileSize(path) == 542);
    }SECTION("Size of : Binary") {
        std::wstring path = getCurrentDirectory() + L"assets\\bin.bin";
        REQUIRE(getFileSize(path) == 87040);
    }SECTION("Not too big") {
        std::wstring path = getCurrentDirectory() + L"assets\\fizziq_prob.csv";
        REQUIRE(isNotTooBig(path));
    }SECTION("Too big") {
        std::wstring path = getCurrentDirectory() + L"assets\\fizziq_prob_too_big.csv";
        REQUIRE(!isNotTooBig(path));
    }SECTION("Not too many line") {
        std::wstring path = getCurrentDirectory() + L"assets\\fizziq_prob.csv";
        REQUIRE(isNotTooManyLines(path));
    }SECTION("Too many line") {
        std::wstring path = getCurrentDirectory() + L"assets\\fizziq_prob_too_big.csv";
        REQUIRE_FALSE(isNotTooManyLines(path));
    }SECTION("Not too less line") {
        std::wstring path = getCurrentDirectory() + L"assets\\fizziq_prob.csv";
        REQUIRE(notTooLessLines(path));
    }SECTION("Too less line") {
        std::wstring path = getCurrentDirectory() + L"assets\\fizziq_prob_too_less.csv";
        REQUIRE_FALSE(notTooLessLines(path));
    }

}

TEST_CASE("Detect Binary_text", "[file]") {

    SECTION("File Exist") {
        std::wstring path = getCurrentDirectory() + L"assets\\bin.bin";
        REQUIRE(fileExistsAndReadable(path));
    };

    SECTION("Get Documents folder") {
        std::string documentDir = "C:\\Users\\remi\\Documents\\";
        REQUIRE(getDocumentsPath() == documentDir);
    }

    SECTION("File Not Exist") {
        std::wstring path = getCurrentDirectory() + L"assets\\bidn.bin";
        REQUIRE_FALSE(fileExistsAndReadable(path));
    };

    SECTION("File Binary") {
        std::wstring path = getCurrentDirectory() + L"assets\\bin.bin";
        REQUIRE(isBinary(path));
    };

    SECTION("File is text") {
        std::wstring path = getCurrentDirectory() + L"assets\\nonread.txt";
        REQUIRE(!isBinary(path));
    };

    SECTION("Can Read") {
        std::wstring path = getCurrentDirectory() + L"assets\\avimeca.txt";
        REQUIRE(isFileHumanReadable(path));
    };

    SECTION("Can't Read") {
        std::wstring path = getCurrentDirectory() + L"assets\\nonread.txt";
        REQUIRE(!isFileHumanReadable(path));
    };

}


TEST_CASE("Extract", "[file]") {
    std::wstring path = getCurrentDirectory() + L"assets\\";

    SECTION("Path is not empty") {
        REQUIRE(!path.empty());
    }

    SECTION("TEXT File") {
        for (int i = 0; i < arraySize; ++i) {
            std::wstring sectionName =
                    filename[i] + L" - " + std::to_wstring(lineStartHeader[i]) + L" - " +
                    std::to_wstring(lineStartData[i]);
            std::wstring newPath = path + filename[i];
            DYNAMIC_SECTION("-> " << wstring_to_string(sectionName)) {
                if (isFileText(newPath)) {
                    SUCCEED("File is text");
                } else {
                    FAIL("File is not text");
                };
            }
        }
    }

    SECTION("Read FirstLine") {
        for (int i = 0; i < arraySize; ++i) {
            std::wstring sectionName =
                    filename[i] + L" - " + std::to_wstring(lineStartHeader[i]) + L" - " +
                    std::to_wstring(lineStartData[i]);
            std::wstring newPath = path + filename[i];
            DYNAMIC_SECTION("-> " << wstring_to_string(sectionName)) {
                std::ifstream inFile(newPath, std::ios::in);
                if (!inFile) {

                    char errorMessage[255];
                    std::string errorMsg;

                    if (strerror_s(errorMessage, sizeof(errorMessage) / sizeof(wchar_t), errno) == 0) {
                        errorMsg = errorMessage;
                    } else {
                        errorMsg = "Could not retrieve error message.";
                    }

                    std::string message_erreur =
                            "Error: Could not open : " + wstring_to_string(newPath) + ".\r\nErrno: " +
                            std::to_string(errno) + " - " + errorMsg;
                    std::cerr << message_erreur << std::endl;
                    FAIL(message_erreur);
                } else {
                    std::string line;
                    std::getline(inFile, line);
                    REQUIRE(!line.empty());
                    inFile.close();
                }
            }
        }
    }
}
