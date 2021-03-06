//
//  lexical_analyzer.cpp
//  PJP-semestral-work
//
//  Created by Michal Štembera on 26/05/15.
//  Copyright (c) 2015 Michal Štembera. All rights reserved.
//

#include "lexical_analyzer.h"
#include "error.h"
#include <climits>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#pragma mark - LexicalAnalyzer
LexicalAnalyzer::LexicalAnalyzer(std::istream & is)
:input_stream(is){
    readInput();
}

LexicalAnalyzer::~LexicalAnalyzer(){
    
}

void LexicalAnalyzer::readInput(void) {
    character = input_stream.get();
    if ((character>='A' && character<='Z') || (character>='a' && character<='z'))
        characterType = TYPE_LETTER;
    else if (character>='0' && character<='9')
        characterType = TYPE_NUMBER;
    else if (character == EOF)
        characterType = TYPE_END;
    else if (character <= ' ')
        characterType = TYPE_WHITE_SPACE;
    else
        characterType = NO_TYPE;
}

LexicalSymbol LexicalAnalyzer::readLexem(void){
    outputSymbol.type = ERR;
    outputSymbol.number = 0;
    outputSymbol.ident[0] = 0;
    actualState = STATE_START;
    
    while (actualState != STATE_FINISHED) {
        switch (actualState) {
            case STATE_START:
                applyStateStart();
                break;
            case STATE_DECIMAL:
                applyStateDecimalNumber();
                break;
            case STATE_OCTAL:
                applyStateOctalNumber();
                break;
            case STATE_HEXADECIMAL:
                applyStateHexadecimalNumber();
                break;
            case STATE_IDEN_KW:
                applyStateIdenKeyword();
                break;
            case STATE_IDEN:
                applyStateIden();
                break;
            case STATE_LT:
                applyStateLT();
                break;
            case STATE_GT:
                applyStateGT();
                break;
            case STATE_DOT:
                applyStateDot();
                break;
            case STATE_COLON:
                applyStateColon();
                break;
            case STATE_COMMENT:
                applyStateComment();
                break;
            default:
                outputSymbol.type = ERR;
                error("Unrecognized automat state.");
                break;
        }
    }
    
    return outputSymbol;
}

void LexicalAnalyzer::checkIdentKeyword(){
    LexSymbolType newType = KeywordLexSymbolMap[outputSymbol.ident];
    if(newType != UNKNOWN){
        outputSymbol.type = newType;
    }
    
}

#pragma mark - LexicalAnalyzer Apply State
void LexicalAnalyzer::applyStateStart(void){
    switch (character) {
        case '+':
            outputSymbol.type = PLUS;
            actualState = STATE_FINISHED;
            readInput();
            return;
        case '-':
            outputSymbol.type = MINUS;
            actualState = STATE_FINISHED;
            readInput();
            return;
        case '*':
            outputSymbol.type = TIMES;
            actualState = STATE_FINISHED;
            readInput();
            return;
        case '/':
            outputSymbol.type = DIVIDE;
            actualState = STATE_FINISHED;
            readInput();
            return;
        case ';':
            outputSymbol.type = SEMICOLON;
            actualState = STATE_FINISHED;
            readInput();
            return;
        case '=':
            outputSymbol.type = EQ;
            actualState = STATE_FINISHED;
            readInput();
            return;
        case ',':
            outputSymbol.type = COMMA;
            actualState = STATE_FINISHED;
            readInput();
            return;
        case '(':
            actualState = STATE_FINISHED;
            outputSymbol.type = LPAR;
            readInput();
            return;
        case ')':
            actualState = STATE_FINISHED;
            outputSymbol.type = RPAR;
            readInput();
            return;
        case '[':
            actualState = STATE_FINISHED;
            outputSymbol.type = LBRACK;
            readInput();
            return;
        case ']':
            actualState = STATE_FINISHED;
            outputSymbol.type = RBRACK;
            readInput();
            return;
            
        case '<':
            actualState = STATE_LT;
            outputSymbol.type = LT;
            readInput();
            return;
        case '>':
            actualState = STATE_GT;
            outputSymbol.type = GT;
            readInput();
            return;
        case '.':
            actualState = STATE_DOT;
            outputSymbol.type = DOT;
            readInput();
            return;
        case ':':
            actualState = STATE_COLON;
            outputSymbol.type = COLON;
            readInput();
            return;
        case '{':
            actualState = STATE_COMMENT;
            readInput();
            return;
        case '&':
            outputSymbol.type = NUMBER;
            actualState = STATE_OCTAL;
            readInput();
            return;
        case '$':
            outputSymbol.type = NUMBER;
            actualState = STATE_HEXADECIMAL;
            readInput();
            return;
        default:
            break;
    }
    switch (characterType) {
        case TYPE_WHITE_SPACE:
            actualState = STATE_START;
            readInput();
            break;
        case TYPE_LETTER:
            actualState = STATE_IDEN_KW;
            outputSymbol.appendCharToIdent(character);
            readInput();
            break;
        case TYPE_NUMBER:
            actualState = STATE_DECIMAL;
            outputSymbol.type = NUMBER;
            outputSymbol.appendCharToDecimal(character);
            readInput();
            break;
        case TYPE_END:
            actualState = STATE_FINISHED;
            outputSymbol.type = EOI;
            break;
        default:
            error("Unknown characterType.");
            break;
    }
}

