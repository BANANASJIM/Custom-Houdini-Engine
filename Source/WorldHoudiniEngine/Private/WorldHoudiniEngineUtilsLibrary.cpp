// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldHoudiniEngineUtilsLibrary.h"
#include "Kismet/KismetMathLibrary.h"

bool UWorldHoudiniEngineUtilsLibrary::GetUnrealMeshData(UStaticMesh* StaticMesh, TArray<FVector>& PositionList, TArray<int>& VertexList, TArray<int>& FaceList, TArray<FVector>& NormalList, TArray<FVector>& Tangentlist, TArray<FVector2D>& UVList, TArray<int>& SectionIndexList, TArray<UMaterialInterface*>& MaterialList)
{
    if (!StaticMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("The static mesh is none."));
        return false;
    }

    if (!StaticMesh->IsValidLowLevel())
    {
        UE_LOG(LogTemp, Warning, TEXT("The static mesh is not valid."));
        return false;
    }

    if (!StaticMesh->RenderData|| StaticMesh->RenderData->LODResources.Num()<=0)
    {
        UE_LOG(LogTemp, Warning, TEXT("The static mesh has empty data."));
        return false;
    }
    FPositionVertexBuffer* PositionVertexBuffer = &StaticMesh->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;
    auto VertexBuffer = &StaticMesh->RenderData->LODResources[0].VertexBuffers;
    int NumberOfPoints = PositionVertexBuffer->GetNumVertices();
    int NumIndices = StaticMesh->RenderData->LODResources[0].IndexBuffer.GetNumIndices();
    FIndexArrayView IndexArray = StaticMesh->RenderData->LODResources[0].IndexBuffer.GetArrayView();
    int FaceCount = StaticMesh->RenderData->LODResources[0].GetNumTriangles();
    auto Sections = StaticMesh->RenderData->LODResources[0].Sections;

    PositionList.SetNumUninitialized(NumberOfPoints);
    VertexList.SetNumUninitialized(NumIndices);
    UVList.SetNumUninitialized(NumberOfPoints);
    NormalList.SetNumUninitialized(NumberOfPoints);
    Tangentlist.SetNumUninitialized(NumberOfPoints);

    MaterialList.SetNumUninitialized(Sections.Num());
    SectionIndexList.SetNumUninitialized(FaceCount);
    FaceList.Init(3, FaceCount);

// Position list
    for (int i = 0; i < NumberOfPoints; i++)
    {
        PositionList[i] = PositionVertexBuffer->VertexPosition(i);
        UVList[i] = VertexBuffer->StaticMeshVertexBuffer.GetVertexUV(i, 0);
        NormalList[i] = VertexBuffer->StaticMeshVertexBuffer.VertexTangentZ(i);
        Tangentlist[i] = VertexBuffer->StaticMeshVertexBuffer.VertexTangentX(i);
    }

// Vertex List UV list Normal List
    for (int j = 0; j < NumIndices; j++)
    {
        VertexList[j] = IndexArray[j];

    }

    for (int k = 0; k < Sections.Num(); k++)
    {
        MaterialList[k] = StaticMesh->GetMaterial(Sections[k].MaterialIndex);
        int NumTriangle = Sections[k].NumTriangles;
        for (int tri = 0; tri < NumTriangle; tri++)
        {
            int FirstIndex = Sections[k].FirstIndex;
            SectionIndexList[tri + FirstIndex / 3] = k;
        }
    }
    
    return true;
}

bool UWorldHoudiniEngineUtilsLibrary::VectorListToFloatList(const TArray<FVector>& VectorList, TArray<float>& FloatList, bool bSwitchYZAxis, float Multication)
{
    if (VectorList.Num() == 0)
    {
        return false;
    }

    for (int i = 0; i < VectorList.Num(); i++)
    {
        FloatList.Add(VectorList[i].X * Multication);
        bSwitchYZAxis ? FloatList.Add(VectorList[i].Z * Multication) : FloatList.Add(VectorList[i].Y * Multication);
        bSwitchYZAxis ? FloatList.Add(VectorList[i].Y * Multication) : FloatList.Add(VectorList[i].Z * Multication);
    }
    return true;
}

bool UWorldHoudiniEngineUtilsLibrary::FloatListToVectorList(const TArray<float>& FloatList, TArray<FVector>& VectorList, bool bSwitchYZAxis, float Multication)
{
    if (FloatList.Num() == 0 || FloatList.Num() % 3 != 0)
    {
        return false;
    }

    for (int i = 0; i < FloatList.Num() / 3; i++)
    {
        int Index1 = bSwitchYZAxis ? 2 : 1;
        int Index2 = bSwitchYZAxis ? 1 : 2;
        VectorList.Add(FVector(FloatList[i * 3 + 0],
                               FloatList[i * 3 + Index1],
                               FloatList[i * 3 + Index2]
                       )* Multication);
    }
    return true;
}

