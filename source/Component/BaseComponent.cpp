#include "BaseComponent.h"

#include "Object/BaseObject.h"
#include "Engine/Renderer.h"
#include "Engine/Engine.h"

void Component::BaseComponent::CheckDependencies() const
{
    for (const auto& dependency : this->GetDependencies()) {
        if (!owner->HasComponentType(dependency)) {
            Debug::LogError("Component dependency not met: " + std::string(dependency.name()));
        }
    }
}

Component::IImGuiUpdatable::IImGuiUpdatable()
{
    GameEngine::renderer->AddImGuiCallback(this);
}

Component::IImGuiUpdatable::~IImGuiUpdatable()
{
    GameEngine::renderer->RemoveImGuiCallback(this);
}
