/*
 * Turbo Rascal Syntax error, “;” expected but “BEGIN” (TRSE, Turbo Rascal SE)
 * 8 bit software development IDE for the Commodore 64
 * Copyright (C) 2018  Nicolaas Ervik Groeneboom (nicolaas.groeneboom@gmail.com)
 *
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program (LICENSE.txt).
 *   If not, see <https://www.gnu.org/licenses/>.
*/

#include "nodevar.h"


NodeVar::NodeVar(Token t):Node() {
    m_op = t;
    value = t.m_value;
}

NodeVar::NodeVar(Token t, QSharedPointer<Node> expr) : Node() {
    m_op = t;
    value = t.m_value;
    m_expr = expr;
    //  if (m_op.m_type==TokenType::INTEGER)
    //     m_isWord = true;

}



TokenType::Type NodeVar::getOrgType(Assembler *as) {

    if (as==nullptr) {
        // Use parser symbtab
        TokenType::Type t = m_op.m_type;
        if (parserSymTab != nullptr) {
            QSharedPointer<Symbol> s = parserSymTab->Lookup(value, m_op.m_lineNumber);
//            qDebug() << "NodeVar::getType "<< s->m_name << TokenType::getType(s->getTokenType()) << " with forcetype " << TokenType::getType(m_forceType);
            if (s!=nullptr)
                t= s->getTokenType();
        }

        return t;
    }

    TokenType::Type t = m_op.m_type;
    if (as->m_symTab->Lookup(value, m_op.m_lineNumber)!=nullptr)
        t= as->m_symTab->Lookup(value, m_op.m_lineNumber)->getTokenType();

    return t;
}



TokenType::Type NodeVar::getType(Assembler *as) {

    if (as==nullptr) {
        // Use parser symbtab
        TokenType::Type t = m_op.m_type;
        if (parserSymTab != nullptr) {
            QSharedPointer<Symbol> s = parserSymTab->Lookup(value, m_op.m_lineNumber);
//            qDebug() << "NodeVar::getType "<< s->m_name << TokenType::getType(s->getTokenType()) << " with forcetype " << TokenType::getType(m_forceType);
            if (s!=nullptr)
                t= s->getTokenType();

        }
        //qDebug() << "Forcetype = " << TokenType::getType(m_forceType);
        if ((m_forceType!=TokenType::NADA))// && (t!=TokenType::POINTER && m_expr==nullptr))
            return m_forceType;

        return t;
    }

    TokenType::Type t = m_op.m_type;
    QSharedPointer<Symbol> s = as->m_symTab->Lookup(value, m_op.m_lineNumber);
//    qDebug() << "VAL "<<getValue(as) ;
    if (s!=nullptr)
        t= as->m_symTab->Lookup(getValue(as), m_op.m_lineNumber)->getTokenType();


//    qDebug() <<  + " " + TokenType::getType(t);



    if (m_forceType!=TokenType::NADA && t!=TokenType::POINTER)
        return m_forceType;


//    if (as->m_symTab->Lookup(value, m_op.m_lineNumber)!=nullptr)
  //      return as->m_symTab->Lookup(value, m_op.m_lineNumber)->getTokenType();

    return t;
}



TokenType::Type NodeVar::getArrayType(Assembler *as)
{
    TokenType::Type t = m_op.m_type;
    if (as==nullptr)
        return t;
    if (as->m_symTab->Lookup(getValue(as), m_op.m_lineNumber)!=nullptr)
        t= as->m_symTab->Lookup(getValue(as), m_op.m_lineNumber)->m_arrayType;


/*    if (m_subNode!=nullptr) {
        qDebug() << getValue(as) << TokenType::getType(as->m_symTab->Lookup(getValue(as), m_op.m_lineNumber)->m_arrayType);

//         Ok this is an array of a record. Need to find the actual type
//        if (m_subNode!=nullptr) {
  //          QSharedPointer<Symbol> s = as->m_symTab->Lookup(v,m_op.m_lineNumber,true);

    }
*/
/*    if (m_forceType!=TokenType::NADA && t!=TokenType::POINTER)
        return m_forceType;
*/
    return t;
}

