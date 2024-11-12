#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include <cctype>

#include <glm/glm.hpp>

#define super __super

#define bindProperty(object) bindPropertyInternal(#object, &object)
#define bindPropertyEx(type, name, object) bindPropertyInternal(type, name, &object)

class Property
{
public:
    enum Type
    {
        Int,
        Float,
        Double,
        Bool,
        String,
        Vector2,
        Vector3,
        Vector4,
        Matrix2,
        Matrix3,
        Matrix4,
        Quat,
        Color,
        Base,
    };

    Type type;
    std::string name;
    void* object;
};

class Base
{
public:
    Base();
    virtual ~Base();

public:
    virtual void bind();
    virtual void unbind();

    uint32_t getID() const;

    void setName(const std::string& name);
    const std::string& getName() const;
    
    const std::vector<Property>& getProperties() const;

protected:
    template <typename T>
    Property::Type typeOf()
    {
        if constexpr (std::is_same_v<T, uint32_t>) return Property::Type::Int;
        else if constexpr (std::is_same_v<T, float>) return Property::Type::Float;
        else if constexpr (std::is_same_v<T, double>) return Property::Type::Double;
        else if constexpr (std::is_same_v<T, bool>) return Property::Type::Bool;
        else if constexpr (std::is_same_v<T, std::string>) return Property::Type::String;
        else if constexpr (std::is_same_v<T, glm::vec2>) return Property::Type::Vector2;
        else if constexpr (std::is_same_v<T, glm::vec3>) return Property::Type::Vector3;
        else if constexpr (std::is_same_v<T, glm::vec4>) return Property::Type::Vector4;
        else if constexpr (std::is_same_v<T, glm::mat2>) return Property::Type::Matrix2;
        else if constexpr (std::is_same_v<T, glm::mat3>) return Property::Type::Matrix3;
        else if constexpr (std::is_same_v<T, glm::mat4>) return Property::Type::Matrix4;
        else if constexpr (std::is_same_v<T, glm::quat>) return Property::Type::Quat;
        else return Property::Type::Base;
    }

    template <typename T>
    void bindPropertyInternal(std::string name, T* object)
    {
        Property prop;
        prop.name = formatString(name);
        prop.object = (void*)object;
        prop.type = Base::typeOf<T>();
        m_properties.push_back(prop);
    }

    template <typename T>
    void bindPropertyInternal(Property::Type type, std::string name, T* object)
    {
        Property prop;
        prop.name = formatString(name);
        prop.object = (void*)object;
        prop.type = type;
        m_properties.push_back(prop);
    }

    std::string formatString(const std::string& camelCase) 
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

protected:
    std::string m_name = "Base";

private:
    uint32_t m_id = 0;
    std::vector<Property> m_properties;
};