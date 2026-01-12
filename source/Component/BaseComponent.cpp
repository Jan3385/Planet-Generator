#include "BaseComponent.h"

#include "Object/BaseObject.h"

void Component::BaseComponent::CheckDependencies() const
{
    for (const auto& dependency : this->GetDependencies()) {
        if (!owner->HasComponentType(dependency)) {
            Debug::LogError("Component dependency not met: " + std::string(dependency.name()));
        }
    }
}