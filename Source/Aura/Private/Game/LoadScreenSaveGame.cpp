// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LoadScreenSaveGame.h"

FSavedMap ULoadScreenSaveGame::GetSavedMapWithName(const FString& InMapName)
{
    for (const FSavedMap& Map : SavedMaps)
    {
        if (Map.MapAssetName == InMapName)
        {
            return Map;
        }
    }
    return FSavedMap();
}

bool ULoadScreenSaveGame::HasMap(const FString& InMapName)
{
    for (const FSavedMap& Map : SavedMaps)
    {
        if (Map.MapAssetName == InMapName)
        {
            return true;
        }
    }
    return false;
}
