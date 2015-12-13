#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#pragma once

#include <boost\thread.hpp>
#include <vector>

#include "Task.h"

class Worker;

class ThreadPool
{
	// ��� ������, ��� ����� ���� �������� �����/������
	boost::mutex *io_mutex;
	// ���� ��� ������� �������
	boost::mutex task_mutex;
	// ���� ��� ������� worker'��
	boost::mutex worker_mutex;
	// �������� ���������� ��� ����������� � ��������� ����� �������
	boost::condition_variable condition_v;

	// ������, ������� ��������� � ����
	std::vector<Task *> queue_tasks;
	// ����� worker'�� ��������� �����
	std::vector<Worker *> queue_workers;
	
	// ��������� ���������� �� ������
	// �����, ��� �� ��������� ����� �������� � �� ��������� ������ �� ������ unsigned
	uint32_t active_threads_count;
	uint32_t hot_threads_count;
	uint32_t inactive_threads_count;
	uint32_t all_threads_count;

	// ��������� ���������� ��������������� ��� �������
	static uint32_t count_uid_threads;
	// �������� ����� ��������� ������ �� ����
	uint32_t delay_to_remove_thread;
	// ���������� ������������ ��������������?
	bool is_stoped;

public:
	// ����������� ��� ����������
	explicit ThreadPool(boost::mutex *_io_mutex, uint32_t _hot_threads_count = 0, uint32_t _delay_to_remove_thread = 0);

	// �������� ����� � ���
	void add_thread_in_pool();
	// ������� �������� �� ����, �� ������� ����� ��� � ������
	void remove_thread_from_pool(Worker *worker);
	// ������� �������� �� ����
	void remove_thread_from_pool(std::vector<Worker *>::iterator it);
	// ��������� ���������� �������� �������
	void inc_active_thread();
	// ��������� ���������� �������� �������
	void dec_active_thread();
	// ���������� �� ������ ���������� � ������ ����
	void display_statistic();

	// ������ ���� ��������������?
	bool thread_pool_is_stoped() const;
	// ������������� ������ ����
	void stop_thread_pool();

	// �������� ������� � ���. ���������� ������� ����� � �������
	void add_task_in_queue(const uint32_t time_to_perform);
	Task* get_task_from_queue();
	// ���������� ������ � ��������� ���������������
	void stop_task_with_uid(const uint32_t uid);

	virtual ~ThreadPool();
};

#endif