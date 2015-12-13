#ifndef _WORKER_H_
#define _WORKER_H_

#pragma once

#include "Task.h"

class ThreadPool;

class Worker
{
	// �������� �������, �������� ����������� ���� �������
	ThreadPool *thread_pool;
	// ��������� �� �������, ������� ������ �������������� � ������ ������
	Task *worker_task;

	boost::mutex *io_mutex;
	boost::mutex task_mutex;
	// �������� ���������� ��� ����������� worker'� � ��������� ����� �������
	boost::condition_variable *condition_v;
	
	// ������ ��� ��������, �������� �� ������ worker
	volatile bool is_stoped;
	// ��������� �� ������� �����, � ������� �������������� ���� �������
	boost::thread *worker_thread;
	// ������� ����� ��������?
	bool worker_is_free;
	// ����� �� ���������������� �����?
	bool worker_is_hot;

	// ���������� ������������� ������
	uint32_t thread_uid;
	// �������� ����� ������������� ������ �� ������ (��������� � �������� �������)
	uint32_t thread_timeout;

public:
	explicit Worker(ThreadPool *_thread_pool = nullptr, 
				    boost::mutex *_io_mutex	= nullptr,
					boost::condition_variable *_condition_v = nullptr,
					uint32_t _thread_timeout = 0,
					bool _worker_is_hot = false,
					uint32_t _thread_uid = 0
				   );

	// ������ ���� ��������������� ����������! (:
	void operator()();

	// ������� ��������� �� ������������� �������
	Task* get_task() const;
	// �������� ��������� ���������� ������
	int32_t get_task_result() const;
	// �������� ���������� ������������� �������
	int32_t get_task_uid() const;
	// ���������� worker'y �������
	void set_worker_task(Task *_worker_task);				// ������� ��� private
	// ���������� ������������� ������
	void stop_worker_task();

	// �������� ���������� ������������� �������������� ������
	uint32_t get_thread_uid() const;
	// ������� ��������� �� ����� ����������� ������� 
	boost::thread *get_worker_thread() const;
	// ������ worker'� ������� �����
	void set_working_thread(boost::thread *_worker_thread);
	// ������������� ���������� ������
	void stop_working_thread();
	// worker ����������?
	bool worker_is_stoped() const;
	// worker � ��������� ������ � �������� ��� ������� ������
	bool worker_ready_for_task() const;

	virtual ~Worker();
};

#endif