/*
 * Copyright 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common.hpp"
#include "scene.hpp"
#include "scene_manager.hpp"

static SceneManager _sceneManager;

SceneManager::SceneManager() {
    mCurScene = NULL;

    // start with non-bogus (though not accurate) values
    mScreenWidth = 320;
    mScreenHeight = 240;

    mSceneToInstall = NULL;

    mHasGraphics = false;
}

void SceneManager::RequestNewScene(Scene *newScene) {
    ALOGI("SceneManager: requesting new scene %p", newScene);
    mSceneToInstall = newScene;
}

void SceneManager::InstallScene(Scene *newScene) {
    ALOGE("SceneManager: installing scene %p.", newScene);

    // kill graphics, if we have them.
    bool hadGraphics = mHasGraphics;
    if (mHasGraphics) {
        KillGraphics();
    }

    // If we have an existing scene, uninstall it.
    if (mCurScene) {
        mCurScene->OnUninstall();
        delete mCurScene;
        mCurScene = NULL;
    }

    // install the new scene
    mCurScene = newScene;
    if (mCurScene) {
        // Specify the controls to use to the Input SDK
        mCurScene->SetInputSdkContext();
        mCurScene->OnInstall();
    }

    // if we had graphics before, start them again.
    if (hadGraphics) {
        StartGraphics();
    }
}

Scene *SceneManager::GetScene() {
    return mCurScene;
}

void SceneManager::DoFrame() {
    if (mSceneToInstall) {
        InstallScene(mSceneToInstall);
        mSceneToInstall = NULL;
    }

    if (mHasGraphics && mCurScene) {
        mCurScene->DoFrame();
    }
}

void SceneManager::KillGraphics() {
    if (mHasGraphics) {
        ALOGI("SceneManager: killing graphics.");
        mHasGraphics = false;
        if (mCurScene) {
            mCurScene->OnKillGraphics();
        }
    }
}

void SceneManager::StartGraphics() {
    if (!mHasGraphics) {
        ALOGI("SceneManager: starting graphics.");
        mHasGraphics = true;
        if (mCurScene) {
            ALOGI("SceneManager: calling mCurScene->OnStartGraphics.");
            mCurScene->OnStartGraphics();
        }
    }

//    // DEBUG
//    android_app* pApp = NativeEngine::GetInstance()->GetAndroidApp();
//    int width = ANativeWindow_getWidth(pApp->window);
//    int height = ANativeWindow_getHeight(pApp->window);
//    ALOGI("SceneManager ANativeWindow size %d, %d", width, height);
}


void SceneManager::SetScreenSize(int width, int height) {
    ALOGI("SceneManager SetScreenSize %d, %d", width, height);
    if (mScreenWidth != width || mScreenHeight != height) {
        mScreenWidth = width;
        mScreenHeight = height;

        if (mCurScene && mHasGraphics) {
            mCurScene->OnScreenResized(width, height);
        }
    }
}

SceneManager *SceneManager::GetInstance() {
    return &_sceneManager;
}

void SceneManager::OnPointerDown(int pointerId, const struct PointerCoords *coords) {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnPointerDown(pointerId, coords);
    }

    // DEBUG
    android_app* pApp = NativeEngine::GetInstance()->GetAndroidApp();
    int width = ANativeWindow_getWidth(pApp->window);
    int height = ANativeWindow_getHeight(pApp->window);
    ALOGI("DBGSIZE SceneManager ANativeWindow size %d, %d", width, height);
}

void SceneManager::OnPointerUp(int pointerId, const struct PointerCoords *coords) {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnPointerUp(pointerId, coords);
    }
}

void SceneManager::OnPointerMove(int pointerId, const struct PointerCoords *coords) {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnPointerMove(pointerId, coords);
    }
}

bool SceneManager::OnBackKeyPressed() {
    if (mHasGraphics && mCurScene) {
        return mCurScene->OnBackKeyPressed();
    }
    return false;
}

void SceneManager::OnKeyDown(int ourKeyCode) {
    if ((ourKeyCode >= 0 && ourKeyCode < OURKEY_COUNT &&
            mHasGraphics && mCurScene)) {
        mCurScene->OnKeyDown(ourKeyCode);

        // if our "escape" key (normally corresponding to joystick button B or Y)
        // was pressed, handle it as a back key
        if (ourKeyCode == OURKEY_ESCAPE) {
            mCurScene->OnBackKeyPressed();
        }
    }
}

void SceneManager::OnKeyUp(int ourKeyCode) {
    if ((ourKeyCode >= 0 && ourKeyCode < OURKEY_COUNT &&
            mHasGraphics && mCurScene)) {
        mCurScene->OnKeyUp(ourKeyCode);
    }
}

void SceneManager::UpdateJoy(float joyX, float joyY) {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnJoy(joyX, joyY);
    }
}

void SceneManager::OnPause() {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnPause();
    }
}

void SceneManager::OnResume() {
    if (mCurScene) {
        mCurScene->OnResume();
    }
}

void SceneManager::OnTextInput() {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnTextInput();
    }
}
