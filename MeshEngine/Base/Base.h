#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <variant>
#include <functional>

#include <glm/glm.hpp>

#define super __super

#define bindProperty(object) bindPropertyInternal(#object, &object)
#define bindPropertyEx(type, name, object) bindPropertyInternal(type, name, &object)

#define bindSeparator() bindSeparatorInternal()

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
        MatrixEx,
        Quat,
        Color3,
        Color4,
        Separator,
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

    virtual void propertyChanged(const Property& property);
    const std::vector<Property>& getProperties() const;

public:
    uint32_t getId() const;

    void setName(const std::string& name);
    const std::string& getName() const;

public:
    bool operator==(const Base& other)
    {
        return getId() == other.getId();
    }

protected:
    template <typename T>
    void bindPropertyInternal(std::string name, T* object)
    {
        Property property;
        property.name = Base::formatString(name);
        property.object = (void*)object;
        property.type = Base::typeOf<T>();
        m_properties.push_back(property);
    }

    template <typename T>
    void bindPropertyInternal(Property::Type type, std::string name, T* object)
    {
        Property property;
        property.name = Base::formatString(name);
        property.object = (void*)object;
        property.type = type;
        m_properties.push_back(property);
    }

    void bindSeparatorInternal()
    {
        Property property;
        property.name = "";
        property.object = nullptr;
        property.type = Property::Separator;
        m_properties.push_back(property);
    }

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

    std::string formatString(const std::string& camelCase);

protected:
    std::string m_name = "Base";

private:
    uint32_t m_id = 0;
    std::vector<Property> m_properties;
};