bool NodeVar::isPointer(Assembler *as)
{
    return as->m_symTab->Lookup(value, m_op.m_lineNumber)->getTokenType()==TokenType::POINTER;

}

bool NodeVar::DataEquals(QSharedPointer<Node> other) {
    QSharedPointer<NodeVar> var = qSharedPointerDynamicCast<NodeVar>(other);
    if (var==nullptr)
        return false;
    return var->value==value;
}

bool NodeVar::isWord(Assembler *as) {

    //    if (getType(as)==TokenType::POINTER)
    //    qDebug() << "NodeVar isWord : is array: " << (getType(as)==TokenType::ADDRESS) << TokenType::getType(getType(as));
    //    if (getType(as)==TokenType::ADDRESS && m_expr!=nullptr)
    if (isReference())
        return true;

    if (getType(as)==TokenType::ADDRESS)
        return (getArrayType(as)==TokenType::INTEGER);
    //qDebug() << "  Array type : " << getArrayType(as);
    // qDebug() << "NodeVar is Word : " << (getType(as)==TokenType::ARRAY) << getArrayType(as);
    //    if (m_fake16bit)
    //      qDebug() << "FAKE 16 bit: "<<value;
    if (getType(as)==TokenType::POINTER && m_expr!=nullptr) {
        if (getArrayType(as)==TokenType::INTEGER)
            return true;
    }

    return getType(as)==TokenType::INTEGER || (getType(as)==TokenType::POINTER && m_expr==nullptr) || m_fake16bit;
    /*    return m_op.m_type==TokenType::INTEGER;
    QSharedPointer<Symbol> s = as->m_symTab->Lookup(value, m_op.m_lineNumber);

    return s->m_type.toLower()=="integer";*/
}


bool NodeVar::isLong(Assembler *as) {
    return getType(as)==TokenType::LONG  && m_expr==nullptr;
}
bool NodeVar::isByte(Assembler *as) {
    return getType(as)==TokenType::BYTE  && m_expr==nullptr;
}

bool NodeVar::containsPointer(Assembler *as)
{
    return (getType(as)==TokenType::POINTER && m_expr!=nullptr);
}

bool NodeVar::isRecord(Assembler *as)
{
    QSharedPointer<Symbol> s = as->m_symTab->Lookup(value,m_op.m_lineNumber,true);
    QString t = s->m_type;
    if (t=="address") {
        t = s->m_arrayTypeText;
    }
    return as->m_symTab->m_records.contains(t);
}


bool NodeVar::isRecordData(Assembler *as)
{
    return m_subNode!=nullptr;
}

bool NodeVar::typeIsArray(Assembler *as)
{
    if (as==nullptr)
        return false;

    QString v = getValue(as);
    QString t = as->m_symTab->Lookup(v, m_op.m_lineNumber)->m_type.toLower();
    //            qDebug() << v<< "TYPE" << as->m_symTab->Lookup(v, m_op.m_lineNumber)->m_type <<m_op.getType();
    if (m_op.m_type!=TokenType::ADDRESS) // const screen_bg_col etc
        if (!as->m_symTab->m_constants.contains(v))
             if ((t=="address") || t=="incbin"  || t=="incsid" || t=="incnsf" || t=="cstring" || t=="string")
                    if (!isPointer(as))
                        return true;


    return false;
}

QString NodeVar::getValue8bit(Assembler *as, bool isHi) {

    if (isReference()) {
        if (isHi)
            return "#>"+getValue(as);
        else
            return "#<"+getValue(as);
    }

    if (isHi) {
        if (getOrgType(as)==TokenType::BYTE)
            return "#0";
       return getValue(as)+"+1";
    }
    else return getValue(as);
}



QString NodeVar::getTypeText(Assembler *as)
{
    QSharedPointer<Symbol> s = as->m_symTab->Lookup(value,m_op.m_lineNumber,true);
    QString t = s->m_type;
    if (t=="address")
        t = s->m_arrayTypeText;
    return t;

}

