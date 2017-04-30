/*=============================================================================
 * TarotClub - JSEngine.h
 *=============================================================================
 * JavaScript engine class
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef JSENGINE_H
#define JSENGINE_H

#include <string>
#include <vector>
#include <iostream>
#include <type_traits>

#include "Observer.h"
#include "duktape.h"
#include "IScriptEngine.h"

/*****************************************************************************/
class JSEngine : public IScriptEngine
{
public:
    typedef duk_context JSContext;
    typedef duk_ret_t JSRetCode;
    using FunctionPtr = std::add_pointer<JSRetCode(JSContext)>::type;

    JSEngine();
    ~JSEngine();

    // From IScriptEngine
    void Initialize();
    void RegisterPrinter(IScriptEngine::IPrinter *printer);
    bool EvaluateFile(const std::string &fileName);
    bool EvaluateString(const std::string &contents, std::string &output);
    Value Call(const std::string &function, const IScriptEngine::StringList &args);
    void Close();
private:
    duk_context *mCtx;
    bool mValidContext;
    std::uint32_t mId; // Id of the current script context

    void PrintError() const;
    void PrintTop() const;

};

#endif // JSENGINE_H

//=============================================================================
// End of file JSEngine.h
//=============================================================================
