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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <string>

#include "Scintilla.h"
#include "LuaExtension.h"

#include "IFaceTableMixer.h"
#include "SciIFaceTable.h"

#include "lua.hpp"

namespace {
struct LuaRuntimeState {
    IFaceTableMixer ifacemixer;
    ScintillaEdit *editor = nullptr;
    lua_State *luaState = nullptr;
    bool luaDisabled = false;
    bool tracebackEnabled = true;
};

LuaRuntimeState &runtimeState() {
    static LuaRuntimeState state;
    return state;
}
}

constexpr const char *EOFMARK = "<eof>";
constexpr size_t marklen = sizeof("<eof>") - 1;

#if !defined(LUA_QL)
#define LUA_QL(x) "'" x "'"
#endif

static bool call_function(lua_State *L, int nargs, bool ignoreFunctionReturnValue);

// Helper function from SciTE
[[maybe_unused]] static int Substitute(std::string &s, std::string_view sFind, std::string_view sReplace) {
    int c = 0;
    size_t lenFind = sFind.size();
    size_t lenReplace = sReplace.size();
    size_t posFound = s.find(sFind);
    while (posFound != std::string::npos) {
        s.replace(posFound, lenFind, sReplace);
        posFound = s.find(sFind, posFound + lenReplace);
        c++;
    }
    return c;
}

using NppExtensionAPIPane = int;
void hostTrace(const char *s)
{
    qInfo("%s", s);
}
void hostTraceError(const char *s)
{
    qWarning("%s", s);
}

// Forward declarations
static NppExtensionAPIPane check_pane_object(lua_State *L, int index);
static void push_pane_object(lua_State *L, NppExtensionAPIPane p);
static int iface_function_helper(lua_State *L, const IFaceFunction &func);

inline void raise_error(lua_State *L, const char *errMsg = nullptr) {
    luaL_where(L, 1);
    if (errMsg) {
        lua_pushstring(L, errMsg);
    } else {
        lua_insert(L, -2);
    }
    lua_concat(L, 2);
    lua_error(L);
}

inline int absolute_index(lua_State *L, int index) {
    return ((index < 0) && (index != LUA_REGISTRYINDEX)) ? (lua_gettop(L) + index + 1) : index;
}

[[maybe_unused]] static int cf_npp_send(lua_State *L) {
    // This is reinstated as a replacement for the old <pane>:send, which was removed
    // due to safety concerns.  Is now exposed as npp.SendEditor / npp.SendOutput.
    // It is rewritten to be typesafe, checking the arguments against the metadata in
    // IFaceTable in the same way that the object interface does.

    IFaceFunction propfunc;
    int paneIndex = lua_upvalueindex(1);
    check_pane_object(L, paneIndex);
    const auto message = static_cast<int>(luaL_checkinteger(L, 1));

    lua_pushvalue(L, paneIndex);
    lua_replace(L, 1);

    const IFaceFunction *func;
    func = SciIFaceTable.GetFunctionByMessage(message);

    if (func == nullptr) {
        propfunc = SciIFaceTable.GetPropertyFuncByMessage(message);
        if (propfunc.value != -1) { func = &propfunc; }
    }

    if (func != nullptr) {
        if (IFaceFunctionIsScriptable(*func)) {
            return iface_function_helper(L, *func);
        } else {
            raise_error(L, "Cannot call send for this function: not scriptable.");
            return 0;
        }
    } else {
        raise_error(L, "Message number does not match any published Scintilla / Notepad++ function or property");
        return 0;
    }
}

void stackdump(lua_State* l)
{
    int top = lua_gettop(l);

    for (int i = 1; i <= top; i++)
    {  /* repeat for each level */
        int t = lua_type(l, i);
        hostTrace(lua_typename(l, t));
        hostTrace("  ");  /* put a separator */
    }
    hostTrace("\n");  /* end the listing */
}

