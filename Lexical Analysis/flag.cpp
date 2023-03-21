class Flag {
public:
    bool varDec;
    bool varColon;
    bool varType;
    bool funcDec;
    bool funcColon;
    bool funcType;

    Flag() {
        this->varType = false;
        this->varDec = false;
        this->varColon = false;
        this->funcType = false;
        this->funcDec = false;
        this->funcColon = false;
    }

    void clear() {
        this->varType = false;
        this->varDec = false;
        this->varColon = false;
        this->funcType = false;
        this->funcDec = false;
        this->funcColon = false;
    }

    void funcWasColoned() {
        this->funcDec = false;
        this->funcColon = true;
    }

    void funcWasTyped() {
        this->funcColon = false;
        this->funcType = true;
    } 

    void varWasColoned() {
        this->varDec = false;
        this->varColon = true;
    }

    void varWasTyped() {
        this->varColon = false;
        this->varType = true;
    }
};