void LexicalAnalyzer::applyStateDecimalNumber(){
    switch (characterType) {
        case TYPE_NUMBER:
            outputSymbol.appendCharToDecimal(character);
            readInput();
            break;
        default:
            actualState = STATE_FINISHED;
            break;
    }

}

void LexicalAnalyzer::applyStateOctalNumber(){
    switch (characterType) {
        case TYPE_NUMBER:
            outputSymbol.appendCharToOctal(character);
            readInput();
            break;
        default:
            actualState = STATE_FINISHED;
            break;
    }
}

void LexicalAnalyzer::applyStateHexadecimalNumber(){
    switch (characterType) {
        case TYPE_NUMBER:
        case TYPE_LETTER:
            outputSymbol.appendCharToHexa(character);
            readInput();
            break;
        default:
            actualState = STATE_FINISHED;
            break;
    }
}

void LexicalAnalyzer::applyStateColon(void){
    switch (character) {
        case '=':
            actualState = STATE_FINISHED;
            outputSymbol.type = ASSIGN;
            readInput();
            break;
        default:
            actualState = STATE_FINISHED;
            outputSymbol.type = COLON;
            break;
    }
}

void LexicalAnalyzer::applyStateLT(void){
    switch (character) {
        case '=':
            actualState = STATE_FINISHED;
            outputSymbol.type = LTE;
            readInput();
            break;
        case '>':
            actualState = STATE_FINISHED;
            outputSymbol.type = NEQ;
            readInput();
            break;
        default:
            actualState = STATE_FINISHED;
            outputSymbol.type = LT;
            break;
    }
}

void LexicalAnalyzer::applyStateGT(void){
    switch (character) {
        case '=':
            actualState = STATE_FINISHED;
            outputSymbol.type = GTE;
            readInput();
            break;
        default:
            actualState = STATE_FINISHED;
            outputSymbol.type = GT;
            break;
    }
}

void LexicalAnalyzer::applyStateDot(void){
    switch (character) {
        case '.':
            actualState = STATE_FINISHED;
            outputSymbol.type = DDOT;
            readInput();
            break;
        default:
            actualState = STATE_FINISHED;
            outputSymbol.type = DOT;
            break;
    }
}

void LexicalAnalyzer::applyStateIdenKeyword(void){
    switch (characterType) {
        case TYPE_LETTER:
            actualState = STATE_IDEN_KW;
            outputSymbol.type = IDENT;
            outputSymbol.appendCharToIdent(character);
            readInput();
            break;
        case TYPE_NUMBER:
            actualState = STATE_IDEN;
            outputSymbol.type = IDENT;
            outputSymbol.appendCharToIdent(character);
            readInput();
            break;
        default:
            actualState = STATE_FINISHED;
            outputSymbol.type = IDENT;
            checkIdentKeyword();
            break;
    }
}

void LexicalAnalyzer::applyStateIden(void){
    switch (characterType) {
        case TYPE_LETTER:
        case TYPE_NUMBER:
            actualState = STATE_IDEN;
            outputSymbol.type = IDENT;
            readInput();
            break;
        default:
            actualState = STATE_FINISHED;
            outputSymbol.type = IDENT;
            break;
    }
}

void LexicalAnalyzer::applyStateComment(){
    switch (character) {
        case '}':
            actualState = STATE_START;
            readInput();
            break;
        default:
            readInput();
            break;
    }
}
