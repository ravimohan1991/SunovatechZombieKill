#include "SunovatechZombieKillHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "SunovatechZombieKill/Public/SunovatechZombieKillPawn.h"

ASunovatechZombieKillHUD::ASunovatechZombieKillHUD()
{
	CrosshairTex = nullptr;
	HealthDisplayCoordinates.X = HealthDisplayCoordinates.Y = 0.0f;
}

void ASunovatechZombieKillHUD::DrawHUD()
{
	Super::DrawHUD();

	ASunovatechZombieKillPawn* MyPawn = Cast<ASunovatechZombieKillPawn>(GetOwningPlayerController()->GetPawn());
	if (MyPawn && TextFont)
	{
		if(MyPawn->GetCurrentReticle())
		{
			CrosshairTex = MyPawn->GetCurrentReticle()->GetResource();
		}
		else
		{
			CrosshairTex = NULL;
		}
		// Draw very simple crosshair
		if(CrosshairTex)
		{
			// find center of the Canvas
			const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

			const FVector2D OffsetFromCenter = MyPawn->GetReticleOffsetFromCenter();

			// offset by designer specified value
			const FVector2D CrosshairDrawPosition((Center.X + OffsetFromCenter.X), (Center.Y + OffsetFromCenter.Y));

			ReticleCoordinates.X = Center.X;
			ReticleCoordinates.Y = Center.Y;

			FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex, FLinearColor(MyPawn->GetReticleColor()));
			TileItem.BlendMode = SE_BLEND_Translucent;

			// draw the reticle
			Canvas->DrawItem(TileItem);
		}

		// Set the draw color to white
		Canvas->SetDrawColor(255, 255, 255);

		float XL, YL;
		float ScaleX, ScaleY;

		ScaleX = ScaleY = 2.5f;

		// Store the width and length of the text
		Canvas->StrLen(TextFont, FString("PlayerLife:"), XL, YL);

		// Draw the actual text
		if(HealthDisplayCoordinates.X == 0.0f && HealthDisplayCoordinates.Y == 0.0f)
		{
			Canvas->DrawText(TextFont, FString::Printf(TEXT("Health: %f"), MyPawn->GetHealth()), 2 * XL * ScaleX, Canvas->ClipY - 1.5 * YL * ScaleY, ScaleX, ScaleY);
		}
		else
		{
			Canvas->DrawText(TextFont, FString::Printf(TEXT("Health: %f"), MyPawn->GetHealth()), HealthDisplayCoordinates.X, HealthDisplayCoordinates.Y, ScaleX, ScaleY);
		}
	}
}

void ASunovatechZombieKillHUD::BeginPlay()
{
	Super::BeginPlay();

	ASunovatechZombieKillPawn* MyPawn = Cast<ASunovatechZombieKillPawn>(GetOwningPlayerController()->GetPawn());

	if(MyPawn && MyPawn->GetCurrentReticle())
	{
		CrosshairTex = MyPawn->GetCurrentReticle()->GetResource();
	}
}