static NppExtensionAPIPane check_pane_object(lua_State *L, int index) {
    const auto *pPane = static_cast<const NppExtensionAPIPane *>(luaL_testudata(L, index, "Nn_MT_Pane"));

    if ((!pPane) && lua_istable(L, index)) {
        // so that nested objects have a convenient way to do a back reference
        int absIndex = absolute_index(L, index);
        lua_pushliteral(L, "pane");
        lua_gettable(L, absIndex);
        pPane = static_cast<const NppExtensionAPIPane *>(luaL_testudata(L, -1, "Nn_MT_Pane"));
    }

    if (pPane) {
        return *pPane;
    }

    pPane = static_cast<const NppExtensionAPIPane *>(luaL_testudata(L, index, "Nn_MT_Application"));

    // NOTE: I'm not sure what the above comment about the "back reference" means. This may or
    // may not apply in this case. So that if statement may need pasted/modified in this case

    if (pPane) {
        return *pPane;
    }

    if (index == 1) {
        lua_pushliteral(L, "Self object is missing in pane method or property access.");
    } else if (index == lua_upvalueindex(1)) {
        lua_pushliteral(L, "Internal error: pane object expected in closure.");
    } else {
        lua_pushliteral(L, "Pane object expected.");
    }

    raise_error(L);
    return 0; // this line never reached
}

static int cf_global_print(lua_State *L) {
    int nargs = lua_gettop(L);

    lua_getglobal(L, "tostring");

    for (int i = 1; i <= nargs; ++i) {
        if (i > 1) {
            hostTrace("\t");
        }

        const char *argStr = lua_tostring(L, i);
        if (argStr) {
            hostTrace(argStr);
        } else {
            lua_pushvalue(L, -1); // tostring
            lua_pushvalue(L, i);
            lua_call(L, 1, 1);
            argStr = lua_tostring(L, -1);
            if (argStr) {
                hostTrace(argStr);
            } else {
                raise_error(L, "tostring (called from print) returned a non-string");
            }
            lua_settop(L, nargs + 1);
        }
    }

    hostTrace("\n");
    return 0;
}

[[maybe_unused]] static bool call_function(lua_State *L, int nargs, bool ignoreFunctionReturnValue=false) {
    bool handled = false;
    if (L) {
        int traceback = 0;
        if (runtimeState().tracebackEnabled) {
            lua_getglobal(L, "debug");
            lua_getfield(L, -1, "traceback");
            lua_remove(L, -2);
            if (lua_isfunction(L, -1)) {
                traceback = lua_gettop(L) - nargs - 1;
                lua_insert(L, traceback);
            } else {
                lua_pop(L, 1);
            }
        }

        int result = lua_pcall(L, nargs, ignoreFunctionReturnValue ? 0 : 1, traceback);

        if (traceback) {
            lua_remove(L, traceback);
        }

        if (result == LUA_OK) {
            if (ignoreFunctionReturnValue) {
                handled = true;
            } else {
                handled = (0 != lua_toboolean(L, -1));
                lua_pop(L, 1);
            }
        } else if (result == LUA_ERRRUN) {
            lua_getglobal(L, "print");
            lua_insert(L, -2); // use pushed error message
            lua_pcall(L, 1, 0, 0);
        } else {
            lua_pop(L, 1);
            if (result == LUA_ERRMEM) {
                hostTraceError("Memory allocation error\n");
            } else if (result == LUA_ERRERR) {
                hostTraceError("An error occurred, but cannot be reported due to failure in _TRACEBACK\n");
            } else {
                hostTraceError("Unexpected error\n");
            }
        }
    }
    return handled;
}

struct ScintillaFailure {
    sptr_t status;
    explicit ScintillaFailure(sptr_t status_) : status(status_) {
    }
};

