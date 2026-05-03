// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreeLeaf_Header.h"
#include "SkillTree/FP_SkillTreeNodeData.h"
#include "Components/TextBlock.h"

void UFP_SkillTreeLeaf_Header::SetNodeData(const UFP_SkillTreeNodeData* Data)
{
	if (!Data) return;

	Text_NodeName->SetText(Data->NodeName);

	if (Text_NodeType)
	{
		FText NodeTypeText;
		switch (Data->NodeType)
		{
			case ESkillTreeNodeType::Minor:   NodeTypeText = FText::FromString(TEXT("Minor"));   break;
			case ESkillTreeNodeType::Major:   NodeTypeText = FText::FromString(TEXT("Major"));   break;
			case ESkillTreeNodeType::Mastery: NodeTypeText = FText::FromString(TEXT("Mastery")); break;
			default:                          NodeTypeText = FText::FromString(TEXT("Minor"));   break;
		}
		Text_NodeType->SetText(NodeTypeText);
	}

	Expand();
}
