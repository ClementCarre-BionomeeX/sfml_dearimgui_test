#pragma once

#include <gtk/gtk.h>
#include <string>

void WaitForCleanup(void);

bool SaveFileDialog(std::string& filePath);

bool LoadFileDialog(std::string& filePath);