static void extractFunctionParams(lua_State *L, const IFaceFunction &func, int loopParamCount, std::array<sptr_t, 2> &params) {
    int arg = 2;
    if (func.value == SCI_SETMARGINLEFT || func.value == SCI_SETMARGINRIGHT) {
        params[1] = static_cast<sptr_t>(luaL_checkinteger(L, arg++));
        return;
    }

    for (int i = 0; i < loopParamCount; ++i) {
        if (func.paramType[i] == iface_string) {
            const char *s = lua_tostring(L, arg++);
            params[i] = reinterpret_cast<sptr_t>(s ? s : "");
        } else if (func.paramType[i] == iface_keymod) {
            const unsigned int keycode = static_cast<unsigned int>(static_cast<int>(luaL_checkinteger(L, arg++))) & 0xFFFFU;
            const unsigned int modifiers = static_cast<unsigned int>(static_cast<int>(luaL_checkinteger(L, arg++))) & (static_cast<unsigned int>(SCMOD_SHIFT) | static_cast<unsigned int>(SCMOD_CTRL) | static_cast<unsigned int>(SCMOD_ALT));
            params[i] = static_cast<sptr_t>(keycode | (modifiers << 16U));
        } else if (func.paramType[i] == iface_bool) {
            params[i] = lua_toboolean(L, arg++);
        } else if (IFaceTypeIsNumeric(func.paramType[i])) {
            params[i] = static_cast<sptr_t>(luaL_checkinteger(L, arg++));
        }
    }
}

static sptr_t invokeScintillaMessage(const IFaceFunction &func, const std::array<sptr_t, 2> &params) {
    try {
        return runtimeState().editor ? runtimeState().editor->send(func.value, params[0], params[1]) : 0;
    } catch (ScintillaFailure &sf) {
        std::string failureExplanation;
        failureExplanation += "Lua: Scintilla failure ";
        failureExplanation += std::to_string(sf.status);
        failureExplanation += " for message ";
        failureExplanation += std::to_string(func.value);
        failureExplanation += ".\n";
        if (runtimeState().editor) {
            runtimeState().editor->send(SCI_SETSTATUS, SC_STATUS_OK, 0);
        }
        hostTraceError(failureExplanation.c_str());
        return 0;
    }
}

static int pushFunctionResults(lua_State *L, const IFaceFunction &func, sptr_t result, const std::vector<char> &stringBuffer) {
    int resultCount = 0;
    if (!stringBuffer.empty()) {
        lua_pushstring(L, stringBuffer.data());
        resultCount++;
    }
    if (func.returnType == iface_bool) {
        lua_pushboolean(L, static_cast<int>(result));
        resultCount++;
    } else if (IFaceTypeIsNumeric(func.returnType)) {
        lua_pushinteger(L, static_cast<lua_Integer>(result));
        resultCount++;
    }
    return resultCount;
}

static int iface_function_helper(lua_State *L, const IFaceFunction &func) {
    NppExtensionAPIPane p = check_pane_object(L, 1);
    Q_UNUSED(p)

    std::array<sptr_t, 2> params = {0, 0};
    std::vector<char> stringBuffer;
    bool needStringResult = false;
    int loopParamCount = 2;

    if (func.paramType[0] == iface_length && func.paramType[1] == iface_string) {
        params[0] = static_cast<sptr_t>(lua_rawlen(L, 2));
        params[1] = reinterpret_cast<sptr_t>(params[0] ? lua_tostring(L, 2) : "");
        loopParamCount = 0;
    } else if ((func.paramType[1] == iface_stringresult) || (func.returnType == iface_stringresult)) {
        needStringResult = true;
        loopParamCount = (func.paramType[0] == iface_length) ? 0 : 1;
    }

    extractFunctionParams(L, func, loopParamCount, params);

    if (needStringResult) {
        const sptr_t stringResultLen = runtimeState().editor ? runtimeState().editor->send(func.value, params[0], 0) : 0;
        if (stringResultLen <= 0) {
            return 0;
        }
        stringBuffer.resize(static_cast<size_t>(stringResultLen + 1), '\0');
        params[1] = reinterpret_cast<sptr_t>(stringBuffer.data());
        if (func.paramType[0] == iface_length) {
            params[0] = stringResultLen;
        }
    }

    const sptr_t result = invokeScintillaMessage(func, params);
    return pushFunctionResults(L, func, result, stringBuffer);
}

