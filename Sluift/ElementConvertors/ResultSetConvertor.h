/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/ResultSet.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class ResultSetConvertor : public GenericLuaElementConvertor<ResultSet> {
        public:
            ResultSetConvertor();
            virtual ~ResultSetConvertor() override;

            virtual std::shared_ptr<ResultSet> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<ResultSet>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}