bool UWorldHoudiniEngineUtilsLibrary::FloatListToProcMeshTangentList(const TArray<float>& FloatList, TArray<FProcMeshTangent>& ProcMeshTangentList, bool bSwitchYZAxis, bool bFlipTangentY)
{
    if (FloatList.Num() == 0 || FloatList.Num() % 3 != 0) 
    {
        return false;
    }

    for (int i = 0; i < FloatList.Num() / 3; i++)
    {
        int Index1 = bSwitchYZAxis ? 2 : 1;
        int Index2 = bSwitchYZAxis ? 1 : 2;
        ProcMeshTangentList.Add(FProcMeshTangent(FVector(FloatList[i * 3 + 0],
                                                         FloatList[i * 3 + Index1],
                                                         FloatList[i * 3 + Index2]
                                                        ),bFlipTangentY));
    }
    return true;
}

bool UWorldHoudiniEngineUtilsLibrary::Vector2DListToFloatList(const TArray<FVector2D>& Vector2DList, TArray<float>& FloatList, bool bFlipY)
{
    if (Vector2DList.Num() == 0)
    {
        return false;
    }

    for (int i = 0; i < Vector2DList.Num(); i++)
    {
        FloatList.Add(Vector2DList[i].X );
        bFlipY ? FloatList.Add(1 - Vector2DList[i].Y) : FloatList.Add(Vector2DList[i].Y);
        FloatList.Add(0.f);
    }
    return true;
}

bool UWorldHoudiniEngineUtilsLibrary::FloatListToVector2DList(const TArray<float>& FloatList, TArray<FVector2D>& Vector2DList, bool bFlipY)
{
    if (FloatList.Num() == 0 || FloatList.Num() % 3 != 0)
    {
        return false;
    }

    for (int i = 0; i < FloatList.Num() / 3; i++)
    {
        Vector2DList.Add(FVector2D(FloatList[i * 3 + 0], bFlipY ? 1 - FloatList[i * 3 + 1] : FloatList[i * 3 + 1]));
    }
    return true;
}

bool UWorldHoudiniEngineUtilsLibrary::SplitVertexList(const TArray<int>& VertexList, const TArray<int>& SectionIndexList, TArray<FVertexListStruct>& SplittedVertexLists)
{
    if (VertexList.Num() == 0 || SectionIndexList.Num() == 0 || VertexList.Num() != SectionIndexList.Num() * 3)
    {
        return false;
    }
    int MaxValueIndex = -1;
    int MaxValue;
    UKismetMathLibrary::MaxOfIntArray(SectionIndexList, MaxValueIndex, MaxValue);
    SplittedVertexLists.SetNum(MaxValue + 1);
    for (int i = 0; i < VertexList.Num(); i++)
    {
        SplittedVertexLists[SectionIndexList[i / 3]].Vertexlist.Add(VertexList[i]);
    }
    return true;
}

bool UWorldHoudiniEngineUtilsLibrary::SplitVertexListByStringAttributes(const TArray<int>& VertexList, const TArray<FString>& StringAttributeList, TArray<FVertexListStruct>& SplittedVertexLists)
{
    if (VertexList.Num() == 0 || StringAttributeList.Num() == 0 || VertexList.Num() != StringAttributeList.Num() * 3)
    {
        return false;
    }
    TArray<FString> UniqueStringAttributeList;
    for (auto& Str : StringAttributeList)
    {
        UniqueStringAttributeList.AddUnique(Str);
    }

    if (UniqueStringAttributeList.Num() == 0)
        return false;

    SplittedVertexLists.SetNum(UniqueStringAttributeList.Num());
    for (int i = 0; i < VertexList.Num(); i++)
    {
        int SectionIndex = UniqueStringAttributeList.Find(StringAttributeList[i / 3]);
        SplittedVertexLists[SectionIndex].Vertexlist.Add(VertexList[i]);
    }

    return true;
}

bool UWorldHoudiniEngineUtilsLibrary::RevertVertexListOrder(TArray<int>& VertexList)
{
    if (VertexList.Num() == 0 || VertexList.Num() % 3 != 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Vertexlist Num %d invaild "),VertexList.Num());
        return false;
    }

    /*for (int i = 0; i < vertexlist.num() / 3; i++)
    {
        swap(vertexlist[i * 3], vertexlist[i * 3 + 1]);
    }*/

    ParallelFor(VertexList.Num() / 3, [&VertexList](int i)
    {
        Swap(VertexList[i * 3], VertexList[i * 3 + 1]);
    });

    return true;
}
