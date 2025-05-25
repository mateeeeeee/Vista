#pragma once

namespace vista
{
	struct BufferField
	{
		enum class DataType { Float, Int, Uint, Bool, FloatMatrix, IntMatrix, UintMatrix, BoolMatrix, Invalid };
		DataType type = DataType::Invalid;
		Int baseComponents = 1;
		Int rows = 1;
		Int columns = 1;
		std::string name;
		Uint64 stride = 0;

		Uint64 GetSize() const
		{
			return stride;
		}
	};

	struct BufferFormat 
	{
		std::vector<BufferField> fields;
		Uint64 totalStride = 0; 
		Bool isValid = false;

		static BufferFormat Parse(std::string const& format);
	};

	std::string BufferToHex(void const* data, Uint64 byteSize);
}