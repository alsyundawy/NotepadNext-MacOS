/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry DS Alsyundawy <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "IFaceTable.h"
#include <cstdio>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <iterator>

template<std::random_access_iterator Iter>
typename std::iterator_traits<Iter>::value_type const *binary_find(Iter begin, Iter end, const char *name) {
    if (!name) {
        return nullptr;
    }
    using ElementType = typename std::iterator_traits<Iter>::value_type;
    auto it = std::lower_bound(begin, end, name, [](const ElementType &lhs, const char *rhs) -> bool {
        if (!lhs.name || !rhs) { return false; }
        return strcmp(lhs.name, rhs) < 0;
    });

    if (it == end || !it->name || strcmp(name, it->name) != 0) {
        return nullptr;
    }
    else {
        return &(*it);
    }
}

const IFaceConstant *IFaceTable::FindConstant(const char *name) const {
    return binary_find(constants.cbegin(), constants.cend(), name);
}

const IFaceFunction *IFaceTable::FindFunction(const char *name) const {
    return binary_find(functions.cbegin(), functions.cend(), name);
}

const IFaceFunction *IFaceTable::FindFunctionByConstantName(const char *name) const {
    if (!name || !prefix) {
        return nullptr;
    }
    if (const size_t prefixLen = strlen(prefix); strncmp(name, prefix, prefixLen) == 0) {
        // This looks like a constant for an iface function.  This requires
        // a sequential search.  Take special care since the function names
        // are mixed case, whereas the constants are all-caps.

        for (const auto &func : functions) {
            if (!func.name) { continue; }
            const char *nm = name + prefixLen;
            const char *fn = func.name;
            while (*nm && *fn && (*nm == toupper(static_cast<unsigned char>(*fn)))) {
                ++nm;
                ++fn;
            }
            if (!*nm && !*fn) {
                return &func;
            }
        }
    }
    return nullptr;
}

const IFaceFunction *IFaceTable::FindFunctionByValue(int value) const {
    auto it = std::ranges::find_if(functions, [value](const IFaceFunction &func) {
        return func.value == value;
    });
    return (it != functions.cend()) ? std::to_address(it) : nullptr;
}

const IFaceProperty *IFaceTable::FindProperty(const char *name) const {
    return binary_find(properties.cbegin(), properties.cend(), name);
}

int IFaceTable::GetConstantName(int value, char *nameOut, unsigned nameBufferLen, const char *hint) const {
    if (nameOut && nameBufferLen > 0) {
        *nameOut = '\0';
    }

    if (!prefix) {
        return 0;
    }

    const size_t prefixLen = strlen(prefix);

    // Look in both the constants table and the functions table.  Start with functions.
    for (const auto &func : functions) {
        if (func.value == value && func.name) {
            const auto len = static_cast<int>(strlen(func.name) + prefixLen);
            if (nameOut && (static_cast<int>(nameBufferLen) > len)) {
                std::snprintf(nameOut, nameBufferLen, "%s%s", prefix, func.name);
                // fix case
                for (char *nm = nameOut + prefixLen; *nm; ++nm) {
                    if (*nm >= 'a' && *nm <= 'z') {
                        *nm = static_cast<char>(*nm - 'a' + 'A');
                    }
                }
                return len;
            } else {
                return -1 - len;
            }
        }
    }

    const size_t hintLen = hint ? strnlen(hint, 256) : 0;
    for (const auto &con : constants) {
        if (con.value == value && con.name && (hint == nullptr || strncmp(hint, con.name, hintLen) == 0)) {
            const auto len = static_cast<int>(strlen(con.name));
            if (nameOut && (static_cast<int>(nameBufferLen) > len)) {
                std::snprintf(nameOut, nameBufferLen, "%s", con.name);
                return len;
            } else {
                return -1 - len;
            }
        }
    }

    return 0;
}

const IFaceFunction *IFaceTable::GetFunctionByMessage(int message) const {
    auto it = std::ranges::find_if(functions, [message](const IFaceFunction &func) {
        return func.value == message;
    });
    return (it != functions.cend()) ? std::to_address(it) : nullptr;
}

IFaceFunction IFaceTable::GetPropertyFuncByMessage(int message) const {
    for (const auto &prop : properties) {
        if (prop.getter == message) {
            return prop.GetterFunction();
        }
        else if (prop.setter == message) {
            return prop.SetterFunction();
        }
    }
    return { "invalid", -1, iface_void, {iface_void, iface_void} };
}

std::vector<std::string> IFaceTable::GetAllConstantNames() const {
    std::vector<std::string> kws;
    std::string prefix_str(prefix);

    kws.reserve(constants.size() + functions.size());

    std::ranges::transform(constants, std::back_inserter(kws), [](const IFaceConstant &ifc) { return ifc.name; });
    std::ranges::transform(functions, std::back_inserter(kws), [&](const IFaceFunction &iff) {
        std::string s = prefix_str + std::string(iff.name);
        std::ranges::transform(s, s.begin(), ::toupper);
        return s;
    });

    std::ranges::sort(kws);
    return kws;
}

std::vector<std::string> IFaceTable::GetAllFunctionNames() const {
    std::vector<std::string> kws;

    kws.reserve(functions.size());
    std::ranges::transform(functions, std::back_inserter(kws), [](const IFaceFunction &iff) { return iff.name; });

    return kws;
}

std::vector<std::string> IFaceTable::GetAllPropertyNames() const {
    std::vector<std::string> kws;

    kws.reserve(properties.size());
    std::ranges::transform(properties, std::back_inserter(kws), [](const IFaceProperty &ifp) { return ifp.name; });

    return kws;
}
