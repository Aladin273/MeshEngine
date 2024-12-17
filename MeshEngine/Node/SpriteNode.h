#pragma once

#include "Node.h"

#include "MeshEngine/Base/Sprite.h"

class SpriteNode : public Node
{
public:
    SpriteNode();
    virtual ~SpriteNode();

public:
    virtual void bind() override
    {
        bindPropertyEx(Property::Base, "m_sprite", *m_sprite.get());
        
        super::bind();
    }

public:
    virtual const BoundingBox& getBoundingBox() const override;

public:
    virtual void start() override;
    virtual void end() override;

    virtual void update(float deltaTime) override;
    virtual void render(RenderSystem* renderSystem) override;

public:
    void attachSprite(std::unique_ptr<Sprite> sprite);
    Sprite* getSprite() const;

protected:
    uint32_t m_renderId = 0;
    std::unique_ptr<Sprite> m_sprite;
};
