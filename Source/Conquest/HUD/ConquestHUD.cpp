// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestHUD.h"
#include "Blueprint/UserWidget.h"

AConquestHUD::AConquestHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> conquestHud(TEXT("/Game/Conquest/UI/HUD/Gameplay_HUD"));
	GameHUDWidgetClass = conquestHud.Class;
}

void AConquestHUD::BeginPlay()
{
	Super::BeginPlay();

	// Setup Game HUD
	GameHUDWidget = CreateWidget<UUserWidget>(GetWorld(), GameHUDWidgetClass, "Game HUD");
	GameHUDWidget->AddToViewport();

}
