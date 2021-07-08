#pragma once

#include "../forward.hh"

class QueryTree
{
private:
    GLuint object;
    int state;

    bool value;
    bool temp;

    std::vector<SharedQueryTree> children;

public:
    QueryTree() : object(0), state(STATE_NO_OBJECT), value(false), temp(false), children() {}
    virtual ~QueryTree();

    SharedQueryTree prepareChild(uint32_t child);

    void startQuery();
    void endQuery();

    void prepare();
    void update();

    bool getResult();

    bool getValue() { return value; }
    void setValue(bool v) { value = v; }

    void cancel();
    void cancelAll();

    void shift(int child);
    void shift(QueryTree* other);

    void applyTemp();

    size_t getChildCount();
    QueryTree* getChild(int child);

    void makeVisible(int child);
};
