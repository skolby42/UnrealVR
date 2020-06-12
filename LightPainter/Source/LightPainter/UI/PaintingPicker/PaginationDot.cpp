// Copyright Scott Kolby 2020


#include "PaginationDot.h"
#include "Components/Image.h"

void UPaginationDot::SetActive(bool bActive)
{
	if (!DotImage) return;
	DotImage->SetOpacity(bActive ? 1.f : InactiveOpacity);
}