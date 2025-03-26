#ifndef SCANNERUSB_H
#define SCANNERUSB_H

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <atomic>
#include <filesystem>

namespace fs = std::filesystem;

extern std::atomic<bool> stopLoading;

void ScannerUSB(const std::string& path);

#endif // SCANNERUSB_H