struct IFacePropertyBinding {
    NppExtensionAPIPane pane;
    const IFaceProperty *prop;
};

static int cf_ifaceprop_metatable_index(lua_State *L) {
    // if there is a getter, __index calls it
    // otherwise, __index raises "property 'name' is write-only".
    const auto *ipb = static_cast<const IFacePropertyBinding *>(luaL_testudata(L, 1, "Nn_MT_IFacePropertyBinding"));
    if (!(ipb && IFacePropertyIsScriptable(*(ipb->prop)))) {
        raise_error(L, "Internal error: property binding is improperly set up");
        return 0;
    }
    if (ipb->prop->getter == 0) {
        raise_error(L, "Attempt to read a write-only indexed property");
        return 0;
    }
    IFaceFunction func = ipb->prop->GetterFunction();

    // rewrite the stack to match what the function expects.  put pane at index 1; param is already at index 2.
    push_pane_object(L, ipb->pane);
    lua_replace(L, 1);
    lua_settop(L, 2);
    return iface_function_helper(L, func);
}

static int cf_ifaceprop_metatable_newindex(lua_State *L) {
    const auto *ipb = static_cast<const IFacePropertyBinding *>(luaL_testudata(L, 1, "Nn_MT_IFacePropertyBinding"));
    if (!(ipb && IFacePropertyIsScriptable(*(ipb->prop)))) {
        raise_error(L, "Internal error: property binding is improperly set up");
        return 0;
    }
    if (ipb->prop->setter == 0) {
        raise_error(L, "Attempt to write a read-only indexed property");
        return 0;
    }
    IFaceFunction func = ipb->prop->SetterFunction();

    // rewrite the stack to match what the function expects.
    // pane at index 1; param at index 2, value at index 3
    push_pane_object(L, ipb->pane);
    lua_replace(L, 1);
    lua_settop(L, 3);
    return iface_function_helper(L, func);
}

static int cf_pane_iface_function(lua_State *L) {
    int funcidx = lua_upvalueindex(1);
    const IFaceFunction *func = static_cast<IFaceFunction *>(lua_touserdata(L, funcidx));
    if (func) {
        return iface_function_helper(L, *func);
    } else {
        raise_error(L, "Internal error - bad upvalue in iface function closure");
        return 0;
    }
}

static int push_iface_function(lua_State *L, const char *name, const IFaceTableInterface *iface) {
    if (const auto *func = iface->FindFunction(name); func != nullptr && IFaceFunctionIsScriptable(*func)) {
        lua_pushlightuserdata(L, static_cast<void *>(const_cast<IFaceFunction *>(func)));
        lua_pushcclosure(L, cf_pane_iface_function, 1);
        return 1;
    }
    return -1; // signal to try next pane index handler
}

