#include "QueryTree.hh"

QueryTree::~QueryTree()
{
    glDeleteQueries(1, &object);
}

SharedQueryTree QueryTree::prepareChild(uint32_t child)
{
    if (child >= children.size())
    {
        children.resize(child + 1);
    }

    if (children[child] == nullptr)
    {
        children[child] = std::make_shared<QueryTree>();
    }

    children[child]->prepare();
    return children[child];
}

void QueryTree::startQuery()
{
    if (state == STATE_OBJECT_IDLE)
    {
        glBeginQuery(GL_ANY_SAMPLES_PASSED, object);
        state = STATE_OBJECT_BUSY;
    }
}

void QueryTree::endQuery()
{
    if (state == STATE_OBJECT_BUSY)
    {
        glEndQuery(GL_ANY_SAMPLES_PASSED);
        state = STATE_OBJECT_PENDING;
    }
}

void QueryTree::prepare()
{
    if (state == STATE_NO_OBJECT)
    {
        glGenQueries(1, &object);
        state = STATE_OBJECT_IDLE;
    }
}

void QueryTree::update()
{
    if (state == STATE_OBJECT_PENDING)
    {
        GLuint result = GL_FALSE;
        glGetQueryObjectuiv(object, GL_QUERY_RESULT_AVAILABLE, &result);

        if (result)
        {
            glGetQueryObjectuiv(object, GL_QUERY_RESULT, &result);

            value = (result > 0);
            state = STATE_OBJECT_IDLE;
        }
    }
}

bool QueryTree::getResult()
{
    update();
    return value;
}

void QueryTree::cancel()
{
    if (state == STATE_OBJECT_PENDING)
    {
        glDeleteQueries(1, &object);
        glGenQueries(1, &object);
        state = STATE_OBJECT_IDLE;
    }
}

void QueryTree::cancelAll()
{
    cancel();
    for (size_t i = 0; i < children.size(); i++)
    {
        if (children[i] != nullptr)
        {
            children[i]->cancelAll();
        }
    }
}

void QueryTree::shift(int child)
{
    shift(getChild(child));
    applyTemp();
}

void QueryTree::shift(QueryTree* other)
{
    if (other == nullptr)
    {
        temp = false;
    }
    else
    {
        temp = other->getValue();

        if (other->getChildCount() > children.size())
        {
            children.resize(other->getChildCount());
        }

        for (size_t i = 0; i < children.size(); i++)
        {
            if (children[i] == nullptr)
            {
                children[i] = std::make_shared<QueryTree>();
            }
            children[i]->shift(other->getChild(i));
        }
    }
}

void QueryTree::applyTemp() {
    value = temp;
    for (size_t i = 0; i < children.size(); i++)
    {
        if (children[i] != nullptr)
        {
            children[i]->applyTemp();
        }
    }
}

size_t QueryTree::getChildCount() {
    return children.size();
}

QueryTree* QueryTree::getChild(int child)
{
    if (child >= 0 && ((size_t)child) < children.size() && children[child] != nullptr)
    {
        return children[child].get();
    }
    else
    {
        return nullptr;
    }
}

void QueryTree::makeVisible(int child)
{
    if (child >= 0)
    {
        if (((size_t)child) >= children.size())
        {
            children.resize(child + 1);
        }

        if (children[child] == nullptr)
        {
            children[child] = std::make_shared<QueryTree>();
        }

        children[child]->setValue(true);
    }
}
