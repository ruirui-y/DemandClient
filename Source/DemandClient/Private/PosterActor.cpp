#include "PosterActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

APosterActor::APosterActor()
{
    PrimaryActorTick.bCanEverTick = true;

    PlaneMesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
    RootComponent = PlaneMesh_;

    // 构造函数里现在只负责寻找最底层的 Mesh 模型
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneFinder(
        TEXT("/Engine/EngineMeshes/Plane.Plane"));
    if (PlaneFinder.Succeeded())
    {
        PlaneMesh_->SetStaticMesh(PlaneFinder.Object);
    }

    PlaneMesh_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PlaneMesh_->SetVisibility(false);
}

void APosterActor::BeginPlay()
{
    Super::BeginPlay();

    // 如果你在蓝图面板里成功指定了基础材质
    if (BaseMaterialAsset)
    {
        // 基于配置的材质创建动态材质实例 (MID)
        PosterMaterial_ = UMaterialInstanceDynamic::Create(BaseMaterialAsset, this);
        PlaneMesh_->SetMaterial(0, PosterMaterial_);
    }
}

// 每一帧追踪相机
void APosterActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APlayerCameraManager* Cam = UGameplayStatics::GetPlayerCameraManager(this, 0);
    if (!Cam) return;

    // 位置：相机正前方 150 uu (1.5 米)
    FVector CamLoc = Cam->GetCameraLocation();
    FRotator CamRot = Cam->GetCameraRotation();
    SetActorLocation(CamLoc + CamRot.Vector() * 150.0f);

    // 旋转修正：让 Plane 面片立起来朝向相机
    FRotator PlaneRot = CamRot;
    PlaneRot.Pitch += 90.0f;
    SetActorRotation(PlaneRot);
}

void APosterActor::SetPosterTexture(UTexture2D* Texture)
{
    if (PosterMaterial_ && Texture)
    {
        // 将传入的图片赋予名为 "PosterTexture" 的材质参数
        PosterMaterial_->SetTextureParameterValue(TEXT("PosterTexture"), Texture);
        PlaneMesh_->SetVisibility(true);

        // 根据图片比例自动调整面片的缩放
        float Aspect = (float)Texture->GetSizeX() / (float)Texture->GetSizeY();
        PlaneMesh_->SetWorldScale3D(FVector(Aspect, 1.0f, 1.0f));
    }
}

void APosterActor::SetPosterVisible(bool bVisible)
{
    if (PlaneMesh_)
    {
        PlaneMesh_->SetVisibility(bVisible);
    }
}