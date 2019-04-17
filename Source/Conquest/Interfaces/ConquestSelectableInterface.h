#pragma once

#include "ConquestSelectableInterface.generated.h"

UINTERFACE()
class CONQUEST_API UConquestSelectableInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class CONQUEST_API IConquestSelectableInterface
{
	GENERATED_IINTERFACE_BODY()

	/** tries to select actor */
	UFUNCTION(BlueprintNativeEvent)
	bool OnSelectionGained(AConquestPlayerController* initiator);

	/** tries to deselect actor */
	UFUNCTION(BlueprintNativeEvent)
	bool OnSelectionChanged(AConquestPlayerController* initiator, AActor* NewSelection);
};
