#include "Base.h"

#include <iostream>

Base::Base()
{
    static uint32_t id = 0;
    m_id = id++;
}

Base::~Base()
{

}

void Base::bind()
{

}

void Base::unbind()
{
    m_properties.clear();
}

void Base::propertyChanged(const Property& property)
{

}

const std::vector<Property>& Base::getProperties() const
{
    return m_properties;
}

uint32_t Base::getId() const
{
    return m_id;
}

void Base::setName(const std::string& name)
{
    m_name = name;
}

const std::string& Base::getName() const
{
    return m_name;
}

std::string Base::formatString(const std::string& camelCase)
{
    std::string title;

    size_t startIdx = (camelCase.rfind("m_", 0) == 0) ? 2 : 0;

    for (size_t i = startIdx; i < camelCase.length(); ++i)
    {
        char ch = camelCase[i];
        if (std::isupper(ch) && !title.empty())
        {
            title += ' ';
        }
        title += ch;
    }

    if (!title.empty())
    {
        title[0] = std::toupper(title[0]);
    }

    return title;
}
