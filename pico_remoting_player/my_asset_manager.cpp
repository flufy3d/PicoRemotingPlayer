#include "my_asset_manager.h"
#include "pch.h"
#include "common.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android_native_app_glue.h>


MyAssetManager::MyAssetManager() : assetManager(nullptr) {}

MyAssetManager::~MyAssetManager() {}

MyAssetManager& MyAssetManager::Instance() {
    static MyAssetManager instance;
    return instance;
}

void MyAssetManager::Initialize(struct android_app* app) {
    assetManager = app->activity->assetManager;
}

AssetData MyAssetManager::LoadAsset(const char* filename) {

    AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);

    if (asset == nullptr) {

       Log::Write(Log::Level::Error, "AAssetManager_open failed");

       return AssetData{nullptr, 0};
    }


    off_t len = AAsset_getLength(asset);
    char* p = new char[len];
    AAsset_read(asset, p, len);

    AAsset_close(asset);
    return AssetData{p, len};
}
