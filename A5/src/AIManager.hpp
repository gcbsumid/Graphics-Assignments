#ifndef AIMANAGER_HPP
#define AIMANAGER_HPP

#include <memory>
#include <vector>

#include "Component.hpp"

class AIManager {
public:
    AIManager();
    virtual ~AIManager();

    void GenerateAIComp(Entity*);

    void UpdateAll();
    void Action(double);
private:
    std::vector<std::weak_ptr<AIComp>> mComponents;    
};

#endif