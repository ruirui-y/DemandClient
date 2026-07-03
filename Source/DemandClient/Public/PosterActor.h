#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PosterActor.generated.h"

UCLASS(BlueprintType)
class DEMANDCLIENT_API APosterActor : public AActor
{
    GENERATED_BODY()

public:
    APosterActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Poster")
    void SetPosterTexture(UTexture2D* Texture);

    UFUNCTION(BlueprintCallable, Category = "Poster")
    void SetPosterVisible(bool bVisible);

    // 暴露给蓝图的接口：让你在编辑器里指定基础材质 (M_PosterBase)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poster")
    UMaterialInterface* BaseMaterialAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poster")
    UTexture2D* DefaultPosterTexture_;

private:
    UPROPERTY(VisibleAnywhere, Category = "Poster")
    UStaticMeshComponent* PlaneMesh_;

    UPROPERTY()
    UMaterialInstanceDynamic* PosterMaterial_;
};