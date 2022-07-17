// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldHoudiniEngineBPLibrary.h"
#include "WorldHoudiniEngine.h"

UWorldHoudiniEngineBPLibrary::UWorldHoudiniEngineBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


FHoudiniSession UWorldHoudiniEngineBPLibrary::StartSeverAndCreateSession(FHoudiniSession HoudiniSession)
{
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_CreateThriftNamedPipeSession(&OrigSession, "hapi");
	if (Result != HAPI_RESULT_SUCCESS)
	{
		HAPI_ThriftServerOptions serverOptions{ 0 };
		serverOptions.autoClose = true;
		serverOptions.timeoutMs = 3000.0f;

		HAPI_Result StartResult = HAPI_StartThriftNamedPipeServer(&serverOptions, "hapi", nullptr);
		if(StartResult == HAPI_RESULT_SUCCESS)
			UE_LOG(LogTemp, Log, TEXT("Houdini named pipe server has been successfully started!"));
		Result = HAPI_CreateThriftNamedPipeSession(&OrigSession, "hapi");
	}
	if(Result == HAPI_RESULT_SUCCESS)
		UE_LOG(LogTemp, Log, TEXT("Houdini named pipe session has been successfully created!"));

	return FHoudiniSession::FromHAPI_Session(OrigSession);
}

FHoudiniCookOptions UWorldHoudiniEngineBPLibrary::CreateHoudiniCookOptions()
{
	HAPI_CookOptions CookOptions;
	CookOptions = HAPI_CookOptions_Create();
	FHoudiniCookOptions HoudiniCookOptions;
	HoudiniCookOptions.HAPICookOptions = CookOptions;
	UE_LOG(LogTemp, Log, TEXT("Houdini CookOptions has been successfully created!"));
	return HoudiniCookOptions;
}

void UWorldHoudiniEngineBPLibrary::HoudiniInitialize(FHoudiniSession HoudiniSession, const FHoudiniCookOptions& HoudiniCookOptions)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		UE_LOG(LogTemp, Log, TEXT("Houdini Session invalided "));
		return;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_Initialize(&OrigSession, &HoudiniCookOptions.HAPICookOptions, true, -1, nullptr, nullptr, nullptr, nullptr, nullptr);
	if (Result == HAPI_RESULT_SUCCESS)
	{
		UE_LOG(LogTemp, Log, TEXT("Houdini Session has been successfully intialized!"));
	}
}

bool UWorldHoudiniEngineBPLibrary::HoudiniLoadAssertLibraryFromFile(FHoudiniSession HoudiniSession, FString OtlFilePath, bool bAllowOverwrite, int& AssertLibraryId)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_AssetLibraryId LibraryId = -1;
	HAPI_Result Result = HAPI_LoadAssetLibraryFromFile(&OrigSession, TCHAR_TO_UTF8(*OtlFilePath), (HAPI_Bool)bAllowOverwrite, &LibraryId);
	AssertLibraryId = (int)LibraryId;
	
	if(Result == HAPI_RESULT_SUCCESS)
		UE_LOG(LogTemp, Log, TEXT("Houdini Assert has been successfully loaded!"));
	return Result == HAPI_RESULT_SUCCESS || Result == HAPI_RESULT_ASSET_DEF_ALREADY_LOADED;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniGetAssertOperatorName(FHoudiniSession HoudiniSession, int AssertLibraryId, FString& FullOpName, FString& Label)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_AssetLibraryId Id = (HAPI_AssetLibraryId)AssertLibraryId;
	int AssertCount = INDEX_NONE;
	HAPI_GetAvailableAssetCount(&OrigSession, Id, &AssertCount);
	check(AssertCount == 1);
	HAPI_StringHandle AssertSH;
	HAPI_Result Result = HAPI_GetAvailableAssets(&OrigSession, Id, &AssertSH, AssertCount);
	FullOpName = ToString(HoudiniSession, AssertSH);
	TArray<FString> OutStringArray;
	FullOpName.ParseIntoArray(OutStringArray, TEXT("/"), true);
	Label = OutStringArray[OutStringArray.Num() - 1];

	if (Result == HAPI_RESULT_SUCCESS)
		UE_LOG(LogTemp, Log, TEXT("Houdini OpName has been successfully got!"));
	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniCreateNode(FHoudiniSession HoudiniSession, FString OperatorName, FString Label, int& NodeId, int ParentNodeId, bool bCookOnCreation)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_NodeId OrigNodeId;
	HAPI_Result Result = HAPI_CreateNode(&OrigSession, (HAPI_NodeId)ParentNodeId, TCHAR_TO_UTF8(*OperatorName), TCHAR_TO_UTF8(*Label), (HAPI_Bool)bCookOnCreation, &OrigNodeId);
	NodeId = (int)OrigNodeId;
	if (Result == HAPI_RESULT_SUCCESS)
		UE_LOG(LogTemp, Log, TEXT("Houdini Node has been successfully create!"));
	return Result == HAPI_RESULT_SUCCESS;
}

