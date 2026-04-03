#include "BaseComponent.h"

#include "Object/BaseObject.h"
#include "Engine/Renderer.h"

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
    Renderer::Ins()->AddImGuiCallback(this);
}

Component::IImGuiUpdatable::~IImGuiUpdatable()
{
    Renderer::Ins()->RemoveImGuiCallback(this);
}
