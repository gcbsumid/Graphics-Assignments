#ifndef AIMANAGER_HPP
#define AIMANAGER_HPP

#include <memory>
#include <vector>

#include "Entity.hpp"
#include "Component.hpp"

class AIManager {
public:
    AIManager();
    virtual ~AIManager();

    void GenerateAIComp(std::shared_ptr<Entity>);

    void UpdateAll();
    void Action(double);
private:
    std::vector<std::weak_ptr<AIComp>> mComponents;    
};

#endif