void UWorldHoudiniEngineBPLibrary::HoudiniCookNode(FHoudiniSession HoudiniSession, int NodeId, const FHoudiniCookOptions& HoudiniCookOptions)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return ;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_CookNode(&OrigSession, (HAPI_NodeId)NodeId,&HoudiniCookOptions.HAPICookOptions);
	if (Result == HAPI_RESULT_SUCCESS)
		UE_LOG(LogTemp, Log, TEXT("Houdini Node has been successfully cooked!"));
}

bool UWorldHoudiniEngineBPLibrary::HoudiniSaveHIPFile(FHoudiniSession HoudiniSession, FString FilePath, bool bLockNodes /*= false*/)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_SaveHIPFile(&OrigSession, TCHAR_TO_UTF8(*FilePath), (HAPI_Bool)bLockNodes);
	if (Result == HAPI_RESULT_SUCCESS)
		UE_LOG(LogTemp, Log, TEXT("Houdini HipFile has been successfully saved!"));
	return Result == HAPI_RESULT_SUCCESS;
}

void UWorldHoudiniEngineBPLibrary::HoudiniCleanupSession(FHoudiniSession HoudiniSession)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_Cleanup(&OrigSession);
	
	if (Result == HAPI_RESULT_SUCCESS)
		UE_LOG(LogTemp, Log, TEXT("Houdini Session has been successfully cleanup!"));
}

void UWorldHoudiniEngineBPLibrary::HoudiniCloseSession(FHoudiniSession HoudiniSession)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_CloseSession(&OrigSession);
	if (Result == HAPI_RESULT_SUCCESS)
		UE_LOG(LogTemp, Log, TEXT("Houdini Session has been successfully closed!"));
}

bool UWorldHoudiniEngineBPLibrary::HoudiniIsInitialize(FHoudiniSession HoudiniSession)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_IsInitialized(&OrigSession);

	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniComposeChildNodeList(FHoudiniSession HoudiniSession, int ParentNodeId, EHoudiniNodeType NodeType, EHoudiniNodeFlags NodeFlags, int& ChildCount, bool bRecursive)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	int ChildListCount;
	FEnumParser<HAPI_NodeType> EnumNodeTypeParser;
	FEnumParser<HAPI_NodeFlags> EnumNodeFlagsParser;
	HAPI_Result Result = HAPI_ComposeChildNodeList(&OrigSession, (HAPI_NodeId)ParentNodeId, (HAPI_NodeTypeBits)EnumNodeTypeParser.ParseEnum(HoudiniEnumToString(NodeType)), 
		(HAPI_NodeFlagsBits)EnumNodeFlagsParser.ParseEnum(HoudiniEnumToString(NodeFlags)), (HAPI_Bool)bRecursive, &ChildListCount);
	ChildCount = ChildListCount;
	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniGetComposedChildNodeList(FHoudiniSession HoudiniSession, int ParentNodeId, TArray<int>& ChildNodeIdList, int ChildCount)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	ChildNodeIdList.SetNumUninitialized(ChildCount);
	HAPI_Result Result = HAPI_GetComposedChildNodeList(&OrigSession, (HAPI_NodeId)ParentNodeId, ChildNodeIdList.GetData(), ChildCount);
	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniConnectNodeInput(FHoudiniSession HoudiniSession, int NodeId, int InputIndex, int NodeIdToConnect, int OutputIndex)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_ConnectNodeInput(&OrigSession, (HAPI_NodeId)NodeId, InputIndex, (HAPI_NodeId)NodeIdToConnect, OutputIndex);
	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniQueryNodeInput(FHoudiniSession HoudiniSession, int NodeId, int InputIndex, int& ConnectedNodeId)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_NodeId Id;
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_QueryNodeInput(&OrigSession, (HAPI_NodeId)NodeId, InputIndex, &Id);
	ConnectedNodeId = (int)Id;
	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniDisconnectNodeInput(FHoudiniSession HoudiniSession, int NodeId, int InputIndex)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_DisconnectNodeInput(&OrigSession, (HAPI_NodeId)NodeId, InputIndex);
	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniDeleteNode(FHoudiniSession HoudiniSession, int NodeId)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_DeleteNode(&OrigSession, (HAPI_NodeId)NodeId);
	return Result == HAPI_RESULT_SUCCESS;
}


