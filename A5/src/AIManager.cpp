#include "AIManager.hpp"
#include <cmath>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SoundManager.h"


using namespace std;

AIManager::AIManager() 
    : mState(Waiting)
    , mIsBlurred(false)
{
    mEerieSound = SM.LoadSound("eerie.wav");
    mIndicatorSound = SM.LoadSound("indicator.wav");
    mStaticSound = SM.LoadSound("static.wav");
    mForrestSound = SM.LoadMusic("resources/nighttime.wav");
}

AIManager::~AIManager() {

}

void AIManager::StartPlayingSounds() {
    SM.PlayMusic(mForrestSound);
}

void AIManager::GenerateAIComp(shared_ptr<Entity> ent) {
    ent->AddComponent(new AIComp(ent));
    weak_ptr<AIComp> aicomp = static_pointer_cast<AIComp>(ent->GetComponent(Component::COMPTYPE_AI));
    mComponents.push_back(aicomp);
}

void AIManager::AttachSlender(shared_ptr<Entity> slender) {
    mSlender = slender;
    mSlenderOriginalPos = mSlender->GetPos();
}

void AIManager::AttachCamera(shared_ptr<Entity> camera) {
    mCamera = static_pointer_cast<Camera>(camera);
}

void AIManager::UpdateAll() {

}

void AIManager::Action(double tick) {
    mTimer += tick;

    switch (mState) {
        case Waiting: {
            if (mTimer > 15.0) {
                mTimer -= 15.0;
                mState = Searching;
            }
            break;
        }
        case Searching: {
            // Calculate location
            SM.PlaySound(mIndicatorSound);
            glm::vec3 camera_pos = mCamera->GetPos();
            glm::vec3 camera_forward = mCamera->Forward();
            glm::vec3 slender_pos = mSlender->GetPos();
            camera_forward = glm::normalize(glm::vec3(camera_forward.x, 0.0, camera_forward.z));
            glm::vec3 new_loc = camera_pos + (glm::normalize(camera_forward) * -10.0f);
            mSlender->SetPosition(glm::vec3(new_loc.x, slender_pos.y, new_loc.z));

            // Calculate angle
            RotateSlender(true);
            mState = Stalking;
            break;
        } 
        case Stalking: {
            // SM.PlaySound(mIndicatorSound);
            glm::vec3 slender_dir = glm::normalize(mSlender->GetPos() - mCamera->GetPos());
            glm::vec3 camera_fwd = glm::normalize(mCamera->Forward());

            if (glm::dot(slender_dir, camera_fwd) > (cos(0.25 * M_PI))) {
                cout << "status moving to Attacking. \ndot is: " << glm::dot(slender_dir, camera_fwd) << endl;
                mState = Attacking;
            } else if (mTimer > 2.0) {
                mTimer -= 2.0;
                mState = Searching;
            }
            break;
        } 
        case Attacking: {
            SM.PlaySound(mStaticSound);

            if(mTimer > 0.6 && mTimer < 0.9) {
                mIsBlurred = true;
            } else {
                mIsBlurred = false;
            }

            if (mTimer > 1.0) {
                mTimer -= 1.0;
                RotateSlender(false);
                random_device rd;
                mt19937 gen(rd());
                uniform_real_distribution<> dist(0.2, 1);
                glm::vec3 slender_dir = mCamera->GetPos() - mSlender->GetPos();
                slender_dir += glm::vec3(dist(gen), 0.0, dist(gen));


                float rand_val = dist(gen);
                glm::vec3 new_loc = mSlender->GetPos() + (slender_dir * rand_val);

                mSlender->SetPosition(glm::vec3(new_loc.x, mSlender->GetPos().y, new_loc.z));

                // temp
                slender_dir = -slender_dir;
                glm::vec3 camera_fwd = glm::normalize(mCamera->Forward());
                
                if (glm::dot(slender_dir, camera_fwd) <= cos(0.25 * M_PI)) {
                    mState = Waiting;

                    mSlender->SetPosition(mSlenderOriginalPos);
                    SM.StopSound(mStaticSound);
                }
            } 
            break;
        }
    }
}

void AIManager::RotateSlender(bool isSameDir) {
    glm::vec3 camera_forward = glm::vec3(-mCamera->Forward().x, mCamera->Forward().y, mCamera->Forward().z);
    camera_forward = glm::normalize(glm::vec3(camera_forward.x, 0.0, camera_forward.z));

    glm::vec4 slender_forward = glm::inverse(mSlender->GetRotate()) * glm::vec4(0,0,-1,1);
    glm::vec3 slender_forward_v3 = glm::normalize(glm::vec3(slender_forward.x, 0.0, slender_forward.z));

    float diff_angle = acos(glm::dot(camera_forward, slender_forward_v3)) * 180.0 / M_PI;

    if (diff_angle > 360.0) diff_angle -= 360.0;
    if (diff_angle < 0.0) diff_angle += 360.0;

    glm::vec4 temp_forward = glm::transpose(glm::rotate(glm::mat4(), diff_angle, glm::vec3(0,1,0))) *
            glm::vec4(slender_forward_v3.x, slender_forward_v3.y, slender_forward_v3.z,1);

    glm::vec3 new_slender_forward = glm::vec3(temp_forward);
    float small_angle = glm::dot(new_slender_forward, camera_forward);

    float final_angle_to_rotate = 0.0;

    if (small_angle > 0.999) {
        final_angle_to_rotate = diff_angle;
    } else { 
        final_angle_to_rotate = -diff_angle;
    }

    if (!isSameDir) {
        final_angle_to_rotate += 180.0;
    }
    mSlender->Rotate(glm::vec3(0,1,0), final_angle_to_rotate);
}

bool AIManager::IsBlurred() {
    return mIsBlurred;
}