static int push_iface_propval(lua_State *L, const char *name, const IFaceTableInterface *iface) {
    // this function doesn't raise errors, but returns 0 if the function is not handled.
    const auto *prop = iface->FindProperty(name);
    if (!prop) {
        return -1;
    }

    if (!IFacePropertyIsScriptable(*prop)) {
        raise_error(L, "Error: iface property is not scriptable.");
        return -1;
    }

    if (prop->paramType == iface_void) {
        if (prop->getter) {
            lua_settop(L, 1);
            return iface_function_helper(L, prop->GetterFunction());
        }
        return -1;
    }

    if (prop->paramType == iface_bool) {
        NppExtensionAPIPane p = check_pane_object(L, 1);
        Q_UNUSED(p)

        if (prop->getter) {
            if (runtimeState().editor && runtimeState().editor->send(prop->getter, 1, 0)) {
                lua_pushnil(L);
                return 1;
            }
            lua_settop(L, 1);
            lua_pushboolean(L, 0);
            return iface_function_helper(L, prop->GetterFunction());
        }
        return -1;
    }

    // Indexed property
    auto *ipb = static_cast<IFacePropertyBinding *>(lua_newuserdata(L, sizeof(IFacePropertyBinding)));
    if (!ipb) {
        raise_error(L, "Internal error: failed to allocate userdata for indexed property");
        return -1;
    }

    ipb->pane = check_pane_object(L, 1);
    ipb->prop = prop;
    if (luaL_newmetatable(L, "Nn_MT_IFacePropertyBinding")) {
        lua_pushliteral(L, "__index");
        lua_pushcfunction(L, cf_ifaceprop_metatable_index);
        lua_settable(L, -3);
        lua_pushliteral(L, "__newindex");
        lua_pushcfunction(L, cf_ifaceprop_metatable_newindex);
        lua_settable(L, -3);
    }
    lua_setmetatable(L, -2);
    return 1;
}

static bool try_get_metatable_field(lua_State *L) {
    lua_getmetatable(L, 1);
    if (!lua_istable(L, -1)) {
        return false;
    }
    lua_pushvalue(L, 2);
    lua_gettable(L, -2);
    return !lua_isnil(L, -1);
}

static int cf_pane_metatable_index(lua_State *L) {
    const auto *iface = static_cast<const IFaceTableInterface *>(lua_touserdata(L, lua_upvalueindex(1)));
    if (lua_isstring(L, 2)) {
        const char *name = lua_tostring(L, 2);

        // these return the number of values pushed (possibly 0), or -1 if no match
        int results = push_iface_function(L, name, iface);
        if (results < 0) {
            results = push_iface_propval(L, name, iface);
        }

        if (results >= 0) {
            return results;
        }

        if (name[0] != '_' && try_get_metatable_field(L)) {
            return 1;
        }
    }

    raise_error(L, "Pane function / readable property / indexed writable property name expected");
    return 0;
}

static int cf_pane_metatable_newindex(lua_State *L) {
    const auto *iface = static_cast<const IFaceTableInterface *>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!lua_isstring(L, 2)) {
        raise_error(L, "Error - (pane object) expected the name of a writable property");
        return 0;
    }

    const auto *prop = iface ? iface->FindProperty(lua_tostring(L, 2)) : nullptr;
    if (!prop || !IFacePropertyIsScriptable(*prop)) {
        raise_error(L, "Error - (pane object) expected the name of a writable property");
        return 0;
    }

    if (!prop->setter) {
        raise_error(L, "Error - (pane object) cannot assign to a read-only property");
        return 0;
    }

    // stack needs to be rearranged to look like an iface function call
    lua_remove(L, 2);
    if (prop->paramType == iface_void) {
        return iface_function_helper(L, prop->SetterFunction());
    }
    if (prop->paramType == iface_bool) {
        if (!lua_isnil(L, 3)) {
            lua_pushboolean(L, 1);
            lua_insert(L, 2);
        } else {
            lua_pushinteger(L, 0);
        }
        return iface_function_helper(L, prop->SetterFunction());
    }

    raise_error(L, "Error - (pane object) cannot assign directly to indexed property");
    return 0;
}

static void push_pane_object(lua_State *L, NppExtensionAPIPane p) {
    *static_cast<NppExtensionAPIPane *>(lua_newuserdata(L, sizeof(p))) = p;
    if (luaL_newmetatable(L, "Nn_MT_Pane")) {
        lua_pushlightuserdata(L, &SciIFaceTable);
        lua_pushcclosure(L, cf_pane_metatable_index, 1);
        lua_setfield(L, -2, "__index");
        lua_pushlightuserdata(L, &SciIFaceTable);
        lua_pushcclosure(L, cf_pane_metatable_newindex, 1);
        lua_setfield(L, -2, "__newindex");

        // Push built-in functions into the metatable, where the custom
        // __index metamethod will find them.
    }
    lua_setmetatable(L, -2);
}