QString NodeVar::getValue(Assembler* as) {
    QString v= value;
//    return v;
//    qDebug() << v;
    if (m_subNode!=nullptr) {
        QSharedPointer<Symbol> s = as->m_symTab->Lookup(v,m_op.m_lineNumber,true);

        QString type = s->m_type;
        if ((m_expr!=nullptr) && !m_ignoreRecordExpr) { // Has array indices
            type = s->m_arrayTypeText;
//            qDebug() << v << type;
  //          qDebug() << as->m_symTab->m_symbols.keys();
//            exit(1);
    }

//        qDebug() << "NodeVar getValue"

        if (!as->m_symTab->m_records.contains(type))
                ErrorHandler::e.Error("Could not find of record type : "+type + " of " + v,m_op.m_lineNumber);
        //QSharedPointer<SymbolTable>  t = as->m_symTab->m_records[type];
        v =v + "_"+type+"_"+qSharedPointerDynamicCast<NodeVar>(m_subNode)->value;//m_subNode->getValue(as);
    }
    if (as->m_symTab->getCurrentProcedure()!="") {
        //value = value.replace(as->m_symTab->getCurrentProcedure(),"");
        QString tstv = as->m_symTab->getCurrentProcedure()+value;
        if (as->m_symTab->m_symbols.contains(tstv))
                v = tstv;


    }
//    if (as!=nullptr)
/*        if (as!=nullptr && Data::data.compilerState == Data::DISPATCHER) {
            QString t = as->m_symTab->Lookup(v, m_op.m_lineNumber)->m_type.toLower();
//            qDebug() << v<< "TYPE" << as->m_symTab->Lookup(v, m_op.m_lineNumber)->m_type <<m_op.getType();
            if (m_op.m_type!=TokenType::ADDRESS) // const screen_bg_col etc
            if (!as->m_symTab->m_constants.contains(v))
            if ((t=="address") || t=="incbin"  || t=="incsid" || t=="incnsf")
                if (!isReference() && !isArrayIndex() && !isPointer(as))
                    ErrorHandler::e.Error("Unknown usage of data or array. <font color=\"orange\">Did you mean to reference it? (#"+v+")</font>",m_op.m_lineNumber);

        }
*/

//    int a;
  //      int* c=a;

//    qDebug() << "NodeVar:: getValue : " << value << "  "  << TokenType::getType(getType(as));
//    if (m_forceAddress && !(getType(as)==TokenType::POINTER)) return "#" + v;
//    if (m_forceAddress && !(getType(as)==TokenType::POINTER)) return "#" + v;
    if (isReference() && Syntax::s.m_currentSystem->m_processor == AbstractSystem::M68000) return "#" + v;
//    if (m_forceAddress && !(isPointer(as))) return "#" + v;
    return v;
}

bool NodeVar::isAddress() {
    //  return m_op.m_type==TokenType::ADDRESS;
    return true;
}

bool NodeVar::isSigned(Assembler* as)
{
    QSharedPointer<Symbol> s = as->m_symTab->Lookup(getValue(as), m_op.m_lineNumber);
    return s->m_flags.contains("signed");
}


void NodeVar::ExecuteSym(QSharedPointer<SymbolTable> symTab) {
    QString varName = m_op.m_value;
    QSharedPointer<Symbol> varSymbol = symTab->Lookup(varName, m_op.m_lineNumber);

}
/*
 * VerifyReferences throws an error if type needs to be referenced
 *
 *
*/
void NodeVar::VerifyReferences(Assembler *as) {

    if (!isPointer(as))
        if (!isReference())
            if (!isArrayIndex())
                if (typeIsArray(as))
                ErrorHandler::e.Error("Unknown usage of data or array. <font color=\"orange\">Did you mean to reference it? (#"+getValue(as)+")</font>",m_op.m_lineNumber);

    if (isPointer(as) && isReference() && !isArrayIndex())
        ErrorHandler::e.Warning("Referencing the address of a pointer <font color=\"orange\">("+getValue(as)+")</font>. Is this what you intended?</font>",m_op.m_lineNumber);

}