bool UWorldHoudiniEngineBPLibrary::HoudiniCreateInputNode(FHoudiniSession HoudiniSession, int& NodeId, FString Label)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_NodeId Id;
	HAPI_Result Result = HAPI_CreateInputNode(&OrigSession, &Id, TCHAR_TO_UTF8(*Label));
	NodeId = (int)Id;
	return Result == HAPI_RESULT_SUCCESS;
}

FHoudiniPartInfo UWorldHoudiniEngineBPLibrary::HoudiniCreatePartInfo(int FaceCount, int VertexCount, int PointCount, EHoudiniPartType HoudiniPartType)
{
	HAPI_PartInfo PartInfo = HAPI_PartInfo_Create();
	PartInfo.faceCount = FaceCount;
	PartInfo.vertexCount = VertexCount;
	PartInfo.pointCount = PointCount;
	FEnumParser<HAPI_PartType> EnumParser;
	PartInfo.type = EnumParser.ParseEnum(HoudiniEnumToString(HoudiniPartType));
	FHoudiniPartInfo HoudiniPartInfo;
	HoudiniPartInfo.HAPIPartInfo = PartInfo;
	return HoudiniPartInfo;

}

bool UWorldHoudiniEngineBPLibrary::HoudiniSetPartInfo(FHoudiniSession HoudiniSession, int NodeId, const FHoudiniPartInfo& HoudiniPartInfo)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_SetPartInfo(&OrigSession, (HAPI_NodeId)NodeId, 0, &HoudiniPartInfo.HAPIPartInfo);
	return Result == HAPI_RESULT_SUCCESS;
}

FHoudiniAttributeInfo UWorldHoudiniEngineBPLibrary::HoudiniCreateAttributeInfo(int Count, int TupleSize, bool bExists, EHoudiniAttributeOwner AttributeOwner, EHoudiniStorageType StorageType)
{
	HAPI_AttributeInfo AttribInfo = HAPI_AttributeInfo_Create();
	AttribInfo.count = Count;
	AttribInfo.exists = (HAPI_Bool)bExists;
	AttribInfo.tupleSize = TupleSize;

	FEnumParser<HAPI_AttributeOwner> AttributeOwnerEnumParser;
	FEnumParser<HAPI_StorageType> StorageTypeEnumParser;

	AttribInfo.owner = AttributeOwnerEnumParser.ParseEnum(HoudiniEnumToString(AttributeOwner));
	AttribInfo.storage = StorageTypeEnumParser.ParseEnum(HoudiniEnumToString(StorageType));
	FHoudiniAttributeInfo HoudiniAttributeInfo;
	HoudiniAttributeInfo.HAPIAttributeInfo = AttribInfo;
	return HoudiniAttributeInfo;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniAddAndSetFloatAttribute(FHoudiniSession HoudiniSession, int NodeId, FString AttributeName, const FHoudiniAttributeInfo& AttribInfo, const TArray<float>& DataArray, int Count)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result AddResult = HAPI_AddAttribute(&OrigSession, (HAPI_NodeId)NodeId, 0, TCHAR_TO_UTF8(*AttributeName), &AttribInfo.HAPIAttributeInfo);
	HAPI_Result SetResult = HAPI_SetAttributeFloatData(&OrigSession, (HAPI_NodeId)NodeId, 0, TCHAR_TO_UTF8(*AttributeName), &AttribInfo.HAPIAttributeInfo, DataArray.GetData(), 0, Count);
	return AddResult == HAPI_RESULT_SUCCESS && SetResult == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniAddAndSetStringAttribute(FHoudiniSession HoudiniSession, int NodeId, FString AttributeName, const FHoudiniAttributeInfo& AttribInfo, const TArray<FString>& DataArray, int Count)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result AddResult = HAPI_AddAttribute(&OrigSession, (HAPI_NodeId)NodeId, 0, TCHAR_TO_UTF8(*AttributeName), &AttribInfo.HAPIAttributeInfo);

	char** strs = new char* [DataArray.Num()];
	for (int i = 0; i < DataArray.Num(); i++)
	{
		strs[i] = _strdup(TCHAR_TO_UTF8(*DataArray[i]));
	}
	HAPI_Result SetResult = HAPI_SetAttributeStringData(&OrigSession, (HAPI_NodeId)NodeId, 0, TCHAR_TO_UTF8(*AttributeName), &AttribInfo.HAPIAttributeInfo, (const char**)strs, 0, Count);
	for (int j = 0; j < DataArray.Num(); j++)
	{
		delete strs[j];
		strs[j] = nullptr;
	}
	delete strs;
	return AddResult == HAPI_RESULT_SUCCESS && SetResult == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniAddAndSetIntAttribute(FHoudiniSession HoudiniSession, int NodeId, FString AttributeName, const FHoudiniAttributeInfo& AttribInfo, const TArray<int>& DataArray, int Count)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result AddResult = HAPI_AddAttribute(&OrigSession, (HAPI_NodeId)NodeId, 0, TCHAR_TO_UTF8(*AttributeName), &AttribInfo.HAPIAttributeInfo);
	HAPI_Result SetResult = HAPI_SetAttributeIntData(&OrigSession, (HAPI_NodeId)NodeId, 0, TCHAR_TO_UTF8(*AttributeName), &AttribInfo.HAPIAttributeInfo, DataArray.GetData(), 0, Count);
	return AddResult == HAPI_RESULT_SUCCESS && SetResult == HAPI_RESULT_SUCCESS;
}


