#pragma once

#include "RServiceClient.h"
#include "ROSBridgeSrvClient.h"
#include "srv/ConfigureJointStatePublisher.h"
// clang-format off
#include "RJointStateConfigurationClient.generated.h"
// clang-format on

UCLASS()
class UROBOSIM_API URJointStateConfigurationClientParameter final : public URServiceClientParameter
{
  GENERATED_BODY()

public:
  URJointStateConfigurationClientParameter()
  {
    ServiceName = TEXT("rosapi/get_param");
    ServiceType = TEXT("rosapi/GetParam");
  }

public:
  UPROPERTY(EditAnywhere)
  FString JointParamTopic;
};

UCLASS()
class UROBOSIM_API URJointStateConfigurationClient final : public URServiceClient
{
  GENERATED_BODY()

public:
  URJointStateConfigurationClient();

public:
  void SetServiceClientParameters(URServiceClientParameter *&ServiceClientParameters) override;

  void CallService() override;

protected:
  void CreateServiceClient() override;

public:
  void GetJointNames(TArray<FString> *OutJointNamesPtr);

public:
  UPROPERTY(EditAnywhere)
  FString JointParamTopic;

private:
  TSharedPtr<class FRJointStateConfigurationClient> ServiceClient;

  TSharedPtr<rosapi::GetParam::Request> Request;

  TSharedPtr<rosapi::GetParam::Response> Response;
};

class FRJointStateConfigurationClient final : public FROSBridgeSrvClient
{
public:
  FRJointStateConfigurationClient(TArray<FString> *OutJointNames, const FString &InName, const FString &InType);

  void Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse) override;

private:
  TArray<FString> *JointNames;
};