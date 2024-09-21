#include "SunovatechZombieKillHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "SunovatechZombieKill/SunovatechZombieKillPawn.h"

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

	// offset by designer specified value
	const FVector2D CrosshairDrawPosition((Center.X + OffsetFromCenter.X), (Center.Y + OffsetFromCenter.Y));

	ReticleCoordinates = CrosshairDrawPosition;

	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;

	// draw the reticle
	Canvas->DrawItem(TileItem);

	// Set the draw color to white
	Canvas->SetDrawColor(255, 255, 255);

	ASunovatechZombieKillPawn* MyPawn = Cast<ASunovatechZombieKillPawn>(GetOwningPlayerController()->GetPawn());
	if (MyPawn && TextFont)
	{
		float XL, YL;

		// Store the width and length of the text
		Canvas->StrLen(TextFont, FString("PlayerLife:"), XL, YL);

		// Draw the actual text
		Canvas->DrawText(TextFont, FString::Printf(TEXT("Health: %d"), MyPawn->GetHealth()), 2 * XL, Canvas->ClipY - 1.5 * YL);
		//Canvas->DrawText(TextFont, FString::Printf(TEXT("Score: %f"), MyController->GetScore()), 4 * XL, Canvas->ClipY - 1.5 * YL);
	}
}

void ASunovatechZombieKillHUD::BeginPlay()
{
	Super::BeginPlay();
}