bool UWorldHoudiniEngineBPLibrary::HoudiniGetAttributeInfo(FHoudiniSession HoudiniSession, int NodeId, int PartId, FString AttributeName, FHoudiniAttributeInfo& AttributeInfo, EHoudiniAttributeOwner AttributeOwner)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();

	FEnumParser<HAPI_AttributeOwner> EnumParser;
	HAPI_Result Result = HAPI_GetAttributeInfo(&OrigSession, (HAPI_NodeId)NodeId, (HAPI_PartId)PartId, TCHAR_TO_UTF8(*AttributeName), EnumParser.ParseEnum(HoudiniEnumToString(AttributeOwner)), &AttributeInfo.HAPIAttributeInfo);
	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniGetAttributeIntData(FHoudiniSession HoudiniSession, int NodeId, int PartId, FString AttributeName, FHoudiniAttributeInfo AttributeInfo, TArray<int>& DataArray)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	DataArray.SetNumUninitialized(AttributeInfo.HAPIAttributeInfo.tupleSize * AttributeInfo.HAPIAttributeInfo.count);
	HAPI_Result Result = HAPI_GetAttributeIntData(&OrigSession, (HAPI_NodeId)NodeId, (HAPI_PartId)PartId, TCHAR_TO_UTF8(*AttributeName), &AttributeInfo.HAPIAttributeInfo, -1, DataArray.GetData(), 0, AttributeInfo.HAPIAttributeInfo.count);
	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniGetAttributeFloatData(FHoudiniSession HoudiniSession, int NodeId, int PartId, FString AttributeName, FHoudiniAttributeInfo AttributeInfo, TArray<float>& DataArray)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	DataArray.SetNumUninitialized(AttributeInfo.HAPIAttributeInfo.tupleSize * AttributeInfo.HAPIAttributeInfo.count);
	HAPI_Result Result = HAPI_GetAttributeFloatData(&OrigSession, (HAPI_NodeId)NodeId, (HAPI_PartId)PartId, TCHAR_TO_UTF8(*AttributeName), &AttributeInfo.HAPIAttributeInfo, -1, DataArray.GetData(), 0, AttributeInfo.HAPIAttributeInfo.count);
	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniGetAttributeStringData(FHoudiniSession HoudiniSession, int NodeId, int PartId, FString AttributeName, FHoudiniAttributeInfo AttributeInfo, TArray<FString>& DataArray)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	TArray<HAPI_StringHandle> StringHandleList;
	int StringCount = AttributeInfo.HAPIAttributeInfo.tupleSize * AttributeInfo.HAPIAttributeInfo.count;
	StringHandleList.SetNumUninitialized(StringCount);
	HAPI_Result Result = HAPI_GetAttributeStringData(&OrigSession, (HAPI_NodeId)NodeId, (HAPI_PartId)PartId, TCHAR_TO_UTF8(*AttributeName), &AttributeInfo.HAPIAttributeInfo, StringHandleList.GetData(), 0, StringCount);
	if(Result != HAPI_RESULT_SUCCESS)
	{
		return false;
	}
	
	TMap<HAPI_StringHandle, FString> HandleToStringMap;
	DataArray.SetNum(StringCount);
	for (int i = 0; i < StringCount; i++)
	{
		if (i > 0 && StringHandleList[i] == StringHandleList[i - 1])
		{
			DataArray[i] = DataArray[i - 1];
		}
		const FString* FoundString = HandleToStringMap.Find(StringHandleList[i]);
		if (FoundString)
		{
			DataArray[i] = *FoundString;
		}
		else
		{
			DataArray[i] = ToString(HoudiniSession, StringHandleList[0]);
			HandleToStringMap.Add(StringHandleList[i], DataArray[i]);
		}
	}
	return true;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniSetVertexListAndFaceCounts(FHoudiniSession HoudiniSession, int NodeId, const TArray<int>& VertexDataArray)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result VertexResult = HAPI_SetVertexList(&OrigSession, (HAPI_NodeId)NodeId, 0, VertexDataArray.GetData(), 0, VertexDataArray.Num());

	TArray<int> FaceDataArray;
	FaceDataArray.Init(3, VertexDataArray.Num() / 3);
	HAPI_Result FaceResult = HAPI_SetFaceCounts(&OrigSession, (HAPI_NodeId)NodeId, 0, FaceDataArray.GetData(), 0, FaceDataArray.Num());
	return VertexResult == HAPI_RESULT_SUCCESS && FaceResult == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniCommitGeo(FHoudiniSession HoudiniSession, int NodeId)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_CommitGeo(&OrigSession, (HAPI_NodeId)NodeId);
	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniGetVertexList(FHoudiniSession HoudiniSession, int NodeId, int PartId, TArray<int>& VertexList, int Count)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	VertexList.SetNumUninitialized(Count);
	HAPI_Result Result = HAPI_GetVertexList(&OrigSession, (HAPI_NodeId)NodeId, (HAPI_PartId)PartId, VertexList.GetData(), 0, Count);
	return Result == HAPI_RESULT_SUCCESS;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniGetPartInfo(FHoudiniSession HoudiniSession, int NodeId, int PartId, FHoudiniPartInfo& PartInfo)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_GetPartInfo(&OrigSession, (HAPI_NodeId)NodeId, (HAPI_PartId)PartId, &PartInfo.HAPIPartInfo);
	return Result == HAPI_RESULT_SUCCESS;
}

