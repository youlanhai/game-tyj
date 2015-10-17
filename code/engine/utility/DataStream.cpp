#include "stdafx.h"
#include "DataStream.h"

namespace Lazy
{
	const size_t BasicSize = sizeof(void*) ;
	const size_t LimitSize = 1024 * 1024 * 1024;

	DataStream::DataStream()
		: pos_(0)
		, stream_(BasicSize)
	{
	}


	DataStream::~DataStream()
	{
	}

	dschar DataStream::get()
	{
		if (empty()) return 0;
		return stream_[pos_++];
	}

	void DataStream::append(dschar ch)
	{
		if (empty()) stream_.resize(pos_ + BasicSize);
		stream_[pos_++] = ch;
	}

	void DataStream::gets(void *dest, size_t len)
	{
		if (len > LimitSize)
			throw(std::invalid_argument("the size is too large!"));

		if (stream_.size() - pos_ < len)
			throw(std::runtime_error("does't has enough stream!"));

		memcpy(dest, &stream_[pos_], len);
		pos_ += len;
	}

	void DataStream::appends(const void *src, size_t len)
	{
		if (len > LimitSize)
			throw(std::invalid_argument("the size is too large!"));

		if (stream_.size() - pos_ < len)
			stream_.resize(pos_ + len + BasicSize);
		memcpy(&stream_[pos_], src, len);
		pos_ += len;
	}

	void DataStream::skip(size_t len)
	{
		pos_ += len;

		if (pos_ >= stream_.size())
		{
			pos_ = stream_.size();
		}
	}
}