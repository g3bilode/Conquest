// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestAttributeSet.h"
#include "Net/UnrealNetwork.h"


UConquestAttributeSet::UConquestAttributeSet()
{

}


void UConquestAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(UConquestAttributeSet, Health, COND_None, REPNOTIFY_Always);
}


void UConquestAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UConquestAttributeSet, Health, OldHealth);
}