void UWorldHoudiniEngineBPLibrary::HoudiniGetPartInfoSubData(const FHoudiniPartInfo& PartInfo, int& FaceCount, int& PointCount)
{
	FaceCount = PartInfo.HAPIPartInfo.faceCount;
	PointCount = PartInfo.HAPIPartInfo.pointCount;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniGetNodeInfo(FHoudiniSession HoudiniSession, int NodeId, FHoudiniNodeInfo& NodeInfo)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return false;
	}
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_GetNodeInfo(&OrigSession, (HAPI_NodeId)NodeId, &NodeInfo.HAPINodeInfo);
	return Result == HAPI_RESULT_SUCCESS;
}

void UWorldHoudiniEngineBPLibrary::HoudiniGetNodeInfoSubData(const FHoudiniNodeInfo& NodeInfo, int& ParentNodeId, bool& bIsValid, int& uniqueHoudiniNodeId)
{
	ParentNodeId = (int)NodeInfo.HAPINodeInfo.parentId;
	bIsValid = (bool)NodeInfo.HAPINodeInfo.isValid;
	uniqueHoudiniNodeId = NodeInfo.HAPINodeInfo.uniqueHoudiniNodeId;
}

FString UWorldHoudiniEngineBPLibrary::ToString(FHoudiniSession HoudiniSession, HAPI_StringHandle InStringHandle)
{
	if (!HoudiniIsSessionValid(HoudiniSession))
	{
		return FString("");
	}
	check(InStringHandle >= 0);
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	int32 StringLength = 0;
	HAPI_Result Result = HAPI_GetStringBufLength(&OrigSession, InStringHandle, &StringLength);
	check(Result == HAPI_RESULT_SUCCESS);
	check(StringLength > 0);
	TArray<ANSICHAR> StringBuffer;
	StringBuffer.SetNumUninitialized(StringLength);
	StringBuffer.Last() = 0;

	HAPI_GetString(&OrigSession, InStringHandle, StringBuffer.GetData(), StringLength);
	check(Result == HAPI_RESULT_SUCCESS);

	FString FinalString = UTF8_TO_TCHAR(StringBuffer.GetData());
	return FinalString;
}

bool UWorldHoudiniEngineBPLibrary::HoudiniIsSessionValid(FHoudiniSession HoudiniSession)
{
	HAPI_Session OrigSession = HoudiniSession.ToHAPi_Session();
	HAPI_Result Result = HAPI_IsSessionValid(&OrigSession);
	return Result == HAPI_RESULT_SUCCESS;
}

template<typename T>
FString UWorldHoudiniEngineBPLibrary::HoudiniEnumToString(const T EnumValue)
{
	FString Name = StaticEnum<T>()->GetNameStringByValue(static_cast<__underlying_type(T)>(EnumValue));
	check(Name.Len() != 0);
	return Name;
}