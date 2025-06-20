// <copyright file="pch.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_OXRSWITCH_PCH_H)
#define _OXRSWITCH_PCH_H
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iterator>
#include <regex>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <sal.h>

#include <nlohmann/json.hpp>

#include <wil/filesystem.h>
#include <wil/registry.h>
#include <wil/resource.h>
#include <wil/result.h>

#endif /* !defined(_OXRSWITCH_PCH_H) */
