/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <WCDB/Lang.h>

namespace WCDB {

namespace Lang {

SelectSTMT::SelectSTMT() : recursive(false), offset(false)
{
}

CopyOnWriteString SelectSTMT::SQL() const
{
    std::string description;
    if (!commonTableExpressions.empty()) {
        description.append("WITH ");
        if (recursive) {
            description.append("RECURSIVE ");
        }
        description.append(commonTableExpressions.description().get() + " ");
    }

    description.append(selectCore.description().get());

    if (!compoundCores.empty()) {
        description.append(" " + compoundCores.description().get());
    }

    if (!orderingTerms.empty()) {
        description.append(" ORDER BY " + orderingTerms.description().get());
    }

    if (!limit.empty()) {
        description.append(" LIMIT " + limit.description().get());
        if (!limitParameter.empty()) {
            if (offset) {
                description.append(" OFFSET ");
            } else {
                description.append(", ");
            }
            description.append(limitParameter.description().get());
        }
    }

    return description;
}

SelectSTMT::Compound::Compound() : compoundOperator(Operator::NotSet)
{
}

CopyOnWriteString SelectSTMT::Compound::SQL() const
{
    std::string description;
    description.append(SelectSTMT::Compound::OperatorName(compoundOperator));
    description.append(" ");
    description.append(selectCore.description().get());
    return description;
}

constexpr const char *
SelectSTMT::Compound::OperatorName(const Operator &compoundOperator)
{
    switch (compoundOperator) {
        case Operator::Union:
            return "UNION";
        case Operator::UnionAll:
            return "UNION ALL";
        case Operator::Intersect:
            return "INTERSECT";
        case Operator::Except:
            return "EXCEPT";
        default:
            return "";
    }
}

STMT::Type SelectSTMT::getSTMTType() const
{
    return STMT::Type::Select;
}

STMT::Type SelectSTMT::getType()
{
    return STMT::Type::Select;
}

//TODO refactor: remove redundant calculatedDescription
template <>
CopyOnWriteString
CopyOnWriteLazyLangList<SelectSTMT::Compound>::calculatedDescription() const
{
    std::string description;
    bool space = false;
    for (const auto &element : this->get()) {
        if (space) {
            description.append(" ");
        } else {
            space = true;
        }
        LangRemedialAssert(!element.empty());
        description.append(element.description().get());
    }
    return description;
}

} // namespace Lang

} // namespace WCDB
