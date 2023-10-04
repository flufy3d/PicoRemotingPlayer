#pragma once
#include <sys/types.h>

struct AssetData {
    char* data;
    off_t length;
    
    ~AssetData() {
        delete[] data;
    }
};

class MyAssetManager {
public:
    static MyAssetManager& Instance();
    void Initialize(struct android_app* app);
    AssetData LoadAsset(const char* filename);

private:
    MyAssetManager();
    ~MyAssetManager();
    MyAssetManager(const MyAssetManager&) = delete;
    MyAssetManager& operator=(const MyAssetManager&) = delete;

    struct AAssetManager* assetManager;
};
