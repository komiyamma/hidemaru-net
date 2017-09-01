#pragma once

#include "pybind11/eval.h"
#include "pybind11/embed.h"

#include "convert_string.h"
#include "output_debugstream.h"
#include "hidemaruexe_export.h"
#include "self_dll_info.h"
#include "dllfunc_interface_internal.h"

#include <iostream>
#include <windows.h>
#include <string>

namespace py = pybind11;

PyMODINIT_FUNC PyInit_hidemaru();