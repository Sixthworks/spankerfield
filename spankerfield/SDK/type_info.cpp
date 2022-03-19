#include "type_info.h"
#include "sdk.h"

TypeInfo::TypeInfoData* TypeInfo::GetTypeInfoData()
{
	return ((TypeInfoData*)m_InfoData);
}

TypeInfo* TypeInfo::GetFirst()
{
	return *(TypeInfo**)OFFSET_FIRSTTYPEINFO;
}

ClassInfo* FindClassInfo(const char* ClassName)
{
	TypeInfo* CurrentTypeInfo = TypeInfo::GetFirst();
	if (!CurrentTypeInfo)
		return nullptr;
	do
	{
		if (!IsValidPtr(CurrentTypeInfo->m_InfoData) || !IsValidPtr(CurrentTypeInfo->m_InfoData->m_Name))
			continue;

		if (_stricmp(CurrentTypeInfo->m_InfoData->m_Name, ClassName) != 0)
			continue;

		if (CurrentTypeInfo->GetTypeCode() != BasicTypesEnum::kTypeCode_Class)
			continue;

		return (ClassInfo*)CurrentTypeInfo;
	} while ((CurrentTypeInfo = CurrentTypeInfo->m_Next) != nullptr);
	return nullptr;
}

//Used that only once but maybe you will find this useful.
void DumpClassPointers()
{
	static bool doOnce = true;
	if (!doOnce) return;

	std::stringstream ss;
	std::vector<ClassDumpInfos_s> classes;

	auto* CurrentTypeInfo = TypeInfo::GetFirst();
	if (CurrentTypeInfo)
	{
		std::ofstream file(xorstr_("Classes.txt"));
		do
		{
			if (CurrentTypeInfo->GetTypeCode() != BasicTypesEnum::kTypeCode_Class)
				continue;

			auto* typeInfo = (ClassInfo*)CurrentTypeInfo;

			if ((uintptr_t*)typeInfo->m_DefaultInstance == NULL) continue;

			ClassDumpInfos_s Infos;
			Infos.className = typeInfo->m_InfoData->m_Name;
			Infos.classPtr = typeInfo->m_DefaultInstance;

			classes.push_back(Infos);

		} while ((CurrentTypeInfo = CurrentTypeInfo->m_Next) != nullptr);

		auto cmpFun = [](const ClassDumpInfos_s& a, const ClassDumpInfos_s& b) { return a.className < b.className; };

		std::sort(classes.begin(), classes.end(), cmpFun);

		for (const auto& it : classes)
		{
			ss << "\n"
				<< it.className << " = 0x" << it.classPtr;
		}

		file << ss.str() << std::endl;
		file.close();
	}

	doOnce = false;
}

ValueTypeInfo::ValueTypeInfoData* ValueTypeInfo::GetValueInfoData()
{
	return ((ValueTypeInfoData*)m_InfoData);
}

EnumFieldInfo::EnumFieldInfoData* EnumFieldInfo::GetEnumInfoData()
{
	return ((EnumFieldInfoData*)m_InfoData);
}

ArrayTypeInfo::ArrayTypeInfoData* ArrayTypeInfo::GetArrayTypeInfoData()
{
	return ((ArrayTypeInfoData*)m_InfoData);
}


ClassInfo::ClassInfoData* ClassInfo::GetClassInfoData()
{
	return ((ClassInfoData*)m_InfoData);
}

FieldInfo::FieldInfoData* FieldInfo::GetFieldInfoData()
{
	return ((FieldInfoData*)m_InfoData);
}

BasicTypesEnum MemberInfo::GetTypeCode()
{
	MemberInfo::MemberInfoData* memberInfoData = GetMemberInfoData();
	if (memberInfoData)
	{
		return (BasicTypesEnum)((memberInfoData->m_Flags.m_FlagBits & 0x1F0) >> 0x4);
	}
	return kTypeCode_BasicTypeCount;
}

std::string MemberInfo::GetTypeName()
{
	switch (GetTypeCode())
	{
	case kTypeCode_Void: return "Void";
	case kTypeCode_DbObject: return "DbObject";
	case kTypeCode_ValueType: return "ValueType";
	case kTypeCode_Class: return "Class";
	case kTypeCode_Array: return "Array";
	case kTypeCode_FixedArray: return "FixedArray";
	case kTypeCode_String: return "String";
	case kTypeCode_CString: return "CString";
	case kTypeCode_Enum: return "Enum";
	case kTypeCode_FileRef: return "FileRef";
	case kTypeCode_Boolean: return "Boolean";
	case kTypeCode_Int8: return "Int8";
	case kTypeCode_Uint8: return "Uint8";
	case kTypeCode_Int16: return "Int16";
	case kTypeCode_Uint16: return "Uint16";
	case kTypeCode_Int32: return "Int32";
	case kTypeCode_Uint32: return "Uint32";
	case kTypeCode_Int64: return "Int64";
	case kTypeCode_Uint64: return "Uint64";
	case kTypeCode_Float32: return "Float32";
	case kTypeCode_Float64: return "Float64";
	case kTypeCode_Guid: return "Guid";
	case kTypeCode_SHA1: return "SHA1";
	case kTypeCode_ResourceRef: return "ResourceRef";
	default:
		char buffer[32];
		sprintf_s(buffer, "Undefined[%i]", GetTypeCode());
		return buffer;
	}
}

MemberInfo::MemberInfoData* MemberInfo::GetMemberInfoData()
{
	return ((MemberInfoData*)m_InfoData);
}