static int cf_global_metatable_index(lua_State *L) {
    if (lua_isstring(L, 2)) {
        const char *name = lua_tostring(L, 2);
        if ((name[0] < 'A') || (name[0] > 'Z') || ((name[1] >= 'a') && (name[1] <= 'z'))) {
            // short circuit; iface constants are always upper-case and start with a letter
            return 0;
        }

        if (const auto *con = runtimeState().ifacemixer.FindConstant(name); con != nullptr) {
            lua_pushinteger(L, con->value);
            return 1;
        }
        if (const auto *func = runtimeState().ifacemixer.FindFunctionByConstantName(name); func != nullptr) {
            lua_pushinteger(L, func->value);

            // FindFunctionByConstantName is slow, so cache the result into the
            // global table.  My tests show this gives an order of magnitude
            // improvement.
            lua_pushvalue(L, 2);
            lua_pushvalue(L, -2);
            lua_rawset(L, 1);

            return 1;
        }
    }

    return 0; // global namespace access should not raise errors
}

// cppcheck-suppress constParameterCallback
static int LuaPanicFunction(lua_State *L) {
    if (L && L == runtimeState().luaState) {
        lua_settop(L, 0);
        lua_close(runtimeState().luaState);
        runtimeState().luaState = nullptr;
        runtimeState().luaDisabled = true;
    }
    hostTraceError("\nError occurred in unprotected call.  This is very bad.\n");
    return 1;
}

static bool InitGlobalScope() {
    auto &state = runtimeState();
    state.tracebackEnabled = true;

    if (state.luaDisabled) {
        return false;
    }
    if (!state.luaState) {
        state.luaDisabled = true;
        hostTraceError("Scripting engine failed to initialise\n");
        return false;
    }

    lua_atpanic(state.luaState, LuaPanicFunction);
    state.ifacemixer.AddIFaceTable(&SciIFaceTable);

    // ...register standard libraries
    luaL_openlibs(state.luaState); // NOLINT(bugprone-signed-bitwise)

    // Sandbox unsafe OS/IO and dynamic loading vectors (CVE-2026-42214 / CWE-94)
    luaL_dostring(state.luaState,
        "if os then os.execute = nil; os.remove = nil; os.rename = nil; os.exit = nil end "
        "if io then io.popen = nil end "
        "if package then package.loadlib = nil end");

    // override a library function whose default impl uses stdout
    lua_register(state.luaState, "print", cf_global_print);

    // pane objects
    push_pane_object(state.luaState, 0);
    lua_setglobal(state.luaState, "editor");

    // get global environment table from registry
    lua_pushglobaltable(state.luaState);
    // Metatable for global namespace, to publish iface constants
    if (luaL_newmetatable(state.luaState, "Nn_MT_GlobalScope")) {
        lua_pushcfunction(state.luaState, cf_global_metatable_index);
        lua_setfield(state.luaState, -2, "__index");
    }
    // set global index callback hook
    lua_setmetatable(state.luaState, -2);
    // remove the global environment table from the stack
    lua_pop(state.luaState, 1);

    return true;
}

LuaExtension::LuaExtension() = default;


LuaExtension &LuaExtension::Instance() {
    static LuaExtension singleton;
    return singleton;
}

// cppcheck-suppress functionStatic
bool LuaExtension::Initialise(lua_State *L, ScintillaNext *editor_) const {
    auto &state = runtimeState();
    state.luaState = L;
    state.editor = editor_;
    InitGlobalScope();

    return false;
}

// cppcheck-suppress functionStatic
void LuaExtension::setEditor(ScintillaEdit *editor_) const {
    runtimeState().editor = editor_;
}

// cppcheck-suppress functionStatic
bool LuaExtension::Finalise() const {
    auto &state = runtimeState();
    if (state.luaState) {
        lua_close(state.luaState);
    }

    state.luaState = nullptr;
    state.editor = nullptr;

    return false;
}

