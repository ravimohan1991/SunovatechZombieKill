#include "SunovatechZombieKillHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
//#include "SunovatechZombieKill/SunovatechZombieKillPawn.h"
#include "Kismet/GameplayStatics.h"

ASunovatechZombieKillHUD::ASunovatechZombieKillHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/UI/FirstPersonCrosshair"));
	CrosshairTex = Cast<UTexture2D>(CrosshairTexObj.Object)->GetResource();

	OffsetFromCenter.X = OffsetFromCenter.Y = 0.0f;
}

void ASunovatechZombieKillHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	/*FVector MuzzleOffset;
	if(GetOwningPawn())
	{
		MuzzleOffset = Cast<ASunovatechZombieKillPawn>(GetOwningPawn())->MuzzleOffset;// for fast prototyping only
	}

	FVector2D ReticleCoordinates;
	if(GetOwningPlayerController() && GetOwningPawn())
	{
		UGameplayStatics::ProjectWorldToScreen(GetOwningPlayerController(), GetOwningPawn()->GetActorLocation() + MuzzleOffset, ReticleCoordinates);
	}*/

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X + OffsetFromCenter.X), (Center.Y + OffsetFromCenter.Y));

	ReticleCoordinates = CrosshairDrawPosition;

	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;

	// draw the reticle
	Canvas->DrawItem(TileItem);
}

void ASunovatechZombieKillHUD::BeginPlay()
{
	Super::BeginPlay();
}
