enum TokenCode {
    op,             //0
    keyword,        //1
    type,           //2
    variable,       //3
    function,       //4
    declarator,     //5

    integerLiteral, //6
    booleanLiteral, //7
    floatingLiteral,//8
    stringLiteral,  //9

    curlyBracket,   //10
    roundBracket,   //11
    squareBracket,  //12

    ofType,         //13
    newLine,        //14

    lexicalError,   //15
};