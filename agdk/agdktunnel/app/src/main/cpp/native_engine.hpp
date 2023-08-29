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

#ifndef agdktunnel_native_engine_hpp
#define agdktunnel_native_engine_hpp

#include "display_manager.h"
#include "common.hpp"
#include "game_asset_manager.hpp"
#include "gfx_manager.hpp"
#include "memory_consumer.hpp"
#include "texture_manager.hpp"
#include "tuning_manager.hpp"
#include "vibration_helper.hpp"
#include "data_loader_machine.hpp"
#include "java/lang/string.h"

struct NativeEngineSavedState {
    bool mHasFocus;
};

class NativeEngine {
public:
    // create an engine
    NativeEngine(struct android_app *app);

    ~NativeEngine();

    // runs application until it dies
    void GameLoop();

    // returns the JNI environment
    JNIEnv *GetJniEnv();

    // returns the Android app object
    android_app *GetAndroidApp();

    // returns the asset manager instance
    GameAssetManager *GetGameAssetManager() { return mGameAssetManager; }

    // returns the graphics resource manager instance
    GfxManager *GetGfxManager() { return mGfxManager; }

    // returns the texture manager instance
    TextureManager *GetTextureManager() { return mTextureManager; }

    // returns the tuning manager instance
    TuningManager *GetTuningManager() { return mTuningManager; }

    // returns the memory consumer instance
    MemoryConsumer *GetMemoryConsumer() { return mMemoryConsumer; }

    // returns the vibration helper instance
    VibrationHelper *GetVibrationHelper() { return mVibrationHelper; }

    // returns the (singleton) instance
    static NativeEngine *GetInstance();

    // This is the env for the app thread. It's different to the main thread.
    JNIEnv *GetAppJniEnv();

    // Returns if cloud save is enabled
    bool IsCloudSaveEnabled() { return mCloudSaveEnabled; }

    // Load data from cloud if it is enabled, or from local data otherwise
    DataLoaderStateMachine *BeginSavedGameLoad();

    // Saves data to local storage and to cloud if it is enabled
    bool SaveProgress(int level, bool forceSave = false);

    DataLoaderStateMachine *GetDataStateMachine() { return mDataStateMachine; }

    void SetInputSdkContext(int context);

    enum SystemService {
      eVibrator,
      eVibrationManager
    };

private:
    // variables to track Android lifecycle:
    // variables to track Android lifecycle:
    bool mHasFocus, mHasStarted, mDisplayInitialized;

    // has active swapchain
    bool mHasSwapchain;

    // True if we are to exit main loop and shutdown
    bool mQuitting;

    // are our OpenGL objects (textures, etc) currently loaded?
    bool mHasGfxObjects;

    // android API version (0 if not yet queried)
    int mApiVersion;

    // Screen density
    int mScreenDensity;

    // known surface size
    int mSurfWidth, mSurfHeight;

    // Most recently connected game controller index
    int32_t mGameControllerIndex;

    // known active motion axis ids (bitfield)
    uint64_t mActiveAxisIds;

    // android_app structure
    struct android_app *mApp;

    // additional saved state
    struct NativeEngineSavedState mState;

    // JNI environment
    JNIEnv *mJniEnv;

    // JNI env for the app native glue thread
    JNIEnv *mAppJniEnv;

    // Game asset manager instance
    GameAssetManager *mGameAssetManager;

    // Texture manager instance
    TextureManager *mTextureManager;

    // Tuning manager instance
    TuningManager *mTuningManager;

    // Memory consumer instance
    MemoryConsumer *mMemoryConsumer;

    // Gfx resource manager instance
    GfxManager *mGfxManager;

    // Vibration helper instance
    VibrationHelper *mVibrationHelper;

    base_game_framework::DisplayManager::SwapchainFrameHandle mSwapchainFrameHandle;

    base_game_framework::DisplayManager::SwapchainHandle mSwapchainHandle;

    base_game_framework::DisplayManager::DisplayFormat mDisplayFormat;

    int mSwapchainImageCount;

    // Are we using Vulkan?
    bool mIsVulkan;

    // is this the first frame we're drawing?
    bool mIsFirstFrame;

    // is cloud save enabled
    bool mCloudSaveEnabled;

    // state machine instance to query the status of the current load of data
    DataLoaderStateMachine *mDataStateMachine;

    // Initial display and graphics API setup
    bool AttemptDisplayInitialization();

    bool CreateSwapchain();

    void InitializeGfxManager();

    // BaseGameFramework callbacks

    // Display Manager
    void SwapchainChanged(const base_game_framework::DisplayManager::SwapchainChangeMessage reason,
                          void* user_data);

    void DisplayResolutionChanged(const base_game_framework::DisplayManager::DisplayChangeInfo
                                  &display_change_info, void* user_data);

    bool InitGfxObjects();

    void KillGfxObjects();

    void ConfigureOpenGL();

    bool PrepareToRender();

    void DoFrame();

    bool IsAnimating();

    void HandleGameActivityInput();

    void CheckForNewAxis();

    // Save the checkpoint level in the cloud
    void SaveGameToCloud(int level);

    // returns whether or not this level is a "checkpoint level" (that is,
    // where progress should be saved)
    bool IsCheckpointLevel(int level) {
        return 0 == level % LEVELS_PER_CHECKPOINT;
    }

public:

    // these are public for simplicity because we have internal static callbacks
    void HandleCommand(int32_t cmd);

    bool HandleInput(AInputEvent *event);

    int32_t GetActiveGameControllerIndex();

    void SetActiveGameControllerIndex(const int32_t controllerIndex);
};

#endif
