#include "BufferFormatParser.h"

namespace vista
{
	static std::string Trim(std::string const& str)
	{
		size_t first = str.find_first_not_of(" \t\n\r");
		if (first == std::string::npos) return "";
		size_t last = str.find_last_not_of(" \t\n\r");
		return str.substr(first, last - first + 1);
	}

	BufferFormat BufferFormat::Parse(std::string const& format)
	{
		BufferFormat result;
		if (format.empty())
		{
			return result;
		}

		std::stringstream ss(format);
		std::string fieldDecl;
		while (std::getline(ss, fieldDecl, ';'))
		{
			fieldDecl = Trim(fieldDecl);
			if (fieldDecl.empty())
			{
				continue;
			}

			BufferField field;
			std::stringstream fieldStream(fieldDecl);
			std::string typeStr, name;
			fieldStream >> typeStr >> name;

			if (name.empty() || typeStr.empty())
			{
				continue;
			}

			std::string typeLower = typeStr;
			std::transform(typeLower.begin(), typeLower.end(), typeLower.begin(), ::tolower);

			Int baseComponents = 1;
			Int rows = 1;
			Int columns = 1;

			Uint64 xPos = typeLower.find('x');
			if (xPos != std::string::npos)
			{
				std::string baseType = typeLower.substr(0, xPos);
				if (baseType == "float" || baseType == "float2" || baseType == "float3" || baseType == "float4" ||
					baseType == "int" || baseType == "int2" || baseType == "int3" || baseType == "int4" ||
					baseType == "uint" || baseType == "uint2" || baseType == "uint3" || baseType == "uint4" ||
					baseType == "bool" || baseType == "bool2" || baseType == "bool3" || baseType == "bool4")
				{
					if (baseType.back() >= '1' && baseType.back() <= '4')
					{
						baseComponents = baseType.back() - '0';
						baseType.pop_back();
					}

					if (xPos + 1 < typeLower.size() && typeLower[xPos - 1] >= '1' && typeLower[xPos - 1] <= '4' &&
						typeLower[xPos + 1] >= '1' && typeLower[xPos + 1] <= '4')
					{
						rows = typeLower[xPos - 1] - '0';
						columns = typeLower[xPos + 1] - '0';

						if (baseType == "float")
						{
							field.type = BufferField::DataType::FloatMatrix;
							field.stride = sizeof(Float) * baseComponents * rows * columns;
						}
						else if (baseType == "int")
						{
							field.type = BufferField::DataType::IntMatrix;
							field.stride = sizeof(Int) * baseComponents * rows * columns;
						}
						else if (baseType == "uint")
						{
							field.type = BufferField::DataType::UintMatrix;
							field.stride = sizeof(Uint32) * baseComponents * rows * columns;
						}
						else if (baseType == "bool")
						{
							field.type = BufferField::DataType::BoolMatrix;
							field.stride = sizeof(Uint32) * baseComponents * rows * columns;
						}
					}
				}
			}
			else
			{
				if (!typeLower.empty() && typeLower.back() >= '1' && typeLower.back() <= '4')
				{
					rows = typeLower.back() - '0';
					typeLower.pop_back();
				}

				if (typeLower == "float")
				{
					field.type = BufferField::DataType::Float;
					field.stride = sizeof(Float) * rows;
				}
				else if (typeLower == "int")
				{
					field.type = BufferField::DataType::Int;
					field.stride = sizeof(Int) * rows;
				}
				else if (typeLower == "uint")
				{
					field.type = BufferField::DataType::Uint;
					field.stride = sizeof(Uint32) * rows;
				}
				else if (typeLower == "bool")
				{
					field.type = BufferField::DataType::Bool;
					field.stride = sizeof(Uint32) * rows;
				}
			}

			field.baseComponents = baseComponents;
			field.rows = rows;
			field.columns = columns;
			field.name = name;
			if (field.type != BufferField::DataType::Invalid)
			{
				result.fields.push_back(field);
				result.totalStride += field.stride;
			}
		}

		result.isValid = !result.fields.empty();
		return result;
	}

	std::string BufferToHex(void const* data, Uint64 byteSize)
	{
		std::stringstream ss;
		Uint8 const* bytes = static_cast<Uint8 const*>(data);
		ss << std::hex << std::setfill('0');
		for (Uint64 i = 0; i < byteSize; ++i)
		{
			ss << std::setw(2) << static_cast<Int>(bytes[i]);
			if (i < byteSize - 1) ss << " ";
			if ((i + 1) % 16 == 0) ss << "\n";
		}
		return ss.str();
	}
}