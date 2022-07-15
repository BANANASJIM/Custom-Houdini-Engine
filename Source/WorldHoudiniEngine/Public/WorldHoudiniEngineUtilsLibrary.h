// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProceduralMeshComponent.h"
#include "HAPI_To_Unreal_Common_Bridge.h"
#include "WorldHoudiniEngineUtilsLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WORLDHOUDINIENGINE_API UWorldHoudiniEngineUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

	UFUNCTION(BlueprintCallable,BlueprintPure,meta = (ToolTip = "Get mesh data from unreal engine."), Category = "WorldHoudiniEngineUtilsLibrary | Mesh")
	static bool GetUnrealMeshData(UStaticMesh* StaticMesh, TArray<FVector>& PositionList, TArray<int>& VertexList, TArray<int>& FaceList, TArray<FVector>& NormalList, TArray<FVector>& Tangentlist, TArray<FVector2D>& UVList, TArray<int>& SectionIndexList, TArray<UMaterialInterface*>& MaterialList);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "Convert vector list to float list."), Category = "WorldHoudiniEngineUtilsLibrary | Mesh")
	static bool VectorListToFloatList(const TArray<FVector>& VectorList,TArray<float>& FloatList, bool bSwitchYZAxis = true, float Multication = 0.01f);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "Convert float list to vector list."), Category = "WorldHoudiniEngineUtilsLibrary | Mesh")
	static bool FloatListToVectorList(const TArray<float>& FloatList, TArray<FVector>& VectorList, bool bSwitchYZAxis = true, float Multication = 100.f);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "Convert float list to procmeshtangent list."), Category = "WorldHoudiniEngineUtilsLibrary | Mesh")
	static bool FloatListToProcMeshTangentList(const TArray<float>& FloatList, TArray<FProcMeshTangent>& ProcMeshTangentList, bool bSwitchYZAxis = true ,bool bFlipTangentY = false);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "Convert vector 2d list to float list."), Category = "WorldHoudiniEngineUtilsLibrary | Mesh")
	static bool Vector2DListToFloatList(const TArray<FVector2D>& Vector2DList, TArray<float>& FloatList, bool bFlipY = true);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "Convert float list to vector 2d list."), Category = "WorldHoudiniEngineUtilsLibrary | Mesh")
	static bool FloatListToVector2DList(const TArray<float>& FloatList, TArray<FVector2D>& Vector2DList, bool bFlipY = true);

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Revert vertex list order."), Category = "WorldHoudiniEngineUtilsLibrary | Mesh")
	static bool RevertVertexListOrder(UPARAM(ref) TArray<int>& VertexList);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "Split Vertex list into multiple vertex list."), Category = "WorldHoudiniEngineUtilsLibrary | Mesh")
	static bool SplitVertexList(const TArray<int>& VertexList, const TArray<int>& SectionIndexList, TArray<FVertexListStruct>& SplittedVertexLists);
};