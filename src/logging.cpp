#include "logging.h"

#include <atomic>
#include <vector>
#include <fstream>
#include <memory>
#include <thread>
#include <inttypes.h>
#include <assert.h>
#include <array>

#include <Windows.h>

struct Logging::Logging_data
{
	Logging::Logging_data()
	{
	}
	std::unique_ptr<std::thread>	logging_thread;
	std::ofstream					log_file;

	enum : unsigned int {
		log_entry_max_size	= 256,
		num_log_entries		= 256
	};
	std::array<char, log_entry_max_size * num_log_entries>	log_entries;
	std::array<uint64_t, log_entry_max_size>				log_entries_time;
	std::atomic_bool										log_entries_ready[log_entry_max_size];
	int					entry_read = 0;
	unsigned			entry_write = 0;

	bool				logging_running = false;
};

void Logging::thread_func(Logging_data* data)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
	data->logging_running = true;
	do {
		Sleep(200);
		const unsigned current_entry_write = data->entry_write;
		while (data->entry_read != current_entry_write)
		{
			const unsigned in_range_entry_read = data->entry_read % Logging_data::num_log_entries;
			if (!data->log_entries_ready[in_range_entry_read])
				break;
			const char* p = &data->log_entries[in_range_entry_read*Logging_data::log_entry_max_size];
			//data->log_entries_time[in_range_entry_read];
//			sprintf_s(buffer_ptr, _data->log_entry_max_size, "[%016" PRIu64 "] > %s\n", __rdtsc(), s);

			OutputDebugStringA(p);	// Option for this?
			data->log_file << p;	// Option for this as well?

			data->log_entries_ready[in_range_entry_read] = false;
			++data->entry_read;
		}
	} while (data->logging_running);
}


Logging::Logging() : _data(std::make_unique<Logging::Logging_data>())
{
	assert(_data->logging_running == false);
	if (0 != CreateDirectoryA("log", nullptr))
	{
		fprintf(stderr, "[Logging] Error creating `log` directory.\n");
		return;
	}

	char filename[128];
	{
		time_t now = time(nullptr);
		struct tm local_time;
		localtime_s(&local_time, &now);
		sprintf_s(filename, "log/%04d-%02d-%02d_%02d-%02d.log", 1900 + local_time.tm_year, 1 + local_time.tm_mon, local_time.tm_mday, local_time.tm_hour, local_time.tm_min);
	}

	_data->log_file.open(filename, std::ios::ate);
	for (int i = 0; i < Logging_data::num_log_entries; ++i)
		_data->log_entries_ready[i] = false;
	_data->logging_thread = std::make_unique<std::thread>(&Logging::thread_func, _data.get() );
}

Logging::~Logging()
{
	if (_data->logging_running == false)
		return;
	_data->logging_running = false;
	_data->logging_thread->join();
	_data->logging_thread.release();
	_data->log_file.close();
}

void Logging::log(const char * s)
{
	if (s == nullptr)
		return;
	if ( (static_cast<int>(_data->entry_write) - _data->entry_read) >= (Logging_data::num_log_entries - 1) )
		return; // overflow warning?

	const unsigned current_buffer_write = (InterlockedIncrement(&_data->entry_write) - 1) % Logging_data::num_log_entries;

	auto buffer_ptr = &_data->log_entries[current_buffer_write*Logging_data::log_entry_max_size];

	_data->log_entries_time[current_buffer_write] = __rdtsc();
//	sprintf_s(buffer_ptr, Logging_data::log_entry_max_size, "%s\n", s);
	sprintf_s(buffer_ptr, Logging_data::log_entry_max_size, "[%016" PRIu64 "] > %s\n", _data->log_entries_time[current_buffer_write], s);

	_data->log_entries_ready[current_buffer_write] = true;
}
