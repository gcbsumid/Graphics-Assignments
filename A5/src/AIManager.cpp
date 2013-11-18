#include "AIManager.hpp"

using namespace std;

AIManager::AIManager() {

}

AIManager::~AIManager() {

}

void AIManager::GenerateAIComp(shared_ptr<Entity> ent) {
    ent->AddComponent(new AIComp(ent));
    weak_ptr<AIComp> aicomp = static_pointer_cast<AIComp>(ent->GetComponent(Component::COMPTYPE_AI));
    mComponents.push_back(aicomp);
}

void AIManager::UpdateAll() {

}

void AIManager::Action(double tick) {

}