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

const std::vector<Property>& Base::getProperties() const
{
    return m_properties;
}
