// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceDripComponent.h"
#include "ResourceDripWidget.h"


UResourceDripComponent::UResourceDripComponent()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> resourceDripWidget(TEXT("/Game/Conquest/UI/HUD/ResourceDrip_W"));
	SetWidgetClass(resourceDripWidget.Class);
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(true);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void UResourceDripComponent::ActivateDripWidget(int32 amount)
{
	UResourceDripWidget* resourceDripWidget = Cast<UResourceDripWidget>(Widget);
	if (IsValid(resourceDripWidget))
	{
		resourceDripWidget->DrawDrip(amount);
	}
}


void UResourceDripComponent::SetupWidget(FLinearColor TextColour)
{
	UResourceDripWidget* resourceDripWidget = Cast<UResourceDripWidget>(Widget);
	if (IsValid(resourceDripWidget))
	{
		resourceDripWidget->Setup(TextColour);
	}
}