// cppcheck-suppress functionStatic
bool LuaExtension::RunString(const char *s) const {
    if (!s) {
        return false;
    }

    const std::string_view sv(s);
    if (auto &state = runtimeState(); state.luaState || InitGlobalScope()) {
        int status = luaL_loadbuffer(state.luaState, sv.data(), sv.size(), "=File");

        if (status == LUA_OK) {
            status = lua_pcall(state.luaState, 0, LUA_MULTRET, 0);
        }

        if (status != LUA_OK) {
            // Print an error message
            hostTraceError(lua_tostring(state.luaState, -1));
            hostTraceError("\n");
            lua_settop(state.luaState, 0); /* clear stack */
            return false;
        }

        lua_settop(state.luaState, 0); /* clear stack */
    }

    return true;
}

static bool handleConsoleReturnStatement(lua_State *L, const std::string_view &sv, int &status) {
    // First try to compile the chunk as a return statement
    lua_pushfstring(L, "return %.*s;", static_cast<int>(sv.size()), sv.data());
    size_t retlen = 0;
    const char *retline = lua_tolstring(L, -1, &retlen);
    status = luaL_loadbuffer(L, retline, retlen, "=Console");
    if (status == 0) {
        lua_remove(L, -2);
    } else {
        lua_pop(L, 2);
    }

    if (status == LUA_OK) {
        status = lua_pcall(L, 0, LUA_MULTRET, 0);
        return true;
    }
    return false;
}

static bool checkEofError(lua_State *L, int status) {
    if (status != LUA_ERRSYNTAX) {
        return false;
    }
    size_t lmsg = 0;
    if (const char *msg = lua_tolstring(L, -1, &lmsg); msg && lmsg >= marklen && strcmp(msg + lmsg - marklen, EOFMARK) == 0) {
        lua_pop(L, 1);
        return true;
    }
    return false;
}

static void printExecutionResult(lua_State *L, int status) {
    if (status == LUA_OK) {
        if (lua_gettop(L) > 0) {
            lua_getglobal(L, "print");
            lua_insert(L, 1);
            if (lua_pcall(L, lua_gettop(L) - 1, 0, 0) != 0) {
                hostTraceError("error calling 'print'");
            }
        }
    } else {
        const char *errmsg = lua_tostring(L, -1);
        hostTraceError(errmsg ? errmsg : "error");
        hostTraceError("\n");
    }
    lua_settop(L, 0);
}

// cppcheck-suppress functionStatic
bool LuaExtension::OnExecute(const char *s) const {
    if (!s) {
        return false;
    }

    const std::string_view sv(s);
    auto &state = runtimeState();
    if (!state.luaState && !InitGlobalScope()) {
        return true;
    }

    lua_State *L = state.luaState;
    static bool isFirstLine = true;
    static std::string chunk;

    int status = LUA_OK;
    if (isFirstLine) {
        if (!handleConsoleReturnStatement(L, sv, status)) {
            status = luaL_loadbuffer(L, sv.data(), sv.size(), "=Console");
            if (status == LUA_OK) {
                status = lua_pcall(L, 0, LUA_MULTRET, 0);
            } else if (checkEofError(L, status)) {
                isFirstLine = false;
                chunk = sv;
                return false;
            }
        }
    } else {
        chunk.append("\n");
        chunk.append(sv);
        status = luaL_loadbuffer(L, chunk.data(), chunk.size(), "=Console");
        if (status == LUA_OK) {
            status = lua_pcall(L, 0, LUA_MULTRET, 0);
        } else if (checkEofError(L, status)) {
            return false;
        }
    }

    chunk.clear();
    isFirstLine = true;
    printExecutionResult(L, status);
    return true;
}

void LuaExtension::CallShortcut(int /*id*/) const {
    // Reserved for future shortcut event dispatching
}
