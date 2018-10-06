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

#include "nodeforloop.h"
#include "nodenumber.h"

NodeForLoop::NodeForLoop(Node *a, Node *b, Node *block, Node *step, bool unroll, int forcePage, int loopCounter) :Node(){
    m_a = a;
    m_b = b;
    m_block = block;
    m_forcePage = forcePage;
    m_step = step;
    m_loopCounter = loopCounter;
    //        m_op = op;
}

void NodeForLoop::Compare(Assembler *as) {

    if (m_loopCounter==0) {
        as->ClearTerm();
        m_b->Build(as);
        as->Term();
        as->Asm("cmp " + as->m_stack["for"].current());
    }
    if (m_loopCounter==1) {
        as->ClearTerm();
        as->Term("cpx ");
        m_b->Build(as);
        as->Term();
    }
    if (m_loopCounter==2) {
        as->ClearTerm();
        as->Term("cpy ");
        m_b->Build(as);
        as->Term();
    }

}

void NodeForLoop::IncreaseCounter(Assembler *as) {
    if (m_step==nullptr) {
        if (m_loopCounter==0)
            as->Asm("inc " + as->m_stack["for"].current());
        if (m_loopCounter==1)
            as->Asm("inx");
        if (m_loopCounter==2)
            as->Asm("iny");
    }
    else {
        if (m_step->isMinusOne()) {
            if (m_loopCounter==0)
                as->Asm("dec " + as->m_stack["for"].current());
            if (m_loopCounter==1)
                as->Asm("dex");
            if (m_loopCounter==2)
                as->Asm("dey");

            return;
        }
        if (m_loopCounter!=0)
            ErrorHandler::e.Error("Error: Loop with step other than 1,-1 cannot have loopy/loopx flag");

        as->Asm("clc");
        as->Asm("lda " + as->m_stack["for"].current());
        as->ClearTerm();
        as->Term("adc ");
        m_step->Build(as);
        as->Term();
        as->Asm("sta "+as->m_stack["for"].current());
    }

}

PVar NodeForLoop::Execute(SymbolTable *symTab, uint lvl) {
    Pmm::Data::d.Set(m_op.m_lineNumber, m_op.m_currentLineText);
    level = lvl+1;
    ErrorHandler::e.DebugLow("Calling Forloop Node",level);
    PVar a = m_a->Execute(symTab, level);
    PVar b = m_b->Execute(symTab, level);
    NodeAssign* avar = (NodeAssign*)m_a;
    NodeVar* var = (NodeVar*)avar->m_left;

    //float val = symTab->Lookup(var->value)->m_value->m_fVal;

    for (float i = a.m_fVal;i<=b.m_fVal;i++) {
        symTab->Lookup(var->value, m_op.m_lineNumber)->m_value->m_fVal = i;
        m_block->Execute(symTab, level);
    }

    return PVar();

}

void NodeForLoop::Delete() {
    if (m_a) {
        m_a->Delete();
        delete m_a;
        m_a = nullptr;
    }
    if (m_b) {
        m_b->Delete();
        delete m_b;
        m_b = nullptr;
    }
    if (m_block) {
        m_block->Delete();
        delete m_block;
        m_block = nullptr;
    }


}

void NodeForLoop::LargeLoop(Assembler *as)
{
    QString loopForFix = as->NewLabel("forLoopFix");
    QString loopDone = as->NewLabel("forLoopDone");

    /*    Compare(as);
    as->Asm("bne "+loopForFix);
    as->Asm("jmp "+loopDone);*/
    as->Label(loopForFix);
    m_block->Build(as);
    //    as->EndForLoop(m_b);
    as->m_stack["for"].pop();

    IncreaseCounter(as);
    Compare(as);
    as->Asm("beq "+loopDone);

    as->Asm("jmp " + as->getLabel("for"));

    as->Label(loopDone);

    as->m_labelStack["for"].pop();
    as->m_labelStack["forLoopFix"].pop();
    as->m_labelStack["forLoopDone"].pop();

}

void NodeForLoop::SmallLoop(Assembler *as)
{
    QString loopDone = as->NewLabel("forLoopDone");
  //  Compare(as);
  //  as->Asm("beq "+loopDone);

    m_block->Build(as);
    as->m_stack["for"].pop();
    IncreaseCounter(as);
    Compare(as);
    as->Asm("bne "+as->getLabel("for"));

//    as->Asm("jmp " + as->getLabel("for"));

    as->Label(loopDone);

    as->m_labelStack["for"].pop();
    as->m_labelStack["forLoopDone"].pop();

}




QString NodeForLoop::Build(Assembler *as) {
    Node::Build(as);

    //QString m_currentVar = ((NodeAssign*)m_a)->m_
    QString var = m_a->Build(as);
    if (m_loopCounter==1) as->Asm("tax");
    if (m_loopCounter==2) as->Asm("tay");
    //QString to = m_b->Build(as);
    QString to = "";
    if (dynamic_cast<const NodeNumber*>(m_b) != nullptr)
        to = QString::number(((NodeNumber*)m_b)->m_val);
    if (dynamic_cast<const NodeVar*>(m_b) != nullptr)
        to = ((NodeVar*)m_b)->value;
  //  if (m_b->m_op.m_type==TokenType::INTEGER ||m_b->m_op.m_type==TokenType::INTEGER_CONST )
  //      to = "#" + to;
//    as->StartForLoop(var, to);

    as->m_stack["for"].push(var);
//    as->m_labelStack["for"].push();
    as->Label(as->NewLabel("for"));


    bool isSmall = verifyBlockBranchSize(as, m_block);

    if (m_forcePage == 1)
        isSmall = false;

    if (m_forcePage == 2)
        isSmall = true;

    if (isSmall)
        SmallLoop(as);
    else
        LargeLoop(as);

//    if (Syntax::s.isNumeric(endVal))
//        endVal = "#" + endVal;


 //   qDebug() << label2;
  //  qDebug() << labelDone;



 //   qDebug() << "loop: " << m_stack["for"].current();
    return